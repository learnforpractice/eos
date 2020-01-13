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

gc.set_debug(gc.DEBUG_STATS)

logger = application.get_logger(__name__)


class TokenTest(ChainTest):

    def __init__(self, uuos_network=False, jit=False):
        super(TokenTest, self).__init__(uuos_network, jit)

    def test_call(self):
        code = r'''
#include <eosio/eosio.hpp>
#include <eosio/action.hpp>
#include <eosio/print.hpp>

extern "C" {
    __attribute__((eosio_wasm_import))
    int call_contract_get_extra_args(void* extra_args, size_t size1);

    __attribute__((eosio_wasm_import))
    int call_contract_set_results(void* result, size_t size1);

    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        eosio::print("hello,worlddddd\n");
    }

   void call(uint64_t func, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
      if (func == eosio::name("calltest1").value) {
         uint64_t extra_args;
         ::call_contract_get_extra_args(&extra_args, sizeof(extra_args));
         eosio::print("+++++++++++call: extra_args:", extra_args, "\n");
         extra_args += 1;
         ::call_contract_set_results(&extra_args, sizeof(extra_args));
      }
   }
}
        '''

        contract_name = 'helloworld12'
        code = wasmcompiler.compile_cpp_src(contract_name, code, entry='call', force=True)
        self.deploy_contract(contract_name, code, b'')
        self.produce_block()

        code = '''
def apply(receiver, code, action):
    ret = call_contract('helloworld12', 'calltest1', 0, 0, 0, int.to_bytes(1, 8, 'little'))
    print('+++call contract return:', ret)
    print(int.from_bytes(ret, 'little'))
'''
        code = self.compile_py_code(code)

        contract_name = 'helloworld11'
        self.deploy_contract(contract_name, code, b'', vmtype=1)
        self.push_action(contract_name, 'sayhello', b'')
        self.produce_block()

class TokenTestCase(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.info('+++++++++++++++++++++TokenTestCase++++++++++++++++')
        super(TokenTestCase, self).__init__(testName)
        self.extra_args = extra_args
#        UUOSTester.chain = self.chain

    def test_call(self):
        TokenTestCase.chain.test_call()

    @classmethod
    def setUpClass(cls):
        cls.chain = TokenTest(uuos_network=False, jit=False)

    @classmethod
    def tearDownClass(cls):
        if cls.chain:
            cls.chain.free()
            cls.chain = None

    def setUp(self):
        pass

    def tearDown(self):
        pass

if __name__ == '__main__':
    unittest.main()
