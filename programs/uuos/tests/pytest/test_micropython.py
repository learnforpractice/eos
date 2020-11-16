import os
import time
import json
import pytest
import logging
import subprocess

from chaintest import ChainTest
from uuos import log
from uuos import wasmcompiler

test_dir = os.path.dirname(__file__)

logger = log.get_logger(__name__)

# print(os.getpid())
# input('<<<')

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.chain = ChainTest(uuos_network=True, jit=True)

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        logger.warning('test start: %s', method.__name__)

    def teardown_method(self, method):
        logger.warning('test end: %s', method.__name__)

    def compile(self, code):
        with open('tmp.py', 'w') as f:
            f.write(code)
        subprocess.check_output(['mpy-cross', '-o', 'tmp.mpy', 'tmp.py'])
        with open('tmp.mpy', 'rb') as f:
            code = f.read()
        os.remove('tmp.py')
        os.remove('tmp.mpy')
        return code

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
        self.chain.produce_block()

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

        code = os.path.join(test_dir, '..', 'test_contracts', 'vm_api_test.py')
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

        # r = self.chain.push_action(contract_name, 'sayhello', b'hello,world again')
        # logger.info(r['elapsed'])

        self.chain.produce_block()

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

        code = os.path.join(test_dir, '..', 'test_contracts', 'test_action.py')
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

        self.chain.produce_block()

    def test_db_api(self):
        code = os.path.join(test_dir, '..', 'test_contracts', 'vm_api_db_test.py')
        with open(code, 'r') as f:
            code = f.read()
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info(r['action_traces'][0]['console'])
        logger.info('+++elapsed: %s', r['elapsed'])

        self.chain.produce_block()

    def test_db_i256(self):
        code = os.path.join(test_dir, '..', 'test_contracts', 'test_db_i256.py')
        with open(code, 'r') as f:
            code = f.read()
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)

        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info('+++elapsed: %s', r['elapsed'])
        self.chain.produce_block()

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
        self.chain.produce_block()

    def test_init_exception(self):
        code = '''
aa = bb
def apply(a, b, c):
    raise Exception('oops!')
'''
        code = self.compile(code)
        e = self.chain.deploy_contract('alice', code, b'', vmtype=3)
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
        self.chain.produce_block()

    def test_bigint(self):
        code = os.path.join(test_dir, '..', 'test_contracts', 'test_bigint.py')
        with open(code, 'r') as f:
            code = f.read()
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info(r['action_traces'][0]['console'])
        logger.info('+++elapsed: %s', r['elapsed'])

        self.chain.produce_block()

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

        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info(r['action_traces'][0]['console'])
        logger.info('+++elapsed: %s', r['elapsed'])
        self.chain.produce_block()

        code = r'''
def apply(a, b, c):
    a = bigint(0xffffffffffffffff)
    b = bigint(0xffffffffffffffff)
    for i in range(500):
        a * b
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info(r['action_traces'][0]['console'])
        logger.info('+++elapsed: %s', r['elapsed'])
        self.chain.produce_block()

        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info(r['action_traces'][0]['console'])
        logger.info('+++elapsed: %s', r['elapsed'])
        self.chain.produce_block()

    def test_float128(self):
        code = os.path.join(test_dir, '..', 'test_contracts', 'test_float128.py')
        with open(code, 'r') as f:
            code = f.read()
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info(r['action_traces'][0]['console'])
        logger.info('+++elapsed: %s', r['elapsed'])
        self.chain.produce_block()

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

        self.chain.produce_block()

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
        self.chain.produce_block()
        print(total_time/count, 1e6/(total_time/count))

    def test_call_contract(self):
        # print(os.getpid())
        # input('<<<')
        code_file = os.path.join(test_dir, '..', 'test_contracts', 'test_call_contract.cpp')
        wasm_file = os.path.join(test_dir, '..', 'test_contracts', 'test_call_contract.wasm')
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

        self.chain.produce_block()

    def test_multi_index(self):
        code = os.path.join(test_dir, '..', 'test_contracts', 'test_multi_index.py')
        with open(code, 'r') as f:
            code = f.read()
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)

        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info('+++elapsed: %s', r['elapsed'])
        self.chain.produce_block()

        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        logger.info('+++elapsed: %s', r['elapsed'])
        self.chain.produce_block()

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
            e = self.chain.deploy_contract('alice', code, b'', vmtype=3)
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
        self.chain.produce_block()

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
        self.chain.produce_block()

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
        return

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
        self.chain.produce_block()

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
        self.chain.produce_block()

    def test_mpy_frozen(self):
        code = r'''
import json
import struct
from db import *

class MyData(object):
    def __init__(self, a: int, b: int, c: int, d: float):
        self.a = a
        self.b = b
        self.c = c
        self.d = d
        self.payer = 0

    def pack(self):
        b = int.to_bytes(self.b, 16, 'little')
        c = int.to_bytes(self.c, 32, 'little')
        return struct.pack('Q16s32sd', self.a, b, c, self.d)

    @classmethod
    def unpack(cls, data):
        a, b, c, d = struct.unpack('Q16s32sd', data)
        b = int.from_bytes(b, 'little')
        c = int.from_bytes(c, 'little')
        return cls(a, b, c, d)

    def get_primary_key(self):
        return self.a

    def get_secondary_values(self):
        return (self.a, self.b, self.c, self.d)

    @staticmethod
    def get_secondary_indexes():
        return (idx64, idx128, idx256, idx_double)

class MyData2(object):
    def __init__(self, a: int, b: int, c: int, d: float):
        self.a = a
        self.b = b
        self.c = c
        self.d = d
        self.payer = 0

    def pack(self):
        data = (self.a, self.b, self.c, self.d)
        return json.dumps(data)

    @classmethod
    def unpack(cls, data):
        data = json.loads(data)
        return cls(data[0], data[1], data[2], data[3])

    def get_primary_key(self):
        return self.a

    def get_secondary_values(self):
        return (self.a, self.b, self.c, self.d)

    @staticmethod
    def get_secondary_indexes():
        return (idx64, idx128, idx256, idx_double)

    def __repr__(self):
        return (self.a, self.b, self.c, self.d)

    def __str__(self):
        data = (self.a, self.b, self.c, self.d)
        return json.dumps(data)

def apply(receiver, code, action):
    code = name('alice')
    scope = name('scope1')
    table = name('table')
    payer = name('alice')

    if True:
        mi = MultiIndex(code, scope, table, MyData)

        try:
            itr = mi.find(1)
            if itr >= 0:
                data = mi.get(itr)
                print(data.a, data.b, data.c, data.d)
        except Exception as e:
            print(e)

        d = MyData(1, 2, 3, 5.0)
        d.payer = payer
        mi[1] = d

        itr, primary = mi.idx_find(3, 4.0)
        print(itr, primary)

        itr, primary, secondary = mi.idx_lowerbound(3, 1.0)
        print(itr, primary, secondary)

    if True:
        table = name('table2')
        mi = MultiIndex(code, scope, table, MyData2)
        d = MyData2(1, 2, 3, 5.0)
        d.payer = payer
        mi[1] = d

        print(mi[1])
        itr, primary, secondary = mi.idx_lowerbound(3, 1.0)
        print(itr, primary, secondary)
'''
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
        self.chain.produce_block()

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
        try:
            r = self.chain.deploy_contract('alice', code, b'', vmtype=3)
            logger.info('+++elapsed: %s', r['elapsed'])
        except:
            pass
        r = self.chain.push_action('alice', 'sayhello', b'hello,world2')
        logger.info('+++elapsed: %s', r['elapsed'])
        self.chain.produce_block()

    def test_token(self):
        code = os.path.join(test_dir, '..', 'test_contracts', 'test_token.py')
        with open(code, 'r') as f:
            code = f.read()
        code = self.compile(code)
        r = self.chain.deploy_contract('alice', code, b'', vmtype=3)
        # print(r)
        assert not isinstance(r, Exception) or r.args[0]['except']['name'] == 'set_exact_code', r

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
        self.chain.produce_block()
