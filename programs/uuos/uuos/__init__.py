__all__ = [
    'application',
    'blocklog',
    'chain_api',
    'config',
    'connection',
    'native_object',
    'p2pmanager',
    'rpc_server',
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
