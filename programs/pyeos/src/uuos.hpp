#include <stdint.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <chain_api.hpp>

extern "C" void say_hello_();

chain_api_cpp* get_chain_api();

#define pack_native_object_ get_chain_api()->pack_native_object
#define unpack_native_object_ get_chain_api()->unpack_native_object

#define chain_get_current_ptr_ get_chain_api()->chain_get_current_ptr
#define chain_set_current_ptr get_chain_api()->chain_set_current_ptr

#define chain_new_ get_chain_api()->chain_new
#define chain_startup_ get_chain_api()->chain_startup
#define chain_free_ get_chain_api()->chain_free
#define chain_id_ get_chain_api()->chain_id
#define chain_start_block_ get_chain_api()->chain_start_block
#define chain_abort_block_ get_chain_api()->chain_abort_block
#define chain_get_preactivated_protocol_features_ get_chain_api()->chain_get_preactivated_protocol_features
#define chain_get_unapplied_transactions_ get_chain_api()->chain_get_unapplied_transactions
#define chain_pack_action_args_ get_chain_api()->chain_pack_action_args
#define chain_unpack_action_args_ get_chain_api()->chain_unpack_action_args
#define chain_gen_transaction_ get_chain_api()->chain_gen_transaction
#define chain_push_transaction_ get_chain_api()->chain_push_transaction
#define chain_push_scheduled_transaction_ get_chain_api()->chain_push_scheduled_transaction
#define chain_commit_block_ get_chain_api()->chain_commit_block
#define chain_finalize_block_ get_chain_api()->chain_finalize_block
#define chain_pop_block_ get_chain_api()->chain_pop_block
#define chain_get_account_ get_chain_api()->chain_get_account
#define chain_get_global_properties_ get_chain_api()->chain_get_global_properties
#define chain_get_dynamic_global_properties_ get_chain_api()->chain_get_dynamic_global_properties
#define chain_get_actor_whitelist_ get_chain_api()->chain_get_actor_whitelist
#define chain_get_actor_blacklist_ get_chain_api()->chain_get_actor_blacklist
#define chain_get_contract_whitelist_ get_chain_api()->chain_get_contract_whitelist
#define chain_get_contract_blacklist_ get_chain_api()->chain_get_contract_blacklist
#define chain_get_action_blacklist_ get_chain_api()->chain_get_action_blacklist
#define chain_get_key_blacklist_ get_chain_api()->chain_get_key_blacklist
#define chain_set_actor_whitelist_ get_chain_api()->chain_set_actor_whitelist
#define chain_set_actor_blacklist_ get_chain_api()->chain_set_actor_blacklist
#define chain_set_contract_whitelist_ get_chain_api()->chain_set_contract_whitelist
#define chain_set_action_blacklist_ get_chain_api()->chain_set_action_blacklist
#define chain_set_key_blacklist_ get_chain_api()->chain_set_key_blacklist
#define chain_head_block_num_ get_chain_api()->chain_head_block_num
#define chain_head_block_time_ get_chain_api()->chain_head_block_time
#define chain_head_block_id_ get_chain_api()->chain_head_block_id
#define chain_head_block_producer_ get_chain_api()->chain_head_block_producer
#define chain_head_block_header_ get_chain_api()->chain_head_block_header
#define chain_head_block_state_ get_chain_api()->chain_head_block_state
#define chain_fork_db_head_block_num_ get_chain_api()->chain_fork_db_head_block_num
#define chain_fork_db_head_block_id_ get_chain_api()->chain_fork_db_head_block_id
#define chain_fork_db_head_block_time_ get_chain_api()->chain_fork_db_head_block_time
#define chain_fork_db_head_block_producer_ get_chain_api()->chain_fork_db_head_block_producer
#define chain_fork_db_pending_head_block_num_ get_chain_api()->chain_fork_db_pending_head_block_num
#define chain_fork_db_pending_head_block_id_ get_chain_api()->chain_fork_db_pending_head_block_id
#define chain_fork_db_pending_head_block_time_ get_chain_api()->chain_fork_db_pending_head_block_time
#define chain_fork_db_pending_head_block_producer_ get_chain_api()->chain_fork_db_pending_head_block_producer
#define chain_pending_block_time_ get_chain_api()->chain_pending_block_time
#define chain_pending_block_producer_ get_chain_api()->chain_pending_block_producer
#define chain_pending_block_signing_key_ get_chain_api()->chain_pending_block_signing_key
#define chain_pending_producer_block_id_ get_chain_api()->chain_pending_producer_block_id
#define chain_get_pending_trx_receipts_ get_chain_api()->chain_get_pending_trx_receipts
#define chain_active_producers_ get_chain_api()->chain_active_producers
#define chain_pending_producers_ get_chain_api()->chain_pending_producers
#define chain_proposed_producers_ get_chain_api()->chain_proposed_producers
#define chain_last_irreversible_block_num_ get_chain_api()->chain_last_irreversible_block_num
#define chain_last_irreversible_block_id_ get_chain_api()->chain_last_irreversible_block_id
#define chain_fetch_block_by_number_ get_chain_api()->chain_fetch_block_by_number
#define chain_fetch_block_by_id_ get_chain_api()->chain_fetch_block_by_id
#define chain_fetch_block_state_by_number_ get_chain_api()->chain_fetch_block_state_by_number
#define chain_fetch_block_state_by_id_ get_chain_api()->chain_fetch_block_state_by_id
#define chain_get_block_id_for_num_ get_chain_api()->chain_get_block_id_for_num
#define chain_calculate_integrity_hash_ get_chain_api()->chain_calculate_integrity_hash
#define chain_sender_avoids_whitelist_blacklist_enforcement_ get_chain_api()->chain_sender_avoids_whitelist_blacklist_enforcement
#define chain_check_actor_list_ get_chain_api()->chain_check_actor_list
#define chain_check_contract_list_ get_chain_api()->chain_check_contract_list
#define chain_check_action_list_ get_chain_api()->chain_check_action_list
#define chain_check_key_list_ get_chain_api()->chain_check_key_list
#define chain_is_building_block_ get_chain_api()->chain_is_building_block
#define chain_is_producing_block_ get_chain_api()->chain_is_producing_block
#define chain_is_ram_billing_in_notify_allowed_ get_chain_api()->chain_is_ram_billing_in_notify_allowed
#define chain_add_resource_greylist_ get_chain_api()->chain_add_resource_greylist
#define chain_remove_resource_greylist_ get_chain_api()->chain_remove_resource_greylist
#define chain_is_resource_greylisted_ get_chain_api()->chain_is_resource_greylisted
#define chain_get_resource_greylist_ get_chain_api()->chain_get_resource_greylist
#define chain_get_config_ get_chain_api()->chain_get_config
#define chain_validate_expiration_ get_chain_api()->chain_validate_expiration
#define chain_validate_tapos_ get_chain_api()->chain_validate_tapos
#define chain_validate_db_available_size_ get_chain_api()->chain_validate_db_available_size
#define chain_validate_reversible_available_size_ get_chain_api()->chain_validate_reversible_available_size
#define chain_is_protocol_feature_activated_ get_chain_api()->chain_is_protocol_feature_activated
#define chain_is_builtin_activated_ get_chain_api()->chain_is_builtin_activated
#define chain_is_known_unexpired_transaction_ get_chain_api()->chain_is_known_unexpired_transaction
#define chain_set_proposed_producers_ get_chain_api()->chain_set_proposed_producers
#define chain_light_validation_allowed_ get_chain_api()->chain_light_validation_allowed
#define chain_skip_auth_check_ get_chain_api()->chain_skip_auth_check
#define chain_skip_db_sessions_ get_chain_api()->chain_skip_db_sessions
#define chain_skip_trx_checks_ get_chain_api()->chain_skip_trx_checks
#define chain_contracts_console_ get_chain_api()->chain_contracts_console
#define chain_is_uuos_mainnet_ get_chain_api()->chain_is_uuos_mainnet
#define chain_get_chain_id_ get_chain_api()->chain_get_chain_id
#define chain_get_read_mode_ get_chain_api()->chain_get_read_mode
#define chain_get_validation_mode_ get_chain_api()->chain_get_validation_mode
#define chain_set_subjective_cpu_leeway_ get_chain_api()->chain_set_subjective_cpu_leeway
#define chain_set_greylist_limit_ get_chain_api()->chain_set_greylist_limit
#define chain_get_greylist_limit_ get_chain_api()->chain_get_greylist_limit
#define chain_add_to_ram_correction_ get_chain_api()->chain_add_to_ram_correction
#define chain_all_subjective_mitigations_disabled_ get_chain_api()->chain_all_subjective_mitigations_disabled
#define chain_get_scheduled_producer_ get_chain_api()->chain_get_scheduled_producer
#define chain_get_db_interface_ get_chain_api()->chain_get_db_interface

#define db_interface_get_i64_ get_chain_api()->db_interface_get_i64
#define db_interface_next_i64_ get_chain_api()->db_interface_next_i64
#define db_interface_previous_i64_ get_chain_api()->db_interface_previous_i64
#define db_interface_find_i64_ get_chain_api()->db_interface_find_i64
#define db_interface_remove_i64_ get_chain_api()->db_interface_remove_i64
#define db_interface_lowerbound_i64_ get_chain_api()->db_interface_lowerbound_i64
#define db_interface_upperbound_i64_ get_chain_api()->db_interface_upperbound_i64
#define db_interface_end_i64_ get_chain_api()->db_interface_end_i64

#define uuos_set_log_level_ get_chain_api()->uuos_set_log_level
#define uuos_set_version_ get_chain_api()->uuos_set_version
#define uuos_set_default_data_dir_ get_chain_api()->uuos_set_default_data_dir
#define uuos_set_default_config_dir_ get_chain_api()->uuos_set_default_config_dir
#define uuos_shutdown_ get_chain_api()->uuos_shutdown

//+++++++++++++++++chain api++++++++++++++
#define chain_api_get_info_ get_chain_api()->chain_api_get_info
#define chain_api_get_activated_protocol_features_ get_chain_api()->chain_api_get_activated_protocol_features
#define chain_api_get_block_ get_chain_api()->chain_api_get_block
#define chain_api_get_block_header_state_ get_chain_api()->chain_api_get_block_header_state
#define chain_api_get_account_ get_chain_api()->chain_api_get_account
#define chain_api_get_code_ get_chain_api()->chain_api_get_code
#define chain_api_get_code_hash_ get_chain_api()->chain_api_get_code_hash
#define chain_api_get_abi_ get_chain_api()->chain_api_get_abi
#define chain_api_get_raw_code_and_abi_ get_chain_api()->chain_api_get_raw_code_and_abi
#define chain_api_get_raw_abi_ get_chain_api()->chain_api_get_raw_abi
#define chain_api_get_table_rows_ get_chain_api()->chain_api_get_table_rows
#define chain_api_get_table_by_scope_ get_chain_api()->chain_api_get_table_by_scope
#define chain_api_get_currency_balance_ get_chain_api()->chain_api_get_currency_balance
#define chain_api_get_currency_stats_ get_chain_api()->chain_api_get_currency_stats
#define chain_api_get_producers_ get_chain_api()->chain_api_get_producers
#define chain_api_get_producer_schedule_ get_chain_api()->chain_api_get_producer_schedule

#define chain_api_get_scheduled_transactions_ get_chain_api()->chain_api_get_scheduled_transactions
#define chain_api_abi_json_to_bin_ get_chain_api()->chain_api_abi_json_to_bin
#define chain_api_abi_bin_to_json_ get_chain_api()->chain_api_abi_bin_to_json
#define chain_api_get_required_keys_ get_chain_api()->chain_api_get_required_keys
#define chain_api_get_transaction_id_ get_chain_api()->chain_api_get_transaction_id
