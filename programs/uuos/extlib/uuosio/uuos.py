from . import _uuos

def set_log_level(logger_name, level):
    _uuos.set_log_level(logger_name, level)

def set_block_interval_ms(ms):
    _uuos.set_block_interval_ms(ms)

def pack_abi(abi):
    return _uuos.pack_abi(abi)
