#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "contracts/eosio-5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59-11999.h"
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


static u32 func_types[18];

static void init_func_types(void) {
  func_types[0] = wasm_rt_register_func_type(3, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32);
  func_types[1] = wasm_rt_register_func_type(5, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[2] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[3] = wasm_rt_register_func_type(2, 0, WASM_RT_I32, WASM_RT_I32);
  func_types[4] = wasm_rt_register_func_type(2, 0, WASM_RT_I32, WASM_RT_I64);
  func_types[5] = wasm_rt_register_func_type(0, 0);
  func_types[6] = wasm_rt_register_func_type(0, 1, WASM_RT_I64);
  func_types[7] = wasm_rt_register_func_type(2, 0, WASM_RT_I64, WASM_RT_I64);
  func_types[8] = wasm_rt_register_func_type(0, 1, WASM_RT_I32);
  func_types[9] = wasm_rt_register_func_type(2, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[10] = wasm_rt_register_func_type(3, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[11] = wasm_rt_register_func_type(1, 0, WASM_RT_I64);
  func_types[12] = wasm_rt_register_func_type(2, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64);
  func_types[13] = wasm_rt_register_func_type(4, 0, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[14] = wasm_rt_register_func_type(2, 0, WASM_RT_I64, WASM_RT_I32);
  func_types[15] = wasm_rt_register_func_type(1, 0, WASM_RT_I32);
  func_types[16] = wasm_rt_register_func_type(3, 0, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[17] = wasm_rt_register_func_type(1, 1, WASM_RT_I32, WASM_RT_I32);
}

static u32 _ZeqRK11checksum256S1_(u32, u32);
static u32 _ZeqRK11checksum160S1_(u32, u32);
static u32 _ZneRK11checksum160S1_(u32, u32);
static u32 now(void);
static void _ZN5eosio12require_authERKNS_16permission_levelE(u32);
static void apply(u64, u64, u64);
static void f20(u32, u64, u32);
static u32 f21(u32, u32);
static void f22(u32, u64, u64, u64, u64);
static u32 f23(u32, u32);
static void f24(u32, u64, u64, u64);
static u32 f25(u32, u32);
static void f26(u32, u32);
static u32 f27(u32, u32);
static void f28(u32, u64);
static u32 f29(u32, u32);
static u32 f30(u32, u32);
static void f31(u32, u32);
static void f32_0(u32, u32);
static u32 f33(u32);
static void f34(u32);
static void f35(u32);
static u32 memcmp_0(u32, u32, u32);
static u32 malloc_0(u32);
static u32 f38(u32, u32);
static u32 f39(u32);
static void free_0(u32);
static void f41(void);

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
  i0 = 16u;
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
  i0 = 32u;
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
  B6:;
  j0 = p1;
  j1 = p0;
  i0 = j0 == j1;
  if (i0) {goto B15;}
  j0 = 0ull;
  l3 = j0;
  j0 = 59ull;
  l2 = j0;
  i0 = 16u;
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
  j1 = 14029496491724111871ull;
  i0 = (u64)((s64)j0 <= (s64)j1);
  if (i0) {goto B25;}
  j0 = p2;
  j1 = 14029496491724111872ull;
  i0 = j0 == j1;
  if (i0) {goto B24;}
  j0 = p2;
  j1 = 14029658218335043584ull;
  i0 = j0 == j1;
  if (i0) {goto B23;}
  j0 = p2;
  j1 = 14029658124516851712ull;
  i0 = j0 != j1;
  if (i0) {goto B14;}
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 84), i1);
  i0 = l6;
  i1 = 1u;
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
  i0 = f21(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B14;
  B25:;
  j0 = p2;
  j1 = 13451246470292307968ull;
  i0 = j0 == j1;
  if (i0) {goto B22;}
  j0 = p2;
  j1 = 14029390938607845376ull;
  i0 = j0 != j1;
  if (i0) {goto B14;}
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 76), i1);
  i0 = l6;
  i1 = 2u;
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
  i0 = f23(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B14;
  B24:;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 68), i1);
  i0 = l6;
  i1 = 3u;
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
  i0 = f25(i0, i1);
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
  i0 = f27(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B14;
  B22:;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 52), i1);
  i0 = l6;
  i1 = 5u;
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
  i0 = f29(i0, i1);
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

static void f20(u32 p0, u64 p1, u32 p2) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0;
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
  (*Z_envZ_require_authZ_vj)(j0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = p1;
  i1 = p2;
  i2 = 0u;
  i1 = i1 != i2;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  (*Z_envZ_set_privilegedZ_vji)(j0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static u32 f21(u32 p0, u32 p1) {
  u32 l0 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  u64 l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 16u;
  i0 -= i1;
  l5 = i0;
  l4 = i0;
  i0 = 0u;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i0 = 0u;
  l2 = i0;
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
  if (i0) {goto B0;}
  i0 = p1;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  l2 = i0;
  goto B2;
  B3:;
  i0 = 0u;
  i1 = l5;
  i2 = p1;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B2:;
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
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l4;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0 + 8), i1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 112u;
  i2 = g0;
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
  i0 = p1;
  i1 = 8u;
  i0 = i0 != i1;
  i1 = 112u;
  i2 = g0;
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
  i1 = 8u;
  i0 += i1;
  l5 = i0;
  i1 = l2;
  i2 = 8u;
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
  i0 = p1;
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
  i0 = p0;
  i1 = l0;
  i2 = 1u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  i0 += i1;
  p1 = i0;
  i0 = l5;
  i0 = i32_load8_u((&memory), (u64)(i0));
  l2 = i0;
  i0 = l4;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  i0 = l0;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B12;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l3;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  B12:;
  i0 = p1;
  j1 = l1;
  i2 = l2;
  i3 = 255u;
  i2 &= i3;
  i3 = l3;
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
  i1 = l4;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 1u;
  FUNC_EPILOGUE;
  return i0;
}

static void f22(u32 p0, u64 p1, u64 p2, u64 p3, u64 p4) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i4, i5;
  u64 j0, j1, j2, j3;
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
  (*Z_envZ_require_authZ_vj)(j0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = p1;
  j1 = p2;
  j2 = p3;
  j3 = p4;
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
  (*Z_envZ_set_resource_limitsZ_vjjjj)(j0, j1, j2, j3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static u32 f23(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l6 = 0, l7 = 0, l8 = 0;
  u64 l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i5, i6, i7;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 64u;
  i0 -= i1;
  l8 = i0;
  l7 = i0;
  i0 = 0u;
  i1 = l8;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
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
  i1 = l8;
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
  i0 = l7;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l7;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l7;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l7;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l7;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l7;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l7;
  i1 = p1;
  i2 = l1;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l7;
  i1 = l7;
  i2 = 32u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l7;
  i1 = l7;
  i32_store((&memory), (u64)(i0 + 56), i1);
  i0 = l7;
  i1 = 56u;
  i0 += i1;
  i1 = l7;
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
  f32_0(i0, i1);
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
  i0 = p0;
  i1 = l0;
  i2 = 1u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  i0 += i1;
  p1 = i0;
  i0 = l7;
  i1 = 24u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  l5 = j0;
  i0 = l7;
  i1 = 16u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  l4 = j0;
  i0 = l7;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  l3 = j0;
  i0 = l7;
  j0 = i64_load((&memory), (u64)(i0));
  l2 = j0;
  i0 = l0;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B10;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l6;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  B10:;
  i0 = p1;
  j1 = l2;
  j2 = l3;
  j3 = l4;
  j4 = l5;
  i5 = l6;
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
  CALL_INDIRECT(T0, void (*)(u32, u64, u64, u64, u64), 1, i5, i0, j1, j2, j3, j4);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = l7;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 1u;
  FUNC_EPILOGUE;
  return i0;
}

static void f24(u32 p0, u64 p1, u64 p2, u64 p3) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  u64 j0;
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
  (*Z_envZ_require_authZ_vj)(j0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static u32 f25(u32 p0, u32 p1) {
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
  i1 = 112u;
  i2 = g0;
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
  i1 = 112u;
  i2 = g0;
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
  i1 = 112u;
  i2 = g0;
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

static void f26(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l0 = i0;
  l1 = i0;
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
  p0 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B2;}
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
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B1;
  B2:;
  i0 = 0u;
  i1 = l0;
  i2 = p0;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B1:;
  i0 = l0;
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
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
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
  j0 = (*Z_envZ_set_proposed_producersZ_jii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = l1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f27(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 48u;
  i0 -= i1;
  l4 = i0;
  l3 = i0;
  i0 = 0u;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i0 = 0u;
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
  p1 = i0;
  goto B2;
  B3:;
  i0 = 0u;
  i1 = l4;
  i2 = l1;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  p1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B2:;
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
  i0 = l3;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l3;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l3;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l3;
  i1 = p1;
  i2 = l1;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l3;
  i1 = 32u;
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
  i0 = f30(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
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
  B7:;
  i0 = l3;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = 0u;
  p1 = i0;
  i0 = l3;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l3;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = l3;
  i1 = i32_load((&memory), (u64)(i1));
  i0 -= i1;
  l1 = i0;
  i1 = 48u;
  i0 = I32_DIV_S(i0, i1);
  l4 = i0;
  i0 = l1;
  i0 = !(i0);
  if (i0) {goto B11;}
  i0 = l4;
  i1 = 89478486u;
  i0 = i0 >= i1;
  if (i0) {goto B10;}
  i0 = l3;
  i1 = 24u;
  i0 += i1;
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
  i1 = f33(i1);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i1;
  i2 = l4;
  i3 = 48u;
  i2 *= i3;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l3;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l3;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = l3;
  i1 = i32_load((&memory), (u64)(i1));
  l4 = i1;
  i0 -= i1;
  l1 = i0;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B11;}
  i0 = p1;
  i1 = l4;
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
  i0 = l3;
  i1 = l3;
  i1 = i32_load((&memory), (u64)(i1 + 20));
  i2 = l1;
  i3 = 48u;
  i2 = DIV_U(i2, i3);
  i3 = 48u;
  i2 *= i3;
  i1 += i2;
  p1 = i1;
  i32_store((&memory), (u64)(i0 + 20), i1);
  B11:;
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
  if (i0) {goto B14;}
  i0 = l1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l2;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  B14:;
  i0 = l3;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l3;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = p1;
  i1 = l3;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  i0 -= i1;
  p1 = i0;
  i1 = 48u;
  i0 = I32_DIV_S(i0, i1);
  l0 = i0;
  i0 = p1;
  i0 = !(i0);
  if (i0) {goto B15;}
  i0 = l0;
  i1 = 89478486u;
  i0 = i0 >= i1;
  if (i0) {goto B9;}
  i0 = l3;
  i1 = 40u;
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
  i1 = f33(i1);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i1;
  i2 = l0;
  i3 = 48u;
  i2 *= i3;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l3;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l3;
  i0 = i32_load((&memory), (u64)(i0 + 20));
  i1 = l3;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  l4 = i1;
  i0 -= i1;
  l0 = i0;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B15;}
  i0 = p1;
  i1 = l4;
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
  i0 = l3;
  i1 = p1;
  i2 = l0;
  i3 = 48u;
  i2 = DIV_U(i2, i3);
  i3 = 48u;
  i2 *= i3;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 36), i1);
  B15:;
  i0 = l1;
  i1 = l3;
  i2 = 32u;
  i1 += i2;
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
  CALL_INDIRECT(T0, void (*)(u32, u32), 3, i2, i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i0 = i32_load((&memory), (u64)(i0 + 32));
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B19;}
  i0 = l3;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 36), i1);
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
  f34(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B19:;
  i0 = l3;
  i0 = i32_load((&memory), (u64)(i0 + 16));
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B21;}
  i0 = l3;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 20), i1);
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
  f34(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B21:;
  i0 = l3;
  i0 = i32_load((&memory), (u64)(i0));
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B23;}
  i0 = l3;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 4), i1);
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
  f34(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B23:;
  i0 = 0u;
  i1 = l3;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 1u;
  goto Bfunc;
  B10:;
  i0 = l3;
  i1 = 16u;
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
  f35(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B9:;
  i0 = l3;
  i1 = 32u;
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
  f35(i0);
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

static void f28(u32 p0, u64 p1) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  u64 j0;
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
  FUNC_EPILOGUE;
}

static u32 f29(u32 p0, u32 p1) {
  u32 l0 = 0, l2 = 0, l3 = 0, l4 = 0;
  u64 l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 16u;
  i0 -= i1;
  l2 = i0;
  l4 = i0;
  i0 = 0u;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
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
  l2 = i0;
  goto B1;
  B3:;
  i0 = 0u;
  l2 = i0;
  goto B0;
  B2:;
  i0 = 0u;
  i1 = l2;
  i2 = p1;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B1:;
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
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 112u;
  i2 = g0;
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
  i0 = l4;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l1 = j0;
  i0 = p1;
  i1 = 513u;
  i0 = i0 < i1;
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
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B9:;
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
  if (i0) {goto B11;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l3;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  B11:;
  i0 = p1;
  j1 = l1;
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
  CALL_INDIRECT(T0, void (*)(u32, u64), 4, i2, i0, j1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = l4;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 1u;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f30(u32 p0, u32 p1) {
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
    i1 = 128u;
    i2 = g0;
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
  i2 = 48u;
  i1 = I32_DIV_S(i1, i2);
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
  f31(i0, i1);
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
  i3 = 48u;
  i2 *= i3;
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
    i1 = 112u;
    i2 = g0;
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
    i1 = 33u;
    i0 = i0 > i1;
    i1 = 112u;
    i2 = g0;
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
    i0 = l2;
    i1 = l2;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 34u;
    i1 += i2;
    l4 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l5;
    i1 = 48u;
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

static void f31(u32 p0, u32 p1) {
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
  i1 = 48u;
  i0 = I32_DIV_S(i0, i1);
  i1 = p1;
  i0 = i0 >= i1;
  if (i0) {goto B4;}
  i0 = l4;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  l3 = i1;
  i0 -= i1;
  i1 = 48u;
  i0 = I32_DIV_S(i0, i1);
  l1 = i0;
  i1 = p1;
  i0 += i1;
  l2 = i0;
  i1 = 89478486u;
  i0 = i0 >= i1;
  if (i0) {goto B2;}
  i0 = 89478485u;
  l4 = i0;
  i0 = l0;
  i1 = l3;
  i0 -= i1;
  i1 = 48u;
  i0 = I32_DIV_S(i0, i1);
  l0 = i0;
  i1 = 44739241u;
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
  i1 = 48u;
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
  i0 = f33(i0);
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
    i2 = 48u;
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
    i0 = (*Z_envZ_memsetZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = p0;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 48u;
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
  f35(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B1:;
  i0 = l0;
  i1 = l4;
  i2 = 48u;
  i1 *= i2;
  i0 += i1;
  l2 = i0;
  i0 = l0;
  i1 = l1;
  i2 = 48u;
  i1 *= i2;
  i0 += i1;
  l3 = i0;
  l4 = i0;
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 0u;
    i2 = 48u;
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
    i0 = (*Z_envZ_memsetZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i1 = 48u;
    i0 += i1;
    l4 = i0;
    i0 = p1;
    i1 = 4294967295u;
    i0 += i1;
    p1 = i0;
    if (i0) {goto L10;}
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
  i2 = 4294967248u;
  i1 = I32_DIV_S(i1, i2);
  i2 = 48u;
  i1 *= i2;
  i0 += i1;
  l3 = i0;
  i0 = l0;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B12;}
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
  B12:;
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
  f34(i0);
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

static void f32_0(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0;
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
  i1 = 112u;
  i2 = g0;
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
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
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
  i1 = 112u;
  i2 = g0;
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
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 112u;
  i2 = g0;
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
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  p1 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 112u;
  i2 = g0;
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
  i1 = p1;
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
  i0 = p1;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f33(u32 p0) {
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
    i0 = i32_load((&memory), (u64)(i0 + 132));
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
    CALL_INDIRECT(T0, void (*)(void), 5, i0);
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

static void f34(u32 p0) {
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

static void f35(u32 p0) {
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
  i0 = 136u;
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
  i0 = f38(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
  return i0;
}

static u32 f38(u32 p0, u32 p1) {
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
    i0 = f39(i0);
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
    i1 = 8544u;
    i2 = g0;
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

static u32 f39(u32 p0) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8388));
  l0 = i0;
  i0 = 0u;
  i0 = i32_load8_u((&memory), (u64)(i0 + 8630));
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 8632));
  l6 = i0;
  goto B0;
  B1:;
  i0 = memory.pages;
  l6 = i0;
  i0 = 0u;
  i1 = 1u;
  i32_store8((&memory), (u64)(i0 + 8630), i1);
  i0 = 0u;
  i1 = l6;
  i2 = 16u;
  i1 <<= (i2 & 31);
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 8632), i1);
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
  i0 = i32_load((&memory), (u64)(i0 + 8632));
  l2 = i0;
  B5:;
  i0 = 0u;
  l7 = i0;
  i0 = 0u;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 8632), i1);
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
  i0 = i32_load8_u((&memory), (u64)(i0 + 8630));
  if (i0) {goto B6;}
  i0 = memory.pages;
  l2 = i0;
  i0 = 0u;
  i1 = 1u;
  i32_store8((&memory), (u64)(i0 + 8630), i1);
  i0 = 0u;
  i1 = l2;
  i2 = 16u;
  i1 <<= (i2 & 31);
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 8632), i1);
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
  i0 = i32_load((&memory), (u64)(i0 + 8632));
  l5 = i0;
  B7:;
  i0 = 0u;
  i1 = l5;
  i2 = l4;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8632), i1);
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
  i0 = i32_load((&memory), (u64)(i0 + 8520));
  l1 = i0;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B1;}
  i0 = 8328u;
  l2 = i0;
  i0 = l1;
  i1 = 12u;
  i0 *= i1;
  i1 = 8328u;
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

static void f41(void) {
  FUNC_PROLOGUE;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  FUNC_EPILOGUE;
}

static const u8 data_segment_data_0[] = {
  0xc0, 0x49, 0x00, 0x00, 
};

static const u8 data_segment_data_1[] = {
  0x6f, 0x6e, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x00, 
};

static const u8 data_segment_data_2[] = {
  0x65, 0x6f, 0x73, 0x69, 0x6f, 0x00, 
};

static const u8 data_segment_data_3[] = {
  0x6f, 0x6e, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x20, 0x61, 0x63, 0x74, 0x69, 
  0x6f, 0x6e, 0x27, 0x73, 0x20, 0x61, 0x72, 0x65, 0x20, 0x6f, 0x6e, 0x6c, 
  0x79, 0x20, 0x76, 0x61, 0x6c, 0x69, 0x64, 0x20, 0x66, 0x72, 0x6f, 0x6d, 
  0x20, 0x74, 0x68, 0x65, 0x20, 0x22, 0x65, 0x6f, 0x73, 0x69, 0x6f, 0x22, 
  0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x20, 0x61, 0x63, 0x63, 0x6f, 
  0x75, 0x6e, 0x74, 0x00, 
};

static const u8 data_segment_data_4[] = {
  0x72, 0x65, 0x61, 0x64, 0x00, 
};

static const u8 data_segment_data_5[] = {
  0x67, 0x65, 0x74, 0x00, 
};

static const u8 data_segment_data_6[] = {
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
  memcpy(&(memory.data[16u]), data_segment_data_1, 8);
  memcpy(&(memory.data[32u]), data_segment_data_2, 6);
  memcpy(&(memory.data[48u]), data_segment_data_3, 64);
  memcpy(&(memory.data[112u]), data_segment_data_4, 5);
  memcpy(&(memory.data[128u]), data_segment_data_5, 4);
  memcpy(&(memory.data[8544u]), data_segment_data_6, 86);
}

static void init_table(void) {
  uint32_t offset;
  wasm_rt_allocate_table((&T0), 6, 6);
  offset = 0u;
  T0.data[offset + 0] = (wasm_rt_elem_t){func_types[5], (wasm_rt_anyfunc_t)(&f41)};
  T0.data[offset + 1] = (wasm_rt_elem_t){func_types[0], (wasm_rt_anyfunc_t)(&f20)};
  T0.data[offset + 2] = (wasm_rt_elem_t){func_types[1], (wasm_rt_anyfunc_t)(&f22)};
  T0.data[offset + 3] = (wasm_rt_elem_t){func_types[2], (wasm_rt_anyfunc_t)(&f24)};
  T0.data[offset + 4] = (wasm_rt_elem_t){func_types[3], (wasm_rt_anyfunc_t)(&f26)};
  T0.data[offset + 5] = (wasm_rt_elem_t){func_types[4], (wasm_rt_anyfunc_t)(&f28)};
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
/* export: 'apply' */
void (*WASM_RT_ADD_PREFIX(apply_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_11999))(u64, u64, u64);
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
  /* export: 'apply' */
  WASM_RT_ADD_PREFIX(apply_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_11999) = (&apply);
  /* export: 'memcmp' */
  WASM_RT_ADD_PREFIX(Z_memcmpZ_iiii) = (&memcmp_0);
  /* export: 'malloc' */
  WASM_RT_ADD_PREFIX(Z_mallocZ_ii) = (&malloc_0);
  /* export: 'free' */
  WASM_RT_ADD_PREFIX(Z_freeZ_vi) = (&free_0);
}

void WASM_RT_ADD_PREFIX(init_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_11999)(void) {
  init_func_types();
  init_globals();
  init_memory();
  init_table();
  init_exports();
}

wasm_rt_memory_t* get_memory_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_11999() {
    return &memory;
}
        