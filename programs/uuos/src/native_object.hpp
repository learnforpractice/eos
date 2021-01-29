#pragma once
#include <string>
#include <Python.h>
using namespace std;

void string_to_float128_(string& s, string& result);
void float128_to_string_(string& s, string& result);

extern "C" PyObject* PyInit__db(void);

typedef void* (*fn_run_py_func)(void *func, void *args);
int run_py_function_(fn_run_py_func run_py_func, void *py_func, void *args, void **result);

typedef int (*fn_on_accepted_block)(string& packed_block, uint32_t num, string& block_id);
void register_on_accepted_block(fn_on_accepted_block cb);

string& uuos_get_last_error_();
void uuos_set_last_error_(string& error);

void pack_native_object_(int type, string& msg, string& packed_message);
void unpack_native_object_(int type, string& packed_message, string& msg);

uint64_t s2n_(string& s);
int n2s_(uint64_t n, string& s);

void *chain_new_(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
bool chain_startup_(void* ptr, bool initdb);
void chain_free_(void *ptr);
void chain_set_apply_context_(void *ptr);
void chain_clear_apply_context_();

void chain_id_(void *ptr, string& chain_id);
int chain_abort_block_(void *ptr);
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
void chain_fetch_block_by_number_(void *ptr, uint32_t block_num, string& raw_block);
void chain_fetch_block_by_id_(void *ptr, string& params, string& raw_block);
void chain_fetch_block_state_by_number_(void *ptr, uint32_t block_num, string& raw_block_state);
void chain_fetch_block_state_by_id_(void *ptr, string& params, string& raw_block_state);
void chain_get_block_id_for_num_(void *ptr, uint32_t block_num, string& result);
void chain_calculate_integrity_hash_(void *ptr, string& result);
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

bool chain_pack_action_args_(void *ptr, string& name, string& action, string& args, vector<char>& result);
bool chain_unpack_action_args_(void *ptr, string& name, string& action, string& _binargs, string& result);

void chain_gen_transaction_(string& _actions, string& expiration, string& reference_block_id, string& _chain_id, bool compress, std::string& _private_key, vector<char>& result);

int     chain_api_get_info_(void *chain_ptr, string& info);

#define DEF_CHAIN_API_RO(api_name) \
    int chain_api_ ## api_name ## _(void *ptr, string& params, string& result);

DEF_CHAIN_API_RO(get_activated_protocol_features)
DEF_CHAIN_API_RO(get_block)
DEF_CHAIN_API_RO(get_block_header_state)
DEF_CHAIN_API_RO(get_account)
DEF_CHAIN_API_RO(get_code)
DEF_CHAIN_API_RO(get_code_hash)
DEF_CHAIN_API_RO(get_abi)
DEF_CHAIN_API_RO(get_raw_code_and_abi)
DEF_CHAIN_API_RO(get_raw_abi)
DEF_CHAIN_API_RO(get_table_rows)
DEF_CHAIN_API_RO(get_table_by_scope)
DEF_CHAIN_API_RO(get_currency_balance)
DEF_CHAIN_API_RO(get_currency_stats)
DEF_CHAIN_API_RO(get_producers)
DEF_CHAIN_API_RO(get_producer_schedule)

DEF_CHAIN_API_RO(get_scheduled_transactions)
DEF_CHAIN_API_RO(abi_json_to_bin)
DEF_CHAIN_API_RO(abi_bin_to_json)
DEF_CHAIN_API_RO(get_required_keys)
DEF_CHAIN_API_RO(get_transaction_id)

int    chain_api_recover_reversible_blocks_(string& old_reversible_blocks_dir, string& new_reversible_blocks_dir, uint32_t reversible_cache_size, uint32_t truncate_at_block);
int    chain_api_repair_log_(string& blocks_dir, uint32_t truncate_at_block, string& backup_blocks_dir);


void*       producer_new_(void *chain_ptr, string& config);
void        producer_free_(void *ptr);
void        producer_on_incoming_block_(void *ptr, string& packed_signed_block, uint32_t& num, string& id);

int         producer_start_block_(void *ptr);
uint64_t    producer_calc_pending_block_time_(void *ptr);
uint64_t    producer_calc_pending_block_deadline_time_(void *ptr);
bool        producer_maybe_produce_block_(void *ptr);
int         producer_get_pending_block_mode_(void *ptr);
int         producer_process_incomming_transaction_(void *ptr, string& packed_trx, string& raw_packed_trx, string& out);
int         producer_process_raw_transaction_(void *ptr, string& raw_packed_trx, string& out);
int         producer_create_snapshot_(void *ptr, string& out);
int         producer_is_producer_key_(void *ptr, string& _public_key);
int         producer_schedule_protocol_feature_activations_(void *ptr, string& _features, string& err);
int         producer_get_runtime_options_(void *ptr, string& result);
void        producer_update_runtime_options_(void *ptr, string& options);
void        producer_get_supported_protocol_features_(void *ptr, string& params, string& result);
void        producer_get_scheduled_protocol_feature_activations_(void *ptr, string& result);

void        producer_pause_(void *ptr);
void        producer_resume_(void *ptr);
bool        producer_paused_(void *ptr);


void*       history_new_(void *chain_ptr, string& cfg);
bool        history_startup_(void *ptr);
void        history_free_(void *ptr);
void        history_get_actions_(void *ptr, const string& param, string& result);
void        history_get_transaction_(void *ptr, const string& param, string& result);
void        history_get_key_accounts_(void *ptr, const string& param, string& result);
void        history_get_key_accounts_ex_(void *ptr, const string& param, string& result);
void        history_get_controlled_accounts_(void *ptr, const string& param, string& result);
void        history_get_db_size_(void *ptr, string& result);

void        uuos_set_version();
void        uuos_recover_key_(string& _digest, string& _sig, string& _pub);
uint64_t    uuos_current_time_nano_();
void        uuos_sign_digest_(string& _priv_key, string& _digest, string& out);
void        uuos_set_log_level_(string& logger_name, int level);

void        uuos_set_default_data_dir_(string& dir);
void        uuos_set_default_config_dir_(string& dir);
void        uuos_shutdown_();
void        uuos_initialize_logging_(string& _config_path);
void        uuos_call_contract_off_chain_(string& _params, string& result);
void        uuos_set_block_interval_ms_(int ms);

void        db_size_api_get_(void *ptr, string& result);

enum {
    handshake_message_type,
    chain_size_message_type,
    go_away_message_type,
    time_message_type,
    notice_message_type,
    request_message_type,
    sync_request_message_type,
    signed_block_type,         // which = 7
    packed_transaction_type, //8
    controller_config_type, //9
    producer_params_type, //10
    genesis_state_type, //11
    abi_def_type,
    transaction_type,
    signed_transaction_type,
};

int app_exec_one();
int eos_main(bool run_exec, int argc, char** argv);
void app_shutdown();
void *app_get_chain_ptr();
int app_exec();
