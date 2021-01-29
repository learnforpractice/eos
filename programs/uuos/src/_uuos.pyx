# cython: c_string_type=str, c_string_encoding=ascii

from cython.operator cimport dereference as deref, preincrement as inc
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.map cimport map
from libcpp cimport bool
from libc.stdlib cimport malloc


cdef extern from * :
    ctypedef long long int64_t
    ctypedef unsigned long long uint64_t
    ctypedef unsigned int uint32_t
    ctypedef unsigned short uint16_t

cdef extern from "Python.h":
    object PyBytes_FromStringAndSize(const char* str, int size)

cdef extern from "uuos.hpp":
    pass
#   void register_on_accepted_block_cb_()

cdef extern from "native_object.hpp":
    object PyInit__db();

    void string_to_float128_(string& s, string& result)
    void float128_to_string_(string& s, string& result)

    ctypedef int (*fn_on_accepted_block)(string& packed_block, uint32_t num, string& block_id)
    void register_on_accepted_block(fn_on_accepted_block cb)

    ctypedef void* (*fn_run_py_func)(void *func, void *args)
    int run_py_function_(fn_run_py_func run_py_func, void *py_func, void *args, void **result)

    uint64_t s2n_(string& s);
    int n2s_(uint64_t n, string& s);

    string& uuos_get_last_error_();
    void uuos_set_last_error_(string& error);
    void uuos_set_block_interval_ms_(int ms);

    void pack_native_object_(int _type, string& msg, string& packed_message)
    void unpack_native_object_(int _type, string& packed_message, string& msg)

    void *chain_new_(string& config, string& genesis, string& protocol_features_dir, string& snapshot_dir);
    bool chain_startup_(void* ptr, bool initdb);
    void chain_free_(void *ptr);
    int chain_abort_block_(void *ptr);
    void chain_set_apply_context_(void *ptr);
    void chain_clear_apply_context_()

    void chain_get_global_properties_(void *ptr, string& result);
    void chain_get_dynamic_global_properties_(void *ptr, string& result);
    void chain_get_actor_whitelist_(void *ptr, string& result);
    void chain_get_actor_blacklist_(void *ptr, string& result);
    void chain_get_contract_whitelist_(void *ptr, string& result);
    void chain_get_contract_blacklist_(void *ptr, string& result);
    void chain_get_action_blacklist_(void *ptr, string& result);
    void chain_get_key_blacklist_(void *ptr, string& result);
    void chain_set_actor_whitelist_(void *ptr, string& params);
    void chain_set_actor_blacklist_(void *ptr, string& params);
    void chain_set_contract_whitelist_(void *ptr, string& params);
    void chain_set_action_blacklist_(void *ptr, string& params);
    void chain_set_key_blacklist_(void *ptr, string& params);
    uint32_t chain_head_block_num_(void *ptr);
    void chain_head_block_time_(void *ptr, string& result);
    void chain_head_block_id_(void *ptr, string& result);
    void chain_head_block_producer_(void *ptr, string& result);
    void chain_head_block_header_(void *ptr, string& result);
    void chain_head_block_state_(void *ptr, string& result);
    uint32_t chain_fork_db_head_block_num_(void *ptr);
    void chain_fork_db_head_block_id_(void *ptr, string& result);
    void chain_fork_db_head_block_time_(void *ptr, string& result);
    void chain_fork_db_head_block_producer_(void *ptr, string& result);
    uint32_t chain_fork_db_pending_head_block_num_(void *ptr);
    void chain_fork_db_pending_head_block_id_(void *ptr, string& result);
    void chain_fork_db_pending_head_block_time_(void *ptr, string& result);
    void chain_fork_db_pending_head_block_producer_(void *ptr, string& result);
    void chain_pending_block_time_(void *ptr, string& result);
    void chain_pending_block_producer_(void *ptr, string& result);
    void chain_pending_block_signing_key_(void *ptr, string& result);

    void chain_pending_producer_block_id_(void *ptr, string& result);
    void chain_get_pending_trx_receipts_(void *ptr, string& result);

    void chain_active_producers_(void *ptr, string& result);
    void chain_pending_producers_(void *ptr, string& result);
    void chain_proposed_producers_(void *ptr, string& result);
    uint32_t chain_last_irreversible_block_num_(void *ptr);
    void chain_last_irreversible_block_id_(void *ptr, string& result);
    void chain_fetch_block_by_number_(void *ptr, uint32_t block_num, string& raw_block );
    void chain_fetch_block_by_id_(void *ptr, string& params, string& raw_block );
    void chain_fetch_block_state_by_number_(void *ptr, uint32_t block_num, string& raw_block_state );
    void chain_fetch_block_state_by_id_(void *ptr, string& params, string& raw_block_state );
    void chain_get_block_id_for_num_(void *ptr, uint32_t block_num, string& result );
    void chain_calculate_integrity_hash_(void *ptr, string& result );
    bool chain_sender_avoids_whitelist_blacklist_enforcement_(void *ptr, string& sender);

    bool chain_check_actor_list_(void *ptr, string& param, string& err);
    bool chain_check_contract_list_(void *ptr, string& param, string& err);

    bool chain_check_action_list_(void *ptr, string& code, string& action, string& err);
    bool chain_check_key_list_(void *ptr, string& param, string& err);
    bool chain_is_building_block_(void *ptr);
    bool chain_is_producing_block_(void *ptr);

    bool chain_is_ram_billing_in_notify_allowed_(void *ptr);
    void chain_add_resource_greylist_(void *ptr, string& param);
    void chain_remove_resource_greylist_(void *ptr, string& param);
    bool chain_is_resource_greylisted_(void *ptr, string& param);
    void chain_get_resource_greylist_(void *ptr, string& result);
    void chain_get_config_(void *ptr, string& result);
    bool chain_validate_expiration_(void *ptr, string& param, string& err);
    bool chain_validate_tapos_(void *ptr, string& param, string& err);
    bool chain_validate_db_available_size_(void *ptr, string& err);
    bool chain_validate_reversible_available_size_(void *ptr, string& err);
    bool chain_is_protocol_feature_activated_(void *ptr, string& param);
    bool chain_is_builtin_activated_(void *ptr, int feature);
    bool chain_is_known_unexpired_transaction_(void *ptr, string& param);
    int64_t chain_set_proposed_producers_(void *ptr, string& param);
    bool chain_light_validation_allowed_(void *ptr, bool replay_opts_disabled_by_policy);
    bool chain_skip_auth_check_(void *ptr);

    bool chain_skip_db_sessions_(void *ptr);
    bool chain_skip_trx_checks_(void *ptr);
    bool chain_contracts_console_(void *ptr);

    bool chain_is_uuos_mainnet_(void *ptr);
    void chain_get_chain_id_(void *ptr, string& result);
    int chain_get_read_mode_(void *ptr);
    int chain_get_validation_mode_(void *ptr);
    void chain_set_subjective_cpu_leeway_(void *ptr, uint64_t leeway);
    void chain_set_greylist_limit_(void *ptr, uint32_t limit);
    uint32_t chain_get_greylist_limit_(void *ptr);
    void chain_add_to_ram_correction_(void *ptr, string& account, uint64_t ram_bytes);
    bool chain_all_subjective_mitigations_disabled_(void *ptr);

    void chain_start_block_(void *ptr, string& _time, uint16_t confirm_block_count, string& _new_features);
    void chain_get_unapplied_transactions_(void *ptr, string& result);
    bool chain_push_transaction_(void *ptr, string& packed_trx, string& deadline, uint32_t billed_cpu_time_us, string& result);
    void chain_push_scheduled_transaction_(void *ptr, string& scheduled_tx_id, string& deadline, uint32_t billed_cpu_time_us, string& result);
    void chain_commit_block_(void *ptr);
    void chain_pop_block_(void *ptr);
    void chain_get_account_(void *ptr, string& account, string& result);
    void chain_get_scheduled_producer_(void *ptr, string& _block_time, string& result);
    void chain_finalize_block_(void *ptr, string& _priv_key);
    bool chain_pack_action_args_(void *ptr, string& name, string& action, string& args, vector[char]& result);
    bool chain_unpack_action_args_(void *ptr, string& name, string& action, string& _binargs, string& result);
    
    void chain_gen_transaction_(string& _actions, string& expiration, string& reference_block_id, string& _chain_id, bool compress, string& _private_key, vector[char]& result);

    int    chain_api_get_info_(void *chain_ptr, string& info)
    int    chain_api_get_activated_protocol_features_(void *ptr, string& params, string& result)
    int    chain_api_get_block_(void *ptr, string& params, string& result)
    int    chain_api_get_block_header_state_(void *ptr, string& params, string& result)
    int    chain_api_get_account_(void *chain_ptr, string& params, string& result)
    int    chain_api_get_abi_(void *ptr, string& params, string& results)
    int    chain_api_get_raw_code_and_abi_(void *ptr, string& params, string& results)
    int    chain_api_get_raw_abi_(void *ptr, string& params, string& results)
    int    chain_api_get_table_by_scope_(void *ptr, string& params, string& result)
    int    chain_api_get_currency_balance_(void *ptr, string& params, string& result)
    int    chain_api_get_currency_stats_(void *ptr, string& params, string& result)
    int    chain_api_get_producers_(void *ptr, string& params, string& result)
    int    chain_api_get_producer_schedule_(void *ptr, string& params, string& result)

    int    chain_api_get_scheduled_transactions_(void *ptr, string& params, string& result)
    int    chain_api_abi_json_to_bin_(void *ptr, string& params, string& result)
    int    chain_api_abi_bin_to_json_(void *ptr, string& params, string& result)
    int    chain_api_get_required_keys_(void *ptr, string& params, string& result)
    int    chain_api_get_transaction_id_(void *ptr, string& params, string& result)

    int    chain_api_get_code_(void *ptr, string& params, string& results)
    int    chain_api_get_code_hash_(void *ptr, string& account, string& code_hash)
    int    chain_api_get_table_rows_(void *ptr, string& params, string& results)

    int    chain_api_recover_reversible_blocks_(string& old_reversible_blocks_dir, string& new_reversible_blocks_dir, uint32_t reversible_cache_size, uint32_t truncate_at_block)
    int    chain_api_repair_log_(string& blocks_dir, uint32_t truncate_at_block, string& backup_blocks_dir)


    void*       producer_new_(void *chain_ptr, string& config);
    void        producer_free_(void *ptr);
    void        producer_on_incoming_block_(void *ptr, string& packed_signed_block, uint32_t& num, string& id)
    int         producer_start_block_(void *ptr)
    uint64_t    producer_calc_pending_block_time_(void *ptr)
    uint64_t    producer_calc_pending_block_deadline_time_(void *ptr)
    bool        producer_maybe_produce_block_(void *ptr)
    int         producer_get_pending_block_mode_(void *ptr)
    int         producer_process_incomming_transaction_(void *ptr, string& packed_trx, string& raw_packed_trx, string& out)
    int         producer_process_raw_transaction_(void *ptr, string& raw_packed_trx, string& out)
    int         producer_create_snapshot_(void *ptr, string& out)
    int         producer_is_producer_key_(void *ptr, string& _public_key)
    int         producer_schedule_protocol_feature_activations_(void *ptr, string& _features, string& err);

    void        producer_pause_(void *ptr)
    void        producer_resume_(void *ptr)
    bool        producer_paused_(void *ptr)
    void        producer_get_runtime_options_(void *ptr, string& result)
    void        producer_update_runtime_options_(void *ptr, string& options)
    void        producer_get_scheduled_protocol_feature_activations_(void *ptr, string& result)
    void        producer_get_supported_protocol_features_(void *ptr, string& params, string& result)

    void*       history_new_(void *ptr, string& cfg)
    bool        history_startup_(void *ptr);
    void        history_free_(void *ptr)
    void        history_get_actions_(void *ptr, const string& param, string& result);
    void        history_get_transaction_(void *ptr, const string& param, string& result);
    void        history_get_key_accounts_(void *ptr, const string& param, string& result);
    void        history_get_key_accounts_ex_(void *ptr, const string& param, string& result);
    void        history_get_controlled_accounts_(void *ptr, const string& param, string& result);
    void        history_get_db_size_(void *ptr, string& result);

    void        uuos_recover_key_(string& _digest, string& _sig, string& _pub)
    uint64_t    uuos_current_time_nano_()
    void        uuos_sign_digest_(string& _priv_key, string& _digest, string& out)
    void        uuos_set_version()
    void        uuos_set_log_level_(string& logger_name, int level)

    void        uuos_set_default_data_dir_(string& dir)
    void        uuos_set_default_config_dir_(string& dir)
    void        uuos_shutdown_()
    void        uuos_initialize_logging_(string& _config_path)
    void        uuos_call_contract_off_chain_(string& _params, string& result)

    void db_size_api_get_(void *ptr, string& result)

    int app_exec_one()
    int app_exec() nogil
    int eos_main(bool run_exec, int argc, char** argv)
    void app_shutdown()
    void *app_get_chain_ptr()


def N(string& s):
    return s2n_(s)

def s2n(string& s):
    return s2n_(s)

def n2s(uint64_t n):
    cdef string s
    n2s_(n, s)
    return s

def uuos_get_last_error():
    return uuos_get_last_error_()

def uuos_set_last_error(string& error):
    uuos_set_last_error_(error)

def pack_native_object(int _type, string& msg):
    cdef string packed_message
    pack_native_object_(_type, msg, packed_message)
    return <bytes>packed_message

def unpack_native_object(int _type, string& packed_message):
    cdef string msg
    unpack_native_object_(_type, packed_message, msg)
    return <bytes>msg

def chain_new(string& config, string& genesis, string& protocol_features_dir, string& snapshot_dir):
    return <unsigned long long>chain_new_(config, genesis, protocol_features_dir, snapshot_dir)

def chain_set_apply_context(uint64_t ptr):
    chain_set_apply_context_(<void *>ptr);

def chain_clear_apply_context():
    chain_clear_apply_context_()

def chain_startup(uint64_t ptr, bool initdb):
    return chain_startup_(<void*> ptr, initdb);

def chain_free(unsigned long long  ptr):
    chain_free_(<void *>ptr);

def chain_abort_block(uint64_t ptr):
    return chain_abort_block_(<void *>ptr)

def chain_get_global_properties(uint64_t ptr):
    cdef string result
    chain_get_global_properties_(<void *>ptr, result)
    return result

def chain_get_dynamic_global_properties(uint64_t ptr):
    cdef string result
    chain_get_dynamic_global_properties_(<void *>ptr, result)
    return result

def chain_get_actor_whitelist(uint64_t ptr):
    cdef string result
    chain_get_actor_whitelist_(<void *>ptr, result)
    return result

def chain_get_actor_blacklist(uint64_t ptr):
    cdef string result
    chain_get_actor_blacklist_(<void *>ptr, result)
    return result

def chain_get_contract_whitelist(uint64_t ptr):
    cdef string result
    chain_get_contract_whitelist_(<void *>ptr, result)
    return result

def chain_get_contract_blacklist(uint64_t ptr):
    cdef string result
    chain_get_contract_blacklist_(<void *>ptr, result)
    return result

def chain_get_action_blacklist(uint64_t ptr):
    cdef string result
    chain_get_action_blacklist_(<void *>ptr, result)
    return result

def chain_get_key_blacklist(uint64_t ptr):
    cdef string result
    chain_get_key_blacklist_(<void *>ptr, result)
    return result

def chain_set_actor_whitelist(uint64_t ptr, string& params):
    chain_set_actor_whitelist_(<void *>ptr, params)

def chain_set_actor_blacklist(uint64_t ptr, string& params):
    chain_set_actor_blacklist_(<void *>ptr, params)

def chain_set_contract_whitelist(uint64_t ptr, string& params):
    chain_set_contract_whitelist_(<void *>ptr, params)

def chain_set_action_blacklist(uint64_t ptr, string& params):
    chain_set_action_blacklist_(<void *>ptr, params)

def chain_set_key_blacklist(uint64_t ptr, string& params):
    chain_set_key_blacklist_(<void *>ptr, params)

def chain_head_block_num(uint64_t ptr):
    return chain_head_block_num_(<void *>ptr)

def chain_head_block_time(uint64_t ptr):
    cdef string result
    chain_head_block_time_(<void *>ptr, result)
    return result

def chain_head_block_id(uint64_t ptr):
    cdef string result
    chain_head_block_id_(<void *>ptr, result)
    return result

def chain_head_block_producer(uint64_t ptr):
    cdef string result
    chain_head_block_producer_(<void *>ptr, result)
    return result

def chain_head_block_header(uint64_t ptr):
    cdef string result
    chain_head_block_header_(<void *>ptr, result)
    return result

def chain_head_block_state(uint64_t ptr):
    cdef string result
    chain_head_block_state_(<void *>ptr, result)
    return result

def chain_fork_db_head_block_num(uint64_t ptr):
    return chain_fork_db_head_block_num_(<void *>ptr)

def chain_fork_db_head_block_id(uint64_t ptr):
    cdef string result
    chain_fork_db_head_block_id_(<void *>ptr, result)
    return result

def chain_fork_db_head_block_time(uint64_t ptr):
    cdef string result
    chain_fork_db_head_block_time_(<void *>ptr, result)
    return result

def chain_fork_db_head_block_producer(uint64_t ptr):
    cdef string result
    chain_fork_db_head_block_producer_(<void *>ptr, result)
    return result

def chain_fork_db_pending_head_block_num(uint64_t ptr):
    return chain_fork_db_pending_head_block_num_(<void *>ptr)

def chain_fork_db_pending_head_block_id(uint64_t ptr):
    cdef string result
    chain_fork_db_pending_head_block_id_(<void *>ptr, result)
    return result

def chain_fork_db_pending_head_block_time(uint64_t ptr):
    cdef string result
    chain_fork_db_pending_head_block_time_(<void *>ptr, result)
    return result

def chain_fork_db_pending_head_block_producer(uint64_t ptr):
    cdef string result
    chain_fork_db_pending_head_block_producer_(<void *>ptr, result)
    return result

def chain_pending_block_time(uint64_t ptr):
    cdef string result
    chain_pending_block_time_(<void *>ptr, result)
    return result

def chain_pending_block_producer(uint64_t ptr):
    cdef string result
    chain_pending_block_producer_(<void *>ptr, result)
    return result

def chain_pending_block_signing_key(uint64_t ptr):
    cdef string result
    ret = chain_pending_block_signing_key_(<void *>ptr, result)
    return result

def chain_pending_producer_block_id(uint64_t ptr):
    cdef string result
    chain_pending_producer_block_id_(<void *>ptr, result)
    return result

def chain_get_pending_trx_receipts(uint64_t ptr):
    cdef string result
    chain_get_pending_trx_receipts_(<void *>ptr, result)
    return result

def chain_active_producers(uint64_t ptr):
    cdef string result
    chain_active_producers_(<void *>ptr, result)
    return result

def chain_pending_producers(uint64_t ptr):
    cdef string result
    chain_pending_producers_(<void *>ptr, result)
    return result

def chain_proposed_producers(uint64_t ptr):
    cdef string result
    chain_proposed_producers_(<void *>ptr, result)
    return result

def chain_last_irreversible_block_num(uint64_t ptr):
    return chain_last_irreversible_block_num_(<void *>ptr)

def chain_last_irreversible_block_id(uint64_t ptr):
    cdef string result
    chain_last_irreversible_block_id_(<void *>ptr, result)
    return result

def chain_fetch_block_by_number(uint64_t ptr, uint32_t block_num):
    cdef string raw_block
    chain_fetch_block_by_number_(<void *>ptr, block_num, raw_block)
    return <bytes>raw_block

def chain_fetch_block_by_id(uint64_t ptr, string& block_id):
    cdef string raw_block
    chain_fetch_block_by_id_(<void *>ptr, block_id, raw_block)
    return <bytes>raw_block

def chain_fetch_block_state_by_number(uint64_t ptr, uint32_t block_num):
    cdef string raw_block_state
    chain_fetch_block_state_by_number_(<void *>ptr, block_num, raw_block_state)
    return <bytes>raw_block_state

def chain_fetch_block_state_by_id(uint64_t ptr, string& block_id):
    cdef string raw_block_state
    chain_fetch_block_state_by_id_(<void *>ptr, block_id, raw_block_state)
    return <bytes>raw_block_state

def chain_get_block_id_for_num(uint64_t ptr, uint32_t block_num):
    cdef string result
    chain_get_block_id_for_num_(<void *>ptr, block_num, result)
    return result

def chain_calculate_integrity_hash(uint64_t ptr):
    cdef string result
    chain_calculate_integrity_hash_(<void *>ptr, result)
    return result

def chain_sender_avoids_whitelist_blacklist_enforcement(uint64_t ptr, string& sender):
    cdef string result
    return chain_sender_avoids_whitelist_blacklist_enforcement_(<void *>ptr, sender)

def chain_check_actor_list(uint64_t ptr, string& param):
    cdef string err
    ret = chain_check_actor_list_(<void *>ptr, param, err)
    return ret, err

def chain_check_contract_list(uint64_t ptr, string& param):
    cdef string err
    ret = chain_check_contract_list_(<void *>ptr, param, err)
    return ret, err

def chain_check_action_list(uint64_t ptr, string& code, string& action):
    cdef string err
    ret = chain_check_action_list_(<void *>ptr, code, action, err)
    return ret, err

def chain_check_key_list(uint64_t ptr, string& param):
    cdef string err
    ret = chain_check_key_list_(<void *>ptr, param, err)
    return ret, err

def chain_is_building_block(uint64_t ptr):
    return chain_is_building_block_(<void *>ptr)

def chain_is_producing_block(uint64_t ptr):
    return chain_is_producing_block_(<void *>ptr)

def chain_is_ram_billing_in_notify_allowed(uint64_t ptr):
    return chain_is_ram_billing_in_notify_allowed_(<void *>ptr)

def chain_add_resource_greylist(uint64_t ptr, string& param):
    chain_add_resource_greylist_(<void *>ptr, param)

def chain_remove_resource_greylist(uint64_t ptr, string& param):
    chain_remove_resource_greylist_(<void *>ptr, param)

def chain_is_resource_greylisted(uint64_t ptr, string& param):
    return chain_is_resource_greylisted_(<void *>ptr, param)

def chain_get_resource_greylist(uint64_t ptr):
    cdef string result
    chain_get_resource_greylist_(<void *>ptr, result)
    return result

def chain_get_config(uint64_t ptr):
    cdef string result
    chain_get_config_(<void *>ptr, result)
    return result

def chain_validate_expiration(uint64_t ptr, string& trx):
    cdef string err
    ret = chain_validate_expiration_(<void *>ptr, trx, err)
    return ret, err

def chain_validate_tapos(uint64_t ptr, string& trx):
    cdef string err
    ret = chain_validate_tapos_(<void *>ptr, trx, err)
    return ret, err

def chain_validate_db_available_size(uint64_t ptr):
    cdef string err
    ret = chain_validate_db_available_size_(<void *>ptr, err)
    return ret, err

def chain_validate_reversible_available_size(uint64_t ptr):
    cdef string err
    ret = chain_validate_reversible_available_size_(<void *>ptr, err)
    return ret, err

def chain_is_protocol_feature_activated(uint64_t ptr, string& digest):
    return chain_is_protocol_feature_activated_(<void *>ptr, digest)

def chain_is_builtin_activated(uint64_t ptr, int feature):
    return chain_is_builtin_activated_(<void *>ptr, feature)

def chain_is_known_unexpired_transaction(uint64_t ptr, string& trx):
    return chain_is_known_unexpired_transaction_(<void *>ptr, trx)

def chain_set_proposed_producers(uint64_t ptr, string& param):
    return chain_set_proposed_producers_(<void *>ptr, param)

def chain_light_validation_allowed(uint64_t ptr, bool replay_opts_disabled_by_policy):
    return chain_light_validation_allowed_(<void *>ptr, replay_opts_disabled_by_policy)

def chain_skip_auth_check(uint64_t ptr):
    return chain_skip_auth_check_(<void *>ptr)

def chain_skip_db_sessions(uint64_t ptr):
    return chain_skip_db_sessions_(<void *>ptr)

def chain_skip_trx_checks(uint64_t ptr):
    return chain_skip_trx_checks_(<void *>ptr)

def chain_contracts_console(uint64_t ptr):
    return chain_contracts_console_(<void *>ptr)

def chain_is_uuos_mainnet(uint64_t ptr):
    return chain_is_uuos_mainnet_(<void *>ptr)

def chain_get_chain_id(uint64_t ptr):
    cdef string chain_id
    chain_get_chain_id_(<void *>ptr, chain_id)
    return chain_id

def chain_get_read_mode(uint64_t ptr):
    return chain_get_read_mode_(<void *>ptr)

def chain_get_validation_mode(uint64_t ptr):
    return chain_get_validation_mode_(<void *>ptr)

def chain_set_subjective_cpu_leeway(uint64_t ptr, leeway):
    chain_set_subjective_cpu_leeway_(<void *>ptr, leeway)

def chain_set_greylist_limit(uint64_t ptr, limit):
    chain_set_greylist_limit_(<void *>ptr, limit)

def chain_get_greylist_limit(uint64_t ptr):
    return chain_get_greylist_limit_(<void *>ptr)

def chain_add_to_ram_correction(uint64_t ptr, string& account, uint64_t ram_bytes):
    return chain_add_to_ram_correction_(<void *>ptr, account, ram_bytes)

def chain_all_subjective_mitigations_disabled(uint64_t ptr):
    return chain_all_subjective_mitigations_disabled_(<void *>ptr)

def chain_fork_db_pending_head_block_num(uint64_t ptr):
    return chain_fork_db_pending_head_block_num_(<void *>ptr)

def chain_last_irreversible_block_num(uint64_t ptr):
    return chain_last_irreversible_block_num_(<void *>ptr)

def chain_get_block_id_for_num(uint64_t ptr, uint32_t num):
    cdef string block_id
    chain_get_block_id_for_num_(<void *>ptr, num, block_id)
    return block_id

def chain_id(uint64_t ptr):
    cdef string chain_id
    chain_get_chain_id_(<void *>ptr, chain_id)
    return chain_id

def chain_fetch_block_by_number(uint64_t ptr, uint32_t block_num ):
    cdef string raw_block
    chain_fetch_block_by_number_(<void *>ptr, block_num, raw_block)
    return <bytes>raw_block

def chain_start_block(uint64_t ptr, string& time, uint16_t confirm_block_count, string& new_features):
    chain_start_block_(<void *>ptr, time, confirm_block_count, new_features)

def chain_get_unapplied_transactions(uint64_t ptr):
    cdef string result
    chain_get_unapplied_transactions_(<void *>ptr, result)
    return result

def chain_push_transaction(uint64_t ptr, string& packed_trx, string& deadline, uint32_t billed_cpu_time_us):
    cdef string _result
    ret = chain_push_transaction_(<void *>ptr, packed_trx, deadline, billed_cpu_time_us, _result)
    result = <bytes>_result
    result = result.decode('utf8')
    return ret, result

def chain_push_scheduled_transaction(uint64_t ptr, string& scheduled_tx_id, string& deadline, uint32_t billed_cpu_time_us):
    cdef string result
    chain_push_scheduled_transaction_(<void *>ptr, scheduled_tx_id, deadline, billed_cpu_time_us, result)
    return result

def chain_commit_block(uint64_t ptr):
    return chain_commit_block_(<void *>ptr)

def chain_pop_block(uint64_t ptr):
    chain_pop_block_(<void *>ptr)

def chain_get_account(uint64_t ptr, string& account):
    cdef string result
    chain_get_account_(<void *>ptr, account, result)
    return result

def chain_get_scheduled_producer(uint64_t ptr, string& block_time):
    cdef string result
    chain_get_scheduled_producer_(<void *>ptr, block_time, result)
    return result

def chain_finalize_block(uint64_t ptr, string& _priv_key):
    chain_finalize_block_(<void *>ptr, _priv_key)

def chain_pack_action_args(uint64_t ptr, string& name, string& action, string& args):
    cdef vector[char] result
    cdef bool ret
    ret = chain_pack_action_args_(<void *>ptr, name, action, args, result)
    return PyBytes_FromStringAndSize(result.data(), result.size())

def chain_unpack_action_args(uint64_t ptr, string& name, string& action, string& binargs):
    cdef string result
    cdef bool ret
    ret = chain_unpack_action_args_(<void *>ptr, name, action, binargs, result)
    return result

def chain_gen_transaction(string& _actions, string& expiration, string& reference_block_id, string& _chain_id, bool compress, string& _private_key):
    cdef vector[char] result
    chain_gen_transaction_(_actions, expiration, reference_block_id, _chain_id, compress, _private_key, result)
    return PyBytes_FromStringAndSize(result.data(), result.size())

#------------chain api----------------

def chain_api_get_info(uint64_t chain_ptr):
    cdef string info
    err = chain_api_get_info_(<void *>chain_ptr, info)
    return err, info

def chain_api_get_activated_protocol_features(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_activated_protocol_features_(<void *>chain_ptr, params, result)
    return err, result

def chain_api_get_block(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_block_(<void *>chain_ptr, params, result)
    return err, result

def chain_api_get_block_header_state(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_block_header_state_(<void *>chain_ptr, params, result)
    return err, result

def chain_api_get_account(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_account_(<void *>chain_ptr, params, result)
    return err, result

def chain_api_get_code(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_code_(<void *>chain_ptr, params, result)
    r = PyBytes_FromStringAndSize(result.c_str(), result.size())
#    print(r)
    return err, r.decode('utf8')

def chain_api_get_code_hash(uint64_t chain_ptr, string& params):
    cdef string code_hash
    err = chain_api_get_code_hash_(<void *>chain_ptr, params, code_hash)
    return err, code_hash

def chain_api_get_abi(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_abi_(<void *>chain_ptr, params, result)
    r = PyBytes_FromStringAndSize(result.c_str(), result.size())
    return err, r.decode('utf8')

def chain_api_get_raw_code_and_abi(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_raw_code_and_abi_(<void *>chain_ptr, params, result)
    return err, result

def chain_api_get_raw_abi(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_raw_abi_(<void *>chain_ptr, params, result)
    r = PyBytes_FromStringAndSize(result.c_str(), result.size())
    return err, r.decode('utf8')

def chain_api_get_table_rows(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_table_rows_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_table_by_scope(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_table_by_scope_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_currency_balance(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_currency_balance_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_currency_stats(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_currency_stats_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_producers(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_producers_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_producer_schedule(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_producer_schedule_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_scheduled_transactions(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_scheduled_transactions_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_abi_json_to_bin(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_abi_json_to_bin_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_abi_bin_to_json(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_abi_bin_to_json_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_required_keys(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_required_keys_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_transaction_id(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_transaction_id_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_recover_reversible_blocks(string& old_reversible_blocks_dir, string& new_reversible_blocks_dir, uint32_t reversible_cache_size, uint32_t truncate_at_block):
    return chain_api_recover_reversible_blocks_(old_reversible_blocks_dir, new_reversible_blocks_dir, reversible_cache_size, truncate_at_block)

def chain_api_repair_log(string& blocks_dir, uint32_t truncate_at_block):
    cdef string backup_blocks_dir
    chain_api_repair_log_(blocks_dir, truncate_at_block, backup_blocks_dir)
    return backup_blocks_dir

def producer_new(uint64_t chain_ptr, string& config):
    return <uint64_t>producer_new_(<void *>chain_ptr, config)

def producer_free(uint64_t ptr):
    producer_free_(<void *>ptr)

def producer_on_incoming_block(uint64_t ptr, string& packed_signed_block):
    cdef uint32_t block_num = 0
    cdef string block_id
    producer_on_incoming_block_(<void *>ptr, packed_signed_block, block_num, block_id)
    return (block_num, block_id)

def producer_start_block(uint64_t ptr):
    return producer_start_block_(<void *>ptr)

def producer_pause(uint64_t ptr):
    producer_pause_(<void *>ptr)

def producer_resume(uint64_t ptr):
    producer_resume_(<void *>ptr)

def producer_paused(uint64_t ptr):
    return producer_paused_(<void *>ptr)

def producer_calc_pending_block_time(uint64_t ptr):
    return producer_calc_pending_block_time_(<void *>ptr)

def producer_calc_pending_block_deadline_time(uint64_t ptr):
    return producer_calc_pending_block_deadline_time_(<void *>ptr)

def producer_maybe_produce_block(uint64_t ptr):
    return producer_maybe_produce_block_(<void *>ptr);

def producer_get_pending_block_mode(uint64_t ptr):
    return producer_get_pending_block_mode_(<void *>ptr)

def producer_process_incomming_transaction(uint64_t ptr, string& packed_trx):
    cdef string out
    cdef string raw_packed_trx
    err = producer_process_incomming_transaction_(<void *>ptr, packed_trx, raw_packed_trx, out)
    return err, out, PyBytes_FromStringAndSize(raw_packed_trx.c_str(), raw_packed_trx.size())

def producer_process_raw_transaction(uint64_t ptr, string& raw_packed_trx):
    cdef string out
    err = producer_process_raw_transaction_(<void *>ptr, raw_packed_trx, out)
    return err, out

def producer_create_snapshot(uint64_t ptr):
    cdef string out
    ret = producer_create_snapshot_(<void *>ptr, out)
    return ret, out

def producer_is_producer_key(uint64_t ptr, string& public_key):
    return producer_is_producer_key_(<void *>ptr, public_key)

def producer_schedule_protocol_feature_activations(uint64_t ptr, string& features):
    cdef string err
    ret = producer_schedule_protocol_feature_activations_(<void *>ptr, features, err)
    return ret, err

def producer_get_runtime_options(uint64_t ptr):
    cdef string result
    producer_get_runtime_options_(<void *>ptr, result)
    return result

def producer_update_runtime_options(uint64_t ptr, string& options):
    producer_update_runtime_options_(<void *>ptr, options)

def producer_get_scheduled_protocol_feature_activations(uint64_t ptr):
    cdef string result
    producer_get_scheduled_protocol_feature_activations_(<void *>ptr, result)
    return result

def producer_get_supported_protocol_features(uint64_t ptr, string& params):
    cdef string result
    producer_get_supported_protocol_features_(<void *>ptr, params, result)
    return result

def history_new(uint64_t ptr, string& cfg):
    return <uint64_t>history_new_(<void *>ptr, cfg)

def history_startup(uint64_t ptr):
    return history_startup_(<void *>ptr);

def history_free(uint64_t ptr):
    history_free_(<void *>ptr)

def history_get_actions(uint64_t ptr, const string& param):
    cdef string result
    history_get_actions_(<void *>ptr, param, result)
    return result

def history_get_transaction(uint64_t ptr, const string& param):
    cdef string result
    history_get_transaction_(<void *>ptr, param, result)
    return result

def history_get_key_accounts(uint64_t ptr, const string& param):
    cdef string result
    history_get_key_accounts_(<void *>ptr, param, result)
    return result

def history_get_key_accounts_ex(uint64_t ptr, const string& param):
    cdef string result
    history_get_key_accounts_ex_(<void *>ptr, param, result)
    return result

def history_get_controlled_accounts(uint64_t ptr, const string& param):
    cdef string result
    history_get_controlled_accounts_(<void *>ptr, param, result)
    return result

def history_get_db_size(uint64_t ptr):
    cdef string result
    history_get_db_size_(<void *>ptr, result)
    return result

g_accepted_block_cb = None
cdef int on_accepted_block(string& packed_block, uint32_t block_num, string& block_id):
    global g_accepted_block_cb
    try:
        if g_accepted_block_cb:
            block = PyBytes_FromStringAndSize(packed_block.c_str(), packed_block.size())
            id = PyBytes_FromStringAndSize(block_id.c_str(), block_id.size())
            g_accepted_block_cb(block, block_num, id)
    except KeyboardInterrupt:
        uuos_shutdown_()
    return 1

def set_accepted_block_callback(cb):
    global g_accepted_block_cb
    g_accepted_block_cb = cb

g_config = None
def uuos_set_config(config):
    global g_config
    g_config = config

cdef extern string uuos_config_get_str(option):
    global g_config
    cdef string ret
    try:
        value = getattr(g_config, option)
        value = str(value)
        ret = value
    except Exception as e:
        pass
    return ret

cdef extern int uuos_config_get_int(option):
    global g_config
    try:
        value = getattr(g_config, option)
        value = int(value)
        return value
    except Exception as e:
        print(e)
    return 0

def uuos_recover_key(string& digest, string& sign):
    cdef string pub
    uuos_recover_key_(digest, sign, pub)
    return pub

def uuos_current_time_nano():
    return uuos_current_time_nano_()

def uuos_sign_digest(string& _priv_key, string& _digest):
    cdef string out
    uuos_sign_digest_(_priv_key, _digest, out)
    return out

def uuos_set_log_level(string& logger_name, int level):
    uuos_set_log_level_(logger_name, level)

def uuos_set_default_data_dir(string& dir):
    return uuos_set_default_data_dir_(dir)

def uuos_set_default_config_dir(string& dir):
    return uuos_set_default_config_dir_(dir)

def uuos_shutdown():
    uuos_shutdown_()

def uuos_initialize_logging(string& _config_path):
    uuos_initialize_logging_(_config_path)

def uuos_call_contract_off_chain(string& params):
    cdef string result
    uuos_call_contract_off_chain_(params, result)
    return result

def db_size_api_get(uint64_t ptr):
    cdef string result
    db_size_api_get_(<void *>ptr, result)
    return result

def s2f(string& s):
    cdef string result
    string_to_float128_(s, result)
    return PyBytes_FromStringAndSize(result.c_str(), result.size())

def f2s(string& s):
    cdef string result
    if s.size() != 16:
        raise Exception('bad size')
    float128_to_string_(s, result)
    return result

cdef extern object run_py_code(object func):
    return func()

cdef object run_py_func(object func, object args):
    try:
        return func(*args)
    except Exception as e:
        return e

def run_py_func_safe(func, args):
    cdef void *result
    cdef int ret
    ret = run_py_function_(<fn_run_py_func>run_py_func, <void *>func, <void *>args, &result)
    if not ret:
        return ret, None
    return ret, <object>result

def uuos_exec_one():
    return app_exec_one()

def uuos_exec():
    cdef int ret;
    with nogil:
        ret = app_exec()
    return ret

def uuos_init(args):
    cdef int argc;
    cdef char **argv
#    for arg in args:
#        print(arg)

    argc = len(args)
    argv = <char **>malloc(argc * sizeof(char *))
    for i in range(argc):
        argv[i] = args[i]

    return eos_main(0, argc, argv)

def uuos_shutdown2():
    app_shutdown()

def uuos_get_chain_ptr():
    return <uint64_t>app_get_chain_ptr()

def uuos_set_block_interval_ms(ms):
    return uuos_set_block_interval_ms_(ms)

register_on_accepted_block(on_accepted_block)

uuos_set_version()
_db = <object>PyInit__db()
