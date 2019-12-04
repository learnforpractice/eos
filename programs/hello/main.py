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

def get_chain_ptr():
    global g_chain_ptr
    return g_chain_ptr

def set_chain_ptr(ptr):
    global g_chain_ptr
    g_chain_ptr = ptr

class Connection(object):
    def __init__(self, reader, writer):
        self.reader = reader
        self.writer = writer
        self.handshake_count = 0
        self.last_handshake = None
        self.target = 0
        self.syncing = False
        self.chain_ptr = get_chain_ptr()

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

    async def read_message(self):
        msg_len = await self.read(4)
        if not msg_len:
            return (None, None)
        msg_len = int.from_bytes(msg_len, 'little')
        if msg_len >=500*1024 or msg_len < 2:
            logger.info(f'bad length: {msg_len}')
            return (None, None)

        msg_type = await self.read(1)
        if not msg_type:
            logger.error('fail to read msg type')
            return (None, None)
        msg_type = msg_type[0]

        msg = await self.read(msg_len-1)
        if not msg:
            logger.error('fail to read msg')
            return (None, None)
        return msg_type, msg

    def write(self, data):
        self.writer.write(data)
    
    def close(self):
        self.writer.close()

    def fork_check(self):
        return False
        lib_num = chain.last_irreversible_block_num()
        msg = self.last_handshake
        if lib_num > msg.last_irreversible_block_num:
            if chain.get_block_id_for_num(lib_num) != msg.last_irreversible_block_id:
                sha256_empty = '0000000000000000000000000000000000000000000000000000000000000000'
                msg = dict(reason=GoAwayReason.forked, node_id=sha256_empty)
                msg = GoAwayMessage(msg)
                logger.info(f'\033[91m++++chain forked, close connection\033[0m')
                self.write(msg.pack())
                self.close()
                return True
        return False

    def notify_last_irr_catch_up(self):
        head_num = chain.fork_db_pending_head_block_num()
        lib_num = chain.last_irreversible_block_num()
        msg = {
            "known_trx":{
                "mode":2, #last_irr_catch_up
                "pending":lib_num,
                "ids":[

                ]
            },
            "known_blocks":{
                "mode":2, #last_irr_catch_up
                "pending":head_num,
                "ids":[

                ]
            }
        }
        msg = NoticeMessage(msg)
        self.writer.write(msg.pack())

    async def send_block_task(self):
        while self.current_block <= self.last_sync_request.end_block:
            block = chain.fetch_block_by_number(self.current_block)
#            len(block) + 1
            msg_len = struct.pack('I', len(block) + 1)
            self.writer.write(msg_len)

            msg_type = int.to_bytes(7, 1, 'little') #signed_block_message_type
            self.writer.write(msg_type)
            self.writer.write(block)
            self.current_block += 1
            await asyncio.sleep(0)

    def start_send_block(self):
        self.current_block = self.last_sync_request.start_block
        self.task = asyncio.create_task(self.send_block_task())
        logger.info('++++start_send_block')

        # self.last_sync_request.start_block
        # self.last_sync_request.end_block

    def send_handshake(self):
        self.handshake_count += 1
        msg = HandshakeMessage(default_handshake_msg)
        msg.network_version = 1206
        msg.chain_id = chain.id()
        num = chain.last_irreversible_block_num()
        msg.last_irreversible_block_num = num
        msg.last_irreversible_block_id = chain.get_block_id_for_num(num)
        num = chain.fork_db_pending_head_block_num()
        msg.head_num = num
        msg.head_id = chain.get_block_id_for_num(num)
        msg.generation = self.handshake_count
        msg.time = str(int(time.time()*1000000))
        logger.info(f'++++send handshake {msg}')
        msg = msg.pack()
        self.write(msg)

    def start_sync(self):
        start_block = chain.last_irreversible_block_num() + 1
        end_block = start_block + sync_req_span
        if end_block > self.target:
            end_block = self.target
        logger.info(f"+++++start sync {start_block} {end_block}")
        self.sync_msg = SyncRequestMessage(start_block, end_block)
#        data = struct.pack('IB', 8+1, sync_request_message_type) + struct.pack('II', 1, 1000000)
        self.write(self.sync_msg.pack())

    async def handle_message(self):
        try:
            msg_type, msg = await self.read_message()
            if msg_type is None or msg is None:
                logger.info('closed connection, exit')
                return
            if msg_type == 0: #handshake_message_type:
                msg = HandshakeMessage.unpack(msg)
                self.target = msg.head_num
                logger.info(f'+++receive handshake {msg}')
                self.last_handshake = msg
                if self.fork_check():
                    return
                if msg.head_num < chain.fork_db_pending_head_block_num():
#                    self.target = msg.head_num
                    self.notify_last_irr_catch_up()
                else: #self.last_handshake.head_num > chain.fork_db_pending_head_block_num():
                    self.target = self.last_handshake.head_num
                    self.start_sync()
            elif msg_type == 2: # go_away_message_type
                msg = GoAwayMessage.unpack(msg)
                print(msg)
                self.writer.close()
                return
            else:
                logger.info('unexpected message: {msg_type}')
                self.writer.close()
                return
        except Exception as e:
            logger.exception(e)
            return
        count = 0
    #    block_file = open('block.bin', 'wb')
        while True:
            msg_type, msg = await self.read_message()
            if msg_type is None or msg is None:
                logger.error('Fail to read msg')
                self.writer.close()
                return
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
            if msg_type == 0: #handshake_message_type
#                logger.info('bad handshake_message')
                self.last_handshake = HandshakeMessage.unpack(msg)
                print(self.last_handshake)
                if self.last_handshake.head_num < chain.fork_db_pending_head_block_num():
#                    self.target = msg.head_num
                    self.notify_last_irr_catch_up()
                else: #self.last_handshake.head_num > chain.fork_db_pending_head_block_num():
                    self.target = self.last_handshake.head_num
                    self.start_sync()
            elif msg_type == 1: # chain_size_message_type
                pass
            elif msg_type == 2: # go_away_message_type
                msg = GoAwayMessage.unpack(msg)
                logger.info(msg)
                self.writer.close()
                return
            elif msg_type == 3:
                msg = TimeMessage.unpack(msg)
                xmt = msg.xmt
                xmt = int(xmt)/1e6
                logger.info(msg)
                logger.info(time.localtime(xmt))
            elif msg_type == 4:
                msg = NoticeMessage.unpack(msg)
                pending = msg.known_blocks['pending']
                if pending > chain.last_irreversible_block_num():
                    self.target = pending
                    self.start_sync()
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
                # self.send(msg.pack())
            elif msg_type == 5:#request_message_type
                msg = RequestMessage.unpack(msg)
                logger.info(msg)
            elif msg_type == 6: #sync_request_message_type
                msg = SyncRequestMessage.unpack(msg)
                print(msg)
                if msg.start_block == 0:
                    continue
                self.last_sync_request = msg
                self.start_send_block()
            elif msg_type == 7:
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
                num, block_id = chain_on_incoming_block(self.chain_ptr, msg)
                if num % 10000 == 0:
                    logger.info(f"{num}, {block_id}")
                if self.target - num < 1000:
                    logger.info(f"{num}, {block_id}")
                if self.sync_msg.end_block == num:
                    if self.target == num:
                        self.send_handshake()
                    else:
                        self.start_sync()
            elif msg_type == 8:
                msg = PackedTransactionMessage(msg)
                logger.info(msg)

class UUOSMain(object):

    def __init__(self, args):
        self.args = args
        self.connections = []
        self.tasks = []
        self.client_count = 0

        cfg = ControllerConfig(default_config)
        # "blocks_dir": "/Users/newworld/dev/uuos2/build/programs/dd/blocks",
        # "state_dir": "/Users/newworld/dev/uuos2/build/programs/dd/state",
        cfg.blocks_dir = 'dd/blocks'
        cfg.state_dir = 'dd/state'
        if self.args.network == 'uuos':
            cfg.genesis = genesis_uuos
        elif self.args.network == 'eos':
            cfg.genesis = genesis_eos
        elif self.args.network == 'test':
            pass
        else:
            raise Exception('unknown network')

        cfg = cfg.dumps()
        self.chain_ptr = chain_new(cfg, 'cd')
        chain.chain_ptr = self.chain_ptr
        chain_api.chain_ptr = self.chain_ptr
        set_chain_ptr(self.chain_ptr)

    def select_connection(self):
        if not self.connections:
            return None
        return self.connections[0]

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
#            logger.exception(e)
#            self.p2p_client_task.cancel()
            return
        logger.info(f'connected to {host}:{port} success!')

    async def handle_connection(self, c):
        try:
            await c.handle_message()
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
        c = Connection(reader, writer)
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
            host, port = address.split(':')
            c = await self.connect_to_p2p_client(host, port)
            if not c:
                continue
            c.send_handshake()
#            await self.handle_message()
            task = asyncio.create_task(c.handle_message())
            self.tasks.append(task)
        logger.info("uuos main task done!")

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

    def run(self):
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
    parser.add_argument('--p2p-listen-endpoint',    type=str, default='127.0.0.1:6666',    help='p2p listen endpoint')
    parser.add_argument('--p2p-peer-address',       type=str, action='append', default=[], help='p2p peer address')
    parser.add_argument('--network',                type=str, default='test',              help='network: uuos, eos, test')
    parser.add_argument('--max-clients',            type=int, default=25,                   help='Maximum number of clients from which connections are accepted, use 0 for no limit')
    args = parser.parse_args()
#    print(args.data_dir, args.config_dir, args.http_server_address, args.p2p_listen_endpoint)
    print(args.p2p_peer_address)

    # signal.signal(signal.SIGHUP, shutting_down)
    # signal.signal(signal.SIGTERM, shutting_down)
    # signal.signal(signal.SIGINT, shutting_down)

    try:
        uuos = UUOSMain(args)
        uuos.run()
        # asyncio.run(main(args))
    except KeyboardInterrupt:
        logger.info("Processing interrupted")
    uuos.finish()
