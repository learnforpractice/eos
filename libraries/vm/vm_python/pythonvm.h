#ifndef PYTHONVM_H_GENERATED_
#define PYTHONVM_H_GENERATED_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "wasm-rt.h"
void *memcpy(void *dest, const void *src, unsigned long n);

#ifndef WASM_RT_MODULE_PREFIX
#define WASM_RT_MODULE_PREFIX
#endif

#define WASM_RT_PASTE_(x, y) x ## y
#define WASM_RT_PASTE(x, y) WASM_RT_PASTE_(x, y)
#define WASM_RT_ADD_PREFIX(x) WASM_RT_PASTE(WASM_RT_MODULE_PREFIX, x)

/* TODO(binji): only use stdint.h types in header */
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

extern void WASM_RT_ADD_PREFIX(init)(void);

/* import: 'eosio_injection' 'checktime' */
extern void (*Z_eosio_injectionZ_checktimeZ_vv)(void);
/* import: 'eosio_injection' '_eosio_f64_ne' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_neZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_div' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_mul' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_add' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_le' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_leZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_trunc_i32s' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_trunc_i32sZ_id)(f64);
/* import: 'eosio_injection' '_eosio_f64_gt' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_gtZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_eq' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_eqZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_sub' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_lt' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_ltZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_neg' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(f64);
/* import: 'eosio_injection' '_eosio_f64_ge' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_geZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_trunc_i32u' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_trunc_i32uZ_id)(f64);
/* import: 'eosio_injection' '_eosio_f64_demote' */
extern f32 (*Z_eosio_injectionZ__eosio_f64_demoteZ_fd)(f64);
/* import: 'eosio_injection' '_eosio_f32_promote' */
extern f64 (*Z_eosio_injectionZ__eosio_f32_promoteZ_df)(f32);
/* import: 'eosio_injection' '_eosio_f32_eq' */
extern u32 (*Z_eosio_injectionZ__eosio_f32_eqZ_iff)(f32, f32);
/* import: 'eosio_injection' '_eosio_f32_div' */
extern f32 (*Z_eosio_injectionZ__eosio_f32_divZ_fff)(f32, f32);
/* import: 'eosio_injection' '_eosio_f32_le' */
extern u32 (*Z_eosio_injectionZ__eosio_f32_leZ_iff)(f32, f32);
/* import: 'eosio_injection' '_eosio_f32_ne' */
extern u32 (*Z_eosio_injectionZ__eosio_f32_neZ_iff)(f32, f32);
/* import: 'eosio_injection' '_eosio_f64_trunc_i64s' */
extern u64 (*Z_eosio_injectionZ__eosio_f64_trunc_i64sZ_jd)(f64);
/* import: 'env' '__addtf3' */
extern void (*Z_envZ___addtf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__eqtf2' */
extern u32 (*Z_envZ___eqtf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__extenddftf2' */
extern void (*Z_envZ___extenddftf2Z_vid)(u32, f64);
/* import: 'env' '__fixtfsi' */
extern u32 (*Z_envZ___fixtfsiZ_ijj)(u64, u64);
/* import: 'env' '__fixunstfsi' */
extern u32 (*Z_envZ___fixunstfsiZ_ijj)(u64, u64);
/* import: 'env' '__floatsitf' */
extern void (*Z_envZ___floatsitfZ_vii)(u32, u32);
/* import: 'env' '__floatunsitf' */
extern void (*Z_envZ___floatunsitfZ_vii)(u32, u32);
/* import: 'env' '__multf3' */
extern void (*Z_envZ___multf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__netf2' */
extern u32 (*Z_envZ___netf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__subtf3' */
extern void (*Z_envZ___subtf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__unordtf2' */
extern u32 (*Z_envZ___unordtf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'action_data_size' */
extern u32 (*Z_envZ_action_data_sizeZ_iv)(void);
/* import: 'env' 'activate_feature' */
extern void (*Z_envZ_activate_featureZ_vj)(u64);
/* import: 'env' 'assert_recover_key' */
extern void (*Z_envZ_assert_recover_keyZ_viiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'assert_ripemd160' */
extern void (*Z_envZ_assert_ripemd160Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_sha1' */
extern void (*Z_envZ_assert_sha1Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_sha256' */
extern void (*Z_envZ_assert_sha256Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_sha512' */
extern void (*Z_envZ_assert_sha512Z_viii)(u32, u32, u32);
/* import: 'env' 'call_contract' */
extern void (*Z_envZ_call_contractZ_vjjjjjii)(u64, u64, u64, u64, u64, u32, u32);
/* import: 'env' 'call_contract_get_results' */
extern u32 (*Z_envZ_call_contract_get_resultsZ_iii)(u32, u32);
/* import: 'env' 'cancel_deferred' */
extern u32 (*Z_envZ_cancel_deferredZ_ii)(u32);
/* import: 'env' 'check_permission_authorization' */
extern u32 (*Z_envZ_check_permission_authorizationZ_ijjiiiij)(u64, u64, u32, u32, u32, u32, u64);
/* import: 'env' 'check_transaction_authorization' */
extern u32 (*Z_envZ_check_transaction_authorizationZ_iiiiiii)(u32, u32, u32, u32, u32, u32);
/* import: 'env' 'current_receiver' */
extern u64 (*Z_envZ_current_receiverZ_jv)(void);
/* import: 'env' 'current_time' */
extern u64 (*Z_envZ_current_timeZ_jv)(void);
/* import: 'env' 'db_end_i64' */
extern u32 (*Z_envZ_db_end_i64Z_ijjj)(u64, u64, u64);
/* import: 'env' 'db_find_i64' */
extern u32 (*Z_envZ_db_find_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_get_count' */
extern u32 (*Z_envZ_db_get_countZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_get_i64' */
extern u32 (*Z_envZ_db_get_i64Z_iiii)(u32, u32, u32);
/* import: 'env' 'db_idx128_end' */
extern u32 (*Z_envZ_db_idx128_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx128_find_primary' */
extern u32 (*Z_envZ_db_idx128_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx128_find_secondary' */
extern u32 (*Z_envZ_db_idx128_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx128_lowerbound' */
extern u32 (*Z_envZ_db_idx128_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx128_next' */
extern u32 (*Z_envZ_db_idx128_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx128_previous' */
extern u32 (*Z_envZ_db_idx128_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx128_remove' */
extern void (*Z_envZ_db_idx128_removeZ_vi)(u32);
/* import: 'env' 'db_idx128_store' */
extern u32 (*Z_envZ_db_idx128_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx128_update' */
extern void (*Z_envZ_db_idx128_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx128_upperbound' */
extern u32 (*Z_envZ_db_idx128_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx256_end' */
extern u32 (*Z_envZ_db_idx256_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx256_find_primary' */
extern u32 (*Z_envZ_db_idx256_find_primaryZ_ijjjiij)(u64, u64, u64, u32, u32, u64);
/* import: 'env' 'db_idx256_find_secondary' */
extern u32 (*Z_envZ_db_idx256_find_secondaryZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx256_lowerbound' */
extern u32 (*Z_envZ_db_idx256_lowerboundZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx256_next' */
extern u32 (*Z_envZ_db_idx256_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx256_previous' */
extern u32 (*Z_envZ_db_idx256_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx256_remove' */
extern void (*Z_envZ_db_idx256_removeZ_vi)(u32);
/* import: 'env' 'db_idx256_update' */
extern void (*Z_envZ_db_idx256_updateZ_vijii)(u32, u64, u32, u32);
/* import: 'env' 'db_idx256_upperbound' */
extern u32 (*Z_envZ_db_idx256_upperboundZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx64_end' */
extern u32 (*Z_envZ_db_idx64_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx64_find_primary' */
extern u32 (*Z_envZ_db_idx64_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx64_find_secondary' */
extern u32 (*Z_envZ_db_idx64_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx64_lowerbound' */
extern u32 (*Z_envZ_db_idx64_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx64_next' */
extern u32 (*Z_envZ_db_idx64_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx64_previous' */
extern u32 (*Z_envZ_db_idx64_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx64_remove' */
extern void (*Z_envZ_db_idx64_removeZ_vi)(u32);
/* import: 'env' 'db_idx64_store' */
extern u32 (*Z_envZ_db_idx64_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx64_update' */
extern void (*Z_envZ_db_idx64_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx64_upperbound' */
extern u32 (*Z_envZ_db_idx64_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_double_end' */
extern u32 (*Z_envZ_db_idx_double_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx_double_find_primary' */
extern u32 (*Z_envZ_db_idx_double_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx_double_find_secondary' */
extern u32 (*Z_envZ_db_idx_double_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_double_lowerbound' */
extern u32 (*Z_envZ_db_idx_double_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_double_next' */
extern u32 (*Z_envZ_db_idx_double_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx_double_previous' */
extern u32 (*Z_envZ_db_idx_double_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx_double_remove' */
extern void (*Z_envZ_db_idx_double_removeZ_vi)(u32);
/* import: 'env' 'db_idx_double_store' */
extern u32 (*Z_envZ_db_idx_double_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx_double_update' */
extern void (*Z_envZ_db_idx_double_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx_double_upperbound' */
extern u32 (*Z_envZ_db_idx_double_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_end' */
extern u32 (*Z_envZ_db_idx_long_double_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx_long_double_find_primary' */
extern u32 (*Z_envZ_db_idx_long_double_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx_long_double_find_secondary' */
extern u32 (*Z_envZ_db_idx_long_double_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_lowerbound' */
extern u32 (*Z_envZ_db_idx_long_double_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_next' */
extern u32 (*Z_envZ_db_idx_long_double_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx_long_double_previous' */
extern u32 (*Z_envZ_db_idx_long_double_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx_long_double_remove' */
extern void (*Z_envZ_db_idx_long_double_removeZ_vi)(u32);
/* import: 'env' 'db_idx_long_double_store' */
extern u32 (*Z_envZ_db_idx_long_double_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx_long_double_update' */
extern void (*Z_envZ_db_idx_long_double_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx_long_double_upperbound' */
extern u32 (*Z_envZ_db_idx_long_double_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_lowerbound_i64' */
extern u32 (*Z_envZ_db_lowerbound_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_next_i64' */
extern u32 (*Z_envZ_db_next_i64Z_iii)(u32, u32);
/* import: 'env' 'db_previous_i64' */
extern u32 (*Z_envZ_db_previous_i64Z_iii)(u32, u32);
/* import: 'env' 'db_remove_i64' */
extern void (*Z_envZ_db_remove_i64Z_vi)(u32);
/* import: 'env' 'db_store_i64' */
extern u32 (*Z_envZ_db_store_i64Z_ijjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'db_update_i64' */
extern void (*Z_envZ_db_update_i64Z_vijii)(u32, u64, u32, u32);
/* import: 'env' 'db_upperbound_i64' */
extern u32 (*Z_envZ_db_upperbound_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'eosio_assert' */
extern void (*Z_envZ_eosio_assertZ_vii)(u32, u32);
/* import: 'env' 'eosio_assert_code' */
extern void (*Z_envZ_eosio_assert_codeZ_vij)(u32, u64);
/* import: 'env' 'eosio_assert_message' */
extern void (*Z_envZ_eosio_assert_messageZ_viii)(u32, u32, u32);
/* import: 'env' 'expiration' */
extern u32 (*Z_envZ_expirationZ_iv)(void);
/* import: 'env' 'find_frozen_code' */
extern u32 (*Z_envZ_find_frozen_codeZ_iiiii)(u32, u32, u32, u32);
/* import: 'env' 'from_base58' */
extern u32 (*Z_envZ_from_base58Z_iiiii)(u32, u32, u32, u32);
/* import: 'env' 'get_account_creation_time' */
extern u64 (*Z_envZ_get_account_creation_timeZ_jj)(u64);
/* import: 'env' 'get_action' */
extern u32 (*Z_envZ_get_actionZ_iiiii)(u32, u32, u32, u32);
/* import: 'env' 'get_active_producers' */
extern u32 (*Z_envZ_get_active_producersZ_iii)(u32, u32);
/* import: 'env' 'get_blockchain_parameters_packed' */
extern u32 (*Z_envZ_get_blockchain_parameters_packedZ_iii)(u32, u32);
/* import: 'env' 'get_code' */
extern u32 (*Z_envZ_get_codeZ_ijii)(u64, u32, u32);
/* import: 'env' 'get_code_size' */
extern u32 (*Z_envZ_get_code_sizeZ_ij)(u64);
/* import: 'env' 'get_context_free_data' */
extern u32 (*Z_envZ_get_context_free_dataZ_iiii)(u32, u32, u32);
/* import: 'env' 'get_permission_last_used' */
extern u64 (*Z_envZ_get_permission_last_usedZ_jjj)(u64, u64);
/* import: 'env' 'get_resource_limits' */
extern void (*Z_envZ_get_resource_limitsZ_vjiii)(u64, u32, u32, u32);
/* import: 'env' 'has_auth' */
extern u32 (*Z_envZ_has_authZ_ij)(u64);
/* import: 'env' 'is_account' */
extern u32 (*Z_envZ_is_accountZ_ij)(u64);
/* import: 'env' 'is_privileged' */
extern u32 (*Z_envZ_is_privilegedZ_ij)(u64);
/* import: 'env' 'n2s' */
extern u32 (*Z_envZ_n2sZ_ijii)(u64, u32, u32);
/* import: 'env' 'printi' */
extern void (*Z_envZ_printiZ_vj)(u64);
/* import: 'env' 'prints' */
extern void (*Z_envZ_printsZ_vi)(u32);
/* import: 'env' 'publication_time' */
extern u64 (*Z_envZ_publication_timeZ_jv)(void);
/* import: 'env' 'read_action_data' */
extern u32 (*Z_envZ_read_action_dataZ_iii)(u32, u32);
/* import: 'env' 'read_transaction' */
extern u32 (*Z_envZ_read_transactionZ_iii)(u32, u32);
/* import: 'env' 'recover_key' */
extern u32 (*Z_envZ_recover_keyZ_iiiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'require_auth' */
extern void (*Z_envZ_require_authZ_vj)(u64);
/* import: 'env' 'require_auth2' */
extern void (*Z_envZ_require_auth2Z_vjj)(u64, u64);
/* import: 'env' 'require_recipient' */
extern void (*Z_envZ_require_recipientZ_vj)(u64);
/* import: 'env' 'ripemd160' */
extern void (*Z_envZ_ripemd160Z_viii)(u32, u32, u32);
/* import: 'env' 's2n' */
extern u64 (*Z_envZ_s2nZ_jii)(u32, u32);
/* import: 'env' 'send_context_free_inline' */
extern void (*Z_envZ_send_context_free_inlineZ_vii)(u32, u32);
/* import: 'env' 'send_deferred' */
extern void (*Z_envZ_send_deferredZ_vijiii)(u32, u64, u32, u32, u32);
/* import: 'env' 'send_inline' */
extern void (*Z_envZ_send_inlineZ_vii)(u32, u32);
/* import: 'env' 'set_blockchain_parameters_packed' */
extern void (*Z_envZ_set_blockchain_parameters_packedZ_vii)(u32, u32);
/* import: 'env' 'set_copy_memory_range' */
extern void (*Z_envZ_set_copy_memory_rangeZ_vii)(u32, u32);
/* import: 'env' 'set_privileged' */
extern void (*Z_envZ_set_privilegedZ_vji)(u64, u32);
/* import: 'env' 'set_proposed_producers' */
extern u64 (*Z_envZ_set_proposed_producersZ_jii)(u32, u32);
/* import: 'env' 'set_resource_limits' */
extern void (*Z_envZ_set_resource_limitsZ_vjjjj)(u64, u64, u64, u64);
/* import: 'env' 'sha1' */
extern void (*Z_envZ_sha1Z_viii)(u32, u32, u32);
/* import: 'env' 'sha256' */
extern void (*Z_envZ_sha256Z_viii)(u32, u32, u32);
/* import: 'env' 'sha512' */
extern void (*Z_envZ_sha512Z_viii)(u32, u32, u32);
/* import: 'env' 'tapos_block_num' */
extern u32 (*Z_envZ_tapos_block_numZ_iv)(void);
/* import: 'env' 'tapos_block_prefix' */
extern u32 (*Z_envZ_tapos_block_prefixZ_iv)(void);
/* import: 'env' 'to_base58' */
extern u32 (*Z_envZ_to_base58Z_iiiii)(u32, u32, u32, u32);
/* import: 'env' 'token_create' */
extern void (*Z_envZ_token_createZ_vjjj)(u64, u64, u64);
/* import: 'env' 'token_issue' */
extern void (*Z_envZ_token_issueZ_vjjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'token_transfer' */
extern void (*Z_envZ_token_transferZ_vjjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'transaction_size' */
extern u32 (*Z_envZ_transaction_sizeZ_iv)(void);

/* export: 'memory' */
extern wasm_rt_memory_t (*WASM_RT_ADD_PREFIX(Z_memory));
/* export: 'setlocale' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_setlocaleZ_iii))(u32, u32);
/* export: '__nl_langinfo_l' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___nl_langinfo_lZ_iii))(u32, u32);
/* export: 'nl_langinfo' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_nl_langinfoZ_ii))(u32);
/* export: '__lctrans' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___lctransZ_iii))(u32, u32);
/* export: 'clock_gettime' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_clock_gettimeZ_iii))(u32, u32);
/* export: 'exit' */
extern void (*WASM_RT_ADD_PREFIX(Z_exitZ_vi))(u32);
/* export: 'flockfile' */
extern void (*WASM_RT_ADD_PREFIX(Z_flockfileZ_vi))(u32);
/* export: 'funlockfile' */
extern void (*WASM_RT_ADD_PREFIX(Z_funlockfileZ_vi))(u32);
/* export: 'fopen' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_fopenZ_iii))(u32, u32);
/* export: 'fdopen' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_fdopenZ_iii))(u32, u32);
/* export: 'ioctl' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_ioctlZ_iiii))(u32, u32, u32);
/* export: 'localeconv' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_localeconvZ_iv))(void);
/* export: 'sigaction' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_sigactionZ_iiii))(u32, u32, u32);
/* export: 'sigemptyset' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_sigemptysetZ_ii))(u32);
/* export: 'strtod' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_strtodZ_dii))(u32, u32);
/* export: '__syscall' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___syscallZ_ii))(u32);
/* export: '__syscall1' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___syscall1Z_iii))(u32, u32);
/* export: '__syscall3' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___syscall3Z_iii))(u32, u32);
/* export: '__syscall5' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___syscall5Z_iii))(u32, u32);
/* export: '__syscall_ret' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___syscall_retZ_ii))(u32);
/* export: '__unlock' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___unlockZ_iv))(void);
/* export: '__block_all_sigs' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___block_all_sigsZ_iv))(void);
/* export: '__lock' */
extern void (*WASM_RT_ADD_PREFIX(Z___lockZ_vi))(u32);
/* export: 'emscripten_asm_const_int' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_emscripten_asm_const_intZ_iiii))(u32, u32, u32);
/* export: 'pthread_self' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_selfZ_iv))(void);
/* export: 'raise' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_raiseZ_ii))(u32);
/* export: 'fprintf' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_fprintfZ_iiii))(u32, u32, u32);
/* export: 'fstat' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_fstatZ_iii))(u32, u32);
/* export: 'ftruncate' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_ftruncateZ_iii))(u32, u32);
/* export: 'open' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_openZ_iiii))(u32, u32, u32);
/* export: 'lseek' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_lseekZ_iiii))(u32, u32, u32);
/* export: 'write' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_writeZ_iiii))(u32, u32, u32);
/* export: 'read' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_readZ_iiii))(u32, u32, u32);
/* export: 'close' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_closeZ_ii))(u32);
/* export: 'fcntl' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_fcntlZ_iiii))(u32, u32, u32);
/* export: 'isatty' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_isattyZ_ii))(u32);
/* export: 'pthread_cond_destroy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_cond_destroyZ_ii))(u32);
/* export: 'pthread_cond_init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_cond_initZ_iii))(u32, u32);
/* export: 'pthread_cond_signal' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_cond_signalZ_ii))(u32);
/* export: 'pthread_cond_timedwait' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_cond_timedwaitZ_iiii))(u32, u32, u32);
/* export: 'pthread_cond_wait' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_cond_waitZ_iii))(u32, u32);
/* export: 'pthread_exit' */
extern void (*WASM_RT_ADD_PREFIX(Z_pthread_exitZ_vi))(u32);
/* export: 'pthread_getspecific' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_getspecificZ_ii))(u32);
/* export: 'pthread_key_create' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_key_createZ_iii))(u32, u32);
/* export: 'pthread_key_delete' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_key_deleteZ_ii))(u32);
/* export: 'pthread_mutex_destroy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_mutex_destroyZ_ii))(u32);
/* export: 'pthread_mutex_init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_mutex_initZ_iii))(u32, u32);
/* export: 'pthread_mutex_lock' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_mutex_lockZ_ii))(u32);
/* export: 'pthread_mutex_trylock' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_mutex_trylockZ_ii))(u32);
/* export: 'pthread_mutex_unlock' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_mutex_unlockZ_ii))(u32);
/* export: 'pthread_setspecific' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pthread_setspecificZ_iii))(u32, u32);
/* export: 'confstr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_confstrZ_iiii))(u32, u32, u32);
/* export: 'dup' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_dupZ_ii))(u32);
/* export: 'getcwd' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_getcwdZ_iii))(u32, u32);
/* export: 'getrusage' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_getrusageZ_iii))(u32, u32);
/* export: 'readlink' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_readlinkZ_iiii))(u32, u32, u32);
/* export: 'select' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_selectZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'siginterrupt' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_siginterruptZ_iii))(u32, u32);
/* export: 'signal' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_signalZ_iii))(u32, u32);
/* export: 'stat' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_statZ_iii))(u32, u32);
/* export: 'syscall' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_syscallZ_iiii))(u32, u32, u32);
/* export: 'sysconf' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_sysconfZ_ii))(u32);
/* export: '__mmap' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___mmapZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '__munmap' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___munmapZ_iii))(u32, u32);
/* export: '__sys_open' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___sys_openZ_iii))(u32, u32);
/* export: 'log_' */
extern void (*WASM_RT_ADD_PREFIX(Z_log_Z_vii))(u32, u32);
/* export: 'realpath' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_realpathZ_iii))(u32, u32);
/* export: '_exit' */
extern void (*WASM_RT_ADD_PREFIX(Z__exitZ_vi))(u32);
/* export: 'apply' */
extern void (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(u64, u64, u64);
/* export: 'call' */
extern void (*WASM_RT_ADD_PREFIX(Z_callZ_vjjjj))(u64, u64, u64, u64);
/* export: '_open' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__openZ_iiii))(u32, u32, u32);
/* export: '_lseek' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__lseekZ_iiii))(u32, u32, u32);
/* export: '_write' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__writeZ_iiii))(u32, u32, u32);
/* export: '_read' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__readZ_iiii))(u32, u32, u32);
/* export: '_close' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__closeZ_ii))(u32);
/* export: 'PyErr_Occurred' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_OccurredZ_iv))(void);
/* export: 'PyErr_Print' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyErr_PrintZ_vv))(void);
/* export: 'PyErr_PrintEx' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyErr_PrintExZ_vi))(u32);
/* export: 'PyErr_ExceptionMatches' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_ExceptionMatchesZ_ii))(u32);
/* export: 'PyErr_Fetch' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyErr_FetchZ_viii))(u32, u32, u32);
/* export: 'PyErr_NormalizeException' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyErr_NormalizeExceptionZ_viii))(u32, u32, u32);
/* export: 'PyException_SetTraceback' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyException_SetTracebackZ_iii))(u32, u32);
/* export: '_PySys_SetObjectId' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PySys_SetObjectIdZ_iii))(u32, u32);
/* export: 'PyErr_Clear' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyErr_ClearZ_vv))(void);
/* export: '_PySys_GetObjectId' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PySys_GetObjectIdZ_ii))(u32);
/* export: '_PyObject_FastCallDict' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_FastCallDictZ_iiiii))(u32, u32, u32, u32);
/* export: 'PySys_WriteStderr' */
extern void (*WASM_RT_ADD_PREFIX(Z_PySys_WriteStderrZ_vii))(u32, u32);
/* export: 'PyErr_Display' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyErr_DisplayZ_viii))(u32, u32, u32);
/* export: 'PyException_GetTraceback' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyException_GetTracebackZ_ii))(u32);
/* export: '_PyObject_Dump' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyObject_DumpZ_vi))(u32);
/* export: 'PySet_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySet_NewZ_ii))(u32);
/* export: 'PyLong_FromVoidPtr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_FromVoidPtrZ_ii))(u32);
/* export: 'PySet_Add' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySet_AddZ_iii))(u32, u32);
/* export: 'PyException_GetCause' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyException_GetCauseZ_ii))(u32);
/* export: 'PyException_GetContext' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyException_GetContextZ_ii))(u32);
/* export: 'PySet_Contains' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySet_ContainsZ_iii))(u32, u32);
/* export: 'PyFile_WriteString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyFile_WriteStringZ_iii))(u32, u32);
/* export: 'PyTraceBack_Print' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyTraceBack_PrintZ_iii))(u32, u32);
/* export: '_PyObject_HasAttrId' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_HasAttrIdZ_iii))(u32, u32);
/* export: '_PyObject_GetAttrId' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_GetAttrIdZ_iii))(u32, u32);
/* export: '_PyUnicode_FromId' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_FromIdZ_ii))(u32);
/* export: '_PyLong_AsInt' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_AsIntZ_ii))(u32);
/* export: 'PyUnicode_AsUTF8' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_AsUTF8Z_ii))(u32);
/* export: '_PyUnicode_EqualToASCIIId' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_EqualToASCIIIdZ_iii))(u32, u32);
/* export: 'PyFile_WriteObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyFile_WriteObjectZ_iiii))(u32, u32, u32);
/* export: 'PyObject_Str' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_StrZ_ii))(u32);
/* export: 'PyUnicode_GetLength' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_GetLengthZ_ii))(u32);
/* export: 'PyErr_BadArgument' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_BadArgumentZ_iv))(void);
/* export: '_PyUnicode_Ready' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_ReadyZ_ii))(u32);
/* export: 'PyErr_Format' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_FormatZ_iiii))(u32, u32, u32);
/* export: 'PyObject_Malloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_MallocZ_ii))(u32);
/* export: 'PyErr_NoMemory' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_NoMemoryZ_iv))(void);
/* export: 'PyObject_Free' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyObject_FreeZ_vi))(u32);
/* export: 'Py_FatalError' */
extern void (*WASM_RT_ADD_PREFIX(Z_Py_FatalErrorZ_vi))(u32);
/* export: 'PyErr_SetObject' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyErr_SetObjectZ_vii))(u32, u32);
/* export: 'PyObject_Call' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_CallZ_iiii))(u32, u32, u32);
/* export: 'PyObject_CallFunctionObjArgs' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_CallFunctionObjArgsZ_iii))(u32, u32);
/* export: 'PyException_SetContext' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyException_SetContextZ_vii))(u32, u32);
/* export: 'PyErr_SetString' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyErr_SetStringZ_vii))(u32, u32);
/* export: 'PyMem_Malloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMem_MallocZ_ii))(u32);
/* export: 'PyMem_Free' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyMem_FreeZ_vi))(u32);
/* export: 'PyUnicode_FromString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_FromStringZ_ii))(u32);
/* export: 'PyUnicode_DecodeUTF8Stateful' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeUTF8StatefulZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyUnicode_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_NewZ_iii))(u32, u32);
/* export: '_PyUnicodeWriter_PrepareInternal' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicodeWriter_PrepareInternalZ_iiii))(u32, u32, u32);
/* export: '_PyUnicodeWriter_Finish' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicodeWriter_FinishZ_ii))(u32);
/* export: 'PyObject_Realloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_ReallocZ_iii))(u32, u32);
/* export: 'PyCodec_LookupError' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCodec_LookupErrorZ_ii))(u32);
/* export: 'PyUnicodeDecodeError_Create' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeDecodeError_CreateZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: 'PyUnicodeDecodeError_SetStart' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeDecodeError_SetStartZ_iii))(u32, u32);
/* export: 'PyUnicodeDecodeError_SetEnd' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeDecodeError_SetEndZ_iii))(u32, u32);
/* export: 'PyUnicodeDecodeError_SetReason' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeDecodeError_SetReasonZ_iii))(u32, u32);
/* export: '_PyArg_ParseTuple_SizeT' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyArg_ParseTuple_SizeTZ_iiii))(u32, u32, u32);
/* export: 'PyUnicodeDecodeError_GetObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeDecodeError_GetObjectZ_ii))(u32);
/* export: '_PyUnicodeWriter_WriteStr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicodeWriter_WriteStrZ_iii))(u32, u32);
/* export: 'PyTuple_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyTuple_NewZ_ii))(u32);
/* export: '_PyErr_BadInternalCall' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyErr_BadInternalCallZ_vii))(u32, u32);
/* export: '_PyObject_GC_NewVar' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_GC_NewVarZ_iii))(u32, u32);
/* export: 'PyObject_GenericGetAttr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_GenericGetAttrZ_iii))(u32, u32);
/* export: 'PyObject_GC_Del' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyObject_GC_DelZ_vi))(u32);
/* export: '_PyArg_NoKeywords' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyArg_NoKeywordsZ_iii))(u32, u32);
/* export: 'PyArg_UnpackTuple' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyArg_UnpackTupleZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PySequence_Tuple' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySequence_TupleZ_ii))(u32);
/* export: 'PyList_AsTuple' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyList_AsTupleZ_ii))(u32);
/* export: 'PyObject_GetIter' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_GetIterZ_ii))(u32);
/* export: 'PyObject_LengthHint' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_LengthHintZ_iii))(u32, u32);
/* export: '_PyTuple_Resize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyTuple_ResizeZ_iii))(u32, u32);
/* export: '_PyObject_GC_Resize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_GC_ResizeZ_iii))(u32, u32);
/* export: 'PyObject_GenericSetAttr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_GenericSetAttrZ_iiii))(u32, u32, u32);
/* export: 'PyObject_GenericGetDict' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_GenericGetDictZ_iii))(u32, u32);
/* export: 'PyObject_GenericSetDict' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_GenericSetDictZ_iiii))(u32, u32, u32);
/* export: '_PyObject_GetDictPtr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_GetDictPtrZ_ii))(u32);
/* export: 'PyDict_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_NewZ_iv))(void);
/* export: '_PyObject_GC_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_GC_NewZ_ii))(u32);
/* export: 'PyObject_HashNotImplemented' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_HashNotImplementedZ_ii))(u32);
/* export: 'PyType_GenericAlloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyType_GenericAllocZ_iii))(u32, u32);
/* export: '_PyObject_GC_Malloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_GC_MallocZ_ii))(u32);
/* export: '_PyObject_LookupAttrId' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_LookupAttrIdZ_iiii))(u32, u32, u32);
/* export: 'PyDict_MergeFromSeq2' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_MergeFromSeq2Z_iiii))(u32, u32, u32);
/* export: 'PyArg_ValidateKeywordArguments' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyArg_ValidateKeywordArgumentsZ_ii))(u32);
/* export: '_PyDict_HasOnlyStringKeys' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_HasOnlyStringKeysZ_ii))(u32);
/* export: 'PyObject_RichCompareBool' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_RichCompareBoolZ_iiii))(u32, u32, u32);
/* export: 'PyObject_RichCompare' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_RichCompareZ_iiii))(u32, u32, u32);
/* export: 'PyObject_IsTrue' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_IsTrueZ_ii))(u32);
/* export: '_PyArg_ParseTupleAndKeywordsFast' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyArg_ParseTupleAndKeywordsFastZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyNumber_Long' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_LongZ_ii))(u32);
/* export: 'PyNumber_AsSsize_t' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_AsSsize_tZ_iii))(u32, u32);
/* export: 'PyLong_FromUnicodeObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_FromUnicodeObjectZ_iii))(u32, u32);
/* export: 'PyType_IsSubtype' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyType_IsSubtypeZ_iii))(u32, u32);
/* export: '_PyLong_FromBytes' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_FromBytesZ_iiii))(u32, u32, u32);
/* export: 'PyLong_FromString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_FromStringZ_iiii))(u32, u32, u32);
/* export: 'PyBytes_FromStringAndSize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBytes_FromStringAndSizeZ_iii))(u32, u32);
/* export: '_PyArg_ParseTupleAndKeywords_SizeT' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyArg_ParseTupleAndKeywords_SizeTZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyUnicode_AsEncodedString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_AsEncodedStringZ_iiii))(u32, u32, u32);
/* export: '_PyObject_LookupSpecial' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_LookupSpecialZ_iii))(u32, u32);
/* export: 'PyObject_Calloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_CallocZ_iii))(u32, u32);
/* export: 'PyBytes_FromObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBytes_FromObjectZ_ii))(u32);
/* export: '_PyBytesWriter_Resize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyBytesWriter_ResizeZ_iiii))(u32, u32, u32);
/* export: '_PyBytesWriter_Finish' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyBytesWriter_FinishZ_iii))(u32, u32);
/* export: 'PyIter_Next' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyIter_NextZ_ii))(u32);
/* export: 'PyByteArray_FromStringAndSize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyByteArray_FromStringAndSizeZ_iii))(u32, u32);
/* export: 'PyByteArray_Resize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyByteArray_ResizeZ_iii))(u32, u32);
/* export: '_PyBytes_Resize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyBytes_ResizeZ_iii))(u32, u32);
/* export: '_PyObject_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_NewZ_ii))(u32);
/* export: 'PyObject_GetBuffer' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_GetBufferZ_iiii))(u32, u32, u32);
/* export: 'PyBuffer_ToContiguous' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBuffer_ToContiguousZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyBuffer_Release' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyBuffer_ReleaseZ_vi))(u32);
/* export: 'PyBuffer_IsContiguous' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBuffer_IsContiguousZ_iii))(u32, u32);
/* export: '_PyType_Lookup' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyType_LookupZ_iii))(u32, u32);
/* export: 'PyType_Modified' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyType_ModifiedZ_vi))(u32);
/* export: 'PyDict_Next' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_NextZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyObject_Hash' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_HashZ_ii))(u32);
/* export: 'PyType_Ready' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyType_ReadyZ_ii))(u32);
/* export: '_PyDict_GetItem_KnownHash' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_GetItem_KnownHashZ_iiii))(u32, u32, u32);
/* export: 'PyTuple_Pack' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyTuple_PackZ_iii))(u32, u32);
/* export: 'PyUnicode_InternFromString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_InternFromStringZ_ii))(u32);
/* export: 'PyDict_GetItem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_GetItemZ_iii))(u32, u32);
/* export: 'PyDict_SetItem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_SetItemZ_iiii))(u32, u32, u32);
/* export: 'PyDescr_NewWrapper' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDescr_NewWrapperZ_iiii))(u32, u32, u32);
/* export: '_PyDict_GetItemId' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_GetItemIdZ_iii))(u32, u32);
/* export: 'PyCFunction_NewEx' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCFunction_NewExZ_iiii))(u32, u32, u32);
/* export: '_PyDict_SetItemId' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_SetItemIdZ_iiii))(u32, u32, u32);
/* export: 'PyDict_GetItemString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_GetItemStringZ_iii))(u32, u32);
/* export: 'PyDescr_NewClassMethod' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDescr_NewClassMethodZ_iii))(u32, u32);
/* export: 'PyStaticMethod_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyStaticMethod_NewZ_ii))(u32);
/* export: 'PyDescr_NewMethod' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDescr_NewMethodZ_iii))(u32, u32);
/* export: 'PyDict_SetItemString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_SetItemStringZ_iiii))(u32, u32, u32);
/* export: 'PyDescr_NewMember' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDescr_NewMemberZ_iii))(u32, u32);
/* export: 'PyDescr_NewGetSet' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDescr_NewGetSetZ_iii))(u32, u32);
/* export: 'PyWeakref_NewRef' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyWeakref_NewRefZ_iii))(u32, u32);
/* export: 'PyObject_GC_Track' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyObject_GC_TrackZ_vi))(u32);
/* export: 'PyObject_SetAttr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_SetAttrZ_iiii))(u32, u32, u32);
/* export: 'PyUnicode_InternInPlace' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyUnicode_InternInPlaceZ_vi))(u32);
/* export: 'PyDict_SetDefault' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_SetDefaultZ_iiii))(u32, u32, u32);
/* export: 'PyObject_GetAttr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_GetAttrZ_iii))(u32, u32);
/* export: 'PyObject_DelItem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_DelItemZ_iii))(u32, u32);
/* export: 'PyObject_SetItem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_SetItemZ_iiii))(u32, u32, u32);
/* export: 'PyObject_GetItem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_GetItemZ_iii))(u32, u32);
/* export: 'PyObject_Size' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_SizeZ_ii))(u32);
/* export: 'PySequence_Contains' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySequence_ContainsZ_iii))(u32, u32);
/* export: '_PySequence_IterSearch' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PySequence_IterSearchZ_iiii))(u32, u32, u32);
/* export: 'PyNumber_Index' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_IndexZ_ii))(u32);
/* export: 'PyErr_WarnFormat' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_WarnFormatZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyUnicode_FromFormatV' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_FromFormatVZ_iii))(u32, u32);
/* export: 'PyThreadState_Get' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyThreadState_GetZ_iv))(void);
/* export: 'PyFrame_GetLineNumber' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyFrame_GetLineNumberZ_ii))(u32);
/* export: 'PyUnicode_Substring' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_SubstringZ_iiii))(u32, u32, u32);
/* export: '_PyUnicode_EqualToASCIIString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_EqualToASCIIStringZ_iii))(u32, u32);
/* export: 'PyList_Size' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyList_SizeZ_ii))(u32);
/* export: 'PyList_GetItem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyList_GetItemZ_iii))(u32, u32);
/* export: 'PyObject_IsInstance' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_IsInstanceZ_iii))(u32, u32);
/* export: 'PyLong_FromLong' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_FromLongZ_ii))(u32);
/* export: 'PyImport_GetModule' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyImport_GetModuleZ_ii))(u32);
/* export: 'PyUnicode_Compare' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_CompareZ_iii))(u32, u32);
/* export: '_PyObject_CallMethodIdObjArgs' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_CallMethodIdObjArgsZ_iiii))(u32, u32, u32);
/* export: 'PyObject_IsSubclass' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_IsSubclassZ_iii))(u32, u32);
/* export: 'PyLong_AsSsize_t' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_AsSsize_tZ_ii))(u32);
/* export: 'PyOS_snprintf' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyOS_snprintfZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_DisplaySourceLine' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_DisplaySourceLineZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyCallable_Check' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCallable_CheckZ_ii))(u32);
/* export: 'PyImport_ImportModuleNoBlock' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyImport_ImportModuleNoBlockZ_ii))(u32);
/* export: '_PyObject_CallMethodId' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_CallMethodIdZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyUnicode_EncodeFSDefault' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_EncodeFSDefaultZ_ii))(u32);
/* export: 'PyObject_AsFileDescriptor' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_AsFileDescriptorZ_ii))(u32);
/* export: 'PyTokenizer_FindEncodingFilename' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyTokenizer_FindEncodingFilenameZ_iii))(u32, u32);
/* export: 'PyFile_GetLine' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyFile_GetLineZ_iii))(u32, u32);
/* export: '_PyObject_CallMethodId_SizeT' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_CallMethodId_SizeTZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_VaBuildStack_SizeT' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_VaBuildStack_SizeTZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_VaBuildStack' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_VaBuildStackZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyErr_Restore' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyErr_RestoreZ_viii))(u32, u32, u32);
/* export: 'PyList_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyList_NewZ_ii))(u32);
/* export: 'PyLong_FromUnsignedLong' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_FromUnsignedLongZ_ii))(u32);
/* export: 'PyLong_FromLongLong' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_FromLongLongZ_ij))(u64);
/* export: 'PyLong_FromUnsignedLongLong' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_FromUnsignedLongLongZ_ij))(u64);
/* export: 'PyUnicode_FromWideChar' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_FromWideCharZ_iii))(u32, u32);
/* export: 'PyFloat_FromDouble' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyFloat_FromDoubleZ_id))(f64);
/* export: 'PyComplex_FromCComplex' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyComplex_FromCComplexZ_ii))(u32);
/* export: 'PyUnicode_FromOrdinal' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_FromOrdinalZ_ii))(u32);
/* export: 'PyUnicode_FromStringAndSize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_FromStringAndSizeZ_iii))(u32, u32);
/* export: '_PyUnicode_TransformDecimalAndSpaceToASCII' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_TransformDecimalAndSpaceToASCIIZ_ii))(u32);
/* export: 'PyUnicode_AsUTF8AndSize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_AsUTF8AndSizeZ_iii))(u32, u32);
/* export: '_Py_string_to_number_with_underscores' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_string_to_number_with_underscoresZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: 'PyNumber_Float' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_FloatZ_ii))(u32);
/* export: 'PyFloat_AsDouble' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_PyFloat_AsDoubleZ_di))(u32);
/* export: 'PyFloat_FromString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyFloat_FromStringZ_ii))(u32);
/* export: 'PyOS_string_to_double' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_PyOS_string_to_doubleZ_diii))(u32, u32, u32);
/* export: '_Py_dg_strtod' */
extern f64 (*WASM_RT_ADD_PREFIX(Z__Py_dg_strtodZ_dii))(u32, u32);
/* export: '_Py_parse_inf_or_nan' */
extern f64 (*WASM_RT_ADD_PREFIX(Z__Py_parse_inf_or_nanZ_dii))(u32, u32);
/* export: '_Py_dg_infinity' */
extern f64 (*WASM_RT_ADD_PREFIX(Z__Py_dg_infinityZ_di))(u32);
/* export: '_Py_dg_stdnan' */
extern f64 (*WASM_RT_ADD_PREFIX(Z__Py_dg_stdnanZ_di))(u32);
/* export: 'PyArg_Parse' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyArg_ParseZ_iiii))(u32, u32, u32);
/* export: '_PyUnicodeWriter_Init' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyUnicodeWriter_InitZ_vi))(u32);
/* export: '_PyFloat_FormatAdvancedWriter' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyFloat_FormatAdvancedWriterZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_PyUnicodeWriter_Dealloc' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyUnicodeWriter_DeallocZ_vi))(u32);
/* export: 'PyOS_double_to_string' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyOS_double_to_stringZ_idiiii))(f64, u32, u32, u32, u32);
/* export: '_PyUnicodeWriter_WriteASCIIString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicodeWriter_WriteASCIIStringZ_iiii))(u32, u32, u32);
/* export: '_PyUnicode_FromASCII' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_FromASCIIZ_iii))(u32, u32);
/* export: '_PyUnicode_FastFill' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyUnicode_FastFillZ_viiii))(u32, u32, u32, u32);
/* export: '_PyUnicode_FastCopyCharacters' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyUnicode_FastCopyCharactersZ_viiiii))(u32, u32, u32, u32, u32);
/* export: '_PyUnicode_InsertThousandsGrouping' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_InsertThousandsGroupingZ_iiiiiiiiii))(u32, u32, u32, u32, u32, u32, u32, u32, u32);
/* export: '_Py_GetLocaleconvNumeric' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_GetLocaleconvNumericZ_iiii))(u32, u32, u32);
/* export: '_PyMem_Strdup' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyMem_StrdupZ_ii))(u32);
/* export: 'PyUnicode_DecodeLocale' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeLocaleZ_iii))(u32, u32);
/* export: '_Py_DecodeLocaleEx' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_DecodeLocaleExZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_PyObject_CallFunction_SizeT' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_CallFunction_SizeTZ_iiii))(u32, u32, u32);
/* export: 'PyCodec_StrictErrors' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCodec_StrictErrorsZ_ii))(u32);
/* export: 'PyMem_RawFree' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyMem_RawFreeZ_vi))(u32);
/* export: 'PyUnicode_FromFormat' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_FromFormatZ_iii))(u32, u32);
/* export: '_Py_DecodeUTF8Ex' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_DecodeUTF8ExZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: 'PyMem_RawMalloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMem_RawMallocZ_ii))(u32);
/* export: '_Py_normalize_encoding' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_normalize_encodingZ_iiii))(u32, u32, u32);
/* export: '_Py_dg_dtoa' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_dg_dtoaZ_idiiiii))(f64, u32, u32, u32, u32, u32);
/* export: '_Py_dg_freedtoa' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_dg_freedtoaZ_vi))(u32);
/* export: '_PyUnicode_ToDecimalDigit' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_ToDecimalDigitZ_ii))(u32);
/* export: 'PySequence_Check' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySequence_CheckZ_ii))(u32);
/* export: 'PySequence_Size' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySequence_SizeZ_ii))(u32);
/* export: 'PySequence_GetItem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySequence_GetItemZ_iii))(u32, u32);
/* export: 'PyLong_AsLong' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_AsLongZ_ii))(u32);
/* export: 'PyLong_AsUnsignedLongMask' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_AsUnsignedLongMaskZ_ii))(u32);
/* export: 'PyLong_AsLongLong' */
extern u64 (*WASM_RT_ADD_PREFIX(Z_PyLong_AsLongLongZ_ji))(u32);
/* export: 'PyLong_AsUnsignedLongLongMask' */
extern u64 (*WASM_RT_ADD_PREFIX(Z_PyLong_AsUnsignedLongLongMaskZ_ji))(u32);
/* export: 'PyComplex_AsCComplex' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyComplex_AsCComplexZ_vii))(u32, u32);
/* export: 'PyBytes_Size' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBytes_SizeZ_ii))(u32);
/* export: 'PyByteArray_Size' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyByteArray_SizeZ_ii))(u32);
/* export: 'PyBuffer_FillInfo' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBuffer_FillInfoZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: 'PyUnicode_AsUnicodeAndSize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_AsUnicodeAndSizeZ_iii))(u32, u32);
/* export: 'PyUnicode_GetDefaultEncoding' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_GetDefaultEncodingZ_iv))(void);
/* export: '_PyLong_FromNbInt' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_FromNbIntZ_ii))(u32);
/* export: '_PyLong_AsByteArray' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_AsByteArrayZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyLong_AsLongAndOverflow' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_AsLongAndOverflowZ_iii))(u32, u32);
/* export: '_PyArg_ParseStack' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyArg_ParseStackZ_iiiii))(u32, u32, u32, u32);
/* export: 'Py_BuildValue' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_Py_BuildValueZ_iii))(u32, u32);
/* export: 'PyErr_SetFromErrno' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_SetFromErrnoZ_ii))(u32);
/* export: 'PyErr_SetFromErrnoWithFilenameObjects' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_SetFromErrnoWithFilenameObjectsZ_iiii))(u32, u32, u32);
/* export: 'PyErr_CheckSignals' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_CheckSignalsZ_iv))(void);
/* export: 'PyThread_get_thread_ident' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyThread_get_thread_identZ_iv))(void);
/* export: 'PyEval_GetFrame' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyEval_GetFrameZ_iv))(void);
/* export: 'PyEval_CallObjectWithKeywords' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyEval_CallObjectWithKeywordsZ_iiii))(u32, u32, u32);
/* export: 'PyLong_AsDouble' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_PyLong_AsDoubleZ_di))(u32);
/* export: '_PyLong_Frexp' */
extern f64 (*WASM_RT_ADD_PREFIX(Z__PyLong_FrexpZ_dii))(u32, u32);
/* export: 'PyLong_FromDouble' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_FromDoubleZ_id))(f64);
/* export: '_PyArg_UnpackStack' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyArg_UnpackStackZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_PyLong_Sign' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_SignZ_ii))(u32);
/* export: '_PyLong_NumBits' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_NumBitsZ_ii))(u32);
/* export: 'PyNumber_Negative' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_NegativeZ_ii))(u32);
/* export: 'PyNumber_Lshift' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_LshiftZ_iii))(u32, u32);
/* export: 'PyNumber_Or' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_OrZ_iii))(u32, u32);
/* export: 'PyBool_FromLong' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBool_FromLongZ_ii))(u32);
/* export: '_Py_HashDouble' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_HashDoubleZ_id))(f64);
/* export: '_PyUnicode_AsUTF8String' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_AsUTF8StringZ_iii))(u32, u32);
/* export: '_PyBytesWriter_Init' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyBytesWriter_InitZ_vi))(u32);
/* export: '_PyBytesWriter_Alloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyBytesWriter_AllocZ_iii))(u32, u32);
/* export: 'PyUnicodeEncodeError_SetStart' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeEncodeError_SetStartZ_iii))(u32, u32);
/* export: 'PyUnicodeEncodeError_SetEnd' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeEncodeError_SetEndZ_iii))(u32, u32);
/* export: 'PyUnicodeEncodeError_SetReason' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeEncodeError_SetReasonZ_iii))(u32, u32);
/* export: '_PyBytesWriter_WriteBytes' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyBytesWriter_WriteBytesZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyBytesWriter_Dealloc' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyBytesWriter_DeallocZ_vi))(u32);
/* export: 'PyUnicode_ReadChar' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_ReadCharZ_iii))(u32, u32);
/* export: '_PyBytesWriter_Prepare' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyBytesWriter_PrepareZ_iiii))(u32, u32, u32);
/* export: '_PyUnicode_IsWhitespace' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_IsWhitespaceZ_ii))(u32);
/* export: 'PyArg_ParseTuple' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyArg_ParseTupleZ_iiii))(u32, u32, u32);
/* export: '_PyComplex_FormatAdvancedWriter' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyComplex_FormatAdvancedWriterZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyComplex_RealAsDouble' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_PyComplex_RealAsDoubleZ_di))(u32);
/* export: 'PyComplex_ImagAsDouble' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_PyComplex_ImagAsDoubleZ_di))(u32);
/* export: 'PyMem_Calloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMem_CallocZ_iii))(u32, u32);
/* export: '_Py_dup' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_dupZ_ii))(u32);
/* export: 'PyTokenizer_Free' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyTokenizer_FreeZ_vi))(u32);
/* export: 'PyTokenizer_Get' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyTokenizer_GetZ_iiii))(u32, u32, u32);
/* export: 'PyUnicode_DecodeUTF8' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeUTF8Z_iiii))(u32, u32, u32);
/* export: 'PyUnicode_IsIdentifier' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_IsIdentifierZ_ii))(u32);
/* export: '_PyUnicode_IsXidStart' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_IsXidStartZ_ii))(u32);
/* export: '_PyUnicode_IsXidContinue' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_IsXidContinueZ_ii))(u32);
/* export: 'PyOS_Readline' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyOS_ReadlineZ_iiii))(u32, u32, u32);
/* export: 'PyMem_Realloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMem_ReallocZ_iii))(u32, u32);
/* export: 'PyUnicode_Decode' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyUnicode_AsUTF8String' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_AsUTF8StringZ_ii))(u32);
/* export: 'PyByteArray_AsString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyByteArray_AsStringZ_ii))(u32);
/* export: 'Py_UniversalNewlineFgets' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_Py_UniversalNewlineFgetsZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyUnicode_FindChar' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_FindCharZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyUnicode_Tailmatch' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_TailmatchZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_PyUnicode_XStrip' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_XStripZ_iiii))(u32, u32, u32);
/* export: 'PyErr_SetFromErrnoWithFilename' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_SetFromErrnoWithFilenameZ_iii))(u32, u32);
/* export: 'PyUnicode_DecodeFSDefault' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeFSDefaultZ_ii))(u32);
/* export: 'PyNumber_Check' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_CheckZ_ii))(u32);
/* export: 'PyTuple_GetSlice' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyTuple_GetSliceZ_iiii))(u32, u32, u32);
/* export: 'PyLong_FromSsize_t' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_FromSsize_tZ_ii))(u32);
/* export: 'PyUnicode_DecodeUTF16Stateful' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeUTF16StatefulZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyUnicode_DecodeUTF32Stateful' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeUTF32StatefulZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyUnicode_DecodeASCII' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeASCIIZ_iiii))(u32, u32, u32);
/* export: 'PyMemoryView_FromBuffer' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMemoryView_FromBufferZ_ii))(u32);
/* export: '_PyCodec_DecodeText' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyCodec_DecodeTextZ_iiii))(u32, u32, u32);
/* export: '_PyCodec_LookupTextEncoding' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyCodec_LookupTextEncodingZ_iii))(u32, u32);
/* export: '_PyErr_TrySetFromCause' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyErr_TrySetFromCauseZ_iii))(u32, u32);
/* export: '_PyCodec_Lookup' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyCodec_LookupZ_ii))(u32);
/* export: 'PyUnicodeEncodeError_GetStart' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeEncodeError_GetStartZ_iii))(u32, u32);
/* export: 'PyUnicodeEncodeError_GetEnd' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeEncodeError_GetEndZ_iii))(u32, u32);
/* export: 'PyUnicodeEncodeError_GetObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeEncodeError_GetObjectZ_ii))(u32);
/* export: 'PyBytes_AsString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBytes_AsStringZ_ii))(u32);
/* export: 'PyUnicodeDecodeError_GetStart' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeDecodeError_GetStartZ_iii))(u32, u32);
/* export: 'PyUnicodeDecodeError_GetEnd' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeDecodeError_GetEndZ_iii))(u32, u32);
/* export: 'PyUnicode_FromKindAndData' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_FromKindAndDataZ_iiii))(u32, u32, u32);
/* export: 'PyUnicodeEncodeError_GetEncoding' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeEncodeError_GetEncodingZ_ii))(u32);
/* export: 'PyUnicodeDecodeError_GetEncoding' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeDecodeError_GetEncodingZ_ii))(u32);
/* export: 'PyCodec_NameReplaceErrors' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCodec_NameReplaceErrorsZ_ii))(u32);
/* export: 'PyCapsule_Import' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCapsule_ImportZ_iii))(u32, u32);
/* export: 'PyObject_GetAttrString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_GetAttrStringZ_iii))(u32, u32);
/* export: 'PyImport_ImportModule' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyImport_ImportModuleZ_ii))(u32);
/* export: 'PyDict_Copy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_CopyZ_ii))(u32);
/* export: 'PyImport_Import' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyImport_ImportZ_ii))(u32);
/* export: 'PyEval_GetGlobals' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyEval_GetGlobalsZ_iv))(void);
/* export: 'PyImport_ImportModuleLevelObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyImport_ImportModuleLevelObjectZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyObject_CallFunction' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_CallFunctionZ_iiii))(u32, u32, u32);
/* export: 'PyObject_Repr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_ReprZ_ii))(u32);
/* export: '_Py_CheckRecursiveCall' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_CheckRecursiveCallZ_ii))(u32);
/* export: 'PyErr_WarnEx' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_WarnExZ_iiii))(u32, u32, u32);
/* export: '_PyTime_GetPerfCounter' */
extern u64 (*WASM_RT_ADD_PREFIX(Z__PyTime_GetPerfCounterZ_jv))(void);
/* export: '_PyTime_AsMicroseconds' */
extern u64 (*WASM_RT_ADD_PREFIX(Z__PyTime_AsMicrosecondsZ_jji))(u64, u32);
/* export: 'PyCodec_BackslashReplaceErrors' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCodec_BackslashReplaceErrorsZ_ii))(u32);
/* export: 'PyUnicodeTranslateError_GetStart' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeTranslateError_GetStartZ_iii))(u32, u32);
/* export: 'PyUnicodeTranslateError_GetEnd' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeTranslateError_GetEndZ_iii))(u32, u32);
/* export: 'PyUnicodeTranslateError_GetObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeTranslateError_GetObjectZ_ii))(u32);
/* export: 'PyCodec_XMLCharRefReplaceErrors' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCodec_XMLCharRefReplaceErrorsZ_ii))(u32);
/* export: 'PyCodec_ReplaceErrors' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCodec_ReplaceErrorsZ_ii))(u32);
/* export: 'PyCodec_IgnoreErrors' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCodec_IgnoreErrorsZ_ii))(u32);
/* export: 'PyArg_ParseTupleAndKeywords' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyArg_ParseTupleAndKeywordsZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyMemoryView_FromObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMemoryView_FromObjectZ_ii))(u32);
/* export: 'PyUnicode_AsASCIIString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_AsASCIIStringZ_ii))(u32);
/* export: 'PyLong_FromSize_t' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_FromSize_tZ_ii))(u32);
/* export: '_Py_strhex' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_strhexZ_iii))(u32, u32);
/* export: 'PyBytes_FromString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBytes_FromStringZ_ii))(u32);
/* export: '_Py_HashBytes' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_HashBytesZ_iii))(u32, u32);
/* export: 'PySlice_Unpack' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySlice_UnpackZ_iiiii))(u32, u32, u32, u32);
/* export: 'PySlice_AdjustIndices' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySlice_AdjustIndicesZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyEval_SliceIndex' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyEval_SliceIndexZ_iii))(u32, u32);
/* export: '_PySlice_GetLongIndices' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PySlice_GetLongIndicesZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyNumber_Add' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_AddZ_iii))(u32, u32);
/* export: 'PyLong_AsUnsignedLong' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_AsUnsignedLongZ_ii))(u32);
/* export: 'PyLong_AsUnsignedLongLong' */
extern u64 (*WASM_RT_ADD_PREFIX(Z_PyLong_AsUnsignedLongLongZ_ji))(u32);
/* export: 'PyLong_AsSize_t' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_AsSize_tZ_ii))(u32);
/* export: 'PyLong_AsVoidPtr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_AsVoidPtrZ_ii))(u32);
/* export: 'PyObject_ClearWeakRefs' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyObject_ClearWeakRefsZ_vi))(u32);
/* export: 'PyErr_WriteUnraisable' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyErr_WriteUnraisableZ_vi))(u32);
/* export: '_PyErr_ChainExceptions' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyErr_ChainExceptionsZ_viii))(u32, u32, u32);
/* export: 'PyOS_StdioReadline' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyOS_StdioReadlineZ_iiii))(u32, u32, u32);
/* export: 'PyThread_allocate_lock' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyThread_allocate_lockZ_iv))(void);
/* export: 'PyEval_SaveThread' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyEval_SaveThreadZ_iv))(void);
/* export: 'PyThread_acquire_lock' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyThread_acquire_lockZ_iii))(u32, u32);
/* export: 'PyEval_RestoreThread' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyEval_RestoreThreadZ_vi))(u32);
/* export: 'PyThread_release_lock' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyThread_release_lockZ_vi))(u32);
/* export: '_Py_IsFinalizing' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_IsFinalizingZ_iv))(void);
/* export: 'PyThread_exit_thread' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyThread_exit_threadZ_vv))(void);
/* export: 'PyThreadState_Swap' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyThreadState_SwapZ_ii))(u32);
/* export: 'PyThread_acquire_lock_timed' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyThread_acquire_lock_timedZ_iiji))(u32, u64, u32);
/* export: 'PyMem_RawRealloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMem_RawReallocZ_iii))(u32, u32);
/* export: 'PyOS_InterruptOccurred' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyOS_InterruptOccurredZ_iv))(void);
/* export: '_Py_EncodeLocaleEx' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_EncodeLocaleExZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_Py_EncodeUTF8Ex' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_EncodeUTF8ExZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: 'PyDict_GetItemWithError' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_GetItemWithErrorZ_iii))(u32, u32);
/* export: 'PyDict_Clear' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyDict_ClearZ_vi))(u32);
/* export: '_PyUnicode_Copy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_CopyZ_ii))(u32);
/* export: 'PyCode_Addr2Line' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCode_Addr2LineZ_iii))(u32, u32);
/* export: 'PyUnicode_Contains' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_ContainsZ_iii))(u32, u32);
/* export: '_PyUnicode_AsKind' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_AsKindZ_iii))(u32, u32);
/* export: 'PyUnicode_Fill' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_FillZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyObject_ASCII' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_ASCIIZ_ii))(u32);
/* export: '_PyUnicodeWriter_WriteLatin1String' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicodeWriter_WriteLatin1StringZ_iiii))(u32, u32, u32);
/* export: '_PyUnicode_AsASCIIString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_AsASCIIStringZ_iii))(u32, u32);
/* export: '_PyUnicode_FindMaxChar' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_FindMaxCharZ_iiii))(u32, u32, u32);
/* export: 'PyNumber_InPlaceTrueDivide' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_InPlaceTrueDivideZ_iii))(u32, u32);
/* export: 'PyNumber_InPlaceFloorDivide' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_InPlaceFloorDivideZ_iii))(u32, u32);
/* export: 'PyNumber_TrueDivide' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_TrueDivideZ_iii))(u32, u32);
/* export: 'PyNumber_FloorDivide' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_FloorDivideZ_iii))(u32, u32);
/* export: 'PyNumber_InPlaceOr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_InPlaceOrZ_iii))(u32, u32);
/* export: 'PyNumber_InPlaceXor' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_InPlaceXorZ_iii))(u32, u32);
/* export: 'PyNumber_InPlaceAnd' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_InPlaceAndZ_iii))(u32, u32);
/* export: 'PyNumber_InPlaceRshift' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_InPlaceRshiftZ_iii))(u32, u32);
/* export: 'PyNumber_InPlaceLshift' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_InPlaceLshiftZ_iii))(u32, u32);
/* export: 'PyNumber_InPlacePower' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_InPlacePowerZ_iiii))(u32, u32, u32);
/* export: 'PyNumber_InPlaceRemainder' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_InPlaceRemainderZ_iii))(u32, u32);
/* export: 'PyNumber_InPlaceMultiply' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_InPlaceMultiplyZ_iii))(u32, u32);
/* export: 'PyNumber_InPlaceSubtract' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_InPlaceSubtractZ_iii))(u32, u32);
/* export: 'PyNumber_InPlaceAdd' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_InPlaceAddZ_iii))(u32, u32);
/* export: 'PyNumber_Xor' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_XorZ_iii))(u32, u32);
/* export: 'PyNumber_And' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_AndZ_iii))(u32, u32);
/* export: 'PyNumber_Rshift' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_RshiftZ_iii))(u32, u32);
/* export: 'PyCFunction_Call' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCFunction_CallZ_iiii))(u32, u32, u32);
/* export: '_PyType_GetTextSignatureFromInternalDoc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyType_GetTextSignatureFromInternalDocZ_iii))(u32, u32);
/* export: 'PyType_GenericNew' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyType_GenericNewZ_iiii))(u32, u32, u32);
/* export: 'PyDict_Keys' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_KeysZ_ii))(u32);
/* export: 'PyModule_GetName' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModule_GetNameZ_ii))(u32);
/* export: 'PyObject_CallMethod' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_CallMethodZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyObject_GC_UnTrack' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyObject_GC_UnTrackZ_vi))(u32);
/* export: 'PySys_FormatStderr' */
extern void (*WASM_RT_ADD_PREFIX(Z_PySys_FormatStderrZ_vii))(u32, u32);
/* export: '_PyType_GetDocFromInternalDoc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyType_GetDocFromInternalDocZ_iii))(u32, u32);
/* export: '_PyEval_GetBuiltinId' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyEval_GetBuiltinIdZ_ii))(u32);
/* export: '_PyDict_GetItemIdWithError' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_GetItemIdWithErrorZ_iii))(u32, u32);
/* export: '_PyErr_FormatFromCause' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyErr_FormatFromCauseZ_iiii))(u32, u32, u32);
/* export: '_PyMethodDef_RawFastCallDict' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyMethodDef_RawFastCallDictZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_PyStack_AsTuple' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyStack_AsTupleZ_iii))(u32, u32);
/* export: '_PyStack_UnpackDict' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyStack_UnpackDictZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyErr_FormatV' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_FormatVZ_iiii))(u32, u32, u32);
/* export: 'PyException_SetCause' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyException_SetCauseZ_vii))(u32, u32);
/* export: '_Py_HashPointer' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_HashPointerZ_ii))(u32);
/* export: 'PyNumber_Invert' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_InvertZ_ii))(u32);
/* export: 'PyNumber_Absolute' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_AbsoluteZ_ii))(u32);
/* export: 'PyNumber_Positive' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_PositiveZ_ii))(u32);
/* export: 'PyNumber_Power' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_PowerZ_iiii))(u32, u32, u32);
/* export: 'PyNumber_Divmod' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_DivmodZ_iii))(u32, u32);
/* export: 'PyNumber_Remainder' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_RemainderZ_iii))(u32, u32);
/* export: 'PyNumber_Multiply' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_MultiplyZ_iii))(u32, u32);
/* export: 'PyNumber_Subtract' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_SubtractZ_iii))(u32, u32);
/* export: 'PyList_Append' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyList_AppendZ_iii))(u32, u32);
/* export: 'PyFunction_NewWithQualName' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyFunction_NewWithQualNameZ_iiii))(u32, u32, u32);
/* export: 'PyTuple_Size' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyTuple_SizeZ_ii))(u32);
/* export: 'PyTuple_GetItem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyTuple_GetItemZ_iii))(u32, u32);
/* export: 'PyCode_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCode_NewZ_iiiiiiiiiiiiiiii))(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
/* export: 'PyObject_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_InitZ_iii))(u32, u32);
/* export: 'PyFrozenSet_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyFrozenSet_NewZ_ii))(u32);
/* export: '_PyDict_Next' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_NextZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_PyUnicode_EQ' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_EQZ_iii))(u32, u32);
/* export: '_PyErr_SetKeyError' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyErr_SetKeyErrorZ_vi))(u32);
/* export: 'PySequence_List' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySequence_ListZ_ii))(u32);
/* export: '_PyList_Extend' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyList_ExtendZ_iii))(u32, u32);
/* export: 'PySequence_Fast' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySequence_FastZ_iii))(u32, u32);
/* export: '_PyDict_Contains' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_ContainsZ_iiii))(u32, u32, u32);
/* export: 'PyObject_SelfIter' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_SelfIterZ_ii))(u32);
/* export: '_PyObject_GetBuiltin' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_GetBuiltinZ_ii))(u32);
/* export: 'Py_ReprEnter' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_Py_ReprEnterZ_ii))(u32);
/* export: 'Py_ReprLeave' */
extern void (*WASM_RT_ADD_PREFIX(Z_Py_ReprLeaveZ_vi))(u32);
/* export: 'PyThreadState_GetDict' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyThreadState_GetDictZ_iv))(void);
/* export: 'PyList_SetSlice' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyList_SetSliceZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyTrash_thread_destroy_chain' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyTrash_thread_destroy_chainZ_vv))(void);
/* export: '_PyTrash_thread_deposit_object' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyTrash_thread_deposit_objectZ_vi))(u32);
/* export: '_PyCode_ConstantKey' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyCode_ConstantKeyZ_ii))(u32);
/* export: '_PySet_NextEntry' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PySet_NextEntryZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyMethod_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMethod_NewZ_iii))(u32, u32);
/* export: '_PyObject_Call_Prepend' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_Call_PrependZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyFunction_FastCallDict' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyFunction_FastCallDictZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyFrame_New_NoTrack' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyFrame_New_NoTrackZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyEval_EvalFrameEx' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyEval_EvalFrameExZ_iii))(u32, u32);
/* export: '_PyEval_EvalCodeWithName' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyEval_EvalCodeWithNameZ_iiiiiiiiiiiiiiii))(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
/* export: 'PyCell_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCell_NewZ_ii))(u32);
/* export: 'PyCoro_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCoro_NewZ_iiii))(u32, u32, u32);
/* export: 'PyAsyncGen_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyAsyncGen_NewZ_iiii))(u32, u32, u32);
/* export: 'PyGen_NewWithQualName' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyGen_NewWithQualNameZ_iiii))(u32, u32, u32);
/* export: '_PyGen_Finalize' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyGen_FinalizeZ_vi))(u32);
/* export: '_PyErr_WarnUnawaitedCoroutine' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyErr_WarnUnawaitedCoroutineZ_vi))(u32);
/* export: 'PyErr_SetNone' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyErr_SetNoneZ_vi))(u32);
/* export: '_PyGen_Send' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyGen_SendZ_iii))(u32, u32);
/* export: 'PyErr_GivenExceptionMatches' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_GivenExceptionMatchesZ_iii))(u32, u32);
/* export: '_PyGen_FetchStopIterationValue' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyGen_FetchStopIterationValueZ_ii))(u32);
/* export: 'PyObject_CallFinalizerFromDealloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_CallFinalizerFromDeallocZ_ii))(u32);
/* export: 'PyUnicode_Join' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_JoinZ_iii))(u32, u32);
/* export: 'PyUnicode_Concat' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_ConcatZ_iii))(u32, u32);
/* export: '_PyUnicode_JoinArray' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_JoinArrayZ_iiii))(u32, u32, u32);
/* export: 'PyModule_GetDict' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModule_GetDictZ_ii))(u32);
/* export: 'PyBytes_AsStringAndSize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBytes_AsStringAndSizeZ_iiii))(u32, u32, u32);
/* export: 'PyFrame_FastToLocalsWithError' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyFrame_FastToLocalsWithErrorZ_ii))(u32);
/* export: 'PyMember_SetOne' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMember_SetOneZ_iiii))(u32, u32, u32);
/* export: 'PyMember_GetOne' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMember_GetOneZ_iii))(u32, u32);
/* export: '_PyObject_RealIsSubclass' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_RealIsSubclassZ_iii))(u32, u32);
/* export: '_Py_CheckFunctionResult' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_CheckFunctionResultZ_iiii))(u32, u32, u32);
/* export: '_PyObject_IsAbstract' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_IsAbstractZ_ii))(u32);
/* export: '_PyObject_LookupAttr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_LookupAttrZ_iiii))(u32, u32, u32);
/* export: '_PyObject_GenericGetAttrWithDict' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_GenericGetAttrWithDictZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyObject_FastCall_Prepend' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_FastCall_PrependZ_iiiii))(u32, u32, u32, u32);
/* export: 'PySeqIter_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySeqIter_NewZ_ii))(u32);
/* export: '_PyObject_HasLen' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_HasLenZ_ii))(u32);
/* export: 'PyUnicode_RichCompare' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_RichCompareZ_iiii))(u32, u32, u32);
/* export: 'PyUnicode_FromEncodedObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_FromEncodedObjectZ_iiii))(u32, u32, u32);
/* export: '_Py_BuildValue_SizeT' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_BuildValue_SizeTZ_iii))(u32, u32);
/* export: '_PyArg_ParseStack_SizeT' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyArg_ParseStack_SizeTZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyArg_Parse_SizeT' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyArg_Parse_SizeTZ_iiii))(u32, u32, u32);
/* export: '_PyUnicode_FormatAdvancedWriter' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_FormatAdvancedWriterZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_PyUnicodeWriter_WriteSubstring' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicodeWriter_WriteSubstringZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyLong_FormatAdvancedWriter' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_FormatAdvancedWriterZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyObject_Format' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_FormatZ_iii))(u32, u32);
/* export: '_PyLong_FormatWriter' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_FormatWriterZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyLong_Format' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_FormatZ_iii))(u32, u32);
/* export: '_PyUnicode_IsPrintable' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_IsPrintableZ_ii))(u32);
/* export: '_PyUnicode_IsAlpha' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_IsAlphaZ_ii))(u32);
/* export: '_PyUnicode_IsDecimalDigit' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_IsDecimalDigitZ_ii))(u32);
/* export: '_PyUnicode_IsDigit' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_IsDigitZ_ii))(u32);
/* export: '_PyUnicode_IsNumeric' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_IsNumericZ_ii))(u32);
/* export: '_PyUnicode_IsUppercase' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_IsUppercaseZ_ii))(u32);
/* export: '_PyUnicode_IsTitlecase' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_IsTitlecaseZ_ii))(u32);
/* export: '_PyUnicode_IsLowercase' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_IsLowercaseZ_ii))(u32);
/* export: '_Py_bytes_upper' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_bytes_upperZ_viii))(u32, u32, u32);
/* export: '_PyUnicode_ToUpperFull' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_ToUpperFullZ_iii))(u32, u32);
/* export: '_PyUnicode_IsCaseIgnorable' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_IsCaseIgnorableZ_ii))(u32);
/* export: '_PyUnicode_IsCased' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_IsCasedZ_ii))(u32);
/* export: '_PyUnicode_ToLowerFull' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_ToLowerFullZ_iii))(u32, u32);
/* export: 'PyUnicodeTranslateError_SetStart' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeTranslateError_SetStartZ_iii))(u32, u32);
/* export: 'PyUnicodeTranslateError_SetEnd' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeTranslateError_SetEndZ_iii))(u32, u32);
/* export: 'PyUnicodeTranslateError_SetReason' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicodeTranslateError_SetReasonZ_iii))(u32, u32);
/* export: '_PyUnicodeTranslateError_Create' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicodeTranslateError_CreateZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyArg_ParseStackAndKeywords_SizeT' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyArg_ParseStackAndKeywords_SizeTZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyUnicode_Splitlines' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_SplitlinesZ_iii))(u32, u32);
/* export: '_PyUnicode_IsLinebreak' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_IsLinebreakZ_ii))(u32);
/* export: 'PyUnicode_RPartition' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_RPartitionZ_iii))(u32, u32);
/* export: '_Py_bytes_lower' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_bytes_lowerZ_viii))(u32, u32, u32);
/* export: 'PyUnicode_Partition' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_PartitionZ_iii))(u32, u32);
/* export: '_PyUnicode_ToTitleFull' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_ToTitleFullZ_iii))(u32, u32);
/* export: '_PyUnicode_ToFoldedFull' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_ToFoldedFullZ_iii))(u32, u32);
/* export: 'PyList_Reverse' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyList_ReverseZ_ii))(u32);
/* export: 'PyUnicode_Format' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_FormatZ_iii))(u32, u32);
/* export: 'PyMapping_Check' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMapping_CheckZ_ii))(u32);
/* export: '_PyUnicode_FormatLong' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_FormatLongZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyNumber_ToBase' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_ToBaseZ_iii))(u32, u32);
/* export: 'PyDict_DelItem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_DelItemZ_iii))(u32, u32);
/* export: '_PyDict_DelItem_KnownHash' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_DelItem_KnownHashZ_iiii))(u32, u32, u32);
/* export: '_PyUnicode_EncodeUTF16' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_EncodeUTF16Z_iiii))(u32, u32, u32);
/* export: '_PyUnicode_EncodeUTF32' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_EncodeUTF32Z_iiii))(u32, u32, u32);
/* export: '_PyCodec_EncodeText' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyCodec_EncodeTextZ_iiii))(u32, u32, u32);
/* export: 'PyEval_GetBuiltins' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyEval_GetBuiltinsZ_iv))(void);
/* export: '_PyWeakref_ClearRef' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyWeakref_ClearRefZ_vi))(u32);
/* export: 'PyDict_Update' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_UpdateZ_iii))(u32, u32);
/* export: '_Py_bytes_title' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_bytes_titleZ_viii))(u32, u32, u32);
/* export: '_Py_bytes_swapcase' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_bytes_swapcaseZ_viii))(u32, u32, u32);
/* export: '_Py_bytes_startswith' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_startswithZ_iiii))(u32, u32, u32);
/* export: '_Py_bytes_rindex' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_rindexZ_iiii))(u32, u32, u32);
/* export: '_Py_bytes_rfind' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_rfindZ_iiii))(u32, u32, u32);
/* export: '_Py_bytes_maketrans' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_maketransZ_iii))(u32, u32);
/* export: '_Py_bytes_isupper' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_isupperZ_iii))(u32, u32);
/* export: '_Py_bytes_istitle' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_istitleZ_iii))(u32, u32);
/* export: '_Py_bytes_isspace' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_isspaceZ_iii))(u32, u32);
/* export: '_Py_bytes_islower' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_islowerZ_iii))(u32, u32);
/* export: '_Py_bytes_isdigit' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_isdigitZ_iii))(u32, u32);
/* export: '_Py_bytes_isascii' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_isasciiZ_iii))(u32, u32);
/* export: '_Py_bytes_isalpha' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_isalphaZ_iii))(u32, u32);
/* export: '_Py_bytes_isalnum' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_isalnumZ_iii))(u32, u32);
/* export: '_Py_bytes_index' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_indexZ_iiii))(u32, u32, u32);
/* export: '_PyBytes_FromHex' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyBytes_FromHexZ_iii))(u32, u32);
/* export: '_Py_bytes_find' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_findZ_iiii))(u32, u32, u32);
/* export: '_Py_bytes_endswith' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_endswithZ_iiii))(u32, u32, u32);
/* export: '_Py_bytes_count' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_countZ_iiii))(u32, u32, u32);
/* export: '_Py_bytes_capitalize' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_bytes_capitalizeZ_viii))(u32, u32, u32);
/* export: 'PyBytes_Repr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBytes_ReprZ_iii))(u32, u32);
/* export: '_Py_bytes_contains' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_bytes_containsZ_iiii))(u32, u32, u32);
/* export: '_PyBytes_FormatEx' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyBytes_FormatExZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyLong_FormatBytesWriter' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_FormatBytesWriterZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyUnicode_DecodeLatin1' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeLatin1Z_iiii))(u32, u32, u32);
/* export: 'PyByteArray_Concat' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyByteArray_ConcatZ_iii))(u32, u32);
/* export: '_PyType_Name' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyType_NameZ_ii))(u32);
/* export: '_PyLong_Copy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_CopyZ_ii))(u32);
/* export: '_PyLong_DivmodNear' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_DivmodNearZ_iii))(u32, u32);
/* export: '_PyArg_ParseStackAndKeywords' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyArg_ParseStackAndKeywordsZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyObject_Bytes' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_BytesZ_ii))(u32);
/* export: '_PyLong_FromByteArray' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_FromByteArrayZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyTuple_SetItem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyTuple_SetItemZ_iiii))(u32, u32, u32);
/* export: 'PyMapping_Keys' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMapping_KeysZ_ii))(u32);
/* export: '_PyDict_FromKeys' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_FromKeysZ_iiii))(u32, u32, u32);
/* export: '_PyDictView_Intersect' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDictView_IntersectZ_iii))(u32, u32);
/* export: '_PyDict_Pop_KnownHash' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_Pop_KnownHashZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyDict_Contains' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_ContainsZ_iii))(u32, u32);
/* export: '_PyUnicodeWriter_WriteChar' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicodeWriter_WriteCharZ_iii))(u32, u32);
/* export: '_PyObject_GenericSetAttrWithDict' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_GenericSetAttrWithDictZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyObjectDict_SetItem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObjectDict_SetItemZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyObject_NextNotImplemented' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_NextNotImplementedZ_ii))(u32);
/* export: '_PyEval_SliceIndexNotNone' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyEval_SliceIndexNotNoneZ_iii))(u32, u32);
/* export: '_PyType_CalculateMetaclass' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyType_CalculateMetaclassZ_iii))(u32, u32);
/* export: '_Py_Mangle' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_MangleZ_iii))(u32, u32);
/* export: 'PyList_Sort' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyList_SortZ_ii))(u32);
/* export: '_PyDict_DelItemId' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_DelItemIdZ_iii))(u32, u32);
/* export: 'PyClassMethod_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyClassMethod_NewZ_ii))(u32);
/* export: 'PyCell_Set' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCell_SetZ_iii))(u32, u32);
/* export: '_PyDict_NewKeysForClass' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_NewKeysForClassZ_iv))(void);
/* export: 'PyUnicode_CopyCharacters' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_CopyCharactersZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyDictProxy_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDictProxy_NewZ_ii))(u32);
/* export: '_PyDict_KeysSize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_KeysSizeZ_ii))(u32);
/* export: '_PyObject_RealIsInstance' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_RealIsInstanceZ_iii))(u32, u32);
/* export: '_PyDictKeys_DecRef' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyDictKeys_DecRefZ_vi))(u32);
/* export: 'PySys_GetObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySys_GetObjectZ_ii))(u32);
/* export: '_PyMem_IsFreed' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyMem_IsFreedZ_iii))(u32, u32);
/* export: 'PyGILState_Ensure' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyGILState_EnsureZ_iv))(void);
/* export: 'PyObject_Print' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_PrintZ_iiii))(u32, u32, u32);
/* export: 'PyGILState_Release' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyGILState_ReleaseZ_vi))(u32);
/* export: 'PyThread_tss_get' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyThread_tss_getZ_ii))(u32);
/* export: 'PyThreadState_Clear' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyThreadState_ClearZ_vi))(u32);
/* export: 'PyThread_tss_set' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyThread_tss_setZ_iii))(u32, u32);
/* export: 'PyEval_ReleaseLock' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyEval_ReleaseLockZ_vv))(void);
/* export: 'PyEval_InitThreads' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyEval_InitThreadsZ_vv))(void);
/* export: 'PyOS_vsnprintf' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyOS_vsnprintfZ_iiiii))(u32, u32, u32, u32);
/* export: 'Py_Exit' */
extern void (*WASM_RT_ADD_PREFIX(Z_Py_ExitZ_vi))(u32);
/* export: 'Py_FinalizeEx' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_Py_FinalizeExZ_iv))(void);
/* export: '_PyImport_GetModuleId' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyImport_GetModuleIdZ_ii))(u32);
/* export: 'PyOS_FiniInterrupts' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyOS_FiniInterruptsZ_vv))(void);
/* export: '_PyGC_CollectIfEnabled' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyGC_CollectIfEnabledZ_iv))(void);
/* export: 'PyImport_Cleanup' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyImport_CleanupZ_vv))(void);
/* export: '_PyTraceMalloc_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyTraceMalloc_FiniZ_vv))(void);
/* export: '_PyImport_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyImport_FiniZ_vv))(void);
/* export: '_PyType_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyType_FiniZ_vv))(void);
/* export: '_PyFaulthandler_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyFaulthandler_FiniZ_vv))(void);
/* export: '_PyHash_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyHash_FiniZ_vv))(void);
/* export: 'PyInterpreterState_Clear' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyInterpreterState_ClearZ_vi))(u32);
/* export: '_PyExc_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyExc_FiniZ_vv))(void);
/* export: 'PyMethod_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyMethod_FiniZ_vv))(void);
/* export: 'PyFrame_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyFrame_FiniZ_vv))(void);
/* export: 'PyCFunction_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyCFunction_FiniZ_vv))(void);
/* export: 'PyTuple_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyTuple_FiniZ_vv))(void);
/* export: 'PyList_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyList_FiniZ_vv))(void);
/* export: 'PySet_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z_PySet_FiniZ_vv))(void);
/* export: 'PyBytes_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyBytes_FiniZ_vv))(void);
/* export: 'PyByteArray_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyByteArray_FiniZ_vv))(void);
/* export: 'PyLong_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyLong_FiniZ_vv))(void);
/* export: 'PyFloat_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyFloat_FiniZ_vv))(void);
/* export: 'PyDict_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyDict_FiniZ_vv))(void);
/* export: 'PySlice_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z_PySlice_FiniZ_vv))(void);
/* export: '_PyGC_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyGC_FiniZ_vv))(void);
/* export: '_Py_HashRandomization_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_HashRandomization_FiniZ_vv))(void);
/* export: '_PyArg_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyArg_FiniZ_vv))(void);
/* export: 'PyAsyncGen_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyAsyncGen_FiniZ_vv))(void);
/* export: '_PyContext_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyContext_FiniZ_vv))(void);
/* export: '_PyUnicode_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyUnicode_FiniZ_vv))(void);
/* export: 'PyGrammar_RemoveAccelerators' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyGrammar_RemoveAcceleratorsZ_vi))(u32);
/* export: '_PyGILState_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyGILState_FiniZ_vv))(void);
/* export: 'PyInterpreterState_Delete' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyInterpreterState_DeleteZ_vi))(u32);
/* export: '_PyRuntimeState_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyRuntimeState_FiniZ_vi))(u32);
/* export: '_PyMem_SetDefaultAllocator' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyMem_SetDefaultAllocatorZ_iii))(u32, u32);
/* export: 'PyThread_free_lock' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyThread_free_lockZ_vi))(u32);
/* export: 'PyMem_SetAllocator' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyMem_SetAllocatorZ_vii))(u32, u32);
/* export: 'PyThread_tss_delete' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyThread_tss_deleteZ_vi))(u32);
/* export: '_PyHamt_Fini' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyHamt_FiniZ_vv))(void);
/* export: '_PyCoreConfig_Clear' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyCoreConfig_ClearZ_vi))(u32);
/* export: '_PyMainInterpreterConfig_Clear' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyMainInterpreterConfig_ClearZ_vi))(u32);
/* export: '_Py_hashtable_clear' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_hashtable_clearZ_vi))(u32);
/* export: '_Py_hashtable_foreach' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_hashtable_foreachZ_iiii))(u32, u32, u32);
/* export: '_Py_hashtable_destroy' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_hashtable_destroyZ_vi))(u32);
/* export: '_PyState_ClearModules' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyState_ClearModulesZ_vv))(void);
/* export: '_PyGC_CollectNoFail' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyGC_CollectNoFailZ_iv))(void);
/* export: '_PyGC_DumpShutdownStats' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyGC_DumpShutdownStatsZ_vv))(void);
/* export: '_PyModule_Clear' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyModule_ClearZ_vi))(u32);
/* export: '_PyModule_ClearDict' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyModule_ClearDictZ_vi))(u32);
/* export: 'PyModule_GetDef' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModule_GetDefZ_ii))(u32);
/* export: 'PyOS_setsig' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyOS_setsigZ_iii))(u32, u32);
/* export: 'python_init' */
extern void (*WASM_RT_ADD_PREFIX(Z_python_initZ_vv))(void);
/* export: 'Py_InitializeEx' */
extern void (*WASM_RT_ADD_PREFIX(Z_Py_InitializeExZ_vi))(u32);
/* export: 'PyRun_SimpleStringFlags' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyRun_SimpleStringFlagsZ_iii))(u32, u32);
/* export: 'PyImport_AddModule' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyImport_AddModuleZ_ii))(u32);
/* export: 'PyRun_StringFlags' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyRun_StringFlagsZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyArena_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyArena_NewZ_iv))(void);
/* export: 'PyParser_ASTFromStringObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyParser_ASTFromStringObjectZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyAST_CompileObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyAST_CompileObjectZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyEval_EvalCode' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyEval_EvalCodeZ_iiii))(u32, u32, u32);
/* export: 'PyArena_Free' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyArena_FreeZ_vi))(u32);
/* export: 'PyFuture_FromASTObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyFuture_FromASTObjectZ_iii))(u32, u32);
/* export: '_PyAST_Optimize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyAST_OptimizeZ_iiii))(u32, u32, u32);
/* export: 'PySymtable_BuildObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySymtable_BuildObjectZ_iiii))(u32, u32, u32);
/* export: 'PyCapsule_GetPointer' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCapsule_GetPointerZ_iii))(u32, u32);
/* export: 'PySequence_DelItem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySequence_DelItemZ_iii))(u32, u32);
/* export: 'PySymtable_Free' */
extern void (*WASM_RT_ADD_PREFIX(Z_PySymtable_FreeZ_vi))(u32);
/* export: 'PyCode_Optimize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCode_OptimizeZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyST_GetScope' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyST_GetScopeZ_iii))(u32, u32);
/* export: 'PyErr_ProgramTextObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_ProgramTextObjectZ_iii))(u32, u32);
/* export: '_Py_fopen_obj' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_fopen_objZ_iii))(u32, u32);
/* export: 'PyUnicode_FSConverter' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_FSConverterZ_iii))(u32, u32);
/* export: 'PyErr_SetFromErrnoWithFilenameObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_SetFromErrnoWithFilenameObjectZ_iii))(u32, u32);
/* export: 'PyOS_FSPath' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyOS_FSPathZ_ii))(u32);
/* export: '_Py_Attribute' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_AttributeZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_Py_Subscript' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_SubscriptZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: 'PyErr_WarnExplicitObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_WarnExplicitObjectZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_PyAST_ExprAsUnicode' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyAST_ExprAsUnicodeZ_ii))(u32);
/* export: 'PyUnicode_Find' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_FindZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyUnicode_Replace' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_ReplaceZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyArena_Malloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyArena_MallocZ_iii))(u32, u32);
/* export: 'PySymtable_Lookup' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySymtable_LookupZ_iii))(u32, u32);
/* export: 'PyCapsule_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCapsule_NewZ_iiii))(u32, u32, u32);
/* export: 'PyUnicode_Append' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyUnicode_AppendZ_vii))(u32, u32);
/* export: 'Py_GetRecursionLimit' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_Py_GetRecursionLimitZ_iv))(void);
/* export: 'PySet_Discard' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySet_DiscardZ_iii))(u32, u32);
/* export: 'PyErr_SyntaxLocationObject' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyErr_SyntaxLocationObjectZ_viii))(u32, u32, u32);
/* export: '_PyObject_SetAttrId' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_SetAttrIdZ_iiii))(u32, u32, u32);
/* export: 'PyArena_AddPyObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyArena_AddPyObjectZ_iii))(u32, u32);
/* export: '_Py_asdl_seq_new' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_asdl_seq_newZ_iii))(u32, u32);
/* export: '_Py_JoinedStr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_JoinedStrZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyParser_ParseStringObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyParser_ParseStringObjectZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: 'PyAST_FromNodeObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyAST_FromNodeObjectZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyNode_Free' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyNode_FreeZ_vi))(u32);
/* export: '_Py_Module' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_ModuleZ_iii))(u32, u32);
/* export: '_Py_Expression' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_ExpressionZ_iii))(u32, u32);
/* export: '_Py_Pass' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_PassZ_iiii))(u32, u32, u32);
/* export: '_Py_Interactive' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_InteractiveZ_iii))(u32, u32);
/* export: '_Py_Tuple' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_TupleZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_arguments' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_argumentsZ_iiiiiiii))(u32, u32, u32, u32, u32, u32, u32);
/* export: '_Py_Lambda' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_LambdaZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_IfExp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_IfExpZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_Py_BoolOp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_BoolOpZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_UnaryOp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_UnaryOpZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_asdl_int_seq_new' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_asdl_int_seq_newZ_iii))(u32, u32);
/* export: '_Py_Compare' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_CompareZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_Py_Starred' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_StarredZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_BinOp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_BinOpZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_Py_YieldFrom' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_YieldFromZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_Yield' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_YieldZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_NameConstant' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_NameConstantZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_Name' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_NameZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_PyBytes_DecodeEscape' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyBytes_DecodeEscapeZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: 'PyBytes_ConcatAndDel' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyBytes_ConcatAndDelZ_vii))(u32, u32);
/* export: 'PyUnicode_AppendAndDel' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyUnicode_AppendAndDelZ_vii))(u32, u32);
/* export: '_Py_Bytes' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_BytesZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_Num' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_NumZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_Ellipsis' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_EllipsisZ_iiii))(u32, u32, u32);
/* export: '_Py_GeneratorExp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_GeneratorExpZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_List' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_ListZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_ListComp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_ListCompZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_Dict' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_DictZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_Set' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_SetZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_SetComp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_SetCompZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_DictComp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_DictCompZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_Py_Await' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_AwaitZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_Call' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_CallZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_Py_ExtSlice' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_ExtSliceZ_iii))(u32, u32);
/* export: '_Py_Index' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_IndexZ_iii))(u32, u32);
/* export: '_Py_Slice' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_SliceZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_keyword' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_keywordZ_iiii))(u32, u32, u32);
/* export: '_Py_comprehension' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_comprehensionZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyOS_strtoul' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyOS_strtoulZ_iiii))(u32, u32, u32);
/* export: 'PyOS_strtol' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyOS_strtolZ_iiii))(u32, u32, u32);
/* export: '_Py_Str' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_StrZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyBytes_Concat' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyBytes_ConcatZ_vii))(u32, u32);
/* export: 'PyParser_SimpleParseStringFlagsFilename' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyParser_SimpleParseStringFlagsFilenameZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_FormattedValue' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_FormattedValueZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: 'PyParser_ParseStringFlagsFilename' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyParser_ParseStringFlagsFilenameZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: 'PyTokenizer_FromUTF8' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyTokenizer_FromUTF8Z_iii))(u32, u32);
/* export: 'PyTokenizer_FromString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyTokenizer_FromStringZ_iii))(u32, u32);
/* export: 'PyParser_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyParser_NewZ_iii))(u32, u32);
/* export: 'PyParser_AddToken' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyParser_AddTokenZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: 'PyParser_Delete' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyParser_DeleteZ_vi))(u32);
/* export: 'PyNode_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNode_NewZ_ii))(u32);
/* export: 'PyGrammar_FindDFA' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyGrammar_FindDFAZ_iii))(u32, u32);
/* export: 'PyNode_AddChild' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNode_AddChildZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyGrammar_AddAccelerators' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyGrammar_AddAcceleratorsZ_vi))(u32);
/* export: '_PyUnicode_DecodeUnicodeEscape' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_DecodeUnicodeEscapeZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_arg' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_argZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_Expr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_ExprZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_AugAssign' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_AugAssignZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_Py_AnnAssign' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_AnnAssignZ_iiiiiiii))(u32, u32, u32, u32, u32, u32, u32);
/* export: '_Py_Assign' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_AssignZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_Delete' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_DeleteZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_Break' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_BreakZ_iiii))(u32, u32, u32);
/* export: '_Py_Continue' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_ContinueZ_iiii))(u32, u32, u32);
/* export: '_Py_Return' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_ReturnZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_Raise' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_RaiseZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_Import' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_ImportZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_ImportFrom' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_ImportFromZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_Py_Global' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_GlobalZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_Nonlocal' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_NonlocalZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_Assert' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_AssertZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_If' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_IfZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_Py_While' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_WhileZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_Py_ExceptHandler' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_ExceptHandlerZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: '_Py_Try' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_TryZ_iiiiiiii))(u32, u32, u32, u32, u32, u32, u32);
/* export: '_Py_ClassDef' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_ClassDefZ_iiiiiiiii))(u32, u32, u32, u32, u32, u32, u32, u32);
/* export: '_Py_AsyncFunctionDef' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_AsyncFunctionDefZ_iiiiiiiii))(u32, u32, u32, u32, u32, u32, u32, u32);
/* export: '_Py_FunctionDef' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_FunctionDefZ_iiiiiiiii))(u32, u32, u32, u32, u32, u32, u32, u32);
/* export: '_Py_withitem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_withitemZ_iiii))(u32, u32, u32);
/* export: '_Py_AsyncWith' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_AsyncWithZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_With' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_WithZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_AsyncFor' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_AsyncForZ_iiiiiiii))(u32, u32, u32, u32, u32, u32, u32);
/* export: '_Py_For' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_ForZ_iiiiiiii))(u32, u32, u32, u32, u32, u32, u32);
/* export: '_Py_alias' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_aliasZ_iiii))(u32, u32, u32);
/* export: '_PyImport_AddModuleObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyImport_AddModuleObjectZ_iii))(u32, u32);
/* export: 'PyModule_NewObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModule_NewObjectZ_ii))(u32);
/* export: '_Py_InitializeFromConfig' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_InitializeFromConfigZ_vii))(u32, u32);
/* export: '_Py_FatalInitError' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_FatalInitErrorZ_vi))(u32);
/* export: 'PyGILState_GetThisThreadState' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyGILState_GetThisThreadStateZ_iv))(void);
/* export: '_Py_DumpTracebackThreads' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_DumpTracebackThreadsZ_iiii))(u32, u32, u32);
/* export: '_PyGILState_GetInterpreterStateUnsafe' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyGILState_GetInterpreterStateUnsafeZ_iv))(void);
/* export: 'PyInterpreterState_ThreadHead' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInterpreterState_ThreadHeadZ_ii))(u32);
/* export: '_Py_write_noraise' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_write_noraiseZ_iiii))(u32, u32, u32);
/* export: 'PyThreadState_Next' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyThreadState_NextZ_ii))(u32);
/* export: '_Py_DumpASCII' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_DumpASCIIZ_vii))(u32, u32);
/* export: '_Py_Initialize_ReadEnvVarsNoAlloc' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_Initialize_ReadEnvVarsNoAllocZ_vv))(void);
/* export: '_Py_InitializeCore' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_InitializeCoreZ_viii))(u32, u32, u32);
/* export: '_PyMainInterpreterConfig_Read' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyMainInterpreterConfig_ReadZ_viii))(u32, u32, u32);
/* export: '_Py_InitializeMainInterpreter' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_InitializeMainInterpreterZ_viii))(u32, u32, u32);
/* export: '_PyMainInterpreterConfig_Copy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyMainInterpreterConfig_CopyZ_iii))(u32, u32);
/* export: '_PyTime_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyTime_InitZ_iv))(void);
/* export: '_PyFaulthandler_Init' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyFaulthandler_InitZ_vii))(u32, u32);
/* export: 'PyList_GetSlice' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyList_GetSliceZ_iiii))(u32, u32, u32);
/* export: '_Py_ResetForceASCII' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_ResetForceASCIIZ_vv))(void);
/* export: '_PyCoreConfig_Copy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyCoreConfig_CopyZ_iii))(u32, u32);
/* export: '_PyCoreConfig_Read' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyCoreConfig_ReadZ_vii))(u32, u32);
/* export: '_Py_InitializeCore_impl' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_InitializeCore_implZ_viii))(u32, u32, u32);
/* export: '_PyMem_GetAllocatorsName' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyMem_GetAllocatorsNameZ_iv))(void);
/* export: '_PyCoreConfig_SetGlobalConfig' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyCoreConfig_SetGlobalConfigZ_vi))(u32);
/* export: '_PyRuntimeState_Init' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyRuntimeState_InitZ_vii))(u32, u32);
/* export: '_PyMem_SetupAllocators' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyMem_SetupAllocatorsZ_ii))(u32);
/* export: '_Py_HashRandomization_Init' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_HashRandomization_InitZ_vii))(u32, u32);
/* export: '_PyInterpreterState_Enable' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyInterpreterState_EnableZ_vii))(u32, u32);
/* export: 'PyInterpreterState_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInterpreterState_NewZ_iv))(void);
/* export: 'PyThreadState_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyThreadState_NewZ_ii))(u32);
/* export: '_PyEval_FiniThreads' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyEval_FiniThreadsZ_vv))(void);
/* export: '_PyGILState_Init' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyGILState_InitZ_vii))(u32, u32);
/* export: '_Py_ReadyTypes' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_ReadyTypesZ_vv))(void);
/* export: '_PyFrame_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyFrame_InitZ_iv))(void);
/* export: '_PyLong_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_InitZ_iv))(void);
/* export: 'PyByteArray_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyByteArray_InitZ_iv))(void);
/* export: '_PyFloat_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyFloat_InitZ_iv))(void);
/* export: '_PySys_BeginInit' */
extern void (*WASM_RT_ADD_PREFIX(Z__PySys_BeginInitZ_vii))(u32, u32);
/* export: '_PyImport_FixupBuiltin' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyImport_FixupBuiltinZ_iiii))(u32, u32, u32);
/* export: '_PyUnicode_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_InitZ_iv))(void);
/* export: '_PyStructSequence_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyStructSequence_InitZ_iv))(void);
/* export: '_PyBuiltin_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyBuiltin_InitZ_iv))(void);
/* export: '_PyExc_Init' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyExc_InitZ_vi))(u32);
/* export: 'PyFile_NewStdPrinter' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyFile_NewStdPrinterZ_ii))(u32);
/* export: 'PySys_SetObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySys_SetObjectZ_iii))(u32, u32);
/* export: '_PyImport_Init' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyImport_InitZ_vii))(u32, u32);
/* export: '_PyImportHooks_Init' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyImportHooks_InitZ_vi))(u32);
/* export: '_PyWarnings_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyWarnings_InitZ_iv))(void);
/* export: '_PyContext_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyContext_InitZ_iv))(void);
/* export: 'PyImport_ImportFrozenModule' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyImport_ImportFrozenModuleZ_ii))(u32);
/* export: 'PyInit__imp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__impZ_iv))(void);
/* export: '_PyImport_SetModuleString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyImport_SetModuleStringZ_iii))(u32, u32);
/* export: 'PyMapping_SetItemString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMapping_SetItemStringZ_iiii))(u32, u32, u32);
/* export: 'PyModule_Create2' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModule_Create2Z_iii))(u32, u32);
/* export: '_PyImport_IsInitialized' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyImport_IsInitializedZ_ii))(u32);
/* export: '_PyModule_CreateInitialized' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyModule_CreateInitializedZ_iii))(u32, u32);
/* export: 'PyModule_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModule_NewZ_ii))(u32);
/* export: 'PyModule_AddFunctions' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModule_AddFunctionsZ_iii))(u32, u32);
/* export: 'PyObject_SetAttrString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_SetAttrStringZ_iiii))(u32, u32, u32);
/* export: '_Py_KeyedHash' */
extern u64 (*WASM_RT_ADD_PREFIX(Z__Py_KeyedHashZ_jjii))(u64, u32, u32);
/* export: 'PyModule_GetState' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModule_GetStateZ_ii))(u32);
/* export: 'PyModule_ExecDef' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModule_ExecDefZ_iii))(u32, u32);
/* export: 'PyInit_array' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit_arrayZ_iv))(void);
/* export: 'PyInit_math' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit_mathZ_iv))(void);
/* export: 'PyInit_cmath' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit_cmathZ_iv))(void);
/* export: 'PyInit__struct' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__structZ_iv))(void);
/* export: 'PyInit__bisect' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__bisectZ_iv))(void);
/* export: 'PyInit__datetime' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__datetimeZ_iv))(void);
/* export: 'PyInit__json' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__jsonZ_iv))(void);
/* export: 'PyInit__pickle' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__pickleZ_iv))(void);
/* export: 'PyInit_binascii' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit_binasciiZ_iv))(void);
/* export: 'PyInit__sha1' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__sha1Z_iv))(void);
/* export: 'PyInit__sha256' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__sha256Z_iv))(void);
/* export: 'PyInit__sha512' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__sha512Z_iv))(void);
/* export: 'PyInit__sha3' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__sha3Z_iv))(void);
/* export: 'PyInit__md5' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__md5Z_iv))(void);
/* export: 'PyInit__blake2' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__blake2Z_iv))(void);
/* export: 'PyInit__sre' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__sreZ_iv))(void);
/* export: 'PyInit__codecs' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__codecsZ_iv))(void);
/* export: 'PyInit__weakref' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__weakrefZ_iv))(void);
/* export: 'PyInit__abc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__abcZ_iv))(void);
/* export: 'PyInit_time' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit_timeZ_iv))(void);
/* export: 'PyInit__io' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__ioZ_iv))(void);
/* export: 'PyMarshal_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMarshal_InitZ_iv))(void);
/* export: 'PyInit__string' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__stringZ_iv))(void);
/* export: 'PyInit__db' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyInit__dbZ_iv))(void);
/* export: 'to_name' */
extern u64 (*WASM_RT_ADD_PREFIX(Z_to_nameZ_ji))(u32);
/* export: 'parse_db_args4' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_parse_db_args4Z_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'parse_db_args5' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_parse_db_args5Z_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: 'PyModule_AddIntConstant' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModule_AddIntConstantZ_iiii))(u32, u32, u32);
/* export: 'PyModule_AddObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModule_AddObjectZ_iiii))(u32, u32, u32);
/* export: '_PyLong_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_NewZ_ii))(u32);
/* export: '_PyFloat_Unpack8' */
extern f64 (*WASM_RT_ADD_PREFIX(Z__PyFloat_Unpack8Z_dii))(u32, u32);
/* export: 'PyMarshal_WriteObjectToString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMarshal_WriteObjectToStringZ_iii))(u32, u32);
/* export: '_Py_hashtable_hash_ptr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_hashtable_hash_ptrZ_iii))(u32, u32);
/* export: '_Py_hashtable_compare_direct' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_hashtable_compare_directZ_iiii))(u32, u32, u32);
/* export: '_Py_hashtable_new' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_hashtable_newZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_hashtable_get_entry' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_hashtable_get_entryZ_iiii))(u32, u32, u32);
/* export: '_Py_hashtable_set' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_hashtable_setZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_PyFloat_Pack8' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyFloat_Pack8Z_idii))(f64, u32, u32);
/* export: '_PyArg_ParseTupleAndKeywordsFast_SizeT' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyArg_ParseTupleAndKeywordsFast_SizeTZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyObject_CallMethodObjArgs' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_CallMethodObjArgsZ_iiii))(u32, u32, u32);
/* export: '_PyIncrementalNewlineDecoder_decode' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyIncrementalNewlineDecoder_decodeZ_iiii))(u32, u32, u32);
/* export: '_PyIO_get_module_state' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyIO_get_module_stateZ_iv))(void);
/* export: '_Py_device_encoding' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_device_encodingZ_ii))(u32);
/* export: '_PyIO_get_locale_module' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyIO_get_locale_moduleZ_ii))(u32);
/* export: '_PyCodecInfo_GetIncrementalEncoder' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyCodecInfo_GetIncrementalEncoderZ_iii))(u32, u32);
/* export: '_PyUnicode_AsLatin1String' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_AsLatin1StringZ_iii))(u32, u32);
/* export: '_PyCodecInfo_GetIncrementalDecoder' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyCodecInfo_GetIncrementalDecoderZ_iii))(u32, u32);
/* export: 'PyState_FindModule' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyState_FindModuleZ_ii))(u32);
/* export: '_PyFileIO_closed' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyFileIO_closedZ_ii))(u32);
/* export: '_PyIOBase_check_closed' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyIOBase_check_closedZ_iii))(u32, u32);
/* export: '_PyBytes_Join' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyBytes_JoinZ_iii))(u32, u32);
/* export: '_PyIO_trap_eintr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyIO_trap_eintrZ_iv))(void);
/* export: '_PyIO_find_line_ending' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyIO_find_line_endingZ_iiiiiiii))(u32, u32, u32, u32, u32, u32, u32);
/* export: '_Py_convert_optional_to_ssize_t' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_convert_optional_to_ssize_tZ_iii))(u32, u32);
/* export: '_PyIOBase_finalize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyIOBase_finalizeZ_ii))(u32);
/* export: 'PyObject_CallFinalizer' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyObject_CallFinalizerZ_vi))(u32);
/* export: '_PyIOBase_check_seekable' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyIOBase_check_seekableZ_iii))(u32, u32);
/* export: '_PyIOBase_check_readable' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyIOBase_check_readableZ_iii))(u32, u32);
/* export: '_PyIOBase_check_writable' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyIOBase_check_writableZ_iii))(u32, u32);
/* export: 'PyNumber_AsOff_t' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_AsOff_tZ_iii))(u32, u32);
/* export: 'PyObject_CallObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_CallObjectZ_iii))(u32, u32);
/* export: '_PyAccu_Destroy' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyAccu_DestroyZ_vi))(u32);
/* export: '_PyAccu_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyAccu_InitZ_ii))(u32);
/* export: '_PyAccu_Accumulate' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyAccu_AccumulateZ_iii))(u32, u32);
/* export: 'PyUnicode_AsUCS4' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_AsUCS4Z_iiiii))(u32, u32, u32, u32);
/* export: '_PyAccu_Finish' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyAccu_FinishZ_ii))(u32);
/* export: 'PyUnicode_AsUCS4Copy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_AsUCS4CopyZ_ii))(u32);
/* export: '_PySys_GetSizeOf' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PySys_GetSizeOfZ_ii))(u32);
/* export: '_Py_set_inheritable' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_set_inheritableZ_iiii))(u32, u32, u32);
/* export: '_Py_fstat_noraise' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_fstat_noraiseZ_iii))(u32, u32);
/* export: 'PyErr_ResourceWarning' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_ResourceWarningZ_iiiii))(u32, u32, u32, u32);
/* export: '_Py_write' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_writeZ_iiii))(u32, u32, u32);
/* export: '_Py_read' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_readZ_iiii))(u32, u32, u32);
/* export: '_PyTime_localtime' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyTime_localtimeZ_iii))(u32, u32);
/* export: 'PyStructSequence_InitType2' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyStructSequence_InitType2Z_iii))(u32, u32);
/* export: '_PyTime_GetSystemClockWithInfo' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyTime_GetSystemClockWithInfoZ_iii))(u32, u32);
/* export: '_PyTime_MulDiv' */
extern u64 (*WASM_RT_ADD_PREFIX(Z__PyTime_MulDivZ_jjjj))(u64, u64, u64);
/* export: '_PyTime_AsSecondsDouble' */
extern f64 (*WASM_RT_ADD_PREFIX(Z__PyTime_AsSecondsDoubleZ_dj))(u64);
/* export: '_PyTime_GetMonotonicClockWithInfo' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyTime_GetMonotonicClockWithInfoZ_iii))(u32, u32);
/* export: '_PyTime_GetPerfCounterWithInfo' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyTime_GetPerfCounterWithInfoZ_iii))(u32, u32);
/* export: '_PyNamespace_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyNamespace_NewZ_ii))(u32);
/* export: '_PyTime_FromTimeval' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyTime_FromTimevalZ_iii))(u32, u32);
/* export: '_PyTime_AsNanosecondsObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyTime_AsNanosecondsObjectZ_ij))(u64);
/* export: '_PyTime_GetMonotonicClock' */
extern u64 (*WASM_RT_ADD_PREFIX(Z__PyTime_GetMonotonicClockZ_jv))(void);
/* export: 'PyUnicode_EncodeLocale' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_EncodeLocaleZ_iii))(u32, u32);
/* export: 'PyUnicode_DecodeLocaleAndSize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeLocaleAndSizeZ_iiii))(u32, u32, u32);
/* export: '_PyTime_ObjectToTime_t' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyTime_ObjectToTime_tZ_iiii))(u32, u32, u32);
/* export: 'PyStructSequence_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyStructSequence_NewZ_ii))(u32);
/* export: '_PyLong_FromTime_t' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_FromTime_tZ_ii))(u32);
/* export: '_PyTime_gmtime' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyTime_gmtimeZ_iii))(u32, u32);
/* export: '_PyTime_FromSecondsObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyTime_FromSecondsObjectZ_iiii))(u32, u32, u32);
/* export: '_PyTime_AsTimeval' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyTime_AsTimevalZ_ijii))(u64, u32, u32);
/* export: '_PyTime_GetSystemClock' */
extern u64 (*WASM_RT_ADD_PREFIX(Z__PyTime_GetSystemClockZ_jv))(void);
/* export: 'PySet_Clear' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySet_ClearZ_ii))(u32);
/* export: 'PySet_Size' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySet_SizeZ_ii))(u32);
/* export: 'PyWeakref_GetObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyWeakref_GetObjectZ_ii))(u32);
/* export: 'PyMapping_Items' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMapping_ItemsZ_ii))(u32);
/* export: 'PyDict_Items' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_ItemsZ_ii))(u32);
/* export: 'PyWeakref_NewProxy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyWeakref_NewProxyZ_iii))(u32, u32);
/* export: '_PyWeakref_GetWeakrefCount' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyWeakref_GetWeakrefCountZ_ii))(u32);
/* export: '_PyDict_DelItemIf' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_DelItemIfZ_iiii))(u32, u32, u32);
/* export: '_PyCodec_Forget' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyCodec_ForgetZ_ii))(u32);
/* export: 'PyCodec_RegisterError' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCodec_RegisterErrorZ_iii))(u32, u32);
/* export: 'PyUnicode_BuildEncodingMap' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_BuildEncodingMapZ_ii))(u32);
/* export: 'PyUnicode_DecodeCharmap' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeCharmapZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyUnicode_EncodeCharmap' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_EncodeCharmapZ_iiii))(u32, u32, u32);
/* export: 'PyUnicode_DecodeRawUnicodeEscape' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeRawUnicodeEscapeZ_iiii))(u32, u32, u32);
/* export: 'PyUnicode_AsRawUnicodeEscapeString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_AsRawUnicodeEscapeStringZ_ii))(u32);
/* export: '_PyUnicode_DecodeUnicodeInternal' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_DecodeUnicodeInternalZ_iiii))(u32, u32, u32);
/* export: 'PyUnicode_DecodeUnicodeEscape' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeUnicodeEscapeZ_iiii))(u32, u32, u32);
/* export: 'PyUnicode_AsUnicodeEscapeString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_AsUnicodeEscapeStringZ_ii))(u32);
/* export: 'PyUnicode_DecodeUTF7Stateful' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeUTF7StatefulZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyUnicode_EncodeUTF7' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_EncodeUTF7Z_iiiii))(u32, u32, u32, u32);
/* export: 'PyBytes_DecodeEscape' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBytes_DecodeEscapeZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyCodec_Decode' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCodec_DecodeZ_iiii))(u32, u32, u32);
/* export: 'PyCodec_Encode' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCodec_EncodeZ_iiii))(u32, u32, u32);
/* export: 'PyCodec_Register' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCodec_RegisterZ_ii))(u32);
/* export: '_PyUnicode_ToLowercase' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_ToLowercaseZ_ii))(u32);
/* export: '_PyUnicode_ToUppercase' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyUnicode_ToUppercaseZ_ii))(u32);
/* export: 'PyObject_InitVar' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_InitVarZ_iiii))(u32, u32, u32);
/* export: '_PyDict_SetItem_KnownHash' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_SetItem_KnownHashZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyCallIter_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCallIter_NewZ_iii))(u32, u32);
/* export: 'PyBlake2_blake2s_init_param' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBlake2_blake2s_init_paramZ_iii))(u32, u32);
/* export: 'PyBlake2_blake2s_update' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBlake2_blake2s_updateZ_iiij))(u32, u32, u64);
/* export: 'PyBlake2_blake2s_final' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBlake2_blake2s_finalZ_iiii))(u32, u32, u32);
/* export: 'PyBlake2_blake2b_update' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBlake2_blake2b_updateZ_iiij))(u32, u32, u64);
/* export: 'PyBlake2_blake2b_final' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyBlake2_blake2b_finalZ_iiii))(u32, u32, u32);
/* export: 'PyModule_AddStringConstant' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModule_AddStringConstantZ_iiii))(u32, u32, u32);
/* export: '_PySHA3_KeccakWidth1600_SpongeAbsorb' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PySHA3_KeccakWidth1600_SpongeAbsorbZ_iiii))(u32, u32, u32);
/* export: '_PySHA3_KeccakP1600_AddBytes' */
extern void (*WASM_RT_ADD_PREFIX(Z__PySHA3_KeccakP1600_AddBytesZ_viiii))(u32, u32, u32, u32);
/* export: '_PySHA3_KeccakP1600_Permute_Nrounds' */
extern void (*WASM_RT_ADD_PREFIX(Z__PySHA3_KeccakP1600_Permute_NroundsZ_vii))(u32, u32);
/* export: '_PySHA3_KeccakP1600_AddLanes' */
extern void (*WASM_RT_ADD_PREFIX(Z__PySHA3_KeccakP1600_AddLanesZ_viii))(u32, u32, u32);
/* export: '_PySHA3_KeccakP1600_AddBytesInLane' */
extern void (*WASM_RT_ADD_PREFIX(Z__PySHA3_KeccakP1600_AddBytesInLaneZ_viiiii))(u32, u32, u32, u32, u32);
/* export: '_PySHA3_KeccakWidth1600_SpongeAbsorbLastFewBits' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PySHA3_KeccakWidth1600_SpongeAbsorbLastFewBitsZ_iii))(u32, u32);
/* export: '_PySHA3_KeccakWidth1600_SpongeSqueeze' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PySHA3_KeccakWidth1600_SpongeSqueezeZ_iiii))(u32, u32, u32);
/* export: '_PySHA3_KeccakP1600_ExtractBytes' */
extern void (*WASM_RT_ADD_PREFIX(Z__PySHA3_KeccakP1600_ExtractBytesZ_viiii))(u32, u32, u32, u32);
/* export: '_PySHA3_KeccakP1600_ExtractLanes' */
extern void (*WASM_RT_ADD_PREFIX(Z__PySHA3_KeccakP1600_ExtractLanesZ_viii))(u32, u32, u32);
/* export: 'PyErr_NewException' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_NewExceptionZ_iiii))(u32, u32, u32);
/* export: '_Py_strhex_bytes' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_strhex_bytesZ_iii))(u32, u32);
/* export: 'PyUnicode_Split' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_SplitZ_iiii))(u32, u32, u32);
/* export: '_PySet_Update' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PySet_UpdateZ_iii))(u32, u32);
/* export: 'PyCFunction_GetFunction' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyCFunction_GetFunctionZ_ii))(u32);
/* export: '_PyAccu_FinishAsList' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyAccu_FinishAsListZ_ii))(u32);
/* export: '_PyTime_ObjectToTimeval' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyTime_ObjectToTimevalZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyUnicode_AsLatin1String' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_AsLatin1StringZ_ii))(u32);
/* export: '_PyLong_AsTime_t' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_AsTime_tZ_ii))(u32);
/* export: 'PyUnicode_WriteChar' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_WriteCharZ_iiii))(u32, u32, u32);
/* export: '_PyTime_AsTimevalTime_t' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyTime_AsTimevalTime_tZ_ijiii))(u64, u32, u32, u32);
/* export: 'PyList_Insert' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyList_InsertZ_iiii))(u32, u32, u32);
/* export: '_PyFloat_Pack4' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyFloat_Pack4Z_idii))(f64, u32, u32);
/* export: '_PyFloat_Unpack4' */
extern f64 (*WASM_RT_ADD_PREFIX(Z__PyFloat_Unpack4Z_dii))(u32, u32);
/* export: '_PyFloat_Pack2' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyFloat_Pack2Z_idii))(f64, u32, u32);
/* export: '_PyFloat_Unpack2' */
extern f64 (*WASM_RT_ADD_PREFIX(Z__PyFloat_Unpack2Z_dii))(u32, u32);
/* export: '_Py_c_abs' */
extern f64 (*WASM_RT_ADD_PREFIX(Z__Py_c_absZ_di))(u32);
/* export: '_Py_log1p' */
extern f64 (*WASM_RT_ADD_PREFIX(Z__Py_log1pZ_dd))(f64);
/* export: '_Py_c_quot' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_c_quotZ_viii))(u32, u32, u32);
/* export: '_Py_c_diff' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_c_diffZ_viii))(u32, u32, u32);
/* export: '_Py_c_neg' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_c_negZ_vii))(u32, u32);
/* export: '_PyLong_GCD' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyLong_GCDZ_iii))(u32, u32);
/* export: 'PyModuleDef_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModuleDef_InitZ_ii))(u32);
/* export: 'PyUnicode_AsUnicode' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_AsUnicodeZ_ii))(u32);
/* export: 'PyUnicode_DecodeUTF16' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeUTF16Z_iiiii))(u32, u32, u32, u32);
/* export: 'PyUnicode_DecodeUTF32' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeUTF32Z_iiiii))(u32, u32, u32, u32);
/* export: 'PyImport_ImportFrozenModuleObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyImport_ImportFrozenModuleObjectZ_ii))(u32);
/* export: 'PyMarshal_ReadObjectFromString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMarshal_ReadObjectFromStringZ_iii))(u32, u32);
/* export: 'PyMapping_HasKey' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyMapping_HasKeyZ_iii))(u32, u32);
/* export: '_PyImport_FindExtensionObjectEx' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyImport_FindExtensionObjectExZ_iiii))(u32, u32, u32);
/* export: 'PyModule_FromDefAndSpec2' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModule_FromDefAndSpec2Z_iiii))(u32, u32, u32);
/* export: '_PyImport_FixupExtensionObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyImport_FixupExtensionObjectZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyState_AddModule' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyState_AddModuleZ_iii))(u32, u32);
/* export: 'PyList_SetItem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyList_SetItemZ_iiii))(u32, u32, u32);
/* export: '_PyImport_AcquireLock' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyImport_AcquireLockZ_vv))(void);
/* export: '_PyHamt_Init' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyHamt_InitZ_iv))(void);
/* export: 'PyContextVar_Reset' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyContextVar_ResetZ_iii))(u32, u32);
/* export: '_PyHamt_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyHamt_NewZ_iv))(void);
/* export: '_PyHamt_Without' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyHamt_WithoutZ_iii))(u32, u32);
/* export: '_PyHamt_Assoc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyHamt_AssocZ_iiii))(u32, u32, u32);
/* export: '_PyHamt_Eq' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyHamt_EqZ_iii))(u32, u32);
/* export: 'PyContextVar_Set' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyContextVar_SetZ_iii))(u32, u32);
/* export: '_PyHamt_Find' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyHamt_FindZ_iiii))(u32, u32, u32);
/* export: 'PyContextVar_Get' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyContextVar_GetZ_iiii))(u32, u32, u32);
/* export: 'PyDict_Size' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_SizeZ_ii))(u32);
/* export: '_PyObject_FastCallKeywords' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_FastCallKeywordsZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyFunction_FastCallKeywords' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyFunction_FastCallKeywordsZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyMethodDef_RawFastCallKeywords' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyMethodDef_RawFastCallKeywordsZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_PyDict_NewPresized' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_NewPresizedZ_ii))(u32);
/* export: '_PyHamt_NewIterValues' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyHamt_NewIterValuesZ_ii))(u32);
/* export: '_PyHamt_NewIterKeys' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyHamt_NewIterKeysZ_ii))(u32);
/* export: '_PyHamt_NewIterItems' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyHamt_NewIterItemsZ_ii))(u32);
/* export: '_PyHamt_Len' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyHamt_LenZ_ii))(u32);
/* export: 'PyDict_DelItemString' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyDict_DelItemStringZ_iii))(u32, u32);
/* export: 'PyObject_Not' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_NotZ_ii))(u32);
/* export: 'PyObject_Type' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_TypeZ_ii))(u32);
/* export: 'PyEval_GetLocals' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyEval_GetLocalsZ_iv))(void);
/* export: 'PyEval_MergeCompilerFlags' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyEval_MergeCompilerFlagsZ_ii))(u32);
/* export: 'PyObject_Dir' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyObject_DirZ_ii))(u32);
/* export: 'PyUnicode_FSDecoder' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_FSDecoderZ_iii))(u32, u32);
/* export: 'PyAST_Check' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyAST_CheckZ_ii))(u32);
/* export: 'PyAST_obj2mod' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyAST_obj2modZ_iiii))(u32, u32, u32);
/* export: 'PyAST_Validate' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyAST_ValidateZ_ii))(u32);
/* export: 'Py_CompileStringObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_Py_CompileStringObjectZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'PyAST_mod2obj' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyAST_mod2objZ_ii))(u32);
/* export: 'XXH64' */
extern u64 (*WASM_RT_ADD_PREFIX(Z_XXH64Z_jiij))(u32, u32, u64);
/* export: '_PyStack_AsTupleSlice' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyStack_AsTupleSliceZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyStack_AsDict' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyStack_AsDictZ_iii))(u32, u32);
/* export: 'PyEval_EvalCodeEx' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyEval_EvalCodeExZ_iiiiiiiiiiii))(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
/* export: 'Py_GetVersion' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_Py_GetVersionZ_iv))(void);
/* export: '_Py_gitidentifier' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_gitidentifierZ_iv))(void);
/* export: '_Py_gitversion' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_gitversionZ_iv))(void);
/* export: 'Py_GetCopyright' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_Py_GetCopyrightZ_iv))(void);
/* export: 'Py_GetPlatform' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_Py_GetPlatformZ_iv))(void);
/* export: 'PyFloat_GetInfo' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyFloat_GetInfoZ_iv))(void);
/* export: 'PyLong_GetInfo' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyLong_GetInfoZ_iv))(void);
/* export: 'PyHash_GetFuncDef' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyHash_GetFuncDefZ_iv))(void);
/* export: 'PyThread_GetInfo' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyThread_GetInfoZ_iv))(void);
/* export: 'PyUnicode_DecodeFSDefaultAndSize' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyUnicode_DecodeFSDefaultAndSizeZ_iii))(u32, u32);
/* export: 'Py_GetBuildInfo' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_Py_GetBuildInfoZ_iv))(void);
/* export: 'Py_GetCompiler' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_Py_GetCompilerZ_iv))(void);
/* export: '_PyEval_GetAsyncGenFirstiter' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyEval_GetAsyncGenFirstiterZ_iv))(void);
/* export: '_PyEval_GetAsyncGenFinalizer' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyEval_GetAsyncGenFinalizerZ_iv))(void);
/* export: '_PyEval_SetAsyncGenFinalizer' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyEval_SetAsyncGenFinalizerZ_vi))(u32);
/* export: '_PyEval_SetAsyncGenFirstiter' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyEval_SetAsyncGenFirstiterZ_vi))(u32);
/* export: '_PyEval_GetCoroutineWrapper' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyEval_GetCoroutineWrapperZ_iv))(void);
/* export: '_PyEval_SetCoroutineWrapper' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyEval_SetCoroutineWrapperZ_vi))(u32);
/* export: '_PyEval_GetCoroutineOriginTrackingDepth' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyEval_GetCoroutineOriginTrackingDepthZ_iv))(void);
/* export: '_PyEval_SetCoroutineOriginTrackingDepth' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyEval_SetCoroutineOriginTrackingDepthZ_vi))(u32);
/* export: '_PyObject_DebugTypeStats' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyObject_DebugTypeStatsZ_vi))(u32);
/* export: '_PyCFunction_DebugMallocStats' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyCFunction_DebugMallocStatsZ_vi))(u32);
/* export: '_PyDict_DebugMallocStats' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyDict_DebugMallocStatsZ_vi))(u32);
/* export: '_PyFloat_DebugMallocStats' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyFloat_DebugMallocStatsZ_vi))(u32);
/* export: '_PyFrame_DebugMallocStats' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyFrame_DebugMallocStatsZ_vi))(u32);
/* export: '_PyList_DebugMallocStats' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyList_DebugMallocStatsZ_vi))(u32);
/* export: '_PyMethod_DebugMallocStats' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyMethod_DebugMallocStatsZ_vi))(u32);
/* export: '_PyTuple_DebugMallocStats' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyTuple_DebugMallocStatsZ_vi))(u32);
/* export: '_PyDebugAllocatorStats' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyDebugAllocatorStatsZ_viiii))(u32, u32, u32, u32);
/* export: '_PyEval_CallTracing' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyEval_CallTracingZ_iii))(u32, u32);
/* export: 'PyEval_SetTrace' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyEval_SetTraceZ_vii))(u32, u32);
/* export: 'PyFrame_LocalsToFast' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyFrame_LocalsToFastZ_vii))(u32, u32);
/* export: 'PyTraceBack_Here' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyTraceBack_HereZ_ii))(u32);
/* export: 'Py_SetRecursionLimit' */
extern void (*WASM_RT_ADD_PREFIX(Z_Py_SetRecursionLimitZ_vi))(u32);
/* export: 'PyEval_SetProfile' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyEval_SetProfileZ_vii))(u32, u32);
/* export: '_PyEval_GetSwitchInterval' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyEval_GetSwitchIntervalZ_iv))(void);
/* export: '_PyEval_SetSwitchInterval' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyEval_SetSwitchIntervalZ_vi))(u32);
/* export: '_Py_GetAllocatedBlocks' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_GetAllocatedBlocksZ_iv))(void);
/* export: '_PyErr_GetTopmostException' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyErr_GetTopmostExceptionZ_ii))(u32);
/* export: '_PyThread_CurrentFrames' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyThread_CurrentFramesZ_iv))(void);
/* export: 'PyType_ClearCache' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyType_ClearCacheZ_iv))(void);
/* export: '_PyMem_RawStrdup' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyMem_RawStrdupZ_ii))(u32);
/* export: '_PyDictView_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDictView_NewZ_iii))(u32, u32);
/* export: '_PyDict_SizeOf' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_SizeOfZ_ii))(u32);
/* export: 'PyODict_DelItem' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyODict_DelItemZ_iii))(u32, u32);
/* export: 'PyThread_tss_create' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyThread_tss_createZ_ii))(u32);
/* export: '_PyEval_EvalFrameDefault' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyEval_EvalFrameDefaultZ_iii))(u32, u32);
/* export: 'Py_MakePendingCalls' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_Py_MakePendingCallsZ_iv))(void);
/* export: '_PyCode_CheckLineNumber' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyCode_CheckLineNumberZ_iiii))(u32, u32, u32);
/* export: 'PyNumber_MatrixMultiply' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_MatrixMultiplyZ_iii))(u32, u32);
/* export: 'PyNumber_InPlaceMatrixMultiply' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyNumber_InPlaceMatrixMultiplyZ_iii))(u32, u32);
/* export: '_PyCoro_GetAwaitableIter' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyCoro_GetAwaitableIterZ_ii))(u32);
/* export: '_PyGen_yf' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyGen_yfZ_ii))(u32);
/* export: '_PyAsyncGenValueWrapperNew' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyAsyncGenValueWrapperNewZ_ii))(u32);
/* export: 'PyFrame_BlockPop' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyFrame_BlockPopZ_ii))(u32);
/* export: '_PyDict_LoadGlobal' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_LoadGlobalZ_iiii))(u32, u32, u32);
/* export: '_PyDict_MergeEx' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyDict_MergeExZ_iiii))(u32, u32, u32);
/* export: 'PyModule_GetFilenameObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyModule_GetFilenameObjectZ_ii))(u32);
/* export: 'PyErr_SetImportError' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_SetImportErrorZ_iiii))(u32, u32, u32);
/* export: 'PyFrame_BlockSetup' */
extern void (*WASM_RT_ADD_PREFIX(Z_PyFrame_BlockSetupZ_viiii))(u32, u32, u32, u32);
/* export: '_PyObject_GetMethod' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyObject_GetMethodZ_iiii))(u32, u32, u32);
/* export: 'PySlice_New' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PySlice_NewZ_iiii))(u32, u32, u32);
/* export: '_PyCFunction_FastCallKeywords' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyCFunction_FastCallKeywordsZ_iiiii))(u32, u32, u32, u32);
/* export: '_PyMethodDescr_FastCallKeywords' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyMethodDescr_FastCallKeywordsZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyErr_SetImportErrorSubclass' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyErr_SetImportErrorSubclassZ_iiiii))(u32, u32, u32, u32);
/* export: 'PyGILState_Check' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyGILState_CheckZ_iv))(void);
/* export: '_PyMem_DumpTraceback' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyMem_DumpTracebackZ_vii))(u32, u32);
/* export: '_Py_hashtable_get' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_hashtable_getZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '_Py_DumpDecimal' */
extern void (*WASM_RT_ADD_PREFIX(Z__Py_DumpDecimalZ_vii))(u32, u32);
/* export: 'PyThread_tss_is_created' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyThread_tss_is_createdZ_ii))(u32);
/* export: '_PyGC_Initialize' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyGC_InitializeZ_vi))(u32);
/* export: '_PyEval_Initialize' */
extern void (*WASM_RT_ADD_PREFIX(Z__PyEval_InitializeZ_vi))(u32);
/* export: 'Py_DecodeLocale' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_Py_DecodeLocaleZ_iii))(u32, u32);
/* export: '_Py_ReadHashSeed' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_ReadHashSeedZ_iiii))(u32, u32, u32);
/* export: '_PyMem_RawWcsdup' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__PyMem_RawWcsdupZ_ii))(u32);
/* export: '_Py_LegacyLocaleDetected' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Py_LegacyLocaleDetectedZ_iv))(void);
/* export: 'python_load_module' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_python_load_moduleZ_iii))(u32, u32);
/* export: 'PyImport_ImportModuleObject' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_PyImport_ImportModuleObjectZ_iiii))(u32, u32, u32);
/* export: 'python_call_module' */
extern void (*WASM_RT_ADD_PREFIX(Z_python_call_moduleZ_vijjj))(u32, u64, u64, u64);
/* export: '__stdio_close' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___stdio_closeZ_ii))(u32);
/* export: '__stdio_write' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___stdio_writeZ_iiii))(u32, u32, u32);
/* export: '__stdio_seek' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___stdio_seekZ_iiii))(u32, u32, u32);
/* export: '__stdio_read' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___stdio_readZ_iiii))(u32, u32, u32);
/* export: '__stdout_write' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___stdout_writeZ_iiii))(u32, u32, u32);
/* export: 'isalnum' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_isalnumZ_ii))(u32);
/* export: 'isalpha' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_isalphaZ_ii))(u32);
/* export: 'isdigit' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_isdigitZ_ii))(u32);
/* export: 'isxdigit' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_isxdigitZ_ii))(u32);
/* export: 'tolower' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_tolowerZ_ii))(u32);
/* export: 'isupper' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_isupperZ_ii))(u32);
/* export: 'toupper' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_toupperZ_ii))(u32);
/* export: 'islower' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_islowerZ_ii))(u32);
/* export: 'getenv' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_getenvZ_ii))(u32);
/* export: 'strlen' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_strlenZ_ii))(u32);
/* export: 'strchr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_strchrZ_iii))(u32, u32);
/* export: 'strncmp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_strncmpZ_iiii))(u32, u32, u32);
/* export: '__strchrnul' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___strchrnulZ_iii))(u32, u32);
/* export: '__errno_location' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___errno_locationZ_iv))(void);
/* export: 'strerror' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_strerrorZ_ii))(u32);
/* export: 'abort' */
extern void (*WASM_RT_ADD_PREFIX(Z_abortZ_vv))(void);
/* export: '_Exit' */
extern void (*WASM_RT_ADD_PREFIX(Z__ExitZ_vi))(u32);
/* export: 'acos' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_acosZ_dd))(f64);
/* export: 'sqrt' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_sqrtZ_dd))(f64);
/* export: 'acosh' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_acoshZ_dd))(f64);
/* export: 'log1p' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_log1pZ_dd))(f64);
/* export: 'log' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_logZ_dd))(f64);
/* export: 'asin' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_asinZ_dd))(f64);
/* export: 'fabs' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_fabsZ_dd))(f64);
/* export: 'asinh' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_asinhZ_dd))(f64);
/* export: 'atan' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_atanZ_dd))(f64);
/* export: 'atan2' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_atan2Z_ddd))(f64, f64);
/* export: 'atanh' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_atanhZ_dd))(f64);
/* export: 'ceil' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_ceilZ_dd))(f64);
/* export: 'copysign' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_copysignZ_ddd))(f64, f64);
/* export: 'cos' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_cosZ_dd))(f64);
/* export: '__cos' */
extern f64 (*WASM_RT_ADD_PREFIX(Z___cosZ_ddd))(f64, f64);
/* export: '__rem_pio2' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___rem_pio2Z_idi))(f64, u32);
/* export: '__sin' */
extern f64 (*WASM_RT_ADD_PREFIX(Z___sinZ_dddi))(f64, f64, u32);
/* export: '__rem_pio2_large' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___rem_pio2_largeZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: 'scalbn' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_scalbnZ_ddi))(f64, u32);
/* export: 'floor' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_floorZ_dd))(f64);
/* export: 'cosh' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_coshZ_dd))(f64);
/* export: 'expm1' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_expm1Z_dd))(f64);
/* export: 'exp' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_expZ_dd))(f64);
/* export: '__expo2' */
extern f64 (*WASM_RT_ADD_PREFIX(Z___expo2Z_dd))(f64);
/* export: 'erf' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_erfZ_dd))(f64);
/* export: 'erfc' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_erfcZ_dd))(f64);
/* export: 'fmod' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_fmodZ_ddd))(f64, f64);
/* export: 'frexp' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_frexpZ_ddi))(f64, u32);
/* export: 'hypot' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_hypotZ_ddd))(f64, f64);
/* export: 'ldexp' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_ldexpZ_ddi))(f64, u32);
/* export: 'log10' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_log10Z_dd))(f64);
/* export: 'log2' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_log2Z_dd))(f64);
/* export: 'modf' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_modfZ_ddi))(f64, u32);
/* export: 'pow' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_powZ_ddd))(f64, f64);
/* export: 'round' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_roundZ_dd))(f64);
/* export: 'sin' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_sinZ_dd))(f64);
/* export: 'sinh' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_sinhZ_dd))(f64);
/* export: 'tan' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_tanZ_dd))(f64);
/* export: '__tan' */
extern f64 (*WASM_RT_ADD_PREFIX(Z___tanZ_dddi))(f64, f64, u32);
/* export: 'tanh' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_tanhZ_dd))(f64);
/* export: 'mbrtowc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_mbrtowcZ_iiiii))(u32, u32, u32, u32);
/* export: 'mbstowcs' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_mbstowcsZ_iiii))(u32, u32, u32);
/* export: 'mbsrtowcs' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_mbsrtowcsZ_iiiii))(u32, u32, u32, u32);
/* export: 'wcstombs' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_wcstombsZ_iiii))(u32, u32, u32);
/* export: 'wcsrtombs' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_wcsrtombsZ_iiiii))(u32, u32, u32, u32);
/* export: 'wcrtomb' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_wcrtombZ_iiii))(u32, u32, u32);
/* export: 'clearerr' */
extern void (*WASM_RT_ADD_PREFIX(Z_clearerrZ_vi))(u32);
/* export: '__lockfile' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___lockfileZ_ii))(u32);
/* export: '__unlockfile' */
extern void (*WASM_RT_ADD_PREFIX(Z___unlockfileZ_vi))(u32);
/* export: 'fclose' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_fcloseZ_ii))(u32);
/* export: '__unlist_locked_file' */
extern void (*WASM_RT_ADD_PREFIX(Z___unlist_locked_fileZ_vi))(u32);
/* export: '__ofl_lock' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___ofl_lockZ_iv))(void);
/* export: '__ofl_unlock' */
extern void (*WASM_RT_ADD_PREFIX(Z___ofl_unlockZ_vv))(void);
/* export: 'fflush' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_fflushZ_ii))(u32);
/* export: 'feof' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_feofZ_ii))(u32);
/* export: 'ferror' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_ferrorZ_ii))(u32);
/* export: 'fgets' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_fgetsZ_iiii))(u32, u32, u32);
/* export: 'memchr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_memchrZ_iiii))(u32, u32, u32);
/* export: 'memcpy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_memcpyZ_iiii))(u32, u32, u32);
/* export: '__uflow' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___uflowZ_ii))(u32);
/* export: '__toread' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___toreadZ_ii))(u32);
/* export: 'fileno' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_filenoZ_ii))(u32);
/* export: 'fputc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_fputcZ_iii))(u32, u32);
/* export: '__overflow' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___overflowZ_iii))(u32, u32);
/* export: '__towrite' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___towriteZ_ii))(u32);
/* export: 'fputs' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_fputsZ_iii))(u32, u32);
/* export: 'fwrite' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_fwriteZ_iiiii))(u32, u32, u32, u32);
/* export: 'fread' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_freadZ_iiiii))(u32, u32, u32, u32);
/* export: 'ftell' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_ftellZ_ii))(u32);
/* export: 'getc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_getcZ_ii))(u32);
/* export: 'getc_unlocked' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_getc_unlockedZ_ii))(u32);
/* export: 'printf' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_printfZ_iii))(u32, u32);
/* export: 'vfprintf' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_vfprintfZ_iiii))(u32, u32, u32);
/* export: '__fwritex' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___fwritexZ_iiii))(u32, u32, u32);
/* export: 'wctomb' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_wctombZ_iii))(u32, u32);
/* export: 'memset' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_memsetZ_iiii))(u32, u32, u32);
/* export: '__signbitl' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___signbitlZ_ijj))(u64, u64);
/* export: '__fpclassifyl' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___fpclassifylZ_ijj))(u64, u64);
/* export: 'frexpl' */
extern void (*WASM_RT_ADD_PREFIX(Z_frexplZ_vijji))(u32, u64, u64, u32);
/* export: 'snprintf' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_snprintfZ_iiiii))(u32, u32, u32, u32);
/* export: 'vsnprintf' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_vsnprintfZ_iiiii))(u32, u32, u32, u32);
/* export: 'sprintf' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_sprintfZ_iiii))(u32, u32, u32);
/* export: 'vsprintf' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_vsprintfZ_iiii))(u32, u32, u32);
/* export: 'ungetc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_ungetcZ_iii))(u32, u32);
/* export: 'abs' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_absZ_ii))(u32);
/* export: 'strtoul' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_strtoulZ_iiii))(u32, u32, u32);
/* export: '__shlim' */
extern void (*WASM_RT_ADD_PREFIX(Z___shlimZ_vii))(u32, u32);
/* export: '__intscan' */
extern u64 (*WASM_RT_ADD_PREFIX(Z___intscanZ_jiiij))(u32, u32, u32, u64);
/* export: '__shgetc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___shgetcZ_ii))(u32);
/* export: 'isspace' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_isspaceZ_ii))(u32);
/* export: 'strtol' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_strtolZ_iiii))(u32, u32, u32);
/* export: 'wcstol' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_wcstolZ_iiii))(u32, u32, u32);
/* export: 'iswspace' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_iswspaceZ_ii))(u32);
/* export: 'wcschr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_wcschrZ_iii))(u32, u32);
/* export: 'wcslen' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_wcslenZ_ii))(u32);
/* export: 'memcmp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_memcmpZ_iiii))(u32, u32, u32);
/* export: 'memmove' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_memmoveZ_iiii))(u32, u32, u32);
/* export: 'strcmp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_strcmpZ_iii))(u32, u32);
/* export: 'strcpy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_strcpyZ_iii))(u32, u32);
/* export: '__stpcpy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___stpcpyZ_iii))(u32, u32);
/* export: 'strncpy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_strncpyZ_iiii))(u32, u32, u32);
/* export: '__stpncpy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___stpncpyZ_iiii))(u32, u32, u32);
/* export: 'strrchr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_strrchrZ_iii))(u32, u32);
/* export: '__memrchr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___memrchrZ_iiii))(u32, u32, u32);
/* export: 'strstr' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_strstrZ_iii))(u32, u32);
/* export: 'wcscmp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_wcscmpZ_iii))(u32, u32);
/* export: 'wcsncmp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_wcsncmpZ_iiii))(u32, u32, u32);
/* export: 'wmemcmp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_wmemcmpZ_iiii))(u32, u32, u32);
/* export: 'clock' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_clockZ_iv))(void);
/* export: '__clock_gettime' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___clock_gettimeZ_iii))(u32, u32);
/* export: 'clock_getres' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_clock_getresZ_iii))(u32, u32);
/* export: 'gettimeofday' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_gettimeofdayZ_iii))(u32, u32);
/* export: 'mktime' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_mktimeZ_ii))(u32);
/* export: '__tm_to_secs' */
extern u64 (*WASM_RT_ADD_PREFIX(Z___tm_to_secsZ_ji))(u32);
/* export: '__secs_to_zone' */
extern void (*WASM_RT_ADD_PREFIX(Z___secs_to_zoneZ_vjiiiii))(u64, u32, u32, u32, u32, u32);
/* export: '__secs_to_tm' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___secs_to_tmZ_iji))(u64, u32);
/* export: '__year_to_secs' */
extern u64 (*WASM_RT_ADD_PREFIX(Z___year_to_secsZ_jji))(u64, u32);
/* export: '__month_to_secs' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___month_to_secsZ_iii))(u32, u32);
/* export: '__map_file' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___map_fileZ_iii))(u32, u32);
/* export: 'strftime' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_strftimeZ_iiiii))(u32, u32, u32, u32);
/* export: '__strftime_l' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___strftime_lZ_iiiiii))(u32, u32, u32, u32, u32);
/* export: '__strftime_fmt_1' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___strftime_fmt_1Z_iiiiii))(u32, u32, u32, u32, u32);
/* export: '__tm_to_tzname' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___tm_to_tznameZ_ii))(u32);
/* export: 'time' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_timeZ_ii))(u32);
/* export: 'timegm' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_timegmZ_ii))(u32);
/* export: '__tzset' */
extern void (*WASM_RT_ADD_PREFIX(Z___tzsetZ_vv))(void);
/* export: '__gmtime_r' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___gmtime_rZ_iii))(u32, u32);
/* export: '__localtime_r' */
extern u32 (*WASM_RT_ADD_PREFIX(Z___localtime_rZ_iii))(u32, u32);
/* export: '_send_deferred' */
extern void (*WASM_RT_ADD_PREFIX(Z__send_deferredZ_vijiii))(u32, u64, u32, u32, u32);
/* export: '_cancel_deferred' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__cancel_deferredZ_ii))(u32);
/* export: 'get_current_memory' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_get_current_memoryZ_iv))(void);
/* export: 'get_code_memory' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_get_code_memoryZ_iv))(void);
/* export: 'get_frozen_code_memory' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_get_frozen_code_memoryZ_iv))(void);
/* export: 'malloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_mallocZ_ii))(u32);
/* export: 'calloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_callocZ_iii))(u32, u32);
/* export: 'realloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_reallocZ_iii))(u32, u32);
/* export: 'free' */
extern void (*WASM_RT_ADD_PREFIX(Z_freeZ_vi))(u32);
/* export: 'pythonvm_send_inline' */
extern void (*WASM_RT_ADD_PREFIX(Z_pythonvm_send_inlineZ_vjjjjii))(u64, u64, u64, u64, u32, u32);
/* export: 'pythonvm_action_new' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pythonvm_action_newZ_ijjjjii))(u64, u64, u64, u64, u32, u32);
/* export: 'pythonvm_action_add_permission' */
extern void (*WASM_RT_ADD_PREFIX(Z_pythonvm_action_add_permissionZ_vijj))(u32, u64, u64);
/* export: 'pythonvm_send_inline2' */
extern void (*WASM_RT_ADD_PREFIX(Z_pythonvm_send_inline2Z_vi))(u32);
/* export: 'pythonvm_action_free' */
extern void (*WASM_RT_ADD_PREFIX(Z_pythonvm_action_freeZ_vi))(u32);
/* export: 'pythonvm_transaction_new' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_pythonvm_transaction_newZ_iiiiiii))(u32, u32, u32, u32, u32, u32);
/* export: 'pythonvm_transaction_add_action' */
extern void (*WASM_RT_ADD_PREFIX(Z_pythonvm_transaction_add_actionZ_vii))(u32, u32);
/* export: 'pythonvm_transaction_add_context_free_action' */
extern void (*WASM_RT_ADD_PREFIX(Z_pythonvm_transaction_add_context_free_actionZ_vii))(u32, u32);
/* export: 'pythonvm_transaction_send' */
extern void (*WASM_RT_ADD_PREFIX(Z_pythonvm_transaction_sendZ_vijjji))(u32, u64, u64, u64, u32);
/* export: 'pythonvm_transaction_free' */
extern void (*WASM_RT_ADD_PREFIX(Z_pythonvm_transaction_freeZ_vi))(u32);
#ifdef __cplusplus
}
#endif

#endif  /* PYTHONVM_H_GENERATED_ */
