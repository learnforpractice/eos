import os
import io
import gc
import time
import sys
import ujson as json
import struct
import logging
import asyncio
import argparse
import signal

from uuos import chain, chain_api

from uuos.connection import Connection
from uuos.producer import Producer

from uuos.rpc_server import rpc_server
from uuos.native_object import *
from uuos import application
from uuos.config import Config

from _uuos import set_accepted_block_callback

gc.set_debug(gc.DEBUG_STATS)

logging.basicConfig(filename='logfile.log', level=logging.INFO,
                    format='%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s')
logger=logging.getLogger(__name__)
logger.addHandler(logging.StreamHandler())

handshake = 'b604cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4f122f727370ec82744c546a3c7e17c508fb3f8f6acd76fb78f91b8efaa531c1b60000000000000000000000000000000000000000000000000000000000000000000098709fe00198db150000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000183132372e302e302e313a39383737202d2031323266373237c2160000000016c2769ea3daa77be00b2613af499d01007a1cb01d2b020493526cfb0115c3160000000016c39195db0558624e6b50fa26bc2342c45c063a2d3472a31df6324ae29d036f73781022454f532054657374204167656e74220100'
handshake = bytes.fromhex(handshake)

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
        self.config = config
        self.connections = []
        self.tasks = []
        self.client_count = 0
        self.chain_ptr = None
        self.producer = None
        UUOSMain.uuos = self

        cfg = ControllerConfig(default_config)
        # "blocks_dir": "/Users/newworld/dev/uuos2/build/programs/dd/blocks",
        # "state_dir": "/Users/newworld/dev/uuos2/build/programs/dd/state",
        cfg.blocks_dir = os.path.join(config.data_dir, 'blocks')
        cfg.state_dir = os.path.join(config.data_dir, 'state')
        if config.snapshot:
            shared_memory_file = os.path.join(cfg.state_dir, 'shared_memory.bin')
            if os.path.exists(shared_memory_file):
                raise Exception("Snapshot can only be used to initialize an empty database.")

        cfg.state_size = config.chain_state_db_size_mb * 1024 * 1024

        print('config.uuos_mainnet', config.uuos_mainnet)
        cfg.uuos_mainnet = False
        if self.config.network == 'uuos':
            cfg.uuos_mainnet = True
            cfg.genesis = genesis_uuos
        elif self.config.network == 'eos':
            cfg.genesis = genesis_eos
        elif self.config.network == 'test':
            pass
        else:
            raise Exception('unknown network')

        cfg = cfg.dumps()
        print(cfg)
        self.chain_ptr = chain_new(cfg, config.config_dir, config.snapshot)
        chain_api.chain_ptr = self.chain_ptr
        chain.set_chain_ptr(self.chain_ptr)
        self.producer = Producer(self.config)
        # self.hub = Hub()

        application.set_app(self)

    # async def handle_transaction(self):
    #     with Subscription(hub) as queue:
    #         while True:
    #             msg = await queue.get()
    #             for c in self.connections:
    #                 c.send_transaction(msg)

    def select_connection(self):
        if not self.connections:
            return None
        return self.connections[0]

    async def handle_connection(self, c):
        try:
            await c.handle_message_loop()
        except Exception as e:
            logger.exception(e)
        finally:
            self.client_count -= 1

    async def handle_p2p_client(self, reader, writer):
        if self.config.max_clients and self.client_count > self.config.max_clients:
            writer.close()
            return
        self.client_count += 1
        addr = writer.get_extra_info('peername')
        print(f"connection from {addr!r}")
        print(f"connection from {addr}")

        host = writer.get_extra_info('peername')
        logger.info(f'++++host:{host}')
        sock = writer.get_extra_info('socket')
        if sock is not None:
            logger.info(f'++++++++++sock.getsockname: {sock.getsockname()}')
        c = Connection(*host)
        c.reader = reader
        c.writer = writer
        c.send_handshake()
        self.connections.append(c)
        task = asyncio.create_task(self.handle_connection(c))

    async def p2p_server(self):
        address, port = self.config.p2p_listen_endpoint.split(':')
        port = int(port)
        server = await asyncio.start_server(self.handle_p2p_client, address, port)
        addr = server.sockets[0].getsockname()
        print(f'Serving on {addr}')
        async with server:
            await server.serve_forever()

    async def analyze_peer(self):
        peers = '''
        p2p-peer-address = api-full1.eoseoul.io:9876
        p2p-peer-address = api-full2.eoseoul.io:9876
        p2p-peer-address = boot.eostitan.com:9876
        p2p-peer-address = bp.cryptolions.io:9876
        p2p-peer-address = bp.eosbeijing.one:8080
        p2p-peer-address = bp.libertyblock.io:9800
        p2p-peer-address = br.eosrio.io:9876
        p2p-peer-address = eos-seed-de.privex.io:9876
        p2p-peer-address = fullnode.eoslaomao.com:443
        p2p-peer-address = mainnet.eoscalgary.io:5222
        p2p-peer-address = node.eosflare.io:1883
        p2p-peer-address = node1.eoscannon.io:59876
        p2p-peer-address = node1.eosnewyork.io:6987
        p2p-peer-address = p2p.eosdetroit.io:3018
        p2p-peer-address = p2p.genereos.io:9876
        p2p-peer-address = p2p.meet.one:9876
        p2p-peer-address = peer.eosn.io:9876
        p2p-peer-address = peer.main.alohaeos.com:9876
        p2p-peer-address = peer1.mainnet.helloeos.com.cn:80
        p2p-peer-address = peer2.mainnet.helloeos.com.cn:80
        p2p-peer-address = peering.mainnet.eoscanada.com:9876
        p2p-peer-address = peering1.mainnet.eosasia.one:80
        p2p-peer-address = peering2.mainnet.eosasia.one:80
        p2p-peer-address = publicnode.cypherglass.com:9876
        p2p-peer-address = seed1.greymass.com:9876
        p2p-peer-address = seed2.greymass.com:9876
        '''
        peers = '''
        p2p-peer-address = peer.main.alohaeos.com:9876
        p2p-peer-address = peer.eos-mainnet.eosblocksmith.io:5010
        p2p-peer-address = eos-p2p.slowmist.io:9876
        p2p-peer-address = peer1.swisseos.com:9876
        p2p-peer-address = mainnet.get-scatter.com:9876
        '''
        peers = peers.split('\n')
        self.delays = []
        import time
        async def check_time(self, host, port):
            try:
                start = time.time()
                c = Connection(host, port)
                ret = await c.connect()
                if not ret:
                    return
                c.send_handshake()
                ret = await c.handle_message()
                c.close()
                if not ret:
                    return
                self.delays.append((time.time() - start, host, port))
                self.delays.sort(key = lambda x: x[0])
                print(self.delays)
            except Exception as e:
                logger.exception(e)
        tasks = []
        for peer in peers:
            peer = peer.split(' = ')
            if len(peer) >= 2:
                peer = peer[1]
                host, port = peer.split(':')
                print(host, port)
                task = asyncio.create_task(check_time(self, host, port))
                tasks.append(task)
        for task in tasks:
            await task
        print(self.delays)

    async def connect_to_peers(self):
        for address in self.config.p2p_peer_address:
            try:
                print(address)
                host, port = address.split(':')
                c = Connection(host, port)
                ret = await c.connect()
                if not ret:
                    continue
                self.connections.append(c)
                print('send hashshake message to ', address)
                await c.start()
            except ConnectionResetError as e:
                print(e)
            except Exception as e:
                print(e)
#                logger.exception(e)
        logger.info("uuos main task done!")

    async def shutdown(self, signal, loop):
        logger.info(f'Shutdown uuos {signal} {self.chain_ptr}')
        if self.chain_ptr:
            chain_free(self.chain_ptr)
            self.chain_ptr = None
        
        if self.producer:
            del self.producer
        for c in self.connections:
            logger.info(f'close {c.host}')
            c.close()
        logger.info('Done!')
#        self.reader.close()
#        self.writer.close()
        import sys;sys.exit(0)

    def on_accepted_block(self, block, num, block_id):
        for c in self.connections:
            if not c.last_handshake:
                continue
            if c.catch_up:
#                print('+++block:',block)
                c.send_block(block)

    def handle_exception(self, loop, context):
        # context["message"] will always be there; but context["exception"] may not
        msg = context.get("exception", context["message"])
        logging.error(f"Caught exception: {msg}")
        logging.info("Shutting down...")
        asyncio.create_task(self.shutdown(0, loop))

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
        
        task = asyncio.create_task(uuos.connect_to_peers())
        tasks.append(task)

        task = asyncio.create_task(uuos.p2p_server())
        tasks.append(task)

#        self.producer = Producer(self.config)
        task = asyncio.create_task(uuos.producer.run())
        tasks.append(task)
        # register accepted block callback
        set_accepted_block_callback(uuos.on_accepted_block)

#        loop.set_exception_handler(uuos.handle_exception)

    #    res = await asyncio.gather(connect_to_peers(config), app.server(host=host, port=port), return_exceptions=True)
        res = await asyncio.gather(*tasks, return_exceptions=False)
        print(res)
        return res

    @classmethod
    def finish(cls):
        self = cls.uuos
        if self.chain_ptr:
            chain_free(self.chain_ptr)
            self.chain_ptr = None

        if self.producer:
            del self.producer

if __name__ == "__main__":
    print(os.getpid())
#    time.sleep(10)
    config = Config()
    config = config.get_config()
    
    if config.replay_blockchain:
        state_dir = os.path.join(config.data_dir, 'state')
        import shutil
        shutil.rmtree(state_dir)
        reversible_dir = os.path.join(config.data_dir, 'blocks/reversible')
        shutil.rmtree(reversible_dir)
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
    UUOSMain.finish()

