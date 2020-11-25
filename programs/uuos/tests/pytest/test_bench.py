import os
import sys
import time
import json
import pytest
import logging
import subprocess
import shutil
import gc

from chaintest import ChainTest
import uuos
from uuos import log
from uuos import wasmcompiler
from pyeoskit import eosapi

test_dir = os.path.dirname(__file__)

logger = log.get_logger(__name__)

# print(os.getpid())
# input('<<<')

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.chain = ChainTest(uuos_network=True, jit=True)
        a = {
            "account": 'alice',
            "permission": "active",
            "parent": "owner",
            "auth": {
                "threshold": 1,
                "keys": [
                    {
                        "key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
                        "weight": 1
                    },
                ],
                "accounts": [{"permission":{"actor":'alice',"permission":"uuos.code"},"weight":1}],
                "waits": []
            }
        }

        cls.chain.push_action(cls.chain.system_contract, 'updateauth', a, actor='alice')

        cls.chain.buy_ram_bytes('alice', 'alice', 256*1024)
        cls.chain.buy_ram_bytes('alice', 'bob', 256*1024)

        cls.chain.delegatebw('alice', 'alice', 1.0, 1.0)
        cls.chain.delegatebw('alice', 'bob', 1.0, 1.0)
        cls.chain.produce_block()

    @classmethod
    def teardown_class(cls):
        cls.chain.free()
        shutil.rmtree(cls.chain.options.config_dir)
        shutil.rmtree(cls.chain.options.data_dir)

    def setup_method(self, method):
        logger.warning('test start: %s', method.__name__)

    def teardown_method(self, method):
        try:
            self.chain.deploy_contract('alice', b'', b'', vmtype=1)
        except Exception as e:
            assert e.args[0]['except']['name'] == 'set_exact_code'
            assert e.args[0]['except']['message'] == 'Contract is already running this version of code'
        self.chain.produce_block()
        logger.warning('test end: %s', method.__name__)
        gc.collect()

    def compile_cpp_file(self, name):
        code_file = os.path.join(test_dir, 'test_contracts', f'{name}.cpp')
        wasm_file = os.path.join(test_dir, 'test_contracts', f'{name}.wasm')
        need_compile = True
        try:
            t1 = os.path.getmtime(code_file)
            t2 = os.path.getmtime(wasm_file)
            if t1 < t2:
                need_compile = False
        except:
            pass
        code = None
        if need_compile:
            code = wasmcompiler.compile_cpp_file(code_file)
        else:
            with open(wasm_file, 'rb') as f:
                code = f.read()
        return code

    def compile(self, code):
#        code = eosapi.compile_py_src(code)
        code = uuos.compile(code)
        assert code
        mpy_code = ((code, len(code)),)

        code_region = b''
        code_size_region = b''
        for code, size in mpy_code:
            code_region += code
            code_size_region += int.to_bytes(size, 4, 'little')

        name_region = b'main.mpy\x00'

        region_sizes = b''
        region_sizes += int.to_bytes(len(name_region), 4, 'little')
        region_sizes += int.to_bytes(len(code_size_region), 4, 'little')
        region_sizes += int.to_bytes(len(code_region), 4, 'little')

        header = int.to_bytes(5, 4, 'little')
        header += bytearray(60)
        frozen_code = header + region_sizes + name_region + code_size_region + code_region
        return frozen_code

    def compile_all(self, code_info):
#        code = eosapi.compile_py_src(code)
        mpy_code = []
        if not os.path.exists('tmp'):
            os.mkdir('tmp')
        for name, code in code_info:
            py_file = f'tmp/{name}.py'
            mpy_file = f'tmp/{name}.mpy'

            with open(py_file, 'w') as f:
                f.write(code)
            subprocess.check_output(['mpy-cross', '-o', mpy_file, py_file])
            with open(mpy_file, 'rb') as f:
                code = f.read()
            os.remove(py_file)
            os.remove(mpy_file)

            mpy_code.append((name, code, len(code)))
        shutil.rmtree('tmp')

        name_region = b''
        code_region = b''
        code_size_region = b''
        for name, code, size in mpy_code:
            code_region += code
            code_size_region += int.to_bytes(size, 4, 'little')
            name_region += b'%s.mpy\x00'%(name.encode(),)

        region_sizes = b''
        region_sizes += int.to_bytes(len(name_region), 4, 'little')
        region_sizes += int.to_bytes(len(code_size_region), 4, 'little')
        region_sizes += int.to_bytes(len(code_region), 4, 'little')

        header = int.to_bytes(5, 4, 'little')
        header += bytearray(60)
        frozen_code = header + region_sizes + name_region + code_size_region + code_region
        return frozen_code

    def run_test(self, code):
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=1, show_elapse=False)
        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        self.chain.produce_block()
        return r['elapsed']/1e6

    def test_bench(self):
        code = '''
def apply(a, b, c):
    def f():
        return 1
    for i in range(5):
        f()
'''
        elapsed = self.run_test(code)
        logger.info("assignment.py: %s", elapsed)

        code = '''
def apply(a, b, c):
    a = 0
    for i in range(1000000):
        a += 1
'''
        elapsed = self.run_test(code)
        logger.info("augm_assign.py: %s", elapsed)

        code = '''
def apply(a, b, c):
    t = []
    i = 0
    while i < 100000:
        t.append(i)
        i += 1
'''

        elapsed = self.run_test(code)
        logger.info("augmented assignment and list append: %s", elapsed)

        code = '''
def apply(a, b, c):
    for i in range(1000000):
        a = 1.0
'''
        elapsed = self.run_test(code)
        logger.info("assignment_float.py: %s", elapsed)

        code = '''
def apply(a, b, c):
    n = 60
    for i in range(10000):
        2 ** n
'''
        elapsed = self.run_test(code)
        logger.info("big integers: %s", elapsed)


        code = '''
def apply(a, b, c):
    for i in range(100000):
        a = {0:0}
'''
        elapsed = self.run_test(code)
        logger.info("build_dict.py: %s", elapsed*10)


        code = '''
def apply(a, b, c):
    d = {}
    for i in range(100000):
        d[i] = i
'''
        elapsed = self.run_test(code)
        logger.info("build_dict2.py: %s", elapsed)

        code = '''
def apply(a, b, c):
    a = {0:0}
    for i in range(1000000):
        a[0] = i
    assert a[0]==999999
'''
        elapsed = self.run_test(code)
        logger.info("set_dict_item.py: %s", elapsed)

        code = '''
def apply(a, b, c):
    for i in range(10000):
        a = {0, 2.7, "x"}
'''
        elapsed = self.run_test(code)
        logger.info("build_set.py: %s", elapsed*100)

        code = '''
def apply(a, b, c):
    for i in range(100000):
        a = [1, 2, 3]
'''
        elapsed = self.run_test(code)
        logger.info("build_list.py: %s", elapsed*10)

        code = '''
def apply(a, b, c):
    a = [0]
    for i in range(1000000):
        a[0] = i
'''
        elapsed = self.run_test(code)
        logger.info("set_list_item.py: %s", elapsed)

        code = '''
def apply(a, b, c):
    a = [1, 2, 3]
    for i in range(100000):
        a[:]
'''
        elapsed = self.run_test(code)
        logger.info("list slice.py: %s", elapsed)

        code = '''
def apply(a, b, c):
    a, b, c = 1, 2, 3
    for i in range(1000000):
        a + b + c
'''
        elapsed = self.run_test(code)
        logger.info("add_integers.py: %s", elapsed)

        code = '''
def apply(a, b, c):
    a, b, c = 'a', 'b', 'c'
    for i in range(10000):
        a + b + c
'''
        elapsed = self.run_test(code)
        logger.info("add_strings.py: %s", elapsed*100)

        code = '''
def apply(a, b, c):
    for _i in range(100000):
        str(_i)
'''
        elapsed = self.run_test(code)
        logger.info("str_of_int.py: %s", elapsed)

        code = '''
def apply(a, b, c):
    for i in range(100000):
        def f():
            pass
'''
        elapsed = self.run_test(code)
        logger.info("create_function.py: %s", elapsed*10)


        code = '''
def apply(a, b, c):
    for i in range(10000):
        def f(x):
            pass
'''
        elapsed = self.run_test(code)
        logger.info("create function, single positional argument.py: %s", elapsed*100)

        code = '''
def apply(a, b, c):
    for i in range(10000):
        def f(x, y=1, *args, **kw):
            pass
'''
        elapsed = self.run_test(code)
        logger.info("create function, complex arguments.py: %s", elapsed*100)

        code = '''
def apply(a, b, c):
    def f():
        return 1
    for i in range(100000):
        f()
'''
        elapsed = self.run_test(code)
        logger.info("function_call.py: %s", elapsed*10)


        code = '''
def apply(a, b, c):
    def f(x, y=0, *args, **kw):
        return x
    for i in range(100000):
        f(i, 5, 6, a=8)
'''
        elapsed = self.run_test(code)
        logger.info("function call, complex arguments.py: %s", elapsed)

        code = '''
def apply(a, b, c):
    for i in range(10000):
        class A:
            pass
'''
        elapsed = self.run_test(code)
        logger.info("create simple class.py: %s", elapsed)

        code = '''
def apply(a, b, c):
    for i in range(10000):
        class A:
            def __init__(self, x):
                self.x = x
'''
        elapsed = self.run_test(code)
        logger.info("create class with int.py: %s", elapsed)


        code = '''
def apply(a, b, c):
    class A:
        pass
    for i in range(1000):
        A()
'''
        elapsed = self.run_test(code)
        logger.info("create instance of simple class.py: %s", elapsed*1000)


        code = '''
def apply(a, b, c):
    class A:
        def __init__(self, x):
            self.x = x
    for i in range(10000):
        A(i)
'''
        elapsed = self.run_test(code)
        logger.info("create instance of class with init.py: %s", elapsed*100)

        code = '''
def apply(a, b, c):
    class A:
        def __init__(self, x):
            self.x = x

        def f(self):
            return self.x
    a = A(1)
    for i in range(100000):
        a.f()
'''
        elapsed = self.run_test(code)
        logger.info("call instance method.py: %s", elapsed)


