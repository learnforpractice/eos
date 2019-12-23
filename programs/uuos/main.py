import os
import io
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

from _uuos import set_accepted_block_callback

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

    def __init__(self, args):
        super().__init__()
        self.args = args
        self.connections = []
        self.tasks = []
        self.client_count = 0
        self.chain_ptr = None
        self.producer = None
        UUOSMain.uuos = self

        cfg = ControllerConfig(default_config)
        # "blocks_dir": "/Users/newworld/dev/uuos2/build/programs/dd/blocks",
        # "state_dir": "/Users/newworld/dev/uuos2/build/programs/dd/state",
        cfg.blocks_dir = os.path.join(args.data_dir, 'blocks')
        cfg.state_dir = os.path.join(args.data_dir, 'state')
        if args.snapshot:
            shared_memory_file = os.path.join(cfg.state_dir, 'shared_memory.bin')
            if os.path.exists(shared_memory_file):
                raise Exception("Snapshot can only be used to initialize an empty database.")

        cfg.state_size = args.chain_state_db_size_mb * 1024 * 1024

        print('args.uuos_mainnet', args.uuos_mainnet)
        cfg.uuos_mainnet = False
        if self.args.network == 'uuos':
            cfg.uuos_mainnet = True
            cfg.genesis = genesis_uuos
        elif self.args.network == 'eos':
            cfg.genesis = genesis_eos
        elif self.args.network == 'test':
            pass
        else:
            raise Exception('unknown network')

        cfg = cfg.dumps()
        print(cfg)
        self.chain_ptr = chain_new(cfg, args.config_dir, args.snapshot)
        chain_api.chain_ptr = self.chain_ptr
        chain.set_chain_ptr(self.chain_ptr)
        self.producer = Producer(self.args)
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
        if self.args.max_clients and self.client_count > self.args.max_clients:
            writer.close()
            return
        self.client_count += 1
        addr = writer.get_extra_info('peername')
        print(f"connection from {addr!r}")
        host = writer.get_extra_info('peername')
        logger.info(host)
        c = Connection(host, 0)
        c.reader = reader
        c.writer = writer
        c.send_handshake()
        self.connections.append(c)
        task = asyncio.create_task(self.handle_connection(c))

    async def p2p_server(self):
        address, port = self.args.p2p_listen_endpoint.split(':')
        port = int(port)
        server = await asyncio.start_server(self.handle_p2p_client, address, port)
        addr = server.sockets[0].getsockname()
        print(f'Serving on {addr}')
        async with server:
            await server.serve_forever()

    async def uuos_main(self):
        for address in self.args.p2p_peer_address:
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
    async def main(cls, args, loop):
        try:
            uuos = UUOSMain(args)
        except Exception as e:
            logger.info(f'+++exception:{e}')
            return

        uuos.loop = loop

        signals = (signal.SIGHUP, signal.SIGTERM, signal.SIGINT)
        for s in signals:
            loop.add_signal_handler(
                s, lambda s=s: asyncio.create_task(uuos.shutdown(s, loop)))

        tasks = []
        uuos.producer.main = uuos
        server = rpc_server(uuos.producer, loop, args.http_server_address)
        task = asyncio.create_task(server)
        tasks.append(task)
        
        task = asyncio.create_task(uuos.uuos_main())
        tasks.append(task)

        task = asyncio.create_task(uuos.p2p_server())
        tasks.append(task)

#        self.producer = Producer(self.args)
        task = asyncio.create_task(uuos.producer.run())
        tasks.append(task)
        # register accepted block callback
        set_accepted_block_callback(uuos.on_accepted_block)

#        loop.set_exception_handler(uuos.handle_exception)

    #    res = await asyncio.gather(uuos_main(args), app.server(host=host, port=port), return_exceptions=True)
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

def str2bool(v):
  return v.lower() in ("yes", "true", "t", "1")

if __name__ == "__main__":
    print(os.getpid())
#    time.sleep(10)
    parser = argparse.ArgumentParser(description='')
    parser.register('type','bool',str2bool) # add type keyword to registries
    parser.add_argument('--data-dir',               type=str, default='dd',                  help='data directory')
    parser.add_argument('--config-dir',             type=str, default='cd',                  help='config directory')
    parser.add_argument('--http-server-address',    type=str, default='127.0.0.1:8888',      help='http server address')

    #p2p
    parser.add_argument('--p2p-listen-endpoint',    type=str, default='127.0.0.1:9877',      help='p2p listen endpoint')
    parser.add_argument('--p2p-peer-address',       type=str, default=[], action='append',   help='p2p peer address')
    parser.add_argument('--network',                type=str, default='test',                help='network: uuos, eos, test')
    parser.add_argument('--max-clients',            type=int, default=25,                    help='Maximum number of clients from which connections are accepted, use 0 for no limit')
    parser.add_argument('--peer-private-key',       type=str, default='',                    help='peer private key')
    parser.add_argument('--peer-key',               type=str, default=[], action='append',   help='peer key')
    parser.add_argument('--p2p-max-nodes-per-host',   type=int, default=1,                   help ='Maximum number of client nodes from any single IP address')

    #chain
    parser.add_argument('--hard-replay-blockchain', default=False, action="store_true",      help='clear chain state database, recover as many blocks as possible from the block log, and then replay those blocks')
    parser.add_argument('--replay-blockchain',      default=False, action="store_true",      help='clear chain state database and replay all blocks')
    parser.add_argument('--fix-reversible-blocks',  default=False, action="store_true",      help='recovers reversible block database if that database is in a bad state')
    parser.add_argument('--uuos-mainnet',           type=str2bool, default=True,             help='uuos main network')
    parser.add_argument('--snapshot',               type=str,      default='',               help='File to read Snapshot State from')
    parser.add_argument('--snapshots-dir',          type=str,      default='snapshots',      help='the location of the snapshots directory (absolute path or relative to application data dir)')
    parser.add_argument('--chain-state-db-size-mb', type=int,      default=300,              help='the location of the snapshots directory (absolute path or relative to application data dir)')


    #producer
    parser.add_argument('-p', '--producer-name',    type=str, default=[], action='append',   help='ID of producer controlled by this node (e.g. inita; may specify multiple times)')
    parser.add_argument('-e', '--enable-stale-production',    default=False, action="store_true", help='Enable block production, even if the chain is stale.')


    args = parser.parse_args()
    print('++++peer key:', args.peer_key)
#    print(args.data_dir, args.config_dir, args.http_server_address, args.p2p_listen_endpoint)
    print(args.p2p_peer_address)
    print(args.data_dir)
    print(args.uuos_mainnet)
    if args.replay_blockchain:
        state_dir = os.path.join(args.data_dir, 'state')
        import shutil
        shutil.rmtree(state_dir)
        reversible_dir = os.path.join(args.data_dir, 'blocks/reversible')
        shutil.rmtree(reversible_dir)
    # signal.signal(signal.SIGHUP, shutting_down)
    # signal.signal(signal.SIGTERM, shutting_down)
    # signal.signal(signal.SIGINT, shutting_down)

    try:
        logger.info(args)
        loop = asyncio.get_event_loop()
        loop.run_until_complete(UUOSMain.main(args, loop))
        # uuos = UUOSMain(args)
        # uuos.run(loop)
        # asyncio.run(main(args))
    except KeyboardInterrupt:
        logger.info("Processing interrupted")
    except Exception as e:
        logger.info(e)
    UUOSMain.finish()

