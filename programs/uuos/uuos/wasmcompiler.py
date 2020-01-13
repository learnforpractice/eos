import os
import sys
import shutil
import hashlib
import marshal
import subprocess
import tempfile

def find_eosio_cdt_path():
    eosio_cpp = shutil.which('eosio-cpp')
    if not eosio_cpp:
        raise "eosio.cdt not installed, please refer to https://github.com/eosio/eosio.cdt for an installation guide"
    eosio_cpp = os.path.realpath(eosio_cpp)
    eosio_cpp = os.path.dirname(eosio_cpp)
    return os.path.dirname(eosio_cpp)

class cpp_compiler(object):

    def __init__(self, cpp_file, includes = [], entry='apply'):
        self.cpp_file = cpp_file
        self.includes = includes
        self.entry = entry
        if not cpp_file.endswith('.cpp'):
            raise 'Not a cpp file'

    def compile_cpp_file(self, opt='O3'):
        tmp_path = self.cpp_file[:-4]
        #%system rm test.obj test.wasm
        #%system eosio-cpp -I/usr/local/Cellar/eosio.cdt/1.6.1/opt/eosio.cdt/include/eosiolib/capi -I/usr/local/Cellar/eosio.cdt/1.6.1/opt/eosio.cdt/include/eosiolib/core -O3 -contract test -o test.obj -c test.cpp
        #%system eosio-ld test.obj -o test.wasm
        #%ls
        if sys.platform == 'darwin':
            dl_sufix = 'dylib'
        else:
            dl_sufix = 'so'
        eosio_cdt_path = find_eosio_cdt_path()
        clang_7_args = [f'{eosio_cdt_path}/bin/clang-7',
        '-o',
        f'{tmp_path}.obj',
        f'{tmp_path}.cpp',
        '--target=wasm32',
        '-ffreestanding',
        '-nostdlib',
        '-fno-builtin',
        '-fno-threadsafe-statics',
        '-fno-exceptions',
        '-fno-rtti',
        '-fmodules-ts',
        '-DBOOST_DISABLE_ASSERTS',
        '-DBOOST_EXCEPTION_DISABLE',
        '-Xclang',
        '-load',
        '-Xclang',
        f'{eosio_cdt_path}/bin/LLVMEosioApply.{dl_sufix}',
        f'-fplugin={eosio_cdt_path}/bin/eosio_plugin.{dl_sufix}',
        '-mllvm',
        '-use-cfl-aa-in-codegen=both',
        f'-I{eosio_cdt_path}/bin/../include/libcxx',
        f'-I{eosio_cdt_path}/bin/../include/libc',
        f'-I{eosio_cdt_path}/bin/../include',
        f'--sysroot={eosio_cdt_path}/bin/../',
        f'-I{eosio_cdt_path}/bin/../include/eosiolib/core',
        f'-I{eosio_cdt_path}/bin/../include/eosiolib/contracts',
        '-c',
        f'-I{eosio_cdt_path}/include/eosiolib/capi',
        f'-I{eosio_cdt_path}/include/eosiolib/core',
        f'-{opt}',
        '--std=c++17',
        ]
        for include in self.includes:
            clang_7_args.append(f"-I{include}")

        wasm_ld_args = [f'{eosio_cdt_path}/bin/wasm-ld',
        '--gc-sections',
        '--strip-all',
        '-zstack-size=8192',
        '--merge-data-segments',
        '-e', f'{self.entry}',
        '--only-export', f'{self.entry}:function',
        '-lc++',
        '-lc',
        '-leosio',
        '-leosio_dsm',
        '-mllvm',
        '-use-cfl-aa-in-codegen=both',
        f'{tmp_path}.obj',
        f'-L{eosio_cdt_path}/bin/../lib',
        '-stack-first',
        '--lto-O3',
        '-o',
        f'{tmp_path}.wasm',
        f'--allow-undefined-file={eosio_cdt_path}/bin/../eosio.imports']

        eosio_pp = [
            f'{eosio_cdt_path}/bin/eosio-pp',
            '-o',
            f'{tmp_path}.wasm',
            f'{tmp_path}.wasm',
        ]

        try:
            ret = subprocess.check_output(clang_7_args, stderr=subprocess.STDOUT)
            print(ret.decode('utf8'))
            ret = subprocess.check_output(wasm_ld_args, stderr=subprocess.STDOUT)
            print(ret.decode('utf8'))
            ret = subprocess.check_output(eosio_pp, stderr=subprocess.STDOUT)
            print(ret.decode('utf8'))
        except subprocess.CalledProcessError as e:
            print("error (code {}):".format(e.returncode))
            print(e.output.decode('utf8'))
            return None

        with open(f'{tmp_path}.wasm', 'rb') as f:
            return f.read()

def compile_cpp_file(src_path, includes=[], entry='apply', opt='O3'):
    compiler = cpp_compiler(src_path, includes, entry)
    return compiler.compile_cpp_file(opt)

def compile_cpp_src(account_name, code, includes = [], entry='apply', opt='O3', force=False):
    temp_dir = tempfile.mktemp()
    src_file = temp_dir + '.cpp'

    with open(src_file, 'w') as f:
        f.write(code)
    wasm_code =  compile_cpp_file(src_file, includes, entry, opt=opt)

    for ext in ('.cpp', '.obj', '.wasm'):
        file_name = temp_dir + ext
        if os.path.exists(file_name):
            os.remove(file_name)

    return wasm_code
