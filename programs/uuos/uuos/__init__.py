__all__ = [
    'application',
    'blocklog',
    'chainapi',
    'config',
    'connection',
    'nativeobject',
    'p2pmanager',
    'rpcserver',
    'set_config',
    'recover_key',
    'current_time_nano',
    'sign_digest',
    'set_log_level',
    'set_default_log_level',
    'pack_native_object',
    'unpack_native_object',
]

import _uuos
import ujson as json

class LogLevel:
    all = 0
    debug = 1
    info = 2
    warn = 3
    error = 4
    off  = 5

log_names = (
    'default',
    'net_plugin_impl',
    'http_plugin',
    'producer_plugin',
    'transaction_tracing'
)

def set_config(config):
    return _uuos.uuos_set_config(config)

def recover_key(digest, sign):
    return _uuos.uuos_recover_key(digest, sign)

def current_time_nano():
    return _uuos.uuos_current_time_nano()

def sign_digest(priv_key, digest):
    return _uuos.uuos_sign_digest(priv_key, digest)

def set_log_level(logger_name='default', level=10):
    return _uuos.uuos_set_log_level(logger_name, level)

def set_default_log_level(level):
    return _uuos.uuos_set_log_level('default', level)

def pack_native_object(obj_type, obj):
    return _uuos.pack_native_object(obj_type, obj)

def unpack_native_object(obj_type, packed_message):
    return _uuos.unpack_native_object(obj_type, packed_message)

def set_default_data_dir(dir):
    _uuos.uuos_set_default_data_dir(dir)

def set_default_config_dir(dir):
    _uuos.uuos_set_default_config_dir(dir)

def get_last_error():
    return _uuos.uuos_get_last_error()

def set_accepted_block_callback(cb):
    _uuos.set_accepted_block_callback(cb)

def initialize_logging(config_path):
    _uuos.uuos_initialize_logging(config_path)

def call_contract_off_chain(params):
    r = _uuos.uuos_call_contract_off_chain(params)
    return json.loads(r)
