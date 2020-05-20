#include <stdint.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <chain_api.hpp>

extern "C" void say_hello_();

chain_api_cpp* get_chain_api();

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

