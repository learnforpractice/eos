#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "contracts/eosio.token-e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb-0.h"
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


static u32 func_types[28];

static void init_func_types(void) {
  func_types[0] = wasm_rt_register_func_type(0, 0);
  func_types[1] = wasm_rt_register_func_type(3, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[2] = wasm_rt_register_func_type(5, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[3] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[4] = wasm_rt_register_func_type(1, 0, WASM_RT_I64);
  func_types[5] = wasm_rt_register_func_type(2, 0, WASM_RT_I32, WASM_RT_I32);
  func_types[6] = wasm_rt_register_func_type(4, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32);
  func_types[7] = wasm_rt_register_func_type(0, 1, WASM_RT_I64);
  func_types[8] = wasm_rt_register_func_type(3, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[9] = wasm_rt_register_func_type(6, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[10] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[11] = wasm_rt_register_func_type(1, 1, WASM_RT_I64, WASM_RT_I32);
  func_types[12] = wasm_rt_register_func_type(2, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[13] = wasm_rt_register_func_type(2, 0, WASM_RT_I32, WASM_RT_F32);
  func_types[14] = wasm_rt_register_func_type(5, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[15] = wasm_rt_register_func_type(2, 0, WASM_RT_I32, WASM_RT_F64);
  func_types[16] = wasm_rt_register_func_type(2, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_F64);
  func_types[17] = wasm_rt_register_func_type(2, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_F32);
  func_types[18] = wasm_rt_register_func_type(2, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32);
  func_types[19] = wasm_rt_register_func_type(0, 1, WASM_RT_I32);
  func_types[20] = wasm_rt_register_func_type(2, 0, WASM_RT_I32, WASM_RT_I64);
  func_types[21] = wasm_rt_register_func_type(1, 0, WASM_RT_I32);
  func_types[22] = wasm_rt_register_func_type(1, 1, WASM_RT_I32, WASM_RT_I32);
  func_types[23] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32);
  func_types[24] = wasm_rt_register_func_type(3, 1, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[25] = wasm_rt_register_func_type(3, 0, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[26] = wasm_rt_register_func_type(3, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[27] = wasm_rt_register_func_type(2, 0, WASM_RT_I64, WASM_RT_I64);
}

static void f39(void);
static u32 f40(u32);
static void f41(u32);
static void f42(void);
static u32 f43(void);
static u32 f44(u32, u32);
static u32 f45(u32, u32, u32);
static u32 f46(u32);
static u32 f47(u32);
static void f48(u32);
static void f49(u32);
static u32 f50(u32, u32);
static u32 f51(u32, u32);
static void f52(u32, u32);
static void f53(u32, u32);
static void f54(u32);
static u32 f55(u32, u32);
static void f56(u32, u32);
static void f57(u32);
static void f58(u32);
static void f59(u32, u32, u32);
static u32 f60(u32, u32);
static void f61(u32, u32);
static void f62(u32, u32, u32, u32);
static void f63(u32, u32, u32, u32);
static void f64_0(u32, u32, u32);
static void f65(u32, u32, u32, u32);
static u32 f66(u32, u32);
static void f67(u32, u32, u32);
static void f68(u32, u32, u32, u32);
static void f69(u32, u32, u32);
static void f70(u32, u32, u32);
static void f71(u32, u32, u32);
static void f72(u32, u32, u64, u32);
static void f73(u32, u32, u32, u32, u32);
static u32 f74(u32, u64, u32);
static void f75(u32, u32, u32, u32);
static void f76(u32, u32);
static void f77(u32, u32, u32);
static void f78(u32, u32);
static void apply(u64, u64, u64);
static u32 f80(u64, u64, u32);
static u32 f81(u64, u64, u32);
static u32 f82(u64, u64, u32);
static u32 f83(u64, u64, u32);
static u32 f84(u64, u64, u32);
static void f85(u32, u32);
static u32 f86(u32, u32);
static void f87(u32, u32);
static void f88(u32, u32);
static void f89(u32, u32);
static void f90(u64, u64);
static void f91(u64, u64);
static u32 f92(u32, u32);
static void f93(u64, u64);
static void f94(u64, u64);
static void f95(u64, u64);
static void f96(u64, u64);
static void f97(u32, u32);
static void f98(u32, u32);

static u32 g0;
static u32 g1;
static u32 g2;
static u32 g3;

static void init_globals(void) {
  g0 = 8192u;
  g1 = 9638u;
  g2 = 9638u;
  g3 = 250u;
}

static wasm_rt_memory_t M0;

static wasm_rt_table_t T0;

static void f39(void) {
  FUNC_PROLOGUE;
  u32 i0, i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  f42();
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static u32 f40(u32 p0) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&M0), (u64)(i1 + 8204));
  i2 = p0;
  i3 = 16u;
  i2 >>= (i3 & 31);
  l0 = i2;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0 + 8204), i1);
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&M0), (u64)(i1 + 8196));
  l2 = i1;
  i2 = p0;
  i1 += i2;
  i2 = 7u;
  i1 += i2;
  i2 = 4294967288u;
  i1 &= i2;
  p0 = i1;
  i32_store((&M0), (u64)(i0 + 8196), i1);
  i0 = l1;
  i1 = 16u;
  i0 <<= (i1 & 31);
  i1 = p0;
  i0 = i0 <= i1;
  if (i0) {goto B2;}
  i0 = l0;
  i0 = wasm_rt_grow_memory((&M0), i0);
  i1 = 4294967295u;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  goto B0;
  B3:;
  i0 = 0u;
  goto Bfunc;
  B2:;
  i0 = 0u;
  i1 = l1;
  i2 = 1u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8204), i1);
  i0 = l0;
  i1 = 1u;
  i0 += i1;
  i0 = wasm_rt_grow_memory((&M0), i0);
  i1 = 4294967295u;
  i0 = i0 != i1;
  if (i0) {goto B0;}
  B1:;
  i0 = 0u;
  i1 = 8220u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  goto Bfunc;
  B0:;
  i0 = l2;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static void f41(u32 p0) {
  FUNC_PROLOGUE;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static void f42(void) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = 0u;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1 + 12));
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 7u;
  i1 += i2;
  i2 = 4294967288u;
  i1 &= i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0 + 8196), i1);
  i0 = 0u;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 8192), i1);
  i0 = 0u;
  i1 = M0.pages;
  i32_store((&M0), (u64)(i0 + 8204), i1);
  FUNC_EPILOGUE;
}

static u32 f43(void) {
  FUNC_PROLOGUE;
  u32 i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 8208u;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f44(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  l0 = i0;
  i0 = 0u;
  i1 = p0;
  i0 -= i1;
  l1 = i0;
  i1 = p0;
  i0 &= i1;
  i1 = p0;
  i0 = i0 != i1;
  if (i0) {goto B1;}
  i0 = p0;
  i1 = 16u;
  i0 = i0 > i1;
  if (i0) {goto B0;}
  i0 = p1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto Bfunc;
  B1:;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  i0 = f43();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = 22u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = 0u;
  goto Bfunc;
  B0:;
  i0 = p0;
  i1 = 4294967295u;
  i0 += i1;
  l2 = i0;
  i1 = p1;
  i0 += i1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  i0 = !(i0);
  if (i0) {goto B6;}
  i0 = p0;
  i1 = l2;
  i2 = p0;
  i1 += i2;
  i2 = l1;
  i1 &= i2;
  l0 = i1;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  i0 = p0;
  i1 = 4294967292u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = 7u;
  i0 &= i1;
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B4;}
  i0 = p0;
  i1 = l2;
  i2 = 4294967288u;
  i1 &= i2;
  i0 += i1;
  l2 = i0;
  i1 = 4294967288u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l4 = i0;
  i0 = l1;
  i1 = p1;
  i2 = l0;
  i3 = p0;
  i2 -= i3;
  l5 = i2;
  i1 |= i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 4294967292u;
  i0 += i1;
  i1 = l2;
  i2 = l0;
  i1 -= i2;
  l1 = i1;
  i2 = p1;
  i1 |= i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 4294967288u;
  i0 += i1;
  i1 = l4;
  i2 = 7u;
  i1 &= i2;
  p1 = i1;
  i2 = l5;
  i1 |= i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l3;
  i1 = p1;
  i2 = l1;
  i1 |= i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f41(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B6:;
  i0 = l0;
  goto Bfunc;
  B5:;
  i0 = p0;
  goto Bfunc;
  B4:;
  i0 = l0;
  i1 = 4294967288u;
  i0 += i1;
  i1 = p0;
  i2 = 4294967288u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = l0;
  i3 = p0;
  i2 -= i3;
  p0 = i2;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 4294967292u;
  i0 += i1;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = p0;
  i1 -= i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f45(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 22u;
  l0 = i0;
  i0 = p1;
  i1 = 4u;
  i0 = i0 < i1;
  if (i0) {goto B1;}
  i0 = p1;
  i1 = p2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f44(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = p0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = 0u;
  l0 = i0;
  B1:;
  i0 = l0;
  goto Bfunc;
  B0:;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  i0 = f43();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = i32_load((&M0), (u64)(i0));
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f46(u32 p0) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 1u;
  i2 = p0;
  i0 = i2 ? i0 : i1;
  l0 = i0;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  if (i0) {goto B0;}
  L2: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = 0u;
    p0 = i0;
    i0 = 0u;
    i0 = i32_load((&M0), (u64)(i0 + 8216));
    l1 = i0;
    i0 = !(i0);
    if (i0) {goto B0;}
    i0 = l1;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    CALL_INDIRECT(T0, void (*)(void), 0, i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    i0 = f40(i0);
    i1 = g3;
    i2 = 1u;
    i1 += i2;
    g3 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    p0 = i0;
    i0 = !(i0);
    if (i0) {goto L2;}
  B0:;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f47(u32 p0) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f46(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
  return i0;
}

static void f48(u32 p0) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = p0;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f41(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  FUNC_EPILOGUE;
}

static void f49(u32 p0) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static u32 f50(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = l0;
  i1 = 12u;
  i0 += i1;
  i1 = p1;
  i2 = 4u;
  i3 = p1;
  i4 = 4u;
  i3 = i3 > i4;
  i1 = i3 ? i1 : i2;
  p1 = i1;
  i2 = p0;
  i3 = 1u;
  i4 = p0;
  i2 = i4 ? i2 : i3;
  l1 = i2;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = f45(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = !(i0);
  if (i0) {goto B0;}
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = 0u;
    i0 = i32_load((&M0), (u64)(i0 + 8216));
    p0 = i0;
    i0 = !(i0);
    if (i0) {goto B2;}
    i0 = p0;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    CALL_INDIRECT(T0, void (*)(void), 0, i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i1 = 12u;
    i0 += i1;
    i1 = p1;
    i2 = l1;
    i3 = g3;
    i3 = !(i3);
    if (i3) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i3 = 4294967295u;
      i4 = g3;
      i3 += i4;
      g3 = i3;
    }
    i0 = f45(i0, i1, i2);
    i1 = g3;
    i2 = 1u;
    i1 += i2;
    g3 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    if (i0) {goto L3;}
    goto B0;
  B2:;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 12), i1);
  B0:;
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 12));
  p0 = i0;
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f51(u32 p0, u32 p1) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p1;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f50(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
  return i0;
}

static void f52(u32 p0, u32 p1) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = p0;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f41(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  FUNC_EPILOGUE;
}

static void f53(u32 p0, u32 p1) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p1;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  f52(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static void f54(u32 p0) {
  FUNC_PROLOGUE;
  u32 i0, i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  (*Z_envZ_abortZ_vv)();
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  FUNC_EPILOGUE;
}

static u32 f55(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i0 = i32_load8_u((&M0), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B0;}
  i0 = p0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  goto Bfunc;
  B0:;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l0 = i0;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  l1 = i0;
  i0 = l0;
  i1 = 11u;
  i0 = i0 >= i1;
  if (i0) {goto B3;}
  i0 = p0;
  i1 = l0;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 1u;
  i0 += i1;
  p1 = i0;
  i0 = l0;
  if (i0) {goto B2;}
  i0 = p1;
  i1 = l0;
  i0 += i1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  goto Bfunc;
  B3:;
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  i1 = 4294967280u;
  i0 &= i1;
  l2 = i0;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f46(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i0 = p0;
  i1 = l2;
  i2 = 1u;
  i1 |= i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = p0;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 4), i1);
  B2:;
  i0 = p1;
  i1 = l1;
  i2 = l0;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = l0;
  i0 += i1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  goto Bfunc;
  B1:;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  (*Z_envZ_abortZ_vv)();
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static void f56(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B3;}
  i0 = p0;
  i0 = i32_load8_u((&M0), (u64)(i0));
  l0 = i0;
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B5;}
  i0 = l0;
  i1 = 1u;
  i0 >>= (i1 & 31);
  l1 = i0;
  i0 = 10u;
  l2 = i0;
  goto B4;
  B5:;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i1 = 4294967294u;
  i0 &= i1;
  i1 = 4294967295u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l1 = i0;
  B4:;
  i0 = 10u;
  l3 = i0;
  i0 = l1;
  i1 = p1;
  i2 = l1;
  i3 = p1;
  i2 = i2 > i3;
  i0 = i2 ? i0 : i1;
  p1 = i0;
  i1 = 11u;
  i0 = i0 < i1;
  if (i0) {goto B6;}
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  i1 = 4294967280u;
  i0 &= i1;
  i1 = 4294967295u;
  i0 += i1;
  l3 = i0;
  B6:;
  i0 = l3;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  i0 = l3;
  i1 = 10u;
  i0 = i0 != i1;
  if (i0) {goto B10;}
  i0 = 1u;
  l4 = i0;
  i0 = p0;
  i1 = 1u;
  i0 += i1;
  p1 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  l2 = i0;
  i0 = 0u;
  l5 = i0;
  i0 = 1u;
  l6 = i0;
  i0 = l0;
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B7;}
  goto B2;
  B10:;
  i0 = l3;
  i1 = 1u;
  i0 += i1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f46(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i0 = l3;
  i1 = l2;
  i0 = i0 > i1;
  if (i0) {goto B8;}
  i0 = p1;
  if (i0) {goto B8;}
  B9:;
  goto Bfunc;
  B8:;
  i0 = p0;
  i0 = i32_load8_u((&M0), (u64)(i0));
  l0 = i0;
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B12;}
  i0 = 1u;
  l5 = i0;
  i0 = p0;
  i1 = 1u;
  i0 += i1;
  l2 = i0;
  i0 = 0u;
  l4 = i0;
  i0 = 1u;
  l6 = i0;
  i0 = l0;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B2;}
  goto B7;
  B12:;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  l2 = i0;
  i0 = 1u;
  l4 = i0;
  i0 = 1u;
  l5 = i0;
  i0 = 1u;
  l6 = i0;
  i0 = l0;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B2;}
  B7:;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  i1 = 1u;
  i0 += i1;
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  goto B1;
  B3:;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  (*Z_envZ_abortZ_vv)();
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B2:;
  i0 = l0;
  i1 = 254u;
  i0 &= i1;
  i1 = l6;
  i0 >>= (i1 & 31);
  i1 = 1u;
  i0 += i1;
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  B1:;
  i0 = p1;
  i1 = l2;
  i2 = l0;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B15;}
  i0 = l2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B15:;
  i0 = l5;
  i0 = !(i0);
  if (i0) {goto B17;}
  i0 = p0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = p0;
  i1 = l3;
  i2 = 1u;
  i1 += i2;
  i2 = 1u;
  i1 |= i2;
  i32_store((&M0), (u64)(i0), i1);
  goto Bfunc;
  B17:;
  i0 = p0;
  i1 = l1;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&M0), (u64)(i0), i1);
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f57(u32 p0) {
  FUNC_PROLOGUE;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static void f58(u32 p0) {
  FUNC_PROLOGUE;
  u32 i0, i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  (*Z_envZ_abortZ_vv)();
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  FUNC_EPILOGUE;
}

static void f59(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l5 = 0, l6 = 0;
  u64 l2 = 0, l3 = 0, l4 = 0, l7 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4, j5;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 128u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p0;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  (*Z_envZ_require_authZ_vj)(j0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  l1 = i0;
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0 + 8));
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l2 = j0;
  l3 = j0;
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B2;}
    j0 = l3;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j0 = l3;
    j1 = 65280ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 == j1;
    if (i0) {goto B4;}
    j0 = l4;
    l3 = j0;
    i0 = l1;
    l5 = i0;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i0 = l5;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L3;}
    goto B1;
    B4:;
    j0 = l4;
    l3 = j0;
    L5: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 65280ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B2;}
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      i0 = l1;
      i1 = 6u;
      i0 = (u32)((s32)i0 < (s32)i1);
      l5 = i0;
      i0 = l1;
      i1 = 1u;
      i0 += i1;
      l6 = i0;
      l1 = i0;
      i0 = l5;
      if (i0) {goto L5;}
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i0 = l6;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L3;}
    goto B1;
  B2:;
  i0 = 0u;
  i1 = 8245u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B1:;
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  l7 = j0;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775806ull;
  i0 = j0 > j1;
  if (i0) {goto B8;}
  i0 = p2;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l3 = j0;
  i0 = 0u;
  l1 = i0;
  L9: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B8;}
    j0 = l3;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j0 = l3;
    j1 = 65280ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 == j1;
    if (i0) {goto B10;}
    j0 = l4;
    l3 = j0;
    i0 = l1;
    l5 = i0;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i0 = l5;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L9;}
    goto B7;
    B10:;
    j0 = l4;
    l3 = j0;
    L11: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 65280ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B8;}
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      i0 = l1;
      i1 = 6u;
      i0 = (u32)((s32)i0 < (s32)i1);
      l5 = i0;
      i0 = l1;
      i1 = 1u;
      i0 += i1;
      l6 = i0;
      l1 = i0;
      i0 = l5;
      if (i0) {goto L11;}
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i0 = l6;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L9;}
    goto B7;
  B8:;
  i0 = 0u;
  i1 = 8265u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  l7 = j0;
  B7:;
  j0 = l7;
  j1 = 0ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 8280u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  l3 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  j1 = l2;
  i64_store((&M0), (u64)(i0 + 16), j1);
  j0 = l3;
  j1 = l2;
  j2 = 14289235522390851584ull;
  j3 = l2;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  i0 = (*Z_envZ_db_find_i64Z_ijjjj)(j0, j1, j2, j3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B15;}
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f60(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = i32_load((&M0), (u64)(i0 + 40));
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i0 = i0 == i1;
  if (i0) {goto B17;}
  i0 = 0u;
  i1 = 8959u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B17:;
  i0 = 0u;
  i1 = 8308u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B15:;
  i0 = p0;
  j0 = i64_load((&M0), (u64)(i0));
  l4 = j0;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  j0 = (*Z_envZ_current_receiverZ_jv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i0 = j0 == j1;
  if (i0) {goto B21;}
  i0 = 0u;
  i1 = 9033u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B21:;
  i0 = 56u;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f46(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = l1;
  i1 = p2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  l3 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  j1 = l3;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l1;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i2 = 40u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 96), i1);
  i0 = l0;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 92), i1);
  i0 = l0;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 88), i1);
  i0 = l0;
  i1 = l0;
  i2 = 88u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 104), i1);
  i0 = l0;
  i1 = l1;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 116), i1);
  i0 = l0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 112), i1);
  i0 = l0;
  i1 = l1;
  i2 = 32u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 120), i1);
  i0 = l0;
  i1 = 112u;
  i0 += i1;
  i1 = l0;
  i2 = 104u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  f61(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 14289235522390851584ull;
  j3 = l4;
  i4 = l1;
  j4 = i64_load((&M0), (u64)(i4 + 8));
  j5 = 8ull;
  j4 >>= (j5 & 63);
  l3 = j4;
  i5 = l0;
  i6 = 48u;
  i5 += i6;
  i6 = 40u;
  i7 = g3;
  i7 = !(i7);
  if (i7) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i7 = 4294967295u;
    i8 = g3;
    i7 += i8;
    g3 = i7;
  }
  i1 = (*Z_envZ_db_store_i64Z_ijjjjii)(j1, j2, j3, j4, i5, i6);
  i2 = g3;
  i3 = 1u;
  i2 += i3;
  g3 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i1;
  i32_store((&M0), (u64)(i0 + 44), i1);
  j0 = l3;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i2 = 16u;
  i1 += i2;
  l5 = i1;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B27;}
  i0 = l5;
  j1 = l3;
  j2 = 1ull;
  j1 += j2;
  i64_store((&M0), (u64)(i0), j1);
  B27:;
  i0 = l0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 112), i1);
  i0 = l0;
  i1 = l1;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  l3 = j1;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l0;
  i1 = l6;
  i32_store((&M0), (u64)(i0 + 88), i1);
  i0 = l0;
  i1 = 36u;
  i0 += i1;
  p2 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B30;}
  i0 = l5;
  j1 = l3;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l5;
  i1 = l6;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 112), i1);
  i0 = l5;
  i1 = l1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p2;
  i1 = l5;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 112));
  l1 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 112), i1);
  i0 = l1;
  if (i0) {goto B29;}
  goto B28;
  B30:;
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i1 = l0;
  i2 = 112u;
  i1 += i2;
  i2 = l0;
  i3 = 48u;
  i2 += i3;
  i3 = l0;
  i4 = 88u;
  i3 += i4;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  f62(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 112));
  l1 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 112), i1);
  i0 = l1;
  i0 = !(i0);
  if (i0) {goto B28;}
  B29:;
  i0 = l1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B28:;
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  l6 = i0;
  i0 = !(i0);
  if (i0) {goto B33;}
  i0 = l0;
  i1 = 36u;
  i0 += i1;
  p2 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto B35;}
  L36: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = 4294967272u;
    i0 += i1;
    l1 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l5 = i0;
    i0 = l1;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l5;
    i0 = !(i0);
    if (i0) {goto B37;}
    i0 = l5;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    f48(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B37:;
    i0 = l6;
    i1 = l1;
    i0 = i0 != i1;
    if (i0) {goto L36;}
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  goto B34;
  B35:;
  i0 = l6;
  l1 = i0;
  B34:;
  i0 = p2;
  i1 = l6;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B33:;
  i0 = l0;
  i1 = 128u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f60(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0;
  u64 l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 48u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 24));
  l2 = i0;
  i1 = p0;
  i2 = 28u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l3 = i1;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  L2: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 4294967288u;
    i0 += i1;
    i0 = i32_load((&M0), (u64)(i0));
    i1 = p1;
    i0 = i0 == i1;
    if (i0) {goto B1;}
    i0 = l2;
    i1 = l3;
    i2 = 4294967272u;
    i1 += i2;
    l3 = i1;
    i0 = i0 != i1;
    if (i0) {goto L2;}
    goto B0;
  B1:;
  i0 = l2;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = l3;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = l1;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  i0 = l3;
  goto Bfunc;
  B0:;
  i0 = p1;
  i1 = 0u;
  i2 = 0u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_db_get_i64Z_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 <= (s32)i1);
  if (i0) {goto B5;}
  i0 = l3;
  i1 = 513u;
  i0 = i0 >= i1;
  if (i0) {goto B4;}
  i0 = l0;
  i1 = l3;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l2 = i0;
  g0 = i0;
  i0 = 0u;
  l4 = i0;
  goto B3;
  B5:;
  i0 = 0u;
  i1 = 9010u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B4:;
  i0 = l3;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = 1u;
  l4 = i0;
  B3:;
  i0 = p1;
  i1 = l2;
  i2 = l3;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_db_get_i64Z_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
  i1 = l2;
  i2 = l3;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = 56u;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f46(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l3;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = l1;
  i1 = l1;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l3;
  i2 = 32u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  i1 = l1;
  i2 = 24u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  f97(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 44), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  l5 = j1;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  l6 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i1 = p0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B14;}
  i0 = l0;
  j1 = l5;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l0;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l6;
  i1 = l0;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l4;
  if (i0) {goto B13;}
  goto B12;
  B14:;
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  i1 = l1;
  i2 = 24u;
  i1 += i2;
  i2 = l1;
  i3 = 32u;
  i2 += i3;
  i3 = l1;
  i4 = 4u;
  i3 += i4;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  f62(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B12;}
  B13:;
  i0 = l2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f41(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B12:;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 24));
  p1 = i0;
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = p1;
  i0 = !(i0);
  if (i0) {goto B17;}
  i0 = p1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B17:;
  i0 = l1;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  i0 = l3;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static void f61(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l3 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 9084u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B0:;
  i0 = l3;
  i1 = l1;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  l3 = i0;
  i1 = l3;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l4 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l2;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l4;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 9084u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  l4 = i0;
  B3:;
  i0 = l4;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = l3;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l3 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B6;}
  i0 = 0u;
  i1 = 9084u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B6:;
  i0 = l3;
  i1 = l1;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  l3 = i0;
  i1 = l3;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l4 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l2;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l4;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 9084u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  l4 = i0;
  B9:;
  i0 = l4;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = l3;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  p0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l3 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B12;}
  i0 = 0u;
  i1 = 9084u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B12:;
  i0 = l3;
  i1 = p0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  l2 = i0;
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f62(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1));
  l0 = i1;
  i0 -= i1;
  i1 = 24u;
  i0 = I32_DIV_S(i0, i1);
  l1 = i0;
  i1 = 1u;
  i0 += i1;
  l2 = i0;
  i1 = 178956971u;
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  i0 = 178956970u;
  l3 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 24u;
  i0 = I32_DIV_S(i0, i1);
  l0 = i0;
  i1 = 89478484u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = l2;
  i1 = l0;
  i2 = 1u;
  i1 <<= (i2 & 31);
  l3 = i1;
  i2 = l3;
  i3 = l2;
  i2 = i2 < i3;
  i0 = i2 ? i0 : i1;
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  B3:;
  i0 = l3;
  i1 = 24u;
  i0 *= i1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f46(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B0;
  B2:;
  i0 = 0u;
  l3 = i0;
  i0 = 0u;
  l0 = i0;
  goto B0;
  B1:;
  i0 = p0;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f58(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B0:;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = p1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = l1;
  i2 = 24u;
  i1 *= i2;
  l4 = i1;
  i0 += i1;
  p1 = i0;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p1;
  i1 = p3;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = l3;
  i2 = 24u;
  i1 *= i2;
  i0 += i1;
  l1 = i0;
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p2 = i0;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1));
  l3 = i1;
  i0 = i0 == i1;
  if (i0) {goto B7;}
  i0 = l0;
  i1 = l4;
  i0 += i1;
  i1 = 4294967272u;
  i0 += i1;
  p1 = i0;
  L8: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 4294967272u;
    i0 += i1;
    l0 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    p3 = i0;
    i0 = l0;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = p1;
    i1 = p3;
    i32_store((&M0), (u64)(i0), i1);
    i0 = p1;
    i1 = 16u;
    i0 += i1;
    i1 = p2;
    i2 = 4294967288u;
    i1 += i2;
    i1 = i32_load((&M0), (u64)(i1));
    i32_store((&M0), (u64)(i0), i1);
    i0 = p1;
    i1 = 8u;
    i0 += i1;
    i1 = p2;
    i2 = 4294967280u;
    i1 += i2;
    j1 = i64_load((&M0), (u64)(i1));
    i64_store((&M0), (u64)(i0), j1);
    i0 = p1;
    i1 = 4294967272u;
    i0 += i1;
    p1 = i0;
    i0 = l0;
    p2 = i0;
    i0 = l3;
    i1 = l0;
    i0 = i0 != i1;
    if (i0) {goto L8;}
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  p1 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  p2 = i0;
  goto B6;
  B7:;
  i0 = l3;
  p2 = i0;
  B6:;
  i0 = p0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  i32_store((&M0), (u64)(i0), i1);
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
    i0 = i32_load((&M0), (u64)(i0));
    p1 = i0;
    i0 = l3;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = p1;
    i0 = !(i0);
    if (i0) {goto B11;}
    i0 = p1;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    f48(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B11:;
    i0 = p2;
    i1 = l3;
    i0 = i0 != i1;
    if (i0) {goto L10;}
  B9:;
  i0 = p2;
  i0 = !(i0);
  if (i0) {goto B13;}
  i0 = p2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static void f63(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l5 = 0, l6 = 0;
  u64 l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 48u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = 0u;
  l1 = i0;
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0 + 8));
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l2 = j0;
  l3 = j0;
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B2;}
    j0 = l3;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j0 = l3;
    j1 = 65280ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 == j1;
    if (i0) {goto B5;}
    j0 = l4;
    l3 = j0;
    i0 = l1;
    l5 = i0;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i0 = l5;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L3;}
    goto B4;
    B5:;
    j0 = l4;
    l3 = j0;
    L6: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 65280ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B2;}
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      i0 = l1;
      i1 = 6u;
      i0 = (u32)((s32)i0 < (s32)i1);
      l5 = i0;
      i0 = l1;
      i1 = 1u;
      i0 += i1;
      l6 = i0;
      l1 = i0;
      i0 = l5;
      if (i0) {goto L6;}
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i0 = l6;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L3;}
    B4:;
  i0 = p3;
  i0 = i32_load8_u((&M0), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B0;}
  goto B1;
  B2:;
  i0 = 0u;
  i1 = 8245u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  i0 = i32_load8_u((&M0), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B0;}
  B1:;
  i0 = p3;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  i1 = 257u;
  i0 = i0 < i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 8341u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = 0u;
  p3 = i0;
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  l3 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  j1 = l2;
  i64_store((&M0), (u64)(i0 + 16), j1);
  j0 = l3;
  j1 = l2;
  j2 = 14289235522390851584ull;
  j3 = l2;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  i0 = (*Z_envZ_db_find_i64Z_ijjjj)(j0, j1, j2, j3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B11;}
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f60(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 40));
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i0 = i0 == i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 8959u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l3 = j0;
  i1 = p3;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i0 = j0 == j1;
  if (i0) {goto B9;}
  goto B10;
  B11:;
  i0 = 0u;
  i1 = 8370u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l3 = j0;
  i1 = 0u;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i0 = j0 == j1;
  if (i0) {goto B9;}
  B10:;
  i0 = 0u;
  i1 = 8430u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  j0 = i64_load((&M0), (u64)(i0 + 32));
  l3 = j0;
  B9:;
  j0 = l3;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  (*Z_envZ_require_authZ_vj)(j0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775806ull;
  i0 = j0 > j1;
  if (i0) {goto B20;}
  i0 = p2;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l3 = j0;
  i0 = 0u;
  l1 = i0;
  L21: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B20;}
    j0 = l3;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j0 = l3;
    j1 = 65280ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 == j1;
    if (i0) {goto B22;}
    j0 = l4;
    l3 = j0;
    i0 = l1;
    l5 = i0;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i0 = l5;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L21;}
    goto B19;
    B22:;
    j0 = l4;
    l3 = j0;
    L23: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 65280ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B20;}
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      i0 = l1;
      i1 = 6u;
      i0 = (u32)((s32)i0 < (s32)i1);
      l5 = i0;
      i0 = l1;
      i1 = 1u;
      i0 += i1;
      l6 = i0;
      l1 = i0;
      i0 = l5;
      if (i0) {goto L23;}
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i0 = l6;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L21;}
    goto B19;
  B20:;
  i0 = 0u;
  i1 = 8474u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  B19:;
  j0 = l2;
  j1 = 0ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  if (i0) {goto B25;}
  i0 = 0u;
  i1 = 8491u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B25:;
  i0 = p2;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p3;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i0 = j0 == j1;
  if (i0) {goto B27;}
  i0 = 0u;
  i1 = 8520u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B27:;
  i0 = p3;
  i1 = 32u;
  i0 += i1;
  l1 = i0;
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p3;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i2 = p3;
  j2 = i64_load((&M0), (u64)(i2));
  j1 -= j2;
  i0 = (u64)((s64)j0 <= (s64)j1);
  if (i0) {goto B29;}
  i0 = 0u;
  i1 = 8546u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B29:;
  i0 = l0;
  i1 = p2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = p3;
  i2 = l0;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  f64_0(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = l1;
  i2 = p2;
  i3 = l1;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  f65(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  l6 = i0;
  i0 = !(i0);
  if (i0) {goto B33;}
  i0 = l0;
  i1 = 36u;
  i0 += i1;
  p2 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto B35;}
  L36: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = 4294967272u;
    i0 += i1;
    l1 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l5 = i0;
    i0 = l1;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l5;
    i0 = !(i0);
    if (i0) {goto B37;}
    i0 = l5;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    f48(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B37:;
    i0 = l6;
    i1 = l1;
    i0 = i0 != i1;
    if (i0) {goto L36;}
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  goto B34;
  B35:;
  i0 = l6;
  l1 = i0;
  B34:;
  i0 = p2;
  i1 = l6;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B33:;
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f64_0(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0;
  u64 l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 48u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 40));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 9090u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  j0 = (*Z_envZ_current_receiverZ_jv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B2;}
  i0 = 0u;
  i1 = 9136u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B2:;
  i0 = p2;
  i0 = i32_load((&M0), (u64)(i0));
  p2 = i0;
  j0 = i64_load((&M0), (u64)(i0 + 8));
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  l2 = j1;
  i0 = j0 == j1;
  if (i0) {goto B5;}
  i0 = 0u;
  i1 = 9246u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B5:;
  i0 = p1;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  i2 = p2;
  j2 = i64_load((&M0), (u64)(i2));
  j1 += j2;
  l3 = j1;
  i64_store((&M0), (u64)(i0), j1);
  j0 = l3;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 9289u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l3 = j0;
  B7:;
  j0 = l2;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l2 = j0;
  j0 = l3;
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 9308u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B9:;
  j0 = l2;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  i0 = j0 == j1;
  if (i0) {goto B11;}
  i0 = 0u;
  i1 = 9187u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  i0 = l0;
  p2 = i0;
  i1 = 4294967248u;
  i0 += i1;
  l0 = i0;
  g0 = i0;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
  i1 = p2;
  i2 = 4294967288u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l1;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = p1;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = p1;
  i2 = 32u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  i1 = l1;
  i2 = 24u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  f61(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 44));
  j1 = 0ull;
  i2 = l0;
  i3 = 40u;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  (*Z_envZ_db_update_i64Z_vijii)(i0, j1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = l2;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B15;}
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = l2;
  j2 = 1ull;
  j1 += j2;
  i64_store((&M0), (u64)(i0), j1);
  B15:;
  i0 = l1;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f65(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0;
  u64 l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4, j5;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 80u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  l1 = j1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  l2 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  j0 = l1;
  j1 = l2;
  j2 = 3607749779137757184ull;
  i3 = p2;
  j3 = i64_load((&M0), (u64)(i3 + 8));
  j4 = 8ull;
  j3 >>= (j4 & 63);
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  i0 = (*Z_envZ_db_find_i64Z_ijjjj)(j0, j1, j2, j3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B2;}
  i0 = l0;
  i1 = p1;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f66(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B4;}
  i0 = 0u;
  i1 = 8959u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B4:;
  i0 = l0;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = l0;
  i1 = p1;
  i2 = l0;
  i3 = 48u;
  i2 += i3;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  f67(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 24));
  p0 = i0;
  if (i0) {goto B1;}
  goto B0;
  B2:;
  i0 = p3;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  j0 = (*Z_envZ_current_receiverZ_jv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B8;}
  i0 = 0u;
  i1 = 9033u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B8:;
  i0 = 32u;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f46(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = p1;
  i1 = p2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p1;
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  i1 = p1;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  i1 = 8u;
  i0 |= i1;
  i1 = l0;
  i2 = 72u;
  i1 += i2;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 3607749779137757184ull;
  j3 = l2;
  i4 = p1;
  j4 = i64_load((&M0), (u64)(i4 + 8));
  j5 = 8ull;
  j4 >>= (j5 & 63);
  l1 = j4;
  i5 = l0;
  i6 = 48u;
  i5 += i6;
  i6 = 16u;
  i7 = g3;
  i7 = !(i7);
  if (i7) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i7 = 4294967295u;
    i8 = g3;
    i7 += i8;
    g3 = i7;
  }
  i1 = (*Z_envZ_db_store_i64Z_ijjjjii)(j1, j2, j3, j4, i5, i6);
  i2 = g3;
  i3 = 1u;
  i2 += i3;
  g3 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i1;
  i32_store((&M0), (u64)(i0 + 20), i1);
  j0 = l1;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  p2 = i1;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B15;}
  i0 = p2;
  j1 = l1;
  j2 = 1ull;
  j1 += j2;
  i64_store((&M0), (u64)(i0), j1);
  B15:;
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 72), i1);
  i0 = l0;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  l1 = j1;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l0;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 44), i1);
  i0 = l0;
  i1 = 28u;
  i0 += i1;
  p3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  p2 = i0;
  i1 = l0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B18;}
  i0 = p2;
  j1 = l1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p2;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 72), i1);
  i0 = p2;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p3;
  i1 = p2;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 72));
  p1 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 72), i1);
  i0 = p1;
  if (i0) {goto B17;}
  goto B16;
  B18:;
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  i1 = l0;
  i2 = 72u;
  i1 += i2;
  i2 = l0;
  i3 = 48u;
  i2 += i3;
  i3 = l0;
  i4 = 44u;
  i3 += i4;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  f68(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 72));
  p1 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 72), i1);
  i0 = p1;
  i0 = !(i0);
  if (i0) {goto B16;}
  B17:;
  i0 = p1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B16:;
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 24));
  p0 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  B1:;
  i0 = l0;
  i1 = 28u;
  i0 += i1;
  p3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  p1 = i0;
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B22;}
  L23: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p1;
    i1 = 4294967272u;
    i0 += i1;
    p1 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    p2 = i0;
    i0 = p1;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = p2;
    i0 = !(i0);
    if (i0) {goto B24;}
    i0 = p2;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    f48(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B24:;
    i0 = p0;
    i1 = p1;
    i0 = i0 != i1;
    if (i0) {goto L23;}
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p1 = i0;
  goto B21;
  B22:;
  i0 = p0;
  p1 = i0;
  B21:;
  i0 = p3;
  i1 = p0;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l0;
  i1 = 80u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f66(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  u64 l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 32u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 24));
  l2 = i0;
  i1 = p0;
  i2 = 28u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l3 = i1;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  L2: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 4294967288u;
    i0 += i1;
    i0 = i32_load((&M0), (u64)(i0));
    i1 = p1;
    i0 = i0 == i1;
    if (i0) {goto B1;}
    i0 = l2;
    i1 = l3;
    i2 = 4294967272u;
    i1 += i2;
    l3 = i1;
    i0 = i0 != i1;
    if (i0) {goto L2;}
    goto B0;
  B1:;
  i0 = l2;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = l3;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  g0 = i0;
  i0 = l3;
  goto Bfunc;
  B0:;
  i0 = p1;
  i1 = 0u;
  i2 = 0u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_db_get_i64Z_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 <= (s32)i1);
  if (i0) {goto B5;}
  i0 = l2;
  i1 = 513u;
  i0 = i0 >= i1;
  if (i0) {goto B4;}
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = 0u;
  l4 = i0;
  goto B3;
  B5:;
  i0 = 0u;
  i1 = 9010u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B4:;
  i0 = l2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = 1u;
  l4 = i0;
  B3:;
  i0 = p1;
  i1 = l0;
  i2 = l2;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_db_get_i64Z_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 32u;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f46(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l3;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B11;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  i0 = l3;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  l5 = i0;
  i0 = l2;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = 8u;
  i0 = i0 != i1;
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
  i0 = l1;
  i1 = 24u;
  i0 += i1;
  i1 = l5;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  l6 = j1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l3;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  j1 = l6;
  j2 = 8ull;
  j1 >>= (j2 & 63);
  l6 = j1;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  l5 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = p0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B19;}
  i0 = l2;
  j1 = l6;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l2;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l2;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l5;
  i1 = l2;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l4;
  if (i0) {goto B18;}
  goto B17;
  B19:;
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  i1 = l1;
  i2 = 16u;
  i1 += i2;
  i2 = l1;
  i3 = 24u;
  i2 += i3;
  i3 = l1;
  i4 = 12u;
  i3 += i4;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  f68(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B17;}
  B18:;
  i0 = l0;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f41(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B17:;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  p1 = i0;
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = p1;
  i0 = !(i0);
  if (i0) {goto B22;}
  i0 = p1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B22:;
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  g0 = i0;
  i0 = l3;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static void f67(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l4 = 0;
  u64 l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 9090u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  j0 = (*Z_envZ_current_receiverZ_jv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B2;}
  i0 = 0u;
  i1 = 9136u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B2:;
  i0 = p2;
  i0 = i32_load((&M0), (u64)(i0));
  p2 = i0;
  j0 = i64_load((&M0), (u64)(i0 + 8));
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  l2 = j1;
  i0 = j0 == j1;
  if (i0) {goto B5;}
  i0 = 0u;
  i1 = 9246u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B5:;
  i0 = p1;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  i2 = p2;
  j2 = i64_load((&M0), (u64)(i2));
  j1 += j2;
  l3 = j1;
  i64_store((&M0), (u64)(i0), j1);
  j0 = l3;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 9289u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l3 = j0;
  B7:;
  j0 = l2;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l2 = j0;
  j0 = l3;
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 9308u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B9:;
  j0 = l2;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  p2 = i1;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  i0 = j0 == j1;
  if (i0) {goto B11;}
  i0 = 0u;
  i1 = 9187u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  i0 = l0;
  l4 = i0;
  i1 = 4294967280u;
  i0 += i1;
  l0 = i0;
  g0 = i0;
  i0 = l0;
  i1 = p1;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l4;
  i1 = 4294967288u;
  i0 += i1;
  i1 = l1;
  i2 = 8u;
  i1 += i2;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 20));
  j1 = 0ull;
  i2 = l0;
  i3 = 16u;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  (*Z_envZ_db_update_i64Z_vijii)(i0, j1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = l2;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B16;}
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = l2;
  j2 = 1ull;
  j1 += j2;
  i64_store((&M0), (u64)(i0), j1);
  B16:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f68(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1));
  l0 = i1;
  i0 -= i1;
  i1 = 24u;
  i0 = I32_DIV_S(i0, i1);
  l1 = i0;
  i1 = 1u;
  i0 += i1;
  l2 = i0;
  i1 = 178956971u;
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  i0 = 178956970u;
  l3 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 24u;
  i0 = I32_DIV_S(i0, i1);
  l0 = i0;
  i1 = 89478484u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = l2;
  i1 = l0;
  i2 = 1u;
  i1 <<= (i2 & 31);
  l3 = i1;
  i2 = l3;
  i3 = l2;
  i2 = i2 < i3;
  i0 = i2 ? i0 : i1;
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  B3:;
  i0 = l3;
  i1 = 24u;
  i0 *= i1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f46(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B0;
  B2:;
  i0 = 0u;
  l3 = i0;
  i0 = 0u;
  l0 = i0;
  goto B0;
  B1:;
  i0 = p0;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f58(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B0:;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = p1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = l1;
  i2 = 24u;
  i1 *= i2;
  l4 = i1;
  i0 += i1;
  p1 = i0;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p1;
  i1 = p3;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = l3;
  i2 = 24u;
  i1 *= i2;
  i0 += i1;
  l1 = i0;
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p2 = i0;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1));
  l3 = i1;
  i0 = i0 == i1;
  if (i0) {goto B7;}
  i0 = l0;
  i1 = l4;
  i0 += i1;
  i1 = 4294967272u;
  i0 += i1;
  p1 = i0;
  L8: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 4294967272u;
    i0 += i1;
    l0 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    p3 = i0;
    i0 = l0;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = p1;
    i1 = p3;
    i32_store((&M0), (u64)(i0), i1);
    i0 = p1;
    i1 = 16u;
    i0 += i1;
    i1 = p2;
    i2 = 4294967288u;
    i1 += i2;
    i1 = i32_load((&M0), (u64)(i1));
    i32_store((&M0), (u64)(i0), i1);
    i0 = p1;
    i1 = 8u;
    i0 += i1;
    i1 = p2;
    i2 = 4294967280u;
    i1 += i2;
    j1 = i64_load((&M0), (u64)(i1));
    i64_store((&M0), (u64)(i0), j1);
    i0 = p1;
    i1 = 4294967272u;
    i0 += i1;
    p1 = i0;
    i0 = l0;
    p2 = i0;
    i0 = l3;
    i1 = l0;
    i0 = i0 != i1;
    if (i0) {goto L8;}
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  p1 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  p2 = i0;
  goto B6;
  B7:;
  i0 = l3;
  p2 = i0;
  B6:;
  i0 = p0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  i32_store((&M0), (u64)(i0), i1);
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
    i0 = i32_load((&M0), (u64)(i0));
    p1 = i0;
    i0 = l3;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = p1;
    i0 = !(i0);
    if (i0) {goto B11;}
    i0 = p1;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    f48(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B11:;
    i0 = p2;
    i1 = l3;
    i0 = i0 != i1;
    if (i0) {goto L10;}
  B9:;
  i0 = p2;
  i0 = !(i0);
  if (i0) {goto B13;}
  i0 = p2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static void f69(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l5 = 0, l6 = 0;
  u64 l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 48u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = 0u;
  l1 = i0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0 + 8));
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l2 = j0;
  l3 = j0;
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B2;}
    j0 = l3;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j0 = l3;
    j1 = 65280ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 == j1;
    if (i0) {goto B5;}
    j0 = l4;
    l3 = j0;
    i0 = l1;
    l5 = i0;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i0 = l5;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L3;}
    goto B4;
    B5:;
    j0 = l4;
    l3 = j0;
    L6: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 65280ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B2;}
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      i0 = l1;
      i1 = 6u;
      i0 = (u32)((s32)i0 < (s32)i1);
      l5 = i0;
      i0 = l1;
      i1 = 1u;
      i0 += i1;
      l6 = i0;
      l1 = i0;
      i0 = l5;
      if (i0) {goto L6;}
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i0 = l6;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L3;}
    B4:;
  i0 = p2;
  i0 = i32_load8_u((&M0), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B0;}
  goto B1;
  B2:;
  i0 = 0u;
  i1 = 8245u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i0 = i32_load8_u((&M0), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B0;}
  B1:;
  i0 = p2;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  i1 = 257u;
  i0 = i0 < i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 8341u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = 0u;
  p2 = i0;
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  l3 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  j1 = l2;
  i64_store((&M0), (u64)(i0 + 16), j1);
  j0 = l3;
  j1 = l2;
  j2 = 14289235522390851584ull;
  j3 = l2;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  i0 = (*Z_envZ_db_find_i64Z_ijjjj)(j0, j1, j2, j3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B10;}
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f60(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 40));
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 8959u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B9;
  B10:;
  i0 = 0u;
  i1 = 8580u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B9:;
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0 + 32));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  (*Z_envZ_require_authZ_vj)(j0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775806ull;
  i0 = j0 > j1;
  if (i0) {goto B17;}
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l3 = j0;
  i0 = 0u;
  l1 = i0;
  L18: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B17;}
    j0 = l3;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j0 = l3;
    j1 = 65280ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 == j1;
    if (i0) {goto B19;}
    j0 = l4;
    l3 = j0;
    i0 = l1;
    l5 = i0;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i0 = l5;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L18;}
    goto B16;
    B19:;
    j0 = l4;
    l3 = j0;
    L20: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 65280ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B17;}
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      i0 = l1;
      i1 = 6u;
      i0 = (u32)((s32)i0 < (s32)i1);
      l5 = i0;
      i0 = l1;
      i1 = 1u;
      i0 += i1;
      l6 = i0;
      l1 = i0;
      i0 = l5;
      if (i0) {goto L20;}
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i0 = l6;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L18;}
    goto B16;
  B17:;
  i0 = 0u;
  i1 = 8474u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  B16:;
  j0 = l2;
  j1 = 0ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  if (i0) {goto B22;}
  i0 = 0u;
  i1 = 8613u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B22:;
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i0 = j0 == j1;
  if (i0) {goto B24;}
  i0 = 0u;
  i1 = 8520u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B24:;
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = p2;
  i2 = l0;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  f70(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p2;
  i2 = 32u;
  i1 += i2;
  i2 = p1;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  f71(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  l6 = i0;
  i0 = !(i0);
  if (i0) {goto B28;}
  i0 = l0;
  i1 = 36u;
  i0 += i1;
  p1 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto B30;}
  L31: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = 4294967272u;
    i0 += i1;
    l1 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l5 = i0;
    i0 = l1;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l5;
    i0 = !(i0);
    if (i0) {goto B32;}
    i0 = l5;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    f48(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B32:;
    i0 = l6;
    i1 = l1;
    i0 = i0 != i1;
    if (i0) {goto L31;}
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  goto B29;
  B30:;
  i0 = l6;
  l1 = i0;
  B29:;
  i0 = p1;
  i1 = l6;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B28:;
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f70(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0;
  u64 l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 48u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 40));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 9090u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  j0 = (*Z_envZ_current_receiverZ_jv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B2;}
  i0 = 0u;
  i1 = 9136u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B2:;
  i0 = p2;
  i0 = i32_load((&M0), (u64)(i0));
  p2 = i0;
  j0 = i64_load((&M0), (u64)(i0 + 8));
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  l2 = j1;
  i0 = j0 == j1;
  if (i0) {goto B5;}
  i0 = 0u;
  i1 = 9326u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B5:;
  i0 = p1;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  i2 = p2;
  j2 = i64_load((&M0), (u64)(i2));
  j1 -= j2;
  l3 = j1;
  i64_store((&M0), (u64)(i0), j1);
  j0 = l3;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 9374u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l3 = j0;
  B7:;
  j0 = l2;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l2 = j0;
  j0 = l3;
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 9396u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B9:;
  j0 = l2;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  i0 = j0 == j1;
  if (i0) {goto B11;}
  i0 = 0u;
  i1 = 9187u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  i0 = l0;
  p2 = i0;
  i1 = 4294967248u;
  i0 += i1;
  l0 = i0;
  g0 = i0;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
  i1 = p2;
  i2 = 4294967288u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l1;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = p1;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = p1;
  i2 = 32u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  i1 = l1;
  i2 = 24u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  f61(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 44));
  j1 = 0ull;
  i2 = l0;
  i3 = 40u;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  (*Z_envZ_db_update_i64Z_vijii)(i0, j1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = l2;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B15;}
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = l2;
  j2 = 1ull;
  j1 += j2;
  i64_store((&M0), (u64)(i0), j1);
  B15:;
  i0 = l1;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f71(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0;
  u64 l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 48u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = 0u;
  l1 = i0;
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  l2 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  l3 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  j0 = l2;
  j1 = l3;
  j2 = 3607749779137757184ull;
  i3 = p2;
  j3 = i64_load((&M0), (u64)(i3 + 8));
  j4 = 8ull;
  j3 >>= (j4 & 63);
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  i0 = (*Z_envZ_db_find_i64Z_ijjjj)(j0, j1, j2, j3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B2;}
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = p0;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f66(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i0 = i0 == i1;
  if (i0) {goto B4;}
  i0 = 0u;
  i1 = 8959u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B4:;
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = (u64)((s64)j0 >= (s64)j1);
  if (i0) {goto B0;}
  goto B1;
  B2:;
  i0 = 0u;
  i1 = 8744u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = (u64)((s64)j0 >= (s64)j1);
  if (i0) {goto B0;}
  B1:;
  i0 = 0u;
  i1 = 8768u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  i0 = l0;
  i1 = p2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j2 = l2;
  i3 = l0;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  f72(i0, i1, j2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B10;}
  i0 = l0;
  i1 = 36u;
  i0 += i1;
  p0 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  p2 = i0;
  i1 = p1;
  i0 = i0 == i1;
  if (i0) {goto B12;}
  L13: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 4294967272u;
    i0 += i1;
    p2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l1 = i0;
    i0 = p2;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l1;
    i0 = !(i0);
    if (i0) {goto B14;}
    i0 = l1;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    f48(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B14:;
    i0 = p1;
    i1 = p2;
    i0 = i0 != i1;
    if (i0) {goto L13;}
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p2 = i0;
  goto B11;
  B12:;
  i0 = p1;
  p2 = i0;
  B11:;
  i0 = p0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B10:;
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f72(u32 p0, u32 p1, u64 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l4 = 0;
  u64 l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 9090u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  j0 = (*Z_envZ_current_receiverZ_jv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B2;}
  i0 = 0u;
  i1 = 9136u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B2:;
  i0 = p3;
  i0 = i32_load((&M0), (u64)(i0));
  p3 = i0;
  j0 = i64_load((&M0), (u64)(i0 + 8));
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  l2 = j1;
  i0 = j0 == j1;
  if (i0) {goto B5;}
  i0 = 0u;
  i1 = 9326u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B5:;
  i0 = p1;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  i2 = p3;
  j2 = i64_load((&M0), (u64)(i2));
  j1 -= j2;
  l3 = j1;
  i64_store((&M0), (u64)(i0), j1);
  j0 = l3;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 9374u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l3 = j0;
  B7:;
  j0 = l2;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l2 = j0;
  j0 = l3;
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 9396u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B9:;
  j0 = l2;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  p3 = i1;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  i0 = j0 == j1;
  if (i0) {goto B11;}
  i0 = 0u;
  i1 = 9187u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  i0 = l0;
  l4 = i0;
  i1 = 4294967280u;
  i0 += i1;
  l0 = i0;
  g0 = i0;
  i0 = l0;
  i1 = p1;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = p3;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l4;
  i1 = 4294967288u;
  i0 += i1;
  i1 = l1;
  i2 = 8u;
  i1 += i2;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 20));
  j1 = p2;
  i2 = l0;
  i3 = 16u;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  (*Z_envZ_db_update_i64Z_vijii)(i0, j1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = l2;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B16;}
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = l2;
  j2 = 1ull;
  j1 += j2;
  i64_store((&M0), (u64)(i0), j1);
  B16:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f73(u32 p0, u32 p1, u32 p2, u32 p3, u32 p4) {
  u32 l0 = 0, l2 = 0, l4 = 0, l6 = 0, l7 = 0;
  u64 l1 = 0, l3 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 48u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l1 = j0;
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 != j1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 8643u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l1 = j0;
  B0:;
  j0 = l1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  (*Z_envZ_require_authZ_vj)(j0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = (*Z_envZ_is_accountZ_ij)(j0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 8667u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B3:;
  i0 = p3;
  j0 = i64_load((&M0), (u64)(i0 + 8));
  l1 = j0;
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  j1 = l1;
  j2 = 8ull;
  j1 >>= (j2 & 63);
  l1 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  j1 = l1;
  i2 = 8693u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = f74(i0, j1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  (*Z_envZ_require_recipientZ_vj)(j0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  (*Z_envZ_require_recipientZ_vj)(j0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  j0 = i64_load((&M0), (u64)(i0));
  l3 = j0;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775806ull;
  i0 = j0 > j1;
  if (i0) {goto B10;}
  i0 = p3;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l1 = j0;
  i0 = 0u;
  l4 = i0;
  L11: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l1;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B10;}
    j0 = l1;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j0 = l1;
    j1 = 65280ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 == j1;
    if (i0) {goto B12;}
    j0 = l5;
    l1 = j0;
    i0 = l4;
    l6 = i0;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i0 = l6;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L11;}
    goto B9;
    B12:;
    j0 = l5;
    l1 = j0;
    L13: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l1;
      j1 = 65280ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B10;}
      j0 = l1;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l1 = j0;
      i0 = l4;
      i1 = 6u;
      i0 = (u32)((s32)i0 < (s32)i1);
      l6 = i0;
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l7 = i0;
      l4 = i0;
      i0 = l6;
      if (i0) {goto L13;}
    i0 = l7;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i0 = l7;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L11;}
    goto B9;
  B10:;
  i0 = 0u;
  i1 = 8474u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  j0 = i64_load((&M0), (u64)(i0));
  l3 = j0;
  B9:;
  j0 = l3;
  j1 = 0ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  if (i0) {goto B15;}
  i0 = 0u;
  i1 = 8712u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B15:;
  i0 = p3;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = l2;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i0 = j0 == j1;
  if (i0) {goto B17;}
  i0 = 0u;
  i1 = 8520u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B17:;
  i0 = p4;
  i0 = i32_load8_u((&M0), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B19;}
  i0 = p4;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  i1 = 257u;
  i0 = i0 < i1;
  if (i0) {goto B19;}
  i0 = 0u;
  i1 = 8341u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B19:;
  i0 = l0;
  i1 = p2;
  i2 = p1;
  i3 = p2;
  j3 = i64_load((&M0), (u64)(i3));
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  i3 = (*Z_envZ_has_authZ_ij)(j3);
  i4 = g3;
  i5 = 1u;
  i4 += i5;
  g3 = i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = i3 ? i1 : i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = p1;
  i2 = p3;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  f71(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p2;
  i2 = p3;
  i3 = l0;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  f65(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  l7 = i0;
  i0 = !(i0);
  if (i0) {goto B24;}
  i0 = l0;
  i1 = 36u;
  i0 += i1;
  p3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l4 = i0;
  i1 = l7;
  i0 = i0 == i1;
  if (i0) {goto B26;}
  L27: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l4 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l6 = i0;
    i0 = l4;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l6;
    i0 = !(i0);
    if (i0) {goto B28;}
    i0 = l6;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    f48(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B28:;
    i0 = l7;
    i1 = l4;
    i0 = i0 != i1;
    if (i0) {goto L27;}
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l4 = i0;
  goto B25;
  B26:;
  i0 = l7;
  l4 = i0;
  B25:;
  i0 = p3;
  i1 = l7;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l4;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B24:;
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f74(u32 p0, u64 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 24));
  l0 = i0;
  i1 = p0;
  i2 = 28u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l3 = i0;
    j0 = i64_load((&M0), (u64)(i0 + 8));
    j1 = 8ull;
    j0 >>= (j1 & 63);
    j1 = p1;
    i0 = j0 == j1;
    if (i0) {goto B2;}
    i0 = l2;
    l1 = i0;
    i0 = l0;
    i1 = l2;
    i0 = i0 != i1;
    if (i0) {goto L3;}
    goto B1;
  B2:;
  i0 = l0;
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0 + 40));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 8959u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  goto Bfunc;
  B1:;
  i0 = 0u;
  l3 = i0;
  i0 = p0;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 14289235522390851584ull;
  j3 = p1;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  i0 = (*Z_envZ_db_find_i64Z_ijjjj)(j0, j1, j2, j3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B5;}
  i0 = p0;
  i1 = l2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f60(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 40));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 8959u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  goto Bfunc;
  B5:;
  i0 = 0u;
  i1 = p2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l3;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static void f75(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0;
  u64 l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 128u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p3;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  (*Z_envZ_require_authZ_vj)(j0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = (*Z_envZ_is_accountZ_ij)(j0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  if (i0) {goto B1;}
  i0 = 0u;
  i1 = 8786u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B1:;
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  l1 = j0;
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  i1 = 32u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = l1;
  j2 = 8ull;
  j1 >>= (j2 & 63);
  l1 = j1;
  i64_store((&M0), (u64)(i0 + 56), j1);
  i0 = l0;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 64), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  j1 = l1;
  i2 = 8815u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = f74(i0, j1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = i64_load((&M0), (u64)(i0 + 8));
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B4;}
  i0 = 0u;
  i1 = 8520u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B4:;
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = 32u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  l2 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  l3 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  j0 = l2;
  j1 = l3;
  j2 = 3607749779137757184ull;
  j3 = l1;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  i0 = (*Z_envZ_db_find_i64Z_ijjjj)(j0, j1, j2, j3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B10;}
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = p0;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f66(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = i32_load((&M0), (u64)(i0 + 16));
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 8959u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  p1 = i0;
  if (i0) {goto B8;}
  goto B7;
  B10:;
  i0 = p3;
  j0 = i64_load((&M0), (u64)(i0));
  l1 = j0;
  i0 = l0;
  i1 = p2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = l1;
  i64_store((&M0), (u64)(i0 + 120), j1);
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  j0 = (*Z_envZ_current_receiverZ_jv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i0 = j0 == j1;
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 9033u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
  i0 = l0;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 100), i1);
  i0 = l0;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 96), i1);
  i0 = l0;
  i1 = l0;
  i2 = 120u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 104), i1);
  i0 = 32u;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f46(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p2;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p2;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = 96u;
  i0 += i1;
  i1 = p2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  f76(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 112), i1);
  i0 = l0;
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  l1 = j1;
  i64_store((&M0), (u64)(i0 + 96), j1);
  i0 = l0;
  i1 = p2;
  i1 = i32_load((&M0), (u64)(i1 + 20));
  p1 = i1;
  i32_store((&M0), (u64)(i0 + 92), i1);
  i0 = l0;
  i1 = 36u;
  i0 += i1;
  p3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  p0 = i0;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B20;}
  i0 = p0;
  j1 = l1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p0;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 112), i1);
  i0 = p0;
  i1 = p2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p3;
  i1 = p0;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 112));
  p2 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 112), i1);
  i0 = p2;
  if (i0) {goto B19;}
  goto B9;
  B20:;
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i1 = l0;
  i2 = 112u;
  i1 += i2;
  i2 = l0;
  i3 = 96u;
  i2 += i3;
  i3 = l0;
  i4 = 92u;
  i3 += i4;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  f68(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 112));
  p2 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 112), i1);
  i0 = p2;
  i0 = !(i0);
  if (i0) {goto B9;}
  B19:;
  i0 = p2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B9:;
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B7;}
  B8:;
  i0 = l0;
  i1 = 36u;
  i0 += i1;
  p3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  p2 = i0;
  i1 = p1;
  i0 = i0 == i1;
  if (i0) {goto B24;}
  L25: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 4294967272u;
    i0 += i1;
    p2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    p0 = i0;
    i0 = p2;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = p0;
    i0 = !(i0);
    if (i0) {goto B26;}
    i0 = p0;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    f48(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B26:;
    i0 = p1;
    i1 = p2;
    i0 = i0 != i1;
    if (i0) {goto L25;}
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p2 = i0;
  goto B23;
  B24:;
  i0 = p1;
  p2 = i0;
  B23:;
  i0 = p3;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B7:;
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 72));
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B29;}
  i0 = l0;
  i1 = 76u;
  i0 += i1;
  p3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  p2 = i0;
  i1 = p1;
  i0 = i0 == i1;
  if (i0) {goto B31;}
  L32: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 4294967272u;
    i0 += i1;
    p2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    p0 = i0;
    i0 = p2;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = p0;
    i0 = !(i0);
    if (i0) {goto B33;}
    i0 = p0;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    f48(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B33:;
    i0 = p1;
    i1 = p2;
    i0 = i0 != i1;
    if (i0) {goto L32;}
  i0 = l0;
  i1 = 72u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p2 = i0;
  goto B30;
  B31:;
  i0 = p1;
  p2 = i0;
  B30:;
  i0 = p3;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B29:;
  i0 = l0;
  i1 = 128u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f76(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l4 = 0, l5 = 0, l7 = 0, l8 = 0;
  u64 l2 = 0, l3 = 0, l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4, j5;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  i0 = i32_load((&M0), (u64)(i0));
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l3 = j0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l4 = i0;
  i0 = 0u;
  l5 = i0;
  L2: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B1;}
    j0 = l3;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j0 = l3;
    j1 = 65280ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 == j1;
    if (i0) {goto B3;}
    j0 = l6;
    l3 = j0;
    i0 = l5;
    l7 = i0;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i0 = l7;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L2;}
    goto B0;
    B3:;
    j0 = l6;
    l3 = j0;
    L4: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 65280ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B1;}
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      i0 = l5;
      i1 = 6u;
      i0 = (u32)((s32)i0 < (s32)i1);
      l7 = i0;
      i0 = l5;
      i1 = 1u;
      i0 += i1;
      l8 = i0;
      l5 = i0;
      i0 = l7;
      if (i0) {goto L4;}
    i0 = l8;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i0 = l8;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L2;}
    goto B0;
  B1:;
  i0 = 0u;
  i1 = 8245u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = p1;
  j1 = l2;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  l7 = i0;
  i1 = 4294967280u;
  i0 += i1;
  l5 = i0;
  g0 = i0;
  i0 = l5;
  i1 = p1;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l7;
  i1 = 4294967288u;
  i0 += i1;
  i1 = l1;
  i2 = 8u;
  i1 += i2;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = l4;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 3607749779137757184ull;
  i3 = p0;
  i3 = i32_load((&M0), (u64)(i3 + 8));
  j3 = i64_load((&M0), (u64)(i3));
  i4 = p1;
  j4 = i64_load((&M0), (u64)(i4 + 8));
  j5 = 8ull;
  j4 >>= (j5 & 63);
  l3 = j4;
  i5 = l5;
  i6 = 16u;
  i7 = g3;
  i7 = !(i7);
  if (i7) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i7 = 4294967295u;
    i8 = g3;
    i7 += i8;
    g3 = i7;
  }
  i1 = (*Z_envZ_db_store_i64Z_ijjjjii)(j1, j2, j3, j4, i5, i6);
  i2 = g3;
  i3 = 1u;
  i2 += i3;
  g3 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i32_store((&M0), (u64)(i0 + 20), i1);
  j0 = l3;
  i1 = l4;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B9;}
  i0 = l4;
  i1 = 16u;
  i0 += i1;
  j1 = l3;
  j2 = 1ull;
  j1 += j2;
  i64_store((&M0), (u64)(i0), j1);
  B9:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f77(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0;
  u64 l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 48u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  (*Z_envZ_require_authZ_vj)(j0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  l1 = i0;
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  l2 = j1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  l3 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  j0 = l2;
  j1 = l3;
  j2 = 3607749779137757184ull;
  i3 = p2;
  j3 = i64_load((&M0), (u64)(i3));
  j4 = 8ull;
  j3 >>= (j4 & 63);
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  i0 = (*Z_envZ_db_find_i64Z_ijjjj)(j0, j1, j2, j3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 <= (s32)i1);
  if (i0) {goto B3;}
  i0 = l0;
  i1 = p1;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f66(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  i0 = 0u;
  i1 = 8959u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B5:;
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0));
  i0 = !(j0);
  if (i0) {goto B1;}
  goto B2;
  B3:;
  i0 = 0u;
  i1 = 8837u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  j0 = i64_load((&M0), (u64)(i0));
  i0 = !(j0);
  if (i0) {goto B1;}
  B2:;
  i0 = 0u;
  i1 = 8913u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B1:;
  i0 = l1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 9417u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 9451u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B10:;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 20));
  i1 = l0;
  i2 = 40u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = (*Z_envZ_db_next_i64Z_iii)(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B13;}
  i0 = l0;
  i1 = p1;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f66(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  i0 = l0;
  i1 = l1;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  f78(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 24));
  p2 = i0;
  i0 = !(i0);
  if (i0) {goto B17;}
  i0 = l0;
  i1 = 28u;
  i0 += i1;
  p0 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B19;}
  L20: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = 4294967272u;
    i0 += i1;
    l1 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    p1 = i0;
    i0 = l1;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = p1;
    i0 = !(i0);
    if (i0) {goto B21;}
    i0 = p1;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    f48(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B21:;
    i0 = p2;
    i1 = l1;
    i0 = i0 != i1;
    if (i0) {goto L20;}
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  goto B18;
  B19:;
  i0 = p2;
  l1 = i0;
  B18:;
  i0 = p0;
  i1 = p2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B17:;
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f78(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l5 = 0, l6 = 0, l7 = 0;
  u64 l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 9481u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  j0 = (*Z_envZ_current_receiverZ_jv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B2;}
  i0 = 0u;
  i1 = 9526u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B2:;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 24));
  l0 = i0;
  i1 = p0;
  i2 = 28u;
  i1 += i2;
  l1 = i1;
  i1 = i32_load((&M0), (u64)(i1));
  l2 = i1;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  i0 = l2;
  l3 = i0;
  i0 = l2;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  j0 = i64_load((&M0), (u64)(i0 + 8));
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  l4 = j1;
  j0 ^= j1;
  j1 = 256ull;
  i0 = j0 < j1;
  if (i0) {goto B11;}
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  l5 = i0;
  i0 = l2;
  l6 = i0;
  L12: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = l6;
    i0 = i0 == i1;
    if (i0) {goto B10;}
    i0 = l6;
    i1 = 4294967248u;
    i0 += i1;
    l7 = i0;
    i0 = l6;
    i1 = 4294967272u;
    i0 += i1;
    l3 = i0;
    l6 = i0;
    i0 = l7;
    i0 = i32_load((&M0), (u64)(i0));
    j0 = i64_load((&M0), (u64)(i0 + 8));
    j1 = l4;
    j0 ^= j1;
    j1 = 256ull;
    i0 = j0 >= j1;
    if (i0) {goto L12;}
  B11:;
  i0 = l0;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  i0 = 4294967272u;
  l7 = i0;
  i0 = l3;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B8;}
  goto B7;
  B10:;
  i0 = l0;
  l3 = i0;
  B9:;
  i0 = 0u;
  i1 = 9576u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 4294967272u;
  l7 = i0;
  i0 = l3;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  l2 = i1;
  i0 = i0 != i1;
  if (i0) {goto B7;}
  B8:;
  i0 = l3;
  i1 = l7;
  i0 += i1;
  l5 = i0;
  goto B6;
  B7:;
  i0 = l3;
  l6 = i0;
  L14: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i0 = i32_load((&M0), (u64)(i0));
    l5 = i0;
    i0 = l6;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l6;
    i1 = l7;
    i0 += i1;
    l0 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l3 = i0;
    i0 = l0;
    i1 = l5;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto B15;}
    i0 = l3;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    f48(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B15:;
    i0 = l6;
    i1 = 4294967288u;
    i0 += i1;
    i1 = l6;
    i2 = 16u;
    i1 += i2;
    i1 = i32_load((&M0), (u64)(i1));
    i32_store((&M0), (u64)(i0), i1);
    i0 = l6;
    i1 = 4294967280u;
    i0 += i1;
    i1 = l6;
    i2 = 8u;
    i1 += i2;
    j1 = i64_load((&M0), (u64)(i1));
    i64_store((&M0), (u64)(i0), j1);
    i0 = l2;
    i1 = l6;
    i2 = 24u;
    i1 += i2;
    l6 = i1;
    i0 = i0 != i1;
    if (i0) {goto L14;}
  i0 = l6;
  i1 = 4294967272u;
  i0 += i1;
  l5 = i0;
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i1 = 24u;
  i0 += i1;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  B6:;
  L17: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = l7;
    i0 += i1;
    l3 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l6 = i0;
    i0 = l3;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l6;
    i0 = !(i0);
    if (i0) {goto B18;}
    i0 = l6;
    i1 = g3;
    i1 = !(i1);
    if (i1) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i1 = 4294967295u;
      i2 = g3;
      i1 += i2;
      g3 = i1;
    }
    f48(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B18:;
    i0 = l5;
    i1 = l3;
    i0 = i0 != i1;
    if (i0) {goto L17;}
  B5:;
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i1 = l5;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 20));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  (*Z_envZ_db_remove_i64Z_vi)(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static void apply(u64 p0, u64 p1, u64 p2) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 80u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  f39();
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = p1;
  j1 = p0;
  i0 = j0 != j1;
  if (i0) {goto B1;}
  j0 = p2;
  j1 = 14829575313431724031ull;
  i0 = (u64)((s64)j0 <= (s64)j1);
  if (i0) {goto B5;}
  j0 = p2;
  j1 = 14829575313431724032ull;
  i0 = j0 == j1;
  if (i0) {goto B4;}
  j0 = p2;
  j1 = 8516769789752901632ull;
  i0 = j0 == j1;
  if (i0) {goto B3;}
  j0 = p2;
  j1 = 5031766152489992192ull;
  i0 = j0 != j1;
  if (i0) {goto B1;}
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 76), i1);
  i0 = l0;
  i1 = 1u;
  i32_store((&M0), (u64)(i0 + 72), i1);
  i0 = l0;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 72));
  i64_store((&M0), (u64)(i0), j1);
  j0 = p1;
  j1 = p1;
  i2 = l0;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = f80(j0, j1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B1;
  B5:;
  j0 = p2;
  j1 = 11913481165836648448ull;
  i0 = j0 == j1;
  if (i0) {goto B2;}
  j0 = p2;
  j1 = 13453074143696125952ull;
  i0 = j0 != j1;
  if (i0) {goto B1;}
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 44), i1);
  i0 = l0;
  i1 = 2u;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = l0;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 40));
  i64_store((&M0), (u64)(i0 + 32), j1);
  j0 = p1;
  j1 = p1;
  i2 = l0;
  i3 = 32u;
  i2 += i3;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = f81(j0, j1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B1;
  B4:;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 60), i1);
  i0 = l0;
  i1 = 3u;
  i32_store((&M0), (u64)(i0 + 56), i1);
  i0 = l0;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 56));
  i64_store((&M0), (u64)(i0 + 16), j1);
  j0 = p1;
  j1 = p1;
  i2 = l0;
  i3 = 16u;
  i2 += i3;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = f82(j0, j1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B1;
  B3:;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 68), i1);
  i0 = l0;
  i1 = 4u;
  i32_store((&M0), (u64)(i0 + 64), i1);
  i0 = l0;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 64));
  i64_store((&M0), (u64)(i0 + 8), j1);
  j0 = p1;
  j1 = p1;
  i2 = l0;
  i3 = 8u;
  i2 += i3;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = f83(j0, j1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B1;
  B2:;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 52), i1);
  i0 = l0;
  i1 = 5u;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = l0;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 48));
  i64_store((&M0), (u64)(i0 + 24), j1);
  j0 = p1;
  j1 = p1;
  i2 = l0;
  i3 = 24u;
  i2 += i3;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = f84(j0, j1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B1:;
  i0 = 0u;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f57(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 80u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f80(u64 p0, u64 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 128u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p2;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l2 = i0;
  i0 = p2;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B2;}
  i0 = l4;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  goto B1;
  B3:;
  i0 = 0u;
  p2 = i0;
  goto B0;
  B2:;
  i0 = l0;
  i1 = l4;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  p2 = i0;
  g0 = i0;
  B1:;
  i0 = p2;
  i1 = l4;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l1;
  i1 = 80u;
  i0 += i1;
  l5 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 64), j1);
  i0 = l1;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 52), i1);
  i0 = l1;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = l1;
  i1 = p2;
  i2 = l4;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 56), i1);
  i0 = l1;
  i1 = l1;
  i2 = 48u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 104), i1);
  i0 = l1;
  i1 = l1;
  i2 = 64u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = l1;
  i2 = 104u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  f85(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  l6 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1 + 56));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 48));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 88u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i1 = l6;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 88), j1);
  i0 = l1;
  i1 = 104u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l6 = i0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  l7 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  i1 = l7;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 88));
  p0 = j1;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 104), j1);
  i0 = l0;
  i1 = l5;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 72));
  i64_store((&M0), (u64)(i0 + 88), j1);
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0 + 64));
  p0 = j0;
  i0 = l6;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 120), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 88));
  i64_store((&M0), (u64)(i0 + 104), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = l2;
  i2 = 1u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  i0 += i1;
  l0 = i0;
  i0 = l2;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B8;}
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l3;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B8:;
  i0 = l0;
  i1 = l1;
  i2 = 120u;
  i1 += i2;
  i2 = l1;
  i3 = 104u;
  i2 += i3;
  i3 = l3;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  CALL_INDIRECT(T0, void (*)(u32, u32, u32), 1, i3, i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B10;}
  i0 = p2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f41(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B10:;
  i0 = l1;
  i1 = 128u;
  i0 += i1;
  g0 = i0;
  i0 = 1u;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f81(u64 p0, u64 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 144u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p2;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l2 = i0;
  i0 = p2;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = 0u;
  p2 = i0;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
  i0 = l4;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  goto B2;
  B3:;
  i0 = l0;
  i1 = l4;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  p2 = i0;
  g0 = i0;
  B2:;
  i0 = p2;
  i1 = l4;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l1;
  i1 = 64u;
  i0 += i1;
  i1 = 24u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 64), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 80), j1);
  i0 = l1;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 52), i1);
  i0 = l1;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = l1;
  i1 = p2;
  i2 = l4;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 56), i1);
  i0 = l1;
  i1 = l1;
  i2 = 48u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 128), i1);
  i0 = l1;
  i1 = l1;
  i2 = 64u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = l1;
  i2 = 128u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  f89(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  l5 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1 + 56));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 48));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 112u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i1 = l5;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 112), j1);
  i0 = l1;
  i1 = 128u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l6 = i0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  l5 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = 24u;
  i0 += i1;
  i1 = l5;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 112));
  p0 = j1;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 128), j1);
  i0 = l0;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 72));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 64));
  i64_store((&M0), (u64)(i0 + 112), j1);
  i0 = l1;
  i1 = 96u;
  i0 += i1;
  i1 = l1;
  i2 = 80u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f55(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
  i0 = l6;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 112));
  i64_store((&M0), (u64)(i0 + 128), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = l2;
  i2 = 1u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  i0 += i1;
  l0 = i0;
  i0 = l2;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B8;}
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l3;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B8:;
  i0 = l0;
  i1 = l1;
  i2 = 128u;
  i1 += i2;
  i2 = l5;
  i3 = l3;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  CALL_INDIRECT(T0, void (*)(u32, u32, u32), 1, i3, i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 96));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B14;}
  i0 = l4;
  i1 = 513u;
  i0 = i0 >= i1;
  if (i0) {goto B13;}
  goto B12;
  B14:;
  i0 = l5;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B12;}
  B13:;
  i0 = p2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f41(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 1u;
  p2 = i0;
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 80));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B11;}
  goto B10;
  B12:;
  i0 = 1u;
  p2 = i0;
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 80));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B10;}
  B11:;
  i0 = l1;
  i1 = 144u;
  i0 += i1;
  g0 = i0;
  i0 = p2;
  goto Bfunc;
  B10:;
  i0 = l1;
  i1 = 88u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 144u;
  i0 += i1;
  g0 = i0;
  i0 = p2;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f82(u64 p0, u64 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 176u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p2;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l2 = i0;
  i0 = p2;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = 0u;
  p2 = i0;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
  i0 = l4;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  goto B2;
  B3:;
  i0 = l0;
  i1 = l4;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  p2 = i0;
  g0 = i0;
  B2:;
  i0 = p2;
  i1 = l4;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l1;
  i1 = 64u;
  i0 += i1;
  i1 = 24u;
  i0 += i1;
  l5 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 104u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 64), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 80), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 96), j1);
  i0 = l1;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 52), i1);
  i0 = l1;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = l1;
  i1 = p2;
  i2 = l4;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 56), i1);
  i0 = l1;
  i1 = l1;
  i2 = 48u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 144), i1);
  i0 = l1;
  i1 = l1;
  i2 = 64u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = l1;
  i2 = 144u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  f87(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  l6 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1 + 56));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 48));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 128u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i1 = l6;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 128), j1);
  i0 = l1;
  i1 = 144u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l6 = i0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  l7 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = 24u;
  i0 += i1;
  i1 = l7;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 128));
  p0 = j1;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 144), j1);
  i0 = l0;
  i1 = l5;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 80));
  i64_store((&M0), (u64)(i0 + 128), j1);
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0 + 72));
  p0 = j0;
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0 + 64));
  p1 = j0;
  i0 = l1;
  i1 = 112u;
  i0 += i1;
  i1 = l1;
  i2 = 96u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f55(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
  i0 = l6;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 168), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 160), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 128));
  i64_store((&M0), (u64)(i0 + 144), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = l2;
  i2 = 1u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  i0 += i1;
  l0 = i0;
  i0 = l2;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B8;}
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l3;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B8:;
  i0 = l0;
  i1 = l1;
  i2 = 168u;
  i1 += i2;
  i2 = l1;
  i3 = 160u;
  i2 += i3;
  i3 = l1;
  i4 = 144u;
  i3 += i4;
  i4 = l5;
  i5 = l3;
  i6 = g3;
  i6 = !(i6);
  if (i6) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i6 = 4294967295u;
    i7 = g3;
    i6 += i7;
    g3 = i6;
  }
  CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32, u32), 2, i5, i0, i1, i2, i3, i4);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 112));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B14;}
  i0 = l4;
  i1 = 513u;
  i0 = i0 >= i1;
  if (i0) {goto B13;}
  goto B12;
  B14:;
  i0 = l5;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B12;}
  B13:;
  i0 = p2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f41(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 1u;
  p2 = i0;
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 96));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B11;}
  goto B10;
  B12:;
  i0 = 1u;
  p2 = i0;
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 96));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B10;}
  B11:;
  i0 = l1;
  i1 = 176u;
  i0 += i1;
  g0 = i0;
  i0 = p2;
  goto Bfunc;
  B10:;
  i0 = l1;
  i1 = 104u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 176u;
  i0 += i1;
  g0 = i0;
  i0 = p2;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f83(u64 p0, u64 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0, 
      l8 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 160u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p2;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l2 = i0;
  i0 = p2;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = 0u;
  p2 = i0;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
  i0 = l4;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  goto B2;
  B3:;
  i0 = l0;
  i1 = l4;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  p2 = i0;
  g0 = i0;
  B2:;
  i0 = p2;
  i1 = l4;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l1;
  i1 = 80u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 96u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 64), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 88), j1);
  i0 = l1;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 52), i1);
  i0 = l1;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = l1;
  i1 = p2;
  i2 = l4;
  i1 += i2;
  l5 = i1;
  i32_store((&M0), (u64)(i0 + 56), i1);
  i0 = p2;
  l0 = i0;
  i0 = l4;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B6;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 56u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 52));
  l0 = i0;
  B6:;
  i0 = l1;
  i1 = 64u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0 + 52), i1);
  i0 = l1;
  i1 = 64u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l6 = i0;
  i0 = l5;
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 48u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 52));
  l0 = i0;
  B9:;
  i0 = l6;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0 + 52), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l5;
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B12;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 52));
  l0 = i0;
  B12:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 80u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 52), i1);
  i0 = l1;
  i1 = 48u;
  i0 += i1;
  i1 = l1;
  i2 = 64u;
  i1 += i2;
  i2 = 24u;
  i1 += i2;
  l5 = i1;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f86(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  l7 = i0;
  i1 = l1;
  i2 = 48u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 48));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 120u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i1 = l7;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 120), j1);
  i0 = l1;
  i1 = 136u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l7 = i0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  l8 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = 24u;
  i0 += i1;
  i1 = l8;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 120));
  p0 = j1;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 136), j1);
  i0 = l0;
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = l6;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 120), j1);
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0 + 64));
  p0 = j0;
  i0 = l1;
  i1 = 104u;
  i0 += i1;
  i1 = l5;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f55(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  i0 = l7;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 152), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 120));
  i64_store((&M0), (u64)(i0 + 136), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = l2;
  i2 = 1u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  i0 += i1;
  l0 = i0;
  i0 = l2;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B17;}
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l3;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B17:;
  i0 = l0;
  i1 = l1;
  i2 = 152u;
  i1 += i2;
  i2 = l1;
  i3 = 136u;
  i2 += i3;
  i3 = l6;
  i4 = l3;
  i5 = g3;
  i5 = !(i5);
  if (i5) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i5 = 4294967295u;
    i6 = g3;
    i5 += i6;
    g3 = i5;
  }
  CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 3, i4, i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 104));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B23;}
  i0 = l4;
  i1 = 513u;
  i0 = i0 >= i1;
  if (i0) {goto B22;}
  goto B21;
  B23:;
  i0 = l6;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B21;}
  B22:;
  i0 = p2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f41(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 1u;
  p2 = i0;
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 88));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B20;}
  goto B19;
  B21:;
  i0 = 1u;
  p2 = i0;
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 88));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B19;}
  B20:;
  i0 = l1;
  i1 = 160u;
  i0 += i1;
  g0 = i0;
  i0 = p2;
  goto Bfunc;
  B19:;
  i0 = l1;
  i1 = 96u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 160u;
  i0 += i1;
  g0 = i0;
  i0 = p2;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f84(u64 p0, u64 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 128u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p2;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l2 = i0;
  i0 = p2;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B2;}
  i0 = l4;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  goto B1;
  B3:;
  i0 = 0u;
  p2 = i0;
  goto B0;
  B2:;
  i0 = l0;
  i1 = l4;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  p2 = i0;
  g0 = i0;
  B1:;
  i0 = p2;
  i1 = l4;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 64), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 80), j1);
  i0 = l1;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 52), i1);
  i0 = l1;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = l1;
  i1 = p2;
  i2 = l4;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 56), i1);
  i0 = l1;
  i1 = l1;
  i2 = 48u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 112), i1);
  i0 = l1;
  i1 = l1;
  i2 = 64u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = l1;
  i2 = 112u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  f88(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1 + 56));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 48));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 96u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l5 = i0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 96), j1);
  i0 = l1;
  i1 = 112u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l5;
  i1 = i32_load((&M0), (u64)(i1));
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 96));
  p0 = j1;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 112), j1);
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0 + 80));
  p0 = j0;
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0 + 64));
  p1 = j0;
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 72));
  i64_store((&M0), (u64)(i0 + 96), j1);
  i0 = l1;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 112), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 88), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = l2;
  i2 = 1u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  i0 += i1;
  l0 = i0;
  i0 = l2;
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B8;}
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l3;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B8:;
  i0 = l0;
  i1 = l1;
  i2 = 112u;
  i1 += i2;
  i2 = l1;
  i3 = 96u;
  i2 += i3;
  i3 = l1;
  i4 = 88u;
  i3 += i4;
  i4 = l3;
  i5 = g3;
  i5 = !(i5);
  if (i5) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i5 = 4294967295u;
    i6 = g3;
    i5 += i6;
    g3 = i5;
  }
  CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 3, i4, i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B10;}
  i0 = p2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f41(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B10:;
  i0 = l1;
  i1 = 128u;
  i0 += i1;
  g0 = i0;
  i0 = 1u;
  FUNC_EPILOGUE;
  return i0;
}

static void f85(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l3 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B0:;
  i0 = l1;
  i1 = l3;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  l2 = i0;
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  p0 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p0 = i0;
  B3:;
  i0 = l1;
  i1 = p0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  p0 = i0;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  p0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l2;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = p0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B6;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p0 = i0;
  B6:;
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = p0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = 16u;
  i0 += i1;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  l2 = i0;
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f86(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 32u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = p0;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f92(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 20));
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1 + 16));
  l1 = i1;
  i0 -= i1;
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B8;}
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l2;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B3;}
  i0 = l2;
  i1 = 10u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = l0;
  i1 = l2;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 1u;
  i0 |= i1;
  l3 = i0;
  goto B6;
  B8:;
  i0 = p1;
  i0 = i32_load8_u((&M0), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B5;}
  i0 = p1;
  i1 = 0u;
  i32_store16((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  goto B4;
  B7:;
  i0 = l2;
  i1 = 16u;
  i0 += i1;
  i1 = 4294967280u;
  i0 &= i1;
  l4 = i0;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f46(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = l0;
  i1 = l4;
  i2 = 1u;
  i1 |= i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 4), i1);
  B6:;
  i0 = l2;
  l5 = i0;
  i0 = l3;
  l4 = i0;
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = l1;
    i1 = i32_load8_u((&M0), (u64)(i1));
    i32_store8((&M0), (u64)(i0), i1);
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i0 = l1;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i0 = l5;
    i1 = 4294967295u;
    i0 += i1;
    l5 = i0;
    if (i0) {goto L10;}
  i0 = l3;
  i1 = l2;
  i0 += i1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p1;
  i0 = i32_load8_u((&M0), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B12;}
  i0 = p1;
  i1 = 0u;
  i32_store16((&M0), (u64)(i0), i1);
  goto B11;
  B12:;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 4), i1);
  B11:;
  i0 = p1;
  i1 = 0u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  f56(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto B1;}
  goto B2;
  B5:;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  B4:;
  i0 = p1;
  i1 = 0u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  f56(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  l1 = i0;
  if (i0) {goto B2;}
  goto B1;
  B3:;
  i0 = l0;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f54(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B2:;
  i0 = l0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B1:;
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  g0 = i0;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f87(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l3 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B0:;
  i0 = l1;
  i1 = l3;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  l2 = i0;
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  p0 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p0 = i0;
  B3:;
  i0 = l1;
  i1 = p0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  l2 = i0;
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l3;
  i1 = 16u;
  i0 += i1;
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  p0 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B6;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p0 = i0;
  B6:;
  i0 = l1;
  i1 = p0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  p0 = i0;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l2;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  B9:;
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = 24u;
  i0 += i1;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l3;
  i2 = 32u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f86(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f88(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l3 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B0:;
  i0 = l1;
  i1 = l3;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  l2 = i0;
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  p0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l2;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l3 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B3:;
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  l2 = i0;
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  p0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  p1 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B6;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p1 = i0;
  B6:;
  i0 = p0;
  i1 = p1;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  l2 = i0;
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f89(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l3 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B0:;
  i0 = l1;
  i1 = l3;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  l3 = i0;
  i1 = l3;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l4 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l2;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l4;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  l4 = i0;
  B3:;
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l3;
  i1 = l3;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 16u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f86(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f90(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 64u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = l2;
  i1 = 512u;
  i0 = i0 < i1;
  if (i0) {goto B2;}
  i0 = l2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B1;
  B3:;
  i0 = 0u;
  l0 = i0;
  goto B0;
  B2:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  B1:;
  i0 = l0;
  i1 = l2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 56), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B7:;
  i0 = l1;
  i1 = 56u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 4294967288u;
  i0 &= i1;
  l4 = i0;
  i1 = 8u;
  i0 = i0 != i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B10:;
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  i1 = l3;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = 24u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = 28u;
  i0 += i1;
  i1 = l0;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  i2 = 56u;
  i1 += i2;
  i2 = l1;
  i3 = 40u;
  i2 += i3;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  f59(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 64u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f91(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 144u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = l2;
  i1 = 512u;
  i0 = i0 < i1;
  if (i0) {goto B2;}
  i0 = l2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B1;
  B3:;
  i0 = 0u;
  l0 = i0;
  goto B0;
  B2:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  B1:;
  i0 = l0;
  i1 = l2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 100), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 96), i1);
  i0 = l1;
  i1 = l0;
  i2 = l2;
  i1 += i2;
  l3 = i1;
  i32_store((&M0), (u64)(i0 + 104), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 88), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 104u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 100));
  l0 = i0;
  B7:;
  i0 = l1;
  i1 = 88u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0 + 100), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 80), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l3;
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 96u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 100));
  l0 = i0;
  B10:;
  i0 = l1;
  i1 = 72u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0 + 100), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l3;
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 100));
  l0 = i0;
  B13:;
  i0 = l1;
  i1 = 24u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 72u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 100), i1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 64), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 56), j1);
  i0 = l1;
  i1 = 96u;
  i0 += i1;
  i1 = l1;
  i2 = 56u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f86(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i1 = l1;
  i2 = 96u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 96));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 112u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l2 = i0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0 + 112), j1);
  i0 = l1;
  i1 = 128u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1));
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = 48u;
  i0 += i1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l1;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 112));
  p0 = j1;
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 128), j1);
  i0 = l1;
  i1 = 24u;
  i0 += i1;
  i1 = l1;
  i2 = 88u;
  i1 += i2;
  i2 = l1;
  i3 = 72u;
  i2 += i3;
  i3 = l1;
  i4 = 56u;
  i3 += i4;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  f63(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 56));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B18;}
  i0 = l1;
  i1 = 56u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B18:;
  i0 = l1;
  i1 = 144u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f92(u32 p0, u32 p1) {
  u32 l0 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l0 = i0;
  j0 = 0ull;
  l1 = j0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l3 = i0;
  i0 = 0u;
  l4 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i1 = l2;
    i1 = i32_load((&M0), (u64)(i1));
    i0 = i0 < i1;
    if (i0) {goto B1;}
    i0 = 0u;
    i1 = 9634u;
    i2 = g3;
    i2 = !(i2);
    if (i2) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i2 = 4294967295u;
      i3 = g3;
      i2 += i3;
      g3 = i2;
    }
    (*Z_envZ_eosio_assertZ_vii)(i0, i1);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i0 = i32_load((&M0), (u64)(i0));
    l0 = i0;
    B1:;
    i0 = l0;
    i0 = i32_load8_u((&M0), (u64)(i0));
    l5 = i0;
    i0 = l3;
    i1 = l0;
    i2 = 1u;
    i1 += i2;
    l6 = i1;
    i32_store((&M0), (u64)(i0), i1);
    j0 = l1;
    i1 = l5;
    i2 = 127u;
    i1 &= i2;
    i2 = l4;
    i3 = 255u;
    i2 &= i3;
    l0 = i2;
    i1 <<= (i2 & 31);
    j1 = (u64)(i1);
    j0 |= j1;
    l1 = j0;
    i0 = l0;
    i1 = 7u;
    i0 += i1;
    l4 = i0;
    i0 = l6;
    l0 = i0;
    i0 = l5;
    i1 = 128u;
    i0 &= i1;
    if (i0) {goto L0;}
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l5 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1));
  l0 = i1;
  i0 -= i1;
  l3 = i0;
  j1 = l1;
  i1 = (u32)(j1);
  l4 = i1;
  i0 = i0 >= i1;
  if (i0) {goto B4;}
  i0 = p1;
  i1 = l4;
  i2 = l3;
  i1 -= i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  f98(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l6 = i0;
  i0 = p1;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  goto B3;
  B4:;
  i0 = l3;
  i1 = l4;
  i0 = i0 <= i1;
  if (i0) {goto B3;}
  i0 = p1;
  i1 = 4u;
  i0 += i1;
  i1 = l0;
  i2 = l4;
  i1 += i2;
  l5 = i1;
  i32_store((&M0), (u64)(i0), i1);
  B3:;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l6;
  i0 -= i1;
  i1 = l5;
  i2 = l0;
  i1 -= i2;
  l5 = i1;
  i0 = i0 >= i1;
  if (i0) {goto B6;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l6 = i0;
  B6:;
  i0 = l0;
  i1 = l6;
  i2 = l5;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l0 = i0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = l5;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f93(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 128u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = l2;
  i1 = 512u;
  i0 = i0 < i1;
  if (i0) {goto B2;}
  i0 = l2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B1;
  B3:;
  i0 = 0u;
  l0 = i0;
  goto B0;
  B2:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  B1:;
  i0 = l0;
  i1 = l2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 84), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l1;
  i1 = l0;
  i2 = l2;
  i1 += i2;
  l3 = i1;
  i32_store((&M0), (u64)(i0 + 88), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 64), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 88u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 84));
  l0 = i0;
  B7:;
  i0 = l1;
  i1 = 64u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0 + 84), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l3;
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 84));
  l0 = i0;
  B10:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 64u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 84), i1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 56), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l1;
  i1 = 80u;
  i0 += i1;
  i1 = l1;
  i2 = 48u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f86(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i1 = l1;
  i2 = 80u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 80));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 96u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l2 = i0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 96), j1);
  i0 = l1;
  i1 = 112u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1));
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 96));
  p0 = j1;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 112), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = l1;
  i2 = 64u;
  i1 += i2;
  i2 = l1;
  i3 = 48u;
  i2 += i3;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  f69(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 48));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B15;}
  i0 = l1;
  i1 = 56u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B15:;
  i0 = l1;
  i1 = 128u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f94(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 144u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = l2;
  i1 = 512u;
  i0 = i0 < i1;
  if (i0) {goto B2;}
  i0 = l2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B1;
  B3:;
  i0 = 0u;
  l0 = i0;
  goto B0;
  B2:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  B1:;
  i0 = l0;
  i1 = l2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 100), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 96), i1);
  i0 = l1;
  i1 = l0;
  i2 = l2;
  i1 += i2;
  l3 = i1;
  i32_store((&M0), (u64)(i0 + 104), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 88), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 104u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 100));
  l0 = i0;
  B7:;
  i0 = l1;
  i1 = 88u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0 + 100), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 80), j1);
  i0 = l3;
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 96u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 100));
  l0 = i0;
  B10:;
  i0 = l1;
  i1 = 80u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0 + 100), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 64), j1);
  i0 = l3;
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 104u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 100));
  l0 = i0;
  B13:;
  i0 = l1;
  i1 = 64u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0 + 100), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l3;
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 100));
  l0 = i0;
  B16:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 64u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 100), i1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 56), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l1;
  i1 = 96u;
  i0 += i1;
  i1 = l1;
  i2 = 48u;
  i1 += i2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = f86(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i1 = l1;
  i2 = 96u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 96));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 112u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 112), j1);
  i0 = l1;
  i1 = 128u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i1 = i32_load((&M0), (u64)(i1));
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 112));
  p0 = j1;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 128), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = l1;
  i2 = 88u;
  i1 += i2;
  i2 = l1;
  i3 = 80u;
  i2 += i3;
  i3 = l1;
  i4 = 64u;
  i3 += i4;
  i4 = l1;
  i5 = 48u;
  i4 += i5;
  i5 = g3;
  i5 = !(i5);
  if (i5) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i5 = 4294967295u;
    i6 = g3;
    i5 += i6;
    g3 = i5;
  }
  f73(i0, i1, i2, i3, i4);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 48));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B21;}
  i0 = l1;
  i1 = 56u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B21:;
  i0 = l1;
  i1 = 144u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f95(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 64u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = l2;
  i1 = 512u;
  i0 = i0 < i1;
  if (i0) {goto B2;}
  i0 = l2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B1;
  B3:;
  i0 = 0u;
  l0 = i0;
  goto B0;
  B2:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  B1:;
  i0 = l0;
  i1 = l2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 56), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B7:;
  i0 = l1;
  i1 = 56u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 4294967288u;
  i0 &= i1;
  l4 = i0;
  i1 = 8u;
  i0 = i0 != i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B10:;
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  i1 = l3;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = 24u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = 28u;
  i0 += i1;
  i1 = l0;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  i2 = 56u;
  i1 += i2;
  i2 = l1;
  i3 = 48u;
  i2 += i3;
  i3 = l1;
  i4 = 40u;
  i3 += i4;
  i4 = g3;
  i4 = !(i4);
  if (i4) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i4 = 4294967295u;
    i5 = g3;
    i4 += i5;
    g3 = i4;
  }
  f75(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 64u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f96(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 48u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = g3;
  i0 = !(i0);
  if (i0) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i0 = 4294967295u;
    i1 = g3;
    i0 += i1;
    g3 = i0;
  }
  i0 = (*Z_envZ_action_data_sizeZ_iv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = l2;
  i1 = 512u;
  i0 = i0 < i1;
  if (i0) {goto B2;}
  i0 = l2;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f40(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B1;
  B3:;
  i0 = 0u;
  l0 = i0;
  goto B0;
  B2:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  B1:;
  i0 = l0;
  i1 = l2;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  i0 = (*Z_envZ_read_action_dataZ_iii)(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B7:;
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l3 = i0;
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  l4 = i0;
  i0 = l2;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = 8u;
  i0 = i0 != i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B10:;
  i0 = l1;
  i1 = l4;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l1;
  i1 = 24u;
  i0 += i1;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = 20u;
  i0 += i1;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = l1;
  i2 = 40u;
  i1 += i2;
  i2 = l1;
  i3 = 32u;
  i2 += i3;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  f77(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f97(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l3 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B0:;
  i0 = l1;
  i1 = l3;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  l3 = i0;
  i1 = l3;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l4 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l2;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l4;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  l4 = i0;
  B3:;
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l3;
  i1 = l3;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l3 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B6;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B6:;
  i0 = l1;
  i1 = l3;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  l3 = i0;
  i1 = l3;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l4 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l2;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l4;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  l4 = i0;
  B9:;
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l3;
  i1 = l3;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  p0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l3 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B12;}
  i0 = 0u;
  i1 = 9629u;
  i2 = g3;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g3;
    i2 += i3;
    g3 = i2;
  }
  (*Z_envZ_eosio_assertZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  B12:;
  i0 = p0;
  i1 = l3;
  i2 = 8u;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = 4u;
  i0 += i1;
  l2 = i0;
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f98(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  l0 = i0;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l1 = i1;
  i0 -= i1;
  i1 = p1;
  i0 = i0 >= i1;
  if (i0) {goto B4;}
  i0 = l1;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1));
  l2 = i1;
  i0 -= i1;
  l3 = i0;
  i1 = p1;
  i0 += i1;
  l4 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 <= (s32)i1);
  if (i0) {goto B2;}
  i0 = 2147483647u;
  l5 = i0;
  i0 = l0;
  i1 = l2;
  i0 -= i1;
  l0 = i0;
  i1 = 1073741822u;
  i0 = i0 > i1;
  if (i0) {goto B5;}
  i0 = l4;
  i1 = l0;
  i2 = 1u;
  i1 <<= (i2 & 31);
  l0 = i1;
  i2 = l0;
  i3 = l4;
  i2 = i2 < i3;
  i0 = i2 ? i0 : i1;
  l5 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  B5:;
  i0 = l5;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  i0 = f46(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
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
    i0 = l1;
    i1 = 0u;
    i32_store8((&M0), (u64)(i0), i1);
    i0 = p0;
    i1 = p0;
    i1 = i32_load((&M0), (u64)(i1));
    i2 = 1u;
    i1 += i2;
    l1 = i1;
    i32_store((&M0), (u64)(i0), i1);
    i0 = p1;
    i1 = 4294967295u;
    i0 += i1;
    p1 = i0;
    if (i0) {goto L7;}
    goto B0;
  B3:;
  i0 = 0u;
  l5 = i0;
  i0 = 0u;
  l0 = i0;
  goto B1;
  B2:;
  i0 = p0;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f58(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B1:;
  i0 = l0;
  i1 = l5;
  i0 += i1;
  l5 = i0;
  i0 = l1;
  i1 = p1;
  i0 += i1;
  i1 = l2;
  i0 -= i1;
  l2 = i0;
  i0 = l0;
  i1 = l3;
  i0 += i1;
  l3 = i0;
  l1 = i0;
  L9: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = 0u;
    i32_store8((&M0), (u64)(i0), i1);
    i0 = l1;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i0 = p1;
    i1 = 4294967295u;
    i0 += i1;
    p1 = i0;
    if (i0) {goto L9;}
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l2 = i0;
  i0 = l3;
  i1 = p0;
  i2 = 4u;
  i1 += i2;
  l4 = i1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = p0;
  i2 = i32_load((&M0), (u64)(i2));
  p1 = i2;
  i1 -= i2;
  l1 = i1;
  i0 -= i1;
  l0 = i0;
  i0 = l1;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B10;}
  i0 = l0;
  i1 = p1;
  i2 = l1;
  i3 = g3;
  i3 = !(i3);
  if (i3) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i3 = 4294967295u;
    i4 = g3;
    i3 += i4;
    g3 = i3;
  }
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  p1 = i0;
  B10:;
  i0 = p0;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l4;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = l5;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = p1;
  i1 = g3;
  i1 = !(i1);
  if (i1) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i1 = 4294967295u;
    i2 = g3;
    i1 += i2;
    g3 = i1;
  }
  f48(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto Bfunc;
  B0:;
  Bfunc:;
  FUNC_EPILOGUE;
}

static const u8 data_segment_data_0[] = {
  0x66, 0x61, 0x69, 0x6c, 0x65, 0x64, 0x20, 0x74, 0x6f, 0x20, 0x61, 0x6c, 
  0x6c, 0x6f, 0x63, 0x61, 0x74, 0x65, 0x20, 0x70, 0x61, 0x67, 0x65, 0x73, 
  0x00, 0x69, 0x6e, 0x76, 0x61, 0x6c, 0x69, 0x64, 0x20, 0x73, 0x79, 0x6d, 
  0x62, 0x6f, 0x6c, 0x20, 0x6e, 0x61, 0x6d, 0x65, 0x00, 
};

static const u8 data_segment_data_1[] = {
  0x69, 0x6e, 0x76, 0x61, 0x6c, 0x69, 0x64, 0x20, 0x73, 0x75, 0x70, 0x70, 
  0x6c, 0x79, 0x00, 
};

static const u8 data_segment_data_2[] = {
  0x6d, 0x61, 0x78, 0x2d, 0x73, 0x75, 0x70, 0x70, 0x6c, 0x79, 0x20, 0x6d, 
  0x75, 0x73, 0x74, 0x20, 0x62, 0x65, 0x20, 0x70, 0x6f, 0x73, 0x69, 0x74, 
  0x69, 0x76, 0x65, 0x00, 
};

static const u8 data_segment_data_3[] = {
  0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x73, 
  0x79, 0x6d, 0x62, 0x6f, 0x6c, 0x20, 0x61, 0x6c, 0x72, 0x65, 0x61, 0x64, 
  0x79, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x73, 0x00, 
};

static const u8 data_segment_data_4[] = {
  0x6d, 0x65, 0x6d, 0x6f, 0x20, 0x68, 0x61, 0x73, 0x20, 0x6d, 0x6f, 0x72, 
  0x65, 0x20, 0x74, 0x68, 0x61, 0x6e, 0x20, 0x32, 0x35, 0x36, 0x20, 0x62, 
  0x79, 0x74, 0x65, 0x73, 0x00, 
};

static const u8 data_segment_data_5[] = {
  0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x73, 
  0x79, 0x6d, 0x62, 0x6f, 0x6c, 0x20, 0x64, 0x6f, 0x65, 0x73, 0x20, 0x6e, 
  0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x2c, 0x20, 0x63, 0x72, 
  0x65, 0x61, 0x74, 0x65, 0x20, 0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x20, 0x62, 
  0x65, 0x66, 0x6f, 0x72, 0x65, 0x20, 0x69, 0x73, 0x73, 0x75, 0x65, 0x00, 
  
};

static const u8 data_segment_data_6[] = {
  0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x73, 0x20, 0x63, 0x61, 0x6e, 0x20, 0x6f, 
  0x6e, 0x6c, 0x79, 0x20, 0x62, 0x65, 0x20, 0x69, 0x73, 0x73, 0x75, 0x65, 
  0x64, 0x20, 0x74, 0x6f, 0x20, 0x69, 0x73, 0x73, 0x75, 0x65, 0x72, 0x20, 
  0x61, 0x63, 0x63, 0x6f, 0x75, 0x6e, 0x74, 0x00, 
};

static const u8 data_segment_data_7[] = {
  0x69, 0x6e, 0x76, 0x61, 0x6c, 0x69, 0x64, 0x20, 0x71, 0x75, 0x61, 0x6e, 
  0x74, 0x69, 0x74, 0x79, 0x00, 
};

static const u8 data_segment_data_8[] = {
  0x6d, 0x75, 0x73, 0x74, 0x20, 0x69, 0x73, 0x73, 0x75, 0x65, 0x20, 0x70, 
  0x6f, 0x73, 0x69, 0x74, 0x69, 0x76, 0x65, 0x20, 0x71, 0x75, 0x61, 0x6e, 
  0x74, 0x69, 0x74, 0x79, 0x00, 
};

static const u8 data_segment_data_9[] = {
  0x73, 0x79, 0x6d, 0x62, 0x6f, 0x6c, 0x20, 0x70, 0x72, 0x65, 0x63, 0x69, 
  0x73, 0x69, 0x6f, 0x6e, 0x20, 0x6d, 0x69, 0x73, 0x6d, 0x61, 0x74, 0x63, 
  0x68, 0x00, 
};

static const u8 data_segment_data_10[] = {
  0x71, 0x75, 0x61, 0x6e, 0x74, 0x69, 0x74, 0x79, 0x20, 0x65, 0x78, 0x63, 
  0x65, 0x65, 0x64, 0x73, 0x20, 0x61, 0x76, 0x61, 0x69, 0x6c, 0x61, 0x62, 
  0x6c, 0x65, 0x20, 0x73, 0x75, 0x70, 0x70, 0x6c, 0x79, 0x00, 
};

static const u8 data_segment_data_11[] = {
  0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x73, 
  0x79, 0x6d, 0x62, 0x6f, 0x6c, 0x20, 0x64, 0x6f, 0x65, 0x73, 0x20, 0x6e, 
  0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x00, 
};

static const u8 data_segment_data_12[] = {
  0x6d, 0x75, 0x73, 0x74, 0x20, 0x72, 0x65, 0x74, 0x69, 0x72, 0x65, 0x20, 
  0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x76, 0x65, 0x20, 0x71, 0x75, 0x61, 
  0x6e, 0x74, 0x69, 0x74, 0x79, 0x00, 
};

static const u8 data_segment_data_13[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x74, 0x72, 0x61, 0x6e, 0x73, 
  0x66, 0x65, 0x72, 0x20, 0x74, 0x6f, 0x20, 0x73, 0x65, 0x6c, 0x66, 0x00, 
  
};

static const u8 data_segment_data_14[] = {
  0x74, 0x6f, 0x20, 0x61, 0x63, 0x63, 0x6f, 0x75, 0x6e, 0x74, 0x20, 0x64, 
  0x6f, 0x65, 0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 
  0x74, 0x00, 
};

static const u8 data_segment_data_15[] = {
  0x75, 0x6e, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x74, 0x6f, 0x20, 0x66, 0x69, 
  0x6e, 0x64, 0x20, 0x6b, 0x65, 0x79, 0x00, 
};

static const u8 data_segment_data_16[] = {
  0x6d, 0x75, 0x73, 0x74, 0x20, 0x74, 0x72, 0x61, 0x6e, 0x73, 0x66, 0x65, 
  0x72, 0x20, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x76, 0x65, 0x20, 0x71, 
  0x75, 0x61, 0x6e, 0x74, 0x69, 0x74, 0x79, 0x00, 
};

static const u8 data_segment_data_17[] = {
  0x6e, 0x6f, 0x20, 0x62, 0x61, 0x6c, 0x61, 0x6e, 0x63, 0x65, 0x20, 0x6f, 
  0x62, 0x6a, 0x65, 0x63, 0x74, 0x20, 0x66, 0x6f, 0x75, 0x6e, 0x64, 0x00, 
  
};

static const u8 data_segment_data_18[] = {
  0x6f, 0x76, 0x65, 0x72, 0x64, 0x72, 0x61, 0x77, 0x6e, 0x20, 0x62, 0x61, 
  0x6c, 0x61, 0x6e, 0x63, 0x65, 0x00, 
};

static const u8 data_segment_data_19[] = {
  0x6f, 0x77, 0x6e, 0x65, 0x72, 0x20, 0x61, 0x63, 0x63, 0x6f, 0x75, 0x6e, 
  0x74, 0x20, 0x64, 0x6f, 0x65, 0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x65, 
  0x78, 0x69, 0x73, 0x74, 0x00, 
};

static const u8 data_segment_data_20[] = {
  0x73, 0x79, 0x6d, 0x62, 0x6f, 0x6c, 0x20, 0x64, 0x6f, 0x65, 0x73, 0x20, 
  0x6e, 0x6f, 0x74, 0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x00, 
};

static const u8 data_segment_data_21[] = {
  0x42, 0x61, 0x6c, 0x61, 0x6e, 0x63, 0x65, 0x20, 0x72, 0x6f, 0x77, 0x20, 
  0x61, 0x6c, 0x72, 0x65, 0x61, 0x64, 0x79, 0x20, 0x64, 0x65, 0x6c, 0x65, 
  0x74, 0x65, 0x64, 0x20, 0x6f, 0x72, 0x20, 0x6e, 0x65, 0x76, 0x65, 0x72, 
  0x20, 0x65, 0x78, 0x69, 0x73, 0x74, 0x65, 0x64, 0x2e, 0x20, 0x41, 0x63, 
  0x74, 0x69, 0x6f, 0x6e, 0x20, 0x77, 0x6f, 0x6e, 0x27, 0x74, 0x20, 0x68, 
  0x61, 0x76, 0x65, 0x20, 0x61, 0x6e, 0x79, 0x20, 0x65, 0x66, 0x66, 0x65, 
  0x63, 0x74, 0x2e, 0x00, 
};

static const u8 data_segment_data_22[] = {
  0x43, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x63, 0x6c, 0x6f, 0x73, 0x65, 
  0x20, 0x62, 0x65, 0x63, 0x61, 0x75, 0x73, 0x65, 0x20, 0x74, 0x68, 0x65, 
  0x20, 0x62, 0x61, 0x6c, 0x61, 0x6e, 0x63, 0x65, 0x20, 0x69, 0x73, 0x20, 
  0x6e, 0x6f, 0x74, 0x20, 0x7a, 0x65, 0x72, 0x6f, 0x2e, 0x00, 
};

static const u8 data_segment_data_23[] = {
  0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x20, 0x70, 0x61, 0x73, 0x73, 0x65, 
  0x64, 0x20, 0x74, 0x6f, 0x20, 0x69, 0x74, 0x65, 0x72, 0x61, 0x74, 0x6f, 
  0x72, 0x5f, 0x74, 0x6f, 0x20, 0x69, 0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 
  0x69, 0x6e, 0x20, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x5f, 0x69, 0x6e, 0x64, 
  0x65, 0x78, 0x00, 
};

static const u8 data_segment_data_24[] = {
  0x65, 0x72, 0x72, 0x6f, 0x72, 0x20, 0x72, 0x65, 0x61, 0x64, 0x69, 0x6e, 
  0x67, 0x20, 0x69, 0x74, 0x65, 0x72, 0x61, 0x74, 0x6f, 0x72, 0x00, 
};

static const u8 data_segment_data_25[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x63, 0x72, 0x65, 0x61, 0x74, 
  0x65, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x73, 0x20, 0x69, 0x6e, 
  0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x6f, 0x66, 0x20, 0x61, 0x6e, 
  0x6f, 0x74, 0x68, 0x65, 0x72, 0x20, 0x63, 0x6f, 0x6e, 0x74, 0x72, 0x61, 
  0x63, 0x74, 0x00, 
};

static const u8 data_segment_data_26[] = {
  0x77, 0x72, 0x69, 0x74, 0x65, 0x00, 
};

static const u8 data_segment_data_27[] = {
  0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x20, 0x70, 0x61, 0x73, 0x73, 0x65, 
  0x64, 0x20, 0x74, 0x6f, 0x20, 0x6d, 0x6f, 0x64, 0x69, 0x66, 0x79, 0x20, 
  0x69, 0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x69, 0x6e, 0x20, 0x6d, 0x75, 
  0x6c, 0x74, 0x69, 0x5f, 0x69, 0x6e, 0x64, 0x65, 0x78, 0x00, 
};

static const u8 data_segment_data_28[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x6d, 0x6f, 0x64, 0x69, 0x66, 
  0x79, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x73, 0x20, 0x69, 0x6e, 
  0x20, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x6f, 0x66, 0x20, 0x61, 0x6e, 
  0x6f, 0x74, 0x68, 0x65, 0x72, 0x20, 0x63, 0x6f, 0x6e, 0x74, 0x72, 0x61, 
  0x63, 0x74, 0x00, 
};

static const u8 data_segment_data_29[] = {
  0x75, 0x70, 0x64, 0x61, 0x74, 0x65, 0x72, 0x20, 0x63, 0x61, 0x6e, 0x6e, 
  0x6f, 0x74, 0x20, 0x63, 0x68, 0x61, 0x6e, 0x67, 0x65, 0x20, 0x70, 0x72, 
  0x69, 0x6d, 0x61, 0x72, 0x79, 0x20, 0x6b, 0x65, 0x79, 0x20, 0x77, 0x68, 
  0x65, 0x6e, 0x20, 0x6d, 0x6f, 0x64, 0x69, 0x66, 0x79, 0x69, 0x6e, 0x67, 
  0x20, 0x61, 0x6e, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x00, 
};

static const u8 data_segment_data_30[] = {
  0x61, 0x74, 0x74, 0x65, 0x6d, 0x70, 0x74, 0x20, 0x74, 0x6f, 0x20, 0x61, 
  0x64, 0x64, 0x20, 0x61, 0x73, 0x73, 0x65, 0x74, 0x20, 0x77, 0x69, 0x74, 
  0x68, 0x20, 0x64, 0x69, 0x66, 0x66, 0x65, 0x72, 0x65, 0x6e, 0x74, 0x20, 
  0x73, 0x79, 0x6d, 0x62, 0x6f, 0x6c, 0x00, 
};

static const u8 data_segment_data_31[] = {
  0x61, 0x64, 0x64, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x75, 0x6e, 0x64, 
  0x65, 0x72, 0x66, 0x6c, 0x6f, 0x77, 0x00, 
};

static const u8 data_segment_data_32[] = {
  0x61, 0x64, 0x64, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x6f, 0x76, 0x65, 
  0x72, 0x66, 0x6c, 0x6f, 0x77, 0x00, 
};

static const u8 data_segment_data_33[] = {
  0x61, 0x74, 0x74, 0x65, 0x6d, 0x70, 0x74, 0x20, 0x74, 0x6f, 0x20, 0x73, 
  0x75, 0x62, 0x74, 0x72, 0x61, 0x63, 0x74, 0x20, 0x61, 0x73, 0x73, 0x65, 
  0x74, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x64, 0x69, 0x66, 0x66, 0x65, 
  0x72, 0x65, 0x6e, 0x74, 0x20, 0x73, 0x79, 0x6d, 0x62, 0x6f, 0x6c, 0x00, 
  
};

static const u8 data_segment_data_34[] = {
  0x73, 0x75, 0x62, 0x74, 0x72, 0x61, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 
  0x75, 0x6e, 0x64, 0x65, 0x72, 0x66, 0x6c, 0x6f, 0x77, 0x00, 
};

static const u8 data_segment_data_35[] = {
  0x73, 0x75, 0x62, 0x74, 0x72, 0x61, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 
  0x6f, 0x76, 0x65, 0x72, 0x66, 0x6c, 0x6f, 0x77, 0x00, 
};

static const u8 data_segment_data_36[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x70, 0x61, 0x73, 0x73, 0x20, 
  0x65, 0x6e, 0x64, 0x20, 0x69, 0x74, 0x65, 0x72, 0x61, 0x74, 0x6f, 0x72, 
  0x20, 0x74, 0x6f, 0x20, 0x65, 0x72, 0x61, 0x73, 0x65, 0x00, 
};

static const u8 data_segment_data_37[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x69, 0x6e, 0x63, 0x72, 0x65, 
  0x6d, 0x65, 0x6e, 0x74, 0x20, 0x65, 0x6e, 0x64, 0x20, 0x69, 0x74, 0x65, 
  0x72, 0x61, 0x74, 0x6f, 0x72, 0x00, 
};

static const u8 data_segment_data_38[] = {
  0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x20, 0x70, 0x61, 0x73, 0x73, 0x65, 
  0x64, 0x20, 0x74, 0x6f, 0x20, 0x65, 0x72, 0x61, 0x73, 0x65, 0x20, 0x69, 
  0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x69, 0x6e, 0x20, 0x6d, 0x75, 0x6c, 
  0x74, 0x69, 0x5f, 0x69, 0x6e, 0x64, 0x65, 0x78, 0x00, 
};

static const u8 data_segment_data_39[] = {
  0x63, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x65, 0x72, 0x61, 0x73, 0x65, 
  0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x73, 0x20, 0x69, 0x6e, 0x20, 
  0x74, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x6f, 0x66, 0x20, 0x61, 0x6e, 0x6f, 
  0x74, 0x68, 0x65, 0x72, 0x20, 0x63, 0x6f, 0x6e, 0x74, 0x72, 0x61, 0x63, 
  0x74, 0x00, 
};

static const u8 data_segment_data_40[] = {
  0x61, 0x74, 0x74, 0x65, 0x6d, 0x70, 0x74, 0x20, 0x74, 0x6f, 0x20, 0x72, 
  0x65, 0x6d, 0x6f, 0x76, 0x65, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63, 0x74, 
  0x20, 0x74, 0x68, 0x61, 0x74, 0x20, 0x77, 0x61, 0x73, 0x20, 0x6e, 0x6f, 
  0x74, 0x20, 0x69, 0x6e, 0x20, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x5f, 0x69, 
  0x6e, 0x64, 0x65, 0x78, 0x00, 
};

static const u8 data_segment_data_41[] = {
  0x72, 0x65, 0x61, 0x64, 0x00, 
};

static const u8 data_segment_data_42[] = {
  0x67, 0x65, 0x74, 0x00, 
};

static const u8 data_segment_data_43[] = {
  0xa8, 0x25, 0x00, 0x00, 
};

static void init_memory(void) {
  wasm_rt_allocate_memory((&M0), 1, 528);
  memcpy(&(M0.data[8220u]), data_segment_data_0, 45);
  memcpy(&(M0.data[8265u]), data_segment_data_1, 15);
  memcpy(&(M0.data[8280u]), data_segment_data_2, 28);
  memcpy(&(M0.data[8308u]), data_segment_data_3, 33);
  memcpy(&(M0.data[8341u]), data_segment_data_4, 29);
  memcpy(&(M0.data[8370u]), data_segment_data_5, 60);
  memcpy(&(M0.data[8430u]), data_segment_data_6, 44);
  memcpy(&(M0.data[8474u]), data_segment_data_7, 17);
  memcpy(&(M0.data[8491u]), data_segment_data_8, 29);
  memcpy(&(M0.data[8520u]), data_segment_data_9, 26);
  memcpy(&(M0.data[8546u]), data_segment_data_10, 34);
  memcpy(&(M0.data[8580u]), data_segment_data_11, 33);
  memcpy(&(M0.data[8613u]), data_segment_data_12, 30);
  memcpy(&(M0.data[8643u]), data_segment_data_13, 24);
  memcpy(&(M0.data[8667u]), data_segment_data_14, 26);
  memcpy(&(M0.data[8693u]), data_segment_data_15, 19);
  memcpy(&(M0.data[8712u]), data_segment_data_16, 32);
  memcpy(&(M0.data[8744u]), data_segment_data_17, 24);
  memcpy(&(M0.data[8768u]), data_segment_data_18, 18);
  memcpy(&(M0.data[8786u]), data_segment_data_19, 29);
  memcpy(&(M0.data[8815u]), data_segment_data_20, 22);
  memcpy(&(M0.data[8837u]), data_segment_data_21, 76);
  memcpy(&(M0.data[8913u]), data_segment_data_22, 46);
  memcpy(&(M0.data[8959u]), data_segment_data_23, 51);
  memcpy(&(M0.data[9010u]), data_segment_data_24, 23);
  memcpy(&(M0.data[9033u]), data_segment_data_25, 51);
  memcpy(&(M0.data[9084u]), data_segment_data_26, 6);
  memcpy(&(M0.data[9090u]), data_segment_data_27, 46);
  memcpy(&(M0.data[9136u]), data_segment_data_28, 51);
  memcpy(&(M0.data[9187u]), data_segment_data_29, 59);
  memcpy(&(M0.data[9246u]), data_segment_data_30, 43);
  memcpy(&(M0.data[9289u]), data_segment_data_31, 19);
  memcpy(&(M0.data[9308u]), data_segment_data_32, 18);
  memcpy(&(M0.data[9326u]), data_segment_data_33, 48);
  memcpy(&(M0.data[9374u]), data_segment_data_34, 22);
  memcpy(&(M0.data[9396u]), data_segment_data_35, 21);
  memcpy(&(M0.data[9417u]), data_segment_data_36, 34);
  memcpy(&(M0.data[9451u]), data_segment_data_37, 30);
  memcpy(&(M0.data[9481u]), data_segment_data_38, 45);
  memcpy(&(M0.data[9526u]), data_segment_data_39, 50);
  memcpy(&(M0.data[9576u]), data_segment_data_40, 53);
  memcpy(&(M0.data[9629u]), data_segment_data_41, 5);
  memcpy(&(M0.data[9634u]), data_segment_data_42, 4);
  memcpy(&(M0.data[0u]), data_segment_data_43, 4);
}

static void init_table(void) {
  uint32_t offset;
  wasm_rt_allocate_table((&T0), 6, 6);
  offset = 1u;
  T0.data[offset + 0] = (wasm_rt_elem_t){func_types[1], (wasm_rt_anyfunc_t)(&f59)};
  T0.data[offset + 1] = (wasm_rt_elem_t){func_types[1], (wasm_rt_anyfunc_t)(&f69)};
  T0.data[offset + 2] = (wasm_rt_elem_t){func_types[2], (wasm_rt_anyfunc_t)(&f73)};
  T0.data[offset + 3] = (wasm_rt_elem_t){func_types[3], (wasm_rt_anyfunc_t)(&f63)};
  T0.data[offset + 4] = (wasm_rt_elem_t){func_types[3], (wasm_rt_anyfunc_t)(&f75)};
}

/* export: 'apply' */
void (*WASM_RT_ADD_PREFIX(apply_eosio_token_e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb_0))(u64, u64, u64);

static void init_exports(void) {
  /* export: 'apply' */
  WASM_RT_ADD_PREFIX(apply_eosio_token_e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb_0) = (&apply);
}

void WASM_RT_ADD_PREFIX(init_eosio_token_e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb_0)(void) {
  init_func_types();
  init_globals();
  init_memory();
  init_table();
  init_exports();
}

wasm_rt_memory_t* get_memory_eosio_token_e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb_0() {
    return &M0;
}
        