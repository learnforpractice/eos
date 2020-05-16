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


class VMAPITest(ChainTest):

    def __init__(self, uuos_network=False, jit=False):
        super(VMAPITest, self).__init__(uuos_network, jit)

    def test_api(self):
        contract_name = 'helloworld11'
        code = self.compile_py_code_from_file('api_test.py')
        self.deploy_contract(contract_name, code, b'', vmtype=1)
        self.produce_block()

        self.push_action(contract_name, 'test1', b'hello,world!')
        self.push_action(contract_name, 'test2', b'hello,world!')
        self.push_action(contract_name, 'test3', b'hello,world!')


class VMAPITestCase(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.debug('+++++++++++++++++++++VMAPITestCase++++++++++++++++')
        super(VMAPITestCase, self).__init__(testName)
        self.extra_args = extra_args

    def test_api(self):
        VMAPITestCase.chain.test_api()

    @classmethod
    def setUpClass(cls):
        logger.info('++++++++++++setUpClass++++++++++++')
        cls.chain = VMAPITest(uuos_network=False, jit=False)

    @classmethod
    def tearDownClass(cls):
        if cls.chain:
            cls.chain.free()
            cls.chain = None

    def setUp(self):
        pass

    def tearDown(self):
        pass

class VMAPITestCaseJIT(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.debug('+++++++++++++++++++++VMAPITestCaseJIT++++++++++++++++')
        super(VMAPITestCaseJIT, self).__init__(testName)
        self.extra_args = extra_args
#        UUOSTester.chain = self.chain

    def test_api(self):
        VMAPITestCaseJIT.chain.test_api()

    @classmethod
    def setUpClass(cls):
        logger.info('++++++++++++setUpClass++++++++++++')
        cls.chain = VMAPITest(uuos_network=False, jit=True)

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
