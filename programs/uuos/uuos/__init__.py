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

from _uuos import (uuos_set_config,
    uuos_set_log_level,
    uuos_recover_key,
    uuos_current_time_nano,
    uuos_sign_digest,
    uuos_set_log_level
)

def set_config(config):
    return uuos_set_config(config)

def recover_key(digest, sign):
    return uuos_recover_key(digest, sign)

def current_time_nano():
    return uuos_current_time_nano()

def sign_digest(priv_key, digest):
    return uuos_sign_digest(priv_key, digest)

def set_log_level(logger_name, level):
    return uuos_set_log_level(logger_name, level)
