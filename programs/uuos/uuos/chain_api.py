from _uuos import *

chain_ptr = None
def get_info():
    return chain_api_get_info(chain_ptr)

def get_activated_protocol_features(params):
    return chain_api_get_activated_protocol_features(chain_ptr, params)

def get_block(params):
    return chain_api_get_block(chain_ptr, params)

def get_account(name):
    return chain_api_get_account(chain_ptr, name)

def recover_reversible_blocks(old_reversible_blocks_dir, new_reversible_blocks_dir, reversible_cache_size = 340*1024*1024, truncate_at_block=0):
    return chain_api_recover_reversible_blocks(old_reversible_blocks_dir, new_reversible_blocks_dir, reversible_cache_size, truncate_at_block)

def repair_log(blocks_dir, truncate_at_block=0):
    return chain_api_repair_log(blocks_dir, truncate_at_block)

def get_code(params):
    return chain_api_get_code(chain_ptr, params)

def get_code_hash(params):
    return chain_api_get_code_hash(chain_ptr, params)

def get_table_rows(params):
    return chain_api_get_table_rows(chain_ptr, params)
