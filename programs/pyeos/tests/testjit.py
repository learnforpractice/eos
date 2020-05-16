import os
import sys
import gc
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos import application
from chaintest import ChainTest

logger = application.get_logger(__name__)

class UUOSTester1(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.info('+++++++++++++++++++++UUOSTester1++++++++++++++++')
        super(UUOSTester1, self).__init__(testName)
        self.extra_args = extra_args
#        UUOSTester.chain = self.chain

    def test_jit(self):
        UUOSTester1.chain.test_jit()

    @classmethod
    def setUpClass(cls):
        logger.info('++++++++++++setUpClass++++++++++++')
        cls.chain = ChainTest(uuos_network=False, jit=True)

    @classmethod
    def tearDownClass(cls):
        if cls.chain:
            cls.chain.free()
            cls.chain = None

    def setUp(self):
        pass

    def tearDown(self):
        pass

class UUOSTester2(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.info('+++++++++++++++++++++UUOSTester1++++++++++++++++')
        super(UUOSTester2, self).__init__(testName)
        self.extra_args = extra_args
#        UUOSTester.chain = self.chain

    def test_jit(self):
        UUOSTester2.chain.test_jit()

    @classmethod
    def setUpClass(cls):
        logger.info('++++++++++++setUpClass++++++++++++')
        cls.chain = ChainTest(uuos_network=False, jit=False)

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
