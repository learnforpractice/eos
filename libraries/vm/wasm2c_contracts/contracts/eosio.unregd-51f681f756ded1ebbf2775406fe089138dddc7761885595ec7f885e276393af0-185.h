#ifndef CONTRACTS_EOSIO_UNREGD_51F681F756DED1EBBF2775406FE089138DDDC7761885595EC7F885E276393AF0_185_H_GENERATED_
#define CONTRACTS_EOSIO_UNREGD_51F681F756DED1EBBF2775406FE089138DDDC7761885595EC7F885E276393AF0_185_H_GENERATED_
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
/* import: 'env' 'abort' */
extern void (*Z_envZ_abortZ_vv)(void);
/* import: 'env' 'action_data_size' */
extern u32 (*Z_envZ_action_data_sizeZ_iv)(void);
/* import: 'env' 'current_receiver' */
extern u64 (*Z_envZ_current_receiverZ_jv)(void);
/* import: 'env' 'current_time' */
extern u64 (*Z_envZ_current_timeZ_jv)(void);
/* import: 'env' 'db_end_i64' */
extern u32 (*Z_envZ_db_end_i64Z_ijjj)(u64, u64, u64);
/* import: 'env' 'db_find_i64' */
extern u32 (*Z_envZ_db_find_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_get_i64' */
extern u32 (*Z_envZ_db_get_i64Z_iiii)(u32, u32, u32);
/* import: 'env' 'db_idx256_find_primary' */
extern u32 (*Z_envZ_db_idx256_find_primaryZ_ijjjiij)(u64, u64, u64, u32, u32, u64);
/* import: 'env' 'db_idx256_lowerbound' */
extern u32 (*Z_envZ_db_idx256_lowerboundZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx256_store' */
extern u32 (*Z_envZ_db_idx256_storeZ_ijjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx256_update' */
extern void (*Z_envZ_db_idx256_updateZ_vijii)(u32, u64, u32, u32);
/* import: 'env' 'db_lowerbound_i64' */
extern u32 (*Z_envZ_db_lowerbound_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_previous_i64' */
extern u32 (*Z_envZ_db_previous_i64Z_iii)(u32, u32);
/* import: 'env' 'db_store_i64' */
extern u32 (*Z_envZ_db_store_i64Z_ijjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'db_update_i64' */
extern void (*Z_envZ_db_update_i64Z_vijii)(u32, u64, u32, u32);
/* import: 'env' 'eosio_assert' */
extern void (*Z_envZ_eosio_assertZ_vii)(u32, u32);
/* import: 'env' 'memcpy' */
extern u32 (*Z_envZ_memcpyZ_iiii)(u32, u32, u32);
/* import: 'env' 'memmove' */
extern u32 (*Z_envZ_memmoveZ_iiii)(u32, u32, u32);
/* import: 'env' 'read_action_data' */
extern u32 (*Z_envZ_read_action_dataZ_iii)(u32, u32);
/* import: 'env' 'require_auth' */
extern void (*Z_envZ_require_authZ_vj)(u64);
/* import: 'env' 'require_auth2' */
extern void (*Z_envZ_require_auth2Z_vjj)(u64, u64);

/* export: 'memory' */
extern wasm_rt_memory_t (*WASM_RT_ADD_PREFIX(Z_memory));
/* export: '_ZeqRK11checksum256S1_' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Z5AeqRK11checksum256S1_Z_iii))(u32, u32);
/* export: '_ZeqRK11checksum160S1_' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Z5AeqRK11checksum160S1_Z_iii))(u32, u32);
/* export: '_ZneRK11checksum160S1_' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Z5AneRK11checksum160S1_Z_iii))(u32, u32);
/* export: 'now' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_nowZ_iv))(void);
/* export: '_ZN5eosio12require_authERKNS_16permission_levelE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio12require_authERKNS_16permission_levelEZ_vi))(u32);
/* export: 'apply' */
extern void (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(u64, u64, u64);
/* export: '_ZN5eosio6unregd3addERKNSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEERKNS_5assetE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio6unregd3addERKNSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEERKNS_5assetEZ_viii))(u32, u32, u32);
/* export: '_ZN5eosio6unregd14update_addressERKNSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEENS1_8functionIFvRNS0_7addressEEEE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio6unregd14update_addressERKNSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEENS1_8functionIFvRNS0_7addressEEEEZ_viii))(u32, u32, u32);
/* export: 'malloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_mallocZ_ii))(u32);
/* export: 'free' */
extern void (*WASM_RT_ADD_PREFIX(Z_freeZ_vi))(u32);
/* export: 'memcmp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_memcmpZ_iiii))(u32, u32, u32);
#ifdef __cplusplus
}
#endif

#endif  /* CONTRACTS_EOSIO_UNREGD_51F681F756DED1EBBF2775406FE089138DDDC7761885595EC7F885E276393AF0_185_H_GENERATED_ */
