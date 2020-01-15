import os
import sys
import gc
import struct
import ujson as json
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos import application
from chaintest import ChainTest

gc.set_debug(gc.DEBUG_STATS)

logger = application.get_logger(__name__)

from uuos import db
from uuos.saferunner import safe_runner

class ChainDBTest(ChainTest):

    def __init__(self, uuos_network=False, jit=False):
        super(ChainDBTest, self).__init__(uuos_network, jit)

    @safe_runner
    def test_chain_db_api(self):
        self.chain.set_apply_context()
        symbol = b'\x04UUOS\x00\x00\x00'
        symbol = int.from_bytes(symbol, 'little')
        symbol >>= 8
        itr = db.find_i64('eosio.token', 'eosio', 'accounts', symbol)
        logger.info(itr)
        value = db.get_i64(itr)
        amount, symbol = struct.unpack('q8s', value)
        amount /=10000
        logger.info(f'{amount}, {symbol}')
    
    def test_safe_runner(self):
        self.chain.set_apply_context()
        db.get_i64(1)

class ChainDBTestCase(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.info('+++++++++++++++++++++ChainDBTestCase++++++++++++++++')
        super(ChainDBTestCase, self).__init__(testName)
        self.extra_args = extra_args

    def test_chain_db_api(self):
        logger.info('+++++++++++++db_test1+++++++++++++++')
        ChainDBTestCase.chain.test_chain_db_api()

    def test_safe_runner(self):
        try:
            ChainDBTestCase.chain.test_safe_runner()
        except Exception as e:
            logger.info(e.args)
            assert e.args[0]['code'] == 3160003

    @classmethod
    def setUpClass(cls):
        cls.chain = ChainDBTest()

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
