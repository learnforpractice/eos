#ifndef CONTRACTS_EOSIO_TOKEN_E536996BC480CCC3486B630F0C42CA1EF3B9F477595E885DCF8862848A2185BB_10_H_GENERATED_
#define CONTRACTS_EOSIO_TOKEN_E536996BC480CCC3486B630F0C42CA1EF3B9F477595E885DCF8862848A2185BB_10_H_GENERATED_
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

/* import: 'eosio_injection' 'checktime' */
extern void (*Z_eosio_injectionZ_checktimeZ_vv)(void);
/* import: 'eosio_injection' 'call_depth_assert' */
extern void (*Z_eosio_injectionZ_call_depth_assertZ_vv)(void);
/* import: 'env' 'require_auth' */
extern void (*Z_envZ_require_authZ_vj)(u64);
/* import: 'env' 'eosio_assert' */
extern void (*Z_envZ_eosio_assertZ_vii)(u32, u32);
/* import: 'env' 'db_find_i64' */
extern u32 (*Z_envZ_db_find_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'current_receiver' */
extern u64 (*Z_envZ_current_receiverZ_jv)(void);
/* import: 'env' 'abort' */
extern void (*Z_envZ_abortZ_vv)(void);
/* import: 'env' 'memset' */
extern u32 (*Z_envZ_memsetZ_iiii)(u32, u32, u32);
/* import: 'env' 'db_store_i64' */
extern u32 (*Z_envZ_db_store_i64Z_ijjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'db_update_i64' */
extern void (*Z_envZ_db_update_i64Z_vijii)(u32, u64, u32, u32);
/* import: 'env' 'memcpy' */
extern u32 (*Z_envZ_memcpyZ_iiii)(u32, u32, u32);
/* import: 'env' 'is_account' */
extern u32 (*Z_envZ_is_accountZ_ij)(u64);
/* import: 'env' 'require_recipient' */
extern void (*Z_envZ_require_recipientZ_vj)(u64);
/* import: 'env' 'has_auth' */
extern u32 (*Z_envZ_has_authZ_ij)(u64);
/* import: 'env' 'db_next_i64' */
extern u32 (*Z_envZ_db_next_i64Z_iii)(u32, u32);
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
/* import: 'env' 'action_data_size' */
extern u32 (*Z_envZ_action_data_sizeZ_iv)(void);
/* import: 'env' 'read_action_data' */
extern u32 (*Z_envZ_read_action_dataZ_iii)(u32, u32);
/* import: 'env' 'eosio_assert_code' */
extern void (*Z_envZ_eosio_assert_codeZ_vij)(u32, u64);
/* import: 'env' 'db_get_i64' */
extern u32 (*Z_envZ_db_get_i64Z_iiii)(u32, u32, u32);
/* import: 'env' 'db_remove_i64' */
extern void (*Z_envZ_db_remove_i64Z_vi)(u32);

/* export: 'apply' */
extern void (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(u64, u64, u64);
#ifdef __cplusplus
}
#endif

#endif  /* CONTRACTS_EOSIO_TOKEN_E536996BC480CCC3486B630F0C42CA1EF3B9F477595E885DCF8862848A2185BB_10_H_GENERATED_ */
