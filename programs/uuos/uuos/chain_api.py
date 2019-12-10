from _uuos import *

chain_ptr = None
def get_info():
    return chain_api_get_info(chain_ptr)

def get_activated_protocol_features(params):
    return chain_api_get_activated_protocol_features(chain_ptr, params)

def get_block(params):
    return chain_api_get_block(chain_ptr, params)

def get_block_header_state(params):
    return chain_api_get_block_header_state(chain_ptr, params)

def get_account(name):
    return chain_api_get_account(chain_ptr, name)

def get_code(params):
    return chain_api_get_code(chain_ptr, params)

def get_code_hash(params):
    return chain_api_get_code_hash(chain_ptr, params)

def get_abi(params):
    return chain_api_get_abi(chain_ptr, params)

def get_raw_code_and_abi(params):
    return chain_api_get_raw_code_and_abi(chain_ptr, params)

def get_raw_abi(params):
    return chain_api_get_raw_abi(chain_ptr, params)

def get_table_rows(params):
    return chain_api_get_table_rows(chain_ptr, params)

def get_table_by_scope(params):
    return chain_api_get_table_by_scope(chain_ptr, params)

def get_currency_balance(params):
    return chain_api_get_currency_balance(chain_ptr, params)

def get_currency_stats(params):
    return chain_api_get_currency_stats(chain_ptr, params)

def get_producers(params):
    return chain_api_get_producers(chain_ptr, params)

def get_producer_schedule(params):
    return chain_api_get_producer_schedule(chain_ptr, params)

def recover_reversible_blocks(old_reversible_blocks_dir, new_reversible_blocks_dir, reversible_cache_size = 340*1024*1024, truncate_at_block=0):
    return chain_api_recover_reversible_blocks(old_reversible_blocks_dir, new_reversible_blocks_dir, reversible_cache_size, truncate_at_block)

def repair_log(blocks_dir, truncate_at_block=0):
    return chain_api_repair_log(blocks_dir, truncate_at_block)

def get_table_rows(params):
    return chain_api_get_table_rows(chain_ptr, params)
