import os
import sys
import unittest
from uuos import application
from uuos import wasmcompiler
from chaintest import ChainTest
import uuos
from uuoskit import uuosapi
print(os.getpid())
# input('<<<')

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

logger = application.get_logger(__name__)


class Test(object):

    @classmethod
    def setup_class(cls):
        cls.main_token = 'UUOS'
        cls.chain = ChainTest(network_type=1, jit=True)

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        pass

    def teardown_method(self, method):
        pass

    def test_call1(self):
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
import chain
def apply(receiver, code, action):
    args = struct.pack('QQ', chain.s2n('calltest1'), 1)
    print(args)
    ret = chain.call_contract('bob', args)
    print(ret)
'''
        code = uuos.compile(code)
        self.chain.deploy_python_contract('alice', code, b'')
        self.chain.push_action('alice', 'sayhello', b'')
        self.chain.produce_block()

    def test_call2(self):
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
         eosio::check(args_size == 16, "bad args size");
         if (args[0] == eosio::name("calltest1").value) {
            eosio::print("+++++++++++call: args[1]:", args[1], "\n");
            args[1] += 1;
            // ::call_contract_set_results(&args[1], sizeof(uint64_t));
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
import chain
def apply(receiver, code, action):
    args = struct.pack('QQ', chain.s2n('calltest1'), 1)
    print(args)
    ret = chain.call_contract('bob', args)
    print(ret)
    assert not ret
'''
        code = uuos.compile(code)
        self.chain.deploy_python_contract('alice', code, b'')
        self.chain.push_action('alice', 'sayhello', b'')
        self.chain.produce_block()

    def test_call3(self):
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
         eosio::check(args_size == 16, "bad args size");
         if (args[0] == eosio::name("calltest1").value) {
            eosio::print("+++++++++++call: args[1]:", args[1], "\n");
            args[1] += 1;
            // ::call_contract_set_results(&args[1], sizeof(uint64_t));
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
import chain
def apply(receiver, code, action):
    args = struct.pack('QQ', chain.s2n('calltest1'), 1)
    print(args)
    ret = chain.call_contract('bob', args)
    print(ret)
    assert not ret
    while True:
        pass
'''
        code = uuos.compile(code)
        self.chain.deploy_python_contract('alice', code, b'')
        try:
            self.chain.push_action('alice', 'sayhello', b'')
        except Exception as e:
#            logger.info(e.args[0])
            assert e.args[0]['action_traces'][0]['console'] == "b'\\x00\\x00\\x08\\x19\\xab\\x1c\\xa3A\\x01\\x00\\x00\\x00\\x00\\x00\\x00\\x00'\r\n+++++++++++call: args[1]:1\nNone\r\n"
            exception_name = e.args[0]['action_traces'][0]['except']['name']
            assert exception_name == 'tx_cpu_usage_exceeded'
        self.chain.produce_block()

    def test_call4(self):
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
         eosio::check(args_size == 16, "bad args size");
         if (args[0] == eosio::name("calltest1").value) {
            eosio::print("+++++++++++call: args[1]:", args[1], "\n");
            args[1] += 1;
            // ::call_contract_set_results(&args[1], sizeof(uint64_t));
         }
         while(true){};
    }
}
        '''

        contract_name = 'bob'
        code = wasmcompiler.compile_cpp_src(contract_name, code, entry='apply', force=True)
        self.chain.deploy_contract(contract_name, code, b'')
        self.chain.produce_block()

        code = '''
import struct
import chain
def apply(receiver, code, action):
    args = struct.pack('QQ', chain.s2n('calltest1'), 1)
    print(args)
    ret = chain.call_contract('bob', args)
    print(ret)
    assert not ret
'''
        code = uuos.compile(code)
        self.chain.deploy_python_contract('alice', code, b'')
        try:
            self.chain.push_action('alice', 'sayhello', b'')
            assert 0
        except Exception as e:
            # logger.info(e.args[0])
            assert e.args[0]['action_traces'][0]['console'] == "b'\\x00\\x00\\x08\\x19\\xab\\x1c\\xa3A\\x01\\x00\\x00\\x00\\x00\\x00\\x00\\x00'\r\n+++++++++++call: args[1]:1\n"
            exception_name = e.args[0]['action_traces'][0]['except']['name']
            assert exception_name == 'tx_cpu_usage_exceeded'
        self.chain.produce_block()

    def test_hello(self):
        contract_name = 'uuos.mpy'
        self.chain.push_action(contract_name, 'hellompy', b'')

        code = '''
def apply(receiver, code, action):
    for i in range(10):
        print('hello,world')
#    while True: pass
'''
        code = uuos.compile(code)
#        print(code)
        args = uuosapi.s2b(contract_name) + code
        print(args)
        r = self.chain.push_action(contract_name, 'setcode', args, {contract_name:'active'})
        r = self.chain.push_action(contract_name, 'sayhello', b'')
        logger.info(r['elapsed'])
        self.chain.produce_block()