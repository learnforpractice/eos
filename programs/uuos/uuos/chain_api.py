import _uuos

chain_ptr = None
def get_info():
    return _uuos.chain_api_get_info(chain_ptr)

def get_activated_protocol_features(params):
    return _uuos.chain_api_get_activated_protocol_features(chain_ptr, params)

def get_block(params):
    return _uuos.chain_api_get_block(chain_ptr, params)

def get_block_header_state(params):
    return _uuos.chain_api_get_block_header_state(chain_ptr, params)

def get_account(name):
    return _uuos.chain_api_get_account(chain_ptr, name)

def get_code(params):
    return _uuos.chain_api_get_code(chain_ptr, params)

def get_code_hash(params):
    return _uuos.chain_api_get_code_hash(chain_ptr, params)

def get_abi(params):
    return _uuos.chain_api_get_abi(chain_ptr, params)

def get_raw_code_and_abi(params):
    return _uuos.chain_api_get_raw_code_and_abi(chain_ptr, params)

def get_raw_abi(params):
    return _uuos.chain_api_get_raw_abi(chain_ptr, params)

def get_table_rows(params):
    return _uuos.chain_api_get_table_rows(chain_ptr, params)

def get_table_by_scope(params):
    return _uuos.chain_api_get_table_by_scope(chain_ptr, params)

def get_currency_balance(params):
    return _uuos.chain_api_get_currency_balance(chain_ptr, params)

def get_currency_stats(params):
    return _uuos.chain_api_get_currency_stats(chain_ptr, params)

def get_producers(params):
    return _uuos.chain_api_get_producers(chain_ptr, params)

def get_producer_schedule(params):
    return _uuos.chain_api_get_producer_schedule(chain_ptr, params)

def get_scheduled_transactions(params):
    return _uuos.chain_api_get_scheduled_transactions(chain_ptr, params)

def abi_json_to_bin(params):
    return _uuos.chain_api_abi_json_to_bin(chain_ptr, params)

def abi_bin_to_json(params):
    return _uuos.chain_api_abi_bin_to_json(chain_ptr, params)

def get_required_keys(params):
    return _uuos.chain_api_get_required_keys(chain_ptr, params)

def get_transaction_id(params):
    return _uuos.chain_api_get_transaction_id(chain_ptr, params)

def recover_reversible_blocks(old_reversible_blocks_dir, new_reversible_blocks_dir, reversible_cache_size = 340*1024*1024, truncate_at_block=0):
    return _uuos.chain_api_recover_reversible_blocks(old_reversible_blocks_dir, new_reversible_blocks_dir, reversible_cache_size, truncate_at_block)

def repair_log(blocks_dir, truncate_at_block=0):
    return _uuos.chain_api_repair_log(blocks_dir, truncate_at_block)

def get_table_rows(params):
    return _uuos.chain_api_get_table_rows(chain_ptr, params)

def db_size_api_get():
    return _uuos.db_size_api_get(chain_ptr)
