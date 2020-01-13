import os
import sys
import json
import gc
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

import uuos
from uuos import application
from uuos import wasmcompiler

from chaintest import ChainTest

gc.set_debug(gc.DEBUG_STATS)

logger = application.get_logger(__name__)


class CallContractTest(ChainTest):

    def __init__(self, uuos_network=False, jit=False):
        super(CallContractTest, self).__init__(uuos_network, jit)

    def test_call_offchain(self):
        code = r'''
#include <eosio/eosio.hpp>
#include <eosio/action.hpp>
#include <eosio/print.hpp>

extern "C" {
    __attribute__((eosio_wasm_import))
    void set_action_return_value(const char *packed_blob, size_t size);

    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        std::vector<char> data;
        size_t size = eosio::action_data_size();
        data.resize(size);
        eosio::read_action_data(data.data(), size);
        set_action_return_value(data.data(), data.size());
    }
}
        '''

        contract_name = 'helloworld11'
        code = wasmcompiler.compile_cpp_src(contract_name, code, entry='apply')
        self.deploy_contract(contract_name, code, b'')
        self.produce_block()

        params = dict(
            code = contract_name,
            action = 'sayhello',
            binargs = 'aabb'
        )
        params = json.dumps(params)
        r = uuos.call_contract_off_chain(params)
        logger.info(f'++++++++call_contract_off_chain return {r}')

class CallContractOffChainTestCase(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.info('+++++++++++++++++++++CallContractOffChainTestCase++++++++++++++++')
        super(CallContractOffChainTestCase, self).__init__(testName)
        self.extra_args = extra_args
#        UUOSTester.chain = self.chain

    def test_call_offchain(self):
        CallContractOffChainTestCase.chain.test_call_offchain()

    @classmethod
    def setUpClass(cls):
        cls.chain = CallContractTest(uuos_network=False, jit=False)

    @classmethod
    def tearDownClass(cls):
        if cls.chain:
            cls.chain.free()
            cls.chain = None

    def setUp(self):
        pass

    def tearDown(self):
        pass

class CallContractOffChainTestCaseJIT(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.info('+++++++++++++++++++++CallContractOffChainTestCaseJIT++++++++++++++++')
        super(CallContractOffChainTestCaseJIT, self).__init__(testName)
        self.extra_args = extra_args
#        UUOSTester.chain = self.chain

    def test_call_offchain(self):
        CallContractOffChainTestCaseJIT.chain.test_call_offchain()

    @classmethod
    def setUpClass(cls):
        cls.chain = CallContractTest(uuos_network=False, jit=True)

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
