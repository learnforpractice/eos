import os
import time
import pytest
import logging
from chaintest import ChainTest

logging.basicConfig(filename='logfile.log', level=logging.INFO, format='%(asctime)s %(levelname)s %(lineno)d %(module)s %(message)s')
logger=logging.getLogger(__name__)

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

    def test_performance(self):
        contract_name = 'alice'
        args = {
            'account':contract_name,
            'is_priv':1
        }
        self.chain.push_action('uuos', 'setpriv', args)

        code = '''
def apply(receiver, code, action):
    print('hello, python')
'''
        code = self.chain.compile_py_code(code)

        self.chain.deploy_contract(contract_name, code, b'', vmtype=2)
        self.chain.push_action(contract_name, 'sayhello', b'hello,world')
        self.chain.push_action(contract_name, 'sayhello', b'hello,world again')

#./uuos/uuos -m pytest ../../programs/uuos/tests/test.py::Test::test_create_account

