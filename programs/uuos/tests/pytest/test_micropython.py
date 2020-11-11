import os
import time
import json
import pytest
import logging
import subprocess

from chaintest import ChainTest
from uuos import application
from uuos import wasmcompiler

test_dir = os.path.dirname(__file__)

logger = application.get_logger(__name__)

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.chain = ChainTest(uuos_network=True)

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        pass

    def teardown_method(self, method):
        pass

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
        except Exception as e:
            assert e.args[0]['except']['name'] == 'deadline_exception'

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
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        try:
            r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        except Exception as e:
            logger.info(e.args[0]['action_traces'][0]['console'])
            assert e.args[0]['except']['name'] == 'python_execution_error'
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
        count = 6000
        start = time.time()
        for i in range(count):
            r = self.chain.push_action('alice', 'sayhello', str(i).encode('utf8'))
            total_time += r['elapsed']
        print('duration:', time.time() - start)
        logger.info(r['action_traces'][0]['console'])
        logger.info('+++elapsed: %s', r['elapsed'])
        self.chain.produce_block()
        print(total_time/count, 1e6/(total_time/count))

    def test_call(self):
        # print(os.getpid())
        # input('<<<')
        code = r'''
#include <eosio/eosio.hpp>
#include <eosio/action.hpp>
#include <eosio/print.hpp>

extern "C" {
    __attribute__((eosio_wasm_import))
    int call_contract_get_args(void* args, size_t size1);

    __attribute__((eosio_wasm_import))
    int call_contract_set_results(void* result, size_t size1);

    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
         uint64_t args[2];
         int args_size = ::call_contract_get_args(&args, sizeof(args));
         eosio::print("+++++++++++call: arg count:", args_size, "\n");
         eosio::check(args_size == 16, "bad args size");
         if (args[0] == eosio::name("calltest1").value) {
            eosio::print("+++++++++++call: args[1]:", args[1], "\n");
            args[1] += 1;
            ::call_contract_set_results(&args[1], sizeof(uint64_t));
         }
    }
}
        '''

        contract_name = 'bob'
        code = wasmcompiler.compile_cpp_src(contract_name, code, entry='apply', force=True)
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
        self.chain.deploy_contract('alice', code, b'', vmtype=3)

        try:
            r = self.chain.push_action('alice', 'sayhello', b'hello,world')
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
def apply(a, b, c):
    a = 'a'*(1024*128)
'''
        code = self.compile(code)
        self.chain.deploy_contract('alice', code, b'', vmtype=3)
        try:
            r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        except Exception as e:
            assert e.args[0]['except']['name'] == 'eosio_assert_message_exception'
            assert e.args[0]['except']['stack'][0]['data']['s'] == 'no free vm memory left!'
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
