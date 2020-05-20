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
    void say_hello_();
#   void register_on_accepted_block_cb_()

    void* chain_new_(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
    bool chain_startup_(void* ptr, bool initdb);
    void chain_free_(void *ptr);
    void chain_id_(void *ptr, string& chain_id);
    void chain_start_block_(void *ptr, string& _time, uint16_t confirm_block_count, string& _new_features);
    int  chain_abort_block_(void *ptr);
    void chain_get_preactivated_protocol_features_(void *ptr, string& result);
    void chain_get_unapplied_transactions_(void *ptr, string& result);
    bool chain_pack_action_args_(void *ptr, string& name, string& action, string& _args, vector[char]& result);
    bool chain_unpack_action_args_(void *ptr, string& name, string& action, string& _binargs, string& result);
    void chain_gen_transaction_(string& _actions, string& expiration, string& reference_block_id, string& _chain_id, bool compress, string& _private_key, vector[char]& result);
    bool chain_push_transaction_(void *ptr, string& _packed_trx, string& deadline, uint32_t billed_cpu_time_us, string& result);
    void chain_push_scheduled_transaction_(void *ptr, string& scheduled_tx_id, string& deadline, uint32_t billed_cpu_time_us, string& result);
    void chain_commit_block_(void *ptr);
    void chain_finalize_block_(void *ptr, string& _priv_key);
    void chain_pop_block_(void *ptr);
    void chain_get_account_(void *ptr, string& account, string& result);
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
    bool chain_sender_avoids_whitelist_blacklist_enforcement_(void *ptr, string& sender );
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
    void chain_get_scheduled_producer_(void *ptr, string& _block_time, string& result);


def say_hello():
    return say_hello_()

def chain_new(string& config, string& genesis, string& protocol_features_dir, string& snapshot_dir):
    return <uint64_t>chain_new_(config, genesis, protocol_features_dir, snapshot_dir)
