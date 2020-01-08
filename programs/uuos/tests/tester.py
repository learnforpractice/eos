import os
import io
import gc
import time
import sys
import ujson as json
import struct
import asyncio
import shutil
import tempfile
import unittest
from datetime import datetime, timedelta

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos.jsonobject import JsonObject
from uuos.config import Config, default_config

from uuos.chain import Chain

from uuos.nativeobject import ControllerConfig
from uuos import application
import uuos

gc.set_debug(gc.DEBUG_STATS)

logger = application.get_logger(__name__)


genesis_uuos = {
  "initial_timestamp": "2019-10-24T00:00:00.888",
  "initial_key": "EOS7rqzK4qFGTSbgQqr5ynNWKqsZTdJxgKUUELkbFXNcjn4JwUuoS",
  "initial_configuration": {
    "max_block_net_usage": 1048576,
    "target_block_net_usage_pct": 1000,
    "max_transaction_net_usage": 524288,
    "base_per_transaction_net_usage": 12,
    "net_usage_leeway": 500,
    "context_free_discount_net_usage_num": 20,
    "context_free_discount_net_usage_den": 100,
    "max_block_cpu_usage": 200000,
    "target_block_cpu_usage_pct": 1000,
    "max_transaction_cpu_usage": 150000,
    "min_transaction_cpu_usage": 100,
    "max_transaction_lifetime": 3600,
    "deferred_trx_expiration_window": 600,
    "max_transaction_delay": 3888000,
    "max_inline_action_size": 4096,
    "max_inline_action_depth": 4,
    "max_authority_depth": 6
  }
}

genesis_eos = {
    "initial_timestamp": "2018-06-08T08:08:08.888",
    "initial_key": "EOS7EarnUhcyYqmdnPon8rm7mBCTnBoot6o7fE2WzjvEX2TdggbL3",
    "initial_configuration": {
        "max_block_net_usage": 1048576,
        "target_block_net_usage_pct": 1000,
        "max_transaction_net_usage": 524288,
        "base_per_transaction_net_usage": 12,
        "net_usage_leeway": 500,
        "context_free_discount_net_usage_num": 20,
        "context_free_discount_net_usage_den": 100,
        "max_block_cpu_usage": 200000,
        "target_block_cpu_usage_pct": 1000,
        "max_transaction_cpu_usage": 150000,
        "min_transaction_cpu_usage": 100,
        "max_transaction_lifetime": 3600,
        "deferred_trx_expiration_window": 600,
        "max_transaction_delay": 3888000,
        "max_inline_action_size": 4096,
        "max_inline_action_depth": 4,
        "max_authority_depth": 6
    }
}

class Object():
    pass

class ChainTest(object):

    def __init__(self):
        logger.info('+++++++++init+++++++++++')
        config = Object()
        config.data_dir = tempfile.mkdtemp()
        config.config_dir = tempfile.mkdtemp()

        print(config.data_dir, config.config_dir)

        config.chain_state_db_size_mb = 50
        config.contracts_console = True
        config.uuos_mainnet = False
        config.network = 'test'
        config.snapshot = ''

        self._chain = None

        chain_cfg = ControllerConfig(default_config)
        chain_cfg.contracts_console = config.contracts_console
        chain_cfg.blocks_dir = os.path.join(config.data_dir, 'blocks')
        chain_cfg.state_dir = os.path.join(config.data_dir, 'state')
#
        chain_cfg.state_guard_size = 5*1024*1024
        chain_cfg.reversible_cache_size = 50*1024*1024
        chain_cfg.reversible_guard_size = 5*1024*1024


        uuos.set_default_data_dir(config.data_dir)
        uuos.set_default_config_dir(config.config_dir)

        chain_cfg.state_size = config.chain_state_db_size_mb * 1024 * 1024

        print('config.uuos_mainnet', config.uuos_mainnet)
        chain_cfg.uuos_mainnet = False

        self.config = config
        if self.config.network == 'uuos':
            chain_cfg.uuos_mainnet = True
            chain_cfg.genesis = genesis_uuos
        elif self.config.network == 'eos':
            chain_cfg.genesis = genesis_eos
        elif self.config.network == 'test':
            pass
        else:
            raise Exception('unknown network')

        chain_cfg = chain_cfg.dumps()
        logger.info(chain_cfg)
        self._chain = Chain(chain_cfg, config.config_dir, config.snapshot)

        self.init()

    def init(self):
        self.chain.startup()
        uuos.set_accepted_block_callback(self.on_accepted_block)

    @property
    def chain(self):
        return self._chain

    def on_accepted_block(self, block, num, block_id):
        pass

    def start_block(self):
        self.chain.abort_block()
        self.chain.start_block(datetime.utcnow().isoformat())

    def producer_block(self, skip_time):
        if isinstance(skip_time, int):
            skip_time = timedelta(microseconds=skip_time)
        head_time = self.chain.head_block_time()
        next_time = head_time + skip_time
        self.chain.is_building_block()
        return
        if self.chain.is_building_block() or self.chain.pending_block_time() != next_time:
            self.start_block( next_time )

    def test1(self):
        logger.info('++++++++++++++test1+++++++++++++++')
        # datetime.strptime('2020-01-08T12:07:18.669513', "%Y-%m-%dT%H:%M:%S.%f")
        # t = datetime.strptime("2018-06-01T12:00:00.000", "%Y-%m-%dT%H:%M:%S.%f")
        # print('+++datetime:', t)
        # t += timedelta(milliseconds=500)
        # print(os.getpid())
        # # input('>>>')
        # print('++++t:', t.isoformat())
        # print('++++self._chain', self.chain)
        # print('head_block_num:', self.chain.head_block_num())
        # print('++++head_block_time:', self.chain.head_block_time())
        skip_time = timedelta(microseconds=500)
        head_time = self.chain.head_block_time()
        next_time = head_time + skip_time
        producer = self.chain.get_scheduled_producer(next_time)
        print(producer)
        self.start_block()
        # self.producer_block(skip_time)

    def free(self):
        self.chain.free()
        shutil.rmtree(self.config.config_dir)
        shutil.rmtree(self.config.data_dir)

class UUOSTester(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        super(UUOSTester, self).__init__(testName)
        self.extra_args = extra_args
        self.chain = ChainTest()
        UUOSTester.chain = self.chain

    def test1(self):
        self.chain.test1()
        # datetime.datetime.utcnow().isoformat()

    @classmethod
    def setUpClass(cls):
        print('+++setup')

    @staticmethod
    def disconnect():
        pass

    @classmethod
    def tearDownClass(cls):
        print('+++teardown')
        if UUOSTester.chain:
            UUOSTester.chain.free()
            UUOSTester.chain = None

if __name__ == '__main__':
    unittest.main()
