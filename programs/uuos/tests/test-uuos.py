import os
import sys
import gc
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos import application
from chaintest import ChainTest


logger = application.get_logger(__name__)

class UUOSTester2(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.info('+++++++++++++++++++++UUOSTester++++++++++++++++')
        super(UUOSTester2, self).__init__(testName)
        self.extra_args = extra_args
#        UUOSTester.chain = self.chain
    def test_create_account(self):
        UUOSTester2.chain.test_create_account()

    @classmethod
    def setUpClass(cls):
        cls.chain = ChainTest(True)

    @classmethod
    def tearDownClass(cls):
        if cls.chain:
            cls.chain.free()
            cls.chain = None

if __name__ == '__main__':
    unittest.main()
