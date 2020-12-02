import os
import time
import pytest
import logging
from chaintest import ChainTest, logger
from uuos import application

test_dir = os.path.dirname(__file__)

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.main_token = 'UUOS'
        cls.chain = ChainTest(network_type=1)
        contract_name = 'testmetestme'
        args = {
            'account':contract_name,
            'is_priv':1
        }
        cls.chain.push_action('uuos', 'setpriv', args)

        contract_name = 'testmetestme'
        code = cls.chain.compile_py_code_from_file('token2.py')
        abi_file = os.path.join(test_dir, 'test_contracts', 'token.abi')
        with open(abi_file, 'rb') as f:
            abi = f.read()
        cls.chain.deploy_contract(contract_name, code, abi, vmtype=2)

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        pass

    def teardown_method(self, method):
        pass

    def test_token(self):
        contract_name = 'testmetestme'
        args = {"issuer": contract_name, "maximum_supply":f"11000000000.0000 UUOS"}
        r = self.chain.push_action(contract_name, 'create', args)

        args = {"to": contract_name, "quantity":f"1000000000.0000 UUOS", "memo":""}
        r = self.chain.push_action(contract_name,'issue', args, contract_name, 'active')

        balance = self.chain.get_balance(contract_name, token_account=contract_name, token_name='UUOS')
        logger.info(f'++++testmetestme balance:{balance}')

        self.chain.transfer('testmetestme', 'bob', 1.0, '1', token_account='testmetestme', token_name='UUOS')
        balance = self.chain.get_balance('bob', token_account=contract_name, token_name='UUOS')
        logger.info(f'++++bob balance:{balance}')

        balance = self.chain.get_balance(contract_name, token_account=contract_name, token_name='UUOS')
        logger.info(f'++++testmetestme balance:{balance}')

#./uuos/uuos -m pytest ../../programs/uuos/tests/test.py::Test::test_create_account

