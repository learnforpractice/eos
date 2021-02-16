#include "vm_api4c.h"

extern "C" {
/* import: 'env' 'require_auth' */
void (*Z_envZ_require_authZ_vj)(u64);
/* import: 'env' 'eosio_assert' */
void (*Z_envZ_eosio_assertZ_vii)(u32, u32);
/* import: 'env' 'db_find_i64' */
u32 (*Z_envZ_db_find_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'current_receiver' */
u64 (*Z_envZ_current_receiverZ_jv)(void);
/* import: 'env' 'abort' */
void (*Z_envZ_abortZ_vv)(void);
/* import: 'env' 'db_store_i64' */
u32 (*Z_envZ_db_store_i64Z_ijjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'db_update_i64' */
void (*Z_envZ_db_update_i64Z_vijii)(u32, u64, u32, u32);
/* import: 'env' 'memcpy' */
u32 (*Z_envZ_memcpyZ_iiii)(u32, u32, u32);
/* import: 'env' 'is_account' */
u32 (*Z_envZ_is_accountZ_ij)(u64);
/* import: 'env' 'require_recipient' */
void (*Z_envZ_require_recipientZ_vj)(u64);
/* import: 'env' 'has_auth' */
u32 (*Z_envZ_has_authZ_ij)(u64);
/* import: 'env' 'db_next_i64' */
u32 (*Z_envZ_db_next_i64Z_iii)(u32, u32);
/* import: 'env' 'memset' */
u32 (*Z_envZ_memsetZ_iiii)(u32, u32, u32);
/* import: 'env' 'memmove' */
u32 (*Z_envZ_memmoveZ_iiii)(u32, u32, u32);
/* import: 'env' '__extendsftf2' */
void (*Z_envZ___extendsftf2Z_vif)(u32, f32);
/* import: 'env' '__floatsitf' */
void (*Z_envZ___floatsitfZ_vii)(u32, u32);
/* import: 'env' '__multf3' */
void (*Z_envZ___multf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__floatunsitf' */
void (*Z_envZ___floatunsitfZ_vii)(u32, u32);
/* import: 'env' '__divtf3' */
void (*Z_envZ___divtf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__addtf3' */
void (*Z_envZ___addtf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__extenddftf2' */
void (*Z_envZ___extenddftf2Z_vid)(u32, f64);
/* import: 'env' '__eqtf2' */
u32 (*Z_envZ___eqtf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__letf2' */
u32 (*Z_envZ___letf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__netf2' */
u32 (*Z_envZ___netf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__subtf3' */
void (*Z_envZ___subtf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__trunctfdf2' */
f64 (*Z_envZ___trunctfdf2Z_djj)(u64, u64);
/* import: 'env' '__getf2' */
u32 (*Z_envZ___getf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__trunctfsf2' */
f32 (*Z_envZ___trunctfsf2Z_fjj)(u64, u64);
/* import: 'env' 'prints_l' */
void (*Z_envZ_prints_lZ_vii)(u32, u32);
/* import: 'env' '__unordtf2' */
u32 (*Z_envZ___unordtf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__fixunstfsi' */
u32 (*Z_envZ___fixunstfsiZ_ijj)(u64, u64);
/* import: 'env' '__fixtfsi' */
u32 (*Z_envZ___fixtfsiZ_ijj)(u64, u64);
/* import: 'env' 'db_get_i64' */
u32 (*Z_envZ_db_get_i64Z_iiii)(u32, u32, u32);
/* import: 'env' 'db_remove_i64' */
void (*Z_envZ_db_remove_i64Z_vi)(u32);
/* import: 'env' 'send_inline' */
void (*Z_envZ_send_inlineZ_vii)(u32, u32);
/* import: 'env' 'send_context_free_inline' */
void (*Z_envZ_send_context_free_inlineZ_vii)(u32, u32);
/* import: 'env' 'read_action_data' */
u32 (*Z_envZ_read_action_dataZ_iii)(u32, u32);
/* import: 'env' 'printqf' */
void (*Z_envZ_printqfZ_vi)(u32);
/* import: 'env' '__lttf2' */
u32 (*Z_envZ___lttf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__gttf2' */
u32 (*Z_envZ___gttf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'action_data_size' */
u32 (*Z_envZ_action_data_sizeZ_iv)(void);
/* import: 'env' 'require_auth2' */
void (*Z_envZ_require_auth2Z_vjj)(u64, u64);
/* import: 'env' 'publication_time' */
u64 (*Z_envZ_publication_timeZ_jv)(void);
/* import: 'env' 'get_active_producers' */
u32 (*Z_envZ_get_active_producersZ_iii)(u32, u32);
/* import: 'env' 'send_deferred' */
void (*Z_envZ_send_deferredZ_vijiii)(u32, u64, u32, u32, u32);
/* import: 'env' 'cancel_deferred' */
u32 (*Z_envZ_cancel_deferredZ_ii)(u32);
/* import: 'env' 'assert_sha256' */
void (*Z_envZ_assert_sha256Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_sha1' */
void (*Z_envZ_assert_sha1Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_sha512' */
void (*Z_envZ_assert_sha512Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_ripemd160' */
void (*Z_envZ_assert_ripemd160Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_recover_key' */
void (*Z_envZ_assert_recover_keyZ_viiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'sha256' */
void (*Z_envZ_sha256Z_viii)(u32, u32, u32);
/* import: 'env' 'sha1' */
void (*Z_envZ_sha1Z_viii)(u32, u32, u32);
/* import: 'env' 'sha512' */
void (*Z_envZ_sha512Z_viii)(u32, u32, u32);
/* import: 'env' 'ripemd160' */
void (*Z_envZ_ripemd160Z_viii)(u32, u32, u32);
/* import: 'env' 'recover_key' */
u32 (*Z_envZ_recover_keyZ_iiiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'db_previous_i64' */
u32 (*Z_envZ_db_previous_i64Z_iii)(u32, u32);
/* import: 'env' 'db_lowerbound_i64' */
u32 (*Z_envZ_db_lowerbound_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_upperbound_i64' */
u32 (*Z_envZ_db_upperbound_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_end_i64' */
u32 (*Z_envZ_db_end_i64Z_ijjj)(u64, u64, u64);
/* import: 'env' 'call_contract' */
void (*Z_envZ_call_contractZ_vjii)(u64, u32, u32);
/* import: 'env' 'call_contract_get_results' */
u32 (*Z_envZ_call_contract_get_resultsZ_iii)(u32, u32);
/* import: 'env' 'db_idx64_store' */
u32 (*Z_envZ_db_idx64_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx_long_double_store' */
u32 (*Z_envZ_db_idx_long_double_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx256_store' */
u32 (*Z_envZ_db_idx256_storeZ_ijjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_double_store' */
u32 (*Z_envZ_db_idx_double_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx128_store' */
u32 (*Z_envZ_db_idx128_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx64_update' */
void (*Z_envZ_db_idx64_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx_long_double_update' */
void (*Z_envZ_db_idx_long_double_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx256_update' */
void (*Z_envZ_db_idx256_updateZ_vijii)(u32, u64, u32, u32);
/* import: 'env' 'db_idx_double_update' */
void (*Z_envZ_db_idx_double_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx128_update' */
void (*Z_envZ_db_idx128_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx64_remove' */
void (*Z_envZ_db_idx64_removeZ_vi)(u32);
/* import: 'env' 'db_idx_long_double_remove' */
void (*Z_envZ_db_idx_long_double_removeZ_vi)(u32);
/* import: 'env' 'db_idx256_remove' */
void (*Z_envZ_db_idx256_removeZ_vi)(u32);
/* import: 'env' 'db_idx_double_remove' */
void (*Z_envZ_db_idx_double_removeZ_vi)(u32);
/* import: 'env' 'db_idx128_remove' */
void (*Z_envZ_db_idx128_removeZ_vi)(u32);
/* import: 'env' 'db_idx64_next' */
u32 (*Z_envZ_db_idx64_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx_long_double_next' */
u32 (*Z_envZ_db_idx_long_double_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx256_next' */
u32 (*Z_envZ_db_idx256_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx_double_next' */
u32 (*Z_envZ_db_idx_double_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx128_next' */
u32 (*Z_envZ_db_idx128_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx64_previous' */
u32 (*Z_envZ_db_idx64_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx_long_double_previous' */
u32 (*Z_envZ_db_idx_long_double_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx256_previous' */
u32 (*Z_envZ_db_idx256_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx_double_previous' */
u32 (*Z_envZ_db_idx_double_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx128_previous' */
u32 (*Z_envZ_db_idx128_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx64_find_primary' */
u32 (*Z_envZ_db_idx64_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx_long_double_find_primary' */
u32 (*Z_envZ_db_idx_long_double_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx256_find_primary' */
u32 (*Z_envZ_db_idx256_find_primaryZ_ijjjiij)(u64, u64, u64, u32, u32, u64);
/* import: 'env' 'db_idx_double_find_primary' */
u32 (*Z_envZ_db_idx_double_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx128_find_primary' */
u32 (*Z_envZ_db_idx128_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx64_find_secondary' */
u32 (*Z_envZ_db_idx64_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_find_secondary' */
u32 (*Z_envZ_db_idx_long_double_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx256_find_secondary' */
u32 (*Z_envZ_db_idx256_find_secondaryZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx_double_find_secondary' */
u32 (*Z_envZ_db_idx_double_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx128_find_secondary' */
u32 (*Z_envZ_db_idx128_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx64_lowerbound' */
u32 (*Z_envZ_db_idx64_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_lowerbound' */
u32 (*Z_envZ_db_idx_long_double_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx256_lowerbound' */
u32 (*Z_envZ_db_idx256_lowerboundZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx_double_lowerbound' */
u32 (*Z_envZ_db_idx_double_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx128_lowerbound' */
u32 (*Z_envZ_db_idx128_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx64_upperbound' */
u32 (*Z_envZ_db_idx64_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_upperbound' */
u32 (*Z_envZ_db_idx_long_double_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx256_upperbound' */
u32 (*Z_envZ_db_idx256_upperboundZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx_double_upperbound' */
u32 (*Z_envZ_db_idx_double_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx128_upperbound' */
u32 (*Z_envZ_db_idx128_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx64_end' */
u32 (*Z_envZ_db_idx64_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx_long_double_end' */
u32 (*Z_envZ_db_idx_long_double_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx256_end' */
u32 (*Z_envZ_db_idx256_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx_double_end' */
u32 (*Z_envZ_db_idx_double_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx128_end' */
u32 (*Z_envZ_db_idx128_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'get_resource_limits' */
void (*Z_envZ_get_resource_limitsZ_vjiii)(u64, u32, u32, u32);
/* import: 'env' 'set_resource_limits' */
void (*Z_envZ_set_resource_limitsZ_vjjjj)(u64, u64, u64, u64);
/* import: 'env' 'set_proposed_producers' */
u64 (*Z_envZ_set_proposed_producersZ_jii)(u32, u32);
/* import: 'env' 'is_privileged' */
u32 (*Z_envZ_is_privilegedZ_ij)(u64);
/* import: 'env' 'set_privileged' */
void (*Z_envZ_set_privilegedZ_vji)(u64, u32);
/* import: 'env' 'set_blockchain_parameters_packed' */
void (*Z_envZ_set_blockchain_parameters_packedZ_vii)(u32, u32);
/* import: 'env' 'get_blockchain_parameters_packed' */
u32 (*Z_envZ_get_blockchain_parameters_packedZ_iii)(u32, u32);
/* import: 'env' 'eosio_assert_code' */
void (*Z_envZ_eosio_assert_codeZ_vij)(u32, u64);
/* import: 'env' 'current_time' */
u64 (*Z_envZ_current_timeZ_jv)(void);
/* import: 'env' 'transaction_size' */
u32 (*Z_envZ_transaction_sizeZ_iv)(void);
/* import: 'env' 'read_transaction' */
u32 (*Z_envZ_read_transactionZ_iii)(u32, u32);
/* import: 'env' 'tapos_block_num' */
u32 (*Z_envZ_tapos_block_numZ_iv)(void);
/* import: 'env' 'tapos_block_prefix' */
u32 (*Z_envZ_tapos_block_prefixZ_iv)(void);
/* import: 'env' 'expiration' */
u32 (*Z_envZ_expirationZ_iv)(void);
/* import: 'env' 'get_action' */
u32 (*Z_envZ_get_actionZ_iiiii)(u32, u32, u32, u32);
/* import: 'env' 'get_context_free_data' */
u32 (*Z_envZ_get_context_free_dataZ_iiii)(u32, u32, u32);
/* import: 'env' 'kv_erase' */
u64 (*Z_envZ_kv_eraseZ_jjii)(u64, u32, u32);
/* import: 'env' 'kv_set' */
u64 (*Z_envZ_kv_setZ_jjiiiij)(u64, u32, u32, u32, u32, u64);
/* import: 'env' 'kv_get' */
u32 (*Z_envZ_kv_getZ_ijiii)(u64, u32, u32, u32);
/* import: 'env' 'kv_get_data' */
u32 (*Z_envZ_kv_get_dataZ_iiii)(u32, u32, u32);
/* import: 'env' 'kv_it_create' */
u32 (*Z_envZ_kv_it_createZ_ijii)(u64, u32, u32);
/* import: 'env' 'kv_it_destroy' */
void (*Z_envZ_kv_it_destroyZ_vi)(u32);
/* import: 'env' 'kv_it_status' */
u32 (*Z_envZ_kv_it_statusZ_ii)(u32);
/* import: 'env' 'kv_it_compare' */
u32 (*Z_envZ_kv_it_compareZ_iii)(u32, u32);
/* import: 'env' 'kv_it_key_compare' */
u32 (*Z_envZ_kv_it_key_compareZ_iiii)(u32, u32, u32);
/* import: 'env' 'kv_it_move_to_end' */
u32 (*Z_envZ_kv_it_move_to_endZ_ii)(u32);
/* import: 'env' 'kv_it_next' */
u32 (*Z_envZ_kv_it_nextZ_iiii)(u32, u32, u32);
/* import: 'env' 'kv_it_prev' */
u32 (*Z_envZ_kv_it_prevZ_iiii)(u32, u32, u32);
/* import: 'env' 'kv_it_lower_bound' */
u32 (*Z_envZ_kv_it_lower_boundZ_iiiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'kv_it_key' */
u32 (*Z_envZ_kv_it_keyZ_iiiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'kv_it_value' */
u32 (*Z_envZ_kv_it_valueZ_iiiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'set_kv_parameters_packed' */
void (*Z_envZ_set_kv_parameters_packedZ_vii)(u32, u32);
/* import: 'env' 'prints' */
void (*Z_envZ_printsZ_vi)(u32);
/* import: 'env' 'printui' */
void (*Z_envZ_printuiZ_vj)(u64);

void (*Z_envZ_printhexZ_vii)(u32, u32);
void (*Z_envZ_printiZ_vj)(u64);

}