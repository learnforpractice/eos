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

from uuos.native_object import *

logging.basicConfig(filename='logfile.log', level=logging.INFO,
                    format='%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s')
logger=logging.getLogger(__name__)
logger.addHandler(logging.StreamHandler())

class Connection(object):
    def __init__(self, reader, writer):
        self.reader = reader
        self.writer = writer
        self.handshake_count = 0
        self.closed = False

    async def read(self, length):
        buffer = io.BytesIO()
        while True:
            data = await self.reader.read(length)
            if not data:
                logger.error("+++++++++wait reader return None")
                return None
            buffer.write(data)
            length -= len(data)
            if length <= 0:
                break
        return buffer.getvalue()

    def write(self, data):
        self.writer.write(data)

    def close(self):
        if not self.closed:
            self.writer.close()
            self.closed = True

class UUOSMain(object):

    def __init__(self):
        self.connections = []
        self.tasks = []

    def show_message(self, which, msg):
        msg_type, msg = msg[0], msg[1:]
        print(msg_type, msg)
        if msg_type == 0:#handshake_message_type:
            msg = HandshakeMessage.unpack(msg)
            print(which, msg)
        elif msg_type == 1:#chain_size_message_type:
            msg = ChainSizeMessage.unpack(msg)
            print(which, msg)
        elif msg_type == 2:#go_away_message_type:
            msg = GoAwayMessage.unpack(msg)
            print(which, msg)
        elif msg_type == 3:#time_message_type:
            msg = TimeMessage.unpack(msg)
            print(which, msg)
        elif msg_type == 4:#notice_message_type:
            msg = NoticeMessage.unpack(msg)
            print(which, msg)
        elif msg_type == 5:#request_message_type:
            msg = RequestMessage.unpack(msg)
            print(which, msg)
        elif msg_type == 6:#sync_request_message_type:
            msg = SyncRequestMessage.unpack(msg)
            print(which, 'sync_request_message_type', msg)
        elif msg_type == 7:#signed_block_message_type:
            msg = SignedBlockMessage.unpack(msg)
            print(msg)
            if not msg:
                print(which, 'decode signed block message error!', msg)
                return
            previous = msg.previous
            previous = previous[:8]
            previous = bytes.fromhex(previous)
            previous = int.from_bytes(previous, 'big')
            print(which, previous+1, msg)
        elif msg_type == 8:#packed_transaction_message_type:
            msg = PackedTransactionMessage.unpack(msg)
            print(which, msg)
        else:
            print('unknown message', msg_type)

    async def handle_server_side(self, server, client):
        while True:
            msg_len = await server.read(4)
            if not msg_len:
                server.close()
                client.close()
                return
            client.write(msg_len)
            msg_len = int.from_bytes(msg_len, 'little')
            msg = await server.read(msg_len)
            if not msg:
                server.close()
                client.close()
                return
            client.write(msg)
            self.show_message('server', msg)

    async def handle_p2p_client(self, reader, writer):
        addr = writer.get_extra_info('peername')
        print(f"connection from {addr!r}")
        client = Connection(reader, writer)

        try:
            reader, writer = await asyncio.open_connection('18.139.253.115', 9010)
#            reader, writer = await asyncio.open_connection('127.0.0.1', 9876)
#            reader, writer = await asyncio.open_connection('18.139.253.115', 9011)
            server = Connection(reader, writer)
        except ConnectionRefusedError as e:
            print(e)
            client.close()
            return
        except Exception as e:
            print(type(e), e)
            client.close()
            return

        task = asyncio.create_task(self.handle_server_side(server, client))

        while True:
            msg_len = await client.read(4)
            if not msg_len:
                client.close()
                server.close()
                return
            server.write(msg_len)
            msg_len = int.from_bytes(msg_len, 'little')
            msg = await client.read(msg_len)
            if not msg:
                client.close()
                server.close()
                return
            server.write(msg)
            self.show_message('client', msg)

    async def shutdown(self, signal, loop):
        print('Done running!')
#        self.reader.close()
#        self.writer.close()
        import sys;sys.exit(0)

    async def main(self):
        server = await asyncio.start_server(self.handle_p2p_client, '127.0.0.1', 6000)
        addr = server.sockets[0].getsockname()
        print(f'Serving on {addr}')
        async with server:
            await server.serve_forever()

    def handle_exception(self, loop, context):
        # context["message"] will always be there; but context["exception"] may not
        msg = context.get("exception", context["message"])
        logger.exception(msg)
        logger.error(f"Caught exception: {msg}")

    def run(self, args):
        self.args = args
        logger.info(args)
        self.loop = asyncio.get_event_loop()

        signals = (signal.SIGHUP, signal.SIGTERM, signal.SIGINT)
        for s in signals:
            self.loop.add_signal_handler(
                s, lambda s=s: asyncio.create_task(self.shutdown(s, self.loop)))

        args.loop = self.loop
        self.loop.set_exception_handler(self.handle_exception)
        self.loop.run_until_complete(self.main())

if __name__ == "__main__":
    print(os.getpid())
#    time.sleep(10)
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('--data-dir',               type=str, default='',                  help='data directory')
    parser.add_argument('--config-dir',             type=str, default='',                  help='config directory')
    parser.add_argument('--http-server-address',    type=str, default='127.0.0.1:8888',    help='http server address')
    parser.add_argument('--p2p-listen-endpoint',    type=str, default='127.0.0.1:9876',    help='p2p peer address')
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