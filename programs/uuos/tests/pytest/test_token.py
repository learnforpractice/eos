import os
import sys
import json
import gc
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos import application
from chaintest import ChainTest

logger = application.get_logger(__name__)

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.main_token = 'UUOS'
        cls.chain = chain = ChainTest(network_type=1)
        contract_name = 'testmetestme'
        code = chain.compile_py_code_from_file('token.py')
        abi_file = os.path.join(test_dir, 'test_contracts', 'token.abi')
        with open(abi_file, 'rb') as f:
            abi = f.read()
        chain.deploy_contract(contract_name, code, abi, vm_type=1)

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        pass

    def teardown_method(self, method):
        pass

    def test_token(self):
        contract_name = 'testmetestme'
        args = {"issuer": contract_name, "maximum_supply":f"11000000000.0000 {self.main_token}"}
        r = self.chain.push_action(contract_name, 'create', args)

        args = {"to": contract_name, "quantity":f"1000000000.0000 {self.main_token}", "memo":""}
        r = self.chain.push_action(contract_name,'issue', args, contract_name, 'active')

        for i in range(5):
            a1 = self.chain.get_balance(contract_name, token_account=contract_name)
            a2 = self.chain.get_balance('uuos', token_account=contract_name)

            args = {"from": contract_name, 'to': 'uuos', "quantity":f"100.0000 {self.main_token}", "memo":str(i)}
            r = self.chain.push_action(contract_name,'transfer', args, contract_name, 'active')
            logger.info(f'++++r.elapsed: {r.elapsed}')

            b1 = self.chain.get_balance(contract_name, token_account=contract_name)
            b2 = self.chain.get_balance('uuos', token_account=contract_name)
            assert a1 - 100.0 == b1
            assert a2 + 100.0 == b2
        self.chain.produce_block()

    def test_token_close(self):
        contract_name = 'testmetestme'
        token_name = 'UUUV'
        args = {"issuer": contract_name, "maximum_supply":f"11000000000.0000 {token_name}"}
        r = self.chain.push_action(contract_name, 'create', args)

        args = {"to": contract_name, "quantity":f"1000000000.0000 {token_name}", "memo":""}
        r = self.chain.push_action(contract_name,'issue', args, contract_name, 'active')

        args = {"owner": 'bob', "symbol":f"4,{token_name}", "ram_payer": contract_name}
        r = self.chain.push_action(contract_name,'open', args, contract_name, 'active')

        args = {"owner": 'bob', "symbol":f"4,{token_name}"}
        r = self.chain.push_action(contract_name,'close', args, 'bob', 'active')
        self.chain.produce_block()

        self.chain.transfer('testmetestme', 'bob', 1.0, '1', token_account='testmetestme', token_name='UUUV')
        self.chain.transfer('bob', 'testmetestme', 1.0, '2', token_account='testmetestme', token_name='UUUV')

        args = {"owner": 'bob', "symbol":f"4,{token_name}"}
        r = self.chain.push_action(contract_name,'close', args, 'bob', 'active')

        self.chain.produce_block()

    def test_retire(self):
        contract_name = 'testmetestme'
        token_name = 'KKKK'

        args = {"issuer": contract_name, "maximum_supply":f"11000000000.0000 {token_name}"}
        r = self.chain.push_action(contract_name, 'create', args)

        args = {"to": contract_name, "quantity":f"1000000000.0000 {token_name}", "memo":""}
        r = self.chain.push_action(contract_name,'issue', args, contract_name, 'active')

        args = {"quantity":f"100.0000 {token_name}", "memo":""}
        r = self.chain.push_action(contract_name,'retire', args, contract_name, 'active')
        assert 1000000000.0 - 100.0 == self.chain.get_balance(contract_name, token_account=contract_name, token_name='KKKK')
        self.chain.produce_block()
