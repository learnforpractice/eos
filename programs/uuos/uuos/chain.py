from _uuos import *
chain_ptr = None

def get_chain_ptr():
    global chain_ptr
    return chain_ptr

def set_chain_ptr(ptr):
    global chain_ptr
    chain_ptr = ptr

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

def is_building_block():
    return chain_is_building_block(chain_ptr)