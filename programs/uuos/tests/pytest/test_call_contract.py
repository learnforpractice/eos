import os
import sys
import json
import gc
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos import application
from uuos import wasmcompiler

from chaintest import ChainTest

logger = application.get_logger(__name__)

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.main_token = 'UUOS'

    @classmethod
    def teardown_class(cls):
        pass

    def setup_method(self, method):
        pass
        self.chain = ChainTest(uuos_network=True)

    def teardown_method(self, method):
        pass
        self.chain.free()

    def test_call(self):
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

        contract_name = 'helloworld12'
        code = wasmcompiler.compile_cpp_src(contract_name, code, entry='apply', force=True)
        self.chain.deploy_contract(contract_name, code, b'')
        self.chain.produce_block()

        code = '''
import struct
def apply(receiver, code, action):
    calltest1 = N('calltest1')
    args = struct.pack('QQ', calltest1, 1)
    ret = call_contract('helloworld12', args)
    print('+++call contract return:', len(ret), ret)
    ret = int.from_bytes(ret, 'little')
    print(ret)
    assert ret == 2
'''
        code = self.chain.compile_py_code(code)

        contract_name = 'helloworld11'
        self.chain.deploy_contract(contract_name, code, b'', vmtype=1)
        self.chain.push_action(contract_name, 'sayhello', b'')
        self.chain.push_action(contract_name, 'sayhello', b'a')
        self.chain.produce_block()
