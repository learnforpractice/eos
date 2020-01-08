import tempfile
# tempfile.mkdtemp()
import os
import io
import gc
import time
import sys
import ujson as json
import struct
import logging
import asyncio
import aioconsole
import argparse

from uuos.config import Config, default_config

from uuos.chainapi import ChainApi
from uuos.chain import Chain
from uuos.historyapi import HistoryApi
from uuos.connection import Connection
from uuos.producer import Producer

from uuos.rpcserver import rpc_server
from uuos.nativeobject import ControllerConfig
from uuos import application
from uuos.p2pmanager import P2pManager
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

class Hub():

    def __init__(self):
        self.subscriptions = set()

    def publish(self, message):
        for queue in self.subscriptions:
            queue.put_nowait(message)


class Subscription():

    def __init__(self, hub):
        self.hub = hub
        self.queue = asyncio.Queue()

    def __enter__(self):
        hub.subscriptions.add(self.queue)
        return self.queue

    def __exit__(self, type, value, traceback):
        hub.subscriptions.remove(self.queue)

class UUOSMain(application.Application):

    def __init__(self, config):
        super().__init__()
        application.set_app(self)
        self.config = config
        self.tasks = []
        self.client_count = 0
        self._chain = None
        self._chain_api = None
        self._history_api = None

        self.producer = None
        self.p2p_manager = P2pManager(config)
        UUOSMain.uuos = self

        chain_cfg = ControllerConfig(default_config)
        # "blocks_dir": "/Users/newworld/dev/uuos2/build/programs/dd/blocks",
        # "state_dir": "/Users/newworld/dev/uuos2/build/programs/dd/state",
        chain_cfg.contracts_console = config.contracts_console
        chain_cfg.blocks_dir = os.path.join(config.data_dir, 'blocks')
        chain_cfg.state_dir = os.path.join(config.data_dir, 'state')

        uuos.set_default_data_dir(config.data_dir)
        uuos.set_default_config_dir(config.config_dir)

        if config.snapshot:
            shared_memory_file = os.path.join(chain_cfg.state_dir, 'shared_memory.bin')
            if os.path.exists(shared_memory_file):
                raise Exception("Snapshot can only be used to initialize an empty database.")

        chain_cfg.state_size = config.chain_state_db_size_mb * 1024 * 1024

        print('config.uuos_mainnet', config.uuos_mainnet)
        chain_cfg.uuos_mainnet = False
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

        self._chain_api = ChainApi(self.chain.ptr)
        self._history_api = HistoryApi()
        self.chain.startup()
        self.history_api.startup()
        self.producer = Producer(self.config)