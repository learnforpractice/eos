from _hello import *
chain_ptr = None

def id():
    return chain_id(chain_ptr)

def fork_db_pending_head_block_num():
    return chain_fork_db_pending_head_block_num(chain_ptr)

def last_irreversible_block_num():
    return chain_last_irreversible_block_num(chain_ptr)

def get_block_id_for_num(num):
    return chain_get_block_id_for_num(chain_ptr, num)

def fetch_block_by_number(block_num):
    return chain_fetch_block_by_number(chain_ptr, block_num)

