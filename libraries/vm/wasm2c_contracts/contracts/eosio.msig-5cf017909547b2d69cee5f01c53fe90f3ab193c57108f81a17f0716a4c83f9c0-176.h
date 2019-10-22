#ifndef CONTRACTS_EOSIO_MSIG_5CF017909547B2D69CEE5F01C53FE90F3AB193C57108F81A17F0716A4C83F9C0_176_H_GENERATED_
#define CONTRACTS_EOSIO_MSIG_5CF017909547B2D69CEE5F01C53FE90F3AB193C57108F81A17F0716A4C83F9C0_176_H_GENERATED_
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
/* import: 'env' 'check_permission_authorization' */
extern u32 (*Z_envZ_check_permission_authorizationZ_ijjiiiij)(u64, u64, u32, u32, u32, u32, u64);
/* import: 'env' 'check_transaction_authorization' */
extern u32 (*Z_envZ_check_transaction_authorizationZ_iiiiiii)(u32, u32, u32, u32, u32, u32);
/* import: 'env' 'current_receiver' */
extern u64 (*Z_envZ_current_receiverZ_jv)(void);
/* import: 'env' 'current_time' */
extern u64 (*Z_envZ_current_timeZ_jv)(void);
/* import: 'env' 'db_find_i64' */
extern u32 (*Z_envZ_db_find_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_get_i64' */
extern u32 (*Z_envZ_db_get_i64Z_iiii)(u32, u32, u32);
/* import: 'env' 'db_remove_i64' */
extern void (*Z_envZ_db_remove_i64Z_vi)(u32);
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
/* import: 'env' 'send_deferred' */
extern void (*Z_envZ_send_deferredZ_vijiii)(u32, u64, u32, u32, u32);

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
/* export: '_ZN5eosio31check_transaction_authorizationERKNS_11transactionERKNSt3__13setINS_16permission_levelENS3_4lessIS5_EENS3_9allocatorIS5_EEEERKNS4_I10public_keyNS6_ISD_EENS8_ISD_EEEE' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio31check_transaction_authorizationERKNS_11transactionERKNSt3__13setINS_16permission_levelENS3_4lessIS5_EENS3_9allocatorIS5_EEEERKNS4_I10public_keyNS6_ISD_EENS8_ISD_EEEEZ_iiii))(u32, u32, u32);
/* export: '_ZN5eosio30check_permission_authorizationEyyRKNSt3__13setI10public_keyNS0_4lessIS2_EENS0_9allocatorIS2_EEEERKNS1_INS_16permission_levelENS3_ISA_EENS5_ISA_EEEEy' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio30check_permission_authorizationEyyRKNSt3__13setI10public_keyNS0_4lessIS2_EENS0_9allocatorIS2_EEEERKNS1_INS_16permission_levelENS3_ISA_EENS5_ISA_EEEEyZ_ijjiij))(u64, u64, u32, u32, u64);
/* export: '_ZN5eosio8multisig7proposeEv' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig7proposeEvZ_vi))(u32);
/* export: '_ZN5eosio8multisig7approveEyNS_4nameENS_16permission_levelE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig7approveEyNS_4nameENS_16permission_levelEZ_vijji))(u32, u64, u64, u32);
/* export: '_ZN5eosio8multisig9unapproveEyNS_4nameENS_16permission_levelE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig9unapproveEyNS_4nameENS_16permission_levelEZ_vijji))(u32, u64, u64, u32);
/* export: '_ZN5eosio8multisig6cancelEyNS_4nameEy' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig6cancelEyNS_4nameEyZ_vijjj))(u32, u64, u64, u64);
/* export: '_ZN5eosio8multisig4execEyNS_4nameEy' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig4execEyNS_4nameEyZ_vijjj))(u32, u64, u64, u64);
/* export: 'apply' */
extern void (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(u64, u64, u64);
/* export: 'memcmp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_memcmpZ_iiii))(u32, u32, u32);
/* export: 'malloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_mallocZ_ii))(u32);
/* export: 'free' */
extern void (*WASM_RT_ADD_PREFIX(Z_freeZ_vi))(u32);
#ifdef __cplusplus
}
#endif

#endif  /* CONTRACTS_EOSIO_MSIG_5CF017909547B2D69CEE5F01C53FE90F3AB193C57108F81A17F0716A4C83F9C0_176_H_GENERATED_ */
