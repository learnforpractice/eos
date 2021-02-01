import os
import json
from chaintest import ChainTest
from uuosio import log, uuos
logger = log.get_logger(__name__)

class TestMicropython(object):

    @classmethod
    def setup_class(cls):
        cls.chain = ChainTest()

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        logger.warning('test start: %s', method.__name__)

    def teardown_method(self, method):
        pass

    def test_hello(self):
        r = self.chain.push_action('eosio.mpy', 'hellompy', b'', {'hello':'active'})
        logger.info(r['action_traces'][0]['console'])
        self.chain.produce_block()
        
        # block = self.chain.chain.fetch_block_by_number(1)
        # logger.info(block)

        # block = self.chain.chain.fetch_block_by_number(2)
        # logger.info(block)
