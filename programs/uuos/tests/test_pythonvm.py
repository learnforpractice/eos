import os
import sys
import json
import gc
import struct
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

import uuos
from uuos import application
from uuos import wasmcompiler

from chaintest import ChainTest

logger = application.get_logger(__name__)


class PythonVMTest(ChainTest):

    def __init__(self, uuos_network=False, jit=False):
        super(PythonVMTest, self).__init__(uuos_network, jit)

    def test_api(self):
        contract_name = 'helloworld11'
        code = self.compile_py_code_from_file('pythonvm_test.py')
        self.deploy_contract(contract_name, code, b'', vmtype=1)
        self.produce_block()

        self.push_action(contract_name, 'test1', b'hello,world!')
        self.push_action(contract_name, 'test2', b'hello,world!')
        self.push_action(contract_name, 'test3', '你好，世界!'.encode('utf8'))
        try:
            self.push_action(contract_name, 'test4', b'')
        except Exception as e:
            assert e.args[0]['code'] == 3080006 #deadline_exception

        try:
            self.push_action(contract_name, 'test5', b'')
        except Exception as e:
            assert e.args[0]['code'] == 3050003 #eosio_assert_message_exception
            assert e.args[0]['stack'][0]['data']['s'] == 'no free vm memory left!'
        self.produce_block()

        start = 0
        for i in range(10):
            end = start + 10 * 1024
            args = struct.pack('III', 3*1024*1024, start, end)
            r = self.push_action(contract_name, 'test51', args)
            if i % 50 == 0:
                self.produce_block()
            start += 10*1024

class PythonVMTestCase(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.debug('+++++++++++++++++++++PythonVMTestCase++++++++++++++++')
        super(PythonVMTestCase, self).__init__(testName)
        self.extra_args = extra_args

    def test_api(self):
        PythonVMTestCase.chain.test_api()

    @classmethod
    def setUpClass(cls):
        cls.chain = PythonVMTest(uuos_network=False, jit=False)

    @classmethod
    def tearDownClass(cls):
        if cls.chain:
            cls.chain.free()
            cls.chain = None

    def setUp(self):
        pass

    def tearDown(self):
        pass

class PythonVMTestCaseJIT(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.debug('+++++++++++++++++++++PythonVMTestCaseJIT++++++++++++++++')
        super(PythonVMTestCaseJIT, self).__init__(testName)
        self.extra_args = extra_args

    def test_api(self):
        PythonVMTestCaseJIT.chain.test_api()

    @classmethod
    def setUpClass(cls):
        cls.chain = PythonVMTest(uuos_network=False, jit=True)

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