from . import _uuos

class NativeType:
    handshake_message = 0
    chain_size_message = 1
    go_away_message = 2
    time_message = 3
    notice_message = 4
    request_message = 5
    sync_request_message = 6
    signed_block_v0 = 7         # which = 7
    packed_transaction_v0 = 8   # which = 8
    signed_block = 9            # which = 9
    trx_message_v1 = 10         # which = 10
    genesis_state = 11
    abi_def = 12

def set_log_level(logger_name, level):
    _uuos.set_log_level(logger_name, level)

def set_block_interval_ms(ms):
    _uuos.set_block_interval_ms(ms)

def pack_native_object(_type, obj):
    return _uuos.pack_native_object(_type, obj)

def unpack_native_object(_type, packed_obj):
    return _uuos.unpack_native_object(_type, packed_message)

def pack_abi(abi):
    return pack_native_object(NativeType.abi_def, abi)
