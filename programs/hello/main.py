from _hello import *

import os
import sys
import ujson
import struct
import logging
import asyncio
import argparse
from quart import Quart, websocket
from quart.logging import create_logger, create_serving_logger

from hypercorn.asyncio import serve
from hypercorn.config import Config as HyperConfig

from typing import (
    Any,
    AnyStr,
    Awaitable,
    Callable,
    cast,
    Dict,
    IO,
    Iterable,
    List,
    Optional,
    Set,
    Tuple,
    Type,
    Union,
    ValuesView,
)

from pyeoskit import eosapi
from native_object import *

class App(Quart):
    def server(
        self,
        host: str = "127.0.0.1",
        port: int = 5000,
        debug: Optional[bool] = None,
        use_reloader: bool = True,
        loop: Optional[asyncio.AbstractEventLoop] = None,
        ca_certs: Optional[str] = None,
        certfile: Optional[str] = None,
        keyfile: Optional[str] = None,
    ) -> None:

        config = HyperConfig()
        config.access_log_format = "%(h)s %(r)s %(s)s %(b)s %(D)s"
        config.accesslog = create_serving_logger()
        config.bind = [f"{host}:{port}"]
        config.ca_certs = ca_certs
        config.certfile = certfile
        if debug is not None:
            self.debug = debug
        config.errorlog = config.accesslog
        config.keyfile = keyfile
        config.use_reloader = use_reloader

        scheme = "https" if config.ssl_enabled else "http"
        print(  # noqa: T001, T002
            "Running on {}://{} (CTRL + C to quit)".format(scheme, config.bind[0])
        )
        
        return serve(self, config)

        if loop is not None:
            loop.set_debug(debug or False)
            loop.run_until_complete(serve(self, config))
        else:
            asyncio.run(serve(self, config), debug=config.debug)

logging.basicConfig(filename='logfile.log', level=logging.INFO, 
                    format='%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s')
logger=logging.getLogger(__name__)
logger.addHandler(logging.StreamHandler())

app = App(__name__)

@app.route('/')
async def hello():
    return 'hello'

@app.websocket('/ws')
async def ws():
    while True:
        await websocket.send('hello')
handshake = 'b604cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4f122f727370ec82744c546a3c7e17c508fb3f8f6acd76fb78f91b8efaa531c1b60000000000000000000000000000000000000000000000000000000000000000000098709fe00198db150000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000183132372e302e302e313a39383737202d2031323266373237c2160000000016c2769ea3daa77be00b2613af499d01007a1cb01d2b020493526cfb0115c3160000000016c39195db0558624e6b50fa26bc2342c45c063a2d3472a31df6324ae29d036f73781022454f532054657374204167656e74220100'
handshake = bytes.fromhex(handshake)

async def p2p_client(host, port):
    reader, writer = await asyncio.open_connection(host, port)
    logger.info(f'connected to {host}:{port} success!')

    msg = HandshakeMessage(handshake_msg)
    msg.network_version = 1206
    msg.chain_id = 'cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4f'
    msg = msg.pack()
    writer.write(msg)
    try:
        msg_len = await reader.read(4)
        msg_len = int.from_bytes(msg_len, 'little')
        print('++++read:', msg_len)
        msg = await reader.read(msg_len)
        print(msg[0], msg)
        if msg[0] == handshake_message_type:
            msg = HandshakeMessage.unpack(msg[1:])
            print(msg)
    except Exception as e:
        logger.exception(e)
        return

    data = struct.pack('IB', 8+1, sync_request_message_type) + struct.pack('II', 1, 5)
    writer.write(data)
    while True:
        msg_len = await reader.read(4)
        msg_len = int.from_bytes(msg_len, 'little')
        print('++++read:', msg_len)
        msg = await reader.read(msg_len)
        print(msg[0], msg)
        if msg[0] == 3:
            msg = TimeMessage.unpack(msg[1:])
            logger.info(msg)
        elif msg[0] == 4:
            msg = NoticeMessage.unpack(msg[1:])
            print(msg)
        elif msg[0] == 7:
            msg = SignedBlockMessage.unpack(msg[1:])
            logger.info(msg)

async def uuos_main(args):
    for address in args.p2p_peer_address:
        host, port = address.split(':')
        await p2p_client(host, port)
    while True:
        await asyncio.sleep(2.0)
        print('hello')

async def main(args):
    try:
        host, port = args.http_server_address.split(':')
        port = int(port)
    except Exception as e:
        logger.exception(e)
        return
#    res = await asyncio.gather(uuos_main(args), app.server(host=host, port=port), return_exceptions=True)
    res = await asyncio.gather(uuos_main(args), return_exceptions=False)
    print(res)
    return res

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('--data-dir',               type=str, default='',                  help='data directory')
    parser.add_argument('--config-dir',             type=str, default='',                  help='config directory')
    parser.add_argument('--http-server-address',    type=str, default='127.0.0.1:8888',    help='http server address')
    parser.add_argument('--p2p-listen-endpoint',    type=str, default='127.0.0.1:9876',    help='p2p peer address')
    parser.add_argument('--p2p-peer-address',       type=str, action='append', default=[], help='p2p peer address')
    args = parser.parse_args()
#    print(args.data_dir, args.config_dir, args.http_server_address, args.p2p_listen_endpoint)
    print(args.p2p_peer_address)
    asyncio.run(main(args))

