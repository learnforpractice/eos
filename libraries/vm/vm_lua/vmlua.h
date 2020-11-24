#ifndef VMLUA_H_GENERATED_
#define VMLUA_H_GENERATED_
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

/* import: 'eosio_injection' '_eosio_f64_ne' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_neZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_eq' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_eqZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_mul' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_div' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_add' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_sub' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_neg' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(f64);
/* import: 'eosio_injection' '_eosio_f64_gt' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_gtZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_le' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_leZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f32_mul' */
extern f32 (*Z_eosio_injectionZ__eosio_f32_mulZ_fff)(f32, f32);
/* import: 'eosio_injection' '_eosio_f64_ge' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_geZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_lt' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_ltZ_idd)(f64, f64);
/* import: 'eosio_injection' '_eosio_f64_trunc' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_truncZ_dd)(f64);
/* import: 'eosio_injection' '_eosio_f64_trunc_i32u' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_trunc_i32uZ_id)(f64);
/* import: 'eosio_injection' '_eosio_f64_abs' */
extern f64 (*Z_eosio_injectionZ__eosio_f64_absZ_dd)(f64);
/* import: 'eosio_injection' '_eosio_f64_trunc_i32s' */
extern u32 (*Z_eosio_injectionZ__eosio_f64_trunc_i32sZ_id)(f64);
/* import: 'eosio_injection' '_eosio_f64_trunc_i64s' */
extern u64 (*Z_eosio_injectionZ__eosio_f64_trunc_i64sZ_jd)(f64);
/* import: 'eosio_injection' '_eosio_f64_demote' */
extern f32 (*Z_eosio_injectionZ__eosio_f64_demoteZ_fd)(f64);
/* import: 'eosio_injection' '_eosio_f32_promote' */
extern f64 (*Z_eosio_injectionZ__eosio_f32_promoteZ_df)(f32);
/* import: 'env' 'abort' */
extern void (*Z_envZ_abortZ_vv)(void);
/* import: 'env' 'memset' */
extern u32 (*Z_envZ_memsetZ_iiii)(u32, u32, u32);
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
/* import: 'env' 'memcpy' */
extern u32 (*Z_envZ_memcpyZ_iiii)(u32, u32, u32);
/* import: 'env' '__unordtf2' */
extern u32 (*Z_envZ___unordtf2Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' '__fixunstfsi' */
extern u32 (*Z_envZ___fixunstfsiZ_ijj)(u64, u64);
/* import: 'env' '__fixtfsi' */
extern u32 (*Z_envZ___fixtfsiZ_ijj)(u64, u64);
/* import: 'env' 'eosio_assert' */
extern void (*Z_envZ_eosio_assertZ_vii)(u32, u32);

/* export: 'apply' */
extern void (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(u64, u64, u64);
#ifdef __cplusplus
}
#endif

#endif  /* VMLUA_H_GENERATED_ */
