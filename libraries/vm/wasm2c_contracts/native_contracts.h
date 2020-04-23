#ifndef NATIVE_CONTRACTS_H_GENERATED_
#define NATIVE_CONTRACTS_H_GENERATED_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "wasm-rt.h"

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

/* import: 'env' 'db_find_i64' */
extern u32 (*Z_envZ_db_find_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'eosio_assert' */
extern void (*Z_envZ_eosio_assertZ_vii)(u32, u32);
/* import: 'env' 'set_blockchain_parameters_packed' */
extern void (*Z_envZ_set_blockchain_parameters_packedZ_vii)(u32, u32);
/* import: 'env' 'memcpy' */
extern u32 (*Z_envZ_memcpyZ_iiii)(u32, u32, u32);
/* import: 'env' 'get_blockchain_parameters_packed' */
extern u32 (*Z_envZ_get_blockchain_parameters_packedZ_iii)(u32, u32);
/* import: 'env' 'set_proposed_producers' */
extern u64 (*Z_envZ_set_proposed_producersZ_jii)(u32, u32);
/* import: 'env' 'abort' */
extern void (*Z_envZ_abortZ_vv)(void);
/* import: 'env' 'memset' */
extern u32 (*Z_envZ_memsetZ_iiii)(u32, u32, u32);
/* import: 'env' 'current_time' */
extern u64 (*Z_envZ_current_timeZ_jv)(void);
/* import: 'env' 'get_active_producers' */
extern u32 (*Z_envZ_get_active_producersZ_iii)(u32, u32);
/* import: 'env' 'memmove' */
extern u32 (*Z_envZ_memmoveZ_iiii)(u32, u32, u32);
/* import: 'env' '__extendsftf2' */
extern void (*Z_envZ___extendsftf2Z_vif)(u32, f32);
/* import: 'env' '__floatsitf' */
extern void (*Z_envZ___floatsitfZ_vii)(u32, u32);
/* import: 'env' '__multf3' */
extern void (*Z_envZ___multf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__floatunsitf' */
extern void (*Z_envZ___floatunsitfZ_vii)(u32, u32);
/* import: 'env' '__divtf3' */
extern void (*Z_envZ___divtf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__addtf3' */
extern void (*Z_envZ___addtf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__extenddftf2' */
extern void (*Z_envZ___extenddftf2Z_vid)(u32, f64);
/* import: 'env' '__eqtf2' */
extern u32 (*Z_envZ___eqtf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__letf2' */
extern u32 (*Z_envZ___letf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__netf2' */
extern u32 (*Z_envZ___netf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__subtf3' */
extern void (*Z_envZ___subtf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__trunctfdf2' */
extern f64 (*Z_envZ___trunctfdf2Z_djj)(u64, u64);
/* import: 'env' '__getf2' */
extern u32 (*Z_envZ___getf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__trunctfsf2' */
extern f32 (*Z_envZ___trunctfsf2Z_fjj)(u64, u64);
/* import: 'env' 'prints_l' */
extern void (*Z_envZ_prints_lZ_vii)(u32, u32);
/* import: 'env' '__unordtf2' */
extern u32 (*Z_envZ___unordtf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__fixunstfsi' */
extern u32 (*Z_envZ___fixunstfsiZ_ijj)(u64, u64);
/* import: 'env' '__fixtfsi' */
extern u32 (*Z_envZ___fixtfsiZ_ijj)(u64, u64);
/* import: 'env' 'require_auth' */
extern void (*Z_envZ_require_authZ_vj)(u64);
/* import: 'env' 'set_privileged' */
extern void (*Z_envZ_set_privilegedZ_vji)(u64, u32);
/* import: 'env' 'set_resource_limits' */
extern void (*Z_envZ_set_resource_limitsZ_vjjjj)(u64, u64, u64, u64);
/* import: 'env' 'get_resource_limits' */
extern void (*Z_envZ_get_resource_limitsZ_vjiii)(u64, u32, u32, u32);
/* import: 'env' 'current_receiver' */
extern u64 (*Z_envZ_current_receiverZ_jv)(void);
/* import: 'env' 'preactivate_feature' */
extern void (*Z_envZ_preactivate_featureZ_vi)(u32);
/* import: 'env' 'db_store_i64' */
extern u32 (*Z_envZ_db_store_i64Z_ijjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'db_next_i64' */
extern u32 (*Z_envZ_db_next_i64Z_iii)(u32, u32);
/* import: 'env' 'is_privileged' */
extern u32 (*Z_envZ_is_privilegedZ_ij)(u64);
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
/* import: 'env' '__ashlti3' */
extern void (*Z_envZ___ashlti3Z_vijji)(u32, u64, u64, u32);
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
/* import: 'env' 'db_lowerbound_i64' */
extern u32 (*Z_envZ_db_lowerbound_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'is_account' */
extern u32 (*Z_envZ_is_accountZ_ij)(u64);
/* import: 'env' 'printn' */
extern void (*Z_envZ_printnZ_vj)(u64);
/* import: 'env' 'prints' */
extern void (*Z_envZ_printsZ_vi)(u32);
/* import: 'env' 'db_idx64_store' */
extern u32 (*Z_envZ_db_idx64_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'cancel_deferred' */
extern u32 (*Z_envZ_cancel_deferredZ_ii)(u32);
/* import: 'env' 'send_deferred' */
extern void (*Z_envZ_send_deferredZ_vijiii)(u32, u64, u32, u32, u32);
/* import: 'env' 'db_update_i64' */
extern void (*Z_envZ_db_update_i64Z_vijii)(u32, u64, u32, u32);
/* import: 'env' 'db_idx_double_find_primary' */
extern u32 (*Z_envZ_db_idx_double_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx_double_update' */
extern void (*Z_envZ_db_idx_double_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'set_proposed_producers_ex' */
extern u64 (*Z_envZ_set_proposed_producers_exZ_jjii)(u64, u32, u32);
/* import: 'env' '__multi3' */
extern void (*Z_envZ___multi3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__udivti3' */
extern void (*Z_envZ___udivti3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' 'db_idx64_find_primary' */
extern u32 (*Z_envZ_db_idx64_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx64_next' */
extern u32 (*Z_envZ_db_idx64_nextZ_iii)(u32, u32);
/* import: 'env' 'check_transaction_authorization' */
extern u32 (*Z_envZ_check_transaction_authorizationZ_iiiiiii)(u32, u32, u32, u32, u32, u32);
/* import: 'env' 'check_permission_authorization' */
extern u32 (*Z_envZ_check_permission_authorizationZ_ijjiiiij)(u64, u64, u32, u32, u32, u32, u64);
/* import: 'env' 'get_permission_last_used' */
extern u64 (*Z_envZ_get_permission_last_usedZ_jjj)(u64, u64);
/* import: 'env' 'get_account_creation_time' */
extern u64 (*Z_envZ_get_account_creation_timeZ_jj)(u64);
/* import: 'env' 'db_idx_double_next' */
extern u32 (*Z_envZ_db_idx_double_nextZ_iii)(u32, u32);
/* import: 'env' 'printi' */
extern void (*Z_envZ_printiZ_vj)(u64);
/* import: 'env' 'send_inline' */
extern void (*Z_envZ_send_inlineZ_vii)(u32, u32);
/* import: 'env' 'require_recipient' */
extern void (*Z_envZ_require_recipientZ_vj)(u64);
/* import: 'env' 'has_auth' */
extern u32 (*Z_envZ_has_authZ_ij)(u64);
/* import: 'env' 'action_data_size' */
extern u32 (*Z_envZ_action_data_sizeZ_iv)(void);
/* import: 'env' 'read_action_data' */
extern u32 (*Z_envZ_read_action_dataZ_iii)(u32, u32);
/* import: 'env' 'eosio_assert_code' */
extern void (*Z_envZ_eosio_assert_codeZ_vij)(u32, u64);
/* import: 'env' 'db_idx64_update' */
extern void (*Z_envZ_db_idx64_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_get_i64' */
extern u32 (*Z_envZ_db_get_i64Z_iiii)(u32, u32, u32);
/* import: 'env' 'db_remove_i64' */
extern void (*Z_envZ_db_remove_i64Z_vi)(u32);
/* import: 'env' 'db_idx64_remove' */
extern void (*Z_envZ_db_idx64_removeZ_vi)(u32);
/* import: 'env' 'db_idx64_lowerbound' */
extern u32 (*Z_envZ_db_idx64_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_double_store' */
extern u32 (*Z_envZ_db_idx_double_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx_double_lowerbound' */
extern u32 (*Z_envZ_db_idx_double_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);

/* export: 'eosio_system_apply' */
extern void (*WASM_RT_ADD_PREFIX(Z_eosio_system_applyZ_vjjj))(u64, u64, u64);
/* export: '_Znwj' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Z5AnwjZ_ii))(u32);
/* export: '_Znaj' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Z5AnajZ_ii))(u32);
/* export: '_ZdlPv' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AdlPvZ_vi))(u32);
/* export: '_ZdaPv' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AdaPvZ_vi))(u32);
/* export: '_ZnwjSt11align_val_t' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Z5AnwjSt11align_val_tZ_iii))(u32, u32);
/* export: '_ZnajSt11align_val_t' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Z5AnajSt11align_val_tZ_iii))(u32, u32);
/* export: '_ZdlPvSt11align_val_t' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AdlPvSt11align_val_tZ_vii))(u32, u32);
/* export: '_ZdaPvSt11align_val_t' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AdaPvSt11align_val_tZ_vii))(u32, u32);
/* export: 'native_eosio_token_apply' */
extern void (*WASM_RT_ADD_PREFIX(Z_native_eosio_token_applyZ_vjjj))(u64, u64, u64);
/* export: 'eosio_system_get_delegated_balance' */
extern u64 (*WASM_RT_ADD_PREFIX(Z_eosio_system_get_delegated_balanceZ_jj))(u64);
/* export: 'eosio_system_get_rex_fund' */
extern u64 (*WASM_RT_ADD_PREFIX(Z_eosio_system_get_rex_fundZ_jj))(u64);
/* export: 'system_contract_is_vm_activated' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_system_contract_is_vm_activatedZ_iii))(u32, u32);
#ifdef __cplusplus
}
#endif

#endif  /* NATIVE_CONTRACTS_H_GENERATED_ */
