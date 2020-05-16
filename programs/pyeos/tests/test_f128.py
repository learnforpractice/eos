import os
import sys
import gc
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos import application
from chaintest import ChainTest

logger = application.get_logger(__name__)

class DBTest(ChainTest):

    def test_float128(self):
        code = r'''
import _float128
def apply(receiver, code, action):
    a = b'\xb4\xc8v\xbe\x9f\x1a\x07\xde\x08@\x00\x00\x00\x00\x00\x00'
    b = a
    c = _float128.add(a, b)
    print(c)

    # c = _float128.add(11.2, 22.3)
    # print(c)
        '''

        name = 'helloworld11'
        code = self.compile_py_code(code)
        self.deploy_contract(name, code, b'', 1)

        r = self.push_action(name, 'destroy', b'')
        self.produce_block()

class DBTesterCase(unittest.TestCase):
    def __init__(self, testName):
        logger.info('+++++++++++++++++++++DBTesterCase++++++++++++++++')
        super(DBTesterCase, self).__init__(testName)

    def test_float128(self):
        logger.info('+++++++++++++db_test1+++++++++++++++')
        DBTesterCase.chain.test_float128()

    @classmethod
    def setUpClass(cls):
        logger.info('++++++++++++setUpClass++++++++++++')
        cls.chain = DBTest()

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
