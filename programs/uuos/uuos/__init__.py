__all__ = [
    'application',
    'blocklog',
    'chain_api',
    'config',
    'connection',
    'native_object',
    'p2pmanager',
    'rpc_server'
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

def set_log_level(logger_name, level):
    return _uuos.uuos_set_log_level(logger_name, level)
