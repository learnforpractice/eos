from _uuos import *

chain_ptr = None
def get_info():
    return chain_api_get_info(chain_ptr)

def get_account(name):
    return chain_api_get_account(chain_ptr, name)

def recover_reversible_blocks(old_reversible_blocks_dir, new_reversible_blocks_dir, reversible_cache_size = 340*1024*1024, truncate_at_block=0):
    return chain_api_recover_reversible_blocks(old_reversible_blocks_dir, new_reversible_blocks_dir, reversible_cache_size, truncate_at_block)

def repair_log(blocks_dir, truncate_at_block=0):
    return chain_api_repair_log(blocks_dir, truncate_at_block)
