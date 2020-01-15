import os
import sys
import gc
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos import application
from chaintest import ChainTest

logger = application.get_logger(__name__)

class UUOSTester(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.info('+++++++++++++++++++++UUOSTester++++++++++++++++')
        super(UUOSTester, self).__init__(testName)
        self.extra_args = extra_args
#        UUOSTester.chain = self.chain

    @classmethod
    def setUpClass(cls):
        cls.chain = ChainTest()

    @classmethod
    def tearDownClass(cls):
        if cls.chain:
            cls.chain.free()
            cls.chain = None

    def test_set_action_return_value(self):
        UUOSTester.chain.test_set_action_return_value()

    def test_jit(self):
        UUOSTester.chain.test_jit()

    def test1(self):
        UUOSTester.chain.test1()

    def test2(self):
        UUOSTester.chain.test2()

    def test3(self):
        UUOSTester.chain.test3()
    
    def test4(self):
        UUOSTester.chain.test4()

    def test5(self):
        UUOSTester.chain.test5()

    def test6(self):
        logger.info('+++++++++++++test6+++++++++++++++')
        UUOSTester.chain.test6()
    
    def test7(self):
        UUOSTester.chain.test7()

if __name__ == '__main__':
    unittest.main()
