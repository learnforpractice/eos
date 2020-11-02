#ifndef __VM_API_WRAP_H__
#define __VM_API_WRAP_H__

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

enum vm_api_function_type {
   enum_read_action_data,
   enum_action_data_size,
   enum_require_recipient,
   enum_require_auth,
   enum_require_auth2,
   enum_has_auth,
   enum_is_account,
   enum_send_inline,
   enum_send_context_free_inline,
   enum_publication_time,
   enum_current_receiver,
   enum_get_active_producers,

   enum_assert_sha256,
   enum_assert_sha1,
   enum_assert_sha512,
   enum_assert_ripemd160,
   enum_assert_recover_key,

   enum_sha256,
   enum_sha1,
   enum_sha512,
   enum_ripemd160,
   enum_recover_key,
   enum_sha3,
   enum_db_store_i64,
   enum_db_update_i64,
   enum_db_remove_i64,
   enum_db_get_i64,
   enum_db_next_i64,
   enum_db_previous_i64,
   enum_db_find_i64,
   enum_db_lowerbound_i64,
   enum_db_upperbound_i64,
   enum_db_end_i64,
   enum_db_store_i256,
   enum_db_update_i256,
   enum_db_remove_i256,
   enum_db_get_i256,
   enum_db_find_i256,
   enum_db_previous_i256,
   enum_db_next_i256,
   enum_db_upperbound_i256,
   enum_db_lowerbound_i256,
   enum_db_end_i256,
   enum_db_get_table_count,
   enum_db_idx64_store,
   enum_db_idx64_update,
   enum_db_idx64_remove,
   enum_db_idx64_next,
   enum_db_idx64_previous,
   enum_db_idx64_find_primary,
   enum_db_idx64_find_secondary,
   enum_db_idx64_lowerbound,
   enum_db_idx64_upperbound,
   enum_db_idx64_end,
   enum_db_idx128_store,
   enum_db_idx128_update,
   enum_db_idx128_remove,
   enum_db_idx128_next,
   enum_db_idx128_previous,
   enum_db_idx128_find_primary,
   enum_db_idx128_find_secondary,
   enum_db_idx128_lowerbound,
   enum_db_idx128_upperbound,
   enum_db_idx128_end,
   enum_db_idx256_store,
   enum_db_idx256_update,
   enum_db_idx256_remove,
   enum_db_idx256_next,
   enum_db_idx256_previous,
   enum_db_idx256_find_primary,
   enum_db_idx256_find_secondary,
   enum_db_idx256_lowerbound,
   enum_db_idx256_upperbound,
   enum_db_idx256_end,
   enum_db_idx_double_store,
   enum_db_idx_double_update,
   enum_db_idx_double_remove,
   enum_db_idx_double_next,
   enum_db_idx_double_previous,
   enum_db_idx_double_find_primary,
   enum_db_idx_double_find_secondary,
   enum_db_idx_double_lowerbound,
   enum_db_idx_double_upperbound,
   enum_db_idx_double_end,
   enum_db_idx_long_double_store,
   enum_db_idx_long_double_update,
   enum_db_idx_long_double_remove,
   enum_db_idx_long_double_next,
   enum_db_idx_long_double_previous,
   enum_db_idx_long_double_find_primary,
   enum_db_idx_long_double_find_secondary,
   enum_db_idx_long_double_lowerbound,
   enum_db_idx_long_double_upperbound,
   enum_db_idx_long_double_end,


   enum_check_transaction_authorization,
   enum_check_permission_authorization,
   enum_get_permission_last_used,
   enum_get_account_creation_time,

   enum_prints,
   enum_prints_l,
   enum_printi,
   enum_printui,
   enum_printi128,
   enum_printui128,
   enum_printsf,
   enum_printdf,
   enum_printqf,

   enum_printn,
   enum_printhex,

   enum_set_resource_limits,
   enum_get_resource_limits,
   enum_set_proposed_producers,
   enum_set_proposed_producers_ex,
   enum_is_privileged,
   enum_set_privileged,
   enum_set_blockchain_parameters_packed,
   enum_get_blockchain_parameters_packed,
   enum_activate_feature,

   enum_eosio_abort,
   enum_eosio_assert,
   enum_eosio_assert_message,
   enum_eosio_assert_code,
   enum_eosio_exit,
   enum_current_time,
   enum_now,
   enum_checktime,
   enum_check_context_free,

   enum_send_deferred,
   enum_cancel_deferred,

   enum_read_transaction,
   enum_transaction_size,
   enum_tapos_block_num,
   enum_tapos_block_prefix,
   enum_expiration,
   enum_get_action,

   enum_assert_privileged,
   enum_assert_context_free,
   enum_get_context_free_data,

   enum_token_create,
   enum_token_issue,
   enum_token_transfer,
   enum_token_open,
   enum_token_retire,
   enum_token_close,

   enum_call_contract_get_args,
   enum_call_contract_set_results,
   enum_call_contract_get_results,
   enum_call_contract,

   enum_is_feature_activated,
   enum_preactivate_feature,
   enum_get_sender,
   enum_call_native,
   enum_vm_api_function_count
};

struct vm_api_arg {
    uint32_t size;
    uint32_t type;
    union {
        uint64_t u64;
        int32_t i32;
        uint32_t u32;
        uint8_t u8;
        void *ptr;
    };
};

enum vm_api_arg_type {
   enum_arg_type_i8,
   enum_arg_type_u8,
   enum_arg_type_i32,
   enum_arg_type_u32,
   enum_arg_type_i64,
   enum_arg_type_u64,
   enum_arg_type_ptr,
};

#define MAX_VM_API_ARGS (10)

int call_vm_api(enum vm_api_function_type function_type,  struct vm_api_arg *args, size_t args_count, struct vm_api_arg *vm_ret);

#ifdef __cplusplus
}
#endif

#endif
