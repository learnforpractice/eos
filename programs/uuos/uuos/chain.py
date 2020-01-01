import _uuos
import ujson as json
from .jsonobject import JsonObject

chain_ptr = None

def get_chain_ptr():
    global chain_ptr
    return chain_ptr

def set_chain_ptr(ptr):
    global chain_ptr
    chain_ptr = ptr

def new(config, protocol_features_dir, snapshot_dir):
    global chain_ptr
    chain_ptr = _uuos.chain_new(config, protocol_features_dir, snapshot_dir)
    return chain_ptr

def free(ptr=None):
    if not ptr:
        if chain_ptr:
            _uuos.chain_free(chain_ptr)
    else:
        _uuos.chain_free(chain_ptr)

def id():
    return _uuos.chain_id(chain_ptr)

def abort_block():
    return _uuos.chain_abort_block(chain_ptr)

def get_global_properties():
    ret = _uuos.chain_get_global_properties(chain_ptr)
    return JsonObject(ret)

def get_dynamic_global_properties():
    ret = _uuos.chain_get_dynamic_global_properties(chain_ptr)
    return JsonObject(ret)

def get_actor_whitelist():
    ret = _uuos.chain_get_actor_whitelist(chain_ptr)
    return json.loads(ret)

def get_actor_blacklist():
    ret = _uuos.chain_get_actor_blacklist(chain_ptr)
    return json.loads(ret)

def get_contract_whitelist():
    ret = _uuos.chain_get_contract_whitelist(chain_ptr)
    return json.loads(ret)

def get_contract_blacklist():
    ret = _uuos.chain_get_contract_blacklist(chain_ptr)
    return json.loads(ret)

def get_action_blacklist():
    ret = _uuos.chain_get_action_blacklist(chain_ptr)
    return json.loads(ret)

def get_key_blacklist():
    ret = _uuos.chain_get_key_blacklist(chain_ptr)
    return json.loads(ret)

def chain_get_key_blacklist():
    ret = _uuos.chain_get_key_blacklist(chain_ptr)
    return json.loads(ret)

def chain_set_actor_whitelist(whitelist):
    whitelist = json.dumps(whitelist)
    _uuos.chain_set_actor_whitelist(chain_ptr, whitelist)

def set_actor_blacklist(blacklist):
    blacklist = json.dumps(blacklist)
    _uuos.chain_set_actor_blacklist(chain_ptr, blacklist)

def set_contract_whitelist(whitelist):
    whitelist = json.dumps(whitelist)
    _uuos.chain_set_contract_whitelist(chain_ptr, whitelist)

def set_action_blacklist(blacklist):
    blacklist = json.dumps(blacklist)
    _uuos.chain_set_action_blacklist(chain_ptr, blacklist)

def set_key_blacklist(blacklist):
    blacklist = json.dumps(blacklist)
    _uuos.chain_set_key_blacklist(chain_ptr, blacklist)

def head_block_num():
    return _uuos.chain_head_block_num(chain_ptr)

def head_block_time():
    return _uuos.chain_head_block_time(chain_ptr)

def head_block_id():
   return _uuos.chain_head_block_id(chain_ptr)

def head_block_producer():
    return _uuos.chain_head_block_producer(chain_ptr)

def head_block_header():
    ret = _uuos.chain_head_block_header(chain_ptr)
    return JsonObject(ret)

def head_block_state():
    ret = _uuos.chain_head_block_state(chain_ptr)
    return JsonObject(ret)

def fork_db_head_block_num():
    return _uuos.chain_fork_db_head_block_num(chain_ptr)

def fork_db_head_block_id():
    return _uuos.chain_fork_db_head_block_id(chain_ptr)

def fork_db_head_block_time():
    return _uuos.chain_fork_db_head_block_time(chain_ptr)

def fork_db_head_block_producer():
    return _uuos.chain_fork_db_head_block_producer(chain_ptr)

def fork_db_pending_head_block_num():
    return _uuos.chain_fork_db_pending_head_block_num(chain_ptr)

def fork_db_pending_head_block_id():
    return _uuos.chain_fork_db_pending_head_block_id(chain_ptr)

def fork_db_pending_head_block_time():
    return _uuos.chain_fork_db_pending_head_block_time(chain_ptr, result)

def fork_db_pending_head_block_producer():
    return _uuos.chain_fork_db_pending_head_block_producer(chain_ptr, result)

def pending_block_time():
    return _uuos.chain_pending_block_time(chain_ptr)

def pending_block_producer():
    return _uuos.chain_pending_block_producer(chain_ptr)

def pending_block_signing_key():
    return _uuos.chain_pending_block_signing_key(chain_ptr)

def pending_producer_block_id():
    return _uuos.chain_pending_producer_block_id(chain_ptr)

def get_pending_trx_receipts():
    return _uuos.chain_get_pending_trx_receipts(chain_ptr)

def active_producers():
    return _uuos.chain_active_producers(chain_ptr)

def active_producers():
    return _uuos.chain_active_producers(chain_ptr)

def pending_producers():
    return _uuos.chain_pending_producers(chain_ptr, result)

def proposed_producers():
    return _uuos.chain_proposed_producers(chain_ptr, result)

def last_irreversible_block_num():
    return _uuos.chain_last_irreversible_block_num(chain_ptr)

def last_irreversible_block_id():
    return _uuos.chain_last_irreversible_block_id(chain_ptr, result)

def fetch_block_by_number(block_num):
    return _uuos.chain_fetch_block_by_number(chain_ptr, block_num, raw_block)

def fetch_block_by_id(block_id):
    return _uuos.chain_fetch_block_by_id(chain_ptr, block_id)

def fetch_block_state_by_number(block_num):
    _uuos.chain_fetch_block_state_by_number(chain_ptr, block_num)

def fetch_block_state_by_id(block_id):
    return _uuos.chain_fetch_block_state_by_id(chain_ptr, block_id)

def get_block_id_for_num(block_num):
    return _uuos.chain_get_block_id_for_num(chain_ptr, block_num)

def calculate_integrity_hash():
    return _uuos.chain_calculate_integrity_hash(chain_ptr)

def sender_avoids_whitelist_blacklist_enforcement(sender):
    return _uuos.chain_sender_avoids_whitelist_blacklist_enforcement(chain_ptr, sender)

def check_actor_list(actors):
    return _uuos.chain_check_actor_list(chain_ptr, actors)

def check_contract_list(code):
    return _uuos.chain_check_contract_list(chain_ptr, code)

def check_action_list(code, action):
    return _uuos.chain_check_action_list(chain_ptr, code, action)

def check_key_list(pub_key):
    return _uuos.chain_check_key_list(chain_ptr, pub_key)

def is_building_block():
    return _uuos.chain_is_building_block(chain_ptr)

def is_producing_block():
    return _uuos.chain_is_producing_block(chain_ptr)

def is_ram_billing_in_notify_allowed():
    return _uuos.chain_is_ram_billing_in_notify_allowed(chain_ptr)

def add_resource_greylist(name):
    _uuos.chain_add_resource_greylist(chain_ptr, name)

def remove_resource_greylist(name):
    _uuos.chain_remove_resource_greylist(chain_ptr, name)

def is_resource_greylisted(name):
    return _uuos.chain_is_resource_greylisted(chain_ptr, name)

def get_resource_greylist():
    return _uuos.chain_get_resource_greylist(chain_ptr)

def get_config():
    return _uuos.chain_get_config(chain_ptr)

def validate_expiration(trx):
    return _uuos.chain_validate_expiration(chain_ptr, trx, err)

def validate_tapos(trx):
    return _uuos.chain_validate_tapos(chain_ptr, trx, err)

def validate_db_available_size():
    return _uuos.chain_validate_db_available_size(chain_ptr, err)

def validate_reversible_available_size():
   return _uuos.chain_validate_reversible_available_size(chain_ptr, err)

def is_protocol_feature_activated(digest):
    return _uuos.chain_is_protocol_feature_activated(chain_ptr, digest)

def is_builtin_activated(feature):
    return _uuos.chain_is_builtin_activated(chain_ptr, feature)

def is_known_unexpired_transaction(trx):
    return _uuos.chain_is_known_unexpired_transaction(chain_ptr, trx)

def set_proposed_producers(producers):
    return _uuos.chain_set_proposed_producers(chain_ptr, producers)

def light_validation_allowed(replay_opts_disabled_by_policy):
    return _uuos.chain_light_validation_allowed(chain_ptr, replay_opts_disabled_by_policy)

def skip_auth_check():
    return _uuos.chain_skip_auth_check(chain_ptr)

def skip_db_sessions():
    return _uuos.chain_skip_db_sessions(chain_ptr)

def skip_trx_checks():
    return _uuos.chain_skip_trx_checks(chain_ptr)

def contracts_console():
    return _uuos.chain_contracts_console(chain_ptr)

def is_uuos_mainnet():
    return _uuos.chain_is_uuos_mainnet(chain_ptr)

def get_chain_id():
    return _uuos.chain_get_chain_id(chain_ptr)

def get_read_mode():
    return _uuos.chain_get_read_mode(chain_ptr)

def get_validation_mode():
    return _uuos.chain_get_validation_mode(chain_ptr)

def set_subjective_cpu_leeway(leeway):
    _uuos.chain_set_subjective_cpu_leeway(chain_ptr, leeway)

def set_greylist_limit(limit):
    _uuos.chain_set_greylist_limit(chain_ptr, limit)

def get_greylist_limit():
    return _uuos.chain_get_greylist_limit(chain_ptr)

def add_to_ram_correction(account, ram_bytes):
    return _uuos.chain_add_to_ram_correction(chain_ptr, account, ram_bytes)

def all_subjective_mitigations_disabled():
    return _uuos.chain_all_subjective_mitigations_disabled(chain_ptr)

def fork_db_pending_head_block_num():
    return _uuos.chain_fork_db_pending_head_block_num(chain_ptr)

def last_irreversible_block_num():
    return _uuos.chain_last_irreversible_block_num(chain_ptr)

def get_block_id_for_num(num):
    return _uuos.chain_get_block_id_for_num(chain_ptr, num)

def fetch_block_by_number(block_num):
    return _uuos.chain_fetch_block_by_number(chain_ptr, block_num)

def is_building_block():
    return _uuos.chain_is_building_block(chain_ptr)