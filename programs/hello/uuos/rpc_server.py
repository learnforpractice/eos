import os
import io
import time
import sys
#import ujson as json
import json
import struct
import logging
import asyncio
import argparse
from quart import Quart, websocket, request
from quart.logging import create_logger, create_serving_logger

from hypercorn.asyncio import serve
from hypercorn.config import Config as HyperConfig
from pyeoskit import eosapi

from . import chain

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

from quart.logging import default_handler

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

logger=logging.getLogger(__name__)
#logger.addHandler(logging.StreamHandler())
logger.setLevel(logging.ERROR)

app = App(__name__)
app.logger.removeHandler(default_handler)

@app.route('/')
async def hello():
    return 'hello'

@app.route('/v1/chain/get_info', methods=["GET", "POST"])
async def get_info():
    return chain.get_info()

@app.route('/v1/chain/get_account', methods=["POST"])
async def get_account():
    data = await request.data
    print(data)
    return chain.get_account(data.decode('utf8'))

@app.websocket('/ws')
async def ws():
    while True:
        await websocket.send('hello')


async def rpc_server(chain_ptr, loop, http_server_address):
    try:
        host, port = http_server_address.split(':')
        await app.server(host=host, port=port, loop=loop, use_reloader=False)
    except Exception as e:
        logger.exception(e)
        return
