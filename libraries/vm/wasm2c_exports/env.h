#include <stdint.h>

#ifndef ENV_H_
#define ENV_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef float f32;
typedef double f64;

/* import: 'env' 'prints_l' */
extern void (*Z_envZ_prints_lZ_vii)(u32, u32);
/* import: 'env' 'prints' */
extern void (*Z_envZ_printsZ_vi)(u32);
/* import: 'env' 'memcpy' */
extern u32 (*Z_envZ_memcpyZ_iiii)(u32, u32, u32);
/* import: 'env' 'memset' */
extern u32 (*Z_envZ_memsetZ_iiii)(u32, u32, u32);
/* import: 'env' '__unordtf2' */
extern u32 (*Z_envZ___unordtf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__eqtf2' */
extern u32 (*Z_envZ___eqtf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__multf3' */
extern void (*Z_envZ___multf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__addtf3' */
extern void (*Z_envZ___addtf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__subtf3' */
extern void (*Z_envZ___subtf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__netf2' */
extern u32 (*Z_envZ___netf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__fixunstfsi' */
extern u32 (*Z_envZ___fixunstfsiZ_ijj)(u64, u64);
/* import: 'env' '__floatunsitf' */
extern void (*Z_envZ___floatunsitfZ_vii)(u32, u32);
/* import: 'env' '__fixtfsi' */
extern u32 (*Z_envZ___fixtfsiZ_ijj)(u64, u64);
/* import: 'env' '__floatsitf' */
extern void (*Z_envZ___floatsitfZ_vii)(u32, u32);
/* import: 'env' '__extenddftf2' */
extern void (*Z_envZ___extenddftf2Z_vid)(u32, f64);
/* import: 'env' 'memmove' */
extern u32 (*Z_envZ_memmoveZ_iiii)(u32, u32, u32);
/* import: 'env' 'abort' */
extern void (*Z_envZ_abortZ_vv)(void);
/* import: 'env' 'read_action_data' */
extern u32 (*Z_envZ_read_action_dataZ_iii)(u32, u32);
/* import: 'env' 'action_data_size' */
extern u32 (*Z_envZ_action_data_sizeZ_iv)(void);
/* import: 'env' 'require_recipient' */
extern void (*Z_envZ_require_recipientZ_vj)(u64);
/* import: 'env' 'require_auth' */
extern void (*Z_envZ_require_authZ_vj)(u64);
/* import: 'env' 'require_auth2' */
extern void (*Z_envZ_require_auth2Z_vjj)(u64, u64);
/* import: 'env' 'has_auth' */
extern u32 (*Z_envZ_has_authZ_ij)(u64);
/* import: 'env' 'is_account' */
extern u32 (*Z_envZ_is_accountZ_ij)(u64);
/* import: 'env' 'send_context_free_inline' */
extern void (*Z_envZ_send_context_free_inlineZ_vii)(u32, u32);
/* import: 'env' 'publication_time' */
extern u64 (*Z_envZ_publication_timeZ_jv)(void);
/* import: 'env' 'current_receiver' */
extern u64 (*Z_envZ_current_receiverZ_jv)(void);
/* import: 'env' 's2n' */
extern u64 (*Z_envZ_s2nZ_jii)(u32, u32);
/* import: 'env' 'n2s' */
extern u32 (*Z_envZ_n2sZ_ijii)(u64, u32, u32);
/* import: 'env' 'call_contract' */
extern void (*Z_envZ_call_contractZ_vjii)(u64, u32, u32);
/* import: 'env' 'call_contract_get_results' */
extern u32 (*Z_envZ_call_contract_get_resultsZ_iii)(u32, u32);
/* import: 'env' 'get_active_producers' */
extern u32 (*Z_envZ_get_active_producersZ_iii)(u32, u32);
/* import: 'env' 'assert_sha1' */
extern void (*Z_envZ_assert_sha1Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_sha256' */
extern void (*Z_envZ_assert_sha256Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_sha512' */
extern void (*Z_envZ_assert_sha512Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_ripemd160' */
extern void (*Z_envZ_assert_ripemd160Z_viii)(u32, u32, u32);
/* import: 'env' 'sha1' */
extern void (*Z_envZ_sha1Z_viii)(u32, u32, u32);
/* import: 'env' 'sha256' */
extern void (*Z_envZ_sha256Z_viii)(u32, u32, u32);
/* import: 'env' 'sha512' */
extern void (*Z_envZ_sha512Z_viii)(u32, u32, u32);
/* import: 'env' 'ripemd160' */
extern void (*Z_envZ_ripemd160Z_viii)(u32, u32, u32);
/* import: 'env' 'recover_key' */
extern u32 (*Z_envZ_recover_keyZ_iiiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'assert_recover_key' */
extern void (*Z_envZ_assert_recover_keyZ_viiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'check_transaction_authorization' */
extern u32 (*Z_envZ_check_transaction_authorizationZ_iiiiiii)(u32, u32, u32, u32, u32, u32);
/* import: 'env' 'check_permission_authorization' */
extern u32 (*Z_envZ_check_permission_authorizationZ_ijjiiiij)(u64, u64, u32, u32, u32, u32, u64);
/* import: 'env' 'get_permission_last_used' */
extern u64 (*Z_envZ_get_permission_last_usedZ_jjj)(u64, u64);
/* import: 'env' 'get_account_creation_time' */
extern u64 (*Z_envZ_get_account_creation_timeZ_jj)(u64);
/* import: 'env' 'get_resource_limits' */
extern void (*Z_envZ_get_resource_limitsZ_vjiii)(u64, u32, u32, u32);
/* import: 'env' 'set_resource_limits' */
extern void (*Z_envZ_set_resource_limitsZ_vjjjj)(u64, u64, u64, u64);
/* import: 'env' 'set_proposed_producers' */
extern u64 (*Z_envZ_set_proposed_producersZ_jii)(u32, u32);
/* import: 'env' 'is_privileged' */
extern u32 (*Z_envZ_is_privilegedZ_ij)(u64);
/* import: 'env' 'set_privileged' */
extern void (*Z_envZ_set_privilegedZ_vji)(u64, u32);
/* import: 'env' 'set_blockchain_parameters_packed' */
extern void (*Z_envZ_set_blockchain_parameters_packedZ_vii)(u32, u32);
/* import: 'env' 'get_blockchain_parameters_packed' */
extern u32 (*Z_envZ_get_blockchain_parameters_packedZ_iii)(u32, u32);
/* import: 'env' 'activate_feature' */
extern void (*Z_envZ_activate_featureZ_vj)(u64);
/* import: 'env' 'current_time' */
extern u64 (*Z_envZ_current_timeZ_jv)(void);
/* import: 'env' 'eosio_assert' */
extern void (*Z_envZ_eosio_assertZ_vii)(u32, u32);
/* import: 'env' 'eosio_assert_message' */
extern void (*Z_envZ_eosio_assert_messageZ_viii)(u32, u32, u32);
/* import: 'env' 'eosio_assert_code' */
extern void (*Z_envZ_eosio_assert_codeZ_vij)(u32, u64);
/* import: 'env' 'token_create' */
extern void (*Z_envZ_token_createZ_vjjj)(u64, u64, u64);
/* import: 'env' 'token_issue' */
extern void (*Z_envZ_token_issueZ_vjjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'token_transfer' */
extern void (*Z_envZ_token_transferZ_vjjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'token_open' */
extern void (*Z_envZ_token_openZ_vjjj)(u64, u64, u64);
/* import: 'env' 'token_retire' */
extern void (*Z_envZ_token_retireZ_vjjii)(u64, u64, u32, u32);
/* import: 'env' 'token_close' */
extern void (*Z_envZ_token_closeZ_vjj)(u64, u64);
/* import: 'env' 'read_transaction' */
extern u32 (*Z_envZ_read_transactionZ_iii)(u32, u32);
/* import: 'env' 'transaction_size' */
extern u32 (*Z_envZ_transaction_sizeZ_iv)(void);
/* import: 'env' 'tapos_block_num' */
extern u32 (*Z_envZ_tapos_block_numZ_iv)(void);
/* import: 'env' 'tapos_block_prefix' */
extern u32 (*Z_envZ_tapos_block_prefixZ_iv)(void);
/* import: 'env' 'expiration' */
extern u32 (*Z_envZ_expirationZ_iv)(void);
/* import: 'env' 'get_action' */
extern u32 (*Z_envZ_get_actionZ_iiiii)(u32, u32, u32, u32);
/* import: 'env' 'get_context_free_data' */
extern u32 (*Z_envZ_get_context_free_dataZ_iiii)(u32, u32, u32);
/* import: 'env' 'printi' */
extern void (*Z_envZ_printiZ_vj)(u64);
/* import: 'env' 'find_frozen_code' */
extern u32 (*Z_envZ_find_frozen_codeZ_iiiii)(u32, u32, u32, u32);
/* import: 'env' 'get_code_size' */
extern u32 (*Z_envZ_get_code_sizeZ_ij)(u64);
/* import: 'env' 'get_code' */
extern u32 (*Z_envZ_get_codeZ_ijii)(u64, u32, u32);
/* import: 'env' 'set_copy_memory_range' */
extern void (*Z_envZ_set_copy_memory_rangeZ_vii)(u32, u32);
/* import: 'env' 'send_inline' */
extern void (*Z_envZ_send_inlineZ_vii)(u32, u32);
/* import: 'env' 'send_deferred' */
extern void (*Z_envZ_send_deferredZ_vijiii)(u32, u64, u32, u32, u32);
/* import: 'env' 'cancel_deferred' */
extern u32 (*Z_envZ_cancel_deferredZ_ii)(u32);
/* import: 'env' 'db_store_i64' */
extern u32 (*Z_envZ_db_store_i64Z_ijjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'db_update_i64' */
extern void (*Z_envZ_db_update_i64Z_vijii)(u32, u64, u32, u32);
/* import: 'env' 'db_find_i64' */
extern u32 (*Z_envZ_db_find_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_get_i64' */
extern u32 (*Z_envZ_db_get_i64Z_iiii)(u32, u32, u32);
/* import: 'env' 'db_remove_i64' */
extern void (*Z_envZ_db_remove_i64Z_vi)(u32);
/* import: 'env' 'db_next_i64' */
extern u32 (*Z_envZ_db_next_i64Z_iii)(u32, u32);
/* import: 'env' 'db_previous_i64' */
extern u32 (*Z_envZ_db_previous_i64Z_iii)(u32, u32);
/* import: 'env' 'db_upperbound_i64' */
extern u32 (*Z_envZ_db_upperbound_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_lowerbound_i64' */
extern u32 (*Z_envZ_db_lowerbound_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_end_i64' */
extern u32 (*Z_envZ_db_end_i64Z_ijjj)(u64, u64, u64);
/* import: 'env' 'db_get_table_count' */
extern u32 (*Z_envZ_db_get_table_countZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx64_store' */
extern u32 (*Z_envZ_db_idx64_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx64_update' */
extern void (*Z_envZ_db_idx64_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx64_remove' */
extern void (*Z_envZ_db_idx64_removeZ_vi)(u32);
/* import: 'env' 'db_idx64_next' */
extern u32 (*Z_envZ_db_idx64_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx64_previous' */
extern u32 (*Z_envZ_db_idx64_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx64_find_primary' */
extern u32 (*Z_envZ_db_idx64_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx64_find_secondary' */
extern u32 (*Z_envZ_db_idx64_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx64_lowerbound' */
extern u32 (*Z_envZ_db_idx64_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx64_upperbound' */
extern u32 (*Z_envZ_db_idx64_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx64_end' */
extern u32 (*Z_envZ_db_idx64_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx128_store' */
extern u32 (*Z_envZ_db_idx128_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx128_update' */
extern void (*Z_envZ_db_idx128_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx128_remove' */
extern void (*Z_envZ_db_idx128_removeZ_vi)(u32);
/* import: 'env' 'db_idx128_next' */
extern u32 (*Z_envZ_db_idx128_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx128_previous' */
extern u32 (*Z_envZ_db_idx128_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx128_find_primary' */
extern u32 (*Z_envZ_db_idx128_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx128_find_secondary' */
extern u32 (*Z_envZ_db_idx128_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx128_lowerbound' */
extern u32 (*Z_envZ_db_idx128_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx128_upperbound' */
extern u32 (*Z_envZ_db_idx128_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx128_end' */
extern u32 (*Z_envZ_db_idx128_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx256_update' */
extern void (*Z_envZ_db_idx256_updateZ_vijii)(u32, u64, u32, u32);
/* import: 'env' 'db_idx256_remove' */
extern void (*Z_envZ_db_idx256_removeZ_vi)(u32);
/* import: 'env' 'db_idx256_next' */
extern u32 (*Z_envZ_db_idx256_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx256_previous' */
extern u32 (*Z_envZ_db_idx256_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx256_find_primary' */
extern u32 (*Z_envZ_db_idx256_find_primaryZ_ijjjiij)(u64, u64, u64, u32, u32, u64);
/* import: 'env' 'db_idx256_find_secondary' */
extern u32 (*Z_envZ_db_idx256_find_secondaryZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx256_lowerbound' */
extern u32 (*Z_envZ_db_idx256_lowerboundZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx256_upperbound' */
extern u32 (*Z_envZ_db_idx256_upperboundZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx256_end' */
extern u32 (*Z_envZ_db_idx256_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx_double_store' */
extern u32 (*Z_envZ_db_idx_double_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx_double_update' */
extern void (*Z_envZ_db_idx_double_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx_double_remove' */
extern void (*Z_envZ_db_idx_double_removeZ_vi)(u32);
/* import: 'env' 'db_idx_double_next' */
extern u32 (*Z_envZ_db_idx_double_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx_double_previous' */
extern u32 (*Z_envZ_db_idx_double_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx_double_find_primary' */
extern u32 (*Z_envZ_db_idx_double_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx_double_find_secondary' */
extern u32 (*Z_envZ_db_idx_double_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_double_lowerbound' */
extern u32 (*Z_envZ_db_idx_double_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_double_upperbound' */
extern u32 (*Z_envZ_db_idx_double_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_double_end' */
extern u32 (*Z_envZ_db_idx_double_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx_long_double_store' */
extern u32 (*Z_envZ_db_idx_long_double_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx_long_double_update' */
extern void (*Z_envZ_db_idx_long_double_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx_long_double_remove' */
extern void (*Z_envZ_db_idx_long_double_removeZ_vi)(u32);
/* import: 'env' 'db_idx_long_double_next' */
extern u32 (*Z_envZ_db_idx_long_double_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx_long_double_previous' */
extern u32 (*Z_envZ_db_idx_long_double_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx_long_double_find_primary' */
extern u32 (*Z_envZ_db_idx_long_double_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx_long_double_find_secondary' */
extern u32 (*Z_envZ_db_idx_long_double_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_lowerbound' */
extern u32 (*Z_envZ_db_idx_long_double_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_upperbound' */
extern u32 (*Z_envZ_db_idx_long_double_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_end' */
extern u32 (*Z_envZ_db_idx_long_double_endZ_ijjj)(u64, u64, u64);
/* import: 'env' '__divtf3' */
extern void (*Z_envZ___divtf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' 'db_idx256_store' */
extern u32 (*Z_envZ_db_idx256_storeZ_ijjjjii)(u64, u64, u64, u64, u32, u32);


extern u32 (*Z_envZ_db_get_countZ_ijjj)(u64, u64, u64);

extern u32 (*Z_envZ_db_get_table_countZ_ijjj)(u64, u64, u64);

extern u32 (*Z_envZ_db_find_i256Z_ijjjii)(u64, u64, u64, u32, u32);
extern void (*Z_envZ_db_update_i256Z_vijii)(u32, u64, u32, u32);
extern u32 (*Z_envZ_db_store_i256Z_ijjjiiii)(u64, u64, u64, u32, u32, u32, u32);
extern u32 (*Z_envZ_db_get_i256Z_iiii)(u32, u32, u32);
extern void (*Z_envZ_db_remove_i256Z_vi)(u32);


extern void (*Z_envZ_printnZ_vj)(u64);
extern void (*Z_envZ_printiZ_vj)(u64);
extern void (*Z_envZ_printuiZ_vj)(u64);


/* import: 'eosio_injection' 'checktime' */
extern void (*Z_eosio_injectionZ_checktimeZ_vv)(void);
/* import: 'eosio_injection' 'call_depth_assert' */
extern void (*Z_eosio_injectionZ_call_depth_assertZ_vv)(void);
/* import: 'eosio_injection' '_eosio_f64_sub' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_lt' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_ltZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_trunc' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_truncZ_dd)(f64);
/* import: 'eosio_injection' '_eosio_f64_mul' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_gt' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_gtZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_ge' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_geZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_trunc_i32u' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_trunc_i32uZ_id)(f64);
/* import: 'eosio_injection' '_eosio_f64_abs' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_absZ_dd)(f64);
/* import: 'eosio_injection' '_eosio_f64_trunc_i32s' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_trunc_i32sZ_id)(f64);
/* import: 'eosio_injection' '_eosio_f64_le' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_leZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_ne' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_neZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_eq' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_eqZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_add' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_div' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_neg' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(f64);
/* import: 'eosio_injection' '_eosio_f64_demote' */
extern f32 (*Z_eosio_injectionZ__eosio_f64_demoteZ_fd)(f64);
/* import: 'eosio_injection' '_eosio_f32_eq' */
extern u32 (*Z_eosio_injectionZ__eosio_f32_eqZ_iff)(f32, f32);
/* import: 'eosio_injection' '_eosio_f32_promote' */
extern f64 (*Z_eosio_injectionZ__eosio_f32_promoteZ_df)(f32);
/* import: 'eosio_injection' '_eosio_f32_div' */
extern f32 (*Z_eosio_injectionZ__eosio_f32_divZ_fff)(f32, f32);
/* import: 'eosio_injection' '_eosio_f32_gt' */
extern u32 (*Z_eosio_injectionZ__eosio_f32_gtZ_iff)(f32, f32);
/* import: 'eosio_injection' '_eosio_f32_ne' */
extern u32 (*Z_eosio_injectionZ__eosio_f32_neZ_iff)(f32, f32);
/* import: 'eosio_injection' '_eosio_f64_trunc_i64s' */
extern u64 (*Z_eosio_injectionZ__eosio_f64_trunc_i64sZ_jd)(f64);
/* import: 'eosio_injection' '_eosio_f32_le' */
extern u32 (*Z_eosio_injectionZ__eosio_f32_leZ_iff)(f32, f32);
/* import: 'eosio_injection' '_eosio_f64_max' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_maxZ_ddd)(f64, f64);

/* import: 'env' 'printhex' */
extern void (*Z_envZ_printhexZ_vii)(u32, u32);

/* import: 'env' 'call_native' */
extern u32 (*Z_envZ_call_nativeZ_iiiiiii)(u32, u32, u32, u32, u32, u32);

#ifdef __cplusplus
}
#endif

#endif

