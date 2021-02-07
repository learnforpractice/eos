#include <stdint.h>

#ifndef __VM_API4C_H
#define __VM_API4C_H

#ifdef __cpulsplus
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


/* import: 'env' 'get_active_producers' */
extern u32 (*Z_envZ_get_active_producersZ_iii)(u32, u32);
/* import: 'env' 'db_store_i64' */
extern u32 (*Z_envZ_db_store_i64Z_ijjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'db_update_i64' */
extern void (*Z_envZ_db_update_i64Z_vijii)(u32, u64, u32, u32);
/* import: 'env' 'db_remove_i64' */
extern void (*Z_envZ_db_remove_i64Z_vi)(u32);
/* import: 'env' 'db_get_i64' */
extern u32 (*Z_envZ_db_get_i64Z_iiii)(u32, u32, u32);
/* import: 'env' 'db_next_i64' */
extern u32 (*Z_envZ_db_next_i64Z_iii)(u32, u32);
/* import: 'env' 'db_previous_i64' */
extern u32 (*Z_envZ_db_previous_i64Z_iii)(u32, u32);
/* import: 'env' 'db_find_i64' */
extern u32 (*Z_envZ_db_find_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_lowerbound_i64' */
extern u32 (*Z_envZ_db_lowerbound_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_upperbound_i64' */
extern u32 (*Z_envZ_db_upperbound_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_end_i64' */
extern u32 (*Z_envZ_db_end_i64Z_ijjj)(u64, u64, u64);
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
/* import: 'env' 'db_idx256_store' */
extern u32 (*Z_envZ_db_idx256_storeZ_ijjjjii)(u64, u64, u64, u64, u32, u32);
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
/* import: 'env' 'check_transaction_authorization' */
extern u32 (*Z_envZ_check_transaction_authorizationZ_iiiiiii)(u32, u32, u32, u32, u32, u32);
/* import: 'env' 'check_permission_authorization' */
extern u32 (*Z_envZ_check_permission_authorizationZ_ijjiiiij)(u64, u64, u32, u32, u32, u32, u64);
/* import: 'env' 'get_permission_last_used' */
extern u64 (*Z_envZ_get_permission_last_usedZ_jjj)(u64, u64);
/* import: 'env' 'get_account_creation_time' */
extern u64 (*Z_envZ_get_account_creation_timeZ_jj)(u64);
/* import: 'env' 'assert_sha256' */
extern void (*Z_envZ_assert_sha256Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_sha1' */
extern void (*Z_envZ_assert_sha1Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_sha512' */
extern void (*Z_envZ_assert_sha512Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_ripemd160' */
extern void (*Z_envZ_assert_ripemd160Z_viii)(u32, u32, u32);
/* import: 'env' 'sha256' */
extern void (*Z_envZ_sha256Z_viii)(u32, u32, u32);
/* import: 'env' 'sha1' */
extern void (*Z_envZ_sha1Z_viii)(u32, u32, u32);
/* import: 'env' 'sha512' */
extern void (*Z_envZ_sha512Z_viii)(u32, u32, u32);
/* import: 'env' 'ripemd160' */
extern void (*Z_envZ_ripemd160Z_viii)(u32, u32, u32);
/* import: 'env' 'recover_key' */
extern u32 (*Z_envZ_recover_keyZ_iiiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'assert_recover_key' */
extern void (*Z_envZ_assert_recover_keyZ_viiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'read_action_data' */
extern u32 (*Z_envZ_read_action_dataZ_iii)(u32, u32);
/* import: 'env' 'action_data_size' */
extern u32 (*Z_envZ_action_data_sizeZ_iv)(void);
/* import: 'env' 'require_recipient' */
extern void (*Z_envZ_require_recipientZ_vj)(u64);
/* import: 'env' 'require_auth' */
extern void (*Z_envZ_require_authZ_vj)(u64);
/* import: 'env' 'has_auth' */
extern u32 (*Z_envZ_has_authZ_ij)(u64);
/* import: 'env' 'require_auth2' */
extern void (*Z_envZ_require_auth2Z_vjj)(u64, u64);
/* import: 'env' 'is_account' */
extern u32 (*Z_envZ_is_accountZ_ij)(u64);
/* import: 'env' 'send_inline' */
extern void (*Z_envZ_send_inlineZ_vii)(u32, u32);
/* import: 'env' 'send_context_free_inline' */
extern void (*Z_envZ_send_context_free_inlineZ_vii)(u32, u32);
/* import: 'env' 'publication_time' */
extern u64 (*Z_envZ_publication_timeZ_jv)(void);
/* import: 'env' 'current_receiver' */
extern u64 (*Z_envZ_current_receiverZ_jv)(void);
/* import: 'env' 'prints' */
extern void (*Z_envZ_printsZ_vi)(u32);
/* import: 'env' 'prints_l' */
extern void (*Z_envZ_prints_lZ_vii)(u32, u32);
/* import: 'env' 'printi' */
extern void (*Z_envZ_printiZ_vj)(u64);
/* import: 'env' 'printui' */
extern void (*Z_envZ_printuiZ_vj)(u64);
/* import: 'env' 'printi128' */
extern void (*Z_envZ_printi128Z_vi)(u32);
/* import: 'env' 'printui128' */
extern void (*Z_envZ_printui128Z_vi)(u32);
/* import: 'env' 'printsf' */
extern void (*Z_envZ_printsfZ_vf)(f32);
/* import: 'env' 'printdf' */
extern void (*Z_envZ_printdfZ_vd)(f64);
/* import: 'env' 'printqf' */
extern void (*Z_envZ_printqfZ_vi)(u32);
/* import: 'env' 'printn' */
extern void (*Z_envZ_printnZ_vj)(u64);
/* import: 'env' 'printhex' */
extern void (*Z_envZ_printhexZ_vii)(u32, u32);
/* import: 'env' 'eosio_assert' */
extern void (*Z_envZ_eosio_assertZ_vii)(u32, u32);
/* import: 'env' 'eosio_assert_message' */
extern void (*Z_envZ_eosio_assert_messageZ_viii)(u32, u32, u32);
/* import: 'env' 'eosio_assert_code' */
extern void (*Z_envZ_eosio_assert_codeZ_vij)(u32, u64);
/* import: 'env' 'eosio_exit' */
extern void (*Z_envZ_eosio_exitZ_vi)(u32);


typedef void* (*fn_offset_to_char_ptr)(u32 offset);
typedef void* (*fn_offset_to_ptr)(u32 offset, u32 size);

void init_vm_api4c();

void *offset_to_ptr(u32 offset, u32 size);
void *offset_to_char_ptr(u32 offset);

void wasm_rt_on_trap(int code);

#ifdef __cpulsplus
}
#endif

#endif