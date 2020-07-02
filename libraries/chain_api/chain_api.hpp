/*
 * chain_api.hpp
 *
 *  Created on: Oct 13, 2018
 *      Author: newworld
 */

#ifndef CHAIN_API_HPP_
#define CHAIN_API_HPP_

#include <stdint.h>
#include <string>
#include <vector>
// #include <eosio/chain/types.hpp>
#include <fc/crypto/sha256.hpp>

using namespace std;

using checksum_type       = fc::sha256;
using digest_type         = checksum_type;

enum class enum_builtin_protocol_feature : uint32_t {
   preactivate_feature,
   only_link_to_existing_permission,
   replace_deferred,
   no_duplicate_deferred_id,
   fix_linkauth_restriction,
   disallow_empty_producer_schedule,
   restrict_action_to_self,
   only_bill_first_authorizer,
   forward_setcode,
   get_sender,
   ram_restrictions,

   python_vm=1024,
   ethereum_vm,
};

struct vm_callback {
   int (*setcode)(uint64_t account, string& code);
   int (*apply)(string& hash, uint8_t vmtype, uint8_t vmversion);
};

struct chain_api_cpp
{
   void *chain_ptr;

   void (*register_vm_callback)(uint8_t vmtype, uint8_t vmversion, vm_callback *cb);
   vm_callback* (*get_vm_callback)(uint8_t vmtype, uint8_t vmversion);

   bool (*get_apply_args)(uint64_t& receiver, uint64_t& code, uint64_t& action);

   void (*init_eosio)();
   int (*start_eosio)(int argc, char** argv);

   void (*uuos_set_log_level)(string& logger_name, int level);
   void (*uuos_set_version)();
   void (*uuos_set_default_data_dir)(string& dir);
   void (*uuos_set_default_config_dir)(string& dir);
   void (*uuos_shutdown)();
   void (*uuos_sign_digest)(string& _priv_key, string& _digest, string& out);

   string& (*uuos_get_last_error)();
   void (*uuos_set_last_error)(string& error);
   void (*uuos_on_error)(string& _ex);

   int64_t (*get_current_exception)(std::string& what);
   void (*n2str)(uint64_t n, string& str_name);
   uint64_t (*str2n)(string& str_name);

   bool (*get_code)(uint64_t contract, string& code_id, string& code);
   const char* (*get_code_ex)( uint64_t contract, size_t* size );
   bool (*get_code_id)( uint64_t receiver, uint8_t* code_id, size_t size);
   int (*get_code_type)( uint64_t receiver);

   bool (*get_code_by_code_hash)(const digest_type& code_hash, const uint8_t vm_type, const uint8_t vm_version, const char** code, size_t* size, uint32_t* first_block_used);

   string (*get_state_dir)();
   bool (*contracts_console)();
   void (*resume_billing_timer)(void);
   void (*pause_billing_timer)(void);
   bool (*is_producing_block)();
   bool (*is_pythonvm_loading_finished)();
//vm_exceptions.cpp
   void (*throw_exception)(int type, const char* fmt, ...);
   uint64_t (*get_microseconds)();

   void (*set_mem_segment_range)(const char *start, int offset, int size);
   void (*get_mem_segment_range)(const char **start, int *offset, int *size);

   void (*get_resource_limits)( uint64_t account, int64_t* ram_bytes, int64_t* net_weight, int64_t* cpu_weight );

   bool (*enable_debug)(bool enable);
   bool (*is_debug_enabled)();
   bool (*add_debug_contract)(string& contract_name, string& path);
   bool (*clear_debug_contract)(string& contract_name);
   void* (*get_debug_contract_entry)(string& contract_name);
   bool (*is_builtin_activated)(uint32_t feature);
   string (*call_contract_off_chain)(uint64_t contract, uint64_t action, const vector<char>& binargs);

//--------------------------------chain api-------------------------------------
   void *(*chain_get_current_ptr)();
   void (*chain_set_current_ptr)(void *ptr);
   void* (*chain_new)(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
   bool (*chain_startup)(void* ptr, bool initdb);
   void (*chain_free)(void *ptr);
   void (*chain_id)(void *ptr, string& chain_id);
   void (*chain_start_block)(void *ptr, string& _time, uint16_t confirm_block_count, string& _new_features);
   int  (*chain_abort_block)(void *ptr);
   void (*chain_get_preactivated_protocol_features)(void *ptr, string& result);
   void (*chain_get_unapplied_transactions)(void *ptr, string& result);
   bool (*chain_pack_action_args)(void *ptr, string& name, string& action, string& _args, vector<char>& result);
   bool (*chain_unpack_action_args)(void *ptr, string& name, string& action, string& _binargs, string& result);
   void (*chain_gen_transaction)(string& _actions, string& expiration, string& reference_block_id, string& _chain_id, bool compress, std::string& _private_key, vector<char>& result);
   bool (*chain_push_transaction)(void *ptr, string& _packed_trx, string& deadline, uint32_t billed_cpu_time_us, string& result);
   void (*chain_push_scheduled_transaction)(void *ptr, string& scheduled_tx_id, string& deadline, uint32_t billed_cpu_time_us, string& result);
   void (*chain_commit_block)(void *ptr);
   void (*chain_finalize_block)(void *ptr, string& _priv_key);
   void (*chain_pop_block)(void *ptr);
   void (*chain_get_account)(void *ptr, string& account, string& result);
   void (*chain_get_global_properties)(void *ptr, string& result);
   void (*chain_get_dynamic_global_properties)(void *ptr, string& result);
   void (*chain_get_actor_whitelist)(void *ptr, string& result);
   void (*chain_get_actor_blacklist)(void *ptr, string& result);
   void (*chain_get_contract_whitelist)(void *ptr, string& result);
   void (*chain_get_contract_blacklist)(void *ptr, string& result);
   void (*chain_get_action_blacklist)(void *ptr, string& result);
   void (*chain_get_key_blacklist)(void *ptr, string& result);
   void (*chain_set_actor_whitelist)(void *ptr, string& params);
   void (*chain_set_actor_blacklist)(void *ptr, string& params);
   void (*chain_set_contract_whitelist)(void *ptr, string& params);
   void (*chain_set_action_blacklist)(void *ptr, string& params);
   void (*chain_set_key_blacklist)(void *ptr, string& params);
   uint32_t (*chain_head_block_num)(void *ptr);
   void (*chain_head_block_time)(void *ptr, string& result);
   void (*chain_head_block_id)(void *ptr, string& result);
   void (*chain_head_block_producer)(void *ptr, string& result);
   void (*chain_head_block_header)(void *ptr, string& result);
   void (*chain_head_block_state)(void *ptr, string& result);
   uint32_t (*chain_fork_db_head_block_num)(void *ptr);
   void (*chain_fork_db_head_block_id)(void *ptr, string& result);
   void (*chain_fork_db_head_block_time)(void *ptr, string& result);
   void (*chain_fork_db_head_block_producer)(void *ptr, string& result);
   uint32_t (*chain_fork_db_pending_head_block_num)(void *ptr);
   void (*chain_fork_db_pending_head_block_id)(void *ptr, string& result);
   void (*chain_fork_db_pending_head_block_time)(void *ptr, string& result);
   void (*chain_fork_db_pending_head_block_producer)(void *ptr, string& result);
   void (*chain_pending_block_time)(void *ptr, string& result);
   void (*chain_pending_block_producer)(void *ptr, string& result);
   void (*chain_pending_block_signing_key)(void *ptr, string& result);
   void (*chain_pending_producer_block_id)(void *ptr, string& result);
   void (*chain_get_pending_trx_receipts)(void *ptr, string& result);
   void (*chain_active_producers)(void *ptr, string& result);
   void (*chain_pending_producers)(void *ptr, string& result);
   void (*chain_proposed_producers)(void *ptr, string& result);
   uint32_t (*chain_last_irreversible_block_num)(void *ptr);
   void (*chain_last_irreversible_block_id)(void *ptr, string& result);
   void (*chain_fetch_block_by_number)(void *ptr, uint32_t block_num, string& raw_block );
   void (*chain_fetch_block_by_id)(void *ptr, string& params, string& raw_block );
   void (*chain_fetch_block_state_by_number)(void *ptr, uint32_t block_num, string& raw_block_state );
   void (*chain_fetch_block_state_by_id)(void *ptr, string& params, string& raw_block_state );
   void (*chain_get_block_id_for_num)(void *ptr, uint32_t block_num, string& result );
   void (*chain_calculate_integrity_hash)(void *ptr, string& result );
   bool (*chain_sender_avoids_whitelist_blacklist_enforcement)(void *ptr, string& sender );
   bool (*chain_check_actor_list)(void *ptr, string& param, string& err);
   bool (*chain_check_contract_list)(void *ptr, string& param, string& err);
   bool (*chain_check_action_list)(void *ptr, string& code, string& action, string& err);
   bool (*chain_check_key_list)(void *ptr, string& param, string& err);
   bool (*chain_is_building_block)(void *ptr);
   bool (*chain_is_producing_block)(void *ptr);
   bool (*chain_is_ram_billing_in_notify_allowed)(void *ptr);
   void (*chain_add_resource_greylist)(void *ptr, string& param);
   void (*chain_remove_resource_greylist)(void *ptr, string& param);
   bool (*chain_is_resource_greylisted)(void *ptr, string& param);
   void (*chain_get_resource_greylist)(void *ptr, string& result);
   void (*chain_get_config)(void *ptr, string& result);
   bool (*chain_validate_expiration)(void *ptr, string& param, string& err);
   bool (*chain_validate_tapos)(void *ptr, string& param, string& err);
   bool (*chain_validate_db_available_size)(void *ptr, string& err);
   bool (*chain_validate_reversible_available_size)(void *ptr, string& err);
   bool (*chain_is_protocol_feature_activated)(void *ptr, string& param);
   bool (*chain_is_builtin_activated)(void *ptr, int feature);
   bool (*chain_is_known_unexpired_transaction)(void *ptr, string& param);
   int64_t (*chain_set_proposed_producers)(void *ptr, string& param);
   bool (*chain_light_validation_allowed)(void *ptr, bool replay_opts_disabled_by_policy);
   bool (*chain_skip_auth_check)(void *ptr);
   bool (*chain_skip_db_sessions)(void *ptr);

   bool (*chain_skip_trx_checks)(void *ptr);
   bool (*chain_contracts_console)(void *ptr);
   bool (*chain_is_uuos_mainnet)(void *ptr);

   void (*chain_get_chain_id)(void *ptr, string& result);
   int (*chain_get_read_mode)(void *ptr);
   int (*chain_get_validation_mode)(void *ptr);
   void (*chain_set_subjective_cpu_leeway)(void *ptr, uint64_t leeway);
   void (*chain_set_greylist_limit)(void *ptr, uint32_t limit);
   uint32_t (*chain_get_greylist_limit)(void *ptr);
   void (*chain_add_to_ram_correction)(void *ptr, string& account, uint64_t ram_bytes);
   bool (*chain_all_subjective_mitigations_disabled)(void *ptr);
   void (*chain_get_scheduled_producer)(void *ptr, string& _block_time, string& result);
   void* (*chain_get_db_interface)(void *ptr);

   void (*pack_native_object)(int type, string& msg, string& packed_message);
   void (*unpack_native_object)(int type, string& packed_message, string& msg);

   int (*db_interface_get_i64)(void *ptr, int itr, string& buffer );

   int (*db_interface_next_i64)(void *ptr, int itr, uint64_t* primary );
   int (*db_interface_previous_i64)(void *ptr, int itr, uint64_t* primary );

   int (*db_interface_find_i64)(void *ptr, uint64_t code, uint64_t scope, uint64_t table, uint64_t id );
   void (*db_interface_remove_i64)(void *ptr,int itr);
   int (*db_interface_lowerbound_i64)(void *ptr, uint64_t code, uint64_t scope, uint64_t table, uint64_t id );
   int (*db_interface_upperbound_i64)(void *ptr, uint64_t code, uint64_t scope, uint64_t table, uint64_t id );
   int (*db_interface_end_i64)(void *ptr, uint64_t code, uint64_t scope, uint64_t table );

   bool (*db_interface_is_account)(void *ptr, uint64_t account);
   const char* (*db_interface_get_code_ex)(void *ptr, uint64_t receiver, size_t* size);

   int (*chain_api_get_info)(void *ptr, string& result);
   int (*chain_api_get_activated_protocol_features)(void *ptr, string& params, string& result);
   int (*chain_api_get_block)(void *ptr, string& params, string& result);
   int (*chain_api_get_block_header_state)(void *ptr, string& params, string& result);
   int (*chain_api_get_account)(void *ptr, string& params, string& result);
   int (*chain_api_get_code)(void *ptr, string& params, string& result);
   int (*chain_api_get_code_hash)(void *ptr, string& params, string& result);
   int (*chain_api_get_abi)(void *ptr, string& params, string& result);
   int (*chain_api_get_raw_code_and_abi)(void *ptr, string& params, string& result);
   int (*chain_api_get_raw_abi)(void *ptr, string& params, string& result);
   int (*chain_api_get_table_rows)(void *ptr, string& params, string& result);
   int (*chain_api_get_table_by_scope)(void *ptr, string& params, string& result);
   int (*chain_api_get_currency_balance)(void *ptr, string& params, string& result);
   int (*chain_api_get_currency_stats)(void *ptr, string& params, string& result);
   int (*chain_api_get_producers)(void *ptr, string& params, string& result);
   int (*chain_api_get_producer_schedule)(void *ptr, string& params, string& result);
   int (*chain_api_get_scheduled_transactions)(void *ptr, string& params, string& result);
   int (*chain_api_abi_json_to_bin)(void *ptr, string& params, string& result);
   int (*chain_api_abi_bin_to_json)(void *ptr, string& params, string& result);
   int (*chain_api_get_required_keys)(void *ptr, string& params, string& result);
   int (*chain_api_get_transaction_id)(void *ptr, string& params, string& result);

   void* (*history_new)(void *ptr, string& cfg);
   void (*history_free)(void *ptr);
   void (*history_get_actions)(void *ptr, const string& param, string& result);
   void (*history_get_transaction)(void *ptr, const string& param, string& result);
   void (*history_get_key_accounts)(void *ptr, const string& param, string& result);
   void (*history_get_key_accounts_ex)(void *ptr, const string& param, string& result);
   void (*history_get_controlled_accounts)(void *ptr, const string& param, string& result);

};

extern "C" void register_chain_api(struct chain_api_cpp* api);
extern "C" struct chain_api_cpp* get_chain_api();
extern "C" [[ noreturn ]] void chain_throw_exception(int type, const char* fmt, ...);

#endif /* CHAIN_API_HPP_ */
