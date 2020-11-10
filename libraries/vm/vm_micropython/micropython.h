#ifndef MICROPYTHON_H_GENERATED_
#define MICROPYTHON_H_GENERATED_
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

/* import: 'env' 'memset' */
extern u32 (*Z_envZ_memsetZ_iiii)(u32, u32, u32);
/* import: 'env' 'memcpy' */
extern u32 (*Z_envZ_memcpyZ_iiii)(u32, u32, u32);
/* import: 'env' 'memmove' */
extern u32 (*Z_envZ_memmoveZ_iiii)(u32, u32, u32);
/* import: 'env' 'prints_l' */
extern void (*Z_envZ_prints_lZ_vii)(u32, u32);
/* import: 'env' 'eosio_assert' */
extern void (*Z_envZ_eosio_assertZ_vii)(u32, u32);
/* import: 'env' 'prints' */
extern void (*Z_envZ_printsZ_vi)(u32);
/* import: 'env' '__fixtfsi' */
extern u32 (*Z_envZ___fixtfsiZ_ijj)(u64, u64);
/* import: 'env' 'printqf' */
extern void (*Z_envZ_printqfZ_vi)(u32);
/* import: 'env' '__trunctfdf2' */
extern f64 (*Z_envZ___trunctfdf2Z_djj)(u64, u64);
/* import: 'env' '__eqtf2' */
extern u32 (*Z_envZ___eqtf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__extenddftf2' */
extern void (*Z_envZ___extenddftf2Z_vid)(u32, f64);
/* import: 'env' '__floatsitf' */
extern void (*Z_envZ___floatsitfZ_vii)(u32, u32);
/* import: 'env' '__addtf3' */
extern void (*Z_envZ___addtf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__netf2' */
extern u32 (*Z_envZ___netf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__subtf3' */
extern void (*Z_envZ___subtf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__multf3' */
extern void (*Z_envZ___multf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__lttf2' */
extern u32 (*Z_envZ___lttf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__divtf3' */
extern void (*Z_envZ___divtf3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' '__gttf2' */
extern u32 (*Z_envZ___gttf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__getf2' */
extern u32 (*Z_envZ___getf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__letf2' */
extern u32 (*Z_envZ___letf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 's2n' */
extern u64 (*Z_envZ_s2nZ_jii)(u32, u32);
/* import: 'env' 'n2s' */
extern u32 (*Z_envZ_n2sZ_ijii)(u64, u32, u32);
/* import: 'env' 'action_data_size' */
extern u32 (*Z_envZ_action_data_sizeZ_iv)(void);
/* import: 'env' 'read_action_data' */
extern u32 (*Z_envZ_read_action_dataZ_iii)(u32, u32);
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
/* import: 'env' 'send_inline' */
extern void (*Z_envZ_send_inlineZ_vii)(u32, u32);
/* import: 'env' 'send_context_free_inline' */
extern void (*Z_envZ_send_context_free_inlineZ_vii)(u32, u32);
/* import: 'env' 'publication_time' */
extern u64 (*Z_envZ_publication_timeZ_jv)(void);
/* import: 'env' 'current_receiver' */
extern u64 (*Z_envZ_current_receiverZ_jv)(void);
/* import: 'env' 'call_vm_api' */
extern u32 (*Z_envZ_call_vm_apiZ_iiiii)(u32, u32, u32, u32);
/* import: 'env' 'call_contract' */
extern void (*Z_envZ_call_contractZ_vjii)(u64, u32, u32);
/* import: 'env' 'call_contract_get_results' */
extern u32 (*Z_envZ_call_contract_get_resultsZ_iii)(u32, u32);
/* import: 'env' 'db_idx64_store' */
extern u32 (*Z_envZ_db_idx64_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx_long_double_store' */
extern u32 (*Z_envZ_db_idx_long_double_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx256_store' */
extern u32 (*Z_envZ_db_idx256_storeZ_ijjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_double_store' */
extern u32 (*Z_envZ_db_idx_double_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx128_store' */
extern u32 (*Z_envZ_db_idx128_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx64_update' */
extern void (*Z_envZ_db_idx64_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx_long_double_update' */
extern void (*Z_envZ_db_idx_long_double_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx256_update' */
extern void (*Z_envZ_db_idx256_updateZ_vijii)(u32, u64, u32, u32);
/* import: 'env' 'db_idx_double_update' */
extern void (*Z_envZ_db_idx_double_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx128_update' */
extern void (*Z_envZ_db_idx128_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx64_remove' */
extern void (*Z_envZ_db_idx64_removeZ_vi)(u32);
/* import: 'env' 'db_idx_long_double_remove' */
extern void (*Z_envZ_db_idx_long_double_removeZ_vi)(u32);
/* import: 'env' 'db_idx256_remove' */
extern void (*Z_envZ_db_idx256_removeZ_vi)(u32);
/* import: 'env' 'db_idx_double_remove' */
extern void (*Z_envZ_db_idx_double_removeZ_vi)(u32);
/* import: 'env' 'db_idx128_remove' */
extern void (*Z_envZ_db_idx128_removeZ_vi)(u32);
/* import: 'env' 'db_idx64_next' */
extern u32 (*Z_envZ_db_idx64_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx_long_double_next' */
extern u32 (*Z_envZ_db_idx_long_double_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx256_next' */
extern u32 (*Z_envZ_db_idx256_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx_double_next' */
extern u32 (*Z_envZ_db_idx_double_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx128_next' */
extern u32 (*Z_envZ_db_idx128_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx64_previous' */
extern u32 (*Z_envZ_db_idx64_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx_long_double_previous' */
extern u32 (*Z_envZ_db_idx_long_double_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx256_previous' */
extern u32 (*Z_envZ_db_idx256_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx_double_previous' */
extern u32 (*Z_envZ_db_idx_double_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx128_previous' */
extern u32 (*Z_envZ_db_idx128_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx64_find_primary' */
extern u32 (*Z_envZ_db_idx64_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx_long_double_find_primary' */
extern u32 (*Z_envZ_db_idx_long_double_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx256_find_primary' */
extern u32 (*Z_envZ_db_idx256_find_primaryZ_ijjjiij)(u64, u64, u64, u32, u32, u64);
/* import: 'env' 'db_idx_double_find_primary' */
extern u32 (*Z_envZ_db_idx_double_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx128_find_primary' */
extern u32 (*Z_envZ_db_idx128_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx64_find_secondary' */
extern u32 (*Z_envZ_db_idx64_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_find_secondary' */
extern u32 (*Z_envZ_db_idx_long_double_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx256_find_secondary' */
extern u32 (*Z_envZ_db_idx256_find_secondaryZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx_double_find_secondary' */
extern u32 (*Z_envZ_db_idx_double_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx128_find_secondary' */
extern u32 (*Z_envZ_db_idx128_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx64_lowerbound' */
extern u32 (*Z_envZ_db_idx64_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_lowerbound' */
extern u32 (*Z_envZ_db_idx_long_double_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx256_lowerbound' */
extern u32 (*Z_envZ_db_idx256_lowerboundZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx_double_lowerbound' */
extern u32 (*Z_envZ_db_idx_double_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx128_lowerbound' */
extern u32 (*Z_envZ_db_idx128_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx64_upperbound' */
extern u32 (*Z_envZ_db_idx64_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_upperbound' */
extern u32 (*Z_envZ_db_idx_long_double_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx256_upperbound' */
extern u32 (*Z_envZ_db_idx256_upperboundZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx_double_upperbound' */
extern u32 (*Z_envZ_db_idx_double_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx128_upperbound' */
extern u32 (*Z_envZ_db_idx128_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx64_end' */
extern u32 (*Z_envZ_db_idx64_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx_long_double_end' */
extern u32 (*Z_envZ_db_idx_long_double_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx256_end' */
extern u32 (*Z_envZ_db_idx256_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx_double_end' */
extern u32 (*Z_envZ_db_idx_double_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx128_end' */
extern u32 (*Z_envZ_db_idx128_endZ_ijjj)(u64, u64, u64);
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
/* import: 'env' 'eosio_assert_message' */
extern void (*Z_envZ_eosio_assert_messageZ_viii)(u32, u32, u32);
/* import: 'env' 'eosio_assert_code' */
extern void (*Z_envZ_eosio_assert_codeZ_vij)(u32, u64);
/* import: 'env' 'current_time' */
extern u64 (*Z_envZ_current_timeZ_jv)(void);
/* import: 'env' 'transaction_size' */
extern u32 (*Z_envZ_transaction_sizeZ_iv)(void);
/* import: 'env' 'read_transaction' */
extern u32 (*Z_envZ_read_transactionZ_iii)(u32, u32);
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
/* import: 'env' 'set_blockchain_parameters_packed' */
extern void (*Z_envZ_set_blockchain_parameters_packedZ_vii)(u32, u32);
/* import: 'env' 'get_blockchain_parameters_packed' */
extern u32 (*Z_envZ_get_blockchain_parameters_packedZ_iii)(u32, u32);
/* import: 'env' '__multi3' */
extern void (*Z_envZ___multi3Z_vijjjj)(u32, u64, u64, u64, u64);
/* import: 'env' 'abort' */
extern void (*Z_envZ_abortZ_vv)(void);
/* import: 'env' '__extendsftf2' */
extern void (*Z_envZ___extendsftf2Z_vif)(u32, f32);
/* import: 'env' '__floatunsitf' */
extern void (*Z_envZ___floatunsitfZ_vii)(u32, u32);
/* import: 'env' '__trunctfsf2' */
extern f32 (*Z_envZ___trunctfsf2Z_fjj)(u64, u64);
/* import: 'env' '__unordtf2' */
extern u32 (*Z_envZ___unordtf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__fixunstfsi' */
extern u32 (*Z_envZ___fixunstfsiZ_ijj)(u64, u64);

/* export: 'apply' */
extern void (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(u64, u64, u64);
#ifdef __cplusplus
}
#endif

#endif  /* MICROPYTHON_H_GENERATED_ */
