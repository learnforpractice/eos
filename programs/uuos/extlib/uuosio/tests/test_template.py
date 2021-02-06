import os
import json
import pytest
import logging
import tempfile
from uuosio import chain, chainapi, uuos
from datetime import datetime, timedelta

logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s')

logger=logging.getLogger(__name__)

data_dir = tempfile.mkdtemp()
config_dir = tempfile.mkdtemp()

class Template(object):

    @classmethod
    def setup_class(cls):
        uuos.set_log_level('default', 0)
        uuos.set_block_interval_ms(1000)

        cls.chain_config = json.dumps(chain_config)
        cls.genesis_test = json.dumps(genesis_test)
        uuos.set_log_level('default', 10)
        cls.c = chain.Chain(cls.chain_config, cls.genesis_test, os.path.join(config_dir, "protocol_features"), "")
        cls.c.startup(True)
        cls.api = chainapi.ChainApi(cls.c.ptr)

        logger.info(cls.api.get_info())
        logger.info(cls.api.get_account('eosio'))

        self.feature_digests = []
        uuos.set_log_level('default', 0)

    @classmethod
    def teardown_class(cls):
        cls.c.free()

    def setup_method(self, method):
        logger.warning('test start: %s', method.__name__)

    def teardown_method(self, method):
        pass

    def test_hello(self):
        r = self.tester.push_action('eosio.mpy', 'hellompy', b'', {'alice':'active'})
        logger.info(r['action_traces'][0]['console'])

