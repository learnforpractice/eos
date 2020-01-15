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

logger = application.get_logger(__name__)


class CallContractTest(ChainTest):

    def __init__(self, uuos_network=False, jit=False):
        super(CallContractTest, self).__init__(uuos_network, jit)

    def test_call_offchain(self):
        code = '''
import db
def apply(receiver, code, action):
    if not receiver == code:
        return

    code = receiver
    scope = 'scope'
    table = 'table'
    payer = receiver
    id = 'hello'
    data = read_action_data()
    itr = db.find_i64(code, scope, table, id)
#    print(itr)
    if N('setvalue') == action:
        if itr < 0:
            db.store_i64(scope, table, payer, id, data)
        else:
            old_data = db.get_i64(itr)
            print(old_data)
            db.update_i64(itr, receiver, data)
    elif N('getvalue') == action:
        data = db.get_i64(itr)
        set_action_return_value(data)
        '''
        contract_name = 'helloworld11'
        code = self.compile_py_code(code)
        self.deploy_contract(contract_name, code, b'', vmtype=1)
        self.produce_block()

        self.push_action(contract_name, 'setvalue', b'hello,world!')

        params = dict(
            code = contract_name,
            action = 'getvalue',
            binargs = ''
        )
        params = json.dumps(params)
        r = uuos.call_contract_off_chain(params)
        print(r)
        assert 'output' in r and bytes.fromhex(r['output']) == b'hello,world!'

        params = dict(
            code = contract_name,
            action = 'setvalue',
            binargs = ''
        )
        params = json.dumps(params)
        r = uuos.call_contract_off_chain(params)
        assert 'error' in r and r['error']['code'] == 3160002 #tx_cpu_usage_exceeded
        logger.info(r['error'])




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
