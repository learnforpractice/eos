#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "contracts/eosio.msig-5cf017909547b2d69cee5f01c53fe90f3ab193c57108f81a17f0716a4c83f9c0-176.h"
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#define LIKELY(x) __builtin_expect(!!(x), 1)

#define TRAP(x) (wasm_rt_trap(WASM_RT_TRAP_##x), 0)

#define FUNC_PROLOGUE                                            \
  if (++wasm_rt_call_stack_depth > WASM_RT_MAX_CALL_STACK_DEPTH) \
    TRAP(EXHAUSTION)

#define FUNC_EPILOGUE --wasm_rt_call_stack_depth

#define UNREACHABLE TRAP(UNREACHABLE)

#define CALL_INDIRECT(table, t, ft, x, ...)          \
  (LIKELY((x) < table.size && table.data[x].func &&  \
          table.data[x].func_type == func_types[ft]) \
       ? ((t)table.data[x].func)(__VA_ARGS__)        \
       : TRAP(CALL_INDIRECT))

#define MEMCHECK(mem, a, t)  \
  if (UNLIKELY((a) + sizeof(t) > mem->size)) TRAP(OOB)

#define DEFINE_LOAD(name, t1, t2, t3)              \
  static inline t3 name(wasm_rt_memory_t* mem, u64 addr) {   \
    MEMCHECK(mem, addr, t1);                       \
    t1 result;                                     \
    memcpy(&result, &mem->data[addr], sizeof(t1)); \
    return (t3)(t2)result;                         \
  }

#define DEFINE_STORE(name, t1, t2)                           \
  static inline void name(wasm_rt_memory_t* mem, u64 addr, t2 value) { \
    MEMCHECK(mem, addr, t1);                                 \
    t1 wrapped = (t1)value;                                  \
    memcpy(&mem->data[addr], &wrapped, sizeof(t1));          \
  }

DEFINE_LOAD(i32_load, u32, u32, u32);
DEFINE_LOAD(i64_load, u64, u64, u64);
DEFINE_LOAD(f32_load, f32, f32, f32);
DEFINE_LOAD(f64_load, f64, f64, f64);
DEFINE_LOAD(i32_load8_s, s8, s32, u32);
DEFINE_LOAD(i64_load8_s, s8, s64, u64);
DEFINE_LOAD(i32_load8_u, u8, u32, u32);
DEFINE_LOAD(i64_load8_u, u8, u64, u64);
DEFINE_LOAD(i32_load16_s, s16, s32, u32);
DEFINE_LOAD(i64_load16_s, s16, s64, u64);
DEFINE_LOAD(i32_load16_u, u16, u32, u32);
DEFINE_LOAD(i64_load16_u, u16, u64, u64);
DEFINE_LOAD(i64_load32_s, s32, s64, u64);
DEFINE_LOAD(i64_load32_u, u32, u64, u64);
DEFINE_STORE(i32_store, u32, u32);
DEFINE_STORE(i64_store, u64, u64);
DEFINE_STORE(f32_store, f32, f32);
DEFINE_STORE(f64_store, f64, f64);
DEFINE_STORE(i32_store8, u8, u32);
DEFINE_STORE(i32_store16, u16, u32);
DEFINE_STORE(i64_store8, u8, u64);
DEFINE_STORE(i64_store16, u16, u64);
DEFINE_STORE(i64_store32, u32, u64);

#define I32_CLZ(x) ((x) ? __builtin_clz(x) : 32)
#define I64_CLZ(x) ((x) ? __builtin_clzll(x) : 64)
#define I32_CTZ(x) ((x) ? __builtin_ctz(x) : 32)
#define I64_CTZ(x) ((x) ? __builtin_ctzll(x) : 64)
#define I32_POPCNT(x) (__builtin_popcount(x))
#define I64_POPCNT(x) (__builtin_popcountll(x))

#define DIV_S(ut, min, x, y)                                 \
   ((UNLIKELY((y) == 0)) ?                TRAP(DIV_BY_ZERO)  \
  : (UNLIKELY((x) == min && (y) == -1)) ? TRAP(INT_OVERFLOW) \
  : (ut)((x) / (y)))

#define REM_S(ut, min, x, y)                                \
   ((UNLIKELY((y) == 0)) ?                TRAP(DIV_BY_ZERO) \
  : (UNLIKELY((x) == min && (y) == -1)) ? 0                 \
  : (ut)((x) % (y)))

#define I32_DIV_S(x, y) DIV_S(u32, INT32_MIN, (s32)x, (s32)y)
#define I64_DIV_S(x, y) DIV_S(u64, INT64_MIN, (s64)x, (s64)y)
#define I32_REM_S(x, y) REM_S(u32, INT32_MIN, (s32)x, (s32)y)
#define I64_REM_S(x, y) REM_S(u64, INT64_MIN, (s64)x, (s64)y)

#define DIVREM_U(op, x, y) \
  ((UNLIKELY((y) == 0)) ? TRAP(DIV_BY_ZERO) : ((x) op (y)))

#define DIV_U(x, y) DIVREM_U(/, x, y)
#define REM_U(x, y) DIVREM_U(%, x, y)

#define ROTL(x, y, mask) \
  (((x) << ((y) & (mask))) | ((x) >> (((mask) - (y) + 1) & (mask))))
#define ROTR(x, y, mask) \
  (((x) >> ((y) & (mask))) | ((x) << (((mask) - (y) + 1) & (mask))))

#define I32_ROTL(x, y) ROTL(x, y, 31)
#define I64_ROTL(x, y) ROTL(x, y, 63)
#define I32_ROTR(x, y) ROTR(x, y, 31)
#define I64_ROTR(x, y) ROTR(x, y, 63)

#define FMIN(x, y)                                          \
   ((UNLIKELY((x) != (x))) ? NAN                            \
  : (UNLIKELY((y) != (y))) ? NAN                            \
  : (UNLIKELY((x) == 0 && (y) == 0)) ? (signbit(x) ? x : y) \
  : (x < y) ? x : y)

#define FMAX(x, y)                                          \
   ((UNLIKELY((x) != (x))) ? NAN                            \
  : (UNLIKELY((y) != (y))) ? NAN                            \
  : (UNLIKELY((x) == 0 && (y) == 0)) ? (signbit(x) ? y : x) \
  : (x > y) ? x : y)

#define TRUNC_S(ut, st, ft, min, max, maxop, x)                             \
   ((UNLIKELY((x) != (x))) ? TRAP(INVALID_CONVERSION)                       \
  : (UNLIKELY((x) < (ft)(min) || (x) maxop (ft)(max))) ? TRAP(INT_OVERFLOW) \
  : (ut)(st)(x))

#define I32_TRUNC_S_F32(x) TRUNC_S(u32, s32, f32, INT32_MIN, INT32_MAX, >=, x)
#define I64_TRUNC_S_F32(x) TRUNC_S(u64, s64, f32, INT64_MIN, INT64_MAX, >=, x)
#define I32_TRUNC_S_F64(x) TRUNC_S(u32, s32, f64, INT32_MIN, INT32_MAX, >,  x)
#define I64_TRUNC_S_F64(x) TRUNC_S(u64, s64, f64, INT64_MIN, INT64_MAX, >=, x)

#define TRUNC_U(ut, ft, max, maxop, x)                                    \
   ((UNLIKELY((x) != (x))) ? TRAP(INVALID_CONVERSION)                     \
  : (UNLIKELY((x) <= (ft)-1 || (x) maxop (ft)(max))) ? TRAP(INT_OVERFLOW) \
  : (ut)(x))

#define I32_TRUNC_U_F32(x) TRUNC_U(u32, f32, UINT32_MAX, >=, x)
#define I64_TRUNC_U_F32(x) TRUNC_U(u64, f32, UINT64_MAX, >=, x)
#define I32_TRUNC_U_F64(x) TRUNC_U(u32, f64, UINT32_MAX, >,  x)
#define I64_TRUNC_U_F64(x) TRUNC_U(u64, f64, UINT64_MAX, >=, x)

#define DEFINE_REINTERPRET(name, t1, t2)  \
  static inline t2 name(t1 x) {           \
    t2 result;                            \
    memcpy(&result, &x, sizeof(result));  \
    return result;                        \
  }

DEFINE_REINTERPRET(f32_reinterpret_i32, u32, f32)
DEFINE_REINTERPRET(i32_reinterpret_f32, f32, u32)
DEFINE_REINTERPRET(f64_reinterpret_i64, u64, f64)
DEFINE_REINTERPRET(i64_reinterpret_f64, f64, u64)


static u32 func_types[23];

static void init_func_types(void) {
  func_types[0] = wasm_rt_register_func_type(1, 0, WASM_RT_I32);
  func_types[1] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32);
  func_types[2] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[3] = wasm_rt_register_func_type(0, 0);
  func_types[4] = wasm_rt_register_func_type(0, 1, WASM_RT_I64);
  func_types[5] = wasm_rt_register_func_type(2, 0, WASM_RT_I64, WASM_RT_I64);
  func_types[6] = wasm_rt_register_func_type(6, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[7] = wasm_rt_register_func_type(2, 0, WASM_RT_I32, WASM_RT_I32);
  func_types[8] = wasm_rt_register_func_type(3, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[9] = wasm_rt_register_func_type(7, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32);
  func_types[10] = wasm_rt_register_func_type(0, 1, WASM_RT_I32);
  func_types[11] = wasm_rt_register_func_type(2, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[12] = wasm_rt_register_func_type(1, 0, WASM_RT_I64);
  func_types[13] = wasm_rt_register_func_type(4, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32);
  func_types[14] = wasm_rt_register_func_type(6, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[15] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[16] = wasm_rt_register_func_type(5, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[17] = wasm_rt_register_func_type(5, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32);
  func_types[18] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[19] = wasm_rt_register_func_type(3, 1, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[20] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32);
  func_types[21] = wasm_rt_register_func_type(3, 0, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[22] = wasm_rt_register_func_type(1, 1, WASM_RT_I32, WASM_RT_I32);
}

static u32 _ZeqRK11checksum256S1_(u32, u32);
static u32 _ZeqRK11checksum160S1_(u32, u32);
static u32 _ZneRK11checksum160S1_(u32, u32);
static u32 now(void);
static void _ZN5eosio12require_authERKNS_16permission_levelE(u32);
static u32 _ZN5eosio31check_transaction_authorizationERKNS_11transactionERKNSt3__13setINS_16permission_levelENS3_4lessIS5_EENS3_9allocatorIS5_EEEERKNS4_I10public_keyNS6_ISD_EENS8_ISD_EEEE(u32, u32, u32);
static void f26(u32, u32);
static void f27(u32, u32);
static u32 f28(u32, u32);
static u32 f29(u32, u32);
static u32 f30(u32, u32);
static u32 f31(u32, u32);
static u32 f32_0(u32, u32);
static u32 f33(u32, u32);
static u32 f34(u32, u32);
static u32 f35(u32, u32);
static u32 _ZN5eosio30check_permission_authorizationEyyRKNSt3__13setI10public_keyNS0_4lessIS2_EENS0_9allocatorIS2_EEEERKNS1_INS_16permission_levelENS3_ISA_EENS5_ISA_EEEEy(u64, u64, u32, u32, u64);
static void _ZN5eosio8multisig7proposeEv(u32);
static u32 f38(u32, u32);
static u32 f39(u32, u32);
static u32 f40(u32, u32);
static void f41(u32, u32);
static void f42(u32, u32, u32, u32);
static void f43(u32, u32);
static void f44(u32, u32, u32, u32);
static u32 f45(u32, u32);
static void f46(u32, u32);
static void _ZN5eosio8multisig7approveEyNS_4nameENS_16permission_levelE(u32, u64, u64, u32);
static u32 f48(u32, u64, u32);
static void f49(u32, u32, u64, u32);
static void f50(u32, u32);
static u32 f51(u32, u32);
static void _ZN5eosio8multisig9unapproveEyNS_4nameENS_16permission_levelE(u32, u64, u64, u32);
static void f53(u32, u32, u64, u32);
static void _ZN5eosio8multisig6cancelEyNS_4nameEy(u32, u64, u64, u64);
static u32 f55(u32, u64, u32);
static void f56(u32, u32);
static void f57(u32, u32);
static void _ZN5eosio8multisig4execEyNS_4nameEy(u32, u64, u64, u64);
static void apply(u64, u64, u64);
static u32 f60(u32, u32);
static u32 f61(u32, u32);
static u32 f62(u32, u32);
static void f63(u32, u32);
static u32 f64_0(u32);
static void f65(u32);
static void f66(u32);
static u32 memcmp_0(u32, u32, u32);
static u32 malloc_0(u32);
static u32 f69(u32, u32);
static u32 f70(u32);
static void free_0(u32);
static void f72(void);

static u32 g0;

static void init_globals(void) {
  g0 = 250u;
}

static wasm_rt_memory_t memory;

static wasm_rt_table_t T0;

static u32 _ZeqRK11checksum256S1_(u32 p0, u32 p1) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p1;
  i2 = 32u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = memcmp_0(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = !(i0);
  FUNC_EPILOGUE;
  return i0;
}

static u32 _ZeqRK11checksum160S1_(u32 p0, u32 p1) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p1;
  i2 = 32u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = memcmp_0(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = !(i0);
  FUNC_EPILOGUE;
  return i0;
}

static u32 _ZneRK11checksum160S1_(u32 p0, u32 p1) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p1;
  i2 = 32u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = memcmp_0(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = 0u;
  i0 = i0 != i1;
  FUNC_EPILOGUE;
  return i0;
}

static u32 now(void) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g0;
    i0 += i1;
    g0 = i0;
  }
  j0 = (*Z_envZ_current_timeZ_jv)();
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j1 = 1000000ull;
  j0 = DIV_U(j0, j1);
  i0 = (u32)(j0);
  FUNC_EPILOGUE;
  return i0;
}

static void _ZN5eosio12require_authERKNS_16permission_levelE(u32 p0) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_require_auth2Z_vjj)(j0, j1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static u32 _ZN5eosio31check_transaction_authorizationERKNS_11transactionERKNSt3__13setINS_16permission_levelENS3_4lessIS5_EENS3_9allocatorIS5_EEEERKNS4_I10public_keyNS6_ISD_EENS8_ISD_EEEE(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0, l8 = 0, 
      l9 = 0;
  u64 l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 48u;
  i1 -= i2;
  l9 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l9;
  i1 = 16u;
  i0 += i1;
  i1 = p0;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f26(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  l8 = i0;
  i0 = 0u;
  l6 = i0;
  i0 = 0u;
  l7 = i0;
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = 0u;
  l3 = i0;
  i0 = l9;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l9;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l0;
  j0 = (u64)(i0);
  l2 = j0;
  L2: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    j0 = l2;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l2 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L2;}
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = p2;
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  L6: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    l5 = i0;
    i0 = i32_load((&memory), (u64)(i0 + 4));
    p0 = i0;
    i0 = !(i0);
    if (i0) {goto B8;}
    L9: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = p0;
      l4 = i0;
      i0 = i32_load((&memory), (u64)(i0));
      p0 = i0;
      if (i0) {goto L9;}
      goto B7;
    B8:;
    i0 = l5;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l4 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i0 = i0 == i1;
    if (i0) {goto B7;}
    i0 = l5;
    i1 = 8u;
    i0 += i1;
    l5 = i0;
    L10: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l5;
      i0 = i32_load((&memory), (u64)(i0));
      p0 = i0;
      i1 = 8u;
      i0 += i1;
      l5 = i0;
      i0 = p0;
      i1 = p0;
      i1 = i32_load((&memory), (u64)(i1 + 8));
      l4 = i1;
      i1 = i32_load((&memory), (u64)(i1));
      i0 = i0 != i1;
      if (i0) {goto L10;}
    B7:;
    i0 = l3;
    i1 = 34u;
    i0 += i1;
    l3 = i0;
    i0 = l4;
    i1 = l1;
    i0 = i0 != i1;
    if (i0) {goto L6;}
  i0 = l3;
  i0 = !(i0);
  if (i0) {goto B4;}
  B5:;
  i0 = l9;
  i1 = l3;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f27(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l9;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l4 = i0;
  i0 = l9;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  goto B3;
  B4:;
  i0 = 0u;
  l4 = i0;
  i0 = 0u;
  p0 = i0;
  B3:;
  i0 = l9;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l9;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l9;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l9;
  i1 = 32u;
  i0 += i1;
  i1 = p2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f28(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l9;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l6 = i0;
  i0 = l9;
  i0 = i32_load((&memory), (u64)(i0));
  l7 = i0;
  B1:;
  i0 = 0u;
  p0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  p2 = i0;
  i0 = !(i0);
  if (i0) {goto B13;}
  i0 = 0u;
  l3 = i0;
  i0 = l9;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l9;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p2;
  j0 = (u64)(i0);
  l2 = j0;
  L14: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    j0 = l2;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l2 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L14;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B17;}
  L18: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    l5 = i0;
    i0 = i32_load((&memory), (u64)(i0 + 4));
    p0 = i0;
    i0 = !(i0);
    if (i0) {goto B20;}
    L21: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = p0;
      l4 = i0;
      i0 = i32_load((&memory), (u64)(i0));
      p0 = i0;
      if (i0) {goto L21;}
      goto B19;
    B20:;
    i0 = l5;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l4 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i0 = i0 == i1;
    if (i0) {goto B19;}
    i0 = l5;
    i1 = 8u;
    i0 += i1;
    l5 = i0;
    L22: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l5;
      i0 = i32_load((&memory), (u64)(i0));
      p0 = i0;
      i1 = 8u;
      i0 += i1;
      l5 = i0;
      i0 = p0;
      i1 = p0;
      i1 = i32_load((&memory), (u64)(i1 + 8));
      l4 = i1;
      i1 = i32_load((&memory), (u64)(i1));
      i0 = i0 != i1;
      if (i0) {goto L22;}
    B19:;
    i0 = l3;
    i1 = 16u;
    i0 += i1;
    l3 = i0;
    i0 = l4;
    i1 = l1;
    i0 = i0 != i1;
    if (i0) {goto L18;}
  i0 = l3;
  i0 = !(i0);
  if (i0) {goto B16;}
  B17:;
  i0 = l9;
  i1 = l3;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f27(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l9;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l4 = i0;
  i0 = l9;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  goto B15;
  B16:;
  i0 = 0u;
  l4 = i0;
  i0 = 0u;
  p0 = i0;
  B15:;
  i0 = l9;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l9;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l9;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l9;
  i1 = 32u;
  i0 += i1;
  i1 = p1;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f29(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l9;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l8 = i0;
  i0 = l9;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  B13:;
  i0 = l9;
  i0 = i32_load((&memory), (u64)(i0 + 16));
  l4 = i0;
  i1 = l9;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i2 = l4;
  i1 -= i2;
  i2 = l7;
  i3 = 0u;
  i4 = l0;
  i2 = i4 ? i2 : i3;
  i3 = l6;
  i4 = l7;
  i3 -= i4;
  i4 = 0u;
  i5 = l0;
  i3 = i5 ? i3 : i4;
  i4 = p0;
  i5 = 0u;
  i6 = p2;
  i4 = i6 ? i4 : i5;
  i5 = l8;
  i6 = p0;
  i5 -= i6;
  i6 = 0u;
  i7 = p2;
  i5 = i7 ? i5 : i6;
  i6 = g0;
  i6 = !(i6);
  if (i6) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i6 = 4294967295u;
    i7 = g0;
    i6 += i7;
    g0 = i6;
  }
  i0 = (*Z_envZ_check_transaction_authorizationZ_iiiiiii)(i0, i1, i2, i3, i4, i5);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = p0;
  i0 = !(i0);
  if (i0) {goto B26;}
  i0 = p0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B26:;
  i0 = l7;
  i0 = !(i0);
  if (i0) {goto B28;}
  i0 = l7;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B28:;
  i0 = l9;
  i0 = i32_load((&memory), (u64)(i0 + 16));
  p0 = i0;
  i0 = !(i0);
  if (i0) {goto B30;}
  i0 = l9;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = p0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B30:;
  i0 = 0u;
  i1 = l9;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l4;
  i1 = 0u;
  i0 = (u32)((s32)i0 > (s32)i1);
  FUNC_EPILOGUE;
  return i0;
}

static void f26(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 0u;
  l1 = i0;
  i0 = p0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l2;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l2;
  i1 = p1;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f30(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  i0 = p0;
  i1 = l0;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f27(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  goto B1;
  B2:;
  i0 = 0u;
  p0 = i0;
  B1:;
  i0 = l2;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l2;
  i1 = p0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l2;
  i1 = l1;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l2;
  i1 = p1;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f31(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = p1;
  i2 = 24u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f32_0(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = p1;
  i2 = 36u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f32_0(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = p1;
  i2 = 48u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f33(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = l2;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f27(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l0 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l4 = i1;
  i0 -= i1;
  i1 = p1;
  i0 = i0 >= i1;
  if (i0) {goto B4;}
  i0 = l4;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  l3 = i1;
  i0 -= i1;
  l1 = i0;
  i1 = p1;
  i0 += i1;
  l2 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 <= (s32)i1);
  if (i0) {goto B2;}
  i0 = 2147483647u;
  l4 = i0;
  i0 = l0;
  i1 = l3;
  i0 -= i1;
  l0 = i0;
  i1 = 1073741822u;
  i0 = i0 > i1;
  if (i0) {goto B5;}
  i0 = l2;
  i1 = l0;
  i2 = 1u;
  i1 <<= (i2 & 31);
  l4 = i1;
  i2 = l4;
  i3 = l2;
  i2 = i2 < i3;
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  B5:;
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = f64_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B1;
  B4:;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  p0 = i0;
  L7: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 0u;
    i32_store8((&memory), (u64)(i0), i1);
    i0 = p0;
    i1 = p0;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 1u;
    i1 += i2;
    l4 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = 4294967295u;
    i0 += i1;
    p1 = i0;
    if (i0) {goto L7;}
    goto B0;
  B3:;
  i0 = 0u;
  l4 = i0;
  i0 = 0u;
  l0 = i0;
  goto B1;
  B2:;
  i0 = p0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f66(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B1:;
  i0 = l0;
  i1 = l4;
  i0 += i1;
  l2 = i0;
  i0 = l0;
  i1 = l1;
  i0 += i1;
  l3 = i0;
  l4 = i0;
  L9: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 0u;
    i32_store8((&memory), (u64)(i0), i1);
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i0 = p1;
    i1 = 4294967295u;
    i0 += i1;
    p1 = i0;
    if (i0) {goto L9;}
  i0 = l3;
  i1 = p0;
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = p0;
  i2 = i32_load((&memory), (u64)(i2));
  p1 = i2;
  i1 -= i2;
  l0 = i1;
  i0 -= i1;
  l3 = i0;
  i0 = l0;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B10;}
  i0 = l3;
  i1 = p1;
  i2 = l0;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  p1 = i0;
  B10:;
  i0 = p0;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l1;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = p1;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto Bfunc;
  B0:;
  Bfunc:;
  FUNC_EPILOGUE;
}

static u32 f28(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  u64 l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 80u;
  i1 -= i2;
  l7 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l3 = i0;
  i0 = p1;
  j0 = i64_load32_u((&memory), (u64)(i0 + 8));
  l2 = j0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l5 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l2;
    i0 = (u32)(j0);
    l4 = i0;
    i0 = l7;
    j1 = l2;
    j2 = 7ull;
    j1 >>= (j2 & 63);
    l2 = j1;
    j2 = 0ull;
    i1 = j1 != j2;
    l6 = i1;
    i2 = 7u;
    i1 <<= (i2 & 31);
    i2 = l4;
    i3 = 127u;
    i2 &= i3;
    i1 |= i2;
    i32_store8((&memory), (u64)(i0 + 40), i1);
    i0 = l1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l3;
    i0 -= i1;
    i1 = 0u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l7;
    i2 = 40u;
    i1 += i2;
    i2 = 1u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = l5;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 1u;
    i1 += i2;
    l3 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l6;
    if (i0) {goto L0;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  p1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l7;
    i1 = 6u;
    i0 += i1;
    i1 = l4;
    l6 = i1;
    i2 = 13u;
    i1 += i2;
    i2 = 34u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l7;
    i1 = 40u;
    i0 += i1;
    i1 = l7;
    i2 = 6u;
    i1 += i2;
    i2 = 34u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l3;
    i0 -= i1;
    i1 = 33u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l7;
    i2 = 40u;
    i1 += i2;
    i2 = 34u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = l1;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 34u;
    i1 += i2;
    l3 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l6;
    i0 = i32_load((&memory), (u64)(i0 + 4));
    l5 = i0;
    i0 = !(i0);
    if (i0) {goto B10;}
    L11: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l5;
      l4 = i0;
      i0 = i32_load((&memory), (u64)(i0));
      l5 = i0;
      if (i0) {goto L11;}
      goto B9;
    B10:;
    i0 = l6;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l4 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l6;
    i0 = i0 == i1;
    if (i0) {goto B9;}
    i0 = l6;
    i1 = 8u;
    i0 += i1;
    l6 = i0;
    L12: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l6;
      i0 = i32_load((&memory), (u64)(i0));
      l5 = i0;
      i1 = 8u;
      i0 += i1;
      l6 = i0;
      i0 = l5;
      i1 = l5;
      i1 = i32_load((&memory), (u64)(i1 + 8));
      l4 = i1;
      i1 = i32_load((&memory), (u64)(i1));
      i0 = i0 != i1;
      if (i0) {goto L12;}
    B9:;
    i0 = l4;
    i1 = p1;
    i0 = i0 != i1;
    if (i0) {goto L4;}
  B3:;
  i0 = 0u;
  i1 = l7;
  i2 = 80u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f29(u32 p0, u32 p1) {
  u32 l0 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l2 = i0;
  i0 = p1;
  j0 = i64_load32_u((&memory), (u64)(i0 + 8));
  l1 = j0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l4 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l1;
    i0 = (u32)(j0);
    l3 = i0;
    i0 = l6;
    j1 = l1;
    j2 = 7ull;
    j1 >>= (j2 & 63);
    l1 = j1;
    j2 = 0ull;
    i1 = j1 != j2;
    l5 = i1;
    i2 = 7u;
    i1 <<= (i2 & 31);
    i2 = l3;
    i3 = 127u;
    i2 &= i3;
    i1 |= i2;
    i32_store8((&memory), (u64)(i0 + 15), i1);
    i0 = l0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l2;
    i0 -= i1;
    i1 = 0u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l6;
    i2 = 15u;
    i1 += i2;
    i2 = 1u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = l4;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 1u;
    i1 += i2;
    l2 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l5;
    if (i0) {goto L0;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  p1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l0 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 8u;
    i0 += i1;
    l4 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l2;
    i0 -= i1;
    i1 = 7u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l3;
    l5 = i1;
    i2 = 16u;
    i1 += i2;
    i2 = 8u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i1 = l0;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 8u;
    i1 += i2;
    l3 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l3;
    i0 -= i1;
    i1 = 7u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i2 = 24u;
    i1 += i2;
    i2 = 8u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i1 = l0;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 8u;
    i1 += i2;
    l2 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l5;
    i0 = i32_load((&memory), (u64)(i0 + 4));
    l4 = i0;
    i0 = !(i0);
    if (i0) {goto B10;}
    L11: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l4;
      l3 = i0;
      i0 = i32_load((&memory), (u64)(i0));
      l4 = i0;
      if (i0) {goto L11;}
      goto B9;
    B10:;
    i0 = l5;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l3 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i0 = i0 == i1;
    if (i0) {goto B9;}
    i0 = l5;
    i1 = 8u;
    i0 += i1;
    l5 = i0;
    L12: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l5;
      i0 = i32_load((&memory), (u64)(i0));
      l4 = i0;
      i1 = 8u;
      i0 += i1;
      l5 = i0;
      i0 = l4;
      i1 = l4;
      i1 = i32_load((&memory), (u64)(i1 + 8));
      l3 = i1;
      i1 = i32_load((&memory), (u64)(i1));
      i0 = i0 != i1;
      if (i0) {goto L12;}
    B9:;
    i0 = l3;
    i1 = p1;
    i0 = i0 != i1;
    if (i0) {goto L4;}
  B3:;
  i0 = 0u;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f30(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  u64 l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  l4 = i1;
  i2 = 10u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l4;
  i1 = 11u;
  i0 += i1;
  l4 = i0;
  i0 = p1;
  j0 = i64_load32_u((&memory), (u64)(i0 + 12));
  l6 = j0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l6;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L0;}
  i0 = p0;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  j0 = i64_load32_u((&memory), (u64)(i0 + 20));
  l6 = j0;
  L1: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l6;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L1;}
  i0 = p0;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 24));
  l5 = i1;
  i0 -= i1;
  i1 = 40u;
  i0 = I32_DIV_S(i0, i1);
  j0 = (u64)(i0);
  l6 = j0;
  L2: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l6;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L2;}
  i0 = p0;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l4 = i0;
    i0 = l5;
    i1 = 20u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    l1 = i0;
    i1 = l5;
    i1 = i32_load((&memory), (u64)(i1 + 16));
    l2 = i1;
    i0 -= i1;
    l3 = i0;
    i1 = 4u;
    i0 = (u32)((s32)i0 >> (i1 & 31));
    j0 = (u64)(i0);
    l6 = j0;
    L5: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      j0 = l6;
      j1 = 7ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto L5;}
    i0 = l2;
    i1 = l1;
    i0 = i0 == i1;
    if (i0) {goto B6;}
    i0 = l3;
    i1 = 4294967280u;
    i0 &= i1;
    i1 = l4;
    i0 += i1;
    l4 = i0;
    B6:;
    i0 = l4;
    i1 = l5;
    i2 = 32u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    l1 = i1;
    i0 += i1;
    i1 = l5;
    i1 = i32_load((&memory), (u64)(i1 + 28));
    l2 = i1;
    i0 -= i1;
    l4 = i0;
    i0 = l1;
    i1 = l2;
    i0 -= i1;
    j0 = (u64)(i0);
    l6 = j0;
    L7: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      j0 = l6;
      j1 = 7ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto L7;}
    i0 = l5;
    i1 = 40u;
    i0 += i1;
    l5 = i0;
    i1 = l0;
    i0 = i0 != i1;
    if (i0) {goto L4;}
  i0 = p0;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  B3:;
  i0 = p1;
  i1 = 40u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 36));
  l5 = i1;
  i0 -= i1;
  i1 = 40u;
  i0 = I32_DIV_S(i0, i1);
  j0 = (u64)(i0);
  l6 = j0;
  L8: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l6;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L8;}
  i0 = p0;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l4 = i0;
    i0 = l5;
    i1 = 20u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    l1 = i0;
    i1 = l5;
    i1 = i32_load((&memory), (u64)(i1 + 16));
    l2 = i1;
    i0 -= i1;
    l3 = i0;
    i1 = 4u;
    i0 = (u32)((s32)i0 >> (i1 & 31));
    j0 = (u64)(i0);
    l6 = j0;
    L11: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      j0 = l6;
      j1 = 7ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto L11;}
    i0 = l2;
    i1 = l1;
    i0 = i0 == i1;
    if (i0) {goto B12;}
    i0 = l3;
    i1 = 4294967280u;
    i0 &= i1;
    i1 = l4;
    i0 += i1;
    l4 = i0;
    B12:;
    i0 = l4;
    i1 = l5;
    i2 = 32u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    l1 = i1;
    i0 += i1;
    i1 = l5;
    i1 = i32_load((&memory), (u64)(i1 + 28));
    l2 = i1;
    i0 -= i1;
    l4 = i0;
    i0 = l1;
    i1 = l2;
    i0 -= i1;
    j0 = (u64)(i0);
    l6 = j0;
    L13: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      j0 = l6;
      j1 = 7ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto L13;}
    i0 = l5;
    i1 = 40u;
    i0 += i1;
    l5 = i0;
    i1 = l0;
    i0 = i0 != i1;
    if (i0) {goto L10;}
  i0 = p0;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  B9:;
  i0 = p1;
  i1 = 52u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 48));
  l5 = i1;
  i0 -= i1;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l6 = j0;
  L14: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l6;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L14;}
  i0 = p0;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B15;}
  L16: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = l5;
    i2 = 8u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    l1 = i1;
    i0 += i1;
    i1 = 2u;
    i0 += i1;
    i1 = l5;
    i1 = i32_load((&memory), (u64)(i1 + 4));
    l2 = i1;
    i0 -= i1;
    l4 = i0;
    i0 = l1;
    i1 = l2;
    i0 -= i1;
    j0 = (u64)(i0);
    l6 = j0;
    L17: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      j0 = l6;
      j1 = 7ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto L17;}
    i0 = l5;
    i1 = 16u;
    i0 += i1;
    l5 = i0;
    i1 = l3;
    i0 = i0 != i1;
    if (i0) {goto L16;}
  i0 = p0;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  B15:;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f31(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l5 = 0;
  u64 l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 16u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p1;
  i2 = 4u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 4u;
  i1 += i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l2;
  i0 -= i1;
  i1 = 1u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 16u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  i2 = 2u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 2u;
  i1 += i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l2;
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 16u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  i2 = 4u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 4u;
  i1 += i2;
  l3 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  j0 = i64_load32_u((&memory), (u64)(i0 + 12));
  l4 = j0;
  L6: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l4;
    i0 = (u32)(j0);
    l2 = i0;
    i0 = l5;
    j1 = l4;
    j2 = 7ull;
    j1 >>= (j2 & 63);
    l4 = j1;
    j2 = 0ull;
    i1 = j1 != j2;
    l0 = i1;
    i2 = 7u;
    i1 <<= (i2 & 31);
    i2 = l2;
    i3 = 127u;
    i2 &= i3;
    i1 |= i2;
    i32_store8((&memory), (u64)(i0 + 14), i1);
    i0 = p0;
    i1 = 8u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l3;
    i0 -= i1;
    i1 = 0u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 4u;
    i0 += i1;
    l2 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i2 = 14u;
    i1 += i2;
    i2 = 1u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = l2;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 1u;
    i1 += i2;
    l3 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l0;
    if (i0) {goto L6;}
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l3;
  i0 -= i1;
  i1 = 0u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 16u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l2 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p1;
  i2 = 16u;
  i1 += i2;
  i2 = 1u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = l2;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 1u;
  i1 += i2;
  l3 = i1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  j0 = i64_load32_u((&memory), (u64)(i0 + 20));
  l4 = j0;
  L11: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l4;
    i0 = (u32)(j0);
    l0 = i0;
    i0 = l5;
    j1 = l4;
    j2 = 7ull;
    j1 >>= (j2 & 63);
    l4 = j1;
    j2 = 0ull;
    i1 = j1 != j2;
    p1 = i1;
    i2 = 7u;
    i1 <<= (i2 & 31);
    i2 = l0;
    i3 = 127u;
    i2 &= i3;
    i1 |= i2;
    i32_store8((&memory), (u64)(i0 + 15), i1);
    i0 = l1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l3;
    i0 -= i1;
    i1 = 0u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i2 = 15u;
    i1 += i2;
    i2 = 1u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = l2;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 1u;
    i1 += i2;
    l3 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    if (i0) {goto L11;}
  i0 = 0u;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f32_0(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1));
  i0 -= i1;
  i1 = 40u;
  i0 = I32_DIV_S(i0, i1);
  j0 = (u64)(i0);
  l3 = j0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l4 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l2 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    l5 = i0;
    i0 = l6;
    j1 = l3;
    j2 = 7ull;
    j1 >>= (j2 & 63);
    l3 = j1;
    j2 = 0ull;
    i1 = j1 != j2;
    l0 = i1;
    i2 = 7u;
    i1 <<= (i2 & 31);
    i2 = l5;
    i3 = 127u;
    i2 &= i3;
    i1 |= i2;
    i32_store8((&memory), (u64)(i0 + 15), i1);
    i0 = l1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l4;
    i0 -= i1;
    i1 = 0u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l6;
    i2 = 15u;
    i1 += i2;
    i2 = 1u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = l2;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 1u;
    i1 += i2;
    l4 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l0;
    if (i0) {goto L0;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l2 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 8u;
    i0 += i1;
    l0 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l4;
    i0 -= i1;
    i1 = 7u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i2 = 8u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = l2;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 8u;
    i1 += i2;
    l4 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l4;
    i0 -= i1;
    i1 = 7u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i2 = 8u;
    i1 += i2;
    i2 = 8u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = l2;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 8u;
    i1 += i2;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p0;
    i1 = l5;
    i2 = 16u;
    i1 += i2;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    i0 = f35(i0, i1);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i1 = l5;
    i2 = 28u;
    i1 += i2;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    i0 = f34(i0, i1);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = 40u;
    i0 += i1;
    l5 = i0;
    i1 = l1;
    i0 = i0 == i1;
    if (i0) {goto B3;}
    i0 = l2;
    i0 = i32_load((&memory), (u64)(i0));
    l4 = i0;
    goto L4;
  B3:;
  i0 = 0u;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f33(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l4 = 0, l5 = 0;
  u64 l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1));
  i0 -= i1;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l3 = j0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l4 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    l2 = i0;
    i0 = l5;
    j1 = l3;
    j2 = 7ull;
    j1 >>= (j2 & 63);
    l3 = j1;
    j2 = 0ull;
    i1 = j1 != j2;
    l0 = i1;
    i2 = 7u;
    i1 <<= (i2 & 31);
    i2 = l2;
    i3 = 127u;
    i2 &= i3;
    i1 |= i2;
    i32_store8((&memory), (u64)(i0 + 15), i1);
    i0 = l1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l4;
    i0 -= i1;
    i1 = 0u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 4u;
    i0 += i1;
    l2 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i2 = 15u;
    i1 += i2;
    i2 = 1u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = l2;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 1u;
    i1 += i2;
    l4 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l0;
    if (i0) {goto L0;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l0 = i1;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l4;
    i0 -= i1;
    i1 = 1u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 4u;
    i0 += i1;
    l4 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l2;
    i2 = 2u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = l4;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 2u;
    i1 += i2;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p0;
    i1 = l2;
    i2 = 4u;
    i1 += i2;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    i0 = f34(i0, i1);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 16u;
    i0 += i1;
    l2 = i0;
    i1 = l0;
    i0 = i0 == i1;
    if (i0) {goto B3;}
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0));
    l4 = i0;
    goto L4;
  B3:;
  i0 = 0u;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f34(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0;
  u64 l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1));
  i0 -= i1;
  j0 = (u64)(i0);
  l5 = j0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l4 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l3 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l5;
    i0 = (u32)(j0);
    l0 = i0;
    i0 = l6;
    j1 = l5;
    j2 = 7ull;
    j1 >>= (j2 & 63);
    l5 = j1;
    j2 = 0ull;
    i1 = j1 != j2;
    l1 = i1;
    i2 = 7u;
    i1 <<= (i2 & 31);
    i2 = l0;
    i3 = 127u;
    i2 &= i3;
    i1 |= i2;
    i32_store8((&memory), (u64)(i0 + 15), i1);
    i0 = l2;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l4;
    i0 -= i1;
    i1 = 0u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l6;
    i2 = 15u;
    i1 += i2;
    i2 = 1u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = l3;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 1u;
    i1 += i2;
    l4 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l1;
    if (i0) {goto L0;}
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l4;
  i0 -= i1;
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = p1;
  i2 = i32_load((&memory), (u64)(i2));
  l0 = i2;
  i1 -= i2;
  l3 = i1;
  i0 = (u32)((s32)i0 >= (s32)i1);
  i1 = 16u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l4 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l0;
  i2 = l3;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = l3;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = 0u;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f35(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l3 = 0, l4 = 0, l5 = 0;
  u64 l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1));
  i0 -= i1;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l2 = j0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l3 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l2;
    i0 = (u32)(j0);
    l1 = i0;
    i0 = l5;
    j1 = l2;
    j2 = 7ull;
    j1 >>= (j2 & 63);
    l2 = j1;
    j2 = 0ull;
    i1 = j1 != j2;
    l4 = i1;
    i2 = 7u;
    i1 <<= (i2 & 31);
    i2 = l1;
    i3 = 127u;
    i2 &= i3;
    i1 |= i2;
    i32_store8((&memory), (u64)(i0 + 15), i1);
    i0 = l0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l3;
    i0 -= i1;
    i1 = 0u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 4u;
    i0 += i1;
    l1 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i2 = 15u;
    i1 += i2;
    i2 = 1u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = l1;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 1u;
    i1 += i2;
    l3 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l4;
    if (i0) {goto L0;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  p1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 8u;
    i0 += i1;
    l0 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l3;
    i0 -= i1;
    i1 = 7u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l4;
    i2 = 8u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = l1;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 8u;
    i1 += i2;
    l3 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l3;
    i0 -= i1;
    i1 = 7u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l4;
    i2 = 8u;
    i1 += i2;
    i2 = 8u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = l1;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 8u;
    i1 += i2;
    l3 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l4 = i0;
    i1 = p1;
    i0 = i0 != i1;
    if (i0) {goto L4;}
  B3:;
  i0 = 0u;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 _ZN5eosio30check_permission_authorizationEyyRKNSt3__13setI10public_keyNS0_4lessIS2_EENS0_9allocatorIS2_EEEERKNS1_INS_16permission_levelENS3_ISA_EENS5_ISA_EEEEy(u64 p0, u64 p1, u32 p2, u32 p3, u64 p4) {
  u32 l0 = 0, l1 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0, l8 = 0, 
      l9 = 0, l10 = 0;
  u64 l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j6;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 32u;
  i1 -= i2;
  l10 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 0u;
  l9 = i0;
  i0 = 0u;
  l7 = i0;
  i0 = 0u;
  l8 = i0;
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = 0u;
  l3 = i0;
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l10;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l0;
  j0 = (u64)(i0);
  l2 = j0;
  L1: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    j0 = l2;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l2 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L1;}
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = p2;
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B4;}
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    l6 = i0;
    i0 = i32_load((&memory), (u64)(i0 + 4));
    l5 = i0;
    i0 = !(i0);
    if (i0) {goto B7;}
    L8: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l5;
      l4 = i0;
      i0 = i32_load((&memory), (u64)(i0));
      l5 = i0;
      if (i0) {goto L8;}
      goto B6;
    B7:;
    i0 = l6;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l4 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l6;
    i0 = i0 == i1;
    if (i0) {goto B6;}
    i0 = l6;
    i1 = 8u;
    i0 += i1;
    l6 = i0;
    L9: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l6;
      i0 = i32_load((&memory), (u64)(i0));
      l5 = i0;
      i1 = 8u;
      i0 += i1;
      l6 = i0;
      i0 = l5;
      i1 = l5;
      i1 = i32_load((&memory), (u64)(i1 + 8));
      l4 = i1;
      i1 = i32_load((&memory), (u64)(i1));
      i0 = i0 != i1;
      if (i0) {goto L9;}
    B6:;
    i0 = l3;
    i1 = 34u;
    i0 += i1;
    l3 = i0;
    i0 = l4;
    i1 = l1;
    i0 = i0 != i1;
    if (i0) {goto L5;}
  i0 = l3;
  i0 = !(i0);
  if (i0) {goto B3;}
  B4:;
  i0 = l10;
  i1 = l3;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f27(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l4 = i0;
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  goto B2;
  B3:;
  i0 = 0u;
  l4 = i0;
  i0 = 0u;
  l5 = i0;
  B2:;
  i0 = l10;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l10;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l10;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l10;
  i1 = 16u;
  i0 += i1;
  i1 = p2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f28(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l7 = i0;
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0));
  l8 = i0;
  B0:;
  i0 = 0u;
  l5 = i0;
  i0 = p3;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  p2 = i0;
  i0 = !(i0);
  if (i0) {goto B12;}
  i0 = 0u;
  l3 = i0;
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l10;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p2;
  j0 = (u64)(i0);
  l2 = j0;
  L13: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    j0 = l2;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l2 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L13;}
  i0 = p3;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = p3;
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B16;}
  L17: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    l6 = i0;
    i0 = i32_load((&memory), (u64)(i0 + 4));
    l5 = i0;
    i0 = !(i0);
    if (i0) {goto B19;}
    L20: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l5;
      l4 = i0;
      i0 = i32_load((&memory), (u64)(i0));
      l5 = i0;
      if (i0) {goto L20;}
      goto B18;
    B19:;
    i0 = l6;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l4 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l6;
    i0 = i0 == i1;
    if (i0) {goto B18;}
    i0 = l6;
    i1 = 8u;
    i0 += i1;
    l6 = i0;
    L21: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l6;
      i0 = i32_load((&memory), (u64)(i0));
      l5 = i0;
      i1 = 8u;
      i0 += i1;
      l6 = i0;
      i0 = l5;
      i1 = l5;
      i1 = i32_load((&memory), (u64)(i1 + 8));
      l4 = i1;
      i1 = i32_load((&memory), (u64)(i1));
      i0 = i0 != i1;
      if (i0) {goto L21;}
    B18:;
    i0 = l3;
    i1 = 16u;
    i0 += i1;
    l3 = i0;
    i0 = l4;
    i1 = l1;
    i0 = i0 != i1;
    if (i0) {goto L17;}
  i0 = l3;
  i0 = !(i0);
  if (i0) {goto B15;}
  B16:;
  i0 = l10;
  i1 = l3;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f27(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l4 = i0;
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  goto B14;
  B15:;
  i0 = 0u;
  l4 = i0;
  i0 = 0u;
  l5 = i0;
  B14:;
  i0 = l10;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l10;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l10;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l10;
  i1 = 16u;
  i0 += i1;
  i1 = p3;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f29(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l9 = i0;
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  B12:;
  j0 = p0;
  j1 = p1;
  i2 = l8;
  i3 = 0u;
  i4 = l0;
  i2 = i4 ? i2 : i3;
  i3 = l7;
  i4 = l8;
  i3 -= i4;
  i4 = 0u;
  i5 = l0;
  i3 = i5 ? i3 : i4;
  i4 = l5;
  i5 = 0u;
  i6 = p2;
  i4 = i6 ? i4 : i5;
  i5 = l9;
  i6 = l5;
  i5 -= i6;
  i6 = 0u;
  i7 = p2;
  i5 = i7 ? i5 : i6;
  j6 = p4;
  i7 = g0;
  i7 = !(i7);
  if (i7) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i7 = 4294967295u;
    i8 = g0;
    i7 += i8;
    g0 = i7;
  }
  i0 = (*Z_envZ_check_permission_authorizationZ_ijjiiiij)(j0, j1, i2, i3, i4, i5, j6);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = l5;
  i0 = !(i0);
  if (i0) {goto B25;}
  i0 = l5;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B25:;
  i0 = l8;
  i0 = !(i0);
  if (i0) {goto B27;}
  i0 = l8;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B27:;
  i0 = 0u;
  i1 = l10;
  i2 = 32u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l4;
  i1 = 0u;
  i0 = (u32)((s32)i0 > (s32)i1);
  FUNC_EPILOGUE;
  return i0;
}

static void _ZN5eosio8multisig7proposeEv(u32 p0) {
  u32 l0 = 0, l1 = 0, l3 = 0, l4 = 0, l6 = 0;
  u64 l2 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 240u;
  i1 -= i2;
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l1;
  l6 = i0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i1 = (*Z_envZ_action_data_sizeZ_iv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i1;
  i32_store((&memory), (u64)(i0 + 200), i1);
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B2;}
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  goto B1;
  B2:;
  i0 = 0u;
  i1 = l1;
  i2 = l4;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B1:;
  i0 = l6;
  i1 = l1;
  i32_store((&memory), (u64)(i0 + 196), i1);
  i0 = l1;
  i1 = l4;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 176), j1);
  i0 = 0u;
  l4 = i0;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 168), i1);
  i0 = l6;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 160), j1);
  i0 = g0;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g0;
    i0 += i1;
    g0 = i0;
  }
  j0 = (*Z_envZ_current_timeZ_jv)();
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = j0;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 148), i1);
  i0 = l6;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0 + 152), i1);
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 156), i1);
  i0 = l6;
  j1 = l5;
  j2 = 1000000ull;
  j1 = DIV_U(j1, j2);
  i1 = (u32)(j1);
  i2 = 60u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 136), i1);
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 196));
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 124), i1);
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 200));
  l0 = i0;
  i0 = l6;
  i1 = l1;
  i32_store((&memory), (u64)(i0 + 120), i1);
  i0 = l6;
  i1 = l1;
  i2 = l0;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 128), i1);
  i0 = l0;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = 184u;
  i0 += i1;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 124));
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 124));
  i2 = 8u;
  i1 += i2;
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 124), i1);
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 128));
  i1 = l1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = 176u;
  i0 += i1;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 124));
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 124));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 124), i1);
  i0 = l6;
  i1 = 120u;
  i0 += i1;
  i1 = l6;
  i2 = 160u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f38(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 124));
  i2 = l6;
  i2 = i32_load((&memory), (u64)(i2 + 120));
  i1 -= i2;
  i32_store((&memory), (u64)(i0 + 116), i1);
  i0 = l6;
  i1 = 120u;
  i0 += i1;
  i1 = l6;
  i2 = 136u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f39(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  j0 = i64_load((&memory), (u64)(i0 + 184));
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  (*Z_envZ_require_authZ_vj)(j0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g0;
    i0 += i1;
    g0 = i0;
  }
  j0 = (*Z_envZ_current_timeZ_jv)();
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = j0;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 136));
  j1 = l5;
  j2 = 1000000ull;
  j1 = DIV_U(j1, j2);
  i1 = (u32)(j1);
  i0 = i0 >= i1;
  i1 = 48u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  j0 = i64_load((&memory), (u64)(i0 + 184));
  l5 = j0;
  i0 = l6;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l2 = j1;
  i64_store((&memory), (u64)(i0 + 72), j1);
  i0 = l6;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 88), j1);
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l6;
  j1 = l5;
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = l6;
  i1 = 100u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = 104u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = 0u;
  l1 = i0;
  j0 = l2;
  j1 = l5;
  j2 = 12531646810004914176ull;
  i3 = l6;
  j3 = i64_load((&memory), (u64)(i3 + 176));
  i4 = g0;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g0;
    i4 += i5;
    g0 = i4;
  }
  i0 = (*Z_envZ_db_find_i64Z_ijjjj)(j0, j1, j2, j3);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B15;}
  i0 = l6;
  i1 = 72u;
  i0 += i1;
  i1 = l0;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f40(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 20));
  i1 = l6;
  i2 = 72u;
  i1 += i2;
  i0 = i0 == i1;
  i1 = 80u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B15:;
  i0 = l1;
  i0 = !(i0);
  i1 = 144u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 56), j1);
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 164));
  l1 = i0;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 160));
  l0 = i1;
  i0 -= i1;
  l3 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  L20: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967295u;
    i0 += i1;
    l4 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L20;}
  i0 = l0;
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B24;}
  i0 = l3;
  i1 = 4294967280u;
  i0 &= i1;
  l1 = i0;
  i1 = l4;
  i0 = i0 != i1;
  if (i0) {goto B23;}
  i0 = 0u;
  l1 = i0;
  i0 = 0u;
  l4 = i0;
  goto B21;
  B24:;
  i0 = 0u;
  i1 = l4;
  i0 -= i1;
  l4 = i0;
  goto B22;
  B23:;
  i0 = l1;
  i1 = l4;
  i0 -= i1;
  l4 = i0;
  B22:;
  i0 = l6;
  i1 = 56u;
  i0 += i1;
  i1 = l4;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f27(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 60));
  l1 = i0;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 56));
  l4 = i0;
  B21:;
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l6;
  i1 = l1;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l6;
  i1 = 16u;
  i0 += i1;
  i1 = l6;
  i2 = 160u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f35(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 196));
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 116));
  l4 = i1;
  i0 += i1;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 200));
  i2 = l4;
  i1 -= i2;
  i2 = 0u;
  i3 = 0u;
  i4 = l6;
  i4 = i32_load((&memory), (u64)(i4 + 56));
  l4 = i4;
  i5 = l6;
  i5 = i32_load((&memory), (u64)(i5 + 60));
  i6 = l4;
  i5 -= i6;
  i6 = g0;
  i6 = !(i6);
  if (i6) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i6 = 4294967295u;
    i7 = g0;
    i6 += i7;
    g0 = i6;
  }
  i0 = (*Z_envZ_check_transaction_authorizationZ_iiiiiii)(i0, i1, i2, i3, i4, i5);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = 0u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 192u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  j0 = i64_load((&memory), (u64)(i0 + 184));
  l5 = j0;
  i0 = l6;
  i1 = l6;
  i2 = 196u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l6;
  i1 = l6;
  i2 = 176u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l6;
  i1 = l6;
  i2 = 116u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l6;
  i1 = l6;
  i2 = 200u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 28), i1);
  i0 = l6;
  j1 = l5;
  i64_store((&memory), (u64)(i0 + 232), j1);
  i0 = l6;
  j0 = i64_load((&memory), (u64)(i0 + 72));
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  j1 = (*Z_envZ_current_receiverZ_jv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = j0 == j1;
  i1 = 240u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 212), i1);
  i0 = l6;
  i1 = l6;
  i2 = 72u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 208), i1);
  i0 = l6;
  i1 = l6;
  i2 = 232u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 216), i1);
  i0 = 32u;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = f64_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l4;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l4;
  i1 = l6;
  i2 = 72u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l6;
  i1 = 208u;
  i0 += i1;
  i1 = l4;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f41(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l6;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1));
  l5 = j1;
  i64_store((&memory), (u64)(i0 + 208), j1);
  i0 = l6;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1 + 24));
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 224), i1);
  i0 = l6;
  i1 = 100u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i1 = l6;
  i2 = 72u;
  i1 += i2;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B34;}
  i0 = l1;
  j1 = l5;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l1;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = l1;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B33;
  B34:;
  i0 = l6;
  i1 = 96u;
  i0 += i1;
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  i2 = l6;
  i3 = 208u;
  i2 += i3;
  i3 = l6;
  i4 = 224u;
  i3 += i4;
  i4 = g0;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g0;
    i4 += i5;
    g0 = i4;
  }
  f42(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B33:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l4 = i0;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B36;}
  i0 = l4;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto B37;}
  i0 = l4;
  i1 = 12u;
  i0 += i1;
  i1 = l1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l1;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B37:;
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B36:;
  i0 = l6;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l6;
  j0 = i64_load((&memory), (u64)(i0 + 184));
  l5 = j0;
  i0 = l6;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l2 = j1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l6;
  j1 = l5;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l6;
  i1 = 44u;
  i0 += i1;
  l1 = i0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = 48u;
  i0 += i1;
  l0 = i0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = l6;
  i2 = 160u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l6;
  i1 = l6;
  i2 = 176u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l6;
  j1 = l5;
  i64_store((&memory), (u64)(i0 + 232), j1);
  j0 = l2;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  j1 = (*Z_envZ_current_receiverZ_jv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = j0 == j1;
  i1 = 240u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 212), i1);
  i0 = l6;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 208), i1);
  i0 = l6;
  i1 = l6;
  i2 = 232u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 216), i1);
  i0 = 48u;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = f64_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l4;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l6;
  i1 = 208u;
  i0 += i1;
  i1 = l4;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f43(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 224), i1);
  i0 = l6;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1));
  l5 = j1;
  i64_store((&memory), (u64)(i0 + 208), j1);
  i0 = l6;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1 + 36));
  l3 = i1;
  i32_store((&memory), (u64)(i0 + 204), i1);
  i0 = l1;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B45;}
  i0 = p0;
  j1 = l5;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p0;
  i1 = l3;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 224), i1);
  i0 = p0;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l1;
  i1 = p0;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B44;
  B45:;
  i0 = l6;
  i1 = 40u;
  i0 += i1;
  i1 = l6;
  i2 = 224u;
  i1 += i2;
  i2 = l6;
  i3 = 208u;
  i2 += i3;
  i3 = l6;
  i4 = 204u;
  i3 += i4;
  i4 = g0;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g0;
    i4 += i5;
    g0 = i4;
  }
  f44(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B44:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 224));
  l4 = i0;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 224), i1);
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B47;}
  i0 = l4;
  i0 = i32_load((&memory), (u64)(i0 + 20));
  p0 = i0;
  i0 = !(i0);
  if (i0) {goto B48;}
  i0 = l4;
  i1 = 24u;
  i0 += i1;
  i1 = p0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B48:;
  i0 = l4;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  p0 = i0;
  i0 = !(i0);
  if (i0) {goto B50;}
  i0 = l4;
  i1 = 12u;
  i0 += i1;
  i1 = p0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B50:;
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B47:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 40));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B53;}
  i0 = l6;
  i1 = 44u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B55;}
  L56: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 4294967272u;
    i0 += i1;
    p0 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l4 = i0;
    i0 = p0;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l4;
    i0 = !(i0);
    if (i0) {goto B57;}
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0 + 20));
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto B58;}
    i0 = l4;
    i1 = 24u;
    i0 += i1;
    i1 = l1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l1;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B58:;
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto B60;}
    i0 = l4;
    i1 = 12u;
    i0 += i1;
    i1 = l1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l1;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B60:;
    i0 = l4;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B57:;
    i0 = l0;
    i1 = p0;
    i0 = i0 != i1;
    if (i0) {goto L56;}
  i0 = l6;
  i1 = 40u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B54;
  B55:;
  i0 = l0;
  l4 = i0;
  B54:;
  i0 = l3;
  i1 = l0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B53:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 56));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B64;}
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 60), i1);
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B64:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 96));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B66;}
  i0 = l6;
  i1 = 100u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B68;}
  L69: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l4 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    p0 = i0;
    i0 = l4;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p0;
    i0 = !(i0);
    if (i0) {goto B70;}
    i0 = p0;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto B71;}
    i0 = p0;
    i1 = 12u;
    i0 += i1;
    i1 = l1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l1;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B71:;
    i0 = p0;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B70:;
    i0 = l0;
    i1 = l4;
    i0 = i0 != i1;
    if (i0) {goto L69;}
  i0 = l6;
  i1 = 96u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B67;
  B68:;
  i0 = l0;
  l4 = i0;
  B67:;
  i0 = l3;
  i1 = l0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B66:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 160));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B75;}
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 164), i1);
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B75:;
  i0 = 0u;
  i1 = l6;
  i2 = 240u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f38(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l4 = 0, l5 = 0;
  u64 l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l5 = i0;
  i0 = 0u;
  l4 = i0;
  j0 = 0ull;
  l3 = j0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = l0;
    i1 = i32_load((&memory), (u64)(i1));
    i0 = i0 < i1;
    i1 = 336u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i0 = i32_load((&memory), (u64)(i0));
    l5 = i0;
    i0 = i32_load8_u((&memory), (u64)(i0));
    l2 = i0;
    i0 = l1;
    i1 = l5;
    i2 = 1u;
    i1 += i2;
    l5 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l2;
    i1 = 127u;
    i0 &= i1;
    i1 = l4;
    i2 = 255u;
    i1 &= i2;
    l4 = i1;
    i0 <<= (i1 & 31);
    j0 = (u64)(i0);
    j1 = l3;
    j0 |= j1;
    l3 = j0;
    i0 = l4;
    i1 = 7u;
    i0 += i1;
    l4 = i0;
    i0 = l2;
    i1 = 7u;
    i0 >>= (i1 & 31);
    if (i0) {goto L0;}
  j0 = l3;
  i0 = (u32)(j0);
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l0 = i1;
  i2 = p1;
  i2 = i32_load((&memory), (u64)(i2));
  l5 = i2;
  i1 -= i2;
  i2 = 4u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  l4 = i1;
  i0 = i0 <= i1;
  if (i0) {goto B4;}
  i0 = p1;
  i1 = l2;
  i2 = l4;
  i1 -= i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f46(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l0 = i1;
  i0 = i0 != i1;
  if (i0) {goto B3;}
  goto B2;
  B4:;
  i0 = l2;
  i1 = l4;
  i0 = i0 >= i1;
  if (i0) {goto B6;}
  i0 = p1;
  i1 = 4u;
  i0 += i1;
  i1 = l5;
  i2 = l2;
  i3 = 4u;
  i2 <<= (i3 & 31);
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0), i1);
  B6:;
  i0 = l5;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B2;}
  B3:;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l2 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  L7: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 8u;
    i0 += i1;
    l1 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l4;
    i0 -= i1;
    i1 = 7u;
    i0 = i0 > i1;
    i1 = 32u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = l2;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 8u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = l2;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 8u;
    i1 += i2;
    l4 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l4;
    i0 -= i1;
    i1 = 7u;
    i0 = i0 > i1;
    i1 = 32u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = 8u;
    i0 += i1;
    i1 = l2;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 8u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = l2;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 8u;
    i1 += i2;
    l4 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l5;
    i1 = 16u;
    i0 += i1;
    l5 = i0;
    i1 = l0;
    i0 = i0 != i1;
    if (i0) {goto L7;}
  B2:;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f39(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l4 = 0, l5 = 0;
  u64 l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 4u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 4u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 1u;
  i0 = i0 > i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 4u;
  i0 += i1;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 2u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 2u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 4u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 4u;
  i1 += i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 0u;
  l4 = i0;
  j0 = 0ull;
  l3 = j0;
  L6: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = p0;
    i2 = 8u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    i0 = i0 < i1;
    i1 = 336u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 4u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l2 = i0;
    i0 = i32_load8_u((&memory), (u64)(i0));
    l0 = i0;
    i0 = l5;
    i1 = l2;
    i2 = 1u;
    i1 += i2;
    l2 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l0;
    i1 = 127u;
    i0 &= i1;
    i1 = l4;
    i2 = 255u;
    i1 &= i2;
    l4 = i1;
    i0 <<= (i1 & 31);
    j0 = (u64)(i0);
    j1 = l3;
    j0 |= j1;
    l3 = j0;
    i0 = l4;
    i1 = 7u;
    i0 += i1;
    l4 = i0;
    i0 = l0;
    i1 = 7u;
    i0 >>= (i1 & 31);
    if (i0) {goto L6;}
  i0 = p1;
  j1 = l3;
  i64_store32((&memory), (u64)(i0 + 12), j1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l2;
  i0 = i0 != i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  i1 = p0;
  i2 = 4u;
  i1 += i2;
  l2 = i1;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 1u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = l2;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 1u;
  i1 += i2;
  l4 = i1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = 0u;
  l5 = i0;
  j0 = 0ull;
  l3 = j0;
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = l1;
    i1 = i32_load((&memory), (u64)(i1));
    i0 = i0 < i1;
    i1 = 336u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i0 = i32_load((&memory), (u64)(i0));
    l4 = i0;
    i0 = i32_load8_u((&memory), (u64)(i0));
    l0 = i0;
    i0 = l2;
    i1 = l4;
    i2 = 1u;
    i1 += i2;
    l4 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l0;
    i1 = 127u;
    i0 &= i1;
    i1 = l5;
    i2 = 255u;
    i1 &= i2;
    l5 = i1;
    i0 <<= (i1 & 31);
    j0 = (u64)(i0);
    j1 = l3;
    j0 |= j1;
    l3 = j0;
    i0 = l5;
    i1 = 7u;
    i0 += i1;
    l5 = i0;
    i0 = l0;
    i1 = 7u;
    i0 >>= (i1 & 31);
    if (i0) {goto L10;}
  i0 = p1;
  j1 = l3;
  i64_store32((&memory), (u64)(i0 + 20), j1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f40(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  u64 l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 48u;
  i0 -= i1;
  l7 = i0;
  l6 = i0;
  i0 = 0u;
  i1 = l7;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 24));
  l0 = i1;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = l0;
  i0 -= i1;
  l1 = i0;
  i0 = l5;
  i1 = 4294967272u;
  i0 += i1;
  l4 = i0;
  L1: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = p1;
    i0 = i0 == i1;
    if (i0) {goto B0;}
    i0 = l4;
    l5 = i0;
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    l4 = i0;
    i0 = l2;
    i1 = l1;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L1;}
  B0:;
  i0 = l5;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = l5;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B2;
  B3:;
  i0 = p1;
  i1 = 0u;
  i2 = 0u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_db_get_i64Z_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i1 = 31u;
  i0 >>= (i1 & 31);
  i1 = 1u;
  i0 ^= i1;
  i1 = 304u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  goto B6;
  B7:;
  i0 = 0u;
  i1 = l7;
  i2 = l4;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B6:;
  i0 = p1;
  i1 = l2;
  i2 = l4;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_db_get_i64Z_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l6;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l6;
  i1 = l2;
  i2 = l4;
  i1 += i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l4;
  i1 = 512u;
  i0 = i0 <= i1;
  if (i0) {goto B10;}
  i0 = l2;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = 40u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 36));
  l2 = i0;
  B10:;
  i0 = 32u;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = f64_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l4;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l4;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l5;
  i1 = l2;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = l2;
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = l2;
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l6;
  i1 = 32u;
  i0 += i1;
  i1 = l4;
  i2 = 8u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f45(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l6;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1));
  l3 = j1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l6;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1 + 24));
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  p1 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = p0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B17;}
  i0 = l2;
  j1 = l3;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l2;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l2;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = l2;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B16;
  B17:;
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  i1 = l6;
  i2 = 24u;
  i1 += i2;
  i2 = l6;
  i3 = 16u;
  i2 += i3;
  i3 = l6;
  i4 = 12u;
  i3 += i4;
  i4 = g0;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g0;
    i4 += i5;
    g0 = i4;
  }
  f42(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B16:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  l2 = i0;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l2;
  i0 = !(i0);
  if (i0) {goto B2;}
  i0 = l2;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l5 = i0;
  i0 = !(i0);
  if (i0) {goto B19;}
  i0 = l2;
  i1 = 12u;
  i0 += i1;
  i1 = l5;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B19:;
  i0 = l2;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B2:;
  i0 = 0u;
  i1 = l6;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l4;
  FUNC_EPILOGUE;
  return i0;
}

static void f41(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l8 = 0;
  u64 l7 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l3 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i0 = p1;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l4 = i1;
  i1 = i32_load((&memory), (u64)(i1));
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l4;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i0 = 0u;
  l6 = i0;
  i0 = l3;
  l8 = i0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l8;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l4;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l1 = i0;
  i0 = l8;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = 0u;
  l2 = i0;
  i0 = l4;
  i0 = i32_load((&memory), (u64)(i0 + 12));
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1));
  l1 = i1;
  i0 -= i1;
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = l4;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 <= (s32)i1);
  if (i0) {goto B0;}
  i0 = l8;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i1 = f64_0(i1);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i1;
  i2 = l4;
  i1 += i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l8;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l2;
  i1 = l5;
  i2 = l1;
  i1 += i2;
  i2 = l4;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l8;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B1:;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B5;}
  i0 = p1;
  i1 = 12u;
  i0 += i1;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  l4 = i0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  goto B4;
  B5:;
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  l4 = i0;
  B4:;
  i0 = l4;
  i1 = l6;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = 12u;
  i0 += i1;
  i1 = l6;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i0 -= i1;
  l4 = i0;
  i0 = l6;
  i1 = l2;
  i0 -= i1;
  j0 = (u64)(i0);
  l7 = j0;
  L7: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l7;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l7 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L7;}
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B9;}
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
  goto B8;
  B9:;
  i0 = 0u;
  i1 = l3;
  i2 = l4;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B8:;
  i0 = l4;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 16u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = p1;
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 12u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 -= i1;
  j0 = (u64)(i0);
  l7 = j0;
  i0 = l5;
  i1 = 8u;
  i0 += i1;
  l6 = i0;
  i0 = l5;
  i1 = l4;
  i0 += i1;
  l1 = i0;
  L13: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l7;
    i0 = (u32)(j0);
    l2 = i0;
    i0 = l8;
    j1 = l7;
    j2 = 7ull;
    j1 >>= (j2 & 63);
    l7 = j1;
    j2 = 0ull;
    i1 = j1 != j2;
    l3 = i1;
    i2 = 7u;
    i1 <<= (i2 & 31);
    i2 = l2;
    i3 = 127u;
    i2 &= i3;
    i1 |= i2;
    i32_store8((&memory), (u64)(i0 + 15), i1);
    i0 = l1;
    i1 = l6;
    i0 -= i1;
    i1 = 0u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 16u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i1 = l8;
    i2 = 15u;
    i1 += i2;
    i2 = 1u;
    i3 = g0;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g0;
      i3 += i4;
      g0 = i3;
    }
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l6 = i0;
    i0 = l3;
    if (i0) {goto L13;}
  i0 = l1;
  i1 = l6;
  i0 -= i1;
  i1 = p1;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = p1;
  i3 = 8u;
  i2 += i3;
  i2 = i32_load((&memory), (u64)(i2));
  l2 = i2;
  i1 -= i2;
  l3 = i1;
  i0 = (u32)((s32)i0 >= (s32)i1);
  i1 = 16u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = l2;
  i2 = l3;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 12531646810004914176ull;
  i3 = p0;
  i3 = i32_load((&memory), (u64)(i3 + 8));
  j3 = i64_load((&memory), (u64)(i3));
  i4 = p1;
  j4 = i64_load((&memory), (u64)(i4));
  l7 = j4;
  i5 = l5;
  i6 = l4;
  i7 = g0;
  i7 = !(i7);
  if (i7) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i7 = 4294967295u;
    i8 = g0;
    i7 += i8;
    g0 = i7;
  }
  i1 = (*Z_envZ_db_store_i64Z_ijjjjii)(j1, j2, j3, j4, i5, i6);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B19;}
  i0 = l5;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B19:;
  j0 = l7;
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B21;}
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l7;
  j3 = 1ull;
  j2 += j3;
  j3 = l7;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&memory), (u64)(i0), j1);
  B21:;
  i0 = 0u;
  i1 = l8;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  goto Bfunc;
  B0:;
  i0 = l8;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f66(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f42(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  l2 = i1;
  i0 -= i1;
  i1 = 24u;
  i0 = I32_DIV_S(i0, i1);
  l0 = i0;
  i1 = 1u;
  i0 += i1;
  l1 = i0;
  i1 = 178956971u;
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  i0 = 178956970u;
  l3 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l2;
  i0 -= i1;
  i1 = 24u;
  i0 = I32_DIV_S(i0, i1);
  l2 = i0;
  i1 = 89478484u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = l1;
  i1 = l2;
  i2 = 1u;
  i1 <<= (i2 & 31);
  l3 = i1;
  i2 = l3;
  i3 = l1;
  i2 = i2 < i3;
  i0 = i2 ? i0 : i1;
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  B3:;
  i0 = l3;
  i1 = 24u;
  i0 *= i1;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = f64_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  goto B0;
  B2:;
  i0 = 0u;
  l3 = i0;
  i0 = 0u;
  l2 = i0;
  goto B0;
  B1:;
  i0 = p0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f66(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B0:;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i0 = p1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l2;
  i1 = l0;
  i2 = 24u;
  i1 *= i2;
  i0 += i1;
  p1 = i0;
  i1 = l1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p1;
  i1 = p3;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l2;
  i1 = l3;
  i2 = 24u;
  i1 *= i2;
  i0 += i1;
  l0 = i0;
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  l1 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  l3 = i1;
  i0 = i0 == i1;
  if (i0) {goto B7;}
  L8: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 4294967272u;
    i0 += i1;
    p2 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    p3 = i0;
    i0 = p2;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = 4294967272u;
    i0 += i1;
    i1 = p3;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = 4294967288u;
    i0 += i1;
    i1 = l2;
    i2 = 4294967288u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = 4294967284u;
    i0 += i1;
    i1 = l2;
    i2 = 4294967284u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = 4294967280u;
    i0 += i1;
    i1 = l2;
    i2 = 4294967280u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = 4294967272u;
    i0 += i1;
    p1 = i0;
    i0 = p2;
    l2 = i0;
    i0 = l3;
    i1 = p2;
    i0 = i0 != i1;
    if (i0) {goto L8;}
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  p2 = i0;
  goto B6;
  B7:;
  i0 = l3;
  p2 = i0;
  B6:;
  i0 = p0;
  i1 = p1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i1 = l1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = l0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    l3 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    p1 = i0;
    i0 = l3;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i0 = !(i0);
    if (i0) {goto B11;}
    i0 = p1;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto B12;}
    i0 = p1;
    i1 = 12u;
    i0 += i1;
    i1 = l2;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l2;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B12:;
    i0 = p1;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B11:;
    i0 = p2;
    i1 = l3;
    i0 = i0 != i1;
    if (i0) {goto L10;}
  B9:;
  i0 = p2;
  i0 = !(i0);
  if (i0) {goto B15;}
  i0 = p2;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B15:;
  FUNC_EPILOGUE;
}

static void f43(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0, l7 = 0, l8 = 0, 
      l9 = 0;
  u64 l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 16u;
  i0 -= i1;
  l9 = i0;
  l8 = i0;
  i0 = 0u;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i0 = p1;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l6 = i1;
  i1 = i32_load((&memory), (u64)(i1));
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l6 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = p1;
  i1 = 12u;
  i0 += i1;
  l7 = i0;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  goto B0;
  B1:;
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = p1;
  i1 = 12u;
  i0 += i1;
  l7 = i0;
  B0:;
  i0 = l1;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l7;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 8));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l7;
  i0 = i32_load((&memory), (u64)(i0));
  l7 = i0;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1));
  l2 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  i0 = 8u;
  l6 = i0;
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l6 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L3;}
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = l7;
  i0 = i0 == i1;
  if (i0) {goto B4;}
  i0 = l4;
  i1 = 4294967280u;
  i0 &= i1;
  i1 = l6;
  i0 += i1;
  l6 = i0;
  B4:;
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l7 = i0;
  i1 = l3;
  i1 = i32_load((&memory), (u64)(i1));
  l2 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l6 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L5;}
  i0 = l2;
  i1 = l7;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  i0 = l4;
  i1 = 4294967280u;
  i0 &= i1;
  i1 = l6;
  i0 += i1;
  l6 = i0;
  B6:;
  i0 = l6;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B8;}
  i0 = l6;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l7 = i0;
  goto B7;
  B8:;
  i0 = 0u;
  i1 = l9;
  i2 = l6;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l7 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B7:;
  i0 = l8;
  i1 = l7;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l8;
  i1 = l7;
  i2 = l6;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l6;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 16u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l7;
  i1 = p1;
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l8;
  i1 = l7;
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l8;
  i1 = l1;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f35(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l8;
  i1 = l3;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f35(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 3849304914312298496ull;
  i3 = p0;
  i3 = i32_load((&memory), (u64)(i3 + 8));
  j3 = i64_load((&memory), (u64)(i3));
  i4 = p1;
  j4 = i64_load((&memory), (u64)(i4));
  l5 = j4;
  i5 = l7;
  i6 = l6;
  i7 = g0;
  i7 = !(i7);
  if (i7) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i7 = 4294967295u;
    i8 = g0;
    i7 += i8;
    g0 = i7;
  }
  i1 = (*Z_envZ_db_store_i64Z_ijjjjii)(j1, j2, j3, j4, i5, i6);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l6;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B15;}
  i0 = l7;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B15:;
  j0 = l5;
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B17;}
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l5;
  j3 = 1ull;
  j2 += j3;
  j3 = l5;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&memory), (u64)(i0), j1);
  B17:;
  i0 = 0u;
  i1 = l8;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f44(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  l2 = i1;
  i0 -= i1;
  i1 = 24u;
  i0 = I32_DIV_S(i0, i1);
  l0 = i0;
  i1 = 1u;
  i0 += i1;
  l1 = i0;
  i1 = 178956971u;
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  i0 = 178956970u;
  l3 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l2;
  i0 -= i1;
  i1 = 24u;
  i0 = I32_DIV_S(i0, i1);
  l2 = i0;
  i1 = 89478484u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = l1;
  i1 = l2;
  i2 = 1u;
  i1 <<= (i2 & 31);
  l3 = i1;
  i2 = l3;
  i3 = l1;
  i2 = i2 < i3;
  i0 = i2 ? i0 : i1;
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  B3:;
  i0 = l3;
  i1 = 24u;
  i0 *= i1;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = f64_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  goto B0;
  B2:;
  i0 = 0u;
  l3 = i0;
  i0 = 0u;
  l2 = i0;
  goto B0;
  B1:;
  i0 = p0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f66(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B0:;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i0 = p1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l2;
  i1 = l0;
  i2 = 24u;
  i1 *= i2;
  i0 += i1;
  p1 = i0;
  i1 = l1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p1;
  i1 = p3;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l2;
  i1 = l3;
  i2 = 24u;
  i1 *= i2;
  i0 += i1;
  l0 = i0;
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  l1 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  l3 = i1;
  i0 = i0 == i1;
  if (i0) {goto B7;}
  L8: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 4294967272u;
    i0 += i1;
    p2 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    p3 = i0;
    i0 = p2;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = 4294967272u;
    i0 += i1;
    i1 = p3;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = 4294967288u;
    i0 += i1;
    i1 = l2;
    i2 = 4294967288u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = 4294967284u;
    i0 += i1;
    i1 = l2;
    i2 = 4294967284u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = 4294967280u;
    i0 += i1;
    i1 = l2;
    i2 = 4294967280u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = 4294967272u;
    i0 += i1;
    p1 = i0;
    i0 = p2;
    l2 = i0;
    i0 = l3;
    i1 = p2;
    i0 = i0 != i1;
    if (i0) {goto L8;}
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  p2 = i0;
  goto B6;
  B7:;
  i0 = l3;
  p2 = i0;
  B6:;
  i0 = p0;
  i1 = p1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i1 = l1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = l0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    l3 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    p1 = i0;
    i0 = l3;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i0 = !(i0);
    if (i0) {goto B11;}
    i0 = p1;
    i0 = i32_load((&memory), (u64)(i0 + 20));
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto B12;}
    i0 = p1;
    i1 = 24u;
    i0 += i1;
    i1 = l2;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l2;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B12:;
    i0 = p1;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto B14;}
    i0 = p1;
    i1 = 12u;
    i0 += i1;
    i1 = l2;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l2;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B14:;
    i0 = p1;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B11:;
    i0 = p2;
    i1 = l3;
    i0 = i0 != i1;
    if (i0) {goto L10;}
  B9:;
  i0 = p2;
  i0 = !(i0);
  if (i0) {goto B17;}
  i0 = p2;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B17:;
  FUNC_EPILOGUE;
}

static u32 f45(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l5 = 0;
  u64 l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l3 = i0;
  i0 = 0u;
  l5 = i0;
  j0 = 0ull;
  l4 = j0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = l0;
    i1 = i32_load((&memory), (u64)(i1));
    i0 = i0 < i1;
    i1 = 336u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i0 = i32_load((&memory), (u64)(i0));
    l3 = i0;
    i0 = i32_load8_u((&memory), (u64)(i0));
    l2 = i0;
    i0 = l1;
    i1 = l3;
    i2 = 1u;
    i1 += i2;
    l3 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l2;
    i1 = 127u;
    i0 &= i1;
    i1 = l5;
    i2 = 255u;
    i1 &= i2;
    l5 = i1;
    i0 <<= (i1 & 31);
    j0 = (u64)(i0);
    j1 = l4;
    j0 |= j1;
    l4 = j0;
    i0 = l5;
    i1 = 7u;
    i0 += i1;
    l5 = i0;
    i0 = l2;
    i1 = 7u;
    i0 >>= (i1 & 31);
    if (i0) {goto L0;}
  j0 = l4;
  i0 = (u32)(j0);
  l1 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l5 = i1;
  i2 = p1;
  i2 = i32_load((&memory), (u64)(i2));
  l2 = i2;
  i1 -= i2;
  l0 = i1;
  i0 = i0 <= i1;
  if (i0) {goto B3;}
  i0 = p1;
  i1 = l1;
  i2 = l0;
  i1 -= i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f27(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i0 = p1;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  goto B2;
  B3:;
  i0 = l1;
  i1 = l0;
  i0 = i0 >= i1;
  if (i0) {goto B2;}
  i0 = p1;
  i1 = 4u;
  i0 += i1;
  i1 = l2;
  i2 = l1;
  i1 += i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0), i1);
  B2:;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l3;
  i0 -= i1;
  i1 = l5;
  i2 = l2;
  i1 -= i2;
  l3 = i1;
  i0 = i0 >= i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = p0;
  i2 = 4u;
  i1 += i2;
  l5 = i1;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = l3;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = l5;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = l3;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f46(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l0 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l5 = i1;
  i0 -= i1;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  i1 = p1;
  i0 = i0 >= i1;
  if (i0) {goto B4;}
  i0 = l5;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  l4 = i1;
  i0 -= i1;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  l1 = i0;
  i1 = p1;
  i0 += i1;
  l2 = i0;
  i1 = 268435456u;
  i0 = i0 >= i1;
  if (i0) {goto B2;}
  i0 = 268435455u;
  l3 = i0;
  i0 = l0;
  i1 = l4;
  i0 -= i1;
  l0 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  i1 = 134217726u;
  i0 = i0 > i1;
  if (i0) {goto B5;}
  i0 = l2;
  i1 = l0;
  i2 = 3u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  l3 = i1;
  i2 = l3;
  i3 = l2;
  i2 = i2 < i3;
  i0 = i2 ? i0 : i1;
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = l3;
  i1 = 268435456u;
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  B5:;
  i0 = l3;
  i1 = 4u;
  i0 <<= (i1 & 31);
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = f64_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B0;
  B4:;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i1 = l5;
  i2 = p1;
  i3 = 4u;
  i2 <<= (i3 & 31);
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto Bfunc;
  B3:;
  i0 = 0u;
  l3 = i0;
  i0 = 0u;
  l0 = i0;
  goto B0;
  B2:;
  i0 = p0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f66(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B1:;
  i0 = g0;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g0;
    i0 += i1;
    g0 = i0;
  }
  (*Z_envZ_abortZ_vv)();
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B0:;
  i0 = l0;
  i1 = l1;
  i2 = 4u;
  i1 <<= (i2 & 31);
  i0 += i1;
  l1 = i0;
  i1 = l5;
  i2 = l4;
  i1 -= i2;
  l5 = i1;
  i0 -= i1;
  l2 = i0;
  i0 = l1;
  i1 = p1;
  i2 = 4u;
  i1 <<= (i2 & 31);
  i0 += i1;
  p1 = i0;
  i0 = l0;
  i1 = l3;
  i2 = 4u;
  i1 <<= (i2 & 31);
  i0 += i1;
  l3 = i0;
  i0 = l5;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B9;}
  i0 = l2;
  i1 = l4;
  i2 = l5;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  B9:;
  i0 = p0;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i1 = p1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B11;}
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void _ZN5eosio8multisig7approveEyNS_4nameENS_16permission_levelE(u32 p0, u64 p1, u64 p2, u32 p3) {
  u32 l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  u64 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 64u;
  i1 -= i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  l1 = j1;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_require_auth2Z_vjj)(j0, j1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = 56u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l5;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = l5;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l5;
  i1 = 24u;
  i0 += i1;
  j1 = p2;
  i2 = 352u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = f48(i0, j1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  p0 = i0;
  i1 = l2;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l4 = i1;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    j0 = i64_load((&memory), (u64)(i0));
    j1 = l0;
    i0 = j0 != j1;
    if (i0) {goto B4;}
    i0 = p0;
    i1 = 8u;
    i0 += i1;
    j0 = i64_load((&memory), (u64)(i0));
    j1 = l1;
    i0 = j0 == j1;
    if (i0) {goto B1;}
    B4:;
    i0 = l4;
    i1 = p0;
    i2 = 16u;
    i1 += i2;
    p0 = i1;
    i0 = i0 != i1;
    if (i0) {goto L3;}
  i0 = l4;
  p0 = i0;
  B1:;
  i0 = l5;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = p0;
  i1 = l4;
  i0 = i0 != i1;
  i1 = 384u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l5;
  i1 = 24u;
  i0 += i1;
  i1 = l2;
  j2 = p1;
  i3 = l5;
  i4 = 8u;
  i3 += i4;
  i4 = g0;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g0;
    i4 += i5;
    g0 = i4;
  }
  f49(i0, i1, j2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B7;}
  i0 = l5;
  i1 = 52u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l4 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    p0 = i0;
    i0 = l4;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p0;
    i0 = !(i0);
    if (i0) {goto B11;}
    i0 = p0;
    i0 = i32_load((&memory), (u64)(i0 + 20));
    p3 = i0;
    i0 = !(i0);
    if (i0) {goto B12;}
    i0 = p0;
    i1 = 24u;
    i0 += i1;
    i1 = p3;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p3;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B12:;
    i0 = p0;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    p3 = i0;
    i0 = !(i0);
    if (i0) {goto B14;}
    i0 = p0;
    i1 = 12u;
    i0 += i1;
    i1 = p3;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p3;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B14:;
    i0 = p0;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B11:;
    i0 = l2;
    i1 = l4;
    i0 = i0 != i1;
    if (i0) {goto L10;}
  i0 = l5;
  i1 = 48u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  goto B8;
  B9:;
  i0 = l2;
  p0 = i0;
  B8:;
  i0 = l3;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B7:;
  i0 = 0u;
  i1 = l5;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f48(u32 p0, u64 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 24));
  l0 = i1;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = l4;
  i1 = 4294967272u;
  i0 += i1;
  l3 = i0;
  i0 = 0u;
  i1 = l0;
  i0 -= i1;
  l1 = i0;
  L1: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i0 = i32_load((&memory), (u64)(i0));
    j0 = i64_load((&memory), (u64)(i0));
    j1 = p1;
    i0 = j0 == j1;
    if (i0) {goto B0;}
    i0 = l3;
    l4 = i0;
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    l3 = i0;
    i0 = l2;
    i1 = l1;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L1;}
  B0:;
  i0 = l4;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = l4;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 32));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 80u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B2;
  B3:;
  i0 = 0u;
  l3 = i0;
  i0 = p0;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 3849304914312298496ull;
  j3 = p1;
  i4 = g0;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g0;
    i4 += i5;
    g0 = i4;
  }
  i0 = (*Z_envZ_db_find_i64Z_ijjjj)(j0, j1, j2, j3);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B2;}
  i0 = p0;
  i1 = l2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f51(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 32));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 80u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B2:;
  i0 = l3;
  i1 = 0u;
  i0 = i0 != i1;
  i1 = p2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  FUNC_EPILOGUE;
  return i0;
}

static void f49(u32 p0, u32 p1, u64 p2, u32 p3) {
  u32 l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0, l7 = 0, l8 = 0;
  u64 l0 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 16u;
  i0 -= i1;
  l8 = i0;
  l7 = i0;
  i0 = 0u;
  i1 = l8;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 32));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 448u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  j1 = (*Z_envZ_current_receiverZ_jv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = j0 == j1;
  i1 = 496u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i1 = p1;
  i2 = 28u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 == i1;
  if (i0) {goto B4;}
  i0 = l1;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l3;
  i1 = l3;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B3;
  B4:;
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  i1 = l6;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f50(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B3:;
  i0 = p1;
  i1 = 12u;
  i0 += i1;
  l6 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p3;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i1 = i32_load((&memory), (u64)(i1));
  p3 = i1;
  i2 = 16u;
  i1 += i2;
  l1 = i1;
  i0 -= i1;
  l2 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B6;}
  i0 = p3;
  i1 = l1;
  i2 = l2;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memmoveZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B6:;
  i0 = l6;
  i1 = p3;
  i2 = l3;
  i3 = 4u;
  i2 <<= (i3 & 31);
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  j0 = l0;
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 560u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 8u;
  p3 = i0;
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 8));
  l2 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  L9: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L9;}
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  i0 = l4;
  i1 = 4294967280u;
  i0 &= i1;
  i1 = p3;
  i0 += i1;
  p3 = i0;
  B10:;
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  i1 = l3;
  i1 = i32_load((&memory), (u64)(i1));
  l2 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  L11: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L11;}
  i0 = l2;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto B12;}
  i0 = l4;
  i1 = 4294967280u;
  i0 &= i1;
  i1 = p3;
  i0 += i1;
  p3 = i0;
  B12:;
  i0 = p3;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B14;}
  i0 = p3;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  goto B13;
  B14:;
  i0 = 0u;
  i1 = l8;
  i2 = p3;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B13:;
  i0 = l7;
  i1 = l6;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l7;
  i1 = l6;
  i2 = p3;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = p3;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 16u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = p1;
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l7;
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l7;
  i1 = l1;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f35(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l7;
  i1 = l3;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f35(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 36));
  j1 = p2;
  i2 = l6;
  i3 = p3;
  i4 = g0;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g0;
    i4 += i5;
    g0 = i4;
  }
  (*Z_envZ_db_update_i64Z_vijii)(i0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B21;}
  i0 = l6;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B21:;
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B23;}
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l0;
  j3 = 1ull;
  j2 += j3;
  j3 = l0;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&memory), (u64)(i0), j1);
  B23:;
  i0 = 0u;
  i1 = l7;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f50(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l4 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  l3 = i1;
  i0 -= i1;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  l0 = i0;
  i1 = 1u;
  i0 += i1;
  l1 = i0;
  i1 = 268435456u;
  i0 = i0 >= i1;
  if (i0) {goto B2;}
  i0 = 268435455u;
  l2 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l3;
  i0 -= i1;
  l5 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  i1 = 134217726u;
  i0 = i0 > i1;
  if (i0) {goto B4;}
  i0 = l1;
  i1 = l5;
  i2 = 3u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  l2 = i1;
  i2 = l2;
  i3 = l1;
  i2 = i2 < i3;
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = l2;
  i1 = 268435456u;
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  B4:;
  i0 = l2;
  i1 = 4u;
  i0 <<= (i1 & 31);
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = f64_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  goto B0;
  B3:;
  i0 = 0u;
  l2 = i0;
  i0 = 0u;
  l5 = i0;
  goto B0;
  B2:;
  i0 = p0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f66(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B1:;
  i0 = g0;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g0;
    i0 += i1;
    g0 = i0;
  }
  (*Z_envZ_abortZ_vv)();
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B0:;
  i0 = l5;
  i1 = l0;
  i2 = 4u;
  i1 <<= (i2 & 31);
  i0 += i1;
  l1 = i0;
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l1;
  i1 = l4;
  i2 = l3;
  i1 -= i2;
  p1 = i1;
  i0 -= i1;
  l4 = i0;
  i0 = l5;
  i1 = l2;
  i2 = 4u;
  i1 <<= (i2 & 31);
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  l1 = i0;
  i0 = p1;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B8;}
  i0 = l4;
  i1 = l3;
  i2 = p1;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  B8:;
  i0 = p0;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i1 = l1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i0 = !(i0);
  if (i0) {goto B10;}
  i0 = l3;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B10:;
  FUNC_EPILOGUE;
}

static u32 f51(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  u64 l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 48u;
  i0 -= i1;
  l7 = i0;
  l6 = i0;
  i0 = 0u;
  i1 = l7;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 24));
  l0 = i1;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = l0;
  i0 -= i1;
  l1 = i0;
  i0 = l5;
  i1 = 4294967272u;
  i0 += i1;
  l4 = i0;
  L1: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = p1;
    i0 = i0 == i1;
    if (i0) {goto B0;}
    i0 = l4;
    l5 = i0;
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    l4 = i0;
    i0 = l2;
    i1 = l1;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L1;}
  B0:;
  i0 = l5;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = l5;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B2;
  B3:;
  i0 = p1;
  i1 = 0u;
  i2 = 0u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_db_get_i64Z_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i1 = 31u;
  i0 >>= (i1 & 31);
  i1 = 1u;
  i0 ^= i1;
  i1 = 304u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  goto B6;
  B7:;
  i0 = 0u;
  i1 = l7;
  i2 = l4;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B6:;
  i0 = p1;
  i1 = l2;
  i2 = l4;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_db_get_i64Z_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l6;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l6;
  i1 = l2;
  i2 = l4;
  i1 += i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l4;
  i1 = 512u;
  i0 = i0 <= i1;
  if (i0) {goto B10;}
  i0 = l2;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = 40u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 36));
  l2 = i0;
  B10:;
  i0 = 48u;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = f64_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l4;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l5;
  i1 = l2;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = l2;
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = l2;
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l6;
  i1 = 32u;
  i0 += i1;
  i1 = l4;
  i2 = 8u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f38(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = 32u;
  i0 += i1;
  i1 = l4;
  i2 = 20u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f38(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l6;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1));
  l3 = j1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l6;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1 + 36));
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  p1 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = p0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B18;}
  i0 = l2;
  j1 = l3;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l2;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l2;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = l2;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B17;
  B18:;
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  i1 = l6;
  i2 = 24u;
  i1 += i2;
  i2 = l6;
  i3 = 16u;
  i2 += i3;
  i3 = l6;
  i4 = 12u;
  i3 += i4;
  i4 = g0;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g0;
    i4 += i5;
    g0 = i4;
  }
  f44(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B17:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  l2 = i0;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l2;
  i0 = !(i0);
  if (i0) {goto B2;}
  i0 = l2;
  i0 = i32_load((&memory), (u64)(i0 + 20));
  l5 = i0;
  i0 = !(i0);
  if (i0) {goto B20;}
  i0 = l2;
  i1 = 24u;
  i0 += i1;
  i1 = l5;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B20:;
  i0 = l2;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l5 = i0;
  i0 = !(i0);
  if (i0) {goto B22;}
  i0 = l2;
  i1 = 12u;
  i0 += i1;
  i1 = l5;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B22:;
  i0 = l2;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B2:;
  i0 = 0u;
  i1 = l6;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l4;
  FUNC_EPILOGUE;
  return i0;
}

static void _ZN5eosio8multisig9unapproveEyNS_4nameENS_16permission_levelE(u32 p0, u64 p1, u64 p2, u32 p3) {
  u32 l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  u64 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 64u;
  i1 -= i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  l1 = j1;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_require_auth2Z_vjj)(j0, j1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = 56u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l5;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = l5;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l5;
  i1 = 24u;
  i0 += i1;
  j1 = p2;
  i2 = 352u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = f48(i0, j1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 20));
  p0 = i0;
  i1 = l2;
  i2 = 24u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l4 = i1;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    j0 = i64_load((&memory), (u64)(i0));
    j1 = l0;
    i0 = j0 != j1;
    if (i0) {goto B4;}
    i0 = p0;
    i1 = 8u;
    i0 += i1;
    j0 = i64_load((&memory), (u64)(i0));
    j1 = l1;
    i0 = j0 == j1;
    if (i0) {goto B1;}
    B4:;
    i0 = l4;
    i1 = p0;
    i2 = 16u;
    i1 += i2;
    p0 = i1;
    i0 = i0 != i1;
    if (i0) {goto L3;}
  i0 = l4;
  p0 = i0;
  B1:;
  i0 = l5;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = p0;
  i1 = l4;
  i0 = i0 != i1;
  i1 = 624u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l5;
  i1 = 24u;
  i0 += i1;
  i1 = l2;
  j2 = p1;
  i3 = l5;
  i4 = 8u;
  i3 += i4;
  i4 = g0;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g0;
    i4 += i5;
    g0 = i4;
  }
  f53(i0, i1, j2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B7;}
  i0 = l5;
  i1 = 52u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l4 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    p0 = i0;
    i0 = l4;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p0;
    i0 = !(i0);
    if (i0) {goto B11;}
    i0 = p0;
    i0 = i32_load((&memory), (u64)(i0 + 20));
    p3 = i0;
    i0 = !(i0);
    if (i0) {goto B12;}
    i0 = p0;
    i1 = 24u;
    i0 += i1;
    i1 = p3;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p3;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B12:;
    i0 = p0;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    p3 = i0;
    i0 = !(i0);
    if (i0) {goto B14;}
    i0 = p0;
    i1 = 12u;
    i0 += i1;
    i1 = p3;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p3;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B14:;
    i0 = p0;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B11:;
    i0 = l2;
    i1 = l4;
    i0 = i0 != i1;
    if (i0) {goto L10;}
  i0 = l5;
  i1 = 48u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  goto B8;
  B9:;
  i0 = l2;
  p0 = i0;
  B8:;
  i0 = l3;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B7:;
  i0 = 0u;
  i1 = l5;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f53(u32 p0, u32 p1, u64 p2, u32 p3) {
  u32 l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0, l7 = 0, l8 = 0;
  u64 l0 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 16u;
  i0 -= i1;
  l8 = i0;
  l7 = i0;
  i0 = 0u;
  i1 = l8;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 32));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 448u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  j1 = (*Z_envZ_current_receiverZ_jv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = j0 == j1;
  i1 = 496u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = p1;
  i1 = 12u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i1 = p1;
  i2 = 16u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 == i1;
  if (i0) {goto B4;}
  i0 = l1;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l3;
  i1 = l3;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B3;
  B4:;
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  i1 = l6;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f50(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B3:;
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  l6 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p3;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i1 = i32_load((&memory), (u64)(i1));
  p3 = i1;
  i2 = 16u;
  i1 += i2;
  l1 = i1;
  i0 -= i1;
  l2 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B6;}
  i0 = p3;
  i1 = l1;
  i2 = l2;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memmoveZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B6:;
  i0 = l6;
  i1 = p3;
  i2 = l3;
  i3 = 4u;
  i2 <<= (i3 & 31);
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  j0 = l0;
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 560u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 8u;
  p3 = i0;
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i0 = p1;
  i1 = 12u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 8));
  l2 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  L9: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L9;}
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  i0 = l4;
  i1 = 4294967280u;
  i0 &= i1;
  i1 = p3;
  i0 += i1;
  p3 = i0;
  B10:;
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  i1 = l3;
  i1 = i32_load((&memory), (u64)(i1));
  l2 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  L11: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L11;}
  i0 = l2;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto B12;}
  i0 = l4;
  i1 = 4294967280u;
  i0 &= i1;
  i1 = p3;
  i0 += i1;
  p3 = i0;
  B12:;
  i0 = p3;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B14;}
  i0 = p3;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  goto B13;
  B14:;
  i0 = 0u;
  i1 = l8;
  i2 = p3;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B13:;
  i0 = l7;
  i1 = l6;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l7;
  i1 = l6;
  i2 = p3;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = p3;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 16u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = p1;
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l7;
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l7;
  i1 = l1;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f35(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l7;
  i1 = l3;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f35(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 36));
  j1 = p2;
  i2 = l6;
  i3 = p3;
  i4 = g0;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g0;
    i4 += i5;
    g0 = i4;
  }
  (*Z_envZ_db_update_i64Z_vijii)(i0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B21;}
  i0 = l6;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B21:;
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B23;}
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l0;
  j3 = 1ull;
  j2 += j3;
  j3 = l0;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&memory), (u64)(i0), j1);
  B23:;
  i0 = 0u;
  i1 = l7;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void _ZN5eosio8multisig6cancelEyNS_4nameEy(u32 p0, u64 p1, u64 p2, u64 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 96u;
  i1 -= i2;
  l4 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  j0 = p3;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  (*Z_envZ_require_authZ_vj)(j0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = 40u;
  i0 += i1;
  i1 = 32u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l4;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = l4;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 56), j1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 64), j1);
  i0 = l4;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = l4;
  i1 = 40u;
  i0 += i1;
  j1 = p2;
  i2 = 352u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = f55(i0, j1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  j0 = p3;
  j1 = p1;
  i0 = j0 == j1;
  if (i0) {goto B2;}
  i0 = l3;
  i1 = 12u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i0 = l3;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l1 = i0;
  i0 = g0;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g0;
    i0 += i1;
    g0 = i0;
  }
  j0 = (*Z_envZ_current_timeZ_jv)();
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = j0;
  i0 = l4;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l4;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0 + 16), i1);
  i0 = l4;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l4;
  j1 = p3;
  j2 = 1000000ull;
  j1 = DIV_U(j1, j2);
  i1 = (u32)(j1);
  i2 = 60u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l4;
  i1 = l1;
  i32_store((&memory), (u64)(i0 + 84), i1);
  i0 = l4;
  i1 = l1;
  i32_store((&memory), (u64)(i0 + 80), i1);
  i0 = l4;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 88), i1);
  i0 = l4;
  i1 = 80u;
  i0 += i1;
  i1 = l4;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f39(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g0;
    i0 += i1;
    g0 = i0;
  }
  j0 = (*Z_envZ_current_timeZ_jv)();
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = j0;
  i0 = l4;
  i0 = i32_load((&memory), (u64)(i0));
  j1 = p3;
  j2 = 1000000ull;
  j1 = DIV_U(j1, j2);
  i1 = (u32)(j1);
  i0 = i0 < i1;
  i1 = 656u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B2:;
  i0 = l4;
  i1 = 32u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l4;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l4;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l4;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l4;
  j1 = p2;
  i2 = 352u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = f48(i0, j1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  i0 = l4;
  i1 = 40u;
  i0 += i1;
  i1 = l3;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f56(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = p0;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f57(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B10;}
  i0 = l4;
  i1 = 28u;
  i0 += i1;
  l2 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B12;}
  L13: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    l3 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    p0 = i0;
    i0 = l3;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p0;
    i0 = !(i0);
    if (i0) {goto B14;}
    i0 = p0;
    i0 = i32_load((&memory), (u64)(i0 + 20));
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto B15;}
    i0 = p0;
    i1 = 24u;
    i0 += i1;
    i1 = l1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l1;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B15:;
    i0 = p0;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto B17;}
    i0 = p0;
    i1 = 12u;
    i0 += i1;
    i1 = l1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l1;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B17:;
    i0 = p0;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B14:;
    i0 = l0;
    i1 = l3;
    i0 = i0 != i1;
    if (i0) {goto L13;}
  i0 = l4;
  i1 = 24u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  goto B11;
  B12:;
  i0 = l0;
  p0 = i0;
  B11:;
  i0 = l2;
  i1 = l0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B10:;
  i0 = l4;
  i0 = i32_load((&memory), (u64)(i0 + 64));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B21;}
  i0 = l4;
  i1 = 68u;
  i0 += i1;
  l2 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B23;}
  L24: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 4294967272u;
    i0 += i1;
    p0 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l3 = i0;
    i0 = p0;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto B25;}
    i0 = l3;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto B26;}
    i0 = l3;
    i1 = 12u;
    i0 += i1;
    i1 = l1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l1;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B26:;
    i0 = l3;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B25:;
    i0 = l0;
    i1 = p0;
    i0 = i0 != i1;
    if (i0) {goto L24;}
  i0 = l4;
  i1 = 64u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  goto B22;
  B23:;
  i0 = l0;
  p0 = i0;
  B22:;
  i0 = l2;
  i1 = l0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B21:;
  i0 = 0u;
  i1 = l4;
  i2 = 96u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f55(u32 p0, u64 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 24));
  l0 = i1;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = l4;
  i1 = 4294967272u;
  i0 += i1;
  l3 = i0;
  i0 = 0u;
  i1 = l0;
  i0 -= i1;
  l1 = i0;
  L1: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i0 = i32_load((&memory), (u64)(i0));
    j0 = i64_load((&memory), (u64)(i0));
    j1 = p1;
    i0 = j0 == j1;
    if (i0) {goto B0;}
    i0 = l3;
    l4 = i0;
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    l3 = i0;
    i0 = l2;
    i1 = l1;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L1;}
  B0:;
  i0 = l4;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = l4;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 20));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 80u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B2;
  B3:;
  i0 = 0u;
  l3 = i0;
  i0 = p0;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 12531646810004914176ull;
  j3 = p1;
  i4 = g0;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g0;
    i4 += i5;
    g0 = i4;
  }
  i0 = (*Z_envZ_db_find_i64Z_ijjjj)(j0, j1, j2, j3);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B2;}
  i0 = p0;
  i1 = l2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f40(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 20));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 80u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B2:;
  i0 = l3;
  i1 = 0u;
  i0 = i0 != i1;
  i1 = p2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  FUNC_EPILOGUE;
  return i0;
}

static void f56(u32 p0, u32 p1) {
  u32 l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 20));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 688u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  j1 = (*Z_envZ_current_receiverZ_jv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = j0 == j1;
  i1 = 736u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 24));
  l1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = 0u;
  i1 = l1;
  i0 -= i1;
  l4 = i0;
  i0 = l5;
  i1 = 4294967272u;
  i0 += i1;
  l6 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i0 = i32_load((&memory), (u64)(i0));
    j0 = i64_load((&memory), (u64)(i0));
    j1 = l0;
    i0 = j0 == j1;
    if (i0) {goto B3;}
    i0 = l6;
    l5 = i0;
    i0 = l6;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    l6 = i0;
    i0 = l2;
    i1 = l4;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L4;}
  B3:;
  i0 = l5;
  i1 = l1;
  i0 = i0 != i1;
  i1 = 800u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = 4294967272u;
  i0 += i1;
  l6 = i0;
  i0 = l5;
  i1 = l3;
  i1 = i32_load((&memory), (u64)(i1));
  l2 = i1;
  i0 = i0 == i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = l2;
  i0 -= i1;
  l1 = i0;
  i0 = l6;
  l5 = i0;
  L8: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = 24u;
    i0 += i1;
    l6 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l4 = i0;
    i0 = l6;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l5;
    i0 = i32_load((&memory), (u64)(i0));
    l2 = i0;
    i0 = l5;
    i1 = l4;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l2;
    i0 = !(i0);
    if (i0) {goto B9;}
    i0 = l2;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l4 = i0;
    i0 = !(i0);
    if (i0) {goto B10;}
    i0 = l2;
    i1 = 12u;
    i0 += i1;
    i1 = l4;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l4;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B10:;
    i0 = l2;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B9:;
    i0 = l5;
    i1 = 16u;
    i0 += i1;
    i1 = l5;
    i2 = 40u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    i32_store((&memory), (u64)(i0), i1);
    i0 = l5;
    i1 = 8u;
    i0 += i1;
    i1 = l5;
    i2 = 32u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = l6;
    l5 = i0;
    i0 = l6;
    i1 = l1;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L8;}
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  B7:;
  L13: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l2 = i0;
    i0 = l5;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l2;
    i0 = !(i0);
    if (i0) {goto B14;}
    i0 = l2;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l4 = i0;
    i0 = !(i0);
    if (i0) {goto B15;}
    i0 = l2;
    i1 = 12u;
    i0 += i1;
    i1 = l4;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l4;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B15:;
    i0 = l2;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B14:;
    i0 = l6;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L13;}
  B6:;
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i1 = l6;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  (*Z_envZ_db_remove_i64Z_vi)(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static void f57(u32 p0, u32 p1) {
  u32 l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 32));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 688u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  j1 = (*Z_envZ_current_receiverZ_jv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = j0 == j1;
  i1 = 736u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  l2 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 24));
  l1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = 0u;
  i1 = l1;
  i0 -= i1;
  l3 = i0;
  i0 = l5;
  i1 = 4294967272u;
  i0 += i1;
  l4 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0));
    j0 = i64_load((&memory), (u64)(i0));
    j1 = l0;
    i0 = j0 == j1;
    if (i0) {goto B3;}
    i0 = l4;
    l5 = i0;
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l6 = i0;
    l4 = i0;
    i0 = l6;
    i1 = l3;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L4;}
  B3:;
  i0 = l5;
  i1 = l1;
  i0 = i0 != i1;
  i1 = 800u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = 4294967272u;
  i0 += i1;
  l6 = i0;
  i0 = l5;
  i1 = l2;
  i1 = i32_load((&memory), (u64)(i1));
  l4 = i1;
  i0 = i0 == i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = l4;
  i0 -= i1;
  l1 = i0;
  i0 = l6;
  l4 = i0;
  L8: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 24u;
    i0 += i1;
    l6 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l3 = i0;
    i0 = l6;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0));
    l5 = i0;
    i0 = l4;
    i1 = l3;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l5;
    i0 = !(i0);
    if (i0) {goto B9;}
    i0 = l5;
    i0 = i32_load((&memory), (u64)(i0 + 20));
    l3 = i0;
    i0 = !(i0);
    if (i0) {goto B10;}
    i0 = l5;
    i1 = 24u;
    i0 += i1;
    i1 = l3;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l3;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B10:;
    i0 = l5;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l3 = i0;
    i0 = !(i0);
    if (i0) {goto B12;}
    i0 = l5;
    i1 = 12u;
    i0 += i1;
    i1 = l3;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l3;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B12:;
    i0 = l5;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B9:;
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    i1 = l4;
    i2 = 40u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    i32_store((&memory), (u64)(i0), i1);
    i0 = l4;
    i1 = 8u;
    i0 += i1;
    i1 = l4;
    i2 = 32u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = l6;
    l4 = i0;
    i0 = l6;
    i1 = l1;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L8;}
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  B7:;
  L15: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l4 = i0;
    i0 = l5;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l4;
    i0 = !(i0);
    if (i0) {goto B16;}
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0 + 20));
    l3 = i0;
    i0 = !(i0);
    if (i0) {goto B17;}
    i0 = l4;
    i1 = 24u;
    i0 += i1;
    i1 = l3;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l3;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B17:;
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l3 = i0;
    i0 = !(i0);
    if (i0) {goto B19;}
    i0 = l4;
    i1 = 12u;
    i0 += i1;
    i1 = l3;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l3;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B19:;
    i0 = l4;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B16:;
    i0 = l6;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L15;}
  B6:;
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i1 = l6;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 36));
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  (*Z_envZ_db_remove_i64Z_vi)(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static void _ZN5eosio8multisig4execEyNS_4nameEy(u32 p0, u64 p1, u64 p2, u64 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0, l7 = 0;
  u64 l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 160u;
  i1 -= i2;
  l7 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  j0 = p3;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  (*Z_envZ_require_authZ_vj)(j0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  l4 = i0;
  i0 = l7;
  i1 = 120u;
  i0 += i1;
  i1 = 32u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l7;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 128), j1);
  i0 = l7;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 136), j1);
  i0 = l7;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 144), j1);
  i0 = l7;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 120), j1);
  i0 = l7;
  i1 = 120u;
  i0 += i1;
  j1 = p2;
  i2 = 352u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = f55(i0, j1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  i0 = l7;
  i1 = 80u;
  i0 += i1;
  i1 = 32u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l7;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 88), j1);
  i0 = l7;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 96), j1);
  i0 = l7;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 104), j1);
  i0 = l7;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = l7;
  i1 = 80u;
  i0 += i1;
  j1 = p2;
  i2 = 352u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = f48(i0, j1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  i0 = g0;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g0;
    i0 += i1;
    g0 = i0;
  }
  j0 = (*Z_envZ_current_timeZ_jv)();
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = j0;
  i0 = l7;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 68), i1);
  i0 = l7;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0 + 72), i1);
  i0 = l7;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 76), i1);
  i0 = l7;
  j1 = l5;
  j2 = 1000000ull;
  j1 = DIV_U(j1, j2);
  i1 = (u32)(j1);
  i2 = 60u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 56), i1);
  i0 = l7;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 8));
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l7;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 44), i1);
  i0 = l7;
  i1 = l6;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l7;
  i1 = 40u;
  i0 += i1;
  i1 = l7;
  i2 = 56u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f39(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g0;
    i0 += i1;
    g0 = i0;
  }
  j0 = (*Z_envZ_current_timeZ_jv)();
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = j0;
  i0 = l7;
  i0 = i32_load((&memory), (u64)(i0 + 56));
  j1 = l5;
  j2 = 1000000ull;
  j1 = DIV_U(j1, j2);
  i1 = (u32)(j1);
  i0 = i0 >= i1;
  i1 = 48u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l7;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l7;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  l1 = i1;
  i0 -= i1;
  l3 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  i0 = p0;
  i1 = 20u;
  i0 += i1;
  l0 = i0;
  L7: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967295u;
    i0 += i1;
    l4 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L7;}
  i0 = l1;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B11;}
  i0 = l3;
  i1 = 4294967280u;
  i0 &= i1;
  l2 = i0;
  i1 = l4;
  i0 = i0 != i1;
  if (i0) {goto B10;}
  i0 = 0u;
  l2 = i0;
  i0 = 0u;
  l4 = i0;
  goto B8;
  B11:;
  i0 = 0u;
  i1 = l4;
  i0 -= i1;
  l4 = i0;
  goto B9;
  B10:;
  i0 = l2;
  i1 = l4;
  i0 -= i1;
  l4 = i0;
  B9:;
  i0 = l7;
  i1 = 24u;
  i0 += i1;
  i1 = l4;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f27(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l7;
  i0 = i32_load((&memory), (u64)(i0 + 28));
  l2 = i0;
  i0 = l7;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  l4 = i0;
  B8:;
  i0 = l7;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l7;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l7;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l7;
  i1 = l0;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f35(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = 8u;
  i0 += i1;
  l4 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = l6;
  i2 = 12u;
  i1 += i2;
  l1 = i1;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = l2;
  i1 -= i2;
  i2 = 0u;
  i3 = 0u;
  i4 = l7;
  i4 = i32_load((&memory), (u64)(i4 + 24));
  l2 = i4;
  i5 = l7;
  i5 = i32_load((&memory), (u64)(i5 + 28));
  i6 = l2;
  i5 -= i6;
  i6 = g0;
  i6 = !(i6);
  if (i6) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i6 = 4294967295u;
    i7 = g0;
    i6 += i7;
    g0 = i6;
  }
  i0 = (*Z_envZ_check_transaction_authorizationZ_iiiiiii)(i0, i1, i2, i3, i4, i5);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = 0u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 192u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l7;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l7;
  j1 = p2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l7;
  j1 = p3;
  i2 = l4;
  i2 = i32_load((&memory), (u64)(i2));
  l4 = i2;
  i3 = l1;
  i3 = i32_load((&memory), (u64)(i3));
  i4 = l4;
  i3 -= i4;
  i4 = 0u;
  i5 = g0;
  i5 = !(i5);
  if (i5) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i5 = 4294967295u;
    i6 = g0;
    i5 += i6;
    g0 = i5;
  }
  (*Z_envZ_send_deferredZ_vijiii)(i0, j1, i2, i3, i4);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l7;
  i1 = 120u;
  i0 += i1;
  i1 = l6;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f56(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l7;
  i1 = 80u;
  i0 += i1;
  i1 = p0;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f57(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l7;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B19;}
  i0 = l7;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 28), i1);
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B19:;
  i0 = l7;
  i0 = i32_load((&memory), (u64)(i0 + 104));
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B21;}
  i0 = l7;
  i1 = 108u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B23;}
  L24: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i1 = 4294967272u;
    i0 += i1;
    l6 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l4 = i0;
    i0 = l6;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l4;
    i0 = !(i0);
    if (i0) {goto B25;}
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0 + 20));
    p0 = i0;
    i0 = !(i0);
    if (i0) {goto B26;}
    i0 = l4;
    i1 = 24u;
    i0 += i1;
    i1 = p0;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p0;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B26:;
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    p0 = i0;
    i0 = !(i0);
    if (i0) {goto B28;}
    i0 = l4;
    i1 = 12u;
    i0 += i1;
    i1 = p0;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p0;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B28:;
    i0 = l4;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B25:;
    i0 = l2;
    i1 = l6;
    i0 = i0 != i1;
    if (i0) {goto L24;}
  i0 = l7;
  i1 = 104u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B22;
  B23:;
  i0 = l2;
  l4 = i0;
  B22:;
  i0 = l1;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B21:;
  i0 = l7;
  i0 = i32_load((&memory), (u64)(i0 + 144));
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B32;}
  i0 = l7;
  i1 = 148u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B34;}
  L35: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l4 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l6 = i0;
    i0 = l4;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l6;
    i0 = !(i0);
    if (i0) {goto B36;}
    i0 = l6;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    p0 = i0;
    i0 = !(i0);
    if (i0) {goto B37;}
    i0 = l6;
    i1 = 12u;
    i0 += i1;
    i1 = p0;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p0;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B37:;
    i0 = l6;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    f65(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B36:;
    i0 = l2;
    i1 = l4;
    i0 = i0 != i1;
    if (i0) {goto L35;}
  i0 = l7;
  i1 = 144u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B33;
  B34:;
  i0 = l2;
  l4 = i0;
  B33:;
  i0 = l1;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l4;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  f65(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B32:;
  i0 = 0u;
  i1 = l7;
  i2 = 160u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void apply(u64 p0, u64 p1, u64 p2) {
  u32 l0 = 0, l1 = 0, l6 = 0;
  u64 l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 96u;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  j0 = 0ull;
  l3 = j0;
  j0 = 59ull;
  l2 = j0;
  i0 = 864u;
  l1 = i0;
  j0 = 0ull;
  l4 = j0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    j1 = 6ull;
    i0 = j0 > j1;
    if (i0) {goto B5;}
    i0 = l1;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l0 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B4;}
    i0 = l0;
    i1 = 165u;
    i0 += i1;
    l0 = i0;
    goto B3;
    B5:;
    j0 = 0ull;
    l5 = j0;
    j0 = l3;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B2;}
    goto B1;
    B4:;
    i0 = l0;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l0;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l0 = i0;
    B3:;
    i0 = l0;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l5 = j0;
    B2:;
    j0 = l5;
    j1 = 31ull;
    j0 &= j1;
    j1 = l2;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l5 = j0;
    B1:;
    i0 = l1;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    j0 = l3;
    j1 = 1ull;
    j0 += j1;
    l3 = j0;
    j0 = l5;
    j1 = l4;
    j0 |= j1;
    l4 = j0;
    j0 = l2;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l2 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L0;}
  j0 = l4;
  j1 = p2;
  i0 = j0 != j1;
  if (i0) {goto B6;}
  j0 = 0ull;
  l3 = j0;
  j0 = 59ull;
  l2 = j0;
  i0 = 880u;
  l1 = i0;
  j0 = 0ull;
  l4 = j0;
  L7: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    j1 = 4ull;
    i0 = j0 > j1;
    if (i0) {goto B12;}
    i0 = l1;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l0 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B11;}
    i0 = l0;
    i1 = 165u;
    i0 += i1;
    l0 = i0;
    goto B10;
    B12:;
    j0 = 0ull;
    l5 = j0;
    j0 = l3;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B9;}
    goto B8;
    B11:;
    i0 = l0;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l0;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l0 = i0;
    B10:;
    i0 = l0;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l5 = j0;
    B9:;
    j0 = l5;
    j1 = 31ull;
    j0 &= j1;
    j1 = l2;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l5 = j0;
    B8:;
    i0 = l1;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    j0 = l3;
    j1 = 1ull;
    j0 += j1;
    l3 = j0;
    j0 = l5;
    j1 = l4;
    j0 |= j1;
    l4 = j0;
    j0 = l2;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l2 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L7;}
  j0 = l4;
  j1 = p1;
  i0 = j0 == j1;
  i1 = 896u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B6:;
  j0 = p1;
  j1 = p0;
  i0 = j0 == j1;
  if (i0) {goto B15;}
  j0 = 0ull;
  l3 = j0;
  j0 = 59ull;
  l2 = j0;
  i0 = 864u;
  l1 = i0;
  j0 = 0ull;
  l4 = j0;
  L16: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    j1 = 6ull;
    i0 = j0 > j1;
    if (i0) {goto B21;}
    i0 = l1;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l0 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B20;}
    i0 = l0;
    i1 = 165u;
    i0 += i1;
    l0 = i0;
    goto B19;
    B21:;
    j0 = 0ull;
    l5 = j0;
    j0 = l3;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B18;}
    goto B17;
    B20:;
    i0 = l0;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l0;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l0 = i0;
    B19:;
    i0 = l0;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l5 = j0;
    B18:;
    j0 = l5;
    j1 = 31ull;
    j0 &= j1;
    j1 = l2;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l5 = j0;
    B17:;
    i0 = l1;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    j0 = l3;
    j1 = 1ull;
    j0 += j1;
    l3 = j0;
    j0 = l5;
    j1 = l4;
    j0 |= j1;
    l4 = j0;
    j0 = l2;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l2 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L16;}
  j0 = l4;
  j1 = p2;
  i0 = j0 != j1;
  if (i0) {goto B14;}
  B15:;
  i0 = l6;
  j1 = p0;
  i64_store((&memory), (u64)(i0 + 88), j1);
  j0 = p2;
  j1 = 3849304916161986559ull;
  i0 = (u64)((s64)j0 <= (s64)j1);
  if (i0) {goto B25;}
  j0 = p2;
  j1 = 3849304916161986560ull;
  i0 = j0 == j1;
  if (i0) {goto B24;}
  j0 = p2;
  j1 = 4730614985703555072ull;
  i0 = j0 == j1;
  if (i0) {goto B23;}
  j0 = p2;
  j1 = 6292795316831780864ull;
  i0 = j0 != j1;
  if (i0) {goto B14;}
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 52), i1);
  i0 = l6;
  i1 = 1u;
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l6;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1 + 48));
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = l6;
  i1 = 88u;
  i0 += i1;
  i1 = l6;
  i2 = 40u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f62(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B14;
  B25:;
  j0 = p2;
  j1 = 12531646811867185152ull;
  i0 = j0 == j1;
  if (i0) {goto B22;}
  j0 = p2;
  j1 = 15334012218401357824ull;
  i0 = j0 != j1;
  if (i0) {goto B14;}
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 68), i1);
  i0 = l6;
  i1 = 2u;
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = l6;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1 + 64));
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l6;
  i1 = 88u;
  i0 += i1;
  i1 = l6;
  i2 = 24u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f61(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B14;
  B24:;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 76), i1);
  i0 = l6;
  i1 = 3u;
  i32_store((&memory), (u64)(i0 + 72), i1);
  i0 = l6;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1 + 72));
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l6;
  i1 = 88u;
  i0 += i1;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f61(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B14;
  B23:;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 60), i1);
  i0 = l6;
  i1 = 4u;
  i32_store((&memory), (u64)(i0 + 56), i1);
  i0 = l6;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1 + 56));
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l6;
  i1 = 88u;
  i0 += i1;
  i1 = l6;
  i2 = 32u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f62(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B14;
  B22:;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 84), i1);
  i0 = l6;
  i1 = 5u;
  i32_store((&memory), (u64)(i0 + 80), i1);
  i0 = l6;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1 + 80));
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l6;
  i1 = 88u;
  i0 += i1;
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f60(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
  i0 = 0u;
  i1 = l6;
  i2 = 96u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f60(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l3 = i0;
  l2 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  p1 = i0;
  i0 = g0;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g0;
    i0 += i1;
    g0 = i0;
  }
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = l1;
  i1 = 512u;
  i0 = i0 <= i1;
  if (i0) {goto B2;}
  i0 = l1;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i1 = l1;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B0;
  B2:;
  i0 = 0u;
  i1 = l3;
  i2 = l1;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l3 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l3;
  i1 = l1;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = p0;
  i1 = l0;
  i2 = 1u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  i0 += i1;
  l1 = i0;
  i0 = l0;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B7;}
  i0 = l1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p1;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p1 = i0;
  B7:;
  i0 = l1;
  i1 = p1;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  CALL_INDIRECT(T0, void (*)(u32), 0, i1, i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 1u;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f61(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l5 = 0, l6 = 0, l7 = 0;
  u64 l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 96u;
  i0 -= i1;
  l7 = i0;
  l6 = i0;
  i0 = 0u;
  i1 = l7;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i0 = g0;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g0;
    i0 += i1;
    g0 = i0;
  }
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = l1;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B2;}
  i0 = l1;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  goto B1;
  B3:;
  i0 = 0u;
  p1 = i0;
  goto B0;
  B2:;
  i0 = 0u;
  i1 = l7;
  i2 = l1;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  p1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B1:;
  i0 = p1;
  i1 = l1;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l6;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l6;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l6;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 84), i1);
  i0 = l6;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 80), i1);
  i0 = l6;
  i1 = p1;
  i2 = l1;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 88), i1);
  i0 = l6;
  i1 = l6;
  i2 = 80u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l6;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = l6;
  i1 = 64u;
  i0 += i1;
  i1 = l6;
  i2 = 48u;
  i1 += i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f63(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B8;}
  i0 = p1;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B8:;
  i0 = l6;
  i1 = 16u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  l3 = j0;
  i0 = l6;
  i1 = 60u;
  i0 += i1;
  i1 = l6;
  i2 = 44u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = 48u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  p1 = i0;
  i1 = l6;
  i2 = 40u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  j0 = i64_load((&memory), (u64)(i0 + 16));
  l2 = j0;
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 32));
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l6;
  i1 = l6;
  i2 = 36u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 52), i1);
  i0 = l6;
  i1 = 64u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l6;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1 + 48));
  i64_store((&memory), (u64)(i0 + 64), j1);
  i0 = p0;
  i1 = l0;
  i2 = 1u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  i0 += i1;
  p1 = i0;
  i0 = l0;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B10;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l5;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  B10:;
  i0 = l6;
  i1 = 80u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l6;
  i2 = 64u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  l4 = j1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l6;
  i1 = 8u;
  i0 += i1;
  j1 = l4;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l6;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1 + 64));
  l4 = j1;
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = l6;
  j1 = l4;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p1;
  j1 = l2;
  j2 = l3;
  i3 = l6;
  i4 = l5;
  i5 = g0;
  i5 = !(i5);
  if (i5) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i5 = 4294967295u;
    i6 = g0;
    i5 += i6;
    g0 = i5;
  }
  CALL_INDIRECT(T0, void (*)(u32, u64, u64, u32), 1, i4, i0, j1, j2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = l6;
  i2 = 96u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 1u;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f62(u32 p0, u32 p1) {
  u32 l0 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0, l8 = 0;
  u64 l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 32u;
  i0 -= i1;
  l6 = i0;
  l8 = i0;
  i0 = 0u;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l7 = i0;
  i0 = g0;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g0;
    i0 += i1;
    g0 = i0;
  }
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = p1;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B2;}
  i0 = p1;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  goto B1;
  B3:;
  i0 = 0u;
  l6 = i0;
  goto B0;
  B2:;
  i0 = 0u;
  i1 = l6;
  i2 = p1;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B1:;
  i0 = l6;
  i1 = p1;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l8;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l8;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l8;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = p1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l8;
  i1 = 8u;
  i0 += i1;
  i1 = l6;
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 4294967288u;
  i0 &= i1;
  l4 = i0;
  i1 = 8u;
  i0 = i0 != i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l8;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l5 = i0;
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l8;
  i1 = 8u;
  i0 += i1;
  i1 = 16u;
  i0 += i1;
  l4 = i0;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B13;}
  i0 = l6;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  i0 = p0;
  i1 = l0;
  i2 = 1u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  i0 += i1;
  p1 = i0;
  i0 = l4;
  j0 = i64_load((&memory), (u64)(i0));
  l3 = j0;
  i0 = l5;
  j0 = i64_load((&memory), (u64)(i0));
  l2 = j0;
  i0 = l8;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l1 = j0;
  i0 = l0;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B15;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l7;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l7 = i0;
  B15:;
  i0 = p1;
  j1 = l1;
  j2 = l2;
  j3 = l3;
  i4 = l7;
  i5 = g0;
  i5 = !(i5);
  if (i5) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i5 = 4294967295u;
    i6 = g0;
    i5 += i6;
    g0 = i5;
  }
  CALL_INDIRECT(T0, void (*)(u32, u64, u64, u64), 2, i4, i0, j1, j2, j3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = l8;
  i2 = 32u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 1u;
  FUNC_EPILOGUE;
  return i0;
}

static void f63(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  p1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l1;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 32u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i3 = g0;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g0;
    i3 += i4;
    g0 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f64_0(u32 p0) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 1u;
  i2 = p0;
  i0 = i2 ? i0 : i1;
  l0 = i0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  if (i0) {goto B0;}
  L2: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = 0u;
    p0 = i0;
    i0 = 0u;
    i0 = i32_load((&memory), (u64)(i0 + 960));
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto B0;}
    i0 = l1;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    CALL_INDIRECT(T0, void (*)(void), 3, i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    i0 = malloc_0(i0);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    p0 = i0;
    i0 = !(i0);
    if (i0) {goto L2;}
  B0:;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f65(u32 p0) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = p0;
  i1 = g0;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g0;
    i1 += i2;
    g0 = i1;
  }
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  FUNC_EPILOGUE;
}

static void f66(u32 p0) {
  FUNC_PROLOGUE;
  u32 i0, i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g0;
    i0 += i1;
    g0 = i0;
  }
  (*Z_envZ_abortZ_vv)();
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  FUNC_EPILOGUE;
}

static u32 memcmp_0(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  l2 = i0;
  i0 = p2;
  i0 = !(i0);
  if (i0) {goto B0;}
  L2: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i0 = i32_load8_u((&memory), (u64)(i0));
    l0 = i0;
    i1 = p1;
    i1 = i32_load8_u((&memory), (u64)(i1));
    l1 = i1;
    i0 = i0 != i1;
    if (i0) {goto B1;}
    i0 = p1;
    i1 = 1u;
    i0 += i1;
    p1 = i0;
    i0 = p0;
    i1 = 1u;
    i0 += i1;
    p0 = i0;
    i0 = p2;
    i1 = 4294967295u;
    i0 += i1;
    p2 = i0;
    if (i0) {goto L2;}
    goto B0;
  B1:;
  i0 = l0;
  i1 = l1;
  i0 -= i1;
  l2 = i0;
  B0:;
  i0 = l2;
  FUNC_EPILOGUE;
  return i0;
}

static u32 malloc_0(u32 p0) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 964u;
  i1 = p0;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f69(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
  return i0;
}

static u32 f69(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0, 
      l8 = 0, l9 = 0, l10 = 0, l11 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8384));
  l11 = i0;
  if (i0) {goto B1;}
  i0 = 16u;
  l11 = i0;
  i0 = p0;
  i1 = 8384u;
  i0 += i1;
  i1 = 16u;
  i32_store((&memory), (u64)(i0), i1);
  B1:;
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  i2 = 7u;
  i1 &= i2;
  l0 = i1;
  i0 -= i1;
  i1 = p1;
  i2 = l0;
  i0 = i2 ? i0 : i1;
  l0 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8388));
  l8 = i0;
  i1 = l11;
  i0 = i0 >= i1;
  if (i0) {goto B4;}
  i0 = p0;
  i1 = l8;
  i2 = 12u;
  i1 *= i2;
  i0 += i1;
  i1 = 8192u;
  i0 += i1;
  p1 = i0;
  i0 = l8;
  if (i0) {goto B5;}
  i0 = p0;
  i1 = 8196u;
  i0 += i1;
  l11 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  if (i0) {goto B5;}
  i0 = p1;
  i1 = 8192u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l11;
  i1 = p0;
  i32_store((&memory), (u64)(i0), i1);
  B5:;
  i0 = l0;
  i1 = 4u;
  i0 += i1;
  l8 = i0;
  L6: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p1;
    i0 = i32_load((&memory), (u64)(i0 + 8));
    l11 = i0;
    i1 = l8;
    i0 += i1;
    i1 = p1;
    i1 = i32_load((&memory), (u64)(i1));
    i0 = i0 > i1;
    if (i0) {goto B7;}
    i0 = p1;
    i0 = i32_load((&memory), (u64)(i0 + 4));
    i1 = l11;
    i0 += i1;
    l11 = i0;
    i1 = l11;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 2147483648u;
    i1 &= i2;
    i2 = l0;
    i1 |= i2;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = 8u;
    i0 += i1;
    p1 = i0;
    i1 = p1;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = l8;
    i1 += i2;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l11;
    i1 = l11;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 2147483648u;
    i1 |= i2;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l11;
    i1 = 4u;
    i0 += i1;
    p1 = i0;
    if (i0) {goto B3;}
    B7:;
    i0 = p0;
    i1 = g0;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g0;
      i1 += i2;
      g0 = i1;
    }
    i0 = f70(i0);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    p1 = i0;
    if (i0) {goto L6;}
  B4:;
  i0 = 2147483644u;
  i1 = l0;
  i0 -= i1;
  l2 = i0;
  i0 = p0;
  i1 = 8392u;
  i0 += i1;
  l9 = i0;
  i0 = p0;
  i1 = 8384u;
  i0 += i1;
  l10 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8392));
  l1 = i0;
  l11 = i0;
  L9: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = l11;
    i2 = 12u;
    i1 *= i2;
    i0 += i1;
    p1 = i0;
    i1 = 8200u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = p1;
    i2 = 8192u;
    i1 += i2;
    l3 = i1;
    i1 = i32_load((&memory), (u64)(i1));
    i0 = i0 == i1;
    i1 = 9360u;
    i2 = g0;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g0;
      i2 += i3;
      g0 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p1;
    i1 = 8196u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    l4 = i0;
    i1 = 4u;
    i0 += i1;
    l11 = i0;
    L11: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l4;
      i1 = l3;
      i1 = i32_load((&memory), (u64)(i1));
      i0 += i1;
      l5 = i0;
      i0 = l11;
      i1 = 4294967292u;
      i0 += i1;
      l6 = i0;
      i0 = i32_load((&memory), (u64)(i0));
      l7 = i0;
      i1 = 2147483647u;
      i0 &= i1;
      p1 = i0;
      i0 = l7;
      i1 = 0u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto B12;}
      i0 = p1;
      i1 = l0;
      i0 = i0 >= i1;
      if (i0) {goto B13;}
      L14: 
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        i0 = l11;
        i1 = p1;
        i0 += i1;
        l8 = i0;
        i1 = l5;
        i0 = i0 >= i1;
        if (i0) {goto B13;}
        i0 = l8;
        i0 = i32_load((&memory), (u64)(i0));
        l8 = i0;
        i1 = 0u;
        i0 = (u32)((s32)i0 < (s32)i1);
        if (i0) {goto B13;}
        i0 = p1;
        i1 = l8;
        i2 = 2147483647u;
        i1 &= i2;
        i0 += i1;
        i1 = 4u;
        i0 += i1;
        p1 = i0;
        i1 = l0;
        i0 = i0 < i1;
        if (i0) {goto L14;}
      B13:;
      i0 = l6;
      i1 = p1;
      i2 = l0;
      i3 = p1;
      i4 = l0;
      i3 = i3 < i4;
      i1 = i3 ? i1 : i2;
      i2 = l7;
      i3 = 2147483648u;
      i2 &= i3;
      i1 |= i2;
      i32_store((&memory), (u64)(i0), i1);
      i0 = p1;
      i1 = l0;
      i0 = i0 <= i1;
      if (i0) {goto B15;}
      i0 = l11;
      i1 = l0;
      i0 += i1;
      i1 = l2;
      i2 = p1;
      i1 += i2;
      i2 = 2147483647u;
      i1 &= i2;
      i32_store((&memory), (u64)(i0), i1);
      B15:;
      i0 = p1;
      i1 = l0;
      i0 = i0 >= i1;
      if (i0) {goto B2;}
      B12:;
      i0 = l11;
      i1 = p1;
      i0 += i1;
      i1 = 4u;
      i0 += i1;
      l11 = i0;
      i1 = l5;
      i0 = i0 < i1;
      if (i0) {goto L11;}
    i0 = 0u;
    p1 = i0;
    i0 = l9;
    i1 = 0u;
    i2 = l9;
    i2 = i32_load((&memory), (u64)(i2));
    i3 = 1u;
    i2 += i3;
    l11 = i2;
    i3 = l11;
    i4 = l10;
    i4 = i32_load((&memory), (u64)(i4));
    i3 = i3 == i4;
    i1 = i3 ? i1 : i2;
    l11 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l11;
    i1 = l1;
    i0 = i0 != i1;
    if (i0) {goto L9;}
  B3:;
  i0 = p1;
  goto Bfunc;
  B2:;
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 2147483648u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l11;
  goto Bfunc;
  B0:;
  i0 = 0u;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f70(u32 p0) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8388));
  l0 = i0;
  i0 = 0u;
  i0 = i32_load8_u((&memory), (u64)(i0 + 9446));
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 9448));
  l6 = i0;
  goto B0;
  B1:;
  i0 = memory.pages;
  l6 = i0;
  i0 = 0u;
  i1 = 1u;
  i32_store8((&memory), (u64)(i0 + 9446), i1);
  i0 = 0u;
  i1 = l6;
  i2 = 16u;
  i1 <<= (i2 & 31);
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 9448), i1);
  B0:;
  i0 = l6;
  l2 = i0;
  i0 = l6;
  i1 = 65535u;
  i0 += i1;
  i1 = 16u;
  i0 >>= (i1 & 31);
  l1 = i0;
  i1 = memory.pages;
  l7 = i1;
  i0 = i0 <= i1;
  if (i0) {goto B5;}
  i0 = l1;
  i1 = l7;
  i0 -= i1;
  i0 = wasm_rt_grow_memory((&memory), i0);
  i0 = 0u;
  l7 = i0;
  i0 = l1;
  i1 = memory.pages;
  i0 = i0 != i1;
  if (i0) {goto B4;}
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 9448));
  l2 = i0;
  B5:;
  i0 = 0u;
  l7 = i0;
  i0 = 0u;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 9448), i1);
  i0 = l6;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B4;}
  i0 = p0;
  i1 = l0;
  i2 = 12u;
  i1 *= i2;
  i0 += i1;
  l1 = i0;
  i0 = l6;
  i1 = 65536u;
  i2 = 131072u;
  i3 = l6;
  i4 = 65535u;
  i3 &= i4;
  l7 = i3;
  i4 = 64513u;
  i3 = i3 < i4;
  l5 = i3;
  i1 = i3 ? i1 : i2;
  i0 += i1;
  i1 = l7;
  i2 = l6;
  i3 = 131071u;
  i2 &= i3;
  i3 = l5;
  i1 = i3 ? i1 : i2;
  i0 -= i1;
  i1 = l6;
  i0 -= i1;
  l6 = i0;
  i0 = 0u;
  i0 = i32_load8_u((&memory), (u64)(i0 + 9446));
  if (i0) {goto B6;}
  i0 = memory.pages;
  l2 = i0;
  i0 = 0u;
  i1 = 1u;
  i32_store8((&memory), (u64)(i0 + 9446), i1);
  i0 = 0u;
  i1 = l2;
  i2 = 16u;
  i1 <<= (i2 & 31);
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 9448), i1);
  B6:;
  i0 = l1;
  i1 = 8192u;
  i0 += i1;
  l1 = i0;
  i0 = l6;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B3;}
  i0 = l2;
  l5 = i0;
  i0 = l6;
  i1 = 7u;
  i0 += i1;
  i1 = 4294967288u;
  i0 &= i1;
  l4 = i0;
  i1 = l2;
  i0 += i1;
  i1 = 65535u;
  i0 += i1;
  i1 = 16u;
  i0 >>= (i1 & 31);
  l7 = i0;
  i1 = memory.pages;
  l3 = i1;
  i0 = i0 <= i1;
  if (i0) {goto B7;}
  i0 = l7;
  i1 = l3;
  i0 -= i1;
  i0 = wasm_rt_grow_memory((&memory), i0);
  i0 = l7;
  i1 = memory.pages;
  i0 = i0 != i1;
  if (i0) {goto B3;}
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 9448));
  l5 = i0;
  B7:;
  i0 = 0u;
  i1 = l5;
  i2 = l4;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 9448), i1);
  i0 = l2;
  i1 = 4294967295u;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = p0;
  i1 = l0;
  i2 = 12u;
  i1 *= i2;
  i0 += i1;
  l0 = i0;
  i1 = 8196u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1));
  l7 = i1;
  i0 += i1;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = l7;
  i1 = l0;
  i2 = 8200u;
  i1 += i2;
  l4 = i1;
  i1 = i32_load((&memory), (u64)(i1));
  l0 = i1;
  i0 = i0 == i1;
  if (i0) {goto B8;}
  i0 = l5;
  i1 = l0;
  i0 += i1;
  l5 = i0;
  i1 = l5;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 2147483648u;
  i1 &= i2;
  i2 = 4294967292u;
  i3 = l0;
  i2 -= i3;
  i3 = l7;
  i2 += i3;
  i1 |= i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l4;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = l5;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 2147483647u;
  i1 &= i2;
  i32_store((&memory), (u64)(i0), i1);
  B8:;
  i0 = p0;
  i1 = 8388u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 1u;
  i1 += i2;
  l1 = i1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = l1;
  i2 = 12u;
  i1 *= i2;
  i0 += i1;
  p0 = i0;
  i1 = 8196u;
  i0 += i1;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 8192u;
  i0 += i1;
  l7 = i0;
  i1 = l6;
  i32_store((&memory), (u64)(i0), i1);
  B4:;
  i0 = l7;
  goto Bfunc;
  B3:;
  i0 = l1;
  i0 = i32_load((&memory), (u64)(i0));
  l7 = i0;
  i1 = p0;
  i2 = l0;
  i3 = 12u;
  i2 *= i3;
  i1 += i2;
  l2 = i1;
  i2 = 8200u;
  i1 += i2;
  l0 = i1;
  i1 = i32_load((&memory), (u64)(i1));
  l6 = i1;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  i0 = l2;
  i1 = 8196u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l6;
  i0 += i1;
  l2 = i0;
  i1 = l2;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 2147483648u;
  i1 &= i2;
  i2 = 4294967292u;
  i3 = l6;
  i2 -= i3;
  i3 = l7;
  i2 += i3;
  i1 |= i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l2;
  i1 = l2;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 2147483647u;
  i1 &= i2;
  i32_store((&memory), (u64)(i0), i1);
  B9:;
  i0 = p0;
  i1 = p0;
  i2 = 8388u;
  i1 += i2;
  l6 = i1;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 1u;
  i1 += i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 8384), i1);
  i0 = l6;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = 0u;
  goto Bfunc;
  B2:;
  i0 = l1;
  i1 = l7;
  i2 = l6;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l1;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static void free_0(u32 p0) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 9348));
  l1 = i0;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B1;}
  i0 = 9156u;
  l2 = i0;
  i0 = l1;
  i1 = 12u;
  i0 *= i1;
  i1 = 9156u;
  i0 += i1;
  l0 = i0;
  L2: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 4u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto B1;}
    i0 = l1;
    i1 = 4u;
    i0 += i1;
    i1 = p0;
    i0 = i0 > i1;
    if (i0) {goto B3;}
    i0 = l1;
    i1 = l2;
    i1 = i32_load((&memory), (u64)(i1));
    i0 += i1;
    i1 = p0;
    i0 = i0 > i1;
    if (i0) {goto B0;}
    B3:;
    i0 = l2;
    i1 = 12u;
    i0 += i1;
    l2 = i0;
    i1 = l0;
    i0 = i0 < i1;
    if (i0) {goto L2;}
  B1:;
  goto Bfunc;
  B0:;
  i0 = p0;
  i1 = 4294967292u;
  i0 += i1;
  l2 = i0;
  i1 = l2;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 2147483647u;
  i1 &= i2;
  i32_store((&memory), (u64)(i0), i1);
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f72(void) {
  FUNC_PROLOGUE;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  FUNC_EPILOGUE;
}

static const u8 data_segment_data_0[] = {
  0xf0, 0x4c, 0x00, 0x00, 
};

static const u8 data_segment_data_1[] = {
  0x77, 0x72, 0x69, 0x74, 0x65, 0x00, 
};

static const u8 data_segment_data_2[] = {
  0x72, 0x65, 0x61, 0x64, 0x00, 
};

static const u8 data_segment_data_3[] = {
  0x74, 0x72, 0x61, 0x6e, 0x73, 0x61, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 
  0x65, 0x78, 0x70, 0x69, 0x72, 0x65, 0x64, 0x00, 
};

static const u8 data_segment_data_4[] = {
  0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x20, 0x70, 0x61, 0x73, 0x73, 0x65, 
  0x64, 0x20, 0x74, 0x6f, 0x20, 0x69, 0x74, 0x65, 0x72, 0x61, 0x74, 0x6f, 
  0x72, 0x5f, 0x74, 0x6f, 0x20, 0x69, 0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 
  0x69, 0x6e, 0x20, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x5f, 0x69, 0x6e, 0x64, 
  0x65, 0x78, 0x00, 
};

static const u8 data_segment_data_5[] = {
  0x70, 0x72, 0x6f, 0x70, 0x6f, 0x73, 0x61, 0x6c, 0x20, 0x77, 0x69, 0x74, 
  0x68, 0x20, 0x74, 0x68, 0x65, 0x20, 0x73, 0x61, 0x6d, 0x65, 0x20, 0x6e, 
  0x61, 0x6d, 0x65, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x73, 0x00, 
};

static const u8 data_segment_data_6[] = {
  0x74, 0x72, 0x61, 0x6e, 0x73, 0x61, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 
  0x61, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69, 0x7a, 0x61, 0x74, 0x69, 0x6f, 
  0x6e, 0x20, 0x66, 0x61, 0x69, 0x6c, 0x65, 0x64, 0x00, 
};

static const u8 data_segment_data_7[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x63, 0x72, 0x65, 0x61, 0x74, 
  0x65, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x73, 0x20, 0x69, 0x6e, 
  0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x6f, 0x66, 0x20, 0x61, 0x6e, 
  0x6f, 0x74, 0x68, 0x65, 0x72, 0x20, 0x63, 0x6f, 0x6e, 0x74, 0x72, 0x61, 
  0x63, 0x74, 0x00, 
};

static const u8 data_segment_data_8[] = {
  0x65, 0x72, 0x72, 0x6f, 0x72, 0x20, 0x72, 0x65, 0x61, 0x64, 0x69, 0x6e, 
  0x67, 0x20, 0x69, 0x74, 0x65, 0x72, 0x61, 0x74, 0x6f, 0x72, 0x00, 
};

static const u8 data_segment_data_9[] = {
  0x67, 0x65, 0x74, 0x00, 
};

static const u8 data_segment_data_10[] = {
  0x70, 0x72, 0x6f, 0x70, 0x6f, 0x73, 0x61, 0x6c, 0x20, 0x6e, 0x6f, 0x74, 
  0x20, 0x66, 0x6f, 0x75, 0x6e, 0x64, 0x00, 
};

static const u8 data_segment_data_11[] = {
  0x61, 0x70, 0x70, 0x72, 0x6f, 0x76, 0x61, 0x6c, 0x20, 0x69, 0x73, 0x20, 
  0x6e, 0x6f, 0x74, 0x20, 0x6f, 0x6e, 0x20, 0x74, 0x68, 0x65, 0x20, 0x6c, 
  0x69, 0x73, 0x74, 0x20, 0x6f, 0x66, 0x20, 0x72, 0x65, 0x71, 0x75, 0x65, 
  0x73, 0x74, 0x65, 0x64, 0x20, 0x61, 0x70, 0x70, 0x72, 0x6f, 0x76, 0x61, 
  0x6c, 0x73, 0x00, 
};

static const u8 data_segment_data_12[] = {
  0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x20, 0x70, 0x61, 0x73, 0x73, 0x65, 
  0x64, 0x20, 0x74, 0x6f, 0x20, 0x6d, 0x6f, 0x64, 0x69, 0x66, 0x79, 0x20, 
  0x69, 0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x69, 0x6e, 0x20, 0x6d, 0x75, 
  0x6c, 0x74, 0x69, 0x5f, 0x69, 0x6e, 0x64, 0x65, 0x78, 0x00, 
};

static const u8 data_segment_data_13[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x6d, 0x6f, 0x64, 0x69, 0x66, 
  0x79, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x73, 0x20, 0x69, 0x6e, 
  0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x6f, 0x66, 0x20, 0x61, 0x6e, 
  0x6f, 0x74, 0x68, 0x65, 0x72, 0x20, 0x63, 0x6f, 0x6e, 0x74, 0x72, 0x61, 
  0x63, 0x74, 0x00, 
};

static const u8 data_segment_data_14[] = {
  0x75, 0x70, 0x64, 0x61, 0x74, 0x65, 0x72, 0x20, 0x63, 0x61, 0x6e, 0x6e, 
  0x6f, 0x74, 0x20, 0x63, 0x68, 0x61, 0x6e, 0x67, 0x65, 0x20, 0x70, 0x72, 
  0x69, 0x6d, 0x61, 0x72, 0x79, 0x20, 0x6b, 0x65, 0x79, 0x20, 0x77, 0x68, 
  0x65, 0x6e, 0x20, 0x6d, 0x6f, 0x64, 0x69, 0x66, 0x79, 0x69, 0x6e, 0x67, 
  0x20, 0x61, 0x6e, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x00, 
};

static const u8 data_segment_data_15[] = {
  0x6e, 0x6f, 0x20, 0x61, 0x70, 0x70, 0x72, 0x6f, 0x76, 0x61, 0x6c, 0x20, 
  0x70, 0x72, 0x65, 0x76, 0x69, 0x6f, 0x75, 0x73, 0x6c, 0x79, 0x20, 0x67, 
  0x72, 0x61, 0x6e, 0x74, 0x65, 0x64, 0x00, 
};

static const u8 data_segment_data_16[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x63, 0x61, 0x6e, 0x63, 0x65, 
  0x6c, 0x20, 0x75, 0x6e, 0x74, 0x69, 0x6c, 0x20, 0x65, 0x78, 0x70, 0x69, 
  0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x00, 
};

static const u8 data_segment_data_17[] = {
  0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x20, 0x70, 0x61, 0x73, 0x73, 0x65, 
  0x64, 0x20, 0x74, 0x6f, 0x20, 0x65, 0x72, 0x61, 0x73, 0x65, 0x20, 0x69, 
  0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x69, 0x6e, 0x20, 0x6d, 0x75, 0x6c, 
  0x74, 0x69, 0x5f, 0x69, 0x6e, 0x64, 0x65, 0x78, 0x00, 
};

static const u8 data_segment_data_18[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x72, 0x61, 0x73, 0x65, 
  0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x73, 0x20, 0x69, 0x6e, 0x20, 
  0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x6f, 0x66, 0x20, 0x61, 0x6e, 0x6f, 
  0x74, 0x68, 0x65, 0x72, 0x20, 0x63, 0x6f, 0x6e, 0x74, 0x72, 0x61, 0x63, 
  0x74, 0x00, 
};

static const u8 data_segment_data_19[] = {
  0x61, 0x74, 0x74, 0x65, 0x6d, 0x70, 0x74, 0x20, 0x74, 0x6f, 0x20, 0x72, 
  0x65, 0x6d, 0x6f, 0x76, 0x65, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 
  0x20, 0x74, 0x68, 0x61, 0x74, 0x20, 0x77, 0x61, 0x73, 0x20, 0x6e, 0x6f, 
  0x74, 0x20, 0x69, 0x6e, 0x20, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x5f, 0x69, 
  0x6e, 0x64, 0x65, 0x78, 0x00, 
};

static const u8 data_segment_data_20[] = {
  0x6f, 0x6e, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x00, 
};

static const u8 data_segment_data_21[] = {
  0x65, 0x6f, 0x73, 0x69, 0x6f, 0x00, 
};

static const u8 data_segment_data_22[] = {
  0x6f, 0x6e, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x20, 0x61, 0x63, 0x74, 0x69, 
  0x6f, 0x6e, 0x27, 0x73, 0x20, 0x61, 0x72, 0x65, 0x20, 0x6f, 0x6e, 0x6c, 
  0x79, 0x20, 0x76, 0x61, 0x6c, 0x69, 0x64, 0x20, 0x66, 0x72, 0x6f, 0x6d, 
  0x20, 0x74, 0x68, 0x65, 0x20, 0x22, 0x65, 0x6f, 0x73, 0x69, 0x6f, 0x22, 
  0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x20, 0x61, 0x63, 0x63, 0x6f, 
  0x75, 0x6e, 0x74, 0x00, 
};

static const u8 data_segment_data_23[] = {
  0x6d, 0x61, 0x6c, 0x6c, 0x6f, 0x63, 0x5f, 0x66, 0x72, 0x6f, 0x6d, 0x5f, 
  0x66, 0x72, 0x65, 0x65, 0x64, 0x20, 0x77, 0x61, 0x73, 0x20, 0x64, 0x65, 
  0x73, 0x69, 0x67, 0x6e, 0x65, 0x64, 0x20, 0x74, 0x6f, 0x20, 0x6f, 0x6e, 
  0x6c, 0x79, 0x20, 0x62, 0x65, 0x20, 0x63, 0x61, 0x6c, 0x6c, 0x65, 0x64, 
  0x20, 0x61, 0x66, 0x74, 0x65, 0x72, 0x20, 0x5f, 0x68, 0x65, 0x61, 0x70, 
  0x20, 0x77, 0x61, 0x73, 0x20, 0x63, 0x6f, 0x6d, 0x70, 0x6c, 0x65, 0x74, 
  0x65, 0x6c, 0x79, 0x20, 0x61, 0x6c, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x65, 
  0x64, 0x00, 
};

static void init_memory(void) {
  wasm_rt_allocate_memory((&memory), 1, 528);
  memcpy(&(memory.data[4u]), data_segment_data_0, 4);
  memcpy(&(memory.data[16u]), data_segment_data_1, 6);
  memcpy(&(memory.data[32u]), data_segment_data_2, 5);
  memcpy(&(memory.data[48u]), data_segment_data_3, 20);
  memcpy(&(memory.data[80u]), data_segment_data_4, 51);
  memcpy(&(memory.data[144u]), data_segment_data_5, 35);
  memcpy(&(memory.data[192u]), data_segment_data_6, 33);
  memcpy(&(memory.data[240u]), data_segment_data_7, 51);
  memcpy(&(memory.data[304u]), data_segment_data_8, 23);
  memcpy(&(memory.data[336u]), data_segment_data_9, 4);
  memcpy(&(memory.data[352u]), data_segment_data_10, 19);
  memcpy(&(memory.data[384u]), data_segment_data_11, 51);
  memcpy(&(memory.data[448u]), data_segment_data_12, 46);
  memcpy(&(memory.data[496u]), data_segment_data_13, 51);
  memcpy(&(memory.data[560u]), data_segment_data_14, 59);
  memcpy(&(memory.data[624u]), data_segment_data_15, 31);
  memcpy(&(memory.data[656u]), data_segment_data_16, 31);
  memcpy(&(memory.data[688u]), data_segment_data_17, 45);
  memcpy(&(memory.data[736u]), data_segment_data_18, 50);
  memcpy(&(memory.data[800u]), data_segment_data_19, 53);
  memcpy(&(memory.data[864u]), data_segment_data_20, 8);
  memcpy(&(memory.data[880u]), data_segment_data_21, 6);
  memcpy(&(memory.data[896u]), data_segment_data_22, 64);
  memcpy(&(memory.data[9360u]), data_segment_data_23, 86);
}

static void init_table(void) {
  uint32_t offset;
  wasm_rt_allocate_table((&T0), 6, 6);
  offset = 0u;
  T0.data[offset + 0] = (wasm_rt_elem_t){func_types[3], (wasm_rt_anyfunc_t)(&f72)};
  T0.data[offset + 1] = (wasm_rt_elem_t){func_types[2], (wasm_rt_anyfunc_t)(&_ZN5eosio8multisig4execEyNS_4nameEy)};
  T0.data[offset + 2] = (wasm_rt_elem_t){func_types[1], (wasm_rt_anyfunc_t)(&_ZN5eosio8multisig9unapproveEyNS_4nameENS_16permission_levelE)};
  T0.data[offset + 3] = (wasm_rt_elem_t){func_types[1], (wasm_rt_anyfunc_t)(&_ZN5eosio8multisig7approveEyNS_4nameENS_16permission_levelE)};
  T0.data[offset + 4] = (wasm_rt_elem_t){func_types[2], (wasm_rt_anyfunc_t)(&_ZN5eosio8multisig6cancelEyNS_4nameEy)};
  T0.data[offset + 5] = (wasm_rt_elem_t){func_types[0], (wasm_rt_anyfunc_t)(&_ZN5eosio8multisig7proposeEv)};
}

/* export: 'memory' */
wasm_rt_memory_t (*WASM_RT_ADD_PREFIX(Z_memory));
/* export: '_ZeqRK11checksum256S1_' */
u32 (*WASM_RT_ADD_PREFIX(Z__Z5AeqRK11checksum256S1_Z_iii))(u32, u32);
/* export: '_ZeqRK11checksum160S1_' */
u32 (*WASM_RT_ADD_PREFIX(Z__Z5AeqRK11checksum160S1_Z_iii))(u32, u32);
/* export: '_ZneRK11checksum160S1_' */
u32 (*WASM_RT_ADD_PREFIX(Z__Z5AneRK11checksum160S1_Z_iii))(u32, u32);
/* export: 'now' */
u32 (*WASM_RT_ADD_PREFIX(Z_nowZ_iv))(void);
/* export: '_ZN5eosio12require_authERKNS_16permission_levelE' */
void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio12require_authERKNS_16permission_levelEZ_vi))(u32);
/* export: '_ZN5eosio31check_transaction_authorizationERKNS_11transactionERKNSt3__13setINS_16permission_levelENS3_4lessIS5_EENS3_9allocatorIS5_EEEERKNS4_I10public_keyNS6_ISD_EENS8_ISD_EEEE' */
u32 (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio31check_transaction_authorizationERKNS_11transactionERKNSt3__13setINS_16permission_levelENS3_4lessIS5_EENS3_9allocatorIS5_EEEERKNS4_I10public_keyNS6_ISD_EENS8_ISD_EEEEZ_iiii))(u32, u32, u32);
/* export: '_ZN5eosio30check_permission_authorizationEyyRKNSt3__13setI10public_keyNS0_4lessIS2_EENS0_9allocatorIS2_EEEERKNS1_INS_16permission_levelENS3_ISA_EENS5_ISA_EEEEy' */
u32 (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio30check_permission_authorizationEyyRKNSt3__13setI10public_keyNS0_4lessIS2_EENS0_9allocatorIS2_EEEERKNS1_INS_16permission_levelENS3_ISA_EENS5_ISA_EEEEyZ_ijjiij))(u64, u64, u32, u32, u64);
/* export: '_ZN5eosio8multisig7proposeEv' */
void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig7proposeEvZ_vi))(u32);
/* export: '_ZN5eosio8multisig7approveEyNS_4nameENS_16permission_levelE' */
void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig7approveEyNS_4nameENS_16permission_levelEZ_vijji))(u32, u64, u64, u32);
/* export: '_ZN5eosio8multisig9unapproveEyNS_4nameENS_16permission_levelE' */
void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig9unapproveEyNS_4nameENS_16permission_levelEZ_vijji))(u32, u64, u64, u32);
/* export: '_ZN5eosio8multisig6cancelEyNS_4nameEy' */
void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig6cancelEyNS_4nameEyZ_vijjj))(u32, u64, u64, u64);
/* export: '_ZN5eosio8multisig4execEyNS_4nameEy' */
void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig4execEyNS_4nameEyZ_vijjj))(u32, u64, u64, u64);
/* export: 'apply' */
void (*WASM_RT_ADD_PREFIX(apply_eosio_msig_5cf017909547b2d69cee5f01c53fe90f3ab193c57108f81a17f0716a4c83f9c0_176))(u64, u64, u64);
/* export: 'memcmp' */
u32 (*WASM_RT_ADD_PREFIX(Z_memcmpZ_iiii))(u32, u32, u32);
/* export: 'malloc' */
u32 (*WASM_RT_ADD_PREFIX(Z_mallocZ_ii))(u32);
/* export: 'free' */
void (*WASM_RT_ADD_PREFIX(Z_freeZ_vi))(u32);

static void init_exports(void) {
  /* export: 'memory' */
  WASM_RT_ADD_PREFIX(Z_memory) = (&memory);
  /* export: '_ZeqRK11checksum256S1_' */
  WASM_RT_ADD_PREFIX(Z__Z5AeqRK11checksum256S1_Z_iii) = (&_ZeqRK11checksum256S1_);
  /* export: '_ZeqRK11checksum160S1_' */
  WASM_RT_ADD_PREFIX(Z__Z5AeqRK11checksum160S1_Z_iii) = (&_ZeqRK11checksum160S1_);
  /* export: '_ZneRK11checksum160S1_' */
  WASM_RT_ADD_PREFIX(Z__Z5AneRK11checksum160S1_Z_iii) = (&_ZneRK11checksum160S1_);
  /* export: 'now' */
  WASM_RT_ADD_PREFIX(Z_nowZ_iv) = (&now);
  /* export: '_ZN5eosio12require_authERKNS_16permission_levelE' */
  WASM_RT_ADD_PREFIX(Z__Z5AN5eosio12require_authERKNS_16permission_levelEZ_vi) = (&_ZN5eosio12require_authERKNS_16permission_levelE);
  /* export: '_ZN5eosio31check_transaction_authorizationERKNS_11transactionERKNSt3__13setINS_16permission_levelENS3_4lessIS5_EENS3_9allocatorIS5_EEEERKNS4_I10public_keyNS6_ISD_EENS8_ISD_EEEE' */
  WASM_RT_ADD_PREFIX(Z__Z5AN5eosio31check_transaction_authorizationERKNS_11transactionERKNSt3__13setINS_16permission_levelENS3_4lessIS5_EENS3_9allocatorIS5_EEEERKNS4_I10public_keyNS6_ISD_EENS8_ISD_EEEEZ_iiii) = (&_ZN5eosio31check_transaction_authorizationERKNS_11transactionERKNSt3__13setINS_16permission_levelENS3_4lessIS5_EENS3_9allocatorIS5_EEEERKNS4_I10public_keyNS6_ISD_EENS8_ISD_EEEE);
  /* export: '_ZN5eosio30check_permission_authorizationEyyRKNSt3__13setI10public_keyNS0_4lessIS2_EENS0_9allocatorIS2_EEEERKNS1_INS_16permission_levelENS3_ISA_EENS5_ISA_EEEEy' */
  WASM_RT_ADD_PREFIX(Z__Z5AN5eosio30check_permission_authorizationEyyRKNSt3__13setI10public_keyNS0_4lessIS2_EENS0_9allocatorIS2_EEEERKNS1_INS_16permission_levelENS3_ISA_EENS5_ISA_EEEEyZ_ijjiij) = (&_ZN5eosio30check_permission_authorizationEyyRKNSt3__13setI10public_keyNS0_4lessIS2_EENS0_9allocatorIS2_EEEERKNS1_INS_16permission_levelENS3_ISA_EENS5_ISA_EEEEy);
  /* export: '_ZN5eosio8multisig7proposeEv' */
  WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig7proposeEvZ_vi) = (&_ZN5eosio8multisig7proposeEv);
  /* export: '_ZN5eosio8multisig7approveEyNS_4nameENS_16permission_levelE' */
  WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig7approveEyNS_4nameENS_16permission_levelEZ_vijji) = (&_ZN5eosio8multisig7approveEyNS_4nameENS_16permission_levelE);
  /* export: '_ZN5eosio8multisig9unapproveEyNS_4nameENS_16permission_levelE' */
  WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig9unapproveEyNS_4nameENS_16permission_levelEZ_vijji) = (&_ZN5eosio8multisig9unapproveEyNS_4nameENS_16permission_levelE);
  /* export: '_ZN5eosio8multisig6cancelEyNS_4nameEy' */
  WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig6cancelEyNS_4nameEyZ_vijjj) = (&_ZN5eosio8multisig6cancelEyNS_4nameEy);
  /* export: '_ZN5eosio8multisig4execEyNS_4nameEy' */
  WASM_RT_ADD_PREFIX(Z__Z5AN5eosio8multisig4execEyNS_4nameEyZ_vijjj) = (&_ZN5eosio8multisig4execEyNS_4nameEy);
  /* export: 'apply' */
  WASM_RT_ADD_PREFIX(apply_eosio_msig_5cf017909547b2d69cee5f01c53fe90f3ab193c57108f81a17f0716a4c83f9c0_176) = (&apply);
  /* export: 'memcmp' */
  WASM_RT_ADD_PREFIX(Z_memcmpZ_iiii) = (&memcmp_0);
  /* export: 'malloc' */
  WASM_RT_ADD_PREFIX(Z_mallocZ_ii) = (&malloc_0);
  /* export: 'free' */
  WASM_RT_ADD_PREFIX(Z_freeZ_vi) = (&free_0);
}

void WASM_RT_ADD_PREFIX(init_eosio_msig_5cf017909547b2d69cee5f01c53fe90f3ab193c57108f81a17f0716a4c83f9c0_176)(void) {
  init_func_types();
  init_globals();
  init_memory();
  init_table();
  init_exports();
}

wasm_rt_memory_t* get_memory_eosio_msig_5cf017909547b2d69cee5f01c53fe90f3ab193c57108f81a17f0716a4c83f9c0_176() {
    return &memory;
}
        