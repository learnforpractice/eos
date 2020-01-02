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
from . import application
from . import producer
from .application import get_app

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
        config.errorlog = config.accesslog
        config.bind = [f"{host}:{port}"]
        config.ca_certs = ca_certs
        config.certfile = certfile
        if debug is not None:
            self.debug = debug
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

logger = application.get_logger(__name__)

app = App(__name__)

internal_server_error = 500
http_return_code = (500, 200)

@app.route('/')
async def hello():
    return 'hello'

async def chain_get_info():
    ret = chain_api.get_info()
    return ret[1], http_return_code[ret[0]]

async def chain_get_activated_protocol_features():
    data = await request.data
    ret = chain_api.get_activated_protocol_features(data)
    return ret[1], http_return_code[ret[0]]
    
async def chain_get_block():
    data = await request.data
    ret = chain_api.get_block(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_block_header_state():
    data = await request.data
    ret = chain_api.get_block_header_state(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_account():
    data = await request.data
    ret = chain_api.get_account(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_code():
    data = await request.data
    ret = chain_api.get_code(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_code_hash():
    data = await request.data
    ret = chain_api.get_code_hash(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_abi():
    data = await request.data
    ret = chain_api.get_abi(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_raw_code_and_abi():
    data = await request.data
    ret = chain_api.get_raw_code_and_abi(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_raw_abi():
    data = await request.data
    ret = chain_api.get_raw_abi(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_table_rows():
    data = await request.data
    ret = chain_api.get_table_rows(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_table_by_scope():
    data = await request.data
    ret = chain_api.get_table_by_scope(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_currency_balance():
    data = await request.data
    ret = chain_api.get_currency_balance(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_currency_stats():
    data = await request.data
    ret = chain_api.get_currency_stats(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_producers():
    data = await request.data
    ret = chain_api.get_producers(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_producer_schedule():
    data = await request.data
    ret = chain_api.get_producer_schedule(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_scheduled_transactions():
    data = await request.data
    ret = chain_api.get_scheduled_transactions(data)
    return ret[1], http_return_code[ret[0]]

async def chain_abi_json_to_bin():
    data = await request.data
    ret = chain_api.abi_json_to_bin(data)
    return ret[1], http_return_code[ret[0]]

async def chain_abi_bin_to_json():
    data = await request.data
    ret = chain_api.abi_bin_to_json(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_required_keys():
    data = await request.data
    ret = chain_api.get_required_keys(data)
    return ret[1], http_return_code[ret[0]]

async def chain_get_transaction_id():
    data = await request.data
    ret = chain_api.get_transaction_id(data)
    return ret[1], http_return_code[ret[0]]

async def chain_push_transaction():
    data = await request.data
    msg = producer.TransactionMessage(data)
    result = await msg.wait()
    return result


#---------------history api----------------
async def history_get_actions():
    data = await request.data

async def history_get_transaction():
    data = await request.data

async def history_get_key_accounts():
    data = await request.data

async def history_get_controlled_accounts():
    data = await request.data

#------------------------net api---------------------
async def net_connect():
    data = await request.data

async def net_disconnect():
    data = await request.data

async def net_connections():
    data = await request.data

async def net_status():
    data = await request.data
    ret = get_app().get_p2p_manager().get_net_status(json.loads(data))
    if not ret:
        return 'null'
    return ret
#------------producer api ---------------
async def producer_pause():
    application.get_app().producer.pause()
    return 'true'

async def producer_resume():
    application.get_app().producer.resume()
    return 'true'

async def producer_paused():
    ret = application.get_app().producer.paused()
    logger.info(f'+++++++++++++++producer_paused: {ret}')
    if ret:
        return 'true'
    else:
        return 'false'

async def producer_get_runtime_options():
    return application.get_app().producer.get_runtime_options(json=False)

async def producer_update_runtime_options():
    data = await request.data
    application.get_app().producer.update_runtime_options(data)
    return 'true'

async def producer_get_greylist():
    pass

async def producer_add_greylist_accounts():
    pass

async def producer_remove_greylist_accounts():
    pass

async def producer_get_whitelist_blacklist():
    pass

async def producer_set_whitelist_blacklist():
    pass

async def producer_create_snapshot():
    logger.info('+++++producer_create_snapshot')
    ret = application.get_app().producer.create_snapshot()
    logger.info(ret)
    return ret[1], http_return_code[ret[0]]

async def producer_get_integrity_hash():
    chain = get_app().chain
    if chain.is_building_block():
        chain.abort_block()
    head_block_id = chain.head_block_id()
    integrity_hash = chain.calculate_integrity_hash()
    body = dict(head_block_id=head_block_id,
                integrity_hash=integrity_hash,
    )
    return json.dumps(body)

async def producer_get_scheduled_protocol_feature_activations():
    return get_app().producer.get_scheduled_protocol_feature_activations()

async def producer_schedule_protocol_feature_activations():
    features = await request.data
    ret = get_app().producer.schedule_protocol_feature_activations(features)
    return ret[1], http_return_code[ret[0]]

async def producer_get_supported_protocol_features():
    params = await request.data
    ret = get_app().producer.get_supported_protocol_features(params)
    return ret

#----------------db size api-------------------
async def db_get_size():
    return chain_api.db_size_api_get()

@app.websocket('/ws')
async def ws():
    while True:
        await websocket.send('hello')

post_method = ["POST"]
get_post_method = ["GET", "POST"]
async def rpc_server(producer, loop, http_server_address):
    log = logging.getLogger('werkzeug')
    log.setLevel(logging.ERROR)

    chain_api_routes = [
        ('/v1/chain/get_info',                          get_post_method, chain_get_info),
        ('/v1/chain/get_activated_protocol_features',   post_method, chain_get_activated_protocol_features),
        ('/v1/chain/get_block',                         post_method, chain_get_block),
        ('/v1/chain/get_block_header_state',            post_method, chain_get_block_header_state),
        ('/v1/chain/get_account',                       post_method, chain_get_account),
        ('/v1/chain/get_code',                          post_method, chain_get_code),
        ('/v1/chain/get_code_hash',                     post_method, chain_get_code_hash),
        ('/v1/chain/get_abi',                           post_method, chain_get_abi),
        ('/v1/chain/get_raw_code_and_abi',              post_method, chain_get_raw_code_and_abi),
        ('/v1/chain/get_raw_abi',                       post_method, chain_get_raw_abi),
        ('/v1/chain/get_table_rows',                    post_method, chain_get_table_rows),
        ('/v1/chain/get_table_by_scope',                post_method, chain_get_table_by_scope),
        ('/v1/chain/get_currency_balance',              post_method, chain_get_currency_balance),
        ('/v1/chain/get_currency_stats',                post_method, chain_get_currency_stats),
        ('/v1/chain/get_producers',                     post_method, chain_get_producers),
        ('/v1/chain/get_producer_schedule',             post_method, chain_get_producer_schedule),
        ('/v1/chain/get_scheduled_transactions',        post_method, chain_get_scheduled_transactions),
        ('/v1/chain/abi_json_to_bin',                   post_method, chain_abi_json_to_bin),
        ('/v1/chain/abi_bin_to_json',                   post_method, chain_abi_bin_to_json),
        ('/v1/chain/get_required_keys',                 post_method, chain_get_required_keys),
        ('/v1/chain/get_transaction_id',                post_method, chain_get_transaction_id),
        ('/v1/chain/push_transaction',                  post_method, chain_push_transaction),
    ]

    history_api_routes = [
        ('/v1/history/get_actions',                     post_method, history_get_actions),
        ('/v1/history/get_transaction',                 post_method, history_get_transaction),
        ('/v1/history/get_key_accounts',                post_method, history_get_key_accounts),
        ('/v1/history/get_controlled_accounts',         post_method, history_get_controlled_accounts),
    ]

    net_api_routes = [
        ('/v1/net/connect',                             post_method, net_connect),
        ('/v1/net/disconnect',                          post_method, net_disconnect),
        ('/v1/net/connections',                         post_method, net_connections),
        ('/v1/net/status',                              post_method, net_status),
    ]
    
    producer_api_routes = [
        ("/v1/producer/pause",                          get_post_method, producer_pause),
        ("/v1/producer/resume",                         get_post_method, producer_resume),
        ("/v1/producer/paused",                         get_post_method, producer_paused),
        ("/v1/producer/get_runtime_options",            get_post_method, producer_get_runtime_options),
        ("/v1/producer/update_runtime_options",         post_method, producer_update_runtime_options),
        ("/v1/producer/get_greylist",                   get_post_method, producer_get_greylist),
        ("/v1/producer/add_greylist_accounts",          post_method, producer_add_greylist_accounts),
        ("/v1/producer/remove_greylist_accounts",       post_method, producer_remove_greylist_accounts),
        ("/v1/producer/get_whitelist_blacklist",        get_post_method, producer_get_whitelist_blacklist),
        ("/v1/producer/set_whitelist_blacklist",        post_method, producer_set_whitelist_blacklist),
        ("/v1/producer/create_snapshot",                post_method, producer_create_snapshot),
        ("/v1/producer/get_integrity_hash",             get_post_method, producer_get_integrity_hash),
        ("/v1/producer/get_scheduled_protocol_feature_activations",  get_post_method, producer_get_scheduled_protocol_feature_activations),
        ("/v1/producer/schedule_protocol_feature_activations",  post_method, producer_schedule_protocol_feature_activations),
        ("/v1/producer/get_supported_protocol_features",        post_method, producer_get_supported_protocol_features),
    ]

    db_size_api_routers = [
        ("/v1/db_size/get",                        get_post_method, db_get_size),
    ]

    if 'eosio::chain_api_plugin' in producer.config.plugin:
        for route, method, view_func in chain_api_routes:
            logger.info(f'route {route}')
            app.route(route, methods=method)(view_func)

    if 'eosio::history_api_plugin' in producer.config.plugin:
        for route, method, view_func in history_api_routes:
            logger.info(f'route {route}')
            app.route(route, methods=method)(view_func)

    if 'eosio::net_api_plugin' in producer.config.plugin:
        for route, method, view_func in net_api_routes:
            logger.info(f'route {route}')
            app.route(route, methods=method)(view_func)

    if 'eosio::producer_api_plugin' in producer.config.plugin:
        for route, method, view_func in producer_api_routes:
            logger.info(f'route {route}')
            app.route(route, methods=method)(view_func)

    if 'eosio::db_size_api_plugin' in producer.config.plugin:
        for route, method, view_func in db_size_api_routers:
            logger.info(f'route {route}')
            app.route(route, methods=method)(view_func)


    app.producer = producer
    try:
        host, port = http_server_address.split(':')
        await app.server(host=host, port=port, loop=loop, use_reloader=False)
    except Exception as e:
        logger.exception(e)
        return
