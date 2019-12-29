import _uuos

chain_ptr = None

def get_chain_ptr():
    global chain_ptr
    return chain_ptr

def set_chain_ptr(ptr):
    global chain_ptr
    chain_ptr = ptr

def free():
    if chain_ptr:
        _uuos.chain_free(chain_ptr)

def id():
    return _uuos.chain_id(chain_ptr)

def fork_db_pending_head_block_num():
    return _uuos.chain_fork_db_pending_head_block_num(chain_ptr)

def last_irreversible_block_num():
    return _uuos.chain_last_irreversible_block_num(chain_ptr)

def get_block_id_for_num(num):
    return _uuos.chain_get_block_id_for_num(chain_ptr, num)

def fetch_block_by_nun(block_num):
    return _uuos.chain_fetch_block_by_number(chain_ptr, block_num)

def is_building_block():
    return _uuos.chain_is_building_block(chain_ptr)