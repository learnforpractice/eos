import _uuos
import ujson as json
from datetime import datetime

class Chain(object):

    def __init__(self, config, genesis, protocol_features_dir, snapshot_dir):
        self.new(config, genesis, protocol_features_dir, snapshot_dir)

    def new(self, config, genesis, protocol_features_dir, snapshot_dir):
        self.ptr = _uuos.chain_new(config, genesis, protocol_features_dir, snapshot_dir)
        if not self.ptr:
            error = _uuos.get_last_error()
            raise Exception(error)
        _uuos.chain_set_current_ptr(self.ptr)

    def startup(self, initdb):
        return _uuos.chain_startup(self.ptr, initdb)

    def free(self):
        if self.ptr:
            _uuos.chain_free(self.ptr)
            self.ptr = 0
        _uuos.chain_set_current_ptr(0)

    def id(self):
        return _uuos.id(self.ptr)

    def start_block(self, _time, confirm_block_count=0, features=None):
        if features:
            if isinstance(features, list):
                features = json.dumps(features)
        else:
            features = ''
        _uuos.start_block(self.ptr, _time, confirm_block_count, features)

    def abort_block(self):
        return _uuos.abort_block(self.ptr)

    def get_global_properties(self, json=True):
        ret = _uuos.get_global_properties(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def get_dynamic_global_properties(self, json=True):
        ret = _uuos.get_dynamic_global_properties(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def get_actor_whitelist(self, json=True):
        ret = _uuos.get_actor_whitelist(self.ptr)
        if ret:
            return json.loads(ret)
        return ret

    def get_actor_blacklist(self, json=True):
        ret = _uuos.get_actor_blacklist(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def get_contract_whitelist(self, json=True):
        ret = _uuos.get_contract_whitelist(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def get_contract_blacklist(self, json=True):
        ret = _uuos.get_contract_blacklist(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def get_action_blacklist(self, json=True):
        ret = _uuos.get_action_blacklist(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def get_key_blacklist(self, json=True):
        ret = _uuos.get_key_blacklist(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def get_key_blacklist(self, json=True):
        ret = _uuos.get_key_blacklist(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def set_actor_whitelist(self, whitelist):
        whitelist = json.dumps(whitelist)
        _uuos.set_actor_whitelist(self.ptr, whitelist)

    def set_actor_blacklist(self, blacklist):
        blacklist = json.dumps(blacklist)
        _uuos.set_actor_blacklist(self.ptr, blacklist)

    def set_contract_whitelist(self, whitelist):
        whitelist = json.dumps(whitelist)
        _uuos.set_contract_whitelist(self.ptr, whitelist)

    def set_action_blacklist(self, blacklist):
        blacklist = json.dumps(blacklist)
        _uuos.set_action_blacklist(self.ptr, blacklist)

    def set_key_blacklist(self, blacklist):
        blacklist = json.dumps(blacklist)
        _uuos.set_key_blacklist(self.ptr, blacklist)

    def head_block_num(self):
        return _uuos.head_block_num(self.ptr)

    def head_block_time(self):
        iso_time = _uuos.head_block_time(self.ptr)
        return datetime.strptime(iso_time, "%Y-%m-%dT%H:%M:%S.%f")

    def head_block_id(self):
        return _uuos.head_block_id(self.ptr)

    def head_block_producer(self):
        return _uuos.head_block_producer(self.ptr)

    def head_block_header(self, json=True):
        ret = _uuos.head_block_header(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def head_block_state(self, json=True):
        ret = _uuos.head_block_state(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def fork_db_head_block_num(self):
        return _uuos.fork_db_head_block_num(self.ptr)

    def fork_db_head_block_id(self):
        return _uuos.fork_db_head_block_id(self.ptr)

    def fork_db_head_block_time(self):
        return _uuos.fork_db_head_block_time(self.ptr)

    def fork_db_head_block_producer(self):
        return _uuos.fork_db_head_block_producer(self.ptr)

    def fork_db_pending_head_block_num(self):
        return _uuos.fork_db_pending_head_block_num(self.ptr)

    def fork_db_pending_head_block_id(self):
        return _uuos.fork_db_pending_head_block_id(self.ptr)

    def fork_db_pending_head_block_time(self):
        return _uuos.fork_db_pending_head_block_time(self.ptr)

    def fork_db_pending_head_block_producer(self):
        return _uuos.fork_db_pending_head_block_producer(self.ptr)

    def pending_block_time(self):
        iso_time = _uuos.pending_block_time(self.ptr)
        return datetime.strptime(iso_time, "%Y-%m-%dT%H:%M:%S.%f")

    def pending_block_producer(self):
        return _uuos.pending_block_producer(self.ptr)

    def pending_block_signing_key(self):
        return _uuos.pending_block_signing_key(self.ptr)

    def pending_producer_block_id(self):
        return _uuos.pending_producer_block_id(self.ptr)

    def get_pending_trx_receipts(self, json=True):
        ret = _uuos.get_pending_trx_receipts(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def active_producers(self, json=True):
        ret = _uuos.active_producers(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def pending_producers(self, json=True):
        ret = _uuos.pending_producers(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def proposed_producers(self, json=True):
        ret = _uuos.proposed_producers(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def last_irreversible_block_num(self):
        return _uuos.last_irreversible_block_num(self.ptr)

    def last_irreversible_block_id(self):
        return _uuos.last_irreversible_block_id(self.ptr)

    def fetch_block_by_number(self, block_num):
        return _uuos.fetch_block_by_number(self.ptr, block_num, raw_block)

    def fetch_block_by_id(self, block_id):
        return _uuos.fetch_block_by_id(self.ptr, block_id)

    def fetch_block_state_by_number(self, block_num):
        _uuos.fetch_block_state_by_number(self.ptr, block_num)

    def fetch_block_state_by_id(self, block_id):
        return _uuos.fetch_block_state_by_id(self.ptr, block_id)

    def get_block_id_for_num(self, block_num):
        return _uuos.get_block_id_for_num(self.ptr, block_num)

    def calculate_integrity_hash(self):
        return _uuos.calculate_integrity_hash(self.ptr)

    def sender_avoids_whitelist_blacklist_enforcement(self, sender):
        return _uuos.sender_avoids_whitelist_blacklist_enforcement(self.ptr, sender)

    def check_actor_list(self, actors):
        return _uuos.check_actor_list(self.ptr, actors)

    def check_contract_list(self, code):
        return _uuos.check_contract_list(self.ptr, code)

    def check_action_list(self, code, action):
        return _uuos.check_action_list(self.ptr, code, action)

    def check_key_list(self, pub_key):
        return _uuos.check_key_list(self.ptr, pub_key)

    def is_building_block(self):
        return _uuos.is_building_block(self.ptr)

    def is_producing_block(self):
        return _uuos.is_producing_block(self.ptr)

    def is_ram_billing_in_notify_allowed(self):
        return _uuos.is_ram_billing_in_notify_allowed(self.ptr)

    def add_resource_greylist(self, name):
        _uuos.add_resource_greylist(self.ptr, name)

    def remove_resource_greylist(self, name):
        _uuos.remove_resource_greylist(self.ptr, name)

    def is_resource_greylisted(self, name):
        return _uuos.is_resource_greylisted(self.ptr, name)

    def get_resource_greylist(self):
        return _uuos.get_resource_greylist(self.ptr)

    def get_config(self, json=True):
        ret = _uuos.get_config(self.ptr)
        if json:
            return json.loads(ret)
        return ret

    def validate_expiration(self, trx):
        return _uuos.validate_expiration(self.ptr, trx)

    def validate_tapos(self, trx):
        return _uuos.validate_tapos(self.ptr, trx)

    def validate_db_available_size(self):
        return _uuos.validate_db_available_size(self.ptr)

    def validate_reversible_available_size(self):
        return _uuos.validate_reversible_available_size(self.ptr)

    def is_protocol_feature_activated(self, digest):
        return _uuos.is_protocol_feature_activated(self.ptr, digest)

    def is_builtin_activated(self, feature):
        return _uuos.is_builtin_activated(self.ptr, feature)

    def is_known_unexpired_transaction(self, trx):
        return _uuos.is_known_unexpired_transaction(self.ptr, trx)

    def set_proposed_producers(self, producers):
        return _uuos.set_proposed_producers(self.ptr, producers)

    def light_validation_allowed(self, replay_opts_disabled_by_policy):
        return _uuos.light_validation_allowed(self.ptr, replay_opts_disabled_by_policy)

    def skip_auth_check(self):
        return _uuos.skip_auth_check(self.ptr)

    def skip_db_sessions(self):
        return _uuos.skip_db_sessions(self.ptr)

    def skip_trx_checks(self):
        return _uuos.skip_trx_checks(self.ptr)

    def contracts_console(self):
        return _uuos.contracts_console(self.ptr)

    def is_uuos_mainnet(self):
        return _uuos.is_uuos_mainnet(self.ptr)

    def id(self):
        return _uuos.id(self.ptr)

    def get_read_mode(self):
        return _uuos.get_read_mode(self.ptr)

    def get_validation_mode(self):
        return _uuos.get_validation_mode(self.ptr)

    def set_subjective_cpu_leeway(self, leeway):
        _uuos.set_subjective_cpu_leeway(self.ptr, leeway)

    def set_greylist_limit(self, limit):
        _uuos.set_greylist_limit(self.ptr, limit)

    def get_greylist_limit(self):
        return _uuos.get_greylist_limit(self.ptr)

    def add_to_ram_correction(self, account, ram_bytes):
        return _uuos.add_to_ram_correction(self.ptr, account, ram_bytes)

    def all_subjective_mitigations_disabled(self):
        return _uuos.all_subjective_mitigations_disabled(self.ptr)

    def fork_db_pending_head_block_num(self):
        return _uuos.fork_db_pending_head_block_num(self.ptr)

    def get_block_id_for_num(self, num):
        return _uuos.get_block_id_for_num(self.ptr, num)

    def fetch_block_by_number(self, block_num):
        return _uuos.fetch_block_by_number(self.ptr, block_num)

    def is_building_block(self):
        return _uuos.is_building_block(self.ptr)


    def get_unapplied_transactions(self):
        return _uuos.get_unapplied_transactions(self.ptr)
 
    def push_transaction(self, packed_trx, deadline, billed_cpu_time_us):
        ret, result = _uuos.push_transaction(self.ptr, packed_trx, deadline, billed_cpu_time_us)
        if not ret:
            if not result:
                result = _uuos.get_last_error()
        return ret, result

    def get_scheduled_transaction(self, sender_id, sender):
        ret = _uuos.get_scheduled_transaction(self.ptr, sender_id, sender)
        if ret:
            return json.loads(ret)

    def get_scheduled_transactions(self):
        ret = _uuos.get_scheduled_transactions(self.ptr)
        return json.loads(ret)

    def push_scheduled_transaction(self, scheduled_tx_id, deadline, billed_cpu_time_us):
        return _uuos.push_scheduled_transaction(self.ptr, scheduled_tx_id, deadline, billed_cpu_time_us)

    def commit_block(self):
        return _uuos.commit_block(self.ptr)

    def pop_block(self):
        return _uuos.pop_block(self.ptr)

    def get_account(self, account):
        return _uuos.get_account(self.ptr, account)

    def get_scheduled_producer(self, block_time):
        return _uuos.get_scheduled_producer(self.ptr, block_time)

    def get_scheduled_producer(self, block_time):
        if not isinstance(block_time, str):
            block_time = block_time.isoformat(timespec='milliseconds')
        return _uuos.get_scheduled_producer(self.ptr, block_time)

    def finalize_block(self, priv_keys):
        if isinstance(priv_keys, list):
            priv_keys = json.dumps(priv_keys)
        _uuos.finalize_block(self.ptr, priv_keys)

    def get_producer_public_keys(self):
        ret = _uuos.get_producer_public_keys(self.ptr)
        return json.loads(ret)

    def pack_action_args(self, name, action, args):
        if isinstance(args, dict):
            args = json.dumps(args)
        return _uuos.pack_action_args(self.ptr, name, action, args)

    def unpack_action_args(self, name, action, raw_args):
        return _uuos.unpack_action_args(self.ptr, name, action, raw_args)

    def gen_transaction(self, _actions, expiration, reference_block_id, _id, compress, _private_keys):
        if isinstance(_actions, dict):
            _actions = json.dumps(_actions)
        return _uuos.gen_transaction(_actions, expiration, reference_block_id, _id, compress, _private_keys)
