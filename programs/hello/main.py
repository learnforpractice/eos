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

from uuos.rpc_server import rpc_server

from native_object import *

logging.basicConfig(filename='logfile.log', level=logging.INFO,
                    format='%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s')
logger=logging.getLogger(__name__)
logger.addHandler(logging.StreamHandler())

sync_req_span = 1000


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

class Connection(object):
    def __init__(self, reader, writer):
        self.reader = reader
        self.writer = writer
        self.handshake_count = 0

    async def read(self, length):
        buffer = io.BytesIO()
        while True:
            data = await self.reader.read(length)
            if not data:
                return None
            buffer.write(data)
            length -= len(data)
            if length <= 0:
                break
        return buffer.getvalue()

    def write(self, data):
        self.writer.write(data)

class UUOSMain(object):

    def __init__(self):
        self.connections = []
        self.tasks = []

        cfg = ControllerConfig(default_config)
        # "blocks_dir": "/Users/newworld/dev/uuos2/build/programs/dd/blocks",
        # "state_dir": "/Users/newworld/dev/uuos2/build/programs/dd/state",
        cfg.blocks_dir = 'dd/blocks'
        cfg.state_dir = 'dd/state'
        cfg.genesis = genesis_uuos

        cfg = cfg.dumps()
        self.chain_ptr = chain_new(cfg, 'cd')
        chain.chain_ptr = self.chain_ptr
        chain_api.chain_ptr = self.chain_ptr

    async def read(self, length):
        buffer = io.BytesIO()
        while True:
            data = await self.reader.read(length)
            if not data:
                return None
            buffer.write(data)
            length -= len(data)
            if length <= 0:
                break
        return buffer.getvalue()

    def select_connection(self):
        if not self.connections:
            return None
        return self.connections[0]

    def start_sync(self, c):
        start_block = chain.last_irreversible_block_num() + 1
        end_block = start_block + sync_req_span
        if end_block > c.target:
            end_block = c.target
        logger.info(f"+++++start sync {start_block} {end_block}")
        self.sync_msg = SyncRequestMessage(start_block, end_block)
#        data = struct.pack('IB', 8+1, sync_request_message_type) + struct.pack('II', 1, 1000000)
        c.write(self.sync_msg.pack())

    async def handle_message(self, c):
        try:
            msg_len = await c.read(4)
            if not msg_len:
                logger.info('closed connection, exit')
                return
            msg_len = int.from_bytes(msg_len, 'little')
            print('++++read:', msg_len)
            msg = await c.read(msg_len)
    #        print(msg[0], msg)
            msg_type = msg[0]
            if msg_type == 0: #handshake_message_type:
                c.handshake_message = HandshakeMessage.unpack(msg[1:])
                c.target = c.handshake_message.head_num
                logger.info(c.handshake_message)
            elif msg_type == 2: # go_away_message_type
                msg = GoAwayMessage.unpack(msg[1:])
                print(msg)
                c.writer.close()
                return
        except Exception as e:
            logger.exception(e)
            return
        self.start_sync(c)
        count = 0
    #    block_file = open('block.bin', 'wb')
        while True:
            msg_len = await c.read(4)
            if not msg_len:
                logger.info('closed connection, exit')
                return
            msg_len = int.from_bytes(msg_len, 'little')
            if msg_len >=500*1024:
                logger.info(f'bad length: {msg_len}')
                return
            msg = await c.read(msg_len)
            count += 1
            # if count % 100 == 0:
            #     print('+++count:', count)

            # handshake_message_type = 0
            # chain_size_message_type = 1
            # go_away_message_type = 2
            # time_message_type = 3
            # notice_message_type = 4
            # request_message_type = 5
            # sync_request_message_type = 6
            # signed_block_message_type = 7
            # packed_transaction_message_type = 8
            # controller_config_type = 9
            msg_type = msg[0]
            if msg_type == 0: #handshake_message_type
                print(count, msg[0], len(msg), msg.hex())
                logger.info('bad handshake_message')
                c.handshake_message = HandshakeMessage.unpack(msg[1:])
                print(c.handshake_message)
                if c.handshake_message.head_num > chain.fork_db_pending_head_block_num():
                    c.target = c.handshake_message.head_num
                    self.start_sync(c)
            elif msg_type == 1: # chain_size_message_type
                pass
            elif msg_type == 2: # go_away_message_type
                msg = GoAwayMessage.unpack(msg[1:])
                logger.info(msg)
                c.writer.close()
                return
            elif msg_type == 3:
                msg = TimeMessage.unpack(msg[1:])
                xmt = msg.xmt
                xmt = int(xmt)/1e6
                logger.info(msg)
                logger.info(time.localtime(xmt))
            elif msg[0] == 4:
                msg = NoticeMessage.unpack(msg[1:])
                pending = msg.known_blocks['pending']
                if pending > chain.last_irreversible_block_num():
                    c.target = pending
                    self.start_sync(c)
                    #self.send_handshake(c)
                logger.info(f'receive notice message: {msg}')
                # msg = NoticeMessage({
                #     "known_trx":{
                #         "mode":0,
                #         "pending":0,
                #         "ids":[]
                #     },
                #     "known_blocks":{
                #         "mode":1,
                #         "pending":chain.fork_db_pending_head_block_num(),
                #         "ids":[]
                #     }
                # })
                # logger.info(msg)
                # c.send(msg.pack())

            elif msg[0] == 7:
                msg = msg[1:]
                if False:
                    block = SignedBlockMessage.unpack(msg)
                    if not block:
                        continue
                        logger.info('bad block')
                        continue
                    block_num = bytes.fromhex(block.previous[:8])
                    block_num = int.from_bytes(block_num, 'big')
                    if block_num % 1 == 0:
                        print('++++block_num:', block_num)
                # print(block)
                # logger.info(f'++++block num {block_num}')
                # logger.info(block)
                num, block_id = chain_on_incoming_block(self.chain_ptr, msg)
                if num % 10000 == 0:
                    logger.info(f"{num}, {block_id}")
                if c.target - num < 1000:
                    logger.info(f"{num}, {block_id}")                    
                if self.sync_msg.end_block == num:
#                    print(num, self.handshake_message.head_num)
                    if c.target == num:
                        self.send_handshake(c)
                    else:
                        self.start_sync(c)
#                        logger.info(f'send sync message from {start_block} to {end_block}')

    async def connect_to_p2p_client(self, host, port):
        try:
            reader, writer = await asyncio.open_connection(host, port, limit=1024*1024)
            c = Connection(reader, writer)
            c.host = host
            c.port = port
            self.connections.append(c)
            return c
        except OSError as e:
            logger.info(f'Connect to {host}:{port} failed!')
            logger.exception(e)
#            self.p2p_client_task.cancel()
            return
        logger.info(f'connected to {host}:{port} success!')

    async def handle_p2p_client(self, reader, writer):
        addr = writer.get_extra_info('peername')
        print(f"connection from {addr!r}")
        c = Connection(reader, writer)
        while True:
            data = await c.read(100)
            logger.info(data)
            await asyncio.sleep(1.0)
            # writer.write(data)
            # await writer.drain()
        writer.close()

    async def p2p_server(self):
        address, port = self.args.p2p_listen_endpoint.split(':')
        port = int(port)
        server = await asyncio.start_server(self.handle_p2p_client, address, port)
        addr = server.sockets[0].getsockname()
        print(f'Serving on {addr}')
        async with server:
            await server.serve_forever()

    def send_handshake(self, c):
        c.handshake_count += 1
        msg = HandshakeMessage(default_handshake_msg)
        msg.network_version = 1206
        msg.chain_id = chain.id()
        num = chain.last_irreversible_block_num()
        msg.last_irreversible_block_num = num
        msg.last_irreversible_block_id = chain.get_block_id_for_num(num)
        num = chain.fork_db_pending_head_block_num()
        msg.head_num = num
        msg.head_id = chain.get_block_id_for_num(num)
        msg.generation = c.handshake_count
        msg.time = str(int(time.time()*1000000))
#        logger.info(msg)
        msg = msg.pack()
        c.write(msg)

    async def uuos_main(self):
        for address in self.args.p2p_peer_address:
            host, port = address.split(':')
            c = await self.connect_to_p2p_client(host, port)
            if not c:
                continue
            self.send_handshake(c)
#            await self.handle_message()
            task = asyncio.create_task(self.handle_message(c))
            self.tasks.append(task)

    async def shutdown(self, signal, loop):
        if self.chain_ptr:
            chain_free(self.chain_ptr)
            self.chain_ptr = None
        print('Done running!')
#        self.reader.close()
#        self.writer.close()
        import sys;sys.exit(0)

    async def main(self):
        tasks = []
        server = rpc_server(self.chain_ptr, self.loop, self.args.http_server_address)
        task = asyncio.create_task(server)
        tasks.append(task)
        
        task = asyncio.create_task(self.uuos_main())
        tasks.append(task)

        task = asyncio.create_task(self.p2p_server())
        tasks.append(task)

    #    res = await asyncio.gather(uuos_main(args), app.server(host=host, port=port), return_exceptions=True)
        res = await asyncio.gather(*tasks, return_exceptions=False)
        print(res)
        return res

    def run(self, args):
        self.args = args
        logger.info(args)
        self.loop = asyncio.get_event_loop()

        signals = (signal.SIGHUP, signal.SIGTERM, signal.SIGINT)
        for s in signals:
            self.loop.add_signal_handler(
                s, lambda s=s: asyncio.create_task(self.shutdown(s, self.loop)))

        args.loop = self.loop
        self.loop.run_until_complete(self.main())

    def finish(self):
        if self.chain_ptr:
            chain_free(self.chain_ptr)
            self.chain_ptr = None

if __name__ == "__main__":
    print(os.getpid())
#    time.sleep(10)
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('--data-dir',               type=str, default='',                  help='data directory')
    parser.add_argument('--config-dir',             type=str, default='',                  help='config directory')
    parser.add_argument('--http-server-address',    type=str, default='127.0.0.1:8888',    help='http server address')
    parser.add_argument('--p2p-listen-endpoint',    type=str, default='127.0.0.1:6666',    help='p2p peer address')
    parser.add_argument('--p2p-peer-address',       type=str, action='append', default=[], help='p2p peer address')
    args = parser.parse_args()
#    print(args.data_dir, args.config_dir, args.http_server_address, args.p2p_listen_endpoint)
    print(args.p2p_peer_address)

    # signal.signal(signal.SIGHUP, shutting_down)
    # signal.signal(signal.SIGTERM, shutting_down)
    # signal.signal(signal.SIGINT, shutting_down)

    try:
        uuos = UUOSMain()
        uuos.run(args)
        # asyncio.run(main(args))
    except KeyboardInterrupt:
        logger.info("Processing interrupted")
    uuos.finish()
