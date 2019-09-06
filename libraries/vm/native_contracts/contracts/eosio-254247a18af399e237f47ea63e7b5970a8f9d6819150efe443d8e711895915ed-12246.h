#ifndef CONTRACTS_EOSIO_254247A18AF399E237F47EA63E7B5970A8F9D6819150EFE443D8E711895915ED_12246_H_GENERATED_
#define CONTRACTS_EOSIO_254247A18AF399E237F47EA63E7B5970A8F9D6819150EFE443D8E711895915ED_12246_H_GENERATED_
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
/* import: 'eosio_injection' '_eosio_f64_neg' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(f64);
/* import: 'eosio_injection' '_eosio_f64_le' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_leZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_ne' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_neZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_mul' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_div' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_trunc_i64s' */
extern u64 (*Z_eosio_injectionZ__eosio_f64_trunc_i64sZ_jd)(f64);
/* import: 'eosio_injection' '_eosio_f64_add' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_sub' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_gt' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_gtZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_lt' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_ltZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_max' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_maxZ_ddd)(f64, f64);
/* import: 'env' 'abort' */
extern void (*Z_envZ_abortZ_vv)(void);
/* import: 'env' 'action_data_size' */
extern u32 (*Z_envZ_action_data_sizeZ_iv)(void);
/* import: 'env' 'cancel_deferred' */
extern u32 (*Z_envZ_cancel_deferredZ_ii)(u32);
/* import: 'env' 'current_receiver' */
extern u64 (*Z_envZ_current_receiverZ_jv)(void);
/* import: 'env' 'current_time' */
extern u64 (*Z_envZ_current_timeZ_jv)(void);
/* import: 'env' 'db_find_i64' */
extern u32 (*Z_envZ_db_find_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_get_i64' */
extern u32 (*Z_envZ_db_get_i64Z_iiii)(u32, u32, u32);
/* import: 'env' 'db_idx64_find_primary' */
extern u32 (*Z_envZ_db_idx64_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx64_lowerbound' */
extern u32 (*Z_envZ_db_idx64_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx64_remove' */
extern void (*Z_envZ_db_idx64_removeZ_vi)(u32);
/* import: 'env' 'db_idx64_store' */
extern u32 (*Z_envZ_db_idx64_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx64_update' */
extern void (*Z_envZ_db_idx64_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx_double_find_primary' */
extern u32 (*Z_envZ_db_idx_double_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx_double_lowerbound' */
extern u32 (*Z_envZ_db_idx_double_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_double_next' */
extern u32 (*Z_envZ_db_idx_double_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx_double_store' */
extern u32 (*Z_envZ_db_idx_double_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx_double_update' */
extern void (*Z_envZ_db_idx_double_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_next_i64' */
extern u32 (*Z_envZ_db_next_i64Z_iii)(u32, u32);
/* import: 'env' 'db_remove_i64' */
extern void (*Z_envZ_db_remove_i64Z_vi)(u32);
/* import: 'env' 'db_store_i64' */
extern u32 (*Z_envZ_db_store_i64Z_ijjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'db_update_i64' */
extern void (*Z_envZ_db_update_i64Z_vijii)(u32, u64, u32, u32);
/* import: 'env' 'eosio_assert' */
extern void (*Z_envZ_eosio_assertZ_vii)(u32, u32);
/* import: 'env' 'get_blockchain_parameters_packed' */
extern u32 (*Z_envZ_get_blockchain_parameters_packedZ_iii)(u32, u32);
/* import: 'env' 'is_account' */
extern u32 (*Z_envZ_is_accountZ_ij)(u64);
/* import: 'env' 'memcpy' */
extern u32 (*Z_envZ_memcpyZ_iiii)(u32, u32, u32);
/* import: 'env' 'memmove' */
extern u32 (*Z_envZ_memmoveZ_iiii)(u32, u32, u32);
/* import: 'env' 'memset' */
extern u32 (*Z_envZ_memsetZ_iiii)(u32, u32, u32);
/* import: 'env' 'printi' */
extern void (*Z_envZ_printiZ_vj)(u64);
/* import: 'env' 'printn' */
extern void (*Z_envZ_printnZ_vj)(u64);
/* import: 'env' 'prints' */
extern void (*Z_envZ_printsZ_vi)(u32);
/* import: 'env' 'prints_l' */
extern void (*Z_envZ_prints_lZ_vii)(u32, u32);
/* import: 'env' 'printui' */
extern void (*Z_envZ_printuiZ_vj)(u64);
/* import: 'env' 'read_action_data' */
extern u32 (*Z_envZ_read_action_dataZ_iii)(u32, u32);
/* import: 'env' 'require_auth' */
extern void (*Z_envZ_require_authZ_vj)(u64);
/* import: 'env' 'require_auth2' */
extern void (*Z_envZ_require_auth2Z_vjj)(u64, u64);
/* import: 'env' 'require_recipient' */
extern void (*Z_envZ_require_recipientZ_vj)(u64);
/* import: 'env' 'send_deferred' */
extern void (*Z_envZ_send_deferredZ_vijiii)(u32, u64, u32, u32, u32);
/* import: 'env' 'send_inline' */
extern void (*Z_envZ_send_inlineZ_vii)(u32, u32);
/* import: 'env' 'set_blockchain_parameters_packed' */
extern void (*Z_envZ_set_blockchain_parameters_packedZ_vii)(u32, u32);
/* import: 'env' 'set_privileged' */
extern void (*Z_envZ_set_privilegedZ_vji)(u64, u32);
/* import: 'env' 'set_proposed_producers' */
extern u64 (*Z_envZ_set_proposed_producersZ_jii)(u32, u32);
/* import: 'env' 'set_resource_limits' */
extern void (*Z_envZ_set_resource_limitsZ_vjjjj)(u64, u64, u64, u64);

/* export: 'memory' */
extern wasm_rt_memory_t (*WASM_RT_ADD_PREFIX(Z_memory));
/* export: 'now' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_nowZ_iv))(void);
/* export: '_ZeqRK11checksum256S1_' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Z5AeqRK11checksum256S1_Z_iii))(u32, u32);
/* export: '_ZeqRK11checksum160S1_' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Z5AeqRK11checksum160S1_Z_iii))(u32, u32);
/* export: '_ZneRK11checksum160S1_' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Z5AneRK11checksum160S1_Z_iii))(u32, u32);
/* export: '_ZN5eosio12require_authERKNS_16permission_levelE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio12require_authERKNS_16permission_levelEZ_vi))(u32);
/* export: '_ZN11eosiosystem15system_contract7onblockEN5eosio15block_timestampEy' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract7onblockEN5eosio15block_timestampEyZ_viij))(u32, u32, u64);
/* export: '_ZN11eosiosystem15system_contract24update_elected_producersEN5eosio15block_timestampE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract24update_elected_producersEN5eosio15block_timestampEZ_vii))(u32, u32);
/* export: '_ZN11eosiosystem15system_contract12claimrewardsERKy' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract12claimrewardsERKyZ_vii))(u32, u32);
/* export: '_ZN11eosiosystem15system_contract11buyrambytesEyym' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract11buyrambytesEyymZ_vijji))(u32, u64, u64, u32);
/* export: '_ZN11eosiosystem14exchange_state7convertEN5eosio5assetENS1_11symbol_typeE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem14exchange_state7convertEN5eosio5assetENS1_11symbol_typeEZ_viiij))(u32, u32, u32, u64);
/* export: '_ZN11eosiosystem15system_contract6buyramEyyN5eosio5assetE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract6buyramEyyN5eosio5assetEZ_vijji))(u32, u64, u64, u32);
/* export: '_ZN11eosiosystem14exchange_state21convert_from_exchangeERNS0_9connectorEN5eosio5assetE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem14exchange_state21convert_from_exchangeERNS0_9connectorEN5eosio5assetEZ_viiii))(u32, u32, u32, u32);
/* export: '_ZN11eosiosystem15system_contract7sellramEyx' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract7sellramEyxZ_vijj))(u32, u64, u64);
/* export: '_ZN11eosiosystem19validate_b1_vestingEx' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem19validate_b1_vestingExZ_vj))(u64);
/* export: '_ZN11eosiosystem15system_contract8changebwEyyN5eosio5assetES2_b' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract8changebwEyyN5eosio5assetES2_bZ_vijjiii))(u32, u64, u64, u32, u32, u32);
/* export: '_ZN11eosiosystem15system_contract12update_votesEyyRKNSt3__16vectorIyNS1_9allocatorIyEEEEb' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract12update_votesEyyRKNSt3__16vectorIyNS1_9allocatorIyEEEEbZ_vijjii))(u32, u64, u64, u32, u32);
/* export: '_ZN11eosiosystem15system_contract23propagate_weight_changeERKNS_10voter_infoE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract23propagate_weight_changeERKNS_10voter_infoEZ_vii))(u32, u32);
/* export: '_ZN11eosiosystem15system_contract10delegatebwEyyN5eosio5assetES2_b' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract10delegatebwEyyN5eosio5assetES2_bZ_vijjiii))(u32, u64, u64, u32, u32, u32);
/* export: '_ZN11eosiosystem15system_contract12undelegatebwEyyN5eosio5assetES2_' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract12undelegatebwEyyN5eosio5assetES2_Z_vijjii))(u32, u64, u64, u32, u32);
/* export: '_ZN11eosiosystem15system_contract6refundEy' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract6refundEyZ_vij))(u32, u64);
/* export: '_ZN11eosiosystem15system_contract11regproducerEyRKN5eosio10public_keyERKNSt3__112basic_stringIcNS5_11char_traitsIcEENS5_9allocatorIcEEEEt' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract11regproducerEyRKN5eosio10public_keyERKNSt3__112basic_stringIcNS5_11char_traitsIcEENS5_9allocatorIcEEEEtZ_vijiii))(u32, u64, u32, u32, u32);
/* export: '_ZN11eosiosystem15system_contract9unregprodEy' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract9unregprodEyZ_vij))(u32, u64);
/* export: '_ZN11eosiosystem10stake2voteEx' */
extern f64 (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem10stake2voteExZ_dj))(u64);
/* export: '_ZN11eosiosystem15system_contract12voteproducerEyyRKNSt3__16vectorIyNS1_9allocatorIyEEEE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract12voteproducerEyyRKNSt3__16vectorIyNS1_9allocatorIyEEEEZ_vijji))(u32, u64, u64, u32);
/* export: '_ZN11eosiosystem15system_contract8regproxyEyb' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract8regproxyEybZ_viji))(u32, u64, u32);
/* export: '_ZN11eosiosystem14exchange_state19convert_to_exchangeERNS0_9connectorEN5eosio5assetE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem14exchange_state19convert_to_exchangeERNS0_9connectorEN5eosio5assetEZ_viiii))(u32, u32, u32, u32);
/* export: '_ZN11eosiosystem15system_contractC2Ey' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contractC2EyZ_iij))(u32, u64);
/* export: '_ZN11eosiosystem15system_contract22get_default_parametersEv' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract22get_default_parametersEvZ_vi))(u32);
/* export: '_ZN11eosiosystem15system_contractD2Ev' */
extern u32 (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contractD2EvZ_ii))(u32);
/* export: '_ZN11eosiosystem15system_contract6setramEy' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract6setramEyZ_vij))(u32, u64);
/* export: '_ZN11eosiosystem15system_contract9setparamsERKN5eosio21blockchain_parametersE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract9setparamsERKN5eosio21blockchain_parametersEZ_vii))(u32, u32);
/* export: '_ZN11eosiosystem15system_contract7setprivEyh' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract7setprivEyhZ_viji))(u32, u64, u32);
/* export: '_ZN11eosiosystem15system_contract11rmvproducerEy' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract11rmvproducerEyZ_vij))(u32, u64);
/* export: '_ZN11eosiosystem15system_contract7bidnameEyyN5eosio5assetE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem15system_contract7bidnameEyyN5eosio5assetEZ_vijji))(u32, u64, u64, u32);
/* export: '_ZN11eosiosystem6native10newaccountEyy' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN11eosiosystem6native10newaccountEyyZ_vijj))(u32, u64, u64);
/* export: 'apply' */
extern void (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(u64, u64, u64);
/* export: 'fabs' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_fabsZ_dd))(f64);
/* export: 'pow' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_powZ_ddd))(f64, f64);
/* export: 'sqrt' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_sqrtZ_dd))(f64);
/* export: 'scalbn' */
extern f64 (*WASM_RT_ADD_PREFIX(Z_scalbnZ_ddi))(f64, u32);
/* export: 'memcmp' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_memcmpZ_iiii))(u32, u32, u32);
/* export: 'strlen' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_strlenZ_ii))(u32);
/* export: '_ZN5eosio25set_blockchain_parametersERKNS_21blockchain_parametersE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio25set_blockchain_parametersERKNS_21blockchain_parametersEZ_vi))(u32);
/* export: '_ZN5eosio25get_blockchain_parametersERNS_21blockchain_parametersE' */
extern void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio25get_blockchain_parametersERNS_21blockchain_parametersEZ_vi))(u32);
/* export: 'malloc' */
extern u32 (*WASM_RT_ADD_PREFIX(Z_mallocZ_ii))(u32);
/* export: 'free' */
extern void (*WASM_RT_ADD_PREFIX(Z_freeZ_vi))(u32);
#ifdef __cplusplus
}
#endif

#endif  /* CONTRACTS_EOSIO_254247A18AF399E237F47EA63E7B5970A8F9D6819150EFE443D8E711895915ED_12246_H_GENERATED_ */
