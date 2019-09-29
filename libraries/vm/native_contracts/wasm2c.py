import os
import sys
import subprocess
import hashlib

#for file in os.listdir('contracts'):

def wasm2c(contract_name, wasm_file):
    if not wasm_file.endswith('.wasm'):
        continue
    hash = hashlib.sha256()
    with open(wasm_file, 'rb') as f:
        hash.update(f.data())
    hash_hex = hash.hexdigest()

    file_injected = wasm_file.replace('.wasm', '.wasm2')
    src_file = contract_name + '.c'

    if os.path.exists(src_file):
        continue

    wasm_injector = [
        "../libraries/vm/vm_wasm/wasm_injector",
        wasm_file,
        file_injected
    ]

    wasm2c = [
        "/Users/newworld/dev/wasm/wabt/build/wasm2c",
        '-o',
        src_file,
        file_injected
    ]

    try:
        print(' '.join(wasm_injector))
        ret = subprocess.check_output(wasm_injector, stderr=subprocess.STDOUT)
        print(ret.decode('utf8'))

        print(' '.join(wasm2c))
        ret = subprocess.check_output(wasm2c, stderr=subprocess.STDOUT)
        print(ret.decode('utf8'))
    except subprocess.CalledProcessError as e:
        file_injected
        print("error (code {}):".format(e.returncode))
        print(e.output.decode('utf8'))

    with open(src_file, 'r') as f:
        src = f.read()
        src = src.replace('Z_applyZ_vjjj', 'apply_' + hash_hex)
        init = f'void WASM_RT_ADD_PREFIX(init_{hash_hex})(void)'
        src = src.replace('void WASM_RT_ADD_PREFIX(init)(void)', init)
    if src.find('static wasm_rt_memory_t memory') >= 0:
        src += '''
wasm_rt_memory_t* get_memory_%s() {
    return &memory;
}
        '''%(hash_hex,)
    elif src.find('static wasm_rt_memory_t M0') >= 0:
        src += '''
wasm_rt_memory_t* get_memory_%s() {
    return &M0;
}
        '''%(hash_hex,)
    else:
        raise Exception('memory declaration not found!')

    with open(src_file, 'w') as f:
        f.write(src)

contract_name = sys.argv[1]
wasm_file = sys.argv[2]
wasm2c(contract_name, wasm_file)
