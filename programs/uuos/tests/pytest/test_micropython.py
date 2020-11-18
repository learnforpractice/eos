import os
import time
import json
import pytest
import logging
import subprocess

from chaintest import ChainTest
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

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        logger.warning('test start: %s', method.__name__)

    def teardown_method(self, method):
        try:
            self.chain.deploy_contract('alice', b'', b'', vmtype=3)
        except Exception as e:
            assert e.args[0]['except']['name'] == 'set_exact_code'
            assert e.args[0]['except']['message'] == 'Contract is already running this version of code'
        self.chain.produce_block()
        logger.warning('test end: %s', method.__name__)

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
        with open('tmp.py', 'w') as f:
            f.write(code)
        subprocess.check_output(['mpy-cross', '-o', 'tmp.mpy', 'tmp.py'])
        with open('tmp.mpy', 'rb') as f:
            code = f.read()
        os.remove('tmp.py')
        os.remove('tmp.mpy')

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
        os.rmdir('tmp')
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

    def test_loop(self):
        code = '''
def apply(a, b, c):
    while True:
        pass
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        try:
            r = self.chain.push_action('alice', 'sayhello', b'hello,world')
            assert 0
        except Exception as e:
            exception_name = e.args[0]['except']['name']
            assert exception_name in ('deadline_exception', 'tx_cpu_usage_exceeded')

    def test_call_depth(self):
        code = '''
def apply(a, b, c):
    pass
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        self.chain.produce_block()

        for i in range(300):
            r = self.chain.push_action('alice', 'sayhello', str(i).encode('utf8'))
            if i % 50 == 0:
                self.chain.produce_block()
        self.chain.produce_block()
        logger.info('+++elapsed: %s', r['elapsed'])

    def test_vm_api(self):
        # print(os.getpid())
        # input('<<<')
        contract_name = 'alice'
        args = {
            'account':contract_name,
            'is_priv':1
        }
        self.chain.push_action('uuos', 'setpriv', args)

        code = os.path.join(test_dir, 'test_contracts', 'test_vm_api.py')
        with open(code, 'r') as f:
            code = f.read()
#subprocess.CalledProcessError
        code = self.compile(code)
        self.chain.deploy_contract(contract_name, code, b'', vmtype=3)
        code = '''
def apply(a, b, c):
    print('hello world from bob')
'''
        code = self.compile(code)
        self.chain.deploy_contract('bob', code, b'', vmtype=3)

        r = self.chain.push_action(contract_name, 'sayhello', b'hello,world')
        logger.info('+++elapsed: %s', r['elapsed'])
        self.chain.deploy_contract('bob', b'', b'', vmtype=3)

        # r = self.chain.push_action(contract_name, 'sayhello', b'hello,world again')
        # logger.info(r['elapsed'])

    def test_action(self):
        # print(os.getpid())
        # input('<<<')
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

        r = self.chain.push_action(self.chain.system_contract, 'updateauth', a, actor='alice')

        code = os.path.join(test_dir, 'test_contracts', 'test_action.py')
        with open(code, 'r') as f:
            code = f.read()

        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)

        r = self.chain.push_action('alice', 'sendinline', b'hello,world from inline')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action('alice', 'sendraw', b'hello,world from raw')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action('alice', 'contextfree', b'hello,world from context free inline')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action('alice', 'contextraw', b'hello,world from context free inline')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action('alice', 'testaction', b'hello,world from context free inline')
        logger.info('+++elapsed: %s', r['elapsed'])

    def test_db_api(self):
        code = os.path.join(test_dir, 'test_contracts', 'vm_api_db_test.py')
        with open(code, 'r') as f:
            code = f.read()
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info(r['action_traces'][0]['console'])
        logger.info('+++elapsed: %s', r['elapsed'])

    def test_db_i256(self):
        code = os.path.join(test_dir, 'test_contracts', 'test_db_i256.py')
        with open(code, 'r') as f:
            code = f.read()
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)

        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info('+++elapsed: %s', r['elapsed'])

    def test_exception(self):
        code = '''
def apply(a, b, c):
    raise Exception('oops!')
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        try:
            r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        except Exception as e:
            logger.info(e.args[0]['action_traces'][0]['console'])
            assert e.args[0]['except']['name'] == 'python_execution_error'
            assert e.args[0]['except']['message'] == 'Python execution error'

    def test_init_exception(self):
        code = '''
aa = bb
def apply(a, b, c):
    raise Exception('oops!')
'''
        code = self.compile(code)
        try:
            self.chain.deploy_contract('alice', code, b'', vmtype=3)
            assert 0
        except Exception as e:
            logger.info(e.args[0]['action_traces'][0]['console'])
            assert e.args[0]['except']['name'] == 'python_execution_error'

    def test_setjmp(self):
        return
        code = '''
import chain
def apply(a, b, c):
    for i in range(20490, 128*1024, 10):
        chain.mod_set_memory(i, 'hello,world')
    raise Exception('oops!')
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        try:
            r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        except Exception as e:
            logger.info(e.args[0]['action_traces'][0]['console'])
            logger.info(e.args[0]['except']['name'])
            assert e.args[0]['except']['name'] == 'eosio_assert_message_exception'

    def test_bigint(self):
        code = os.path.join(test_dir, 'test_contracts', 'test_bigint.py')
        with open(code, 'r') as f:
            code = f.read()
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info(r['action_traces'][0]['console'])
        logger.info('+++elapsed: %s', r['elapsed'])

    def test_bigint_performance(self):
        code = r'''
def apply(a, b, c):
    a = 0xffffffffffffffff
    b = 0xffffffffffffffff
    for i in range(500):
        a * b
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)

        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info(r['action_traces'][0]['console'])
        logger.info('+++elapsed: %s', r['elapsed'])
        self.chain.produce_block()

        code = r'''
def apply(a, b, c):
    a = bigint(0xffffffffffffffff)
    b = bigint(0xffffffffffffffff)
    for i in range(600):
        a * b
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)

        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info(r['action_traces'][0]['console'])
        logger.info('+++elapsed: %s', r['elapsed'])

    def test_float128(self):
        code = os.path.join(test_dir, 'test_contracts', 'test_float128.py')
        with open(code, 'r') as f:
            code = f.read()
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info(r['action_traces'][0]['console'])
        logger.info('+++elapsed: %s', r['elapsed'])

    def test_name(self):
        code = r'''
from chain import *
def apply(a, b, c):
    print(type(a) == name, type(b) == name, type(c) == name)
    print('++++name:', a)
    assert a == name('alice')
    assert int(a) == s2n('alice')
    assert int(b) == s2n('alice')
    assert str(a) == 'alice'
    assert name('alice') == name(s2n('alice'))

    print(name('alice') , name(s2n('alice')))

    assert a.to_int() == s2n('alice')
    assert 'alice' == a.to_str()
    assert name('helloworld111') == name(s2n('helloworld111'))
    assert name('helloworld111').to_str() == 'helloworld111'
    assert name('helloworld111').to_int() == s2n('helloworld111')

#    print(name('hello') == s2n('hello'))

'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info(r['action_traces'][0]['console'])
        logger.info('+++elapsed: %s', r['elapsed'])

    def test_trx(self):
        code = r'''
from chain import *
def apply(a, b, c):
    float128(128.0) * float128(889.0)
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        self.chain.produce_block()
        logger.info('+++++++++++++++get_balance("alice") %s', self.chain.get_balance('alice'))
        total_time = 0
        count = 10
        start = time.time()
        for i in range(count):
            r = self.chain.push_action('alice', 'sayhello', str(i).encode('utf8'))
            total_time += r['elapsed']
        print('duration:', time.time() - start)
        logger.info(r['action_traces'][0]['console'])
        logger.info('+++elapsed: %s', r['elapsed'])
        print(total_time/count, 1e6/(total_time/count))

    def test_call_contract(self):
        # print(os.getpid())
        # input('<<<')
        code_file = os.path.join(test_dir, 'test_contracts', 'test_call_contract.cpp')
        wasm_file = os.path.join(test_dir, 'test_contracts', 'test_call_contract.wasm')
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
        contract_name = 'bob'
        self.chain.deploy_contract(contract_name, code, b'')
        self.chain.produce_block()

        code = '''
import struct
from chain import *
def apply(receiver, code, action):
    contract = s2n('calltest1')
    args = struct.pack('QQ', contract, 1)
    ret = call_contract('bob', args)
    print('+++call contract return:', len(ret), ret)
    ret = int.from_bytes(ret, 'little')
    print(ret)
    assert ret == 2
'''
        code = self.compile(code)

        contract_name = 'alice'
        self.chain.deploy_contract(contract_name, code, b'', vmtype=3)

        r = self.chain.push_action(contract_name, 'sayhello', b'a')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action(contract_name, 'sayhello', b'b')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action(contract_name, 'sayhello', b'c')
        logger.info('+++elapsed: %s', r['elapsed'])
        self.chain.deploy_contract('bob', b'', b'')

    def test_multi_index(self):
        code = os.path.join(test_dir, 'test_contracts', 'test_multi_index.py')
        with open(code, 'r') as f:
            code = f.read()
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)

        r = self.chain.push_action('alice', 'test1', b'hello,world')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action('alice', 'test2', b'hello,world')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action('alice', 'test3', b'hello,world')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action('alice', 'test4', b'hello,world')
        logger.info('+++elapsed: %s', r['elapsed'])

# test for out of context
# calling vm_api from module level not allowed!
    def test_context(self):
        code = r'''
import chain
receiver = chain.current_receiver();
def apply(a, b, c):
    a = 0xffffffffffffffff
    b = 0xffffffffffffffff
    for i in range(500):
        a * b
'''
        code = self.compile(code)
        try:
            self.chain.deploy_contract('alice', code, b'', vmtype=3)
            assert 0
        except Exception as e:
            assert e.args[0]['except']['stack'][0]['data']['s'] == 'access apply context not allowed!'
 
    def test_json(self):
        code = r'''
import chain
import json
def apply(a, b, c):
    a = {'a':1, 'b':2, 'c':3.5}
    for i in range(600):
        json.dumps(a)
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)

        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info('+++elapsed: %s', r['elapsed'])
        self.chain.produce_block()
        try:
            r = self.chain.push_action('alice', 'sayhello', b'hello,world')
            logger.info('+++elapsed: %s', r['elapsed'])
        except Exception as e:
            assert e.args[0]['except']['stack'][0]['data']['s'] == 'access apply context not allowed!'

    def test_oob(self):
        code = r'''
def apply(a, b, c):
    for i in range(10000):
        bigint(0xffffffffffffffff) * bigint(0xffffffffffffffff)
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        try:
            r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        except Exception as e:
            assert e.args[0]['except']['stack'][0]['data']['s'] == 'vm error out of bounds'

    def test_memory(self):
        code = r'''
large_str = 'a'*(1024)
def apply(a, b, c):
    print(large_str[1024-1])
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)

        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info('+++elapsed: %s', r['elapsed'])
        self.chain.produce_block()

        code = r'''
large_str = 'a'*(1024*512)
def apply(a, b, c):
    pass
    print(large_str[1024*200])
#    large_str[1024*512-1]
#    print(large_str[1024*512-1])
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)

        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action('alice', 'sayhello', b'hello,world2')
        logger.info('+++elapsed: %s', r['elapsed'])

        self.chain.produce_block()

#allolc a large memory
        code = r'''
large_data = bytearray(5*1024*1024)
def apply(a, b, c):
    print(large_data[5*1024*1024-1])
#    large_str[1024*512-1]
#    print(large_str[1024*512-1])
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)

        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action('alice', 'sayhello', b'hello,world2')
        logger.info('+++elapsed: %s', r['elapsed'])

        self.chain.produce_block()

#out of memory test
        code = r'''
def apply(a, b, c):
    a = 'a'*(1024*1024*10)
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        try:
            r = self.chain.push_action('alice', 'sayhello', b'hello,world')
            assert 0
        except Exception as e:
            logger.info(e.args[0]['except']['stack'][0]['data']['s'])
            assert e.args[0]['except']['name'] == 'eosio_assert_message_exception'
            assert e.args[0]['except']['stack'][0]['data']['s'] == 'failed to allocate pages'
        self.chain.produce_block()

        code = r'''
g_a = 1
def apply(a, b, c):
    global g_a
    assert g_a == 1
    g_a += 1
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        r = self.chain.push_action('alice', 'sayhello', b'hello,world1')
        r = self.chain.push_action('alice', 'sayhello', b'hello,world2')

    def test_frozen(self):
        code = r'''
def apply(a, b, c):
    import foo
    foo.say_hello()
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        try:
            r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        except Exception as e:
            assert e.args[0]['except']['name'] == 'eosio_assert_message_exception'
            assert e.args[0]['except']['stack'][0]['data']['s'] == 'no free vm memory left!'

    def test_mpy_frozen(self):
        code = os.path.join(test_dir, 'test_contracts', 'test_frozen.py')
        with open(code, 'r') as f:
            code = f.read()
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        try:
            r = self.chain.push_action('alice', 'sayhello', b'hello,world')
            logger.info('+++elapsed: %s', r['elapsed'])
            r = self.chain.push_action('alice', 'sayhello', b'hello,world1')
            logger.info('+++elapsed: %s', r['elapsed'])
        except Exception as e:
            name = e.args[0]['except']['name']
            # == 'eosio_assert_message_exception'
            msg = e.args[0]['except']['stack'][0]['data']['s']
            logger.info(name)
            logger.info(msg)
#             == 'no free vm memory left!'

    def test_performance(self):
        code = '''
def apply(a, b, c):
    a = bytearray(5*1024*1024)
'''
        code = self.compile(code)
        r = self.chain.deploy_contract('alice', code, b'', vmtype=3)
        # logger.info('+++elapsed: %s', r['elapsed'])
        r = self.chain.push_action('alice', 'sayhello', b'hello,world1')
        logger.info('+++elapsed: %s', r['elapsed'])


        code = '''
def apply(a, b, c):
    return
'''
        code = self.compile(code)
        r = self.chain.deploy_contract('alice', code, b'', vmtype=3)
        r = self.chain.push_action('alice', 'sayhello', b'hello,world2')
        logger.info('+++elapsed: %s', r['elapsed'])

    def test_token(self):
        code = os.path.join(test_dir, 'test_contracts', 'test_token.py')
        with open(code, 'r') as f:
            code = f.read()
        code = self.compile(code)
        r = self.chain.deploy_contract('alice', code, b'', vmtype=3)

        r = self.chain.push_action('alice', 'test1', b'hello,world2')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action('alice', 'test2', b'hello,world2')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action('alice', 'test2', b'hello,world3')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action('alice', 'test3', b'hello,world3')
        logger.info('+++elapsed: %s', r['elapsed'])

        r = self.chain.push_action('alice', 'test3', b'hello,world4')
        logger.info('+++elapsed: %s', r['elapsed'])

        logger.info(self.chain.get_balance('alice'))
        r = self.chain.transfer('alice', 'bob', 1)
        logger.info('+++elapsed: %s', r['elapsed'])
        logger.info(self.chain.get_balance('alice'))

        logger.info(self.chain.get_balance('alice'))
        r = self.chain.transfer('alice', 'bob', 2)
        logger.info('+++elapsed: %s', r['elapsed'])
        logger.info(self.chain.get_balance('alice'))
    
    def test_deploy_contract(self):
        hello = '''
def say_hello():
    print('hello')
'''
        world = '''
def say_world():
    print('world')
'''
        main = '''
import hello
import world
def apply(a, b, c):
    hello.say_hello()
    world.say_world()
'''
        code = (('hello',hello),('world', world), ('main', main))
        code = self.compile_all(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        r = self.chain.push_action('alice', 'test3', b'hello,world4')
        logger.info('+++elapsed: %s', r['elapsed'])

    def gen_mpy_code(self, code_info):
        mpy_code = []
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
        os.rmdir('tmp')
        return mpy_code

    def gen_bad_name_frozen(self, code_info):
#        code = eosapi.compile_py_src(code)
        mpy_code = self.gen_mpy_code(code_info)
        name_region = b''
        code_region = b''
        code_size_region = b''
        for name, code, size in mpy_code:
            code_region += code
            code_size_region += int.to_bytes(size, 4, 'little')
#            name_region += b'%s.mpy\x00'%(name.encode(),)
            name_region += b'%s.mpy'%(name.encode(),)

        region_sizes = b''
        region_sizes += int.to_bytes(len(name_region), 4, 'little')
        region_sizes += int.to_bytes(len(code_size_region), 4, 'little')
        region_sizes += int.to_bytes(len(code_region), 4, 'little')

        header = int.to_bytes(5, 4, 'little')
        header += bytearray(60)
        frozen_code = header + region_sizes + name_region + code_size_region + code_region
        return frozen_code

    def gen_bad_code_size_frozen(self, code_info):
#        code = eosapi.compile_py_src(code)
        mpy_code = self.gen_mpy_code(code_info)
        name_region = b''
        code_region = b''
        code_size_region = b''
        for name, code, size in mpy_code:
            code_region += code
            code_size_region += int.to_bytes(size+1, 4, 'little')
#            code_size_region += int.to_bytes(size, 4, 'little')
            name_region += b'%s.mpy\x00'%(name.encode(),)

        region_sizes = b''
        region_sizes += int.to_bytes(len(name_region), 4, 'little')
        region_sizes += int.to_bytes(len(code_size_region), 4, 'little')
        region_sizes += int.to_bytes(len(code_region), 4, 'little')

        header = int.to_bytes(5, 4, 'little')
        header += bytearray(60)
        frozen_code = header + region_sizes + name_region + code_size_region + code_region
        return frozen_code

    def gen_large_code_size_frozen(self, code_info):
#        code = eosapi.compile_py_src(code)
        mpy_code = self.gen_mpy_code(code_info)
        name_region = b''
        code_region = b''
        code_size_region = b''
        for name, code, size in mpy_code:
            code_region += code
            code_size_region += int.to_bytes(11*1024*1024, 4, 'little')
#            code_size_region += int.to_bytes(size, 4, 'little')
            name_region += b'%s.mpy\x00'%(name.encode(),)

        region_sizes = b''
        region_sizes += int.to_bytes(len(name_region), 4, 'little')
        region_sizes += int.to_bytes(len(code_size_region), 4, 'little')
        region_sizes += int.to_bytes(len(code_region), 4, 'little')

        header = int.to_bytes(5, 4, 'little')
        header += bytearray(60)
        frozen_code = header + region_sizes + name_region + code_size_region + code_region
        return frozen_code

    def test_bad_frozen(self):
        hello = '''
def say_hello():
    print('hello')
'''
        world = '''
def say_world():
    print('world')
'''
        main = '''
import hello
import world
def apply(a, b, c):
    hello.say_hello()
    world.say_world()
'''
        code = (('hello',hello),('world', world))
        code = self.compile_all(code)
        try:
            self.chain.deploy_contract('alice', code, b'', vmtype=3)
            assert 0
        except Exception as e:
            assert e.args[0]['except']['name'] == 'python_execution_error'
            assert e.args[0]['except']['stack'][0]['format'] == 'main module not found!'

        code = (('hello',hello),('world', world), ('main', main))
        code = self.gen_bad_name_frozen(code)
        try:
            self.chain.deploy_contract('alice', code, b'', vmtype=3)
            assert 0
        except Exception as e:
            assert e.args[0]['except']['name'] == 'eosio_assert_message_exception'
            assert e.args[0]['except']['stack'][0]['data']['s'] == 'invalid module name count!'

        code = (('hello',hello),('world', world), ('main', main))
        code = self.gen_bad_code_size_frozen(code)
        try:
            self.chain.deploy_contract('alice', code, b'', vmtype=3)
            assert 0
        except Exception as e:
            assert e.args[0]['except']['name'] == 'eosio_assert_message_exception'
            assert e.args[0]['except']['stack'][0]['data']['s'] == 'invalid code region size!'

        code = []
        for i in range(100):
            code.append(('hello'+str(i), hello))
        code = self.compile_all(code)
        try:
            self.chain.deploy_contract('alice', code, b'', vmtype=3)
            assert 0
        except Exception as e:
            assert e.args[0]['except']['name'] == 'eosio_assert_message_exception'
            assert e.args[0]['except']['stack'][0]['data']['s'] == 'frozen module count must <= 100'

        code = (('hello',hello),('world', world), ('main', main))
        code = self.gen_large_code_size_frozen(code)
        try:
            self.chain.deploy_contract('alice', code, b'', vmtype=3)
            assert 0
        except Exception as e:
            assert e.args[0]['except']['name'] == 'eosio_assert_message_exception'
            assert e.args[0]['except']['stack'][0]['data']['s'] == 'module_size too large!'

        try:
            self.chain.deploy_contract('alice', bytearray(76), b'', vmtype=3)
            assert 0
        except Exception as e:
            assert e.args[0]['except']['name'] == 'eosio_assert_message_exception'
            assert e.args[0]['except']['stack'][0]['data']['s'] == 'invalid code size!'

        try:
            self.chain.deploy_contract('alice', bytearray(77), b'', vmtype=3)
            assert 0
        except Exception as e:
            assert e.args[0]['except']['name'] == 'eosio_assert_message_exception'
            assert e.args[0]['except']['stack'][0]['data']['s'] == 'contract_code not valid!'

    def test_clear_code(self):
        code = r'''
def apply(a, b, c):
    print('hello,world')
'''
        code = self.compile(code)
        
        self.chain.deploy_contract('alice', code, b'', vmtype=3)

        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info('+++elapsed: %s', r['elapsed'])

        self.chain.deploy_contract('alice', b'', b'', vmtype=3)
        self.chain.produce_block()

    def test_transaction(self):
        code = os.path.join(test_dir, 'test_contracts', 'test_transaction.py')
        with open(code, 'r') as f:
            code = f.read()
        code = self.compile(code)

        self.chain.deploy_contract('alice', code, b'', vmtype=3)

        self.chain.push_action('alice', 'senddefer', b'hello,world')
        self.chain.produce_block()

        sender_id = 0x1ffffffffffffffff
        r = self.chain.chain.get_scheduled_transaction(sender_id, 'alice')
        # print('get_scheduled_transaction:', r)
        assert r and r['sender'] == 'alice' and r['sender_id'] == str(sender_id)

        for _ in range(10):
            self.chain.produce_block()

        r = self.chain.chain.get_scheduled_transaction(sender_id, 'alice')
        assert not r

#=============test cancel========================
        self.chain.push_action('alice', 'senddefer', b'hello,world')
        self.chain.produce_block()

        r = self.chain.chain.get_scheduled_transaction(sender_id, 'alice')
        assert r

        self.chain.push_action('alice', 'cancel', b'hello,world')
        self.chain.produce_block()

        r = self.chain.chain.get_scheduled_transaction(sender_id, 'alice')
        assert not r

    def test_vm_api_wasm(self):
        code = self.compile_cpp_file('test_vm_api')
        self.chain.deploy_contract('alice', code, b'')
        self.chain.push_action('alice', 'sayhello', b'hello,world')


