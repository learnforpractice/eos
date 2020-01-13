import os
import sys
import json
import gc
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos import application
from chaintest import ChainTest

gc.set_debug(gc.DEBUG_STATS)

logger = application.get_logger(__name__)

class TokenTest(ChainTest):

    def __init__(self, uuos_network=False, jit=False):
        super(TokenTest, self).__init__(uuos_network, jit)

    def test_token(self):
        contract_name = 'helloworld11'
        code = self.compile_py_code_from_file('token.py')
        abi_file = os.path.join(test_dir, 'test_contracts', 'token.abi')
        with open(abi_file, 'rb') as f:
            abi = f.read()
        self.deploy_contract(contract_name, code, abi, vmtype=1)

        args = {"issuer": contract_name, "maximum_supply":f"11000000000.0000 {self.main_token}"}
        r = self.push_action(contract_name, 'create', args)

        args = {"to": contract_name, "quantity":f"1000000000.0000 {self.main_token}", "memo":""}
        r = self.push_action(contract_name,'issue', args, contract_name, 'active')

        a1 = self.get_balance(contract_name, token_account=contract_name)
        a2 = self.get_balance('helloworld12', token_account=contract_name)

        args = {"from": contract_name, 'to': 'helloworld12', "quantity":f"100.0000 {self.main_token}", "memo":""}
        r = self.push_action(contract_name,'transfer', args, contract_name, 'active')

        b1 = self.get_balance(contract_name, token_account=contract_name)
        b2 = self.get_balance('helloworld12', token_account=contract_name)
        assert a1 - 100.0 == b1
        assert a2 + 100.0 == b2
        self.produce_block()

class TokenTestCase(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.info('+++++++++++++++++++++TokenTestCase++++++++++++++++')
        super(TokenTestCase, self).__init__(testName)
        self.extra_args = extra_args
#        UUOSTester.chain = self.chain

    def test_token(self):
        TokenTestCase.chain.test_token()

    @classmethod
    def setUpClass(cls):
        cls.chain = TokenTest(uuos_network=False, jit=True)

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
