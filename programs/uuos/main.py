import os
import gc
import sys
import ujson as json
import struct
import asyncio
import aioconsole
import signal
import shutil

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

genesis_default = {
    "initial_timestamp": "2018-06-08T08:08:08.888",
    "initial_key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
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

        if os.path.exists(config.logconf):
            uuos.initialize_logging(config.logconf)
        else:
            logger.error(f'log config file not foud: {config.logconf}')

        self.producer = None
        self.p2p_manager = P2pManager(config)
        UUOSMain.uuos = self

        chain_cfg = ControllerConfig(default_config)
        # "blocks_dir": "/Users/newworld/dev/uuos2/build/programs/dd/blocks",
        # "state_dir": "/Users/newworld/dev/uuos2/build/programs/dd/state",
        chain_cfg.contracts_console = config.contracts_console
        chain_cfg.blocks_dir = os.path.join(config.data_dir, 'blocks')
        chain_cfg.state_dir = os.path.join(config.data_dir, 'state')

        chain_cfg.wasm_runtime = config.wasm_runtime.replace('-', '_')

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
            genesis = genesis_uuos
        elif self.config.network == 'eos':
            genesis = genesis_eos
        elif self.config.network == 'test':
            genesis = genesis_default
        else:
            raise Exception('unknown network')

        initdb = not os.path.exists(os.path.join(chain_cfg.state_dir, 'shared_memory.bin'))

        # logger.info(chain_cfg)
        genesis = json.dumps(genesis)
        self._chain = Chain(chain_cfg.dumps(), genesis, config.config_dir, config.snapshot)

        self._chain_api = ChainApi(self.chain.ptr)
        self._history_api = HistoryApi()

        self.chain.startup(initdb)

        self.history_api.startup()
        self.producer = Producer(self.config)
        uuos.set_default_log_level(uuos.LogLevel.info)
        uuos.set_log_level('producer_plugin', uuos.LogLevel.info)

        # self.hub = Hub()

    @property
    def chain(self):
        return self._chain

    @property
    def chain_api(self):
        return self._chain_api

    @property
    def history_api(self):
        return self._history_api

    def get_p2p_manager(self):
        return self.p2p_manager

    async def shutdown(self, signal, loop):
        logger.info(f'Shutdown uuos {signal} {self.chain.ptr}')
        if self.chain:
            self.chain.free()

        if self.history_api:
            self.history_api.free()

        if self.producer:
            del self.producer
        self.p2p_manager.close()
        logger.info('Done!')
#        self.reader.close()
#        self.writer.close()
        import sys;sys.exit(0)

    def handle_exception(self, loop, context):
        # context["message"] will always be there; but context["exception"] may not
        msg = context.get("exception", context["message"])
        logger.error(f"Caught exception: {msg}")
        logger.info("Shutting down...")
#        asyncio.create_task(self.shutdown(0, loop))

    async def interactive_console(self):
        await aioconsole.interact()

    @classmethod
    async def main(cls, config, loop):
        try:
            uuos = UUOSMain(config)
        except Exception as e:
            logger.info(f'+++exception:{e}')
            return

        uuos.loop = loop

        signals = (signal.SIGHUP, signal.SIGTERM, signal.SIGINT)
        for s in signals:
            loop.add_signal_handler(
                s, lambda s=s: asyncio.create_task(uuos.shutdown(s, loop)))

        tasks = []

#        task = asyncio.create_task(uuos.analyze_peer())
#        tasks.append(task)

        uuos.producer.main = uuos
        server = rpc_server(uuos.producer, loop, config.http_server_address)
        task = asyncio.create_task(server)
        tasks.append(task)
        
        task = asyncio.create_task(uuos.p2p_manager.connect_to_peers())
        tasks.append(task)

        task = asyncio.create_task(uuos.p2p_manager.p2p_server())
        tasks.append(task)

        if uuos.config.interact:
            task = asyncio.create_task(uuos.interactive_console())
            tasks.append(task)

        # task = asyncio.create_task(uuos.p2p_manager.analyze_peer())
        # tasks.append(task)
        if uuos.config.interact_server:
            host, port = uuos.config.interact_server.split(':')
            port = int(port)
            task = asyncio.create_task(aioconsole.start_interactive_server(host=host, port=port))
        tasks.append(task)

#        self.producer = Producer(self.config)
        task = asyncio.create_task(uuos.producer.run())
        tasks.append(task)
        # register accepted block callback

        loop.set_exception_handler(uuos.handle_exception)

    #    res = await asyncio.gather(connect_to_peers(config), app.server(host=host, port=port), return_exceptions=True)
        res = await asyncio.gather(*tasks, return_exceptions=False)
        print(res)
        return res

    @classmethod
    def finish(cls):
        self = cls.uuos
        if self.chain:
            self.chain.free()

        if self.history_api:
            self.history_api.free()

        if self.producer:
            del self.producer

if __name__ == "__main__":
    print(os.getpid())
#    time.sleep(10)
    config = Config()
    config = config.get_config()
    
    if config.replay_blockchain:
        state_dir = os.path.join(config.data_dir, 'state')
        if os.path.exists(state_dir):
            shutil.rmtree(state_dir)
        reversible_dir = os.path.join(config.data_dir, 'blocks/reversible')
        if os.path.exists(reversible_dir):
            shutil.rmtree(reversible_dir)
        history_dir = os.path.join(config.data_dir, 'history')
        if os.path.exists(history_dir):
            shutil.rmtree(history_dir)
    # signal.signal(signal.SIGHUP, shutting_down)
    # signal.signal(signal.SIGTERM, shutting_down)
    # signal.signal(signal.SIGINT, shutting_down)

    try:
        logger.info(config)
        loop = asyncio.get_event_loop()
        loop.run_until_complete(UUOSMain.main(config, loop))
        # uuos = UUOSMain(config)
        # uuos.run(loop)
        # asyncio.run(main(config))
    except KeyboardInterrupt:
        logger.info("Processing interrupted")
    except Exception as e:
        logger.exception(e)
    except RuntimeError as e:
        logger.exception(e)
    finally:
        logger.info('exiting...')
    UUOSMain.finish()

