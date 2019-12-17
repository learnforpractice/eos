import os
import io
import time
import sys
import ujson as json
import struct
import logging
import asyncio
import argparse
from quart import Quart, websocket, request
from quart.logging import create_logger, create_serving_logger

from hypercorn.asyncio import serve
from hypercorn.config import Config as HyperConfig
from pyeoskit import eosapi

from . import chain_api

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
logger.addHandler(logging.StreamHandler())
app = App(__name__)

@app.route('/')
async def hello():
    return 'hello'

@app.route('/v1/chain/get_info', methods=["GET", "POST"])
async def get_info():
    return chain_api.get_info()

@app.route('/v1/chain/get_activated_protocol_features', methods=["POST"])
async def get_activated_protocol_features():
    data = await request.data
    return chain_api.get_activated_protocol_features(data.decode('utf8'))

@app.route('/v1/chain/get_block', methods=["POST"])
async def get_block():
    data = await request.data
    return chain_api.get_block(data.decode('utf8'))

@app.route('/v1/chain/get_block_header_state', methods=["POST"])
async def get_block_header_state():
    data = await request.data
    return chain_api.get_block_header_state(data.decode('utf8'))

@app.route('/v1/chain/get_account', methods=["POST"])
async def get_account():
    data = await request.data
    return chain_api.get_account(data.decode('utf8'))

@app.route('/v1/chain/get_code', methods=["POST"])
async def get_code():
    data = await request.data
    result = chain_api.get_code(data.decode('utf8'))
    return result

@app.route('/v1/chain/get_code_hash', methods=["POST"])
async def get_code_hash():
    data = await request.data
    result = chain_api.get_code_hash(data.decode('utf8'))
    return result

@app.route('/v1/chain/get_abi', methods=["POST"])
async def get_abi():
    data = await request.data
    result = chain_api.get_abi(data.decode('utf8'))
    return result

@app.route('/v1/chain/get_raw_code_and_abi', methods=["POST"])
async def get_raw_code_and_abi():
    data = await request.data
    result = chain_api.get_raw_code_and_abi(data.decode('utf8'))
    return result

@app.route('/v1/chain/get_raw_abi', methods=["POST"])
async def get_raw_abi():
    data = await request.data
    result = chain_api.get_raw_abi(data.decode('utf8'))
    return result

@app.route('/v1/chain/get_table_rows', methods=["POST"])
async def get_table_rows():
    data = await request.data
    result = chain_api.get_table_rows(data.decode('utf8'))
    return result

@app.route('/v1/chain/get_table_by_scope', methods=["POST"])
async def get_table_by_scope():
    data = await request.data
    result = chain_api.get_table_by_scope(data.decode('utf8'))
    return result

@app.route('/v1/chain/get_currency_balance', methods=["POST"])
async def get_currency_balance():
    data = await request.data
    result = chain_api.get_currency_balance(data.decode('utf8'))
    return result

@app.route('/v1/chain/get_currency_stats', methods=["POST"])
async def get_currency_stats():
    data = await request.data
    result = chain_api.get_currency_stats(data.decode('utf8'))
    return result

@app.route('/v1/chain/get_producers', methods=["POST"])
async def get_producers():
    data = await request.data
    result = chain_api.get_producers(data.decode('utf8'))
    return result

@app.route('/v1/chain/get_producer_schedule', methods=["POST"])
async def get_producer_schedule():
    data = await request.data
    result = chain_api.get_producer_schedule(data.decode('utf8'))
    return result

@app.route('/v1/chain/get_scheduled_transactions', methods=["POST"])
async def get_scheduled_transactions():
    data = await request.data
    result = chain_api.get_scheduled_transactions(data.decode('utf8'))
    return result

@app.route('/v1/chain/abi_json_to_bin', methods=["POST"])
async def abi_json_to_bin():
    data = await request.data
    result = chain_api.abi_json_to_bin(data.decode('utf8'))
    return result

@app.route('/v1/chain/abi_bin_to_json', methods=["POST"])
async def abi_bin_to_json():
    data = await request.data
    result = chain_api.abi_bin_to_json(data.decode('utf8'))
    return result

@app.route('/v1/chain/get_required_keys', methods=["POST"])
async def get_required_keys():
    data = await request.data
    result = chain_api.get_required_keys(data.decode('utf8'))
    return result

@app.route('/v1/chain/get_transaction_id', methods=["POST"])
async def get_transaction_id():
    data = await request.data
    result = chain_api.get_transaction_id(data.decode('utf8'))
    return result

@app.route('/v1/chain/push_transaction', methods=["POST"])
async def push_transaction():
    data = await request.data
    app.producer.start_block()
    result, raw_packed_trx = app.producer.process_incomming_transaction(data.decode('utf8'))
    for c in app.producer.main.connections:
        c.send_transaction(raw_packed_trx)
    return result

@app.websocket('/ws')
async def ws():
    while True:
        await websocket.send('hello')

async def rpc_server(producer, loop, http_server_address):
    app.producer = producer
    try:
        host, port = http_server_address.split(':')
        await app.server(host=host, port=port, loop=loop, use_reloader=False)
    except Exception as e:
        logger.exception(e)
        return