#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "contracts/eosiochaince-3e0cf4172ab025f9fff5f1db11ee8a34d44779492e1d668ae1dc2d129e865348-1488192.h"
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


static u32 func_types[21];

static void init_func_types(void) {
  func_types[0] = wasm_rt_register_func_type(3, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32);
  func_types[1] = wasm_rt_register_func_type(5, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[2] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[3] = wasm_rt_register_func_type(0, 0);
  func_types[4] = wasm_rt_register_func_type(0, 1, WASM_RT_I64);
  func_types[5] = wasm_rt_register_func_type(2, 0, WASM_RT_I64, WASM_RT_I64);
  func_types[6] = wasm_rt_register_func_type(1, 0, WASM_RT_I64);
  func_types[7] = wasm_rt_register_func_type(2, 0, WASM_RT_I32, WASM_RT_I32);
  func_types[8] = wasm_rt_register_func_type(4, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32);
  func_types[9] = wasm_rt_register_func_type(6, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[10] = wasm_rt_register_func_type(3, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[11] = wasm_rt_register_func_type(1, 1, WASM_RT_I64, WASM_RT_I32);
  func_types[12] = wasm_rt_register_func_type(1, 0, WASM_RT_I32);
  func_types[13] = wasm_rt_register_func_type(0, 1, WASM_RT_I32);
  func_types[14] = wasm_rt_register_func_type(2, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[15] = wasm_rt_register_func_type(1, 1, WASM_RT_I32, WASM_RT_I32);
  func_types[16] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[17] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I64);
  func_types[18] = wasm_rt_register_func_type(4, 0, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[19] = wasm_rt_register_func_type(3, 1, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[20] = wasm_rt_register_func_type(3, 0, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
}

static u32 _ZeqRK11checksum256S1_(u32, u32);
static u32 _ZeqRK11checksum160S1_(u32, u32);
static u32 _ZneRK11checksum160S1_(u32, u32);
static u32 now(void);
static void _ZN5eosio12require_authERKNS_16permission_levelE(u32);
static void _ZN5eosio5token6createEyNS_5assetE(u32, u64, u32);
static u32 f25(u32, u32);
static u32 f26(u32);
static void f27(u32, u32);
static void f28(u32, u32, u32, u32);
static void f29(u32, u32);
static void _ZN5eosio5token5issueEyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEE(u32, u64, u32, u32);
static void _ZN5eosio5token11add_balanceEyNS_5assetEy(u32, u64, u32, u64);
static void f32_0(u64, u64, u32, u32);
static void f33(u32, u32);
static void f34(u32, u32);
static void f35(u32, u32);
static u32 f36(u32, u32);
static u32 f37(u32, u32);
static u32 f38(u32, u32);
static u32 f39(u32, u32);
static void f40(u32, u32, u32, u32);
static void _ZN5eosio5token8transferEyyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEE(u32, u64, u64, u32, u32);
static u32 f42(u32, u64, u32);
static void _ZN5eosio5token11sub_balanceEyNS_5assetE(u32, u64, u32);
static u32 f44(u32, u64, u32);
static void f45(u32, u32);
static void apply(u64, u64, u64);
static u32 f47(u32, u32);
static u32 f48(u32, u32);
static u32 f49(u32, u32);
static void f50(u32, u32);
static void f51(u32, u32);
static u32 f52(u32, u32);
static u32 f53(u32, u32);
static void f54(u32, u32);
static u32 f55(u32);
static void f56(u32);
static void f57(u32);
static void f58(u32, u32);
static void f59(u32);
static u32 f60(u32, u32);
static u32 memcmp_0(u32, u32, u32);
static u32 malloc_0(u32);
static u32 f63(u32, u32);
static u32 f64_0(u32);
static void free_0(u32);
static void f66(void);

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

static void _ZN5eosio5token6createEyNS_5assetE(u32 p0, u64 p1, u32 p2) {
  u32 l3 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0, l1 = 0, l2 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4, j5;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 128u;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
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
  (*Z_envZ_require_authZ_vj)(j0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  l5 = i0;
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l0 = j0;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l1 = j0;
  l4 = j0;
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l4;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B2;}
    j0 = l4;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B4;}
    L5: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l4;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l4 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B2;}
      i0 = l5;
      i1 = 1u;
      i0 += i1;
      l5 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L5;}
    B4:;
    i0 = 1u;
    l3 = i0;
    i0 = l5;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L3;}
    goto B1;
  B2:;
  i0 = 0u;
  l3 = i0;
  B1:;
  i0 = l3;
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
  i0 = 0u;
  l3 = i0;
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0));
  l2 = j0;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775806ull;
  i0 = j0 > j1;
  if (i0) {goto B7;}
  i0 = 0u;
  l5 = i0;
  j0 = l1;
  l4 = j0;
  L9: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l4;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B8;}
    j0 = l4;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B10;}
    L11: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l4;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l4 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B8;}
      i0 = l5;
      i1 = 1u;
      i0 += i1;
      l5 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L11;}
    B10:;
    i0 = 1u;
    l3 = i0;
    i0 = l5;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L9;}
    goto B7;
  B8:;
  i0 = 0u;
  l3 = i0;
  B7:;
  i0 = l3;
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
  j0 = l2;
  j1 = 0ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 64u;
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
  i1 = 8u;
  i0 += i1;
  i1 = 32u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l6;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l6;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l4 = j1;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l6;
  j1 = l1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  j0 = l4;
  j1 = l1;
  j2 = 14289235522390851584ull;
  j3 = l1;
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
  l5 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B15;}
  i0 = l6;
  i1 = 8u;
  i0 += i1;
  i1 = l5;
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
  i0 = f25(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = i32_load((&memory), (u64)(i0 + 40));
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  i0 = i0 == i1;
  i1 = 96u;
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
  i0 = 0u;
  l5 = i0;
  goto B14;
  B15:;
  i0 = 1u;
  l5 = i0;
  B14:;
  i0 = l5;
  i1 = 160u;
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
  l1 = j0;
  i0 = l6;
  j0 = i64_load((&memory), (u64)(i0 + 8));
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
  i1 = 208u;
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
  i0 = 56u;
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
  i0 = f55(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
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
  i0 = f26(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l5;
  j1 = l0;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l5;
  i1 = 28u;
  i0 += i1;
  i1 = p2;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = 24u;
  i0 += i1;
  i1 = p2;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = 20u;
  i0 += i1;
  i1 = p2;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l5;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l6;
  i1 = l6;
  i2 = 48u;
  i1 += i2;
  i2 = 40u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l6;
  i1 = l6;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 92), i1);
  i0 = l6;
  i1 = l6;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 88), i1);
  i0 = l6;
  i1 = l6;
  i2 = 88u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 104), i1);
  i0 = l6;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 116), i1);
  i0 = l6;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 112), i1);
  i0 = l6;
  i1 = l5;
  i2 = 32u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 120), i1);
  i0 = l6;
  i1 = 112u;
  i0 += i1;
  i1 = l6;
  i2 = 104u;
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
  f27(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 14289235522390851584ull;
  j3 = l1;
  i4 = l5;
  j4 = i64_load((&memory), (u64)(i4 + 8));
  j5 = 8ull;
  j4 >>= (j5 & 63);
  l4 = j4;
  i5 = l6;
  i6 = 48u;
  i5 += i6;
  i6 = 40u;
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
  l3 = i1;
  i32_store((&memory), (u64)(i0 + 44), i1);
  j0 = l4;
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  i2 = 16u;
  i1 += i2;
  p2 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B26;}
  i0 = p2;
  j1 = l4;
  j2 = 1ull;
  j1 += j2;
  i64_store((&memory), (u64)(i0), j1);
  B26:;
  i0 = l6;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 112), i1);
  i0 = l6;
  i1 = l5;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  l4 = j1;
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = l6;
  i1 = l3;
  i32_store((&memory), (u64)(i0 + 88), i1);
  i0 = l6;
  i1 = 8u;
  i0 += i1;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p2 = i0;
  i1 = l6;
  i2 = 40u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B28;}
  i0 = p2;
  j1 = l4;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p2;
  i1 = l3;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 112), i1);
  i0 = p2;
  i1 = l5;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = 36u;
  i0 += i1;
  i1 = p2;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B27;
  B28:;
  i0 = l6;
  i1 = 32u;
  i0 += i1;
  i1 = l6;
  i2 = 112u;
  i1 += i2;
  i2 = l6;
  i3 = 48u;
  i2 += i3;
  i3 = l6;
  i4 = 88u;
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
  f28(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B27:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 112));
  l5 = i0;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 112), i1);
  i0 = l5;
  i0 = !(i0);
  if (i0) {goto B30;}
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B30:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 32));
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B32;}
  i0 = l6;
  i1 = 36u;
  i0 += i1;
  p0 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B34;}
  L35: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    p2 = i0;
    i0 = l5;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p2;
    i0 = !(i0);
    if (i0) {goto B36;}
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
    f56(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B36:;
    i0 = l3;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L35;}
  i0 = l6;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  goto B33;
  B34:;
  i0 = l3;
  l5 = i0;
  B33:;
  i0 = p0;
  i1 = l3;
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B32:;
  i0 = 0u;
  i1 = l6;
  i2 = 128u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f25(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  u64 l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
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
  i1 = 352u;
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
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l6;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l6;
  i1 = l2;
  i2 = l4;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
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
  B10:;
  i0 = 56u;
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
  i0 = f55(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
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
  i0 = f26(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l6;
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l6;
  i1 = l4;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l6;
  i1 = l4;
  i2 = 32u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l6;
  i1 = 32u;
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
  f29(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 44), i1);
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l6;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  l3 = j1;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l6;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1 + 44));
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
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
  if (i0) {goto B16;}
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
  goto B15;
  B16:;
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  i1 = l6;
  i2 = 24u;
  i1 += i2;
  i2 = l6;
  i3 = 32u;
  i2 += i3;
  i3 = l6;
  i4 = 4u;
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
  f28(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B15:;
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
  f56(i0);
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

static u32 f26(u32 p0) {
  u32 l1 = 0, l2 = 0;
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = 1u;
  i1 = 288u;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l0 = j0;
  i0 = 0u;
  l1 = i0;
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l0;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B2;}
    j0 = l0;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l0 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B4;}
    L5: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l0;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l0 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B2;}
      i0 = l1;
      i1 = 1u;
      i0 += i1;
      l1 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L5;}
    B4:;
    i0 = 1u;
    l2 = i0;
    i0 = l1;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L3;}
    goto B1;
  B2:;
  i0 = 0u;
  l2 = i0;
  B1:;
  i0 = l2;
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
  i1 = 24u;
  i0 += i1;
  l1 = i0;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = 1u;
  i1 = 288u;
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
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l0 = j0;
  i0 = 0u;
  l1 = i0;
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l0;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B9;}
    j0 = l0;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l0 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B11;}
    L12: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l0;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l0 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B9;}
      i0 = l1;
      i1 = 1u;
      i0 += i1;
      l1 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L12;}
    B11:;
    i0 = 1u;
    l2 = i0;
    i0 = l1;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L10;}
    goto B8;
  B9:;
  i0 = 0u;
  l2 = i0;
  B8:;
  i0 = l2;
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
  FUNC_EPILOGUE;
  return i0;
}

static void f27(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 272u;
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
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = l1;
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
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l2;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 272u;
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
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = l1;
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
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 272u;
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
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = l1;
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
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l2;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 272u;
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
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = l1;
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
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  p0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 272u;
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
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p0;
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
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f28(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f55(i0);
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
  f59(i0);
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
  l2 = i0;
  goto B6;
  B7:;
  i0 = l3;
  l2 = i0;
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
  i1 = l2;
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
    f56(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B11:;
    i0 = l2;
    i1 = l3;
    i0 = i0 != i1;
    if (i0) {goto L10;}
  B9:;
  i0 = l2;
  i0 = !(i0);
  if (i0) {goto B13;}
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static void f29(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
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
  i0 = l1;
  i1 = l0;
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
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l2;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
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
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l0;
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
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
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
  i0 = l1;
  i1 = l0;
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
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l2;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
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
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l0;
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
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  p0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
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
  i0 = p0;
  i1 = l0;
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
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void _ZN5eosio5token5issueEyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEE(u32 p0, u64 p1, u32 p2, u32 p3) {
  u32 l0 = 0, l3 = 0, l5 = 0, l6 = 0, l10 = 0;
  u64 l1 = 0, l2 = 0, l4 = 0, l7 = 0, l8 = 0, l9 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 224u;
  i1 -= i2;
  l10 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 0u;
  l5 = i0;
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l7 = j0;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l9 = j0;
  l4 = j0;
  L2: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l4;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B1;}
    j0 = l4;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B3;}
    L4: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l4;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l4 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B1;}
      i0 = l5;
      i1 = 1u;
      i0 += i1;
      l5 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L4;}
    B3:;
    i0 = 1u;
    l3 = i0;
    i0 = l5;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L2;}
    goto B0;
  B1:;
  i0 = 0u;
  l3 = i0;
  B0:;
  i0 = l3;
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
  i0 = p3;
  i0 = i32_load8_u((&memory), (u64)(i0));
  l5 = i0;
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B7;}
  i0 = l5;
  i1 = 1u;
  i0 >>= (i1 & 31);
  l5 = i0;
  goto B6;
  B7:;
  i0 = p3;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l5 = i0;
  B6:;
  i0 = l5;
  i1 = 257u;
  i0 = i0 < i1;
  i1 = 400u;
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
  i0 = 0u;
  l6 = i0;
  i0 = l10;
  i1 = 88u;
  i0 += i1;
  i1 = 32u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 104), j1);
  i0 = l10;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 112), j1);
  i0 = l10;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l4 = j1;
  i64_store((&memory), (u64)(i0 + 88), j1);
  i0 = l10;
  j1 = l9;
  i64_store((&memory), (u64)(i0 + 96), j1);
  i0 = 0u;
  l3 = i0;
  j0 = l4;
  j1 = l9;
  j2 = 14289235522390851584ull;
  j3 = l9;
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
  l5 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B9;}
  i0 = l10;
  i1 = 88u;
  i0 += i1;
  i1 = l5;
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
  i0 = f25(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 40));
  i1 = l10;
  i2 = 88u;
  i1 += i2;
  i0 = i0 == i1;
  i1 = 96u;
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
  B9:;
  i0 = l3;
  i1 = 0u;
  i0 = i0 != i1;
  i1 = 432u;
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
  j0 = i64_load((&memory), (u64)(i0 + 32));
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
  i0 = l3;
  i1 = 32u;
  i0 += i1;
  l0 = i0;
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0));
  l4 = j0;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775806ull;
  i0 = j0 > j1;
  if (i0) {goto B15;}
  i0 = 0u;
  l5 = i0;
  L17: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l9;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B16;}
    j0 = l9;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l9 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B18;}
    L19: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l9;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l9 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B16;}
      i0 = l5;
      i1 = 1u;
      i0 += i1;
      l5 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L19;}
    B18:;
    i0 = 1u;
    l6 = i0;
    i0 = l5;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L17;}
    goto B15;
  B16:;
  i0 = 0u;
  l6 = i0;
  B15:;
  i0 = l6;
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
  j0 = l4;
  j1 = 0ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 528u;
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
  j0 = l7;
  i1 = l3;
  j1 = i64_load((&memory), (u64)(i1 + 8));
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
  j0 = l4;
  i1 = l3;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i2 = l3;
  j2 = i64_load((&memory), (u64)(i2));
  j1 -= j2;
  i0 = (u64)((s64)j0 <= (s64)j1);
  i1 = 592u;
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
  i0 = i32_load((&memory), (u64)(i0 + 40));
  i1 = l10;
  i2 = 88u;
  i1 += i2;
  i0 = i0 == i1;
  i1 = 640u;
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
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 88));
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
  j0 = l7;
  i1 = l3;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  l9 = j1;
  i0 = j0 == j1;
  i1 = 752u;
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
  i1 = l3;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = l4;
  j1 += j2;
  l4 = j1;
  i64_store((&memory), (u64)(i0), j1);
  j0 = l4;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
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
  i0 = l3;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  i1 = 832u;
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
  j0 = l9;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l4 = j0;
  i1 = l3;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  i0 = j0 == j1;
  i1 = 864u;
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
  i0 = l10;
  i1 = l10;
  i2 = 128u;
  i1 += i2;
  i2 = 40u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 192), i1);
  i0 = l10;
  i1 = l10;
  i2 = 128u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 188), i1);
  i0 = l10;
  i1 = l10;
  i2 = 128u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 184), i1);
  i0 = l10;
  i1 = l10;
  i2 = 184u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 200), i1);
  i0 = l10;
  i1 = l3;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 212), i1);
  i0 = l10;
  i1 = l3;
  i32_store((&memory), (u64)(i0 + 208), i1);
  i0 = l10;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 216), i1);
  i0 = l10;
  i1 = 208u;
  i0 += i1;
  i1 = l10;
  i2 = 200u;
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
  f27(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i0 = i32_load((&memory), (u64)(i0 + 44));
  j1 = 0ull;
  i2 = l10;
  i3 = 128u;
  i2 += i3;
  i3 = 40u;
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
  j0 = l4;
  i1 = l10;
  i2 = 88u;
  i1 += i2;
  i2 = 16u;
  i1 += i2;
  l5 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B33;}
  i0 = l5;
  j1 = l4;
  j2 = 1ull;
  j1 += j2;
  i64_store((&memory), (u64)(i0), j1);
  B33:;
  i0 = l10;
  i1 = 72u;
  i0 += i1;
  i1 = 12u;
  i0 += i1;
  l5 = i0;
  i1 = p2;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = 72u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i1 = p2;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = p2;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 76), i1);
  i0 = l10;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 72), i1);
  i0 = l0;
  j0 = i64_load((&memory), (u64)(i0));
  l4 = j0;
  i0 = l10;
  i1 = 8u;
  i0 += i1;
  i1 = 12u;
  i0 += i1;
  i1 = l5;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = l10;
  i1 = i32_load((&memory), (u64)(i1 + 76));
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l10;
  i1 = l10;
  i1 = i32_load((&memory), (u64)(i1 + 72));
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = p0;
  j1 = l4;
  i2 = l10;
  i3 = 8u;
  i2 += i3;
  j3 = l4;
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
  _ZN5eosio5token11add_balanceEyNS_5assetEy(i0, j1, i2, j3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  j1 = p1;
  i0 = j0 == j1;
  if (i0) {goto B35;}
  i0 = p0;
  j0 = i64_load((&memory), (u64)(i0));
  l2 = j0;
  j0 = 0ull;
  l4 = j0;
  j0 = 59ull;
  l7 = j0;
  i0 = 928u;
  l5 = i0;
  j0 = 0ull;
  l8 = j0;
  L36: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l4;
    j1 = 5ull;
    i0 = j0 > j1;
    if (i0) {goto B41;}
    i0 = l5;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l3 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B40;}
    i0 = l3;
    i1 = 165u;
    i0 += i1;
    l3 = i0;
    goto B39;
    B41:;
    j0 = 0ull;
    l9 = j0;
    j0 = l4;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B38;}
    goto B37;
    B40:;
    i0 = l3;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l3;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l3 = i0;
    B39:;
    i0 = l3;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l9 = j0;
    B38:;
    j0 = l9;
    j1 = 31ull;
    j0 &= j1;
    j1 = l7;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l9 = j0;
    B37:;
    i0 = l5;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    j0 = l4;
    j1 = 1ull;
    j0 += j1;
    l4 = j0;
    j0 = l9;
    j1 = l8;
    j0 |= j1;
    l8 = j0;
    j0 = l7;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l7 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L36;}
  i0 = l10;
  i1 = 52u;
  i0 += i1;
  i1 = p2;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = 24u;
  i0 += i1;
  i1 = 24u;
  i0 += i1;
  l3 = i0;
  i1 = p2;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = 44u;
  i0 += i1;
  i1 = p2;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l10;
  j1 = l1;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l10;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l10;
  i1 = 56u;
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
  i0 = f60(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 16u;
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
  i0 = f55(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
  j1 = l1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l5;
  j1 = l8;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l10;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 208), i1);
  i0 = l10;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 216), i1);
  i0 = l10;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 212), i1);
  i0 = l10;
  i1 = l10;
  j1 = i64_load((&memory), (u64)(i1 + 24));
  i64_store((&memory), (u64)(i0 + 128), j1);
  i0 = l10;
  i1 = l10;
  j1 = i64_load((&memory), (u64)(i1 + 32));
  i64_store((&memory), (u64)(i0 + 136), j1);
  i0 = l10;
  i1 = 128u;
  i0 += i1;
  i1 = 24u;
  i0 += i1;
  i1 = l3;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l10;
  i1 = l10;
  j1 = i64_load((&memory), (u64)(i1 + 40));
  i64_store((&memory), (u64)(i0 + 144), j1);
  i0 = l10;
  i1 = 128u;
  i0 += i1;
  i1 = 40u;
  i0 += i1;
  l3 = i0;
  i1 = l10;
  i2 = 24u;
  i1 += i2;
  i2 = 40u;
  i1 += i2;
  l5 = i1;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = l10;
  j1 = i64_load((&memory), (u64)(i1 + 56));
  i64_store((&memory), (u64)(i0 + 160), j1);
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 56), i1);
  i0 = l10;
  i1 = 60u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  j0 = l2;
  j1 = 14829575313431724032ull;
  i2 = l10;
  i3 = 208u;
  i2 += i3;
  i3 = l10;
  i4 = 128u;
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
  f32_0(j0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l10;
  i0 = i32_load8_u((&memory), (u64)(i0 + 160));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B45;}
  i0 = l3;
  i0 = i32_load((&memory), (u64)(i0));
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B45:;
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 208));
  l5 = i0;
  i0 = !(i0);
  if (i0) {goto B47;}
  i0 = l10;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 212), i1);
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B47:;
  i0 = l10;
  i1 = 56u;
  i0 += i1;
  i0 = i32_load8_u((&memory), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B35;}
  i0 = l10;
  i1 = 64u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B35:;
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 112));
  p2 = i0;
  i0 = !(i0);
  if (i0) {goto B50;}
  i0 = l10;
  i1 = 116u;
  i0 += i1;
  l6 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B52;}
  L53: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l3 = i0;
    i0 = l5;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto B54;}
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
    f56(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B54:;
    i0 = p2;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L53;}
  i0 = l10;
  i1 = 112u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  goto B51;
  B52:;
  i0 = p2;
  l5 = i0;
  B51:;
  i0 = l6;
  i1 = p2;
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B50:;
  i0 = 0u;
  i1 = l10;
  i2 = 224u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void _ZN5eosio5token11add_balanceEyNS_5assetEy(u32 p0, u64 p1, u32 p2, u64 p3) {
  u32 l1 = 0, l3 = 0, l4 = 0, l5 = 0;
  u64 l0 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4, j5;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 80u;
  i1 -= i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 0u;
  l4 = i0;
  i0 = l5;
  i1 = 8u;
  i0 += i1;
  i1 = 32u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l5;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l2 = j1;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l5;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  j0 = l2;
  j1 = p1;
  j2 = 3607749779137757184ull;
  i3 = p2;
  j3 = i64_load((&memory), (u64)(i3 + 8));
  l0 = j3;
  j4 = 8ull;
  j3 >>= (j4 & 63);
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
  p0 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B3;}
  i0 = l5;
  i1 = 8u;
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
  i0 = f39(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 16));
  i1 = l5;
  i2 = 8u;
  i1 += i2;
  i0 = i0 == i1;
  i1 = 96u;
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
  i0 = 1u;
  i1 = 944u;
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
  i0 = i32_load((&memory), (u64)(i0 + 16));
  i1 = l5;
  i2 = 8u;
  i1 += i2;
  i0 = i0 == i1;
  i1 = 640u;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
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
  j0 = l0;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  p1 = j1;
  i0 = j0 == j1;
  i1 = 752u;
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
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1));
  i2 = p2;
  j2 = i64_load((&memory), (u64)(i2));
  j1 += j2;
  l2 = j1;
  i64_store((&memory), (u64)(i0), j1);
  j0 = l2;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
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
  i0 = l4;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  i1 = 832u;
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
  j0 = p1;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  p1 = j0;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  i0 = j0 == j1;
  i1 = 864u;
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
  i0 = 1u;
  i1 = 272u;
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
  i1 = 64u;
  i0 += i1;
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
  i0 = 1u;
  i1 = 272u;
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
  i1 = 64u;
  i0 += i1;
  i1 = 8u;
  i0 |= i1;
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
  i0 = l4;
  i0 = i32_load((&memory), (u64)(i0 + 20));
  j1 = 0ull;
  i2 = l5;
  i3 = 64u;
  i2 += i3;
  i3 = 16u;
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
  j0 = p1;
  i1 = l5;
  i2 = 8u;
  i1 += i2;
  i2 = 16u;
  i1 += i2;
  l4 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B2;}
  i0 = l4;
  j1 = p1;
  j2 = 1ull;
  j1 += j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 32));
  p2 = i0;
  if (i0) {goto B1;}
  goto B0;
  B3:;
  i0 = l5;
  j0 = i64_load((&memory), (u64)(i0 + 8));
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
  i1 = 208u;
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
  i0 = f55(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = 1u;
  i1 = 288u;
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
  l1 = i0;
  j0 = 5459781ull;
  p1 = j0;
  L25: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = 0u;
    l3 = i0;
    j0 = p1;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B24;}
    j0 = p1;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    p1 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B26;}
    L27: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = p1;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      p1 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B24;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L27;}
    B26:;
    i0 = 1u;
    l3 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L25;}
  B24:;
  i0 = l3;
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
  i1 = l5;
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l4 = i0;
  i1 = p2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = 1u;
  i1 = 272u;
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
  i1 = 64u;
  i0 += i1;
  i1 = p0;
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
  i0 = 1u;
  i1 = 272u;
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
  i1 = 64u;
  i0 += i1;
  i1 = 8u;
  i0 |= i1;
  i1 = l1;
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
  i0 = p0;
  i1 = l5;
  i2 = 8u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 3607749779137757184ull;
  j3 = p3;
  i4 = l4;
  j4 = i64_load((&memory), (u64)(i4));
  j5 = 8ull;
  j4 >>= (j5 & 63);
  p1 = j4;
  i5 = l5;
  i6 = 64u;
  i5 += i6;
  i6 = 16u;
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
  p2 = i1;
  i32_store((&memory), (u64)(i0 + 20), i1);
  j0 = p1;
  i1 = l5;
  i2 = 8u;
  i1 += i2;
  i2 = 16u;
  i1 += i2;
  l3 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B34;}
  i0 = l3;
  j1 = p1;
  j2 = 1ull;
  j1 += j2;
  i64_store((&memory), (u64)(i0), j1);
  B34:;
  i0 = l5;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 56), i1);
  i0 = l5;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  p1 = j1;
  i64_store((&memory), (u64)(i0 + 64), j1);
  i0 = l5;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 52), i1);
  i0 = l5;
  i1 = 36u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = l5;
  i2 = 40u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B36;}
  i0 = l4;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l4;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l5;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 56), i1);
  i0 = l4;
  i1 = p0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = l4;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B35;
  B36:;
  i0 = l5;
  i1 = 32u;
  i0 += i1;
  i1 = l5;
  i2 = 56u;
  i1 += i2;
  i2 = l5;
  i3 = 64u;
  i2 += i3;
  i3 = l5;
  i4 = 52u;
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
  f40(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B35:;
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 56));
  l4 = i0;
  i0 = l5;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 56), i1);
  i0 = l4;
  i0 = !(i0);
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B2:;
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 32));
  p2 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  B1:;
  i0 = l5;
  i1 = 36u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B40;}
  L41: 
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
    if (i0) {goto B42;}
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
    f56(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B42:;
    i0 = p2;
    i1 = l4;
    i0 = i0 != i1;
    if (i0) {goto L41;}
  i0 = l5;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B39;
  B40:;
  i0 = p2;
  l4 = i0;
  B39:;
  i0 = l3;
  i1 = p2;
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = 0u;
  i1 = l5;
  i2 = 80u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f32_0(u64 p0, u64 p1, u32 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 96u;
  i1 -= i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l5;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = 0u;
  l2 = i0;
  i0 = 0u;
  l3 = i0;
  i0 = 0u;
  l4 = i0;
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 -= i1;
  l0 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = l1;
  i1 = 268435456u;
  i0 = i0 >= i1;
  if (i0) {goto B0;}
  i0 = l5;
  i1 = 16u;
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
  i1 = f55(i1);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i1;
  i2 = l1;
  i3 = 4u;
  i2 <<= (i3 & 31);
  i1 += i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = p2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1));
  l3 = i1;
  i0 -= i1;
  p2 = i0;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B3;}
  i0 = l4;
  i1 = l3;
  i2 = p2;
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
  i1 = l4;
  i2 = p2;
  i1 += i2;
  l3 = i1;
  i32_store((&memory), (u64)(i0 + 12), i1);
  goto B1;
  B3:;
  i0 = l4;
  l3 = i0;
  B1:;
  i0 = l5;
  i1 = 44u;
  i0 += i1;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l5;
  i1 = 16u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = 48u;
  i0 += i1;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  j1 = p0;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l5;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 52), i1);
  i0 = l5;
  i1 = 24u;
  i0 += i1;
  i1 = 32u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = 24u;
  i0 += i1;
  i1 = 36u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p3;
  i1 = 36u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p3;
  i1 = i32_load8_u((&memory), (u64)(i1 + 32));
  l4 = i1;
  i2 = 1u;
  i1 >>= (i2 & 31);
  i2 = l4;
  i3 = 1u;
  i2 &= i3;
  i0 = i2 ? i0 : i1;
  p2 = i0;
  i1 = 32u;
  i0 += i1;
  l4 = i0;
  i0 = p2;
  j0 = (u64)(i0);
  p0 = j0;
  i0 = l5;
  i1 = 52u;
  i0 += i1;
  p2 = i0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = p0;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    p0 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L5;}
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B7;}
  i0 = p2;
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
  f33(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = 56u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p2 = i0;
  i0 = l5;
  i1 = 52u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B6;
  B7:;
  i0 = 0u;
  p2 = i0;
  i0 = 0u;
  l4 = i0;
  B6:;
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 84), i1);
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 80), i1);
  i0 = l5;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 88), i1);
  i0 = l5;
  i1 = l5;
  i2 = 80u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = l5;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 72), i1);
  i0 = l5;
  i1 = 72u;
  i0 += i1;
  i1 = l5;
  i2 = 64u;
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
  f34(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = 80u;
  i0 += i1;
  i1 = l5;
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
  f35(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 80));
  l4 = i0;
  i1 = l5;
  i1 = i32_load((&memory), (u64)(i1 + 84));
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
  (*Z_envZ_send_inlineZ_vii)(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 80));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B12;}
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 84), i1);
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B12:;
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 52));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B14;}
  i0 = l5;
  i1 = 56u;
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 40));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B16;}
  i0 = l5;
  i1 = 44u;
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B16:;
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B18;}
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 12), i1);
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B18:;
  i0 = 0u;
  i1 = l5;
  i2 = 96u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  goto Bfunc;
  B0:;
  i0 = l5;
  i1 = 8u;
  i0 += i1;
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
  f59(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f33(u32 p0, u32 p1) {
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
  i0 = f55(i0);
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
  f59(i0);
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
  f56(i0);
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

static void f34(u32 p0, u32 p1) {
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
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 272u;
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
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = l0;
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
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 272u;
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
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p0;
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
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 272u;
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
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p0;
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
  i0 = l1;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l1;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 272u;
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
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p0;
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
  i0 = l1;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p0;
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
  i0 = f38(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static void f35(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l5 = 0, l6 = 0;
  u64 l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = 16u;
  l3 = i0;
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  l0 = i0;
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  l1 = i1;
  i0 -= i1;
  l2 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l4 = j0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    j0 = l4;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L0;}
  i0 = l1;
  i1 = l5;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  i0 = l2;
  i1 = 4294967280u;
  i0 &= i1;
  i1 = l3;
  i0 += i1;
  l3 = i0;
  B1:;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 28));
  l5 = i0;
  i1 = l3;
  i0 -= i1;
  i1 = p1;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l1 = i1;
  i0 -= i1;
  l3 = i0;
  i0 = p1;
  i1 = 28u;
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = l5;
  i0 -= i1;
  j0 = (u64)(i0);
  l4 = j0;
  L2: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 4294967295u;
    i0 += i1;
    l3 = i0;
    j0 = l4;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L2;}
  i0 = 0u;
  l5 = i0;
  i0 = l3;
  i0 = !(i0);
  if (i0) {goto B4;}
  i0 = p0;
  i1 = 0u;
  i2 = l3;
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
  f33(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  goto B3;
  B4:;
  i0 = 0u;
  l3 = i0;
  B3:;
  i0 = l6;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
  i1 = l3;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 272u;
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
  i0 = l5;
  i1 = l3;
  i2 = 8u;
  i1 += i2;
  p0 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 272u;
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
  i1 = p1;
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
  i0 = l6;
  i1 = l3;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l6;
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
  i0 = f36(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
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
  i0 = f37(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f36(u32 p0, u32 p1) {
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
    i1 = 272u;
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
    i1 = 272u;
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
    i1 = 272u;
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

static u32 f37(u32 p0, u32 p1) {
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
    i1 = 272u;
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
  i1 = 272u;
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

static u32 f38(u32 p0, u32 p1) {
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
  i1 = i32_load8_u((&memory), (u64)(i1));
  l3 = i1;
  i2 = 1u;
  i1 >>= (i2 & 31);
  i2 = l3;
  i3 = 1u;
  i2 &= i3;
  i0 = i2 ? i0 : i1;
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
    i1 = 272u;
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
  i0 = p1;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p1;
  i1 = i32_load8_u((&memory), (u64)(i1));
  l3 = i1;
  i2 = 1u;
  i1 >>= (i2 & 31);
  i2 = l3;
  i3 = 1u;
  i2 &= i3;
  l0 = i2;
  i0 = i2 ? i0 : i1;
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l1 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l4;
  i0 -= i1;
  i1 = l3;
  i0 = (u32)((s32)i0 >= (s32)i1);
  i1 = 272u;
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
  i1 = l1;
  i2 = p1;
  i3 = 1u;
  i2 += i3;
  i3 = l0;
  i1 = i3 ? i1 : i2;
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

static u32 f39(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l7 = 0, l8 = 0;
  u64 l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 32u;
  i0 -= i1;
  l8 = i0;
  l7 = i0;
  i0 = 0u;
  i1 = l8;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 24));
  l1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = l1;
  i0 -= i1;
  l2 = i0;
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
    l3 = i0;
    l4 = i0;
    i0 = l3;
    i1 = l2;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L1;}
  B0:;
  i0 = l5;
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = l5;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
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
  l5 = i0;
  i1 = 31u;
  i0 >>= (i1 & 31);
  i1 = 1u;
  i0 ^= i1;
  i1 = 352u;
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
  i1 = 512u;
  i0 = i0 <= i1;
  if (i0) {goto B7;}
  i0 = p1;
  i1 = l5;
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
  i1 = malloc_0(i1);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i1;
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
  i0 = (*Z_envZ_db_get_i64Z_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
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
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B6;
  B7:;
  i0 = 0u;
  i1 = l8;
  i2 = l5;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i1 = l1;
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
  i0 = (*Z_envZ_db_get_i64Z_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B6:;
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  l0 = i0;
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
  i0 = f55(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = 1u;
  i1 = 288u;
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
  i1 = 8u;
  i0 += i1;
  l8 = i0;
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L16: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B15;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B17;}
    L18: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B15;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L18;}
    B17:;
    i0 = 1u;
    l2 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L16;}
    goto B14;
  B15:;
  i0 = 0u;
  l2 = i0;
  B14:;
  i0 = l2;
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
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l5;
  i1 = 7u;
  i0 = i0 > i1;
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
  i0 = l3;
  i1 = l1;
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
  i0 = l5;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = 8u;
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
  i0 = l8;
  i1 = l1;
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
  i0 = l3;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l7;
  i1 = l3;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l7;
  i1 = l3;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  l6 = j1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l7;
  i1 = l3;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  p1 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = p0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B25;}
  i0 = l4;
  j1 = l6;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l4;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l7;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l4;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = l4;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B24;
  B25:;
  i0 = l0;
  i1 = l7;
  i2 = 24u;
  i1 += i2;
  i2 = l7;
  i3 = 16u;
  i2 += i3;
  i3 = l7;
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
  f40(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B24:;
  i0 = l7;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  l4 = i0;
  i0 = l7;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l4;
  i0 = !(i0);
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B2:;
  i0 = 0u;
  i1 = l7;
  i2 = 32u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l3;
  FUNC_EPILOGUE;
  return i0;
}

static void f40(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f55(i0);
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
  f59(i0);
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
  l2 = i0;
  goto B6;
  B7:;
  i0 = l3;
  l2 = i0;
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
  i1 = l2;
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
    f56(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B11:;
    i0 = l2;
    i1 = l3;
    i0 = i0 != i1;
    if (i0) {goto L10;}
  B9:;
  i0 = l2;
  i0 = !(i0);
  if (i0) {goto B13;}
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static void _ZN5eosio5token8transferEyyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEE(u32 p0, u64 p1, u64 p2, u32 p3, u32 p4) {
  u32 l1 = 0, l3 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0, l2 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 112u;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  j0 = p1;
  j1 = p2;
  i0 = j0 != j1;
  i1 = 992u;
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
  j0 = p1;
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
  j0 = p2;
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
  i0 = (*Z_envZ_is_accountZ_ij)(j0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = 1024u;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l0 = j0;
  i0 = 0u;
  l3 = i0;
  i0 = l6;
  i1 = 104u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  j1 = l0;
  j2 = 8ull;
  j1 >>= (j2 & 63);
  l4 = j1;
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = l6;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 88), j1);
  i0 = l6;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 96), j1);
  i0 = l6;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 72), j1);
  i0 = l6;
  i1 = 72u;
  i0 += i1;
  j1 = l4;
  i2 = 1056u;
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
  i0 = f42(i0, j1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  j0 = p1;
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
  (*Z_envZ_require_recipientZ_vj)(j0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = p2;
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
  (*Z_envZ_require_recipientZ_vj)(j0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0));
  l2 = j0;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775806ull;
  i0 = j0 > j1;
  if (i0) {goto B7;}
  i0 = 0u;
  l5 = i0;
  L9: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l4;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B8;}
    j0 = l4;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B10;}
    L11: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l4;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l4 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B8;}
      i0 = l5;
      i1 = 1u;
      i0 += i1;
      l5 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L11;}
    B10:;
    i0 = 1u;
    l3 = i0;
    i0 = l5;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L9;}
    goto B7;
  B8:;
  i0 = 0u;
  l3 = i0;
  B7:;
  i0 = l3;
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
  j0 = l2;
  j1 = 0ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 1088u;
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
  j0 = l0;
  i1 = l1;
  j1 = i64_load((&memory), (u64)(i1 + 8));
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
  i0 = p4;
  i0 = i32_load8_u((&memory), (u64)(i0));
  l5 = i0;
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B16;}
  i0 = l5;
  i1 = 1u;
  i0 >>= (i1 & 31);
  l5 = i0;
  goto B15;
  B16:;
  i0 = p4;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l5 = i0;
  B15:;
  i0 = l5;
  i1 = 257u;
  i0 = i0 < i1;
  i1 = 400u;
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
  i1 = 56u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l5 = i0;
  i1 = p3;
  i2 = 8u;
  i1 += i2;
  l3 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0));
  l4 = j0;
  i0 = l6;
  i1 = 24u;
  i0 += i1;
  i1 = 12u;
  i0 += i1;
  i1 = l6;
  i2 = 56u;
  i1 += i2;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = 24u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l5;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  j1 = l4;
  i64_store((&memory), (u64)(i0 + 56), j1);
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 60));
  i32_store((&memory), (u64)(i0 + 28), i1);
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 56));
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = p0;
  j1 = p1;
  i2 = l6;
  i3 = 24u;
  i2 += i3;
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
  _ZN5eosio5token11sub_balanceEyNS_5assetE(i0, j1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = 40u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l5 = i0;
  i1 = l3;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0));
  l4 = j0;
  i0 = l6;
  i1 = 8u;
  i0 += i1;
  i1 = 12u;
  i0 += i1;
  i1 = l6;
  i2 = 40u;
  i1 += i2;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l5;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  j1 = l4;
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 44));
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 40));
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = p0;
  j1 = p2;
  i2 = l6;
  i3 = 8u;
  i2 += i3;
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
  _ZN5eosio5token11add_balanceEyNS_5assetEy(i0, j1, i2, j3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 96));
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B20;}
  i0 = l6;
  i1 = 100u;
  i0 += i1;
  p0 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B22;}
  L23: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    p3 = i0;
    i0 = l5;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p3;
    i0 = !(i0);
    if (i0) {goto B24;}
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
    f56(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B24:;
    i0 = l3;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L23;}
  i0 = l6;
  i1 = 96u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  goto B21;
  B22:;
  i0 = l3;
  l5 = i0;
  B21:;
  i0 = p0;
  i1 = l3;
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B20:;
  i0 = 0u;
  i1 = l6;
  i2 = 112u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f42(u32 p0, u64 p1, u32 p2) {
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
    j0 = i64_load((&memory), (u64)(i0 + 8));
    j1 = 8ull;
    j0 >>= (j1 & 63);
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
  i0 = i32_load((&memory), (u64)(i0 + 40));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 96u;
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
  j2 = 14289235522390851584ull;
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
  i0 = f25(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 40));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 96u;
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

static void _ZN5eosio5token11sub_balanceEyNS_5assetE(u32 p0, u64 p1, u32 p2) {
  u32 l2 = 0, l4 = 0, l5 = 0;
  u64 l0 = 0, l1 = 0, l3 = 0;
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
  i0 = l5;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l5;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l5;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l5;
  i1 = 8u;
  i0 += i1;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  l0 = j1;
  j2 = 8ull;
  j1 >>= (j2 & 63);
  i2 = 1120u;
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
  i0 = f44(i0, j1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1));
  l1 = j1;
  i0 = (u64)((s64)j0 >= (s64)j1);
  i1 = 1152u;
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
  j0 = l1;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 != j1;
  if (i0) {goto B4;}
  i0 = l5;
  i1 = 8u;
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
  f45(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 32));
  l2 = i0;
  if (i0) {goto B3;}
  goto B2;
  B4:;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 16));
  i1 = l5;
  i2 = 8u;
  i1 += i2;
  i0 = i0 == i1;
  i1 = 640u;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
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
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  l3 = j1;
  i0 = j0 == j1;
  i1 = 1184u;
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
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = l1;
  j1 -= j2;
  l1 = j1;
  i64_store((&memory), (u64)(i0), j1);
  j0 = l1;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 1232u;
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
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  i1 = 1264u;
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
  j0 = l3;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l1 = j0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  i0 = j0 == j1;
  i1 = 864u;
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
  i0 = 1u;
  i1 = 272u;
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
  i1 = 48u;
  i0 += i1;
  i1 = p0;
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
  i0 = 1u;
  i1 = 272u;
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
  i1 = 48u;
  i0 += i1;
  i1 = 8u;
  i0 |= i1;
  i1 = p0;
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
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 20));
  j1 = p1;
  i2 = l5;
  i3 = 48u;
  i2 += i3;
  i3 = 16u;
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
  j0 = l1;
  i1 = l5;
  i2 = 8u;
  i1 += i2;
  i2 = 16u;
  i1 += i2;
  p0 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B18;}
  i0 = p0;
  j1 = l1;
  j2 = 1ull;
  j1 += j2;
  i64_store((&memory), (u64)(i0), j1);
  B18:;
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 32));
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  B3:;
  i0 = l5;
  i1 = 36u;
  i0 += i1;
  l4 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B20;}
  L21: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 4294967272u;
    i0 += i1;
    p0 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    p2 = i0;
    i0 = p0;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p2;
    i0 = !(i0);
    if (i0) {goto B22;}
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
    f56(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B22:;
    i0 = l2;
    i1 = p0;
    i0 = i0 != i1;
    if (i0) {goto L21;}
  i0 = l5;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  goto B19;
  B20:;
  i0 = l2;
  p0 = i0;
  B19:;
  i0 = l4;
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B2:;
  i0 = 0u;
  i1 = l5;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f44(u32 p0, u64 p1, u32 p2) {
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
    j0 = i64_load((&memory), (u64)(i0 + 8));
    j1 = 8ull;
    j0 >>= (j1 & 63);
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
  i0 = i32_load((&memory), (u64)(i0 + 16));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 96u;
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
  j2 = 3607749779137757184ull;
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
  i0 = f39(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 16));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 96u;
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

static void f45(u32 p0, u32 p1) {
  u32 l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 16));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 1296u;
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
  i1 = 1344u;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
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
    j0 = i64_load((&memory), (u64)(i0 + 8));
    j1 = l0;
    j0 ^= j1;
    j1 = 256ull;
    i0 = j0 < j1;
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
  i1 = 1408u;
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
    f56(i0);
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
  L11: 
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
    if (i0) {goto B12;}
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
    f56(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B12:;
    i0 = l6;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L11;}
  B6:;
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i1 = l6;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 20));
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
  i2 = 64u;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  j0 = 0ull;
  l3 = j0;
  j0 = 59ull;
  l2 = j0;
  i0 = 1472u;
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
  i0 = 1488u;
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
  i1 = 1504u;
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
  i0 = 1472u;
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
  i64_store((&memory), (u64)(i0 + 56), j1);
  j0 = p2;
  j1 = 14829575313431724032ull;
  i0 = j0 == j1;
  if (i0) {goto B23;}
  j0 = p2;
  j1 = 8516769789752901632ull;
  i0 = j0 == j1;
  if (i0) {goto B22;}
  j0 = p2;
  j1 = 5031766152489992192ull;
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
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l6;
  i1 = 56u;
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
  i0 = f47(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B14;
  B23:;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l6;
  i1 = 2u;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l6;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1 + 32));
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l6;
  i1 = 56u;
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
  i0 = f49(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B14;
  B22:;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 44), i1);
  i0 = l6;
  i1 = 3u;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l6;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1 + 40));
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l6;
  i1 = 56u;
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
  i0 = f48(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
  i0 = 0u;
  i1 = l6;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f47(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l3 = 0, l5 = 0, l6 = 0, l7 = 0;
  u64 l2 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 96u;
  i0 -= i1;
  l5 = i0;
  l7 = i0;
  i0 = 0u;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  i0 = 0u;
  p1 = i0;
  i0 = 0u;
  l3 = i0;
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
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  goto B2;
  B3:;
  i0 = 0u;
  i1 = l5;
  i2 = l1;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l3 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B2:;
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
  i0 = l7;
  i1 = 40u;
  i0 += i1;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l7;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l7;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = 1u;
  i1 = 288u;
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
  j0 = 5459781ull;
  l4 = j0;
  L8: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = 0u;
    l5 = i0;
    j0 = l4;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B7;}
    j0 = l4;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B9;}
    L10: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l4;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l4 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B7;}
      i0 = p1;
      i1 = 1u;
      i0 += i1;
      p1 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L10;}
    B9:;
    i0 = 1u;
    l5 = i0;
    i0 = p1;
    i1 = 1u;
    i0 += i1;
    p1 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L8;}
  B7:;
  i0 = l5;
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
  i1 = 7u;
  i0 = i0 > i1;
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
  i0 = l7;
  i1 = 24u;
  i0 += i1;
  i1 = l3;
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
  i1 = 4294967288u;
  i0 &= i1;
  l5 = i0;
  i1 = 8u;
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
  i0 = l7;
  i1 = 24u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  p1 = i0;
  i1 = l3;
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
  i0 = l5;
  i1 = 16u;
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
  i0 = l7;
  i1 = 24u;
  i0 += i1;
  i1 = 16u;
  i0 += i1;
  i1 = l3;
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
  i0 = l1;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B18;}
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
  B18:;
  i0 = l7;
  i1 = 48u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l5 = i0;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l7;
  j0 = i64_load((&memory), (u64)(i0 + 24));
  l4 = j0;
  i0 = l7;
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = l7;
  i1 = 64u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l5;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l7;
  i1 = l7;
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
  if (i0) {goto B20;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l6;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  B20:;
  i0 = l7;
  i1 = 80u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l7;
  i2 = 64u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  l2 = j1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l7;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  j1 = l2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l7;
  i1 = l7;
  j1 = i64_load((&memory), (u64)(i1 + 64));
  l2 = j1;
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = l7;
  j1 = l2;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p1;
  j1 = l4;
  i2 = l7;
  i3 = 8u;
  i2 += i3;
  i3 = l6;
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
  CALL_INDIRECT(T0, void (*)(u32, u64, u32), 0, i3, i0, j1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = l7;
  i2 = 96u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 1u;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f48(u32 p0, u32 p1) {
  u32 l0 = 0, l2 = 0, l3 = 0;
  u64 l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 80u;
  i1 -= i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l2;
  l3 = i0;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 60), i1);
  i0 = l3;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l3;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i32_store((&memory), (u64)(i0 + 52), i1);
  i0 = 0u;
  p1 = i0;
  i0 = 0u;
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
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = l0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  goto B2;
  B3:;
  i0 = 0u;
  i1 = l2;
  i2 = l0;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  p0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B2:;
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
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l3;
  i1 = 24u;
  i0 += i1;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = 1u;
  i1 = 288u;
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
  j0 = 5459781ull;
  l1 = j0;
  L9: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l1;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B8;}
    j0 = l1;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l1 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B10;}
    L11: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l1;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l1 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B8;}
      i0 = p1;
      i1 = 1u;
      i0 += i1;
      p1 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L11;}
    B10:;
    i0 = 1u;
    l2 = i0;
    i0 = p1;
    i1 = 1u;
    i0 += i1;
    p1 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L9;}
    goto B7;
  B8:;
  i0 = 0u;
  l2 = i0;
  B7:;
  i0 = l2;
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
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l3;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = l3;
  i1 = p0;
  i2 = l0;
  i1 += i2;
  p1 = i1;
  i32_store((&memory), (u64)(i0 + 72), i1);
  i0 = l0;
  i1 = 7u;
  i0 = i0 > i1;
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
  i0 = l3;
  i1 = 8u;
  i0 += i1;
  i1 = p0;
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
  i1 = p0;
  i2 = 8u;
  i1 += i2;
  l2 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
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
  i0 = l3;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
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
  i0 = p1;
  i1 = p0;
  i2 = 16u;
  i1 += i2;
  l2 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
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
  i0 = l3;
  i1 = 8u;
  i0 += i1;
  i1 = 16u;
  i0 += i1;
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
  i0 = l3;
  i1 = p0;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 68), i1);
  i0 = l3;
  i1 = 64u;
  i0 += i1;
  i1 = l3;
  i2 = 8u;
  i1 += i2;
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
  i0 = f52(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B20;}
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
  B20:;
  i0 = l3;
  i1 = l3;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 68), i1);
  i0 = l3;
  i1 = l3;
  i2 = 60u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = l3;
  i1 = 64u;
  i0 += i1;
  i1 = l3;
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
  f54(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i0 = i32_load8_u((&memory), (u64)(i0 + 32));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B23;}
  i0 = l3;
  i1 = 40u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B23:;
  i0 = 0u;
  i1 = l3;
  i2 = 80u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 1u;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f49(u32 p0, u32 p1) {
  u32 l0 = 0, l2 = 0, l3 = 0;
  u64 l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 96u;
  i1 -= i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l2;
  l3 = i0;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 60), i1);
  i0 = l3;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l3;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i32_store((&memory), (u64)(i0 + 52), i1);
  i0 = 0u;
  p1 = i0;
  i0 = 0u;
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
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = l0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  goto B2;
  B3:;
  i0 = 0u;
  i1 = l2;
  i2 = l0;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  p0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B2:;
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
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l3;
  i1 = 24u;
  i0 += i1;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = 1u;
  i1 = 288u;
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
  j0 = 5459781ull;
  l1 = j0;
  L9: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l1;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B8;}
    j0 = l1;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l1 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B10;}
    L11: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l1;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l1 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B8;}
      i0 = p1;
      i1 = 1u;
      i0 += i1;
      p1 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L11;}
    B10:;
    i0 = 1u;
    l2 = i0;
    i0 = p1;
    i1 = 1u;
    i0 += i1;
    p1 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L9;}
    goto B7;
  B8:;
  i0 = 0u;
  l2 = i0;
  B7:;
  i0 = l2;
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
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l3;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 68), i1);
  i0 = l3;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = l3;
  i1 = p0;
  i2 = l0;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 72), i1);
  i0 = l3;
  i1 = l3;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 80), i1);
  i0 = l3;
  i1 = l3;
  i32_store((&memory), (u64)(i0 + 88), i1);
  i0 = l3;
  i1 = 88u;
  i0 += i1;
  i1 = l3;
  i2 = 80u;
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
  f50(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B14;}
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
  B14:;
  i0 = l3;
  i1 = l3;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 68), i1);
  i0 = l3;
  i1 = l3;
  i2 = 60u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = l3;
  i1 = 64u;
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
  f51(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i0 = i32_load8_u((&memory), (u64)(i0 + 32));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B17;}
  i0 = l3;
  i1 = 40u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B17:;
  i0 = 0u;
  i1 = l3;
  i2 = 96u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 1u;
  FUNC_EPILOGUE;
  return i0;
}

static void f50(u32 p0, u32 p1) {
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
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l1;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
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
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p0;
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
  i0 = f52(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static void f51(u32 p0, u32 p1) {
  u32 l2 = 0, l3 = 0;
  u64 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 96u;
  i1 -= i2;
  l3 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l3;
  i1 = 44u;
  i0 += i1;
  i1 = p1;
  i2 = 28u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = 32u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l2 = i0;
  i1 = p1;
  i2 = 24u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l3;
  i1 = p1;
  i2 = 20u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l1 = j0;
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = l3;
  i1 = 16u;
  i0 += i1;
  i1 = p1;
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
  i0 = f60(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = 48u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l3;
  i1 = l3;
  j1 = i64_load((&memory), (u64)(i1 + 32));
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  p1 = i1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l2 = i1;
  i2 = 1u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  i0 += i1;
  p0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  p1 = i0;
  i0 = l2;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p1;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p1 = i0;
  B1:;
  i0 = l3;
  i1 = 80u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l2 = i0;
  i1 = l3;
  i2 = 48u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l3;
  i1 = l3;
  j1 = i64_load((&memory), (u64)(i1 + 48));
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = l3;
  i1 = 64u;
  i0 += i1;
  i1 = l3;
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
  i0 = f60(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l3;
  i1 = l3;
  j1 = i64_load((&memory), (u64)(i1 + 80));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  j1 = l0;
  j2 = l1;
  i3 = l3;
  i4 = l3;
  i5 = 64u;
  i4 += i5;
  i5 = p1;
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
  CALL_INDIRECT(T0, void (*)(u32, u64, u64, u32, u32), 1, i5, i0, j1, j2, i3, i4);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i0 = i32_load8_u((&memory), (u64)(i0 + 64));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B4;}
  i0 = l3;
  i0 = i32_load((&memory), (u64)(i0 + 72));
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B4:;
  i0 = l3;
  i0 = i32_load8_u((&memory), (u64)(i0 + 16));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B6;}
  i0 = l3;
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B6:;
  i0 = 0u;
  i1 = l3;
  i2 = 96u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f52(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 32u;
  i1 -= i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l5;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
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
  i0 = f53(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 20));
  l3 = i0;
  i1 = l5;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  l2 = i1;
  i0 = i0 != i1;
  if (i0) {goto B9;}
  i0 = p1;
  i0 = i32_load8_u((&memory), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B8;}
  i0 = p1;
  i1 = 0u;
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  l2 = i0;
  goto B7;
  B9:;
  i0 = l5;
  i1 = 8u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l3;
  i1 = l2;
  i0 -= i1;
  l0 = i0;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  i0 = l0;
  i1 = 11u;
  i0 = i0 >= i1;
  if (i0) {goto B6;}
  i0 = l5;
  i1 = l0;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = 1u;
  i0 |= i1;
  l4 = i0;
  i0 = l0;
  if (i0) {goto B5;}
  goto B4;
  B8:;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  l2 = i0;
  B7:;
  i0 = p1;
  i1 = 0u;
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
  f58(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 16));
  l2 = i0;
  if (i0) {goto B3;}
  goto B2;
  B6:;
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  i1 = 4294967280u;
  i0 &= i1;
  l3 = i0;
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
  i0 = f55(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = l5;
  i1 = l3;
  i2 = 1u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B5:;
  i0 = l0;
  l1 = i0;
  i0 = l4;
  l3 = i0;
  L12: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = l2;
    i1 = i32_load8_u((&memory), (u64)(i1));
    i32_store8((&memory), (u64)(i0), i1);
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i0 = l2;
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    i0 = l1;
    i1 = 4294967295u;
    i0 += i1;
    l1 = i0;
    if (i0) {goto L12;}
  i0 = l4;
  i1 = l0;
  i0 += i1;
  l4 = i0;
  B4:;
  i0 = l4;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = p1;
  i0 = i32_load8_u((&memory), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B14;}
  i0 = p1;
  i1 = 0u;
  i32_store16((&memory), (u64)(i0), i1);
  goto B13;
  B14:;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B13:;
  i0 = p1;
  i1 = 0u;
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
  f58(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  i1 = l5;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = l5;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 16));
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  B3:;
  i0 = l5;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 20), i1);
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B2:;
  i0 = 0u;
  i1 = l5;
  i2 = 32u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  goto Bfunc;
  B1:;
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
  f57(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f53(u32 p0, u32 p1) {
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
    i1 = 1568u;
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
  f33(i0, i1);
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

static void f54(u32 p0, u32 p1) {
  u32 l1 = 0, l2 = 0;
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 96u;
  i1 -= i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l2;
  i1 = 32u;
  i0 += i1;
  i1 = 12u;
  i0 += i1;
  i1 = p1;
  i2 = 20u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l2;
  i1 = 32u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i1 = p1;
  i2 = 16u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l2;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 8));
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l2;
  i1 = p1;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = l2;
  i1 = 16u;
  i0 += i1;
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
  i0 = f60(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 48u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l2;
  i1 = l2;
  j1 = i64_load((&memory), (u64)(i1 + 32));
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  p1 = i1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l1 = i1;
  i2 = 1u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  i0 += i1;
  p0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  p1 = i0;
  i0 = l1;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p1;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p1 = i0;
  B1:;
  i0 = l2;
  i1 = 80u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i1 = l2;
  i2 = 48u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l2;
  i1 = l2;
  j1 = i64_load((&memory), (u64)(i1 + 48));
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = l2;
  i1 = 64u;
  i0 += i1;
  i1 = l2;
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
  i0 = f60(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l2;
  i1 = l2;
  j1 = i64_load((&memory), (u64)(i1 + 80));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  j1 = l0;
  i2 = l2;
  i3 = l2;
  i4 = 64u;
  i3 += i4;
  i4 = p1;
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
  CALL_INDIRECT(T0, void (*)(u32, u64, u32, u32), 2, i4, i0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i0 = i32_load8_u((&memory), (u64)(i0 + 64));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B4;}
  i0 = l2;
  i0 = i32_load((&memory), (u64)(i0 + 72));
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B4:;
  i0 = l2;
  i0 = i32_load8_u((&memory), (u64)(i0 + 16));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B6;}
  i0 = l2;
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B6:;
  i0 = 0u;
  i1 = l2;
  i2 = 96u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f55(u32 p0) {
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
    i0 = i32_load((&memory), (u64)(i0 + 1572));
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

static void f56(u32 p0) {
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

static void f57(u32 p0) {
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

static void f58(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B0;}
  i0 = 10u;
  l0 = i0;
  i0 = p0;
  i0 = i32_load8_u((&memory), (u64)(i0));
  l3 = i0;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i1 = 4294967294u;
  i0 &= i1;
  i1 = 4294967295u;
  i0 += i1;
  l0 = i0;
  B1:;
  i0 = l3;
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B3;}
  i0 = l3;
  i1 = 254u;
  i0 &= i1;
  i1 = 1u;
  i0 >>= (i1 & 31);
  l1 = i0;
  goto B2;
  B3:;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l1 = i0;
  B2:;
  i0 = 10u;
  l2 = i0;
  i0 = l1;
  i1 = p1;
  i2 = l1;
  i3 = p1;
  i2 = i2 > i3;
  i0 = i2 ? i0 : i1;
  p1 = i0;
  i1 = 11u;
  i0 = i0 < i1;
  if (i0) {goto B4;}
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  i1 = 4294967280u;
  i0 &= i1;
  i1 = 4294967295u;
  i0 += i1;
  l2 = i0;
  B4:;
  i0 = l2;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  i0 = l2;
  i1 = 10u;
  i0 = i0 != i1;
  if (i0) {goto B7;}
  i0 = 1u;
  l4 = i0;
  i0 = p0;
  i1 = 1u;
  i0 += i1;
  p1 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l0 = i0;
  i0 = 0u;
  l5 = i0;
  goto B6;
  B7:;
  i0 = l2;
  i1 = 1u;
  i0 += i1;
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
  i0 = f55(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i0 = l2;
  i1 = l0;
  i0 = i0 > i1;
  if (i0) {goto B9;}
  i0 = p1;
  i0 = !(i0);
  if (i0) {goto B5;}
  B9:;
  i0 = p0;
  i0 = i32_load8_u((&memory), (u64)(i0));
  l3 = i0;
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B10;}
  i0 = 1u;
  l5 = i0;
  i0 = p0;
  i1 = 1u;
  i0 += i1;
  l0 = i0;
  i0 = 0u;
  l4 = i0;
  goto B6;
  B10:;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l0 = i0;
  i0 = 1u;
  l4 = i0;
  i0 = 1u;
  l5 = i0;
  B6:;
  i0 = l3;
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B12;}
  i0 = l3;
  i1 = 254u;
  i0 &= i1;
  i1 = 1u;
  i0 >>= (i1 & 31);
  l3 = i0;
  goto B11;
  B12:;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l3 = i0;
  B11:;
  i0 = l3;
  i1 = 1u;
  i0 += i1;
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B13;}
  i0 = p1;
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
  B13:;
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B15;}
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
  f56(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B15:;
  i0 = l5;
  i0 = !(i0);
  if (i0) {goto B17;}
  i0 = p0;
  i1 = l1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = p0;
  i1 = l2;
  i2 = 1u;
  i1 += i2;
  i2 = 1u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0), i1);
  goto Bfunc;
  B17:;
  i0 = p0;
  i1 = l1;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&memory), (u64)(i0), i1);
  B5:;
  goto Bfunc;
  B0:;
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
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f59(u32 p0) {
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

static u32 f60(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i0 = i32_load8_u((&memory), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B0;}
  i0 = p0;
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l1;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  goto Bfunc;
  B0:;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l1 = i0;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l0 = i0;
  i0 = l1;
  i1 = 11u;
  i0 = i0 >= i1;
  if (i0) {goto B4;}
  i0 = p0;
  i1 = l1;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 1u;
  i0 += i1;
  p1 = i0;
  i0 = l1;
  if (i0) {goto B3;}
  goto B2;
  B4:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = 4294967280u;
  i0 &= i1;
  l2 = i0;
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
  i0 = f55(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i0 = p0;
  i1 = l2;
  i2 = 1u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = p0;
  i1 = l1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B3:;
  i0 = p1;
  i1 = l0;
  i2 = l1;
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
  B2:;
  i0 = p1;
  i1 = l1;
  i0 += i1;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = p0;
  goto Bfunc;
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
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
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
  i0 = 1576u;
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
  i0 = f63(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
  return i0;
}

static u32 f63(u32 p0, u32 p1) {
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
    i0 = f64_0(i0);
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
    i1 = 9984u;
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

static u32 f64_0(u32 p0) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8388));
  l0 = i0;
  i0 = 0u;
  i0 = i32_load8_u((&memory), (u64)(i0 + 10070));
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 10072));
  l6 = i0;
  goto B0;
  B1:;
  i0 = memory.pages;
  l6 = i0;
  i0 = 0u;
  i1 = 1u;
  i32_store8((&memory), (u64)(i0 + 10070), i1);
  i0 = 0u;
  i1 = l6;
  i2 = 16u;
  i1 <<= (i2 & 31);
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 10072), i1);
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
  i0 = i32_load((&memory), (u64)(i0 + 10072));
  l2 = i0;
  B5:;
  i0 = 0u;
  l7 = i0;
  i0 = 0u;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 10072), i1);
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
  i0 = i32_load8_u((&memory), (u64)(i0 + 10070));
  if (i0) {goto B6;}
  i0 = memory.pages;
  l2 = i0;
  i0 = 0u;
  i1 = 1u;
  i32_store8((&memory), (u64)(i0 + 10070), i1);
  i0 = 0u;
  i1 = l2;
  i2 = 16u;
  i1 <<= (i2 & 31);
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 10072), i1);
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
  i0 = i32_load((&memory), (u64)(i0 + 10072));
  l5 = i0;
  B7:;
  i0 = 0u;
  i1 = l5;
  i2 = l4;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 10072), i1);
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
  i0 = i32_load((&memory), (u64)(i0 + 9960));
  l1 = i0;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B1;}
  i0 = 9768u;
  l2 = i0;
  i0 = l1;
  i1 = 12u;
  i0 *= i1;
  i1 = 9768u;
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

static void f66(void) {
  FUNC_PROLOGUE;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  FUNC_EPILOGUE;
}

static const u8 data_segment_data_0[] = {
  0x60, 0x4f, 0x00, 0x00, 
};

static const u8 data_segment_data_1[] = {
  0x69, 0x6e, 0x76, 0x61, 0x6c, 0x69, 0x64, 0x20, 0x73, 0x79, 0x6d, 0x62, 
  0x6f, 0x6c, 0x20, 0x6e, 0x61, 0x6d, 0x65, 0x00, 
};

static const u8 data_segment_data_2[] = {
  0x69, 0x6e, 0x76, 0x61, 0x6c, 0x69, 0x64, 0x20, 0x73, 0x75, 0x70, 0x70, 
  0x6c, 0x79, 0x00, 
};

static const u8 data_segment_data_3[] = {
  0x6d, 0x61, 0x78, 0x2d, 0x73, 0x75, 0x70, 0x70, 0x6c, 0x79, 0x20, 0x6d, 
  0x75, 0x73, 0x74, 0x20, 0x62, 0x65, 0x20, 0x70, 0x6f, 0x73, 0x69, 0x74, 
  0x69, 0x76, 0x65, 0x00, 
};

static const u8 data_segment_data_4[] = {
  0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x20, 0x70, 0x61, 0x73, 0x73, 0x65, 
  0x64, 0x20, 0x74, 0x6f, 0x20, 0x69, 0x74, 0x65, 0x72, 0x61, 0x74, 0x6f, 
  0x72, 0x5f, 0x74, 0x6f, 0x20, 0x69, 0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 
  0x69, 0x6e, 0x20, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x5f, 0x69, 0x6e, 0x64, 
  0x65, 0x78, 0x00, 
};

static const u8 data_segment_data_5[] = {
  0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x73, 
  0x79, 0x6d, 0x62, 0x6f, 0x6c, 0x20, 0x61, 0x6c, 0x72, 0x65, 0x61, 0x64, 
  0x79, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x73, 0x00, 
};

static const u8 data_segment_data_6[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x63, 0x72, 0x65, 0x61, 0x74, 
  0x65, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x73, 0x20, 0x69, 0x6e, 
  0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x6f, 0x66, 0x20, 0x61, 0x6e, 
  0x6f, 0x74, 0x68, 0x65, 0x72, 0x20, 0x63, 0x6f, 0x6e, 0x74, 0x72, 0x61, 
  0x63, 0x74, 0x00, 
};

static const u8 data_segment_data_7[] = {
  0x77, 0x72, 0x69, 0x74, 0x65, 0x00, 
};

static const u8 data_segment_data_8[] = {
  0x6d, 0x61, 0x67, 0x6e, 0x69, 0x74, 0x75, 0x64, 0x65, 0x20, 0x6f, 0x66, 
  0x20, 0x61, 0x73, 0x73, 0x65, 0x74, 0x20, 0x61, 0x6d, 0x6f, 0x75, 0x6e, 
  0x74, 0x20, 0x6d, 0x75, 0x73, 0x74, 0x20, 0x62, 0x65, 0x20, 0x6c, 0x65, 
  0x73, 0x73, 0x20, 0x74, 0x68, 0x61, 0x6e, 0x20, 0x32, 0x5e, 0x36, 0x32, 
  0x00, 
};

static const u8 data_segment_data_9[] = {
  0x65, 0x72, 0x72, 0x6f, 0x72, 0x20, 0x72, 0x65, 0x61, 0x64, 0x69, 0x6e, 
  0x67, 0x20, 0x69, 0x74, 0x65, 0x72, 0x61, 0x74, 0x6f, 0x72, 0x00, 
};

static const u8 data_segment_data_10[] = {
  0x72, 0x65, 0x61, 0x64, 0x00, 
};

static const u8 data_segment_data_11[] = {
  0x6d, 0x65, 0x6d, 0x6f, 0x20, 0x68, 0x61, 0x73, 0x20, 0x6d, 0x6f, 0x72, 
  0x65, 0x20, 0x74, 0x68, 0x61, 0x6e, 0x20, 0x32, 0x35, 0x36, 0x20, 0x62, 
  0x79, 0x74, 0x65, 0x73, 0x00, 
};

static const u8 data_segment_data_12[] = {
  0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x73, 
  0x79, 0x6d, 0x62, 0x6f, 0x6c, 0x20, 0x64, 0x6f, 0x65, 0x73, 0x20, 0x6e, 
  0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x2c, 0x20, 0x63, 0x72, 
  0x65, 0x61, 0x74, 0x65, 0x20, 0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x20, 0x62, 
  0x65, 0x66, 0x6f, 0x72, 0x65, 0x20, 0x69, 0x73, 0x73, 0x75, 0x65, 0x00, 
  
};

static const u8 data_segment_data_13[] = {
  0x69, 0x6e, 0x76, 0x61, 0x6c, 0x69, 0x64, 0x20, 0x71, 0x75, 0x61, 0x6e, 
  0x74, 0x69, 0x74, 0x79, 0x00, 
};

static const u8 data_segment_data_14[] = {
  0x6d, 0x75, 0x73, 0x74, 0x20, 0x69, 0x73, 0x73, 0x75, 0x65, 0x20, 0x70, 
  0x6f, 0x73, 0x69, 0x74, 0x69, 0x76, 0x65, 0x20, 0x71, 0x75, 0x61, 0x6e, 
  0x74, 0x69, 0x74, 0x79, 0x00, 
};

static const u8 data_segment_data_15[] = {
  0x73, 0x79, 0x6d, 0x62, 0x6f, 0x6c, 0x20, 0x70, 0x72, 0x65, 0x63, 0x69, 
  0x73, 0x69, 0x6f, 0x6e, 0x20, 0x6d, 0x69, 0x73, 0x6d, 0x61, 0x74, 0x63, 
  0x68, 0x00, 
};

static const u8 data_segment_data_16[] = {
  0x71, 0x75, 0x61, 0x6e, 0x74, 0x69, 0x74, 0x79, 0x20, 0x65, 0x78, 0x63, 
  0x65, 0x65, 0x64, 0x73, 0x20, 0x61, 0x76, 0x61, 0x69, 0x6c, 0x61, 0x62, 
  0x6c, 0x65, 0x20, 0x73, 0x75, 0x70, 0x70, 0x6c, 0x79, 0x00, 
};

static const u8 data_segment_data_17[] = {
  0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x20, 0x70, 0x61, 0x73, 0x73, 0x65, 
  0x64, 0x20, 0x74, 0x6f, 0x20, 0x6d, 0x6f, 0x64, 0x69, 0x66, 0x79, 0x20, 
  0x69, 0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x69, 0x6e, 0x20, 0x6d, 0x75, 
  0x6c, 0x74, 0x69, 0x5f, 0x69, 0x6e, 0x64, 0x65, 0x78, 0x00, 
};

static const u8 data_segment_data_18[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x6d, 0x6f, 0x64, 0x69, 0x66, 
  0x79, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x73, 0x20, 0x69, 0x6e, 
  0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x6f, 0x66, 0x20, 0x61, 0x6e, 
  0x6f, 0x74, 0x68, 0x65, 0x72, 0x20, 0x63, 0x6f, 0x6e, 0x74, 0x72, 0x61, 
  0x63, 0x74, 0x00, 
};

static const u8 data_segment_data_19[] = {
  0x61, 0x74, 0x74, 0x65, 0x6d, 0x70, 0x74, 0x20, 0x74, 0x6f, 0x20, 0x61, 
  0x64, 0x64, 0x20, 0x61, 0x73, 0x73, 0x65, 0x74, 0x20, 0x77, 0x69, 0x74, 
  0x68, 0x20, 0x64, 0x69, 0x66, 0x66, 0x65, 0x72, 0x65, 0x6e, 0x74, 0x20, 
  0x73, 0x79, 0x6d, 0x62, 0x6f, 0x6c, 0x00, 
};

static const u8 data_segment_data_20[] = {
  0x61, 0x64, 0x64, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x75, 0x6e, 0x64, 
  0x65, 0x72, 0x66, 0x6c, 0x6f, 0x77, 0x00, 
};

static const u8 data_segment_data_21[] = {
  0x61, 0x64, 0x64, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x6f, 0x76, 0x65, 
  0x72, 0x66, 0x6c, 0x6f, 0x77, 0x00, 
};

static const u8 data_segment_data_22[] = {
  0x75, 0x70, 0x64, 0x61, 0x74, 0x65, 0x72, 0x20, 0x63, 0x61, 0x6e, 0x6e, 
  0x6f, 0x74, 0x20, 0x63, 0x68, 0x61, 0x6e, 0x67, 0x65, 0x20, 0x70, 0x72, 
  0x69, 0x6d, 0x61, 0x72, 0x79, 0x20, 0x6b, 0x65, 0x79, 0x20, 0x77, 0x68, 
  0x65, 0x6e, 0x20, 0x6d, 0x6f, 0x64, 0x69, 0x66, 0x79, 0x69, 0x6e, 0x67, 
  0x20, 0x61, 0x6e, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x00, 
};

static const u8 data_segment_data_23[] = {
  0x61, 0x63, 0x74, 0x69, 0x76, 0x65, 0x00, 
};

static const u8 data_segment_data_24[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x70, 0x61, 0x73, 0x73, 0x20, 
  0x65, 0x6e, 0x64, 0x20, 0x69, 0x74, 0x65, 0x72, 0x61, 0x74, 0x6f, 0x72, 
  0x20, 0x74, 0x6f, 0x20, 0x6d, 0x6f, 0x64, 0x69, 0x66, 0x79, 0x00, 
};

static const u8 data_segment_data_25[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x74, 0x72, 0x61, 0x6e, 0x73, 
  0x66, 0x65, 0x72, 0x20, 0x74, 0x6f, 0x20, 0x73, 0x65, 0x6c, 0x66, 0x00, 
  
};

static const u8 data_segment_data_26[] = {
  0x74, 0x6f, 0x20, 0x61, 0x63, 0x63, 0x6f, 0x75, 0x6e, 0x74, 0x20, 0x64, 
  0x6f, 0x65, 0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 
  0x74, 0x00, 
};

static const u8 data_segment_data_27[] = {
  0x75, 0x6e, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x74, 0x6f, 0x20, 0x66, 0x69, 
  0x6e, 0x64, 0x20, 0x6b, 0x65, 0x79, 0x00, 
};

static const u8 data_segment_data_28[] = {
  0x6d, 0x75, 0x73, 0x74, 0x20, 0x74, 0x72, 0x61, 0x6e, 0x73, 0x66, 0x65, 
  0x72, 0x20, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x76, 0x65, 0x20, 0x71, 
  0x75, 0x61, 0x6e, 0x74, 0x69, 0x74, 0x79, 0x00, 
};

static const u8 data_segment_data_29[] = {
  0x6e, 0x6f, 0x20, 0x62, 0x61, 0x6c, 0x61, 0x6e, 0x63, 0x65, 0x20, 0x6f, 
  0x62, 0x6a, 0x65, 0x63, 0x74, 0x20, 0x66, 0x6f, 0x75, 0x6e, 0x64, 0x00, 
  
};

static const u8 data_segment_data_30[] = {
  0x6f, 0x76, 0x65, 0x72, 0x64, 0x72, 0x61, 0x77, 0x6e, 0x20, 0x62, 0x61, 
  0x6c, 0x61, 0x6e, 0x63, 0x65, 0x00, 
};

static const u8 data_segment_data_31[] = {
  0x61, 0x74, 0x74, 0x65, 0x6d, 0x70, 0x74, 0x20, 0x74, 0x6f, 0x20, 0x73, 
  0x75, 0x62, 0x74, 0x72, 0x61, 0x63, 0x74, 0x20, 0x61, 0x73, 0x73, 0x65, 
  0x74, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x64, 0x69, 0x66, 0x66, 0x65, 
  0x72, 0x65, 0x6e, 0x74, 0x20, 0x73, 0x79, 0x6d, 0x62, 0x6f, 0x6c, 0x00, 
  
};

static const u8 data_segment_data_32[] = {
  0x73, 0x75, 0x62, 0x74, 0x72, 0x61, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 
  0x75, 0x6e, 0x64, 0x65, 0x72, 0x66, 0x6c, 0x6f, 0x77, 0x00, 
};

static const u8 data_segment_data_33[] = {
  0x73, 0x75, 0x62, 0x74, 0x72, 0x61, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 
  0x6f, 0x76, 0x65, 0x72, 0x66, 0x6c, 0x6f, 0x77, 0x00, 
};

static const u8 data_segment_data_34[] = {
  0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x20, 0x70, 0x61, 0x73, 0x73, 0x65, 
  0x64, 0x20, 0x74, 0x6f, 0x20, 0x65, 0x72, 0x61, 0x73, 0x65, 0x20, 0x69, 
  0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x69, 0x6e, 0x20, 0x6d, 0x75, 0x6c, 
  0x74, 0x69, 0x5f, 0x69, 0x6e, 0x64, 0x65, 0x78, 0x00, 
};

static const u8 data_segment_data_35[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x72, 0x61, 0x73, 0x65, 
  0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x73, 0x20, 0x69, 0x6e, 0x20, 
  0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x6f, 0x66, 0x20, 0x61, 0x6e, 0x6f, 
  0x74, 0x68, 0x65, 0x72, 0x20, 0x63, 0x6f, 0x6e, 0x74, 0x72, 0x61, 0x63, 
  0x74, 0x00, 
};

static const u8 data_segment_data_36[] = {
  0x61, 0x74, 0x74, 0x65, 0x6d, 0x70, 0x74, 0x20, 0x74, 0x6f, 0x20, 0x72, 
  0x65, 0x6d, 0x6f, 0x76, 0x65, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 
  0x20, 0x74, 0x68, 0x61, 0x74, 0x20, 0x77, 0x61, 0x73, 0x20, 0x6e, 0x6f, 
  0x74, 0x20, 0x69, 0x6e, 0x20, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x5f, 0x69, 
  0x6e, 0x64, 0x65, 0x78, 0x00, 
};

static const u8 data_segment_data_37[] = {
  0x6f, 0x6e, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x00, 
};

static const u8 data_segment_data_38[] = {
  0x65, 0x6f, 0x73, 0x69, 0x6f, 0x00, 
};

static const u8 data_segment_data_39[] = {
  0x6f, 0x6e, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x20, 0x61, 0x63, 0x74, 0x69, 
  0x6f, 0x6e, 0x27, 0x73, 0x20, 0x61, 0x72, 0x65, 0x20, 0x6f, 0x6e, 0x6c, 
  0x79, 0x20, 0x76, 0x61, 0x6c, 0x69, 0x64, 0x20, 0x66, 0x72, 0x6f, 0x6d, 
  0x20, 0x74, 0x68, 0x65, 0x20, 0x22, 0x65, 0x6f, 0x73, 0x69, 0x6f, 0x22, 
  0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x20, 0x61, 0x63, 0x63, 0x6f, 
  0x75, 0x6e, 0x74, 0x00, 
};

static const u8 data_segment_data_40[] = {
  0x67, 0x65, 0x74, 0x00, 
};

static const u8 data_segment_data_41[] = {
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
  memcpy(&(memory.data[16u]), data_segment_data_1, 20);
  memcpy(&(memory.data[48u]), data_segment_data_2, 15);
  memcpy(&(memory.data[64u]), data_segment_data_3, 28);
  memcpy(&(memory.data[96u]), data_segment_data_4, 51);
  memcpy(&(memory.data[160u]), data_segment_data_5, 33);
  memcpy(&(memory.data[208u]), data_segment_data_6, 51);
  memcpy(&(memory.data[272u]), data_segment_data_7, 6);
  memcpy(&(memory.data[288u]), data_segment_data_8, 49);
  memcpy(&(memory.data[352u]), data_segment_data_9, 23);
  memcpy(&(memory.data[384u]), data_segment_data_10, 5);
  memcpy(&(memory.data[400u]), data_segment_data_11, 29);
  memcpy(&(memory.data[432u]), data_segment_data_12, 60);
  memcpy(&(memory.data[496u]), data_segment_data_13, 17);
  memcpy(&(memory.data[528u]), data_segment_data_14, 29);
  memcpy(&(memory.data[560u]), data_segment_data_15, 26);
  memcpy(&(memory.data[592u]), data_segment_data_16, 34);
  memcpy(&(memory.data[640u]), data_segment_data_17, 46);
  memcpy(&(memory.data[688u]), data_segment_data_18, 51);
  memcpy(&(memory.data[752u]), data_segment_data_19, 43);
  memcpy(&(memory.data[800u]), data_segment_data_20, 19);
  memcpy(&(memory.data[832u]), data_segment_data_21, 18);
  memcpy(&(memory.data[864u]), data_segment_data_22, 59);
  memcpy(&(memory.data[928u]), data_segment_data_23, 7);
  memcpy(&(memory.data[944u]), data_segment_data_24, 35);
  memcpy(&(memory.data[992u]), data_segment_data_25, 24);
  memcpy(&(memory.data[1024u]), data_segment_data_26, 26);
  memcpy(&(memory.data[1056u]), data_segment_data_27, 19);
  memcpy(&(memory.data[1088u]), data_segment_data_28, 32);
  memcpy(&(memory.data[1120u]), data_segment_data_29, 24);
  memcpy(&(memory.data[1152u]), data_segment_data_30, 18);
  memcpy(&(memory.data[1184u]), data_segment_data_31, 48);
  memcpy(&(memory.data[1232u]), data_segment_data_32, 22);
  memcpy(&(memory.data[1264u]), data_segment_data_33, 21);
  memcpy(&(memory.data[1296u]), data_segment_data_34, 45);
  memcpy(&(memory.data[1344u]), data_segment_data_35, 50);
  memcpy(&(memory.data[1408u]), data_segment_data_36, 53);
  memcpy(&(memory.data[1472u]), data_segment_data_37, 8);
  memcpy(&(memory.data[1488u]), data_segment_data_38, 6);
  memcpy(&(memory.data[1504u]), data_segment_data_39, 64);
  memcpy(&(memory.data[1568u]), data_segment_data_40, 4);
  memcpy(&(memory.data[9984u]), data_segment_data_41, 86);
}

static void init_table(void) {
  uint32_t offset;
  wasm_rt_allocate_table((&T0), 4, 4);
  offset = 0u;
  T0.data[offset + 0] = (wasm_rt_elem_t){func_types[3], (wasm_rt_anyfunc_t)(&f66)};
  T0.data[offset + 1] = (wasm_rt_elem_t){func_types[0], (wasm_rt_anyfunc_t)(&_ZN5eosio5token6createEyNS_5assetE)};
  T0.data[offset + 2] = (wasm_rt_elem_t){func_types[1], (wasm_rt_anyfunc_t)(&_ZN5eosio5token8transferEyyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEE)};
  T0.data[offset + 3] = (wasm_rt_elem_t){func_types[2], (wasm_rt_anyfunc_t)(&_ZN5eosio5token5issueEyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEE)};
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
/* export: '_ZN5eosio5token6createEyNS_5assetE' */
void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio5token6createEyNS_5assetEZ_viji))(u32, u64, u32);
/* export: '_ZN5eosio5token5issueEyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEE' */
void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio5token5issueEyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEEZ_vijii))(u32, u64, u32, u32);
/* export: '_ZN5eosio5token11add_balanceEyNS_5assetEy' */
void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio5token11add_balanceEyNS_5assetEyZ_vijij))(u32, u64, u32, u64);
/* export: '_ZN5eosio5token8transferEyyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEE' */
void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio5token8transferEyyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEEZ_vijjii))(u32, u64, u64, u32, u32);
/* export: '_ZN5eosio5token11sub_balanceEyNS_5assetE' */
void (*WASM_RT_ADD_PREFIX(Z__Z5AN5eosio5token11sub_balanceEyNS_5assetEZ_viji))(u32, u64, u32);
/* export: 'apply' */
void (*WASM_RT_ADD_PREFIX(apply_eosiochaince_3e0cf4172ab025f9fff5f1db11ee8a34d44779492e1d668ae1dc2d129e865348_1488192))(u64, u64, u64);
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
  /* export: '_ZN5eosio5token6createEyNS_5assetE' */
  WASM_RT_ADD_PREFIX(Z__Z5AN5eosio5token6createEyNS_5assetEZ_viji) = (&_ZN5eosio5token6createEyNS_5assetE);
  /* export: '_ZN5eosio5token5issueEyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEE' */
  WASM_RT_ADD_PREFIX(Z__Z5AN5eosio5token5issueEyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEEZ_vijii) = (&_ZN5eosio5token5issueEyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEE);
  /* export: '_ZN5eosio5token11add_balanceEyNS_5assetEy' */
  WASM_RT_ADD_PREFIX(Z__Z5AN5eosio5token11add_balanceEyNS_5assetEyZ_vijij) = (&_ZN5eosio5token11add_balanceEyNS_5assetEy);
  /* export: '_ZN5eosio5token8transferEyyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEE' */
  WASM_RT_ADD_PREFIX(Z__Z5AN5eosio5token8transferEyyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEEZ_vijjii) = (&_ZN5eosio5token8transferEyyNS_5assetENSt3__112basic_stringIcNS2_11char_traitsIcEENS2_9allocatorIcEEEE);
  /* export: '_ZN5eosio5token11sub_balanceEyNS_5assetE' */
  WASM_RT_ADD_PREFIX(Z__Z5AN5eosio5token11sub_balanceEyNS_5assetEZ_viji) = (&_ZN5eosio5token11sub_balanceEyNS_5assetE);
  /* export: 'apply' */
  WASM_RT_ADD_PREFIX(apply_eosiochaince_3e0cf4172ab025f9fff5f1db11ee8a34d44779492e1d668ae1dc2d129e865348_1488192) = (&apply);
  /* export: 'memcmp' */
  WASM_RT_ADD_PREFIX(Z_memcmpZ_iiii) = (&memcmp_0);
  /* export: 'malloc' */
  WASM_RT_ADD_PREFIX(Z_mallocZ_ii) = (&malloc_0);
  /* export: 'free' */
  WASM_RT_ADD_PREFIX(Z_freeZ_vi) = (&free_0);
}

void WASM_RT_ADD_PREFIX(init_eosiochaince_3e0cf4172ab025f9fff5f1db11ee8a34d44779492e1d668ae1dc2d129e865348_1488192)(void) {
  init_func_types();
  init_globals();
  init_memory();
  init_table();
  init_exports();
}

wasm_rt_memory_t* get_memory_eosiochaince_3e0cf4172ab025f9fff5f1db11ee8a34d44779492e1d668ae1dc2d129e865348_1488192() {
    return &memory;
}
        