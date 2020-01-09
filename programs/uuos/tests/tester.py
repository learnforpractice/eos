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

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from datetime import datetime, timedelta
from uuos.jsonobject import JsonObject
from uuos.nativeobject import SignedBlockMessage, PackedTransactionMessage

from uuos import config

from uuos.chain import Chain

from uuos.nativeobject import ControllerConfig
from uuos import application
import uuos

gc.set_debug(gc.DEBUG_STATS)

logger = application.get_logger(__name__)

def isoformat(dt):
    return dt.isoformat(timespec='milliseconds')

#logger = LogWrapper(logger)

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
        options = Object()
        options.data_dir = tempfile.mkdtemp()
        options.config_dir = tempfile.mkdtemp()

        print(options.data_dir, options.config_dir)

        options.chain_state_db_size_mb = 50
        options.contracts_console = True
        options.uuos_mainnet = False
        options.network = 'test'
        options.snapshot = ''

        self._chain = None

        chain_cfg = ControllerConfig(config.default_config)
        chain_cfg.contracts_console = options.contracts_console
        chain_cfg.blocks_dir = os.path.join(options.data_dir, 'blocks')
        chain_cfg.state_dir = os.path.join(options.data_dir, 'state')
#
        chain_cfg.state_guard_size = 5*1024*1024
        chain_cfg.reversible_cache_size = 50*1024*1024
        chain_cfg.reversible_guard_size = 5*1024*1024


        uuos.set_default_data_dir(options.data_dir)
        uuos.set_default_config_dir(options.config_dir)

        chain_cfg.state_size = options.chain_state_db_size_mb * 1024 * 1024

        print('config.uuos_mainnet', options.uuos_mainnet)
        chain_cfg.uuos_mainnet = False

        self.options = options
        if self.options.network == 'uuos':
            chain_cfg.uuos_mainnet = True
            chain_cfg.genesis = genesis_uuos
        elif self.options.network == 'eos':
            chain_cfg.genesis = genesis_eos
        elif self.options.network == 'test':
            pass
        else:
            raise Exception('unknown network')

        chain_cfg = chain_cfg.dumps()
        logger.info(chain_cfg)
        self._chain = Chain(chain_cfg, options.config_dir, options.snapshot)

        self.init()

    def init(self):
        self.chain.startup()
        uuos.set_accepted_block_callback(self.on_accepted_block)

    @property
    def chain(self):
        return self._chain

    def on_accepted_block(self, block, num, block_id):
        msg = SignedBlockMessage.unpack(block)
        print(msg)

    def calc_pending_block_time(self):
#        self.chain.abort_block()
        now = datetime.utcnow()
        base = self.chain.head_block_time()
        print(isoformat(base), isoformat(now))
        if base < now:
            base = now
        min_time_to_next_block = config.block_interval_us - int(base.timestamp()*1e6) % config.block_interval_us
        print('min_time_to_next_block:', min_time_to_next_block)
        logger.info(f"+++++++++base: {isoformat(base)}")
        block_time = base + timedelta(microseconds=min_time_to_next_block)
        logger.info(f"+++block_time: {isoformat(block_time)}")
        if block_time - now < timedelta(microseconds=config.block_interval_us/10):
            block_time += timedelta(microseconds=config.block_interval_us)        
        logger.info(f"+++block_time: {isoformat(block_time)}")
        return block_time

    #   permission_level
    #   account_name    actor;
    #   permission_name permission;

    #   account_name               account;
    #   action_name                name;
    #   vector<permission_level>   authorization;
    #   bytes                      data;


    def start_block(self):
        for i in range(10):
            self.chain.abort_block()
            self.chain.start_block(isoformat(self.calc_pending_block_time()))
            trx = self.gen_trx()
            deadline = datetime.utcnow() + timedelta(microseconds=30000)
            billed_cpu_time_us = 2000
            ret = self.chain.push_transaction(trx, isoformat(deadline), billed_cpu_time_us)
            print(ret)
            trxs = self.chain.get_unapplied_transactions()
            print(trxs)
            self.chain.finalize_block('5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3')
            self.chain.commit_block()
            print('head_block_num:', self.chain.head_block_num())

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

    def test2(self):
        self.calc_pending_block_time()

    def gen_trx(self):
        chain_id = self.chain.id()
        ref_block_id = self.chain.last_irreversible_block_id()
        priv_key = '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3'
        actions = []
        action = {
            'account':'eosio',
            'name':'sayhello',
            'data':'aabbcc',
            'authorization':[{'actor':'eosio', 'permission':'active'}]
        }
        actions.append(action)
        actions = json.dumps(actions)
        expiration = datetime.utcnow() + timedelta(seconds=60)
        expiration = isoformat(expiration)
        r = self.chain.gen_transaction(actions, expiration, ref_block_id, chain_id, False, priv_key)
        print(r)
        return r

        print(r)
        r = PackedTransactionMessage.unpack(r)
        print(r)

    def free(self):
        self.chain.free()
        shutil.rmtree(self.options.config_dir)
        shutil.rmtree(self.options.data_dir)

class UUOSTester(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        super(UUOSTester, self).__init__(testName)
        self.extra_args = extra_args
        self.chain = ChainTest()
        UUOSTester.chain = self.chain

    def test1(self):
        self.chain.test1()
        # datetime.datetime.utcnow().isoformat()

    def test2(self):
        self.chain.test2()

    def test3(self):
        self.chain.test3()

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
