#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "eosio.system.h"
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


static u32 func_types[62];

static void init_func_types(void) {
  func_types[0] = wasm_rt_register_func_type(0, 0);
  func_types[1] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[2] = wasm_rt_register_func_type(4, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32);
  func_types[3] = wasm_rt_register_func_type(2, 0, WASM_RT_I32, WASM_RT_I32);
  func_types[4] = wasm_rt_register_func_type(3, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[5] = wasm_rt_register_func_type(2, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[6] = wasm_rt_register_func_type(2, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64);
  func_types[7] = wasm_rt_register_func_type(0, 1, WASM_RT_I64);
  func_types[8] = wasm_rt_register_func_type(1, 0, WASM_RT_I64);
  func_types[9] = wasm_rt_register_func_type(2, 0, WASM_RT_I64, WASM_RT_I32);
  func_types[10] = wasm_rt_register_func_type(4, 0, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[11] = wasm_rt_register_func_type(4, 0, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[12] = wasm_rt_register_func_type(3, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[13] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32);
  func_types[14] = wasm_rt_register_func_type(5, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[15] = wasm_rt_register_func_type(5, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[16] = wasm_rt_register_func_type(6, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[17] = wasm_rt_register_func_type(0, 1, WASM_RT_I32);
  func_types[18] = wasm_rt_register_func_type(2, 0, WASM_RT_I32, WASM_RT_F32);
  func_types[19] = wasm_rt_register_func_type(5, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[20] = wasm_rt_register_func_type(2, 0, WASM_RT_I32, WASM_RT_F64);
  func_types[21] = wasm_rt_register_func_type(2, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_F64);
  func_types[22] = wasm_rt_register_func_type(2, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_F32);
  func_types[23] = wasm_rt_register_func_type(2, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32);
  func_types[24] = wasm_rt_register_func_type(2, 0, WASM_RT_I32, WASM_RT_I64);
  func_types[25] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[26] = wasm_rt_register_func_type(5, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32);
  func_types[27] = wasm_rt_register_func_type(3, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32);
  func_types[28] = wasm_rt_register_func_type(1, 0, WASM_RT_I32);
  func_types[29] = wasm_rt_register_func_type(5, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[30] = wasm_rt_register_func_type(1, 1, WASM_RT_I64, WASM_RT_I32);
  func_types[31] = wasm_rt_register_func_type(5, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[32] = wasm_rt_register_func_type(1, 1, WASM_RT_I32, WASM_RT_I32);
  func_types[33] = wasm_rt_register_func_type(5, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[34] = wasm_rt_register_func_type(3, 0, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[35] = wasm_rt_register_func_type(2, 1, WASM_RT_F64, WASM_RT_I32, WASM_RT_F64);
  func_types[36] = wasm_rt_register_func_type(1, 1, WASM_RT_F64, WASM_RT_F64);
  func_types[37] = wasm_rt_register_func_type(11, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[38] = wasm_rt_register_func_type(4, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[39] = wasm_rt_register_func_type(8, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[40] = wasm_rt_register_func_type(4, 1, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[41] = wasm_rt_register_func_type(1, 1, WASM_RT_I32, WASM_RT_I64);
  func_types[42] = wasm_rt_register_func_type(3, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64);
  func_types[43] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32);
  func_types[44] = wasm_rt_register_func_type(5, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[45] = wasm_rt_register_func_type(2, 0, WASM_RT_I64, WASM_RT_I64);
  func_types[46] = wasm_rt_register_func_type(1, 1, WASM_RT_I64, WASM_RT_I64);
  func_types[47] = wasm_rt_register_func_type(6, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[48] = wasm_rt_register_func_type(6, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[49] = wasm_rt_register_func_type(2, 1, WASM_RT_F64, WASM_RT_F64, WASM_RT_F64);
  func_types[50] = wasm_rt_register_func_type(3, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[51] = wasm_rt_register_func_type(5, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64);
  func_types[52] = wasm_rt_register_func_type(6, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64);
  func_types[53] = wasm_rt_register_func_type(5, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32);
  func_types[54] = wasm_rt_register_func_type(5, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[55] = wasm_rt_register_func_type(4, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_F64, WASM_RT_F64, WASM_RT_F64);
  func_types[56] = wasm_rt_register_func_type(3, 1, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[57] = wasm_rt_register_func_type(5, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_F64, WASM_RT_I32, WASM_RT_F64);
  func_types[58] = wasm_rt_register_func_type(6, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[59] = wasm_rt_register_func_type(2, 1, WASM_RT_F64, WASM_RT_F64, WASM_RT_I32);
  func_types[60] = wasm_rt_register_func_type(1, 1, WASM_RT_F64, WASM_RT_I32);
  func_types[61] = wasm_rt_register_func_type(1, 1, WASM_RT_F64, WASM_RT_I64);
}

static void f95(void);
static void apply(u64, u64, u64);
static u32 f97(u32);
static void f98(u32);
static void f99(void);
static u32 f100(void);
static u32 f101(u32, u32);
static u32 f102(u32, u32, u32);
static u32 f103(u32);
static u32 f104(u32);
static void f105(u32);
static void f106(u32);
static u32 f107(u32, u32);
static u32 f108(u32, u32);
static void f109(u32, u32);
static void f110(u32, u32);
static void f111(u32);
static void f112(u32);
static void f113(u32);
static void f114(u32, u32);
static u32 f115(u32, u32);
static u64 f116(void);
static u32 f117(void);
static void f118(u32, u32, u32);
static u32 f119(u32);
static u32 f120(u32, u32, u32);
static f64 f121(f64, u32);
static f64 f122(f64);
static u32 f123(u32, u32, u32, u32, u32);
static void f124(u32, u32, u32, u32);
static u32 f125(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
static void f126(u32, u32, u32, u32);
static u32 f127(u32, u32, u32, u32);
static void f128(u32);
static u32 f129(u32, u32);
static void f130(u32, u32, u32, u32, u32, u32, u32, u32);
static void f131(u32, u32);
static u32 f132(u32, u32);
static u32 f133(u32, u32, u32);
static u32 f134(u32, u32, u32, u32);
static u32 f135(u32, u32, u32);
static void f136(u32);
static u32 f137(u32, u64, u64, u32);
static u32 f138(u32, u32);
static u32 f139(u32, u32);
static u32 f140(u32, u32);
static u64 f141(u32);
static u64 f142(u32);
static u32 f143(u32, u32);
static u32 f144(u32);
static void f145(u32, u32, u64);
static void f146(u32, u32, u64);
static void f147(u32, u32, u64);
static u32 f148(u32);
static void f149(u32, u32, u64, u32);
static void f150(u32, u32, u64, u32);
static void f151(u32, u32, u64, u32);
static void f152(u32, u32, u64, u32);
static void f153(u32, u32, u64, u32);
static void f154(u32, u32, u64, u32);
static void f155(u32, u64);
static void f156(u32, u32, u32);
static void f157(u32);
static void f158(u32, u32, u32);
static void f159(u32, u32, u64, u64, u64);
static u32 f160(u32, u32);
static u32 f161(u32, u32);
static void f162(u32, u32, u32);
static void f163(u32, u32);
static void f164(u32, u32);
static void f165(u32, u32, u32, u32);
static void f166(u32, u32);
static void f167(u32, u32, u32);
static void f168(u32, u32);
static void f169(u32, u32);
static void f170(u32, u32);
static void f171(u32, u32, u32);
static void f172(u32, u32);
static void f173(u32, u32);
static void f174(u32, u32);
static void f175(u32, u32);
static u32 f176(u32, u32);
static void f177(u32, u32);
static void f178(u32, u32, u32);
static u64 f179(u32);
static u32 f180(u32, u32);
static void f181(u32, u32);
static void f182(u32, u32);
static void f183(u32, u32, u32, u32);
static void f184(u32, u32, u32);
static u32 f185(u32, u32);
static void f186(u32, u32, u32);
static u32 f187(u32, u32);
static void f188(u32, u32, u32, u32);
static void f189(u32, u32, u32);
static void f190(u32, u32, u32);
static u32 f191(u32, u32);
static void f192(u32, u32, u32, u32);
static void f193(u32, u32, u32, u32);
static u32 f194(u32, u32);
static void f195(u32, u32, u32, u32, u32);
static void f196(u32, u32);
static u32 f197(u32, u32);
static void f198(u64, u64);
static void f199(u64, u64);
static void f200(u64, u64);
static void f201(u64, u64);
static void f202(u64, u64);
static void f203(u64, u64);
static void f204(u64, u64);
static void f205(u64, u64);
static u32 f206(u32, u32);
static void f207(u64, u64);
static void f208(u64, u64);
static void f209(u64, u64);
static void f210(u64, u64);
static void f211(u64, u64);
static void f212(u64, u64);
static void f213(u64, u64);
static void f214(u64, u64);
static void f215(u64, u64);
static void f216(u64, u64);
static void f217(u64, u64);
static void f218(u64, u64);
static void f219(u64, u64);
static void f220(u64, u64);
static void f221(u64, u64);
static void f222(u64, u64);
static void f223(u64, u64);
static void f224(u64, u64);
static void f225(u64, u64);
static void f226(u64, u64);
static void f227(u64, u64);
static void f228(u64, u64);
static void f229(u64, u64);
static void f230(u64, u64);
static void f231(u64, u64);
static void f232(u64, u64);
static void f233(u64, u64);
static void f234(u64, u64);
static void f235(u64, u64);
static void f236(u64, u64);
static void f237(u64, u64);
static void f238(u64, u64);
static void f239(u64, u64);
static void f240(u64, u64);
static u32 f241(u32, u32);
static void f242(u64, u64);
static void f243(u64, u64);
static void f244(u64, u64);
static void f245(u64, u64);
static u32 f246(u32, u32);
static void f247(u32, u32);
static void f248(u64, u64);
static void f249(u64, u64);
static u32 f250(u32, u32);
static void f251(u64, u64);
static void f252(u64, u64);
static void f253(u64, u64);
static void f254(u64, u64);
static void f255(u64, u64);
static void f256(u64, u64);
static void f257(u32, u32);
static void f258(u32, u32, u32, u32);
static u32 f259(u32, u32);
static u32 f260(u32, u32);
static u32 f261(u32, u32);
static void f262(u32, u32, u32, u32);
static u32 f263(u32, u32);
static void f264(u32, u32, u32, u32);
static void f265(u32, u32, u32, u32);
static u32 f266(u32, u32);
static u32 f267(u32, u32);
static u32 f268(u32, u32);
static u32 f269(u32, u32);
static u32 f270(u32, u32);
static u32 f271(u32, u32);
static u32 f272(u32, u32);
static u32 f273(u32, u32);
static u32 f274(u32, u32);
static void f275(u32, u32, u32, u32);
static u32 f276(u32, u32);
static u32 f277(u32, u32);
static void f278(u32, u32);
static void f279(u32, u32, u32, u32);
static u32 f280(u32, u32);
static void f281(u32, u32);
static u32 f282(u32, u32);
static u32 f283(u32, u32);
static u64 f284(u64);
static void f285(u32, u32, u32, u32);
static void f286(u32, u32, u32, u32, u32, u32);
static void f287(u32);
static void f288(u32, u32);
static u32 f289(u32, u32);
static void f290(u32, u32, u32);
static void f291(u32, u32);
static void f292(u32, u32);
static void f293(u32, u32, u32, u32);
static void f294(u32, u32, u32, u32, u32);
static void f295(u32, u32, u64, u32);
static void f296(u32, u32, u64, u32);
static u32 f297(u32);
static u32 f298(u32, u32);
static u32 f299(u32, u32);
static void f300(u32, u32, u32);
static void f301(u32, u32, u32, u32, u32, u32);
static void f302(u32, u32);
static void f303(u32, u32, u32, u32);
static void f304(u32, u32, u32, u32);
static void f305(u32, u32, u32, u32, u32, u32);
static void f306(u32, u32, u32);
static void f307(u32, u32, u64, u32);
static void f308(u32, u32);
static void f309(u32, u32, u64);
static void f310(u32, u32, u32);
static void f311(u32, u32, u64, u32);
static void f312(u32, u32, u32, u32, u32, u32);
static void f313(u32, u32, u32, u32, u32, u32);
static void f314(u32, u64, u32, u32, u32, u32);
static u32 f315(u32, u32);
static void f316(u32, u32, u32);
static u32 f317(u32, u32);
static void f318(u32, u32, u32, u32);
static void f319(u32, u32);
static void f320(u32, u32, u64, u32);
static void f321(u32, u32);
static u32 f322(u32, u32);
static void f323(u32, u32, u32);
static void f324(u32, u32);
static void f325(u32, u32);
static void f326(u32, u32, u32, u32);
static void f327(u32, u32, u32, u32, u32);
static void f328(u32, u32, u32, u32, u32, u32);
static void f329(u32, u32, u32);
static void f330(u32, u32, u32);
static void f331(u32, u32);
static void f332(u32, u32, u32, u32, u32, u32);
static void f333(u32, u32, u32, u32, u32);
static void f334(u32, u32);
static void f335(u32, u32, u32, u32, u32, u32);
static u32 f336(u32, u32);
static u32 f337(u32, u32);
static void f338(u32, u32);
static u32 f339(u32, u32);
static u32 f340(u32, u32);
static u32 f341(u32, u32);
static u32 f342(u32, u32);
static f64 f343(f64);
static f64 f344(f64, f64);
static void f345(u32, u32, u32, u32);
static u64 f346(u64, u64, u64);
static u64 f347(u64, u64, u64);
static void f348(u32);
static void f349(u32);
static void f350(u32);
static void f351(u32, u32);
static void f352(u32, u32, u32);
static void f353(u32, u32);
static void f354(u32, u32);
static void f355(u32, u32, u32, u32, u32);
static void f356(u32, u32, u32, u32, u32, u32);
static void f357(u32, u32, u32, u32, u32, u32);
static u32 f358(u32, u32);
static u32 f359(u32, u32);
static void f360(u32, u32, u32, u32);
static void f361(u32, u32, u32);
static void f362(u32, u32, u32, u32, u32, u32);
static void f363(u32, u32, u32, u32, u32, u32);
static void f364(u32, u32);
static u32 f365(u32, u32);
static void f366(u32, u32, u32);
static void f367(u32, u32, u32, u32, u32, u32);
static void f368(u32, u32, u32);
static u32 f369(u32, u32);
static void f370(u32, u32, u32);
static void f371(u32, u32);
static void f372(u32, u32, u32, u32);
static void f373(u32, u32, u32);
static void f374(u32, u32, u32, u32, u32, u32);
static void f375(u32, u32, u32);
static void f376(u32, u32, u32, u32, u32, u32);
static u32 f377(u32, u32);
static void f378(u32, u32);
static void f379(u32, u32, u64, u32);
static void f380(u32, u32, u32);
static void f381(u32, u32, u32);
static void f382(u32, u32, u32);
static void f383(u32, u32, u32);
static void f384(u32, u32, u32, u32, u32);
static void f385(u32, u32);
static void f386(u32, u32, u32);
static u32 f387(u32, u32);
static void f388(u32, u32, u32);
static void f389(u32, u32, u32);
static void f390(u32, u32);
static void f391(u32, u32, u32, u32);
static u32 f392(u32, u32);
static void f393(u32, u32, u32);
static void f394(u32, u32);
static void f395(u32, u32, u32, u32);
static void f396(u32, u32, u32);
static void f397(u32, u32, u32);
static void f398(u32, u32, u32);
static void f399(u32, u32, u32);
static void f400(u32, u32, u32);
static void f401(u32, u32);
static void f402(u32, u32, u32);
static void f403(u32, u32);
static u32 f404(u32);
static void f405(u32, u32, u32);
static void f406(u32, u32, u64, u32);
static void f407(u32, u32, u32, u64, u64);
static u32 f408(u32);
static void f409(u32, u32);
static void f410(u32, u32, u32);
static void f411(u32, u32, u64, u32);
static u32 f412(u32);
static void f413(u32, u32);
static void f414(u32, u32, u32);
static u32 f415(u32);
static void f416(u32, u32, u32, u32);
static void f417(u32, u32, u32);
static void f418(u32, u32, u32, u32);
static void f419(u32, u32, u32, u32, u32);
static void f420(u32, u32, u64, u32);
static void f421(u32, u32, u32);
static void f422(u32, u32, u32, u32, u32, u32);
static void f423(u32, u32, u32);
static void f424(u32, u32);
static void f425(u32, u32, u32);
static void f426(u32, u32, u64, u32);
static void f427(u32, u32, u32);
static void f428(u32, u32);
static void f429(u32, u32, u32, u32);
static void f430(u32, u32, u32);
static void f431(u32, u32, u32);
static void f432(u32, u32, u32);
static void f433(u32, u32);
static void f434(u32, u32, u64, u32);
static void f435(u32, u32, u32, u32, u32);
static u64 f436(u32, u32, u32, u32, u32, u32);
static void f437(u32, u32, u64, u32);
static void f438(u32, u32, u32);
static void f439(u32, u32, u32, u32, u32);
static u64 f440(u32, u32, u32, u32, u32, u32);
static void f441(u32, u32, u64, u32);
static void f442(u32, u32, u64, u32);
static void f443(u32, u32, u32, u64, u32);
static void f444(u32, u32, u64, u32);
static void f445(u32, u32, u64, u32);
static void f446(u32, u32, u64, u32);
static void f447(u32, u32, u32, u64, u32);
static void f448(u32, u32, u64, u32);
static void f449(u32, u32, u64, u32);
static void f450(u32, u32, u64, u32);
static void f451(u32, u32, u32, u64, u32);
static void f452(u32, u32, u64, u32);
static void f453(u32, u32, u64, u32);
static void f454(u32, u32, u32, u64, u32);
static void f455(u32, u32, u64, u32);
static void f456(u32, u32);
static void f457(u32, u32, u32);
static void f458(u32, u32);
static void f459(u32, u32, u32);
static void f460(u32, u32, u32);
static void f461(u32, u32);
static void f462(u32, u32, u32);
static void f463(u32, u32, u32);
static void f464(u32, u32, u32);
static void f465(u32, u32, u32);
static void f466(u32, u32, u32);
static void f467(u32, u32, u32);
static void f468(u32, u32);
static void f469(u32, u32, u32);
static void f470(u32, u32, u32);
static void f471(u32, u32);
static void f472(u32, u32);
static void f473(u32, u32, u32);
static void f474(u32, u32, u32);
static void f475(u32, u32, u32);
static void f476(u32, u64);
static void f477(u32, u32, u32);
static void f478(u32, u32);
static void f479(u32, u32, u32);
static void f480(u32, u32, u32);
static void f481(u32, u32);
static void f482(u32, u32, u32);
static void f483(u32, u32, u32);
static void f484(u32, u32);
static u32 f485(u32, u32, u64, u64, u32);
static void f486(u32, u32);
static u32 f487(u32, u32);
static void f488(u32, u32);
static void f489(u32, u32);
static void f490(u32, u32);
static void f491(u32, u32);
static void f492(u32, u32);
static void f493(u32, u32);
static void f494(u32, u32);
static void f495(u32, u32);
static void f496(u32, u32);
static void f497(u32, u32);
static void f498(u32, u32, u32, u32);
static void f499(u32, u32);
static void f500(u32, u32);
static void f501(u32, u32, u32, u32);
static void f502(u32, u32);
static u32 f503(u32, u32);
static void f504(u32, u32);
static void f505(u32, u32, u32);
static u32 f506(u32, u32);
static void f507(u32, u32, u32);
static void f508(u32, u32, u32);
static void f509(u32, u32, u32);
static void f510(u32, u32);
static u32 f511(u32, u32);
static void f512(u32, u32);
static void f513(u32);
static void f514(u32, u32);
static u32 f515(u32, u32);
static void f516(u32, u32);
static void f517(u32, u32, u32, u32, u32);
static u32 f518(u32, u32);
static void f519(u32, u32, u64, u32);
static void f520(u32, u32);
static f64 f521(u32, u32, f64, f64);
static void f522(u32, u32);
static u32 f523(u32, u64, u32);
static void f524(u32, u32);
static void f525(u32, u32);
static void f526(u32, u32, u32);
static void f527(u32, u32);
static u32 f528(u32);
static void f529(u32, u32, u32);
static void f530(u32, u32);
static f64 f531(u32, u32, u32, f64, u32);
static void f532(u32, u32, u32);
static void f533(u32, u32, u32, u32);
static void f534(u32, u32, u32, u32, u32);
static void f535(u32, u32, u32);
static void f536(u32, u32);
static void f537(u32, u32);
static void f538(u32, u32, u32);
static void f539(u32, u32, u32);
static void f540(u32, u32, u32);
static void f541(u32, u32);
static void f542(u32, u32, u32);
static void f543(u32, u32, u32);
static void f544(u32, u32, u32);
static void f545(u32, u32, u32);
static void f546(u32, u32, u32);
static void f547(u32, u32);
static u32 f548(u32, u32, u32, u32, u32, u32);
static u32 f549(u32, u32, u32, u32);
static u32 f550(u32, u32, u32, u32, u32);
static u32 f551(u32, u32, u32);
static void f552(u32, u32, u32);

static u32 g0;
static u32 g1;
static u32 g2;
static u32 g3;

static void init_globals(void) {
  g0 = 8192u;
  g1 = 17007u;
  g2 = 17007u;
  g3 = 250u;
}

static wasm_rt_memory_t M0;

static wasm_rt_table_t T0;

static void f95(void) {
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
  f99();
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static void apply(u64 p0, u64 p1, u64 p2) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
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
  f95();
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = p0;
  j1 = p1;
  i0 = j0 == j1;
  if (i0) {
    j0 = 11148770977341390848ull;
    j1 = p2;
    i0 = j0 == j1;
    if (i0) {
      j0 = p0;
      j1 = p1;
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
      f198(j0, j1);
      i0 = g3;
      i1 = 1u;
      i0 += i1;
      g3 = i0;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
    } else {
      j0 = 15371467950649982976ull;
      j1 = p2;
      i0 = j0 == j1;
      if (i0) {
        j0 = p0;
        j1 = p1;
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
        f199(j0, j1);
        i0 = g3;
        i1 = 1u;
        i0 += i1;
        g3 = i0;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
      } else {
        j0 = 5378050746259030016ull;
        j1 = p2;
        i0 = j0 == j1;
        if (i0) {
          j0 = p0;
          j1 = p1;
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
          f200(j0, j1);
          i0 = g3;
          i1 = 1u;
          i0 += i1;
          g3 = i0;
          (*Z_eosio_injectionZ_checktimeZ_vv)();
        } else {
          j0 = 10063015651234021376ull;
          j1 = p2;
          i0 = j0 == j1;
          if (i0) {
            j0 = p0;
            j1 = p1;
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
            f201(j0, j1);
            i0 = g3;
            i1 = 1u;
            i0 += i1;
            g3 = i0;
            (*Z_eosio_injectionZ_checktimeZ_vv)();
          } else {
            j0 = 15340080295291076608ull;
            j1 = p2;
            i0 = j0 == j1;
            if (i0) {
              j0 = p0;
              j1 = p1;
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
              f202(j0, j1);
              i0 = g3;
              i1 = 1u;
              i0 += i1;
              g3 = i0;
              (*Z_eosio_injectionZ_checktimeZ_vv)();
            } else {
              j0 = 4730614990712192000ull;
              j1 = p2;
              i0 = j0 == j1;
              if (i0) {
                j0 = p0;
                j1 = p1;
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
                f203(j0, j1);
                i0 = g3;
                i1 = 1u;
                i0 += i1;
                g3 = i0;
                (*Z_eosio_injectionZ_checktimeZ_vv)();
              } else {
                j0 = 11877535737890996224ull;
                j1 = p2;
                i0 = j0 == j1;
                if (i0) {
                  j0 = p0;
                  j1 = p1;
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
                  f204(j0, j1);
                  i0 = g3;
                  i1 = 1u;
                  i0 += i1;
                  g3 = i0;
                  (*Z_eosio_injectionZ_checktimeZ_vv)();
                } else {
                  j0 = 14029385431137648640ull;
                  j1 = p2;
                  i0 = j0 == j1;
                  if (i0) {
                    j0 = p0;
                    j1 = p1;
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
                    f205(j0, j1);
                    i0 = g3;
                    i1 = 1u;
                    i0 += i1;
                    g3 = i0;
                    (*Z_eosio_injectionZ_checktimeZ_vv)();
                  } else {
                    j0 = 14029427681804681216ull;
                    j1 = p2;
                    i0 = j0 == j1;
                    if (i0) {
                      j0 = p0;
                      j1 = p1;
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
                      f207(j0, j1);
                      i0 = g3;
                      i1 = 1u;
                      i0 += i1;
                      g3 = i0;
                      (*Z_eosio_injectionZ_checktimeZ_vv)();
                    } else {
                      j0 = 8421045207927095296ull;
                      j1 = p2;
                      i0 = j0 == j1;
                      if (i0) {
                        j0 = p0;
                        j1 = p1;
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
                        f208(j0, j1);
                        i0 = g3;
                        i1 = 1u;
                        i0 += i1;
                        g3 = i0;
                        (*Z_eosio_injectionZ_checktimeZ_vv)();
                      } else {
                        j0 = 11875739475730497536ull;
                        j1 = p2;
                        i0 = j0 == j1;
                        if (i0) {
                          j0 = p0;
                          j1 = p1;
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
                          f209(j0, j1);
                          i0 = g3;
                          i1 = 1u;
                          i0 += i1;
                          g3 = i0;
                          (*Z_eosio_injectionZ_checktimeZ_vv)();
                        } else {
                          j0 = 14029390938607845376ull;
                          j1 = p2;
                          i0 = j0 == j1;
                          if (i0) {
                            j0 = p0;
                            j1 = p1;
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
                            f210(j0, j1);
                            i0 = g3;
                            i1 = 1u;
                            i0 += i1;
                            g3 = i0;
                            (*Z_eosio_injectionZ_checktimeZ_vv)();
                          } else {
                            j0 = 14029385891625336832ull;
                            j1 = p2;
                            i0 = j0 == j1;
                            if (i0) {
                              j0 = p0;
                              j1 = p1;
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
                              f211(j0, j1);
                              i0 = g3;
                              i1 = 1u;
                              i0 += i1;
                              g3 = i0;
                              (*Z_eosio_injectionZ_checktimeZ_vv)();
                            } else {
                              j0 = 14029385891560439808ull;
                              j1 = p2;
                              i0 = j0 == j1;
                              if (i0) {
                                j0 = p0;
                                j1 = p1;
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
                                f212(j0, j1);
                                i0 = g3;
                                i1 = 1u;
                                i0 += i1;
                                g3 = i0;
                                (*Z_eosio_injectionZ_checktimeZ_vv)();
                              } else {
                                j0 = 14029385891381673984ull;
                                j1 = p2;
                                i0 = j0 == j1;
                                if (i0) {
                                  j0 = p0;
                                  j1 = p1;
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
                                  f213(j0, j1);
                                  i0 = g3;
                                  i1 = 1u;
                                  i0 += i1;
                                  g3 = i0;
                                  (*Z_eosio_injectionZ_checktimeZ_vv)();
                                } else {
                                  j0 = 3617214701412286464ull;
                                  j1 = p2;
                                  i0 = j0 == j1;
                                  if (i0) {
                                    j0 = p0;
                                    j1 = p1;
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
                                    f214(j0, j1);
                                    i0 = g3;
                                    i1 = 1u;
                                    i0 += i1;
                                    g3 = i0;
                                    (*Z_eosio_injectionZ_checktimeZ_vv)();
                                  } else {
                                    j0 = 5378043540636893184ull;
                                    j1 = p2;
                                    i0 = j0 == j1;
                                    if (i0) {
                                      j0 = p0;
                                      j1 = p1;
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
                                      f215(j0, j1);
                                      i0 = g3;
                                      i1 = 1u;
                                      i0 += i1;
                                      g3 = i0;
                                      (*Z_eosio_injectionZ_checktimeZ_vv)();
                                    } else {
                                      j0 = 14029686405265883136ull;
                                      j1 = p2;
                                      i0 = j0 == j1;
                                      if (i0) {
                                        j0 = p0;
                                        j1 = p1;
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
                                        f216(j0, j1);
                                        i0 = g3;
                                        i1 = 1u;
                                        i0 += i1;
                                        g3 = i0;
                                        (*Z_eosio_injectionZ_checktimeZ_vv)();
                                      } else {
                                        j0 = 5380477996647841792ull;
                                        j1 = p2;
                                        i0 = j0 == j1;
                                        if (i0) {
                                          j0 = p0;
                                          j1 = p1;
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
                                          f217(j0, j1);
                                          i0 = g3;
                                          i1 = 1u;
                                          i0 += i1;
                                          g3 = i0;
                                          (*Z_eosio_injectionZ_checktimeZ_vv)();
                                        } else {
                                          j0 = 16407410437513019392ull;
                                          j1 = p2;
                                          i0 = j0 == j1;
                                          if (i0) {
                                            j0 = p0;
                                            j1 = p1;
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
                                            f218(j0, j1);
                                            i0 = g3;
                                            i1 = 1u;
                                            i0 += i1;
                                            g3 = i0;
                                            (*Z_eosio_injectionZ_checktimeZ_vv)();
                                          } else {
                                            j0 = 4520898742026502144ull;
                                            j1 = p2;
                                            i0 = j0 == j1;
                                            if (i0) {
                                              j0 = p0;
                                              j1 = p1;
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
                                              f219(j0, j1);
                                              i0 = g3;
                                              i1 = 1u;
                                              i0 += i1;
                                              g3 = i0;
                                              (*Z_eosio_injectionZ_checktimeZ_vv)();
                                            } else {
                                              j0 = 15344207314314188240ull;
                                              j1 = p2;
                                              i0 = j0 == j1;
                                              if (i0) {
                                                j0 = p0;
                                                j1 = p1;
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
                                                f220(j0, j1);
                                                i0 = g3;
                                                i1 = 1u;
                                                i0 += i1;
                                                g3 = i0;
                                                (*Z_eosio_injectionZ_checktimeZ_vv)();
                                              } else {
                                                j0 = 14025084088499568640ull;
                                                j1 = p2;
                                                i0 = j0 == j1;
                                                if (i0) {
                                                  j0 = p0;
                                                  j1 = p1;
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
                                                  f221(j0, j1);
                                                  i0 = g3;
                                                  i1 = 1u;
                                                  i0 += i1;
                                                  g3 = i0;
                                                  (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                } else {
                                                  j0 = 4958775088997094768ull;
                                                  j1 = p2;
                                                  i0 = j0 == j1;
                                                  if (i0) {
                                                    j0 = p0;
                                                    j1 = p1;
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
                                                    f222(j0, j1);
                                                    i0 = g3;
                                                    i1 = 1u;
                                                    i0 += i1;
                                                    g3 = i0;
                                                    (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                  } else {
                                                    j0 = 13449881914622083072ull;
                                                    j1 = p2;
                                                    i0 = j0 == j1;
                                                    if (i0) {
                                                      j0 = p0;
                                                      j1 = p1;
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
                                                      f223(j0, j1);
                                                      i0 = g3;
                                                      i1 = 1u;
                                                      i0 += i1;
                                                      g3 = i0;
                                                      (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                    } else {
                                                      j0 = 13449887772420603904ull;
                                                      j1 = p2;
                                                      i0 = j0 == j1;
                                                      if (i0) {
                                                        j0 = p0;
                                                        j1 = p1;
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
                                                        f224(j0, j1);
                                                        i0 = g3;
                                                        i1 = 1u;
                                                        i0 += i1;
                                                        g3 = i0;
                                                        (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                      } else {
                                                        j0 = 6820301788451808768ull;
                                                        j1 = p2;
                                                        i0 = j0 == j1;
                                                        if (i0) {
                                                          j0 = p0;
                                                          j1 = p1;
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
                                                          f225(j0, j1);
                                                          i0 = g3;
                                                          i1 = 1u;
                                                          i0 += i1;
                                                          g3 = i0;
                                                          (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                        } else {
                                                          j0 = 6820307646250329600ull;
                                                          j1 = p2;
                                                          i0 = j0 == j1;
                                                          if (i0) {
                                                            j0 = p0;
                                                            j1 = p1;
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
                                                            f226(j0, j1);
                                                            i0 = g3;
                                                            i1 = 1u;
                                                            i0 += i1;
                                                            g3 = i0;
                                                            (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                          } else {
                                                            j0 = 5374635943816118272ull;
                                                            j1 = p2;
                                                            i0 = j0 == j1;
                                                            if (i0) {
                                                              j0 = p0;
                                                              j1 = p1;
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
                                                              f227(j0, j1);
                                                              i0 = g3;
                                                              i1 = 1u;
                                                              i0 += i1;
                                                              g3 = i0;
                                                              (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                            } else {
                                                              j0 = 5374823393368784896ull;
                                                              j1 = p2;
                                                              i0 = j0 == j1;
                                                              if (i0) {
                                                                j0 = p0;
                                                                j1 = p1;
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
                                                                f228(j0, j1);
                                                                i0 = g3;
                                                                i1 = 1u;
                                                                i0 += i1;
                                                                g3 = i0;
                                                                (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                              } else {
                                                                j0 = 15371467959510237184ull;
                                                                j1 = p2;
                                                                i0 = j0 == j1;
                                                                if (i0) {
                                                                  j0 = p0;
                                                                  j1 = p1;
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
                                                                  f229(j0, j1);
                                                                  i0 = g3;
                                                                  i1 = 1u;
                                                                  i0 += i1;
                                                                  g3 = i0;
                                                                  (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                } else {
                                                                  j0 = 13455258877595484160ull;
                                                                  j1 = p2;
                                                                  i0 = j0 == j1;
                                                                  if (i0) {
                                                                    j0 = p0;
                                                                    j1 = p1;
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
                                                                    f230(j0, j1);
                                                                    i0 = g3;
                                                                    i1 = 1u;
                                                                    i0 += i1;
                                                                    g3 = i0;
                                                                    (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                  } else {
                                                                    j0 = 4983103545041900544ull;
                                                                    j1 = p2;
                                                                    i0 = j0 == j1;
                                                                    if (i0) {
                                                                      j0 = p0;
                                                                      j1 = p1;
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
                                                                      f231(j0, j1);
                                                                      i0 = g3;
                                                                      i1 = 1u;
                                                                      i0 += i1;
                                                                      g3 = i0;
                                                                      (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                    } else {
                                                                      j0 = 10877121205733371904ull;
                                                                      j1 = p2;
                                                                      i0 = j0 == j1;
                                                                      if (i0) {
                                                                        j0 = p0;
                                                                        j1 = p1;
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
                                                                        f232(j0, j1);
                                                                        i0 = g3;
                                                                        i1 = 1u;
                                                                        i0 += i1;
                                                                        g3 = i0;
                                                                        (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                      } else {
                                                                        j0 = 10869292682943606784ull;
                                                                        j1 = p2;
                                                                        i0 = j0 == j1;
                                                                        if (i0) {
                                                                          j0 = p0;
                                                                          j1 = p1;
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
                                                                          f233(j0, j1);
                                                                          i0 = g3;
                                                                          i1 = 1u;
                                                                          i0 += i1;
                                                                          g3 = i0;
                                                                          (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                        } else {
                                                                          j0 = 4929617903172452352ull;
                                                                          j1 = p2;
                                                                          i0 = j0 == j1;
                                                                          if (i0) {
                                                                            j0 = p0;
                                                                            j1 = p1;
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
                                                                            f234(j0, j1);
                                                                            i0 = g3;
                                                                            i1 = 1u;
                                                                            i0 += i1;
                                                                            g3 = i0;
                                                                            (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                          } else {
                                                                            j0 = 15335505127214321600ull;
                                                                            j1 = p2;
                                                                            i0 = j0 == j1;
                                                                            if (i0) {
                                                                              j0 = p0;
                                                                              j1 = p1;
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
                                                                              f235(j0, j1);
                                                                              i0 = g3;
                                                                              i1 = 1u;
                                                                              i0 += i1;
                                                                              g3 = i0;
                                                                              (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                            } else {
                                                                              j0 = 4520896354024685568ull;
                                                                              j1 = p2;
                                                                              i0 = j0 == j1;
                                                                              if (i0) {
                                                                                j0 = p0;
                                                                                j1 = p1;
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
                                                                                f236(j0, j1);
                                                                                i0 = g3;
                                                                                i1 = 1u;
                                                                                i0 += i1;
                                                                                g3 = i0;
                                                                                (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                              } else {
                                                                                j0 = 4520896358299381760ull;
                                                                                j1 = p2;
                                                                                i0 = j0 == j1;
                                                                                if (i0) {
                                                                                  j0 = p0;
                                                                                  j1 = p1;
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
                                                                                  f237(j0, j1);
                                                                                  i0 = g3;
                                                                                  i1 = 1u;
                                                                                  i0 += i1;
                                                                                  g3 = i0;
                                                                                  (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                } else {
                                                                                  j0 = 14025084013874511872ull;
                                                                                  j1 = p2;
                                                                                  i0 = j0 == j1;
                                                                                  if (i0) {
                                                                                    j0 = p0;
                                                                                    j1 = p1;
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
                                                                                    f238(j0, j1);
                                                                                    i0 = g3;
                                                                                    i1 = 1u;
                                                                                    i0 += i1;
                                                                                    g3 = i0;
                                                                                    (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                  } else {
                                                                                    j0 = 13445401734377635840ull;
                                                                                    j1 = p2;
                                                                                    i0 = j0 == j1;
                                                                                    if (i0) {
                                                                                      j0 = p0;
                                                                                      j1 = p1;
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
                                                                                      f239(j0, j1);
                                                                                      i0 = g3;
                                                                                      i1 = 1u;
                                                                                      i0 += i1;
                                                                                      g3 = i0;
                                                                                      (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                    } else {
                                                                                      j0 = 13445879116675067392ull;
                                                                                      j1 = p2;
                                                                                      i0 = j0 == j1;
                                                                                      if (i0) {
                                                                                        j0 = p0;
                                                                                        j1 = p1;
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
                                                                                        f240(j0, j1);
                                                                                        i0 = g3;
                                                                                        i1 = 1u;
                                                                                        i0 += i1;
                                                                                        g3 = i0;
                                                                                        (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                      } else {
                                                                                        j0 = 15343383872893616128ull;
                                                                                        j1 = p2;
                                                                                        i0 = j0 == j1;
                                                                                        if (i0) {
                                                                                          j0 = p0;
                                                                                          j1 = p1;
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
                                                                                          f242(j0, j1);
                                                                                          i0 = g3;
                                                                                          i1 = 1u;
                                                                                          i0 += i1;
                                                                                          g3 = i0;
                                                                                          (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                        } else {
                                                                                          j0 = 14029684017264066560ull;
                                                                                          j1 = p2;
                                                                                          i0 = j0 == j1;
                                                                                          if (i0) {
                                                                                            j0 = p0;
                                                                                            j1 = p1;
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
                                                                                            f243(j0, j1);
                                                                                            i0 = g3;
                                                                                            i1 = 1u;
                                                                                            i0 += i1;
                                                                                            g3 = i0;
                                                                                            (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                          } else {
                                                                                            j0 = 14029684029726031872ull;
                                                                                            j1 = p2;
                                                                                            i0 = j0 == j1;
                                                                                            if (i0) {
                                                                                              j0 = p0;
                                                                                              j1 = p1;
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
                                                                                              f244(j0, j1);
                                                                                              i0 = g3;
                                                                                              i1 = 1u;
                                                                                              i0 += i1;
                                                                                              g3 = i0;
                                                                                              (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                            } else {
                                                                                              j0 = 15938989903989314928ull;
                                                                                              j1 = p2;
                                                                                              i0 = j0 == j1;
                                                                                              if (i0) {
                                                                                                j0 = p0;
                                                                                                j1 = p1;
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
                                                                                                f245(j0, j1);
                                                                                                i0 = g3;
                                                                                                i1 = 1u;
                                                                                                i0 += i1;
                                                                                                g3 = i0;
                                                                                                (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                              } else {
                                                                                                j0 = 13445879127475224576ull;
                                                                                                j1 = p2;
                                                                                                i0 = j0 == j1;
                                                                                                if (i0) {
                                                                                                  j0 = p0;
                                                                                                  j1 = p1;
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
                                                                                                  f248(j0, j1);
                                                                                                  i0 = g3;
                                                                                                  i1 = 1u;
                                                                                                  i0 += i1;
                                                                                                  g3 = i0;
                                                                                                  (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                                } else {
                                                                                                  j0 = 14029648922327121920ull;
                                                                                                  j1 = p2;
                                                                                                  i0 = j0 == j1;
                                                                                                  if (i0) {
                                                                                                    j0 = p0;
                                                                                                    j1 = p1;
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
                                                                                                    f249(j0, j1);
                                                                                                    i0 = g3;
                                                                                                    i1 = 1u;
                                                                                                    i0 += i1;
                                                                                                    g3 = i0;
                                                                                                    (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                                  } else {
                                                                                                    j0 = 4921565079997371264ull;
                                                                                                    j1 = p2;
                                                                                                    i0 = j0 == j1;
                                                                                                    if (i0) {
                                                                                                      j0 = p0;
                                                                                                      j1 = p1;
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
                                                                                                      f251(j0, j1);
                                                                                                      i0 = g3;
                                                                                                      i1 = 1u;
                                                                                                      i0 += i1;
                                                                                                      g3 = i0;
                                                                                                      (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                                    } else {
                                                                                                      j0 = 14029658124516851712ull;
                                                                                                      j1 = p2;
                                                                                                      i0 = j0 == j1;
                                                                                                      if (i0) {
                                                                                                        j0 = p0;
                                                                                                        j1 = p1;
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
                                                                                                        f252(j0, j1);
                                                                                                        i0 = g3;
                                                                                                        i1 = 1u;
                                                                                                        i0 += i1;
                                                                                                        g3 = i0;
                                                                                                        (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                                      } else {
                                                                                                        j0 = 13598438554052242944ull;
                                                                                                        j1 = p2;
                                                                                                        i0 = j0 == j1;
                                                                                                        if (i0) {
                                                                                                          j0 = p0;
                                                                                                          j1 = p1;
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
                                                                                                          f253(j0, j1);
                                                                                                          i0 = g3;
                                                                                                          i1 = 1u;
                                                                                                          i0 += i1;
                                                                                                          g3 = i0;
                                                                                                          (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                                        } else {
                                                                                                          j0 = 15371801113745664304ull;
                                                                                                          j1 = p2;
                                                                                                          i0 = j0 == j1;
                                                                                                          if (i0) {
                                                                                                            j0 = p0;
                                                                                                            j1 = p1;
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
                                                                                                            f254(j0, j1);
                                                                                                            i0 = g3;
                                                                                                            i1 = 1u;
                                                                                                            i0 += i1;
                                                                                                            g3 = i0;
                                                                                                            (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                                          } else {
                                                                                                            j0 = 4292831259513585664ull;
                                                                                                            j1 = p2;
                                                                                                            i0 = j0 == j1;
                                                                                                            if (i0) {
                                                                                                              j0 = p0;
                                                                                                              j1 = p1;
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
                                                                                                              f255(j0, j1);
                                                                                                              i0 = g3;
                                                                                                              i1 = 1u;
                                                                                                              i0 += i1;
                                                                                                              g3 = i0;
                                                                                                              (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                                            } else {
                                                                                                              j0 = 4292903715935354880ull;
                                                                                                              j1 = p2;
                                                                                                              i0 = j0 == j1;
                                                                                                              if (i0) {
                                                                                                                j0 = p0;
                                                                                                                j1 = p1;
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
                                                                                                                f256(j0, j1);
                                                                                                                i0 = g3;
                                                                                                                i1 = 1u;
                                                                                                                i0 += i1;
                                                                                                                g3 = i0;
                                                                                                                (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                                              } else {
                                                                                                                j0 = p0;
                                                                                                                j1 = 6138663577826885632ull;
                                                                                                                i0 = j0 != j1;
                                                                                                                if (i0) {
                                                                                                                  i0 = 0u;
                                                                                                                  j1 = 8000000000000000000ull;
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
                                                                                                                  (*Z_envZ_eosio_assert_codeZ_vij)(i0, j1);
                                                                                                                  i0 = g3;
                                                                                                                  i1 = 1u;
                                                                                                                  i0 += i1;
                                                                                                                  g3 = i0;
                                                                                                                  (*Z_eosio_injectionZ_checktimeZ_vv)();
                                                                                                                }
                                                                                                              }
                                                                                                            }
                                                                                                          }
                                                                                                        }
                                                                                                      }
                                                                                                    }
                                                                                                  }
                                                                                                }
                                                                                              }
                                                                                            }
                                                                                          }
                                                                                        }
                                                                                      }
                                                                                    }
                                                                                  }
                                                                                }
                                                                              }
                                                                            }
                                                                          }
                                                                        }
                                                                      }
                                                                    }
                                                                  }
                                                                }
                                                              }
                                                            }
                                                          }
                                                        }
                                                      }
                                                    }
                                                  }
                                                }
                                              }
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  } else {
    j0 = 6138663577826885632ull;
    j1 = p1;
    i0 = j0 == j1;
    if (i0) {
      j0 = 11877535737890996224ull;
      j1 = p2;
      i0 = j0 == j1;
      if (i0) {
        i0 = 0u;
        j1 = 8000000000000000001ull;
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
        (*Z_envZ_eosio_assert_codeZ_vij)(i0, j1);
        i0 = g3;
        i1 = 1u;
        i0 += i1;
        g3 = i0;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
      }
    }
  }
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
  f136(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static u32 f97(u32 p0) {
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
  i1 = 8305u;
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

static void f98(u32 p0) {
  FUNC_PROLOGUE;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static void f99(void) {
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

static u32 f100(void) {
  FUNC_PROLOGUE;
  u32 i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 8208u;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f101(u32 p0, u32 p1) {
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
  i0 = f97(i0);
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
  i0 = f100();
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
  i0 = f97(i0);
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
  f98(i0);
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

static u32 f102(u32 p0, u32 p1, u32 p2) {
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
  i0 = f101(i0, i1);
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
  i0 = f100();
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

static u32 f103(u32 p0) {
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
  i0 = f97(i0);
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
    i0 = f97(i0);
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

static u32 f104(u32 p0) {
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
  return i0;
}

static void f105(u32 p0) {
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
  f98(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  FUNC_EPILOGUE;
}

static void f106(u32 p0) {
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static u32 f107(u32 p0, u32 p1) {
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
  i0 = f102(i0, i1, i2);
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
    i0 = f102(i0, i1, i2);
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

static u32 f108(u32 p0, u32 p1) {
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
  i0 = f107(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
  return i0;
}

static void f109(u32 p0, u32 p1) {
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
  f98(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  FUNC_EPILOGUE;
}

static void f110(u32 p0, u32 p1) {
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
  f109(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static void f111(u32 p0) {
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

static void f112(u32 p0) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 96u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = l0;
  i1 = l0;
  i2 = 88u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l0;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
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
  i0 = f267(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  i2 = l0;
  i2 = i32_load((&M0), (u64)(i2));
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
  (*Z_envZ_set_blockchain_parameters_packedZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 96u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f113(u32 p0) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 96u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  i1 = 72u;
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
  i0 = (*Z_envZ_get_blockchain_parameters_packedZ_iii)(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i1 = 73u;
  i0 = i0 < i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 8330u;
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
  i0 = l0;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i2 = l1;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l0;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
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
  i0 = f250(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 96u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f114(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  u64 l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 32u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = 0u;
  l1 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1));
  l3 = i1;
  i0 -= i1;
  i1 = 48u;
  i0 = I32_DIV_S(i0, i1);
  j0 = (u64)(i0);
  l4 = j0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    j0 = l4;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L0;}
  i0 = l3;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = 41u;
    i0 += i1;
    l1 = i0;
    i0 = l3;
    j0 = i64_load32_u((&M0), (u64)(i0 + 8));
    l4 = j0;
    L5: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l1;
      i1 = 1u;
      i0 += i1;
      l1 = i0;
      j0 = l4;
      j1 = 7ull;
      j0 >>= (j1 & 63);
      l4 = j0;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto L5;}
    i0 = l3;
    i1 = 48u;
    i0 += i1;
    l3 = i0;
    i1 = l2;
    i0 = i0 != i1;
    if (i0) {goto L4;}
  i0 = l1;
  i0 = !(i0);
  if (i0) {goto B2;}
  B3:;
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
  f196(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l3 = i0;
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  goto B1;
  B2:;
  i0 = 0u;
  l3 = i0;
  i0 = 0u;
  l1 = i0;
  B1:;
  i0 = l0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
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
  i0 = f115(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  i2 = l1;
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
  j0 = (*Z_envZ_set_proposed_producersZ_jii)(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = j0;
  j1 = 0ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B10;}
  i0 = p0;
  j1 = l4;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 1u;
  i32_store8((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto B8;}
  goto B9;
  B10:;
  i0 = p0;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto B8;}
  B9:;
  i0 = l0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 4), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B8:;
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f115(u32 p0, u32 p1) {
  u32 l0 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  u64 l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1));
  i0 -= i1;
  i1 = 48u;
  i0 = I32_DIV_S(i0, i1);
  j0 = (u64)(i0);
  l1 = j0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l4 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l1;
    i0 = (u32)(j0);
    l5 = i0;
    i0 = l0;
    j1 = l1;
    j2 = 7ull;
    j1 >>= (j2 & 63);
    l1 = j1;
    j2 = 0ull;
    i1 = j1 != j2;
    l6 = i1;
    i2 = 7u;
    i1 <<= (i2 & 31);
    i2 = l5;
    i3 = 127u;
    i2 &= i3;
    i1 |= i2;
    i32_store8((&M0), (u64)(i0 + 15), i1);
    i0 = l3;
    i0 = i32_load((&M0), (u64)(i0));
    i1 = l2;
    i0 -= i1;
    i1 = 0u;
    i0 = (u32)((s32)i0 > (s32)i1);
    if (i0) {goto B1;}
    i0 = 0u;
    i1 = 8592u;
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
    i0 = l4;
    i0 = i32_load((&M0), (u64)(i0));
    l2 = i0;
    B1:;
    i0 = l2;
    i1 = l0;
    i2 = 15u;
    i1 += i2;
    i2 = 1u;
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
    i0 = l4;
    i1 = l4;
    i1 = i32_load((&M0), (u64)(i1));
    i2 = 1u;
    i1 += i2;
    l2 = i1;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l6;
    if (i0) {goto L0;}
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l7 = i0;
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  p1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B4;}
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l4 = i0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i0 = i32_load((&M0), (u64)(i0));
    i1 = l2;
    i0 -= i1;
    i1 = 7u;
    i0 = (u32)((s32)i0 > (s32)i1);
    if (i0) {goto B6;}
    i0 = 0u;
    i1 = 8592u;
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
    i0 = l4;
    i0 = i32_load((&M0), (u64)(i0));
    l2 = i0;
    B6:;
    i0 = l2;
    i1 = l7;
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
    i0 = l4;
    i1 = l4;
    i1 = i32_load((&M0), (u64)(i1));
    i2 = 8u;
    i1 += i2;
    l2 = i1;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l7;
    j0 = i64_load32_u((&M0), (u64)(i0 + 8));
    l1 = j0;
    L9: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l1;
      i0 = (u32)(j0);
      l5 = i0;
      i0 = l0;
      j1 = l1;
      j2 = 7ull;
      j1 >>= (j2 & 63);
      l1 = j1;
      j2 = 0ull;
      i1 = j1 != j2;
      l6 = i1;
      i2 = 7u;
      i1 <<= (i2 & 31);
      i2 = l5;
      i3 = 127u;
      i2 &= i3;
      i1 |= i2;
      i32_store8((&M0), (u64)(i0 + 14), i1);
      i0 = l3;
      i0 = i32_load((&M0), (u64)(i0));
      i1 = l2;
      i0 -= i1;
      i1 = 0u;
      i0 = (u32)((s32)i0 > (s32)i1);
      if (i0) {goto B10;}
      i0 = 0u;
      i1 = 8592u;
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
      i0 = l4;
      i0 = i32_load((&M0), (u64)(i0));
      l2 = i0;
      B10:;
      i0 = l2;
      i1 = l0;
      i2 = 14u;
      i1 += i2;
      i2 = 1u;
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
      i0 = l4;
      i1 = l4;
      i1 = i32_load((&M0), (u64)(i1));
      i2 = 1u;
      i1 += i2;
      l2 = i1;
      i32_store((&M0), (u64)(i0), i1);
      i0 = l6;
      if (i0) {goto L9;}
    i0 = l7;
    i1 = 12u;
    i0 += i1;
    l5 = i0;
    i0 = l3;
    i0 = i32_load((&M0), (u64)(i0));
    i1 = l2;
    i0 -= i1;
    i1 = 32u;
    i0 = (u32)((s32)i0 > (s32)i1);
    if (i0) {goto B13;}
    i0 = 0u;
    i1 = 8592u;
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
    i0 = l4;
    i0 = i32_load((&M0), (u64)(i0));
    l2 = i0;
    B13:;
    i0 = l2;
    i1 = l5;
    i2 = 33u;
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
    i0 = l4;
    i1 = l4;
    i1 = i32_load((&M0), (u64)(i1));
    i2 = 33u;
    i1 += i2;
    l2 = i1;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l7;
    i1 = 48u;
    i0 += i1;
    l7 = i0;
    i1 = p1;
    i0 = i0 != i1;
    if (i0) {goto L5;}
  B4:;
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u64 f116(void) {
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load8_u((&M0), (u64)(i0 + 8220));
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = 0u;
  j0 = i64_load((&M0), (u64)(i0 + 8232));
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
  j0 = (*Z_envZ_current_timeZ_jv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = j0;
  i0 = 0u;
  i1 = 1u;
  i32_store8((&M0), (u64)(i0 + 8220), i1);
  i0 = 0u;
  j1 = l0;
  i64_store((&M0), (u64)(i0 + 8232), j1);
  j0 = l0;
  Bfunc:;
  FUNC_EPILOGUE;
  return j0;
}

static u32 f117(void) {
  u32 l1 = 0;
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load8_u((&M0), (u64)(i0 + 8221));
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = 0u;
  i0 = i32_load((&M0), (u64)(i0 + 8224));
  goto Bfunc;
  B0:;
  i0 = 0u;
  i0 = i32_load8_u((&M0), (u64)(i0 + 8220));
  i0 = !(i0);
  if (i0) {goto B2;}
  i0 = 0u;
  j0 = i64_load((&M0), (u64)(i0 + 8232));
  l0 = j0;
  goto B1;
  B2:;
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
  j0 = (*Z_envZ_current_timeZ_jv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = j0;
  i0 = 0u;
  i1 = 1u;
  i32_store8((&M0), (u64)(i0 + 8220), i1);
  i0 = 0u;
  j1 = l0;
  i64_store((&M0), (u64)(i0 + 8232), j1);
  B1:;
  i0 = 0u;
  i1 = 1u;
  i32_store8((&M0), (u64)(i0 + 8221), i1);
  i0 = 0u;
  j1 = l0;
  j2 = 1000ull;
  j1 = I64_DIV_S(j1, j2);
  j2 = 18446743127024751616ull;
  j1 += j2;
  j2 = 500ull;
  j1 = I64_DIV_S(j1, j2);
  i1 = (u32)(j1);
  l1 = i1;
  i32_store((&M0), (u64)(i0 + 8224), i1);
  i0 = l1;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static void f118(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l3 = 0;
  u64 l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 48u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p1;
  i1 = p2;
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
  (*Z_envZ_sha256Z_viii)(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = 0ull;
  l1 = j0;
  i0 = 16u;
  p1 = i0;
  i0 = 0u;
  p2 = i0;
  j0 = 0ull;
  l2 = j0;
  L2: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i1 = 16u;
    i0 += i1;
    i1 = p2;
    i0 += i1;
    l3 = i0;
    i0 = p1;
    i1 = 2u;
    i0 = i0 < i1;
    if (i0) {goto B3;}
    j0 = l2;
    j1 = 8ull;
    j0 <<= (j1 & 63);
    j1 = l1;
    i2 = l3;
    j2 = i64_load8_u((&M0), (u64)(i2));
    j1 |= j2;
    l1 = j1;
    j2 = 56ull;
    j1 >>= (j2 & 63);
    j0 |= j1;
    l2 = j0;
    i0 = p1;
    i1 = 4294967295u;
    i0 += i1;
    p1 = i0;
    j0 = l1;
    j1 = 8ull;
    j0 <<= (j1 & 63);
    l1 = j0;
    i0 = p2;
    i1 = 1u;
    i0 += i1;
    p2 = i0;
    i1 = 32u;
    i0 = i0 != i1;
    if (i0) {goto L2;}
    goto B1;
    B3:;
    i0 = p1;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto B4;}
    i0 = 0u;
    i1 = 8350u;
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
    i0 = p0;
    j1 = l2;
    i64_store((&M0), (u64)(i0 + 8), j1);
    i0 = p0;
    j1 = l1;
    i2 = l3;
    j2 = i64_load8_u((&M0), (u64)(i2));
    j1 |= j2;
    i64_store((&M0), (u64)(i0), j1);
    i0 = 16u;
    p1 = i0;
    i0 = p0;
    i1 = 16u;
    i0 += i1;
    p0 = i0;
    j0 = 0ull;
    l1 = j0;
    j0 = 0ull;
    l2 = j0;
    i0 = p2;
    i1 = 1u;
    i0 += i1;
    p2 = i0;
    i1 = 32u;
    i0 = i0 != i1;
    if (i0) {goto L2;}
  B1:;
  i0 = p1;
  i1 = 16u;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  i0 = p1;
  i1 = 2u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
  i0 = l0;
  j1 = l1;
  j2 = l2;
  i3 = p1;
  i4 = 3u;
  i3 <<= (i4 & 31);
  i4 = 4294967288u;
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
  (*Z_envZ___ashlti3Z_vijji)(i0, j1, j2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  i0 = l0;
  j0 = i64_load((&M0), (u64)(i0));
  l1 = j0;
  B7:;
  i0 = p0;
  j1 = l1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  j1 = l2;
  i64_store((&M0), (u64)(i0 + 8), j1);
  B6:;
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f119(u32 p0) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  l0 = i0;
  i0 = p0;
  i1 = 3u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B2;}
  i0 = p0;
  i0 = i32_load8_u((&M0), (u64)(i0));
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = p0;
  i1 = 1u;
  i0 += i1;
  l0 = i0;
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i1 = 3u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto B2;}
    i0 = l0;
    i0 = i32_load8_u((&M0), (u64)(i0));
    l1 = i0;
    i0 = l0;
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    l0 = i0;
    i0 = l1;
    if (i0) {goto L3;}
  i0 = l2;
  i1 = 4294967295u;
  i0 += i1;
  i1 = p0;
  i0 -= i1;
  goto Bfunc;
  B2:;
  i0 = l0;
  i1 = 4294967292u;
  i0 += i1;
  l0 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i1 = 4u;
    i0 += i1;
    l0 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l1 = i0;
    i1 = 4294967295u;
    i0 ^= i1;
    i1 = l1;
    i2 = 4278124287u;
    i1 += i2;
    i0 &= i1;
    i1 = 2155905152u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto L4;}
  i0 = l1;
  i1 = 255u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B0;}
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i0 = i32_load8_u((&M0), (u64)(i0 + 1));
    l1 = i0;
    i0 = l0;
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    l0 = i0;
    i0 = l1;
    if (i0) {goto L5;}
  i0 = l2;
  i1 = p0;
  i0 -= i1;
  goto Bfunc;
  B1:;
  i0 = p0;
  i1 = p0;
  i0 -= i1;
  goto Bfunc;
  B0:;
  i0 = l0;
  i1 = p0;
  i0 -= i1;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f120(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i0 = !(i0);
  if (i0) {goto B1;}
  L2: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i0 = i32_load8_u((&M0), (u64)(i0));
    l0 = i0;
    i1 = p1;
    i1 = i32_load8_u((&M0), (u64)(i1));
    l1 = i1;
    i0 = i0 != i1;
    if (i0) {goto B0;}
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
  B1:;
  i0 = 0u;
  goto Bfunc;
  B0:;
  i0 = l0;
  i1 = l1;
  i0 -= i1;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static f64 f121(f64 p0, u32 p1) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j1, j2;
  f64 d0, d1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 1024u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B3;}
  d0 = p0;
  d1 = 8.9884656743115795e+307;
  d0 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d0, d1);
  p0 = d0;
  i0 = p1;
  i1 = 2047u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B2;}
  i0 = p1;
  i1 = 4294965250u;
  i0 += i1;
  p1 = i0;
  i1 = 1023u;
  i2 = p1;
  i3 = 1023u;
  i2 = (u32)((s32)i2 < (s32)i3);
  i0 = i2 ? i0 : i1;
  p1 = i0;
  d0 = p0;
  d1 = 8.9884656743115795e+307;
  d0 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d0, d1);
  p0 = d0;
  goto B0;
  B3:;
  i0 = p1;
  i1 = 4294966273u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B0;}
  d0 = p0;
  d1 = 2.0041683600089728e-292;
  d0 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d0, d1);
  p0 = d0;
  i0 = p1;
  i1 = 4294965304u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B1;}
  i0 = p1;
  i1 = 1938u;
  i0 += i1;
  p1 = i0;
  i1 = 4294966274u;
  i2 = p1;
  i3 = 4294966274u;
  i2 = (u32)((s32)i2 > (s32)i3);
  i0 = i2 ? i0 : i1;
  p1 = i0;
  d0 = p0;
  d1 = 2.0041683600089728e-292;
  d0 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d0, d1);
  p0 = d0;
  goto B0;
  B2:;
  i0 = p1;
  i1 = 4294966273u;
  i0 += i1;
  p1 = i0;
  goto B0;
  B1:;
  i0 = p1;
  i1 = 969u;
  i0 += i1;
  p1 = i0;
  B0:;
  d0 = p0;
  i1 = p1;
  i2 = 1023u;
  i1 += i2;
  j1 = (u64)(i1);
  j2 = 52ull;
  j1 <<= (j2 & 63);
  d1 = f64_reinterpret_i64(j1);
  d0 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d0, d1);
  FUNC_EPILOGUE;
  return d0;
}

static f64 f122(f64 p0) {
  FUNC_PROLOGUE;
  u64 j0, j1;
  f64 d0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  d0 = p0;
  j0 = i64_reinterpret_f64(d0);
  j1 = 9223372036854775807ull;
  j0 &= j1;
  d0 = f64_reinterpret_i64(j0);
  FUNC_EPILOGUE;
  return d0;
}

static u32 f123(u32 p0, u32 p1, u32 p2, u32 p3, u32 p4) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0, 
      l8 = 0, l9 = 0, l10 = 0, l11 = 0, l13 = 0, l14 = 0, l22 = 0;
  u64 l15 = 0, l16 = 0, l17 = 0, l18 = 0;
  f64 l12 = 0, l19 = 0, l20 = 0, l21 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8, i9, i10, i11, i12;
  u64 j0, j1, j2, j3, j4, j5, j6, j7;
  f64 d0, d1, d2, d3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 32u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p0;
  i1 = 1u;
  i2 = p1;
  i0 = i2 ? i0 : i1;
  l1 = i0;
  i0 = 0u;
  l2 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = 0u;
    i1 = l2;
    l4 = i1;
    i0 -= i1;
    l5 = i0;
    i0 = p3;
    i1 = 2u;
    i0 += i1;
    p0 = i0;
    i0 = 0u;
    l6 = i0;
    i0 = p3;
    i1 = 0u;
    i0 += i1;
    i0 = i32_load8_u((&M0), (u64)(i0));
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto B4;}
    L6: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l4;
      i1 = l6;
      i0 += i1;
      l7 = i0;
      i0 = l2;
      i1 = 37u;
      i0 = i0 == i1;
      if (i0) {goto B5;}
      i0 = l2;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (u32)((s32)i0 >> (i1 & 31));
      i1 = p1;
      i2 = l7;
      i3 = p2;
      i4 = l1;
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
      CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
      i0 = g3;
      i1 = 1u;
      i0 += i1;
      g3 = i0;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l5;
      i1 = 4294967295u;
      i0 += i1;
      l5 = i0;
      i0 = p0;
      i1 = 1u;
      i0 += i1;
      p0 = i0;
      i0 = p3;
      i1 = l6;
      i2 = 1u;
      i1 += i2;
      l6 = i1;
      i0 += i1;
      i0 = i32_load8_u((&M0), (u64)(i0));
      l2 = i0;
      if (i0) {goto L6;}
      goto B4;
    B5:;
    i0 = 0u;
    l8 = i0;
    L9: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = p0;
      i1 = 4294967295u;
      i0 += i1;
      l3 = i0;
      i0 = i32_load8_s((&M0), (u64)(i0));
      l2 = i0;
      i1 = 4294967264u;
      i0 += i1;
      l9 = i0;
      i1 = 16u;
      i0 = i0 > i1;
      if (i0) {goto B8;}
      i0 = 1u;
      p3 = i0;
      i0 = l9;
      switch (i0) {
        case 0: goto B14;
        case 1: goto B8;
        case 2: goto B8;
        case 3: goto B13;
        case 4: goto B8;
        case 5: goto B8;
        case 6: goto B8;
        case 7: goto B8;
        case 8: goto B8;
        case 9: goto B8;
        case 10: goto B8;
        case 11: goto B12;
        case 12: goto B8;
        case 13: goto B11;
        case 14: goto B8;
        case 15: goto B8;
        case 16: goto B10;
        default: goto B14;
      }
      B14:;
      i0 = 8u;
      p3 = i0;
      goto B10;
      B13:;
      i0 = 16u;
      p3 = i0;
      goto B10;
      B12:;
      i0 = 4u;
      p3 = i0;
      goto B10;
      B11:;
      i0 = 2u;
      p3 = i0;
      B10:;
      i0 = p0;
      i1 = 1u;
      i0 += i1;
      p0 = i0;
      i0 = l8;
      i1 = p3;
      i0 |= i1;
      l8 = i0;
      goto L9;
    B8:;
    i0 = l2;
    i1 = 4294967248u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 9u;
    i0 = i0 > i1;
    if (i0) {goto B15;}
    i0 = 0u;
    p3 = i0;
    L16: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = p3;
      i1 = 10u;
      i0 *= i1;
      i1 = l2;
      i2 = 24u;
      i1 <<= (i2 & 31);
      i2 = 24u;
      i1 = (u32)((s32)i1 >> (i2 & 31));
      i0 += i1;
      i1 = 4294967248u;
      i0 += i1;
      p3 = i0;
      i0 = l3;
      i0 = i32_load8_u((&M0), (u64)(i0 + 1));
      l2 = i0;
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      p0 = i0;
      l3 = i0;
      i0 = l2;
      i1 = 4294967248u;
      i0 += i1;
      i1 = 255u;
      i0 &= i1;
      i1 = 10u;
      i0 = i0 < i1;
      if (i0) {goto L16;}
    i0 = 0u;
    l9 = i0;
    i0 = l2;
    i1 = 255u;
    i0 &= i1;
    i1 = 46u;
    i0 = i0 != i1;
    if (i0) {goto B3;}
    goto B2;
    B15:;
    i0 = 0u;
    p3 = i0;
    i0 = l2;
    i1 = 42u;
    i0 = i0 != i1;
    if (i0) {goto B17;}
    i0 = l8;
    i1 = 2u;
    i0 |= i1;
    i1 = l8;
    i2 = p4;
    i2 = i32_load((&M0), (u64)(i2));
    l2 = i2;
    i3 = 0u;
    i2 = (u32)((s32)i2 < (s32)i3);
    i0 = i2 ? i0 : i1;
    l8 = i0;
    i0 = l2;
    i1 = l2;
    i2 = 31u;
    i1 = (u32)((s32)i1 >> (i2 & 31));
    p3 = i1;
    i0 += i1;
    i1 = p3;
    i0 ^= i1;
    p3 = i0;
    i0 = p4;
    i1 = 4u;
    i0 += i1;
    p4 = i0;
    i0 = 0u;
    l9 = i0;
    i0 = p0;
    i0 = i32_load8_u((&M0), (u64)(i0));
    l2 = i0;
    i1 = 255u;
    i0 &= i1;
    i1 = 46u;
    i0 = i0 == i1;
    if (i0) {goto B2;}
    goto B3;
    B17:;
    i0 = l3;
    p0 = i0;
    i0 = 0u;
    l9 = i0;
    i0 = l2;
    i1 = 255u;
    i0 &= i1;
    i1 = 46u;
    i0 = i0 == i1;
    if (i0) {goto B2;}
    goto B3;
    B4:;
    i0 = 0u;
    i1 = p1;
    i2 = l4;
    i3 = l6;
    i2 += i3;
    p0 = i2;
    i3 = p2;
    i4 = 4294967295u;
    i3 += i4;
    i4 = p0;
    i5 = p2;
    i4 = i4 < i5;
    i2 = i4 ? i2 : i3;
    i3 = p2;
    i4 = l1;
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
    CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i1 = 32u;
    i0 += i1;
    g0 = i0;
    i0 = p0;
    goto Bfunc;
    B3:;
    i0 = p0;
    l3 = i0;
    goto B1;
    B2:;
    i0 = p0;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i0 = l8;
    i1 = 1024u;
    i0 |= i1;
    l8 = i0;
    i0 = p0;
    i0 = i32_load8_u((&M0), (u64)(i0 + 1));
    l2 = i0;
    i1 = 4294967248u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 9u;
    i0 = i0 > i1;
    if (i0) {goto B19;}
    i0 = 0u;
    l9 = i0;
    L20: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l9;
      i1 = 10u;
      i0 *= i1;
      i1 = l2;
      i2 = 24u;
      i1 <<= (i2 & 31);
      i2 = 24u;
      i1 = (u32)((s32)i1 >> (i2 & 31));
      i0 += i1;
      i1 = 4294967248u;
      i0 += i1;
      l9 = i0;
      i0 = l3;
      i0 = i32_load8_u((&M0), (u64)(i0 + 1));
      l2 = i0;
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      p0 = i0;
      l3 = i0;
      i0 = l2;
      i1 = 4294967248u;
      i0 += i1;
      i1 = 255u;
      i0 &= i1;
      i1 = 10u;
      i0 = i0 < i1;
      if (i0) {goto L20;}
    i0 = p0;
    l3 = i0;
    goto B1;
    B19:;
    i0 = l2;
    i1 = 255u;
    i0 &= i1;
    i1 = 42u;
    i0 = i0 != i1;
    if (i0) {goto B1;}
    i0 = p4;
    i0 = i32_load((&M0), (u64)(i0));
    l2 = i0;
    i1 = 0u;
    i2 = l2;
    i3 = 0u;
    i2 = (u32)((s32)i2 > (s32)i3);
    i0 = i2 ? i0 : i1;
    l9 = i0;
    i0 = p0;
    i1 = 2u;
    i0 += i1;
    l3 = i0;
    i0 = p4;
    i1 = 4u;
    i0 += i1;
    p4 = i0;
    i0 = p0;
    i0 = i32_load8_u((&M0), (u64)(i0 + 2));
    l2 = i0;
    B1:;
    i0 = l2;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (u32)((s32)i0 >> (i1 & 31));
    i1 = 4294967192u;
    i0 += i1;
    i1 = 31u;
    i0 = I32_ROTL(i0, i1);
    p0 = i0;
    i1 = 9u;
    i0 = i0 > i1;
    if (i0) {goto B24;}
    i0 = p0;
    switch (i0) {
      case 0: goto B27;
      case 1: goto B26;
      case 2: goto B25;
      case 3: goto B24;
      case 4: goto B24;
      case 5: goto B24;
      case 6: goto B28;
      case 7: goto B24;
      case 8: goto B24;
      case 9: goto B28;
      default: goto B27;
    }
    B28:;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i0 = l8;
    i1 = 256u;
    i0 |= i1;
    l8 = i0;
    goto B24;
    B27:;
    i0 = l3;
    i0 = i32_load8_u((&M0), (u64)(i0 + 1));
    p0 = i0;
    i1 = 104u;
    i0 = i0 != i1;
    if (i0) {goto B23;}
    i0 = l3;
    i1 = 2u;
    i0 += i1;
    l3 = i0;
    i0 = l8;
    i1 = 192u;
    i0 |= i1;
    l8 = i0;
    goto B24;
    B26:;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i0 = l8;
    i1 = 512u;
    i0 |= i1;
    l8 = i0;
    goto B24;
    B25:;
    i0 = l3;
    i0 = i32_load8_u((&M0), (u64)(i0 + 1));
    p0 = i0;
    i1 = 108u;
    i0 = i0 != i1;
    if (i0) {goto B22;}
    i0 = l3;
    i1 = 2u;
    i0 += i1;
    l3 = i0;
    i0 = l8;
    i1 = 768u;
    i0 |= i1;
    l8 = i0;
    B24:;
    i0 = l3;
    i0 = i32_load8_u((&M0), (u64)(i0));
    p0 = i0;
    goto B21;
    B23:;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i0 = l8;
    i1 = 128u;
    i0 |= i1;
    l8 = i0;
    goto B21;
    B22:;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i0 = l8;
    i1 = 256u;
    i0 |= i1;
    l8 = i0;
    B21:;
    i0 = p0;
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 24u;
    i0 = (u32)((s32)i0 >> (i1 & 31));
    l10 = i0;
    i1 = 4294967259u;
    i0 += i1;
    l2 = i0;
    i1 = 83u;
    i0 = i0 > i1;
    if (i0) {goto B52;}
    i0 = l2;
    switch (i0) {
      case 0: goto B56;
      case 1: goto B52;
      case 2: goto B52;
      case 3: goto B52;
      case 4: goto B52;
      case 5: goto B52;
      case 6: goto B52;
      case 7: goto B52;
      case 8: goto B52;
      case 9: goto B52;
      case 10: goto B52;
      case 11: goto B52;
      case 12: goto B52;
      case 13: goto B52;
      case 14: goto B52;
      case 15: goto B52;
      case 16: goto B52;
      case 17: goto B52;
      case 18: goto B52;
      case 19: goto B52;
      case 20: goto B52;
      case 21: goto B52;
      case 22: goto B52;
      case 23: goto B52;
      case 24: goto B52;
      case 25: goto B52;
      case 26: goto B52;
      case 27: goto B52;
      case 28: goto B52;
      case 29: goto B52;
      case 30: goto B52;
      case 31: goto B52;
      case 32: goto B52;
      case 33: goto B62;
      case 34: goto B52;
      case 35: goto B52;
      case 36: goto B52;
      case 37: goto B52;
      case 38: goto B52;
      case 39: goto B52;
      case 40: goto B52;
      case 41: goto B52;
      case 42: goto B52;
      case 43: goto B52;
      case 44: goto B52;
      case 45: goto B52;
      case 46: goto B52;
      case 47: goto B52;
      case 48: goto B52;
      case 49: goto B52;
      case 50: goto B52;
      case 51: goto B63;
      case 52: goto B52;
      case 53: goto B52;
      case 54: goto B52;
      case 55: goto B52;
      case 56: goto B52;
      case 57: goto B52;
      case 58: goto B52;
      case 59: goto B52;
      case 60: goto B52;
      case 61: goto B63;
      case 62: goto B55;
      case 63: goto B63;
      case 64: goto B52;
      case 65: goto B62;
      case 66: goto B52;
      case 67: goto B52;
      case 68: goto B63;
      case 69: goto B52;
      case 70: goto B52;
      case 71: goto B52;
      case 72: goto B52;
      case 73: goto B52;
      case 74: goto B63;
      case 75: goto B54;
      case 76: goto B52;
      case 77: goto B52;
      case 78: goto B53;
      case 79: goto B52;
      case 80: goto B63;
      case 81: goto B52;
      case 82: goto B52;
      case 83: goto B63;
      default: goto B56;
    }
    B63:;
    i0 = p0;
    i1 = 255u;
    i0 &= i1;
    l6 = i0;
    i1 = 4294967208u;
    i0 += i1;
    l2 = i0;
    i1 = 32u;
    i0 = i0 > i1;
    if (i0) {goto B61;}
    i0 = 8u;
    p0 = i0;
    i0 = 16u;
    l5 = i0;
    i0 = l2;
    switch (i0) {
      case 0: goto B60;
      case 1: goto B61;
      case 2: goto B61;
      case 3: goto B61;
      case 4: goto B61;
      case 5: goto B61;
      case 6: goto B61;
      case 7: goto B61;
      case 8: goto B61;
      case 9: goto B61;
      case 10: goto B64;
      case 11: goto B61;
      case 12: goto B61;
      case 13: goto B61;
      case 14: goto B61;
      case 15: goto B61;
      case 16: goto B61;
      case 17: goto B61;
      case 18: goto B61;
      case 19: goto B61;
      case 20: goto B61;
      case 21: goto B61;
      case 22: goto B61;
      case 23: goto B59;
      case 24: goto B61;
      case 25: goto B61;
      case 26: goto B61;
      case 27: goto B61;
      case 28: goto B61;
      case 29: goto B61;
      case 30: goto B61;
      case 31: goto B61;
      case 32: goto B60;
      default: goto B60;
    }
    B64:;
    i0 = 2u;
    p0 = i0;
    goto B59;
    B62:;
    d0 = 0;
    i1 = p4;
    i2 = 7u;
    i1 += i2;
    i2 = 4294967288u;
    i1 &= i2;
    l11 = i1;
    d1 = f64_load((&M0), (u64)(i1));
    l12 = d1;
    d0 = (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(d0, d1);
    d1 = l12;
    d2 = l12;
    d3 = 0;
    i2 = (*Z_eosio_injectionZ__eosio_f64_ltZ_idd)(d2, d3);
    l13 = i2;
    d0 = i2 ? d0 : d1;
    l12 = d0;
    i0 = l9;
    i1 = 6u;
    i2 = l8;
    i3 = 1024u;
    i2 &= i3;
    i0 = i2 ? i0 : i1;
    p0 = i0;
    i1 = 10u;
    i0 = i0 < i1;
    if (i0) {goto B49;}
    i0 = 1u;
    l9 = i0;
    L66: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l0;
      i1 = l9;
      l2 = i1;
      i0 += i1;
      i1 = 4294967295u;
      i0 += i1;
      i1 = 48u;
      i32_store8((&M0), (u64)(i0), i1);
      i0 = l2;
      i1 = 1u;
      i0 += i1;
      l9 = i0;
      i0 = p0;
      i1 = 4294967295u;
      i0 += i1;
      p0 = i0;
      i0 = l2;
      i1 = 31u;
      i0 = i0 > i1;
      if (i0) {goto B65;}
      i0 = p0;
      i1 = 9u;
      i0 = i0 > i1;
      if (i0) {goto L66;}
    B65:;
    i0 = l2;
    i1 = 32u;
    i0 = i0 < i1;
    l14 = i0;
    i0 = l9;
    i1 = 4294967295u;
    i0 += i1;
    l9 = i0;
    goto B48;
    B61:;
    i0 = l8;
    i1 = 4294967279u;
    i0 &= i1;
    l8 = i0;
    i0 = 10u;
    l5 = i0;
    B60:;
    i0 = l8;
    i1 = 32u;
    i0 |= i1;
    i1 = l8;
    i2 = l6;
    i3 = 88u;
    i2 = i2 == i3;
    i0 = i2 ? i0 : i1;
    l8 = i0;
    i0 = l6;
    i1 = 100u;
    i0 = i0 == i1;
    if (i0) {goto B58;}
    i0 = l5;
    p0 = i0;
    i0 = l6;
    i1 = 105u;
    i0 = i0 == i1;
    if (i0) {goto B57;}
    B59:;
    i0 = l8;
    i1 = 4294967283u;
    i0 &= i1;
    l8 = i0;
    goto B57;
    B58:;
    i0 = l5;
    p0 = i0;
    B57:;
    i0 = l8;
    i1 = 4294967294u;
    i0 &= i1;
    i1 = l8;
    i2 = l8;
    i3 = 1024u;
    i2 &= i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    i0 = l6;
    i1 = 105u;
    i0 = i0 == i1;
    if (i0) {goto B71;}
    i0 = l6;
    i1 = 100u;
    i0 = i0 != i1;
    if (i0) {goto B70;}
    B71:;
    i0 = l2;
    i1 = 512u;
    i0 &= i1;
    if (i0) {goto B69;}
    i0 = l2;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto B68;}
    i0 = l2;
    i1 = 64u;
    i0 &= i1;
    if (i0) {goto B47;}
    i0 = l2;
    i1 = l2;
    i2 = 4294967279u;
    i1 &= i2;
    i2 = p4;
    i2 = i32_load((&M0), (u64)(i2));
    l6 = i2;
    i3 = 16u;
    i2 <<= (i3 & 31);
    i3 = 16u;
    i2 = (u32)((s32)i2 >> (i3 & 31));
    i3 = l6;
    i4 = l2;
    i5 = 128u;
    i4 &= i5;
    i2 = i4 ? i2 : i3;
    l10 = i2;
    i0 = i2 ? i0 : i1;
    l14 = i0;
    i0 = l10;
    if (i0) {goto B30;}
    goto B31;
    B70:;
    i0 = l2;
    i1 = 512u;
    i0 &= i1;
    if (i0) {goto B67;}
    i0 = l2;
    i1 = 256u;
    i0 &= i1;
    if (i0) {goto B46;}
    i0 = l2;
    i1 = 64u;
    i0 &= i1;
    if (i0) {goto B45;}
    i0 = l2;
    i1 = l2;
    i2 = 4294967279u;
    i1 &= i2;
    i2 = p4;
    i2 = i32_load((&M0), (u64)(i2));
    l6 = i2;
    i3 = 65535u;
    i2 &= i3;
    i3 = l6;
    i4 = l2;
    i5 = 128u;
    i4 &= i5;
    i2 = i4 ? i2 : i3;
    l6 = i2;
    i0 = i2 ? i0 : i1;
    l10 = i0;
    i0 = l6;
    i0 = !(i0);
    if (i0) {goto B44;}
    goto B43;
    B69:;
    i0 = l2;
    i1 = l2;
    i2 = 4294967279u;
    i1 &= i2;
    i2 = p4;
    i3 = 7u;
    i2 += i3;
    i3 = 4294967288u;
    i2 &= i3;
    l4 = i2;
    j2 = i64_load((&M0), (u64)(i2));
    l15 = j2;
    j3 = 0ull;
    i2 = j2 != j3;
    l6 = i2;
    i0 = i2 ? i0 : i1;
    l8 = i0;
    i0 = l6;
    if (i0) {goto B73;}
    i0 = 0u;
    l6 = i0;
    i0 = l8;
    i1 = 1024u;
    i0 &= i1;
    if (i0) {goto B72;}
    B73:;
    i0 = p0;
    j0 = (u64)(i0);
    l16 = j0;
    j0 = l15;
    j1 = l15;
    j2 = 63ull;
    j1 = (u64)((s64)j1 >> (j2 & 63));
    l17 = j1;
    j0 += j1;
    j1 = l17;
    j0 ^= j1;
    l17 = j0;
    i0 = l8;
    i1 = 32u;
    i0 &= i1;
    i1 = 97u;
    i0 ^= i1;
    i1 = 246u;
    i0 += i1;
    l5 = i0;
    i0 = 0u;
    l6 = i0;
    L74: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l0;
      i1 = l6;
      i0 += i1;
      i1 = 48u;
      i2 = l5;
      j3 = l17;
      j4 = l17;
      j5 = l16;
      j4 = DIV_U(j4, j5);
      l18 = j4;
      j5 = l16;
      j4 *= j5;
      j3 -= j4;
      i3 = (u32)(j3);
      l2 = i3;
      i4 = 24u;
      i3 <<= (i4 & 31);
      i4 = 167772160u;
      i3 = (u32)((s32)i3 < (s32)i4);
      i1 = i3 ? i1 : i2;
      i2 = l2;
      i1 += i2;
      i32_store8((&M0), (u64)(i0), i1);
      i0 = l6;
      i1 = 1u;
      i0 += i1;
      l6 = i0;
      i1 = 31u;
      i0 = i0 > i1;
      if (i0) {goto B72;}
      j0 = l17;
      j1 = l16;
      i0 = j0 >= j1;
      l2 = i0;
      j0 = l18;
      l17 = j0;
      i0 = l2;
      if (i0) {goto L74;}
    B72:;
    i0 = l4;
    i1 = 8u;
    i0 += i1;
    p4 = i0;
    i0 = l1;
    i1 = p1;
    i2 = l7;
    i3 = p2;
    i4 = l0;
    i5 = l6;
    j6 = l15;
    j7 = 63ull;
    j6 >>= (j7 & 63);
    i6 = (u32)(j6);
    i7 = p0;
    i8 = l9;
    i9 = p3;
    i10 = l8;
    i11 = g3;
    i11 = !(i11);
    if (i11) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i11 = 4294967295u;
      i12 = g3;
      i11 += i12;
      g3 = i11;
    }
    i0 = f125(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);
    i1 = g3;
    i2 = 1u;
    i1 += i2;
    g3 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    l2 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
    B68:;
    i0 = l2;
    i1 = l2;
    i2 = 4294967279u;
    i1 &= i2;
    i2 = p4;
    i2 = i32_load((&M0), (u64)(i2));
    l10 = i2;
    i0 = i2 ? i0 : i1;
    l14 = i0;
    i0 = l10;
    if (i0) {goto B77;}
    i0 = 0u;
    l2 = i0;
    i0 = l14;
    i1 = 1024u;
    i0 &= i1;
    if (i0) {goto B76;}
    B77:;
    i0 = l10;
    i1 = l10;
    i2 = 31u;
    i1 = (u32)((s32)i1 >> (i2 & 31));
    l6 = i1;
    i0 += i1;
    i1 = l6;
    i0 ^= i1;
    l6 = i0;
    i0 = l14;
    i1 = 32u;
    i0 &= i1;
    i1 = 97u;
    i0 ^= i1;
    i1 = 246u;
    i0 += i1;
    l4 = i0;
    i0 = 0u;
    l2 = i0;
    L78: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l0;
      i1 = l2;
      i0 += i1;
      i1 = 48u;
      i2 = l4;
      i3 = l6;
      i4 = l6;
      i5 = p0;
      i4 = DIV_U(i4, i5);
      l8 = i4;
      i5 = p0;
      i4 *= i5;
      i3 -= i4;
      l5 = i3;
      i4 = 24u;
      i3 <<= (i4 & 31);
      i4 = 167772160u;
      i3 = (u32)((s32)i3 < (s32)i4);
      i1 = i3 ? i1 : i2;
      i2 = l5;
      i1 += i2;
      i32_store8((&M0), (u64)(i0), i1);
      i0 = l2;
      i1 = 1u;
      i0 += i1;
      l2 = i0;
      i1 = 31u;
      i0 = i0 > i1;
      if (i0) {goto B76;}
      i0 = l6;
      i1 = p0;
      i0 = i0 >= i1;
      l5 = i0;
      i0 = l8;
      l6 = i0;
      i0 = l5;
      if (i0) {goto L78;}
    B76:;
    i0 = p4;
    i1 = 4u;
    i0 += i1;
    p4 = i0;
    i0 = l1;
    i1 = p1;
    i2 = l7;
    i3 = p2;
    i4 = l0;
    i5 = l2;
    i6 = l10;
    i7 = 31u;
    i6 >>= (i7 & 31);
    i7 = p0;
    i8 = l9;
    i9 = p3;
    i10 = l14;
    i11 = g3;
    i11 = !(i11);
    if (i11) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i11 = 4294967295u;
      i12 = g3;
      i11 += i12;
      g3 = i11;
    }
    i0 = f125(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);
    i1 = g3;
    i2 = 1u;
    i1 += i2;
    g3 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    l2 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
    B67:;
    i0 = l2;
    i1 = l2;
    i2 = 4294967279u;
    i1 &= i2;
    i2 = p4;
    i3 = 7u;
    i2 += i3;
    i3 = 4294967288u;
    i2 &= i3;
    l4 = i2;
    j2 = i64_load((&M0), (u64)(i2));
    l17 = j2;
    j3 = 0ull;
    i2 = j2 != j3;
    l6 = i2;
    i0 = i2 ? i0 : i1;
    l8 = i0;
    i0 = l6;
    if (i0) {goto B81;}
    i0 = 0u;
    l6 = i0;
    i0 = l8;
    i1 = 1024u;
    i0 &= i1;
    if (i0) {goto B80;}
    B81:;
    i0 = p0;
    j0 = (u64)(i0);
    l16 = j0;
    i0 = l8;
    i1 = 32u;
    i0 &= i1;
    i1 = 97u;
    i0 ^= i1;
    i1 = 246u;
    i0 += i1;
    l5 = i0;
    i0 = 0u;
    l6 = i0;
    L82: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l0;
      i1 = l6;
      i0 += i1;
      i1 = 48u;
      i2 = l5;
      j3 = l17;
      j4 = l17;
      j5 = l16;
      j4 = DIV_U(j4, j5);
      l18 = j4;
      j5 = l16;
      j4 *= j5;
      j3 -= j4;
      i3 = (u32)(j3);
      l2 = i3;
      i4 = 24u;
      i3 <<= (i4 & 31);
      i4 = 167772160u;
      i3 = (u32)((s32)i3 < (s32)i4);
      i1 = i3 ? i1 : i2;
      i2 = l2;
      i1 += i2;
      i32_store8((&M0), (u64)(i0), i1);
      i0 = l6;
      i1 = 1u;
      i0 += i1;
      l6 = i0;
      i1 = 31u;
      i0 = i0 > i1;
      if (i0) {goto B80;}
      j0 = l17;
      j1 = l16;
      i0 = j0 >= j1;
      l2 = i0;
      j0 = l18;
      l17 = j0;
      i0 = l2;
      if (i0) {goto L82;}
    B80:;
    i0 = l4;
    i1 = 8u;
    i0 += i1;
    p4 = i0;
    i0 = l1;
    i1 = p1;
    i2 = l7;
    i3 = p2;
    i4 = l0;
    i5 = l6;
    i6 = 0u;
    i7 = p0;
    i8 = l9;
    i9 = p3;
    i10 = l8;
    i11 = g3;
    i11 = !(i11);
    if (i11) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i11 = 4294967295u;
      i12 = g3;
      i11 += i12;
      g3 = i11;
    }
    i0 = f125(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);
    i1 = g3;
    i2 = 1u;
    i1 += i2;
    g3 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    l2 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
    B56:;
    i0 = 37u;
    i1 = p1;
    i2 = l7;
    i3 = p2;
    i4 = l1;
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
    CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    goto B51;
    B55:;
    i0 = 1u;
    l5 = i0;
    i0 = l8;
    i1 = 2u;
    i0 &= i1;
    l8 = i0;
    if (i0) {goto B85;}
    i0 = 2u;
    l5 = i0;
    i0 = p3;
    i1 = 2u;
    i0 = i0 < i1;
    if (i0) {goto B85;}
    i0 = p3;
    i1 = 4294967295u;
    i0 += i1;
    p0 = i0;
    i0 = p3;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    L86: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = 32u;
      i1 = p1;
      i2 = l7;
      i3 = p2;
      i4 = l1;
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
      CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
      i0 = g3;
      i1 = 1u;
      i0 += i1;
      g3 = i0;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l7;
      i1 = 1u;
      i0 += i1;
      l7 = i0;
      i0 = p0;
      i1 = 4294967295u;
      i0 += i1;
      p0 = i0;
      if (i0) {goto L86;}
    i0 = p3;
    i1 = l4;
    i0 += i1;
    i1 = l6;
    i0 += i1;
    i1 = 4294967295u;
    i0 += i1;
    l7 = i0;
    B85:;
    i0 = p4;
    i0 = i32_load8_s((&M0), (u64)(i0));
    i1 = p1;
    i2 = l7;
    i3 = p2;
    i4 = l1;
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
    CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l7;
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    i0 = p4;
    i1 = 4u;
    i0 += i1;
    p4 = i0;
    i0 = l8;
    i0 = !(i0);
    if (i0) {goto B50;}
    i0 = l5;
    i1 = p3;
    i0 = i0 >= i1;
    if (i0) {goto B50;}
    i0 = p3;
    i1 = l5;
    i0 -= i1;
    p0 = i0;
    i0 = l2;
    i1 = p3;
    i0 += i1;
    l6 = i0;
    L89: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = 32u;
      i1 = p1;
      i2 = l2;
      i3 = p2;
      i4 = l1;
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
      CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
      i0 = g3;
      i1 = 1u;
      i0 += i1;
      g3 = i0;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l2;
      i1 = 1u;
      i0 += i1;
      l2 = i0;
      i0 = p0;
      i1 = 4294967295u;
      i0 += i1;
      p0 = i0;
      if (i0) {goto L89;}
    i0 = l6;
    i1 = l5;
    i0 -= i1;
    l2 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
    B54:;
    i0 = l8;
    i1 = 33u;
    i0 |= i1;
    p0 = i0;
    i1 = p0;
    i2 = 4294967279u;
    i1 &= i2;
    i2 = p4;
    i2 = i32_load((&M0), (u64)(i2));
    p0 = i2;
    i0 = i2 ? i0 : i1;
    l5 = i0;
    i0 = p0;
    if (i0) {goto B92;}
    i0 = 0u;
    l6 = i0;
    i0 = l5;
    i1 = 1024u;
    i0 &= i1;
    if (i0) {goto B91;}
    B92:;
    i0 = 0u;
    l6 = i0;
    L93: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l0;
      i1 = l6;
      i0 += i1;
      i1 = 48u;
      i2 = 55u;
      i3 = p0;
      i4 = 15u;
      i3 &= i4;
      l2 = i3;
      i4 = 10u;
      i3 = i3 < i4;
      i1 = i3 ? i1 : i2;
      i2 = l2;
      i1 += i2;
      i32_store8((&M0), (u64)(i0), i1);
      i0 = l6;
      i1 = 1u;
      i0 += i1;
      l6 = i0;
      i1 = 31u;
      i0 = i0 > i1;
      if (i0) {goto B91;}
      i0 = p0;
      i1 = 15u;
      i0 = i0 > i1;
      l2 = i0;
      i0 = p0;
      i1 = 4u;
      i0 >>= (i1 & 31);
      p0 = i0;
      i0 = l2;
      if (i0) {goto L93;}
    B91:;
    i0 = p4;
    i1 = 4u;
    i0 += i1;
    p4 = i0;
    i0 = l1;
    i1 = p1;
    i2 = l7;
    i3 = p2;
    i4 = l0;
    i5 = l6;
    i6 = 0u;
    i7 = 16u;
    i8 = l9;
    i9 = 8u;
    i10 = l5;
    i11 = g3;
    i11 = !(i11);
    if (i11) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i11 = 4294967295u;
      i12 = g3;
      i11 += i12;
      g3 = i11;
    }
    i0 = f125(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);
    i1 = g3;
    i2 = 1u;
    i1 += i2;
    g3 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    l2 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
    B53:;
    i0 = p4;
    i0 = i32_load((&M0), (u64)(i0));
    l5 = i0;
    i1 = 4294967295u;
    i0 += i1;
    p0 = i0;
    L95: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = p0;
      i1 = 1u;
      i0 += i1;
      p0 = i0;
      i0 = i32_load8_u((&M0), (u64)(i0));
      if (i0) {goto L95;}
    i0 = p0;
    i1 = l5;
    i0 -= i1;
    p0 = i0;
    i1 = l9;
    i2 = p0;
    i3 = l9;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    i1 = p0;
    i2 = l8;
    i3 = 1024u;
    i2 &= i3;
    l10 = i2;
    i3 = 10u;
    i2 >>= (i3 & 31);
    i0 = i2 ? i0 : i1;
    l2 = i0;
    i0 = l8;
    i1 = 2u;
    i0 &= i1;
    l8 = i0;
    if (i0) {goto B40;}
    i0 = l2;
    i1 = p3;
    i0 = i0 >= i1;
    if (i0) {goto B41;}
    i0 = l4;
    i1 = p3;
    i2 = l2;
    i1 -= i2;
    i0 += i1;
    i1 = l6;
    i0 += i1;
    p0 = i0;
    L96: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = 32u;
      i1 = p1;
      i2 = l7;
      i3 = p2;
      i4 = l1;
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
      CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
      i0 = g3;
      i1 = 1u;
      i0 += i1;
      g3 = i0;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l7;
      i1 = 1u;
      i0 += i1;
      l7 = i0;
      i0 = l2;
      i1 = 1u;
      i0 += i1;
      l2 = i0;
      i1 = p3;
      i0 = i0 < i1;
      if (i0) {goto L96;}
    i0 = p3;
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    i0 = p0;
    l7 = i0;
    i0 = l5;
    i0 = i32_load8_u((&M0), (u64)(i0));
    p0 = i0;
    if (i0) {goto B39;}
    goto B38;
    B52:;
    i0 = l10;
    i1 = p1;
    i2 = l7;
    i3 = p2;
    i4 = l1;
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
    CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B51:;
    i0 = l7;
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    B50:;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
    B49:;
    i0 = 1u;
    l14 = i0;
    i0 = 0u;
    l9 = i0;
    B48:;
    d0 = l12;
    d1 = l12;
    d1 = (*Z_eosio_injectionZ__eosio_f64_truncZ_dd)(d1);
    d0 = (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(d0, d1);
    i1 = p0;
    i2 = 3u;
    i1 <<= (i2 & 31);
    i2 = 8816u;
    i1 += i2;
    d1 = f64_load((&M0), (u64)(i1));
    l19 = d1;
    d0 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d0, d1);
    l20 = d0;
    d1 = l20;
    d1 = (*Z_eosio_injectionZ__eosio_f64_truncZ_dd)(d1);
    d0 = (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(d0, d1);
    l21 = d0;
    d1 = 0.5;
    i0 = (*Z_eosio_injectionZ__eosio_f64_gtZ_idd)(d0, d1);
    l2 = i0;
    d0 = l20;
    d1 = 4294967296;
    i0 = (*Z_eosio_injectionZ__eosio_f64_ltZ_idd)(d0, d1);
    d1 = l20;
    d2 = 0;
    i1 = (*Z_eosio_injectionZ__eosio_f64_geZ_idd)(d1, d2);
    i0 &= i1;
    if (i0) {goto B100;}
    i0 = 0u;
    p4 = i0;
    goto B99;
    B100:;
    d0 = l20;
    i0 = (*Z_eosio_injectionZ__eosio_f64_trunc_i32uZ_id)(d0);
    p4 = i0;
    B99:;
    i0 = l2;
    i1 = 1u;
    i0 ^= i1;
    l2 = i0;
    d0 = l12;
    d0 = (*Z_eosio_injectionZ__eosio_f64_absZ_dd)(d0);
    d1 = 2147483648;
    i0 = (*Z_eosio_injectionZ__eosio_f64_ltZ_idd)(d0, d1);
    if (i0) {goto B102;}
    i0 = 2147483648u;
    l10 = i0;
    i0 = l2;
    i0 = !(i0);
    if (i0) {goto B101;}
    goto B37;
    B102:;
    d0 = l12;
    i0 = (*Z_eosio_injectionZ__eosio_f64_trunc_i32sZ_id)(d0);
    l10 = i0;
    i0 = l2;
    if (i0) {goto B37;}
    B101:;
    d0 = l19;
    i1 = p4;
    i2 = 1u;
    i1 += i2;
    p4 = i1;
    d1 = (f64)(i1);
    i0 = (*Z_eosio_injectionZ__eosio_f64_leZ_idd)(d0, d1);
    i1 = 1u;
    i0 ^= i1;
    if (i0) {goto B36;}
    i0 = l10;
    i1 = 1u;
    i0 += i1;
    l10 = i0;
    i0 = 0u;
    p4 = i0;
    goto B36;
    B47:;
    i0 = l2;
    i1 = l2;
    i2 = 4294967279u;
    i1 &= i2;
    i2 = p4;
    i2 = i32_load8_s((&M0), (u64)(i2));
    l10 = i2;
    i0 = i2 ? i0 : i1;
    l14 = i0;
    i0 = l10;
    i0 = !(i0);
    if (i0) {goto B31;}
    goto B30;
    B46:;
    i0 = l2;
    i1 = l2;
    i2 = 4294967279u;
    i1 &= i2;
    i2 = p4;
    i2 = i32_load((&M0), (u64)(i2));
    l6 = i2;
    i0 = i2 ? i0 : i1;
    l10 = i0;
    i0 = l6;
    if (i0) {goto B104;}
    i0 = 0u;
    l2 = i0;
    i0 = l10;
    i1 = 1024u;
    i0 &= i1;
    if (i0) {goto B103;}
    B104:;
    i0 = l10;
    i1 = 32u;
    i0 &= i1;
    i1 = 97u;
    i0 ^= i1;
    i1 = 246u;
    i0 += i1;
    l4 = i0;
    i0 = 0u;
    l2 = i0;
    L105: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l0;
      i1 = l2;
      i0 += i1;
      i1 = 48u;
      i2 = l4;
      i3 = l6;
      i4 = l6;
      i5 = p0;
      i4 = DIV_U(i4, i5);
      l8 = i4;
      i5 = p0;
      i4 *= i5;
      i3 -= i4;
      l5 = i3;
      i4 = 24u;
      i3 <<= (i4 & 31);
      i4 = 167772160u;
      i3 = (u32)((s32)i3 < (s32)i4);
      i1 = i3 ? i1 : i2;
      i2 = l5;
      i1 += i2;
      i32_store8((&M0), (u64)(i0), i1);
      i0 = l2;
      i1 = 1u;
      i0 += i1;
      l2 = i0;
      i1 = 31u;
      i0 = i0 > i1;
      if (i0) {goto B103;}
      i0 = l6;
      i1 = p0;
      i0 = i0 >= i1;
      l5 = i0;
      i0 = l8;
      l6 = i0;
      i0 = l5;
      if (i0) {goto L105;}
    B103:;
    i0 = p4;
    i1 = 4u;
    i0 += i1;
    p4 = i0;
    i0 = l1;
    i1 = p1;
    i2 = l7;
    i3 = p2;
    i4 = l0;
    i5 = l2;
    i6 = 0u;
    i7 = p0;
    i8 = l9;
    i9 = p3;
    i10 = l10;
    i11 = g3;
    i11 = !(i11);
    if (i11) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i11 = 4294967295u;
      i12 = g3;
      i11 += i12;
      g3 = i11;
    }
    i0 = f125(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);
    i1 = g3;
    i2 = 1u;
    i1 += i2;
    g3 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    l2 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
    B45:;
    i0 = l2;
    i1 = l2;
    i2 = 4294967279u;
    i1 &= i2;
    i2 = p4;
    i2 = i32_load8_u((&M0), (u64)(i2));
    l6 = i2;
    i0 = i2 ? i0 : i1;
    l10 = i0;
    i0 = l6;
    if (i0) {goto B43;}
    B44:;
    i0 = 0u;
    l2 = i0;
    i0 = l10;
    i1 = 1024u;
    i0 &= i1;
    if (i0) {goto B42;}
    B43:;
    i0 = l10;
    i1 = 32u;
    i0 &= i1;
    i1 = 97u;
    i0 ^= i1;
    i1 = 246u;
    i0 += i1;
    l4 = i0;
    i0 = 0u;
    l2 = i0;
    L107: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l0;
      i1 = l2;
      i0 += i1;
      i1 = 48u;
      i2 = l4;
      i3 = l6;
      i4 = l6;
      i5 = p0;
      i4 = DIV_U(i4, i5);
      l8 = i4;
      i5 = p0;
      i4 *= i5;
      i3 -= i4;
      l5 = i3;
      i4 = 24u;
      i3 <<= (i4 & 31);
      i4 = 167772160u;
      i3 = (u32)((s32)i3 < (s32)i4);
      i1 = i3 ? i1 : i2;
      i2 = l5;
      i1 += i2;
      i32_store8((&M0), (u64)(i0), i1);
      i0 = l2;
      i1 = 1u;
      i0 += i1;
      l2 = i0;
      i1 = 31u;
      i0 = i0 > i1;
      if (i0) {goto B42;}
      i0 = l6;
      i1 = p0;
      i0 = i0 >= i1;
      l5 = i0;
      i0 = l8;
      l6 = i0;
      i0 = l5;
      if (i0) {goto L107;}
    B42:;
    i0 = p4;
    i1 = 4u;
    i0 += i1;
    p4 = i0;
    i0 = l1;
    i1 = p1;
    i2 = l7;
    i3 = p2;
    i4 = l0;
    i5 = l2;
    i6 = 0u;
    i7 = p0;
    i8 = l9;
    i9 = p3;
    i10 = l10;
    i11 = g3;
    i11 = !(i11);
    if (i11) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i11 = 4294967295u;
      i12 = g3;
      i11 += i12;
      g3 = i11;
    }
    i0 = f125(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);
    i1 = g3;
    i2 = 1u;
    i1 += i2;
    g3 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    l2 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
    B41:;
    i0 = l2;
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    B40:;
    i0 = l5;
    i0 = i32_load8_u((&M0), (u64)(i0));
    p0 = i0;
    i0 = !(i0);
    if (i0) {goto B38;}
    B39:;
    i0 = l10;
    i0 = !(i0);
    if (i0) {goto B109;}
    i0 = l5;
    i1 = 1u;
    i0 += i1;
    l6 = i0;
    L110: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l9;
      i0 = !(i0);
      if (i0) {goto B38;}
      i0 = p0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (u32)((s32)i0 >> (i1 & 31));
      i1 = p1;
      i2 = l7;
      i3 = p2;
      i4 = l1;
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
      CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
      i0 = g3;
      i1 = 1u;
      i0 += i1;
      g3 = i0;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l7;
      i1 = 1u;
      i0 += i1;
      l7 = i0;
      i0 = l9;
      i1 = 4294967295u;
      i0 += i1;
      l9 = i0;
      i0 = l6;
      i0 = i32_load8_u((&M0), (u64)(i0));
      p0 = i0;
      i0 = l6;
      i1 = 1u;
      i0 += i1;
      l6 = i0;
      i0 = p0;
      if (i0) {goto L110;}
      goto B38;
    B109:;
    i0 = l5;
    i1 = 1u;
    i0 += i1;
    l6 = i0;
    L112: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = p0;
      i1 = 24u;
      i0 <<= (i1 & 31);
      i1 = 24u;
      i0 = (u32)((s32)i0 >> (i1 & 31));
      i1 = p1;
      i2 = l7;
      i3 = p2;
      i4 = l1;
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
      CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
      i0 = g3;
      i1 = 1u;
      i0 += i1;
      g3 = i0;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l7;
      i1 = 1u;
      i0 += i1;
      l7 = i0;
      i0 = l6;
      i0 = i32_load8_u((&M0), (u64)(i0));
      p0 = i0;
      i0 = l6;
      i1 = 1u;
      i0 += i1;
      l6 = i0;
      i0 = p0;
      if (i0) {goto L112;}
    B38:;
    i0 = p4;
    i1 = 4u;
    i0 += i1;
    p4 = i0;
    i0 = l8;
    i0 = !(i0);
    if (i0) {goto B115;}
    i0 = l2;
    i1 = p3;
    i0 = i0 >= i1;
    if (i0) {goto B114;}
    i0 = p3;
    i1 = l2;
    i0 -= i1;
    p0 = i0;
    i0 = l7;
    i1 = p3;
    i0 += i1;
    l6 = i0;
    L116: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = 32u;
      i1 = p1;
      i2 = l7;
      i3 = p2;
      i4 = l1;
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
      CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
      i0 = g3;
      i1 = 1u;
      i0 += i1;
      g3 = i0;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l7;
      i1 = 1u;
      i0 += i1;
      l7 = i0;
      i0 = p0;
      i1 = 4294967295u;
      i0 += i1;
      p0 = i0;
      if (i0) {goto L116;}
    i0 = l6;
    i1 = l2;
    i0 -= i1;
    l2 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
    B115:;
    i0 = l7;
    l2 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
    B114:;
    i0 = l7;
    l2 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
    B37:;
    d0 = l21;
    d1 = 0.5;
    i0 = (*Z_eosio_injectionZ__eosio_f64_neZ_idd)(d0, d1);
    if (i0) {goto B36;}
    i0 = p4;
    i1 = 1u;
    i0 &= i1;
    i1 = p4;
    i1 = !(i1);
    i0 |= i1;
    i1 = p4;
    i0 += i1;
    p4 = i0;
    i0 = 0u;
    l2 = i0;
    d0 = l12;
    d1 = 2147483647;
    i0 = (*Z_eosio_injectionZ__eosio_f64_gtZ_idd)(d0, d1);
    if (i0) {goto B34;}
    goto B35;
    B36:;
    i0 = 0u;
    l2 = i0;
    d0 = l12;
    d1 = 2147483647;
    i0 = (*Z_eosio_injectionZ__eosio_f64_gtZ_idd)(d0, d1);
    if (i0) {goto B34;}
    B35:;
    i0 = p0;
    i0 = !(i0);
    if (i0) {goto B121;}
    i0 = 32u;
    l2 = i0;
    i0 = l14;
    i0 = !(i0);
    if (i0) {goto B118;}
    L123: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l0;
      i1 = l9;
      i0 += i1;
      i1 = p4;
      i2 = p4;
      i3 = 10u;
      i2 = DIV_U(i2, i3);
      l2 = i2;
      i3 = 10u;
      i2 *= i3;
      i1 -= i2;
      i2 = 48u;
      i1 |= i2;
      i32_store8((&M0), (u64)(i0), i1);
      i0 = p0;
      i1 = 4294967295u;
      i0 += i1;
      p0 = i0;
      i0 = l9;
      i1 = 1u;
      i0 += i1;
      l9 = i0;
      i0 = p4;
      i1 = 10u;
      i0 = i0 < i1;
      if (i0) {goto B122;}
      i0 = l2;
      p4 = i0;
      i0 = l9;
      i1 = 32u;
      i0 = i0 < i1;
      if (i0) {goto L123;}
    B122:;
    i0 = l9;
    i1 = 32u;
    i0 = i0 < i1;
    p4 = i0;
    i0 = l9;
    i1 = 31u;
    i0 = i0 > i1;
    if (i0) {goto B124;}
    i0 = p0;
    i0 = !(i0);
    if (i0) {goto B124;}
    i0 = p0;
    i1 = 4294967295u;
    i0 += i1;
    l14 = i0;
    i0 = l0;
    i1 = l9;
    i0 += i1;
    l22 = i0;
    i0 = 0u;
    p0 = i0;
    L126: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l22;
      i1 = p0;
      i0 += i1;
      i1 = 48u;
      i32_store8((&M0), (u64)(i0), i1);
      i0 = p0;
      i1 = 1u;
      i0 += i1;
      l2 = i0;
      i0 = l9;
      i1 = p0;
      i0 += i1;
      i1 = 1u;
      i0 += i1;
      p4 = i0;
      i0 = l14;
      i1 = p0;
      i0 = i0 == i1;
      if (i0) {goto B125;}
      i0 = l2;
      p0 = i0;
      i0 = p4;
      i1 = 32u;
      i0 = i0 < i1;
      if (i0) {goto L126;}
    B125:;
    i0 = p4;
    i1 = 32u;
    i0 = i0 < i1;
    p4 = i0;
    i0 = l9;
    i1 = l2;
    i0 += i1;
    l9 = i0;
    B124:;
    i0 = p4;
    i0 = !(i0);
    if (i0) {goto B119;}
    i0 = l0;
    i1 = l9;
    i0 += i1;
    i1 = 46u;
    i32_store8((&M0), (u64)(i0), i1);
    i0 = 32u;
    l2 = i0;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    i1 = 31u;
    i0 = i0 <= i1;
    if (i0) {goto B120;}
    goto B118;
    B121:;
    d0 = l12;
    i1 = l10;
    d1 = (f64)(s32)(i1);
    d0 = (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(d0, d1);
    l12 = d0;
    d1 = 0.5;
    i0 = (*Z_eosio_injectionZ__eosio_f64_gtZ_idd)(d0, d1);
    i1 = 1u;
    i0 ^= i1;
    if (i0) {goto B127;}
    i0 = l10;
    i1 = 1u;
    i0 += i1;
    l10 = i0;
    i0 = 32u;
    l2 = i0;
    i0 = l9;
    i1 = 31u;
    i0 = i0 <= i1;
    if (i0) {goto B120;}
    goto B118;
    B127:;
    i0 = l10;
    i1 = l10;
    d2 = l12;
    d3 = 0.5;
    i2 = (*Z_eosio_injectionZ__eosio_f64_eqZ_idd)(d2, d3);
    i1 &= i2;
    i0 += i1;
    l10 = i0;
    i0 = 32u;
    l2 = i0;
    i0 = l9;
    i1 = 31u;
    i0 = i0 > i1;
    if (i0) {goto B118;}
    B120:;
    i0 = l9;
    l2 = i0;
    L128: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l0;
      i1 = l2;
      i0 += i1;
      i1 = l10;
      i2 = l10;
      i3 = 10u;
      i2 = I32_DIV_S(i2, i3);
      p0 = i2;
      i3 = 10u;
      i2 *= i3;
      i1 -= i2;
      i2 = 48u;
      i1 += i2;
      i32_store8((&M0), (u64)(i0), i1);
      i0 = l2;
      i1 = 1u;
      i0 += i1;
      l2 = i0;
      i1 = 31u;
      i0 = i0 > i1;
      if (i0) {goto B118;}
      i0 = l10;
      i1 = 9u;
      i0 += i1;
      l9 = i0;
      i0 = p0;
      l10 = i0;
      i0 = l9;
      i1 = 18u;
      i0 = i0 > i1;
      if (i0) {goto L128;}
      goto B118;
    B119:;
    i0 = l9;
    l2 = i0;
    B118:;
    i0 = l2;
    i1 = 31u;
    i0 = i0 > i1;
    if (i0) {goto B129;}
    i0 = l8;
    i1 = 3u;
    i0 &= i1;
    i1 = 1u;
    i0 = i0 != i1;
    if (i0) {goto B129;}
    i0 = l2;
    i1 = p3;
    i0 = i0 >= i1;
    if (i0) {goto B129;}
    L130: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l0;
      i1 = l2;
      i0 += i1;
      i1 = 48u;
      i32_store8((&M0), (u64)(i0), i1);
      i0 = l2;
      i1 = 1u;
      i0 += i1;
      l2 = i0;
      i1 = 31u;
      i0 = i0 > i1;
      if (i0) {goto B129;}
      i0 = l2;
      i1 = p3;
      i0 = i0 < i1;
      if (i0) {goto L130;}
    B129:;
    i0 = l2;
    i1 = p3;
    i0 = i0 != i1;
    if (i0) {goto B131;}
    i0 = p3;
    i1 = l8;
    i2 = 12u;
    i1 &= i2;
    i2 = 0u;
    i1 = i1 != i2;
    i2 = l13;
    i1 |= i2;
    i0 -= i1;
    l2 = i0;
    B131:;
    i0 = l8;
    i1 = 1u;
    i0 &= i1;
    p0 = i0;
    i0 = l2;
    i1 = 31u;
    i0 = i0 > i1;
    if (i0) {goto B132;}
    i0 = l13;
    i1 = 1u;
    i0 ^= i1;
    if (i0) {goto B134;}
    i0 = l0;
    i1 = l2;
    i0 += i1;
    i1 = 45u;
    i32_store8((&M0), (u64)(i0), i1);
    goto B133;
    B134:;
    i0 = l8;
    i1 = 4u;
    i0 &= i1;
    if (i0) {goto B135;}
    i0 = l8;
    i1 = 8u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto B132;}
    i0 = l0;
    i1 = l2;
    i0 += i1;
    i1 = 32u;
    i32_store8((&M0), (u64)(i0), i1);
    goto B133;
    B135:;
    i0 = l0;
    i1 = l2;
    i0 += i1;
    i1 = 43u;
    i32_store8((&M0), (u64)(i0), i1);
    B133:;
    i0 = l2;
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    B132:;
    i0 = l8;
    i1 = 2u;
    i0 &= i1;
    l9 = i0;
    i0 = p0;
    if (i0) {goto B136;}
    i0 = l9;
    if (i0) {goto B136;}
    i0 = l2;
    i1 = p3;
    i0 = i0 >= i1;
    if (i0) {goto B136;}
    i0 = p3;
    i1 = l2;
    i0 -= i1;
    p0 = i0;
    L137: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = 32u;
      i1 = p1;
      i2 = l7;
      i3 = p2;
      i4 = l1;
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
      CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
      i0 = g3;
      i1 = 1u;
      i0 += i1;
      g3 = i0;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l7;
      i1 = 1u;
      i0 += i1;
      l7 = i0;
      i0 = p0;
      i1 = 4294967295u;
      i0 += i1;
      p0 = i0;
      if (i0) {goto L137;}
    i0 = l4;
    i1 = p3;
    i2 = l2;
    i1 -= i2;
    i0 += i1;
    i1 = l6;
    i0 += i1;
    l7 = i0;
    B136:;
    i0 = l2;
    i0 = !(i0);
    if (i0) {goto B139;}
    i0 = l2;
    i1 = 4294967295u;
    i0 += i1;
    p0 = i0;
    i0 = l7;
    l8 = i0;
    L140: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l0;
      i1 = p0;
      i0 += i1;
      i0 = i32_load8_s((&M0), (u64)(i0));
      i1 = p1;
      i2 = l8;
      i3 = p2;
      i4 = l1;
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
      CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
      i0 = g3;
      i1 = 1u;
      i0 += i1;
      g3 = i0;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l8;
      i1 = 1u;
      i0 += i1;
      l8 = i0;
      i0 = p0;
      i1 = 4294967295u;
      i0 += i1;
      p0 = i0;
      i1 = 4294967295u;
      i0 = i0 != i1;
      if (i0) {goto L140;}
    i0 = l7;
    i1 = l2;
    i0 += i1;
    l7 = i0;
    B139:;
    i0 = l9;
    i0 = !(i0);
    if (i0) {goto B32;}
    i0 = l7;
    i1 = l4;
    i0 -= i1;
    i1 = l6;
    i0 -= i1;
    i1 = p3;
    i0 = i0 >= i1;
    if (i0) {goto B33;}
    i0 = l7;
    i1 = l5;
    i0 += i1;
    p0 = i0;
    L142: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = 32u;
      i1 = p1;
      i2 = l7;
      i3 = p2;
      i4 = l1;
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
      CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
      i0 = g3;
      i1 = 1u;
      i0 += i1;
      g3 = i0;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l7;
      i1 = 1u;
      i0 += i1;
      l7 = i0;
      i0 = p0;
      i1 = 1u;
      i0 += i1;
      p0 = i0;
      i1 = p3;
      i0 = i0 < i1;
      if (i0) {goto L142;}
    i0 = p3;
    i1 = l4;
    i0 += i1;
    i1 = l6;
    i0 += i1;
    l2 = i0;
    B34:;
    i0 = l11;
    i1 = 8u;
    i0 += i1;
    p4 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
    B33:;
    i0 = l7;
    l2 = i0;
    i0 = l11;
    i1 = 8u;
    i0 += i1;
    p4 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
    B32:;
    i0 = l7;
    l2 = i0;
    i0 = l11;
    i1 = 8u;
    i0 += i1;
    p4 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
    B31:;
    i0 = 0u;
    l2 = i0;
    i0 = l14;
    i1 = 1024u;
    i0 &= i1;
    if (i0) {goto B29;}
    B30:;
    i0 = l10;
    i1 = l10;
    i2 = 31u;
    i1 = (u32)((s32)i1 >> (i2 & 31));
    l6 = i1;
    i0 += i1;
    i1 = l6;
    i0 ^= i1;
    l6 = i0;
    i0 = l14;
    i1 = 32u;
    i0 &= i1;
    i1 = 97u;
    i0 ^= i1;
    i1 = 246u;
    i0 += i1;
    l4 = i0;
    i0 = 0u;
    l2 = i0;
    L144: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l0;
      i1 = l2;
      i0 += i1;
      i1 = 48u;
      i2 = l4;
      i3 = l6;
      i4 = l6;
      i5 = p0;
      i4 = DIV_U(i4, i5);
      l8 = i4;
      i5 = p0;
      i4 *= i5;
      i3 -= i4;
      l5 = i3;
      i4 = 24u;
      i3 <<= (i4 & 31);
      i4 = 167772160u;
      i3 = (u32)((s32)i3 < (s32)i4);
      i1 = i3 ? i1 : i2;
      i2 = l5;
      i1 += i2;
      i32_store8((&M0), (u64)(i0), i1);
      i0 = l2;
      i1 = 1u;
      i0 += i1;
      l2 = i0;
      i1 = 31u;
      i0 = i0 > i1;
      if (i0) {goto B29;}
      i0 = l6;
      i1 = p0;
      i0 = i0 >= i1;
      l5 = i0;
      i0 = l8;
      l6 = i0;
      i0 = l5;
      if (i0) {goto L144;}
    B29:;
    i0 = p4;
    i1 = 4u;
    i0 += i1;
    p4 = i0;
    i0 = l1;
    i1 = p1;
    i2 = l7;
    i3 = p2;
    i4 = l0;
    i5 = l2;
    i6 = l10;
    i7 = 31u;
    i6 >>= (i7 & 31);
    i7 = p0;
    i8 = l9;
    i9 = p3;
    i10 = l14;
    i11 = g3;
    i11 = !(i11);
    if (i11) {
      (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
    } else {
      i11 = 4294967295u;
      i12 = g3;
      i11 += i12;
      g3 = i11;
    }
    i0 = f125(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10);
    i1 = g3;
    i2 = 1u;
    i1 += i2;
    g3 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    l2 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    goto L0;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static void f124(u32 p0, u32 p1, u32 p2, u32 p3) {
  FUNC_PROLOGUE;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static u32 f125(u32 p0, u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, 
    u32 p8, u32 p9, u32 p10) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p10;
  i1 = 2u;
  i0 &= i1;
  l0 = i0;
  if (i0) {goto B0;}
  i0 = p5;
  i1 = p8;
  i0 = i0 >= i1;
  if (i0) {goto B0;}
  i0 = p5;
  i1 = 31u;
  i0 = i0 > i1;
  if (i0) {goto B0;}
  L1: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p4;
    i1 = p5;
    i0 += i1;
    i1 = 48u;
    i32_store8((&M0), (u64)(i0), i1);
    i0 = p5;
    i1 = 1u;
    i0 += i1;
    p5 = i0;
    i1 = p8;
    i0 = i0 >= i1;
    if (i0) {goto B0;}
    i0 = p5;
    i1 = 32u;
    i0 = i0 < i1;
    if (i0) {goto L1;}
  B0:;
  i0 = p10;
  i1 = 3u;
  i0 &= i1;
  i1 = 1u;
  i0 = i0 != i1;
  if (i0) {goto B2;}
  i0 = p5;
  i1 = p9;
  i0 = i0 >= i1;
  if (i0) {goto B2;}
  i0 = p5;
  i1 = 31u;
  i0 = i0 > i1;
  if (i0) {goto B2;}
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p4;
    i1 = p5;
    i0 += i1;
    i1 = 48u;
    i32_store8((&M0), (u64)(i0), i1);
    i0 = p5;
    i1 = 1u;
    i0 += i1;
    p5 = i0;
    i1 = p9;
    i0 = i0 >= i1;
    if (i0) {goto B2;}
    i0 = p5;
    i1 = 32u;
    i0 = i0 < i1;
    if (i0) {goto L3;}
  B2:;
  i0 = p10;
  i1 = 16u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B4;}
  i0 = p10;
  i1 = 1024u;
  i0 &= i1;
  if (i0) {goto B10;}
  i0 = p5;
  i0 = !(i0);
  if (i0) {goto B10;}
  i0 = p5;
  i1 = p8;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  i0 = p5;
  i1 = p9;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  B10:;
  i0 = p7;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B8;}
  goto B7;
  B9:;
  i0 = p5;
  i1 = 4294967294u;
  i0 += i1;
  i1 = p5;
  i2 = 4294967295u;
  i1 += i2;
  p5 = i1;
  i2 = p5;
  i0 = i2 ? i0 : i1;
  i1 = p5;
  i2 = p7;
  i3 = 16u;
  i2 = i2 == i3;
  i0 = i2 ? i0 : i1;
  p5 = i0;
  i0 = p7;
  i1 = 16u;
  i0 = i0 == i1;
  if (i0) {goto B7;}
  B8:;
  i0 = p7;
  i1 = 2u;
  i0 = i0 != i1;
  if (i0) {goto B6;}
  i0 = p5;
  i1 = 31u;
  i0 = i0 > i1;
  if (i0) {goto B6;}
  i0 = p4;
  i1 = p5;
  i0 += i1;
  i1 = 98u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p5;
  i1 = 1u;
  i0 += i1;
  p5 = i0;
  i1 = 31u;
  i0 = i0 <= i1;
  if (i0) {goto B5;}
  goto B4;
  B7:;
  i0 = p10;
  i1 = 32u;
  i0 &= i1;
  p8 = i0;
  if (i0) {goto B11;}
  i0 = p5;
  i1 = 31u;
  i0 = i0 > i1;
  if (i0) {goto B11;}
  i0 = p4;
  i1 = p5;
  i0 += i1;
  i1 = 120u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p5;
  i1 = 1u;
  i0 += i1;
  p5 = i0;
  i1 = 31u;
  i0 = i0 <= i1;
  if (i0) {goto B5;}
  goto B4;
  B11:;
  i0 = p8;
  i0 = !(i0);
  if (i0) {goto B6;}
  i0 = p5;
  i1 = 31u;
  i0 = i0 > i1;
  if (i0) {goto B6;}
  i0 = p4;
  i1 = p5;
  i0 += i1;
  i1 = 88u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p5;
  i1 = 1u;
  i0 += i1;
  p5 = i0;
  B6:;
  i0 = p5;
  i1 = 31u;
  i0 = i0 > i1;
  if (i0) {goto B4;}
  B5:;
  i0 = p4;
  i1 = p5;
  i0 += i1;
  i1 = 48u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p5;
  i1 = 1u;
  i0 += i1;
  p5 = i0;
  B4:;
  i0 = p5;
  i0 = !(i0);
  if (i0) {goto B12;}
  i0 = p5;
  i1 = p9;
  i0 = i0 != i1;
  if (i0) {goto B12;}
  i0 = p9;
  i1 = p10;
  i2 = 12u;
  i1 &= i2;
  i2 = 0u;
  i1 = i1 != i2;
  i2 = p6;
  i1 |= i2;
  i0 -= i1;
  p5 = i0;
  B12:;
  i0 = p10;
  i1 = 1u;
  i0 &= i1;
  p8 = i0;
  i0 = p5;
  i1 = 31u;
  i0 = i0 > i1;
  if (i0) {goto B15;}
  i0 = p6;
  i0 = !(i0);
  if (i0) {goto B16;}
  i0 = p4;
  i1 = p5;
  i0 += i1;
  i1 = 45u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p5;
  i1 = 1u;
  i0 += i1;
  p5 = i0;
  i0 = p2;
  p6 = i0;
  i0 = p8;
  i0 = !(i0);
  if (i0) {goto B14;}
  goto B13;
  B16:;
  i0 = p10;
  i1 = 4u;
  i0 &= i1;
  if (i0) {goto B17;}
  i0 = p10;
  i1 = 8u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B15;}
  i0 = p4;
  i1 = p5;
  i0 += i1;
  i1 = 32u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p5;
  i1 = 1u;
  i0 += i1;
  p5 = i0;
  i0 = p2;
  p6 = i0;
  i0 = p8;
  i0 = !(i0);
  if (i0) {goto B14;}
  goto B13;
  B17:;
  i0 = p4;
  i1 = p5;
  i0 += i1;
  i1 = 43u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p5;
  i1 = 1u;
  i0 += i1;
  p5 = i0;
  B15:;
  i0 = p2;
  p6 = i0;
  i0 = p8;
  if (i0) {goto B13;}
  B14:;
  i0 = p2;
  p6 = i0;
  i0 = l0;
  if (i0) {goto B13;}
  i0 = p2;
  p6 = i0;
  i0 = p5;
  i1 = p9;
  i0 = i0 >= i1;
  if (i0) {goto B13;}
  i0 = p9;
  i1 = p5;
  i0 -= i1;
  p10 = i0;
  i0 = p9;
  i1 = p2;
  i0 += i1;
  p6 = i0;
  i0 = p2;
  p8 = i0;
  L18: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = 32u;
    i1 = p1;
    i2 = p8;
    i3 = p3;
    i4 = p0;
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
    CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p8;
    i1 = 1u;
    i0 += i1;
    p8 = i0;
    i0 = p10;
    i1 = 4294967295u;
    i0 += i1;
    p10 = i0;
    if (i0) {goto L18;}
  i0 = p6;
  i1 = p5;
  i0 -= i1;
  p6 = i0;
  B13:;
  i0 = p5;
  i0 = !(i0);
  if (i0) {goto B20;}
  i0 = p5;
  i1 = 4294967295u;
  i0 += i1;
  p8 = i0;
  i0 = p6;
  p10 = i0;
  L21: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p4;
    i1 = p8;
    i0 += i1;
    i0 = i32_load8_s((&M0), (u64)(i0));
    i1 = p1;
    i2 = p10;
    i3 = p3;
    i4 = p0;
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
    CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p10;
    i1 = 1u;
    i0 += i1;
    p10 = i0;
    i0 = p8;
    i1 = 4294967295u;
    i0 += i1;
    p8 = i0;
    i1 = 4294967295u;
    i0 = i0 != i1;
    if (i0) {goto L21;}
  i0 = p5;
  i1 = p6;
  i0 += i1;
  p6 = i0;
  B20:;
  i0 = l0;
  i0 = !(i0);
  if (i0) {goto B23;}
  i0 = p6;
  i1 = p2;
  i0 -= i1;
  i1 = p9;
  i0 = i0 >= i1;
  if (i0) {goto B23;}
  i0 = 0u;
  i1 = p2;
  i0 -= i1;
  p5 = i0;
  L24: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = 32u;
    i1 = p1;
    i2 = p6;
    i3 = p3;
    i4 = p0;
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
    CALL_INDIRECT(T0, void (*)(u32, u32, u32, u32), 1, i4, i0, i1, i2, i3);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p5;
    i1 = p6;
    i2 = 1u;
    i1 += i2;
    p6 = i1;
    i0 += i1;
    i1 = p9;
    i0 = i0 < i1;
    if (i0) {goto L24;}
  i0 = p9;
  i1 = p2;
  i0 += i1;
  p6 = i0;
  B23:;
  i0 = p6;
  FUNC_EPILOGUE;
  return i0;
}

static void f126(u32 p0, u32 p1, u32 p2, u32 p3) {
  FUNC_PROLOGUE;
  u32 i0, i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i1 = p3;
  i0 = i0 >= i1;
  if (i0) {goto B0;}
  i0 = p1;
  i1 = p2;
  i0 += i1;
  i1 = p0;
  i32_store8((&M0), (u64)(i0), i1);
  B0:;
  FUNC_EPILOGUE;
}

static u32 f127(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = l0;
  i1 = p3;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = 2u;
  i1 = p0;
  i2 = p1;
  i3 = p2;
  i4 = p3;
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
  i0 = f123(i0, i1, i2, i3, i4);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = i0;
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  i0 = p3;
  FUNC_EPILOGUE;
  return i0;
}

static void f128(u32 p0) {
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

static u32 f129(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8, i9;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p1;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  i1 = p1;
  i1 = i32_load8_u((&M0), (u64)(i1));
  l0 = i1;
  i2 = 1u;
  i1 >>= (i2 & 31);
  i2 = l0;
  i3 = 1u;
  i2 &= i3;
  l1 = i2;
  i0 = i2 ? i0 : i1;
  l0 = i0;
  i0 = p1;
  i1 = 1u;
  i0 += i1;
  l2 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  l3 = i0;
  i0 = 10u;
  p1 = i0;
  i0 = p0;
  i0 = i32_load8_u((&M0), (u64)(i0));
  l4 = i0;
  i1 = 1u;
  i0 &= i1;
  l5 = i0;
  i0 = !(i0);
  if (i0) {goto B4;}
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = 4294967294u;
  i0 &= i1;
  i1 = 4294967295u;
  i0 += i1;
  p1 = i0;
  B4:;
  i0 = l3;
  i1 = l2;
  i2 = l1;
  i0 = i2 ? i0 : i1;
  l1 = i0;
  i0 = l0;
  i1 = p1;
  i0 = i0 <= i1;
  if (i0) {goto B7;}
  i0 = l5;
  if (i0) {goto B6;}
  i0 = l4;
  i1 = 1u;
  i0 >>= (i1 & 31);
  l2 = i0;
  goto B5;
  B7:;
  i0 = l5;
  if (i0) {goto B2;}
  i0 = p0;
  i1 = 1u;
  i0 += i1;
  p1 = i0;
  i0 = l0;
  if (i0) {goto B1;}
  goto B0;
  B6:;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l2 = i0;
  B5:;
  i0 = p0;
  i1 = p1;
  i2 = l0;
  i3 = p1;
  i2 -= i3;
  i3 = l2;
  i4 = 0u;
  i5 = l2;
  i6 = l0;
  i7 = l1;
  i8 = g3;
  i8 = !(i8);
  if (i8) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i8 = 4294967295u;
    i9 = g3;
    i8 += i9;
    g3 = i8;
  }
  f130(i0, i1, i2, i3, i4, i5, i6, i7);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B3:;
  i0 = p0;
  goto Bfunc;
  B2:;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  p1 = i0;
  i0 = l0;
  i0 = !(i0);
  if (i0) {goto B0;}
  B1:;
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
  i0 = (*Z_envZ_memmoveZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = p1;
  i1 = l0;
  i0 += i1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  i0 = i32_load8_u((&M0), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B10;}
  i0 = p0;
  i1 = l0;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  goto Bfunc;
  B10:;
  i0 = p0;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = p0;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static void f130(u32 p0, u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 4294967278u;
  i1 = p1;
  i0 -= i1;
  i1 = p2;
  i0 = i0 < i1;
  if (i0) {goto B0;}
  i0 = p0;
  i0 = i32_load8_u((&M0), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B3;}
  i0 = p0;
  i1 = 1u;
  i0 += i1;
  l0 = i0;
  i0 = 4294967279u;
  l1 = i0;
  i0 = p1;
  i1 = 2147483622u;
  i0 = i0 <= i1;
  if (i0) {goto B2;}
  goto B1;
  B3:;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  l0 = i0;
  i0 = 4294967279u;
  l1 = i0;
  i0 = p1;
  i1 = 2147483622u;
  i0 = i0 > i1;
  if (i0) {goto B1;}
  B2:;
  i0 = 11u;
  l1 = i0;
  i0 = p1;
  i1 = 1u;
  i0 <<= (i1 & 31);
  l2 = i0;
  i1 = p2;
  i2 = p1;
  i1 += i2;
  p2 = i1;
  i2 = p2;
  i3 = l2;
  i2 = i2 < i3;
  i0 = i2 ? i0 : i1;
  p2 = i0;
  i1 = 11u;
  i0 = i0 < i1;
  if (i0) {goto B1;}
  i0 = p2;
  i1 = 16u;
  i0 += i1;
  i1 = 4294967280u;
  i0 &= i1;
  l1 = i0;
  B1:;
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  i0 = p4;
  i0 = !(i0);
  if (i0) {goto B5;}
  i0 = p2;
  i1 = l0;
  i2 = p4;
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
  B5:;
  i0 = p6;
  i0 = !(i0);
  if (i0) {goto B7;}
  i0 = p2;
  i1 = p4;
  i0 += i1;
  i1 = p7;
  i2 = p6;
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
  B7:;
  i0 = p3;
  i1 = p5;
  i0 -= i1;
  p3 = i0;
  i1 = p4;
  i0 -= i1;
  p7 = i0;
  i0 = !(i0);
  if (i0) {goto B9;}
  i0 = p2;
  i1 = p4;
  i0 += i1;
  i1 = p6;
  i0 += i1;
  i1 = l0;
  i2 = p4;
  i1 += i2;
  i2 = p5;
  i1 += i2;
  i2 = p7;
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
  B9:;
  i0 = p1;
  i1 = 10u;
  i0 = i0 == i1;
  if (i0) {goto B11;}
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  i0 = p0;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = p0;
  i1 = l1;
  i2 = 1u;
  i1 |= i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = p3;
  i2 = p6;
  i1 += i2;
  p4 = i1;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = p2;
  i1 = p4;
  i0 += i1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
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
  (*Z_envZ_abortZ_vv)();
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f131(u32 p0, u32 p1) {
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
  i0 = f103(i0);
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
  f105(i0);
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

static u32 f132(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8, i9;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
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
  i0 = f119(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = p0;
  i0 = i32_load8_u((&M0), (u64)(i0));
  l1 = i0;
  i1 = 1u;
  i0 &= i1;
  l2 = i0;
  if (i0) {goto B6;}
  i0 = 10u;
  l3 = i0;
  i0 = 10u;
  i1 = l1;
  i2 = 1u;
  i1 >>= (i2 & 31);
  l1 = i1;
  i0 -= i1;
  i1 = l0;
  i0 = i0 < i1;
  if (i0) {goto B5;}
  goto B4;
  B6:;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = 4294967294u;
  i0 &= i1;
  i1 = 4294967295u;
  i0 += i1;
  l3 = i0;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l1 = i1;
  i0 -= i1;
  i1 = l0;
  i0 = i0 >= i1;
  if (i0) {goto B4;}
  B5:;
  i0 = p0;
  i1 = l3;
  i2 = l1;
  i3 = l0;
  i2 += i3;
  i3 = l3;
  i2 -= i3;
  i3 = l1;
  i4 = l1;
  i5 = 0u;
  i6 = l0;
  i7 = p1;
  i8 = g3;
  i8 = !(i8);
  if (i8) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i8 = 4294967295u;
    i9 = g3;
    i8 += i9;
    g3 = i8;
  }
  f130(i0, i1, i2, i3, i4, i5, i6, i7);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B3;
  B4:;
  i0 = l0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = l2;
  if (i0) {goto B2;}
  i0 = p0;
  i1 = 1u;
  i0 += i1;
  l3 = i0;
  goto B1;
  B3:;
  i0 = p0;
  goto Bfunc;
  B2:;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  l3 = i0;
  B1:;
  i0 = l3;
  i1 = l1;
  i0 += i1;
  i1 = p1;
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
  i0 = l1;
  i1 = l0;
  i0 += i1;
  l0 = i0;
  i0 = p0;
  i0 = i32_load8_u((&M0), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B9;}
  i0 = p0;
  i1 = l0;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&M0), (u64)(i0), i1);
  i0 = l3;
  i1 = l0;
  i0 += i1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  goto Bfunc;
  B9:;
  i0 = p0;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l3;
  i1 = l0;
  i0 += i1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f133(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8, i9;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load8_u((&M0), (u64)(i0));
  l0 = i0;
  i1 = 1u;
  i0 &= i1;
  l1 = i0;
  if (i0) {goto B5;}
  i0 = 10u;
  l2 = i0;
  i0 = 10u;
  i1 = l0;
  i2 = 1u;
  i1 >>= (i2 & 31);
  l0 = i1;
  i0 -= i1;
  i1 = p2;
  i0 = i0 < i1;
  if (i0) {goto B4;}
  goto B3;
  B5:;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = 4294967294u;
  i0 &= i1;
  i1 = 4294967295u;
  i0 += i1;
  l2 = i0;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l0 = i1;
  i0 -= i1;
  i1 = p2;
  i0 = i0 >= i1;
  if (i0) {goto B3;}
  B4:;
  i0 = p0;
  i1 = l2;
  i2 = l0;
  i3 = p2;
  i2 += i3;
  i3 = l2;
  i2 -= i3;
  i3 = l0;
  i4 = l0;
  i5 = 0u;
  i6 = p2;
  i7 = p1;
  i8 = g3;
  i8 = !(i8);
  if (i8) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i8 = 4294967295u;
    i9 = g3;
    i8 += i9;
    g3 = i8;
  }
  f130(i0, i1, i2, i3, i4, i5, i6, i7);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B2;
  B3:;
  i0 = p2;
  i0 = !(i0);
  if (i0) {goto B2;}
  i0 = l1;
  if (i0) {goto B1;}
  i0 = p0;
  i1 = 1u;
  i0 += i1;
  l2 = i0;
  goto B0;
  B2:;
  i0 = p0;
  goto Bfunc;
  B1:;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  l2 = i0;
  B0:;
  i0 = l2;
  i1 = l0;
  i0 += i1;
  i1 = p1;
  i2 = p2;
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
  i1 = p2;
  i0 += i1;
  p2 = i0;
  i0 = p0;
  i0 = i32_load8_u((&M0), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B8;}
  i0 = p0;
  i1 = p2;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&M0), (u64)(i0), i1);
  i0 = l2;
  i1 = p2;
  i0 += i1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  goto Bfunc;
  B8:;
  i0 = p0;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l2;
  i1 = p2;
  i0 += i1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f134(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8, i9;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load8_u((&M0), (u64)(i0));
  l0 = i0;
  i1 = 1u;
  i0 &= i1;
  l1 = i0;
  if (i0) {goto B4;}
  i0 = l0;
  i1 = 1u;
  i0 >>= (i1 & 31);
  l0 = i0;
  i1 = p1;
  i0 = i0 >= i1;
  if (i0) {goto B3;}
  goto B2;
  B4:;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l0 = i0;
  i1 = p1;
  i0 = i0 < i1;
  if (i0) {goto B2;}
  B3:;
  i0 = 10u;
  l2 = i0;
  i0 = l1;
  i0 = !(i0);
  if (i0) {goto B5;}
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = 4294967294u;
  i0 &= i1;
  i1 = 4294967295u;
  i0 += i1;
  l2 = i0;
  B5:;
  i0 = l2;
  i1 = l0;
  i0 -= i1;
  i1 = p3;
  i0 = i0 >= i1;
  if (i0) {goto B9;}
  i0 = p0;
  i1 = l2;
  i2 = l0;
  i3 = p3;
  i2 += i3;
  i3 = l2;
  i2 -= i3;
  i3 = l0;
  i4 = p1;
  i5 = 0u;
  i6 = p3;
  i7 = p2;
  i8 = g3;
  i8 = !(i8);
  if (i8) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i8 = 4294967295u;
    i9 = g3;
    i8 += i9;
    g3 = i8;
  }
  f130(i0, i1, i2, i3, i4, i5, i6, i7);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B8;
  B9:;
  i0 = p3;
  i0 = !(i0);
  if (i0) {goto B8;}
  i0 = l1;
  if (i0) {goto B7;}
  i0 = p0;
  i1 = 1u;
  i0 += i1;
  l1 = i0;
  i1 = p1;
  i0 += i1;
  l2 = i0;
  i0 = l0;
  i1 = p1;
  i0 -= i1;
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B6;}
  goto B1;
  B8:;
  i0 = p0;
  goto Bfunc;
  B7:;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  l1 = i0;
  i1 = p1;
  i0 += i1;
  l2 = i0;
  i0 = l0;
  i1 = p1;
  i0 -= i1;
  p1 = i0;
  if (i0) {goto B1;}
  B6:;
  i0 = l2;
  i1 = p2;
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
  i0 = (*Z_envZ_memmoveZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B0;
  B2:;
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
  B1:;
  i0 = l2;
  i1 = p3;
  i0 += i1;
  i1 = l2;
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
  i0 = (*Z_envZ_memmoveZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i1 = p2;
  i2 = p3;
  i1 += i2;
  i2 = p2;
  i3 = l1;
  i4 = l0;
  i3 += i4;
  i4 = p2;
  i3 = i3 > i4;
  i1 = i3 ? i1 : i2;
  i2 = p2;
  i3 = l2;
  i4 = p2;
  i3 = i3 <= i4;
  i1 = i3 ? i1 : i2;
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
  i0 = (*Z_envZ_memmoveZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l0;
  i1 = p3;
  i0 += i1;
  p3 = i0;
  i0 = p0;
  i0 = i32_load8_u((&M0), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B15;}
  i0 = p0;
  i1 = p3;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = p3;
  i0 += i1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  goto Bfunc;
  B15:;
  i0 = p0;
  i1 = p3;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = p3;
  i0 += i1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f135(u32 p0, u32 p1, u32 p2) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p1;
  i2 = p2;
  i3 = p2;
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
  i3 = f119(i3);
  i4 = g3;
  i5 = 1u;
  i4 += i5;
  g3 = i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
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
  i0 = f134(i0, i1, i2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
  return i0;
}

static void f136(u32 p0) {
  FUNC_PROLOGUE;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static u32 f137(u32 p0, u64 p1, u64 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  f64 l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  f64 d0, d1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 208u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = l0;
  i1 = 176u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i1 = p3;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = p3;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 176), j1);
  i0 = l0;
  i1 = 192u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  p3 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 176));
  i64_store((&M0), (u64)(i0 + 192), j1);
  i0 = p0;
  j1 = p1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  j1 = p2;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = p3;
  i1 = i32_load((&M0), (u64)(i1));
  p3 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 192));
  p2 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  i1 = p3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = p2;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 40u;
  i0 += i1;
  j1 = p1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = p0;
  i1 = 48u;
  i0 += i1;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 64u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = p0;
  i1 = 80u;
  i0 += i1;
  j1 = p1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 88u;
  i0 += i1;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 96u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 104u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 108u;
  i0 += i1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 112), j1);
  i0 = p0;
  i1 = 120u;
  i0 += i1;
  j1 = p1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 128u;
  i0 += i1;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 136u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 144u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 152), j1);
  i0 = p0;
  i1 = 160u;
  i0 += i1;
  j1 = p1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 168u;
  i0 += i1;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 176u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 184u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 192), j1);
  i0 = p0;
  i1 = 200u;
  i0 += i1;
  j1 = p1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 208u;
  i0 += i1;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 216u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 224u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 232), j1);
  i0 = p0;
  i1 = 240u;
  i0 += i1;
  j1 = p1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 248u;
  i0 += i1;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 256u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 264u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 344u;
  i0 += i1;
  j1 = 68719476736ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 352u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 360u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 368u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 376u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 392u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 384u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 400u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 408u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 416u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 424u;
  i0 += i1;
  i1 = 0u;
  i32_store16((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 432u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 440u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 0u;
  i32_store16((&M0), (u64)(i0 + 448), i1);
  i0 = p0;
  i1 = 452u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 464u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 472u;
  i0 += i1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 480), j1);
  i0 = p0;
  i1 = 488u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 496), j1);
  i0 = p0;
  i1 = 504u;
  i0 += i1;
  j1 = p1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 512u;
  i0 += i1;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 520u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 536), j1);
  i0 = p0;
  i1 = 524u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 544u;
  i0 += i1;
  j1 = p1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 552u;
  i0 += i1;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 560u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 568u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 576), j1);
  i0 = p0;
  i1 = 584u;
  i0 += i1;
  j1 = p1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 592u;
  i0 += i1;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 600u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 608u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 616), j1);
  i0 = p0;
  i1 = 624u;
  i0 += i1;
  j1 = p1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 632u;
  i0 += i1;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 640u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 648u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 656), j1);
  i0 = p0;
  i1 = 664u;
  i0 += i1;
  j1 = p1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 672u;
  i0 += i1;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 680u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 688u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 692u;
  i0 += i1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 696), i1);
  i0 = p0;
  i1 = 272u;
  i0 += i1;
  p3 = i0;
  j0 = p1;
  j1 = p1;
  j2 = 7235159537265672192ull;
  j3 = 7235159537265672192ull;
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
  if (i0) {goto B4;}
  i0 = p0;
  i1 = 152u;
  i0 += i1;
  l1 = i0;
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
  i0 = f138(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = i32_load((&M0), (u64)(i0 + 176));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  i0 = 0u;
  i1 = 12734u;
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
  B6:;
  i0 = p0;
  i1 = 180u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = p0;
  i2 = 176u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = l2;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 176));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l2;
  if (i0) {goto B1;}
  goto B2;
  B4:;
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 80), j1);
  i0 = l0;
  j1 = 68719476736ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 88), j1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 96), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 104), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 112), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 120), j1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 128), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 136), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 144), j1);
  i0 = l0;
  i1 = 0u;
  i32_store16((&M0), (u64)(i0 + 152), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 160), j1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 168), i1);
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
  f113(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B0;
  B3:;
  i0 = p0;
  i1 = 152u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 160u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 7235159537265672192ull;
  j3 = 7235159537265672192ull;
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
  if (i0) {goto B2;}
  i0 = l1;
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
  i0 = f138(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 176));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  i0 = 0u;
  i1 = 12734u;
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
  goto B1;
  B2:;
  i0 = 0u;
  l2 = i0;
  i0 = 0u;
  i1 = 13108u;
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
  i0 = l0;
  i1 = l2;
  i2 = 176u;
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
  i0 = p0;
  i1 = 192u;
  i0 += i1;
  l1 = i0;
  i0 = p3;
  i1 = l0;
  i2 = 172u;
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
  i1 = 220u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p3 = i0;
  i1 = p0;
  i2 = 216u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 == i1;
  if (i0) {goto B25;}
  i0 = p3;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p3 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B26;}
  i0 = 0u;
  i1 = 12734u;
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
  B26:;
  i0 = p3;
  if (i0) {goto B24;}
  i0 = 0u;
  l2 = i0;
  goto B23;
  B25:;
  i0 = 0u;
  l2 = i0;
  i0 = p0;
  i1 = 192u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 200u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 7235159538339414016ull;
  j3 = 7235159538339414016ull;
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
  p3 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B23;}
  i0 = l1;
  i1 = p3;
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
  i0 = f139(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = i32_load((&M0), (u64)(i0 + 32));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B24;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = p0;
  i1 = 220u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p3 = i0;
  i1 = p0;
  i2 = 216u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 == i1;
  if (i0) {goto B22;}
  i0 = p3;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p3 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B31;}
  i0 = 0u;
  i1 = 12734u;
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
  B31:;
  i0 = p3;
  if (i0) {goto B21;}
  goto B19;
  B23:;
  d0 = 0;
  l3 = d0;
  i0 = 0u;
  l4 = i0;
  i0 = 0u;
  l5 = i0;
  i0 = 0u;
  l6 = i0;
  goto B20;
  B22:;
  i0 = p0;
  i1 = 192u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 200u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 7235159538339414016ull;
  j3 = 7235159538339414016ull;
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
  p3 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B19;}
  i0 = l1;
  i1 = p3;
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
  i0 = f139(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B21;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = p3;
  i0 = i32_load8_u((&M0), (u64)(i0 + 24));
  l2 = i0;
  i0 = p3;
  d0 = f64_load((&M0), (u64)(i0 + 16));
  l3 = d0;
  i0 = p3;
  i0 = i32_load((&M0), (u64)(i0 + 12));
  l7 = i0;
  i0 = p3;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  l4 = i0;
  i0 = p3;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l5 = i0;
  i0 = p3;
  i0 = i32_load16_u((&M0), (u64)(i0 + 2));
  l1 = i0;
  i0 = p3;
  i0 = i32_load16_u((&M0), (u64)(i0));
  l6 = i0;
  B20:;
  i0 = p0;
  i1 = 232u;
  i0 += i1;
  p3 = i0;
  i0 = p0;
  i1 = 450u;
  i0 += i1;
  i1 = l1;
  i32_store16((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 448u;
  i0 += i1;
  i1 = l6;
  i32_store16((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 452u;
  i0 += i1;
  i1 = l5;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 456u;
  i0 += i1;
  i1 = l4;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 460u;
  i0 += i1;
  i1 = l7;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 464u;
  i0 += i1;
  d1 = l3;
  f64_store((&M0), (u64)(i0), d1);
  i0 = p0;
  i1 = 472u;
  i0 += i1;
  i1 = l2;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 260u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i1 = p0;
  i2 = 256u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 == i1;
  if (i0) {goto B41;}
  i0 = l1;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  i1 = p3;
  i0 = i0 == i1;
  if (i0) {goto B42;}
  i0 = 0u;
  i1 = 12734u;
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
  B42:;
  i0 = l1;
  if (i0) {goto B40;}
  goto B39;
  B41:;
  i0 = p0;
  i1 = 232u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 240u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 7235159538876284928ull;
  j3 = 7235159538876284928ull;
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
  if (i0) {goto B39;}
  i0 = p3;
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
  i0 = f140(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = i32_load((&M0), (u64)(i0 + 16));
  i1 = p3;
  i0 = i0 == i1;
  if (i0) {goto B40;}
  i0 = 0u;
  i1 = 12734u;
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
  B40:;
  i0 = p0;
  i1 = 260u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i1 = p0;
  i2 = 256u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 == i1;
  if (i0) {goto B38;}
  i0 = l1;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  i1 = p3;
  i0 = i0 == i1;
  if (i0) {goto B47;}
  i0 = 0u;
  i1 = 12734u;
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
  B47:;
  i0 = l1;
  if (i0) {goto B37;}
  goto B18;
  B39:;
  j0 = 0ull;
  p1 = j0;
  d0 = 0;
  l3 = d0;
  goto B36;
  B38:;
  i0 = p0;
  i1 = 232u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 240u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 7235159538876284928ull;
  j3 = 7235159538876284928ull;
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
  if (i0) {goto B18;}
  i0 = p3;
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
  i0 = f140(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  i1 = p3;
  i0 = i0 == i1;
  if (i0) {goto B37;}
  i0 = 0u;
  i1 = 12734u;
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
  B37:;
  i0 = l1;
  d0 = f64_load((&M0), (u64)(i0 + 8));
  l3 = d0;
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0));
  p1 = j0;
  B36:;
  i0 = p0;
  i1 = 488u;
  i0 += i1;
  d1 = l3;
  f64_store((&M0), (u64)(i0), d1);
  i0 = p0;
  i1 = 480u;
  i0 += i1;
  j1 = p1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  i1 = 208u;
  i0 += i1;
  g0 = i0;
  i0 = p0;
  goto Bfunc;
  B19:;
  i0 = 0u;
  i1 = 13108u;
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
  UNREACHABLE;
  B18:;
  i0 = 0u;
  i1 = 13108u;
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
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f138(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
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
  i1 = 12917u;
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
  i0 = f97(i0);
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
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l2;
  i2 = l3;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = 192u;
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 80), j1);
  i0 = l3;
  j1 = 68719476736ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 88), j1);
  i0 = l3;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 96), i1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 104), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 112), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 120), j1);
  i0 = l3;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 128), i1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 136), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 144), j1);
  i0 = l3;
  i1 = 0u;
  i32_store16((&M0), (u64)(i0 + 152), i1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 160), j1);
  i0 = l3;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 168), i1);
  i0 = l3;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 176), i1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  i1 = l3;
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
  i0 = f261(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 180), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  j1 = 7235159537265672192ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  l5 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i1 = p0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B14;}
  i0 = l0;
  j1 = 7235159537265672192ull;
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
  i0 = l5;
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
  i3 = 16u;
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
  f262(i0, i1, i2, i3);
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
  f98(i0);
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
  f105(i0);
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

static u32 f139(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
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
  i1 = 12917u;
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
  i0 = f97(i0);
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
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l2;
  i2 = l3;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = 48u;
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i1 = 0u;
  i32_store16((&M0), (u64)(i0), i1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 4), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l3;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 24), i1);
  i0 = l3;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  i1 = l3;
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
  i0 = f263(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  j1 = 7235159538339414016ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  l5 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i1 = p0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B14;}
  i0 = l0;
  j1 = 7235159538339414016ull;
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
  i0 = l5;
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
  i3 = 16u;
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
  f264(i0, i1, i2, i3);
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
  f98(i0);
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
  f105(i0);
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

static u32 f140(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
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
  i1 = 12917u;
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
  i0 = f97(i0);
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
  i0 = f103(i0);
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
  i1 = 13028u;
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
  i1 = 8u;
  i0 += i1;
  l5 = i0;
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
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  l6 = i0;
  i0 = l2;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = 8u;
  i0 = i0 != i1;
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = l5;
  i1 = l6;
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
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  j1 = 7235159538876284928ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
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
  j1 = 7235159538876284928ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l2;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 24), i1);
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
  i2 = 24u;
  i1 += i2;
  i2 = l1;
  i3 = 16u;
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
  f265(i0, i1, i2, i3);
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
  f98(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B17:;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 24));
  p1 = i0;
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 24), i1);
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
  f105(i0);
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

static u64 f141(u32 p0) {
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load8_u((&M0), (u64)(i0 + 8248));
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = 0u;
  j0 = i64_load((&M0), (u64)(i0 + 8240));
  goto Bfunc;
  B0:;
  i0 = p0;
  i1 = 496u;
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
  j0 = f142(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = j0;
  i0 = 0u;
  i1 = 1u;
  i32_store8((&M0), (u64)(i0 + 8248), i1);
  i0 = 0u;
  j1 = l0;
  i64_store((&M0), (u64)(i0 + 8240), j1);
  j0 = l0;
  Bfunc:;
  FUNC_EPILOGUE;
  return j0;
}

static u64 f142(u32 p0) {
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
  if (i0) {goto B2;}
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l3 = i0;
    j0 = i64_load((&M0), (u64)(i0 + 8));
    j1 = 4995142087184830980ull;
    i0 = j0 == j1;
    if (i0) {goto B3;}
    i0 = l2;
    l1 = i0;
    i0 = l0;
    i1 = l2;
    i0 = i0 != i1;
    if (i0) {goto L4;}
    goto B2;
  B3:;
  i0 = l0;
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0 + 64));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  i0 = 0u;
  i1 = 12734u;
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
  i1 = 48u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  goto Bfunc;
  B2:;
  i0 = p0;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 13377137154988703744ull;
  j3 = 4995142087184830980ull;
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
  if (i0) {goto B0;}
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
  i0 = f143(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 64));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l3;
  i1 = 48u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  goto Bfunc;
  B0:;
  i0 = 0u;
  i1 = 12504u;
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
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
  return j0;
}

static u32 f143(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0;
  u64 l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
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
  i1 = 12917u;
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
  i0 = f97(i0);
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
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l2;
  i2 = l3;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = 80u;
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
  i0 = f103(i0);
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
  j1 = 4602678819172646912ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l3;
  j1 = 4602678819172646912ull;
  i64_store((&M0), (u64)(i0 + 56), j1);
  i0 = l3;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 64), i1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  i1 = l3;
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
  i0 = f259(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 68), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  l5 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 12), i1);
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
  i3 = 16u;
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
  f192(i0, i1, i2, i3);
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
  f98(i0);
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
  f105(i0);
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

static u32 f144(u32 p0) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 152u;
  i0 += i1;
  i1 = p0;
  i2 = 272u;
  i1 += i2;
  i2 = p0;
  j2 = i64_load((&M0), (u64)(i2));
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
  f145(i0, i1, j2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 192u;
  i0 += i1;
  i1 = p0;
  i2 = 448u;
  i1 += i2;
  i2 = p0;
  j2 = i64_load((&M0), (u64)(i2));
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
  f146(i0, i1, j2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 232u;
  i0 += i1;
  i1 = p0;
  i2 = 480u;
  i1 += i2;
  i2 = p0;
  j2 = i64_load((&M0), (u64)(i2));
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
  f147(i0, i1, j2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 680u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = p0;
  i1 = 684u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  L6: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l3 = i0;
    i0 = l2;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto B7;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B7:;
    i0 = l0;
    i1 = l2;
    i0 = i0 != i1;
    if (i0) {goto L6;}
  i0 = p0;
  i1 = 680u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  goto B4;
  B5:;
  i0 = l0;
  l2 = i0;
  B4:;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B3:;
  i0 = p0;
  i1 = 640u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B10;}
  i0 = p0;
  i1 = 644u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B12;}
  i0 = l2;
  i1 = 4294967272u;
  i0 += i1;
  l2 = i0;
  L13: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
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
    i0 = f148(i0);
    i1 = g3;
    i2 = 1u;
    i1 += i2;
    g3 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    l3 = i0;
    i0 = l2;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    i0 = l3;
    i1 = l0;
    i0 = i0 != i1;
    if (i0) {goto L13;}
  i0 = p0;
  i1 = 640u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  goto B11;
  B12:;
  i0 = l0;
  l2 = i0;
  B11:;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B10:;
  i0 = p0;
  i1 = 600u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B16;}
  i0 = p0;
  i1 = 604u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B18;}
  L19: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l3 = i0;
    i0 = l2;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto B20;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B20:;
    i0 = l0;
    i1 = l2;
    i0 = i0 != i1;
    if (i0) {goto L19;}
  i0 = p0;
  i1 = 600u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  goto B17;
  B18:;
  i0 = l0;
  l2 = i0;
  B17:;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B16:;
  i0 = p0;
  i1 = 560u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B23;}
  i0 = p0;
  i1 = 564u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B25;}
  L26: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l3 = i0;
    i0 = l2;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto B27;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B27:;
    i0 = l0;
    i1 = l2;
    i0 = i0 != i1;
    if (i0) {goto L26;}
  i0 = p0;
  i1 = 560u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  goto B24;
  B25:;
  i0 = l0;
  l2 = i0;
  B24:;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B23:;
  i0 = p0;
  i1 = 520u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B30;}
  i0 = p0;
  i1 = 524u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B32;}
  L33: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l3 = i0;
    i0 = l2;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto B34;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B34:;
    i0 = l0;
    i1 = l2;
    i0 = i0 != i1;
    if (i0) {goto L33;}
  i0 = p0;
  i1 = 520u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  goto B31;
  B32:;
  i0 = l0;
  l2 = i0;
  B31:;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B30:;
  i0 = p0;
  i1 = 256u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B37;}
  i0 = p0;
  i1 = 260u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B39;}
  L40: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l3 = i0;
    i0 = l2;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto B41;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B41:;
    i0 = l0;
    i1 = l2;
    i0 = i0 != i1;
    if (i0) {goto L40;}
  i0 = p0;
  i1 = 256u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  goto B38;
  B39:;
  i0 = l0;
  l2 = i0;
  B38:;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B37:;
  i0 = p0;
  i1 = 216u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B44;}
  i0 = p0;
  i1 = 220u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B46;}
  L47: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l3 = i0;
    i0 = l2;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto B48;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B48:;
    i0 = l0;
    i1 = l2;
    i0 = i0 != i1;
    if (i0) {goto L47;}
  i0 = p0;
  i1 = 216u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  goto B45;
  B46:;
  i0 = l0;
  l2 = i0;
  B45:;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B44:;
  i0 = p0;
  i1 = 176u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B51;}
  i0 = p0;
  i1 = 180u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B53;}
  L54: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l3 = i0;
    i0 = l2;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto B55;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B55:;
    i0 = l0;
    i1 = l2;
    i0 = i0 != i1;
    if (i0) {goto L54;}
  i0 = p0;
  i1 = 176u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  goto B52;
  B53:;
  i0 = l0;
  l2 = i0;
  B52:;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B51:;
  i0 = p0;
  i1 = 136u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B58;}
  i0 = p0;
  i1 = 140u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B60;}
  L61: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l3 = i0;
    i0 = l2;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto B62;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B62:;
    i0 = l0;
    i1 = l2;
    i0 = i0 != i1;
    if (i0) {goto L61;}
  i0 = p0;
  i1 = 136u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  goto B59;
  B60:;
  i0 = l0;
  l2 = i0;
  B59:;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B58:;
  i0 = p0;
  i1 = 96u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B65;}
  i0 = p0;
  i1 = 100u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B67;}
  L68: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l3 = i0;
    i0 = l2;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto B69;}
    i0 = l3;
    i0 = i32_load8_u((&M0), (u64)(i0 + 60));
    i1 = 1u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto B70;}
    i0 = l3;
    i1 = 68u;
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B70:;
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B69:;
    i0 = l0;
    i1 = l2;
    i0 = i0 != i1;
    if (i0) {goto L68;}
  i0 = p0;
  i1 = 96u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  goto B66;
  B67:;
  i0 = l0;
  l2 = i0;
  B66:;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B65:;
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto B74;}
  i0 = p0;
  i1 = 60u;
  i0 += i1;
  l4 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B76;}
  L77: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l3 = i0;
    i0 = l2;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l3;
    i0 = !(i0);
    if (i0) {goto B78;}
    i0 = l3;
    i0 = i32_load((&M0), (u64)(i0 + 16));
    l0 = i0;
    i0 = !(i0);
    if (i0) {goto B79;}
    i0 = l3;
    i1 = 20u;
    i0 += i1;
    i1 = l0;
    i32_store((&M0), (u64)(i0), i1);
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B79:;
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B78:;
    i0 = l1;
    i1 = l2;
    i0 = i0 != i1;
    if (i0) {goto L77;}
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  goto B75;
  B76:;
  i0 = l1;
  l2 = i0;
  B75:;
  i0 = l4;
  i1 = l1;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B74:;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f145(u32 p0, u32 p1, u64 p2) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 24));
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = l1;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 176));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l1;
  if (i0) {goto B1;}
  goto B0;
  B2:;
  i0 = p0;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 7235159537265672192ull;
  j3 = 7235159537265672192ull;
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
  if (i0) {goto B0;}
  i0 = p0;
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
  i0 = f138(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 176));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = l1;
  j2 = p2;
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
  f149(i0, i1, j2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  goto Bfunc;
  B0:;
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  i1 = p0;
  j2 = p2;
  i3 = l0;
  i4 = 8u;
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
  f150(i0, i1, j2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f146(u32 p0, u32 p1, u64 p2) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 24));
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = l1;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l1;
  if (i0) {goto B1;}
  goto B0;
  B2:;
  i0 = p0;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 7235159538339414016ull;
  j3 = 7235159538339414016ull;
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
  if (i0) {goto B0;}
  i0 = p0;
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
  i0 = f139(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = l1;
  j2 = p2;
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
  f151(i0, i1, j2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  goto Bfunc;
  B0:;
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  i1 = p0;
  j2 = p2;
  i3 = l0;
  i4 = 8u;
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
  f152(i0, i1, j2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f147(u32 p0, u32 p1, u64 p2) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 24));
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = l1;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l1;
  if (i0) {goto B1;}
  goto B0;
  B2:;
  i0 = p0;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 7235159538876284928ull;
  j3 = 7235159538876284928ull;
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
  if (i0) {goto B0;}
  i0 = p0;
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
  i0 = f140(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = l1;
  j2 = p2;
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
  f153(i0, i1, j2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  goto Bfunc;
  B0:;
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  i1 = p0;
  j2 = p2;
  i3 = l0;
  i4 = 8u;
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
  f154(i0, i1, j2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  Bfunc:;
  FUNC_EPILOGUE;
}

static u32 f148(u32 p0) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i0 = p0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = l0;
  i1 = 64u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i1 = l0;
  i2 = 60u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l2 = i1;
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = l2;
  i1 = l0;
  i2 = 76u;
  i1 += i2;
  l3 = i1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = l0;
  i3 = 72u;
  i2 += i3;
  i2 = i32_load((&M0), (u64)(i2));
  l4 = i2;
  i1 += i2;
  l5 = i1;
  i2 = 6u;
  i1 >>= (i2 & 31);
  i2 = 67108860u;
  i1 &= i2;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l5;
  i2 = 255u;
  i1 &= i2;
  i2 = 4u;
  i1 <<= (i2 & 31);
  i0 += i1;
  l6 = i0;
  i1 = l2;
  i2 = l4;
  i3 = 6u;
  i2 >>= (i3 & 31);
  i3 = 67108860u;
  i2 &= i3;
  i1 += i2;
  l5 = i1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = l4;
  i3 = 255u;
  i2 &= i3;
  i3 = 4u;
  i2 <<= (i3 & 31);
  i1 += i2;
  l4 = i1;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 16u;
    i0 += i1;
    l4 = i0;
    i1 = l5;
    i1 = i32_load((&M0), (u64)(i1));
    i0 -= i1;
    i1 = 4096u;
    i0 = i0 != i1;
    if (i0) {goto B4;}
    i0 = l5;
    i0 = i32_load((&M0), (u64)(i0 + 4));
    l4 = i0;
    i0 = l5;
    i1 = 4u;
    i0 += i1;
    l5 = i0;
    B4:;
    i0 = l6;
    i1 = l4;
    i0 = i0 != i1;
    if (i0) {goto L3;}
    goto B1;
  B2:;
  i0 = l0;
  i1 = 76u;
  i0 += i1;
  l3 = i0;
  B1:;
  i0 = l3;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l2;
  i0 -= i1;
  i1 = 2u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  l5 = i0;
  i1 = 3u;
  i0 = i0 < i1;
  if (i0) {goto B5;}
  i0 = l0;
  i1 = 60u;
  i0 += i1;
  l4 = i0;
  i0 = l0;
  i1 = 64u;
  i0 += i1;
  l6 = i0;
  L6: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = l4;
    i1 = i32_load((&M0), (u64)(i1));
    i2 = 4u;
    i1 += i2;
    l2 = i1;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l6;
    i0 = i32_load((&M0), (u64)(i0));
    l1 = i0;
    i1 = l2;
    i0 -= i1;
    i1 = 2u;
    i0 = (u32)((s32)i0 >> (i1 & 31));
    l5 = i0;
    i1 = 2u;
    i0 = i0 > i1;
    if (i0) {goto L6;}
  B5:;
  i0 = l5;
  i1 = 1u;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  i0 = l5;
  i1 = 2u;
  i0 = i0 != i1;
  if (i0) {goto B8;}
  i0 = 256u;
  l4 = i0;
  goto B9;
  B10:;
  i0 = 128u;
  l4 = i0;
  B9:;
  i0 = l0;
  i1 = 72u;
  i0 += i1;
  i1 = l4;
  i32_store((&M0), (u64)(i0), i1);
  B8:;
  i0 = l2;
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B11;}
  L12: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = l2;
    i2 = 4u;
    i1 += i2;
    l2 = i1;
    i0 = i0 != i1;
    if (i0) {goto L12;}
  i0 = l0;
  i1 = 64u;
  i0 += i1;
  l4 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = l0;
  i2 = 60u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B11;}
  i0 = l4;
  i1 = l2;
  i2 = l2;
  i3 = 4294967292u;
  i2 += i3;
  i3 = l1;
  i2 -= i3;
  i3 = 4294967295u;
  i2 ^= i3;
  i3 = 4294967292u;
  i2 &= i3;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  B11:;
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 56));
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B14;}
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f149(u32 p0, u32 p1, u64 p2, u32 p3) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 176));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13402u;
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
  i1 = 13513u;
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
  i0 = p1;
  i1 = p3;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 172u;
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
  p3 = i0;
  i0 = l0;
  l0 = i0;
  i1 = 4294967136u;
  i0 += i1;
  p1 = i0;
  g0 = i0;
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i2 = 4294967290u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
  i1 = p3;
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
  i0 = f266(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  i0 = i32_load((&M0), (u64)(i0 + 180));
  j1 = p2;
  i2 = p1;
  i3 = 154u;
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
  i0 = p0;
  j0 = i64_load((&M0), (u64)(i0 + 16));
  j1 = 7235159537265672192ull;
  i0 = j0 > j1;
  if (i0) {goto B8;}
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 7235159537265672193ull;
  i64_store((&M0), (u64)(i0), j1);
  B8:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f150(u32 p0, u32 p1, u64 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 192u;
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
  j0 = (*Z_envZ_current_receiverZ_jv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13837u;
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
  i0 = 192u;
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 80), j1);
  i0 = l1;
  j1 = 68719476736ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 88), j1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 96), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 104), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 112), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 120), j1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 128), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 136), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 144), j1);
  i0 = l1;
  i1 = 0u;
  i32_store16((&M0), (u64)(i0 + 152), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 160), j1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 168), i1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 176), i1);
  i0 = l1;
  i1 = p3;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 172u;
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
  l1 = i0;
  i0 = l0;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i2 = 154u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 184), i1);
  i0 = l0;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 180), i1);
  i0 = l0;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 176), i1);
  i0 = l0;
  i1 = 176u;
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
  i0 = f266(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 7235159537265672192ull;
  j3 = p2;
  j4 = 7235159537265672192ull;
  i5 = l0;
  i6 = 16u;
  i5 += i6;
  i6 = 154u;
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
  l2 = i1;
  i32_store((&M0), (u64)(i0 + 180), i1);
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0 + 16));
  j1 = 7235159537265672192ull;
  i0 = j0 > j1;
  if (i0) {goto B7;}
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  j1 = 7235159537265672193ull;
  i64_store((&M0), (u64)(i0), j1);
  B7:;
  i0 = l0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 176), i1);
  i0 = l0;
  j1 = 7235159537265672192ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l0;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = p1;
  i1 = 28u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  p3 = i0;
  i1 = p1;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B9;}
  i0 = p3;
  j1 = 7235159537265672192ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p3;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 176), i1);
  i0 = p3;
  i1 = l1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l3;
  i1 = p3;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  goto B8;
  B9:;
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  i1 = l0;
  i2 = 176u;
  i1 += i2;
  i2 = l0;
  i3 = 16u;
  i2 += i3;
  i3 = l0;
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
  f262(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B8:;
  i0 = p0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 176));
  l1 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 176), i1);
  i0 = l1;
  i0 = !(i0);
  if (i0) {goto B11;}
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  i0 = l0;
  i1 = 192u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f151(u32 p0, u32 p1, u64 p2, u32 p3) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13402u;
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
  i1 = 13513u;
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
  i0 = p1;
  i1 = p3;
  i1 = i32_load((&M0), (u64)(i1));
  p3 = i1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  i1 = p3;
  i2 = 24u;
  i1 += i2;
  i1 = i32_load8_u((&M0), (u64)(i1));
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  i1 = p3;
  i2 = 16u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  i1 = p3;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  l0 = i0;
  i1 = 4294967264u;
  i0 += i1;
  p3 = i0;
  g0 = i0;
  i0 = l1;
  i1 = p3;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = p3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i2 = 4294967283u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
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
  i0 = f268(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 36u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  j1 = p2;
  i2 = p3;
  i3 = 19u;
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
  i0 = p0;
  j0 = i64_load((&M0), (u64)(i0 + 16));
  j1 = 7235159538339414016ull;
  i0 = j0 > j1;
  if (i0) {goto B7;}
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 7235159538339414017ull;
  i64_store((&M0), (u64)(i0), j1);
  B7:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f152(u32 p0, u32 p1, u64 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 64u;
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
  j0 = (*Z_envZ_current_receiverZ_jv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13837u;
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
  i0 = 48u;
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i1 = 0u;
  i32_store16((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 4), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = p3;
  i1 = i32_load((&M0), (u64)(i1));
  p3 = i1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = p3;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = p3;
  i2 = 16u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  i1 = p3;
  i2 = 24u;
  i1 += i2;
  i1 = i32_load8_u((&M0), (u64)(i1));
  i32_store8((&M0), (u64)(i0 + 24), i1);
  i0 = l0;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i2 = 19u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 56), i1);
  i0 = l0;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 52), i1);
  i0 = l0;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = l0;
  i1 = 48u;
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
  i0 = f268(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 7235159538339414016ull;
  j3 = p2;
  j4 = 7235159538339414016ull;
  i5 = l0;
  i6 = 16u;
  i5 += i6;
  i6 = 19u;
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
  l2 = i1;
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0 + 16));
  j1 = 7235159538339414016ull;
  i0 = j0 > j1;
  if (i0) {goto B6;}
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  j1 = 7235159538339414017ull;
  i64_store((&M0), (u64)(i0), j1);
  B6:;
  i0 = l0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = l0;
  j1 = 7235159538339414016ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l0;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = p1;
  i1 = 28u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  p3 = i0;
  i1 = p1;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B8;}
  i0 = p3;
  j1 = 7235159538339414016ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p3;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = p3;
  i1 = l1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l3;
  i1 = p3;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  goto B7;
  B8:;
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i2 = l0;
  i3 = 16u;
  i2 += i3;
  i3 = l0;
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
  f264(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B7:;
  i0 = p0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 48));
  l1 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = l1;
  i0 = !(i0);
  if (i0) {goto B10;}
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B10:;
  i0 = l0;
  i1 = 64u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f153(u32 p0, u32 p1, u64 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  l0 = i0;
  l1 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13402u;
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
  i1 = 13513u;
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
  i0 = p1;
  i1 = p3;
  i1 = i32_load((&M0), (u64)(i1));
  p3 = i1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  l2 = i0;
  i1 = p3;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  l0 = i0;
  i1 = 4294967280u;
  i0 += i1;
  p3 = i0;
  g0 = i0;
  i0 = p3;
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
  i1 = 4294967288u;
  i0 += i1;
  i1 = l2;
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
  i2 = p3;
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
  i0 = p0;
  j0 = i64_load((&M0), (u64)(i0 + 16));
  j1 = 7235159538876284928ull;
  i0 = j0 > j1;
  if (i0) {goto B8;}
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 7235159538876284929ull;
  i64_store((&M0), (u64)(i0), j1);
  B8:;
  i0 = l1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f154(u32 p0, u32 p1, u64 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 32u;
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
  j0 = (*Z_envZ_current_receiverZ_jv)();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13837u;
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = p3;
  i1 = i32_load((&M0), (u64)(i1));
  p3 = i1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = p3;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = 16u;
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
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  i1 = 8u;
  i0 |= i1;
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
  i0 = l1;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 7235159538876284928ull;
  j3 = p2;
  j4 = 7235159538876284928ull;
  i5 = l0;
  i6 = 16u;
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
  l2 = i1;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0 + 16));
  j1 = 7235159538876284928ull;
  i0 = j0 > j1;
  if (i0) {goto B7;}
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  j1 = 7235159538876284929ull;
  i64_store((&M0), (u64)(i0), j1);
  B7:;
  i0 = l0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  j1 = 7235159538876284928ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l0;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = p1;
  i1 = 28u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  p3 = i0;
  i1 = p1;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B9;}
  i0 = p3;
  j1 = 7235159538876284928ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p3;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = p3;
  i1 = l1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l3;
  i1 = p3;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  goto B8;
  B9:;
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i2 = l0;
  i3 = 16u;
  i2 += i3;
  i3 = l0;
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
  f265(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B8:;
  i0 = p0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  l1 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
  i0 = !(i0);
  if (i0) {goto B11;}
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f155(u32 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
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
  i0 = p0;
  i1 = 344u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  j1 = p1;
  i0 = j0 < j1;
  if (i0) {goto B1;}
  i0 = 0u;
  i1 = 8394u;
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
  j0 = p1;
  j1 = 1125899906842624ull;
  i0 = j0 < j1;
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 8598u;
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
  i0 = p0;
  i1 = 352u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  j1 = p1;
  i0 = j0 < j1;
  if (i0) {goto B5;}
  i0 = 0u;
  i1 = 8896u;
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
  i0 = l0;
  j1 = p1;
  i2 = p0;
  i3 = 344u;
  i2 += i3;
  j2 = i64_load((&M0), (u64)(i2));
  j1 -= j2;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p0;
  i1 = 496u;
  i0 += i1;
  l1 = i0;
  i0 = p0;
  i1 = 520u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = p0;
  i2 = 524u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l3 = i1;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  L12: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    l4 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l5 = i0;
    j0 = i64_load((&M0), (u64)(i0 + 8));
    j1 = 4995142087184830980ull;
    i0 = j0 == j1;
    if (i0) {goto B11;}
    i0 = l4;
    l3 = i0;
    i0 = l2;
    i1 = l4;
    i0 = i0 != i1;
    if (i0) {goto L12;}
    goto B10;
  B11:;
  i0 = l2;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  i0 = l5;
  i0 = i32_load((&M0), (u64)(i0 + 64));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 12734u;
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
  l5 = i0;
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 504u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 13377137154988703744ull;
  j3 = 4995142087184830980ull;
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
  l4 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B8;}
  i0 = l1;
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
  i0 = f143(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 64));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l0;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  goto B7;
  B8:;
  i0 = l0;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = 0u;
  i1 = 13253u;
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
  i1 = l5;
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
  f156(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 344u;
  i0 += i1;
  j1 = p1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f156(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0;
  u64 l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 64));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13402u;
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
  i1 = 13513u;
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
  i0 = p1;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i2 = p2;
  i2 = i32_load((&M0), (u64)(i2));
  j2 = i64_load((&M0), (u64)(i2));
  j1 += j2;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0 + 8));
  l2 = j0;
  i0 = l0;
  p2 = i0;
  i1 = 4294967232u;
  i0 += i1;
  l0 = i0;
  g0 = i0;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
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
  i0 = f191(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 68));
  j1 = 0ull;
  i2 = l0;
  i3 = 64u;
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
  if (i0) {goto B7;}
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l2;
  j3 = 1ull;
  j2 += j3;
  j3 = l2;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&M0), (u64)(i0), j1);
  B7:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f157(u32 p0) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l7 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
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
  i0 = f117();
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i1 = p0;
  i2 = 452u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 <= i1;
  if (i0) {goto B0;}
  i0 = p0;
  i1 = 496u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 520u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i1 = p0;
  i2 = 524u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l4 = i1;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l6 = i0;
    j0 = i64_load((&M0), (u64)(i0 + 8));
    j1 = 4995142087184830980ull;
    i0 = j0 == j1;
    if (i0) {goto B4;}
    i0 = l5;
    l4 = i0;
    i0 = l3;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L5;}
    goto B3;
  B4:;
  i0 = l3;
  i1 = l4;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = l6;
  i0 = i32_load((&M0), (u64)(i0 + 64));
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = 0u;
  i1 = 12734u;
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
  goto B2;
  B3:;
  i0 = 0u;
  l6 = i0;
  i0 = l2;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 504u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 13377137154988703744ull;
  j3 = 4995142087184830980ull;
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
  l5 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B2;}
  i0 = l2;
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
  i0 = f143(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 64));
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = 0u;
  i1 = 12734u;
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
  i1 = 344u;
  i0 += i1;
  l5 = i0;
  j0 = i64_load((&M0), (u64)(i0));
  l7 = j0;
  i0 = l0;
  i1 = l1;
  i2 = p0;
  i3 = 452u;
  i2 += i3;
  l4 = i2;
  i2 = i32_load((&M0), (u64)(i2));
  i1 -= i2;
  i2 = p0;
  i2 = i32_load16_u((&M0), (u64)(i2 + 448));
  i1 *= i2;
  l3 = i1;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = l5;
  j1 = l7;
  i2 = l3;
  j2 = (u64)(i2);
  j1 += j2;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  i1 = l0;
  i2 = 12u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l6;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 13253u;
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
  i0 = l2;
  i1 = l6;
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
  f158(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = l1;
  i32_store((&M0), (u64)(i0), i1);
  B0:;
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f158(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0;
  u64 l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 64));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13402u;
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
  i1 = 13513u;
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
  i0 = p1;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i2 = p2;
  i2 = i32_load((&M0), (u64)(i2));
  j2 = i64_load32_u((&M0), (u64)(i2));
  j1 += j2;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0 + 8));
  l2 = j0;
  i0 = l0;
  p2 = i0;
  i1 = 4294967232u;
  i0 += i1;
  l0 = i0;
  g0 = i0;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
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
  i0 = f191(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 68));
  j1 = 0ull;
  i2 = l0;
  i3 = 64u;
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
  if (i0) {goto B7;}
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l2;
  j3 = 1ull;
  j2 += j3;
  j3 = l2;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&M0), (u64)(i0), j1);
  B7:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f159(u32 p0, u32 p1, u64 p2, u64 p3, u64 p4) {
  u32 l0 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  u64 l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 48u;
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
  l1 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  l2 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  j0 = l1;
  j1 = l2;
  j2 = 15426372072997126144ull;
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
  l3 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B1;}
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
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
  i0 = f160(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = i32_load((&M0), (u64)(i0 + 48));
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = 0u;
  i1 = 9398u;
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
  i0 = p0;
  i1 = 32u;
  i0 += i1;
  l4 = i0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  i1 = p0;
  i2 = 60u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l6 = i1;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  L12: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i1 = 4294967272u;
    i0 += i1;
    l3 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l7 = i0;
    j0 = i64_load((&M0), (u64)(i0));
    j1 = l2;
    i0 = j0 == j1;
    if (i0) {goto B11;}
    i0 = l3;
    l6 = i0;
    i0 = l5;
    i1 = l3;
    i0 = i0 != i1;
    if (i0) {goto L12;}
    goto B10;
  B11:;
  i0 = l5;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  i0 = l7;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = l4;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l7;
  i0 = i32_load8_u((&M0), (u64)(i0 + 60));
  i1 = 7u;
  i0 &= i1;
  if (i0) {goto B8;}
  goto B7;
  B10:;
  i0 = p0;
  i1 = 32u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 40u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 15938991009778630656ull;
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
  l3 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B7;}
  i0 = l4;
  i1 = l3;
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
  i0 = f161(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l7 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = l4;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l7;
  i0 = i32_load8_u((&M0), (u64)(i0 + 60));
  i1 = 7u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B7;}
  B8:;
  i0 = 0u;
  i1 = 9565u;
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
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  j1 = p2;
  j2 = p3;
  j3 = p4;
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
  (*Z_envZ_set_resource_limitsZ_vjjjj)(j0, j1, j2, j3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  l7 = i0;
  i0 = !(i0);
  if (i0) {goto B19;}
  i0 = l0;
  i1 = 36u;
  i0 += i1;
  l5 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i1 = l7;
  i0 = i0 == i1;
  if (i0) {goto B21;}
  L22: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    l3 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l6 = i0;
    i0 = l3;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l6;
    i0 = !(i0);
    if (i0) {goto B23;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B23:;
    i0 = l7;
    i1 = l3;
    i0 = i0 != i1;
    if (i0) {goto L22;}
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  goto B20;
  B21:;
  i0 = l7;
  l3 = i0;
  B20:;
  i0 = l5;
  i1 = l7;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B19:;
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f160(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0;
  u64 l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
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
  i1 = 12917u;
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
  i0 = f97(i0);
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
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l2;
  i2 = l3;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = 64u;
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
  i0 = f103(i0);
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
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = l3;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  i1 = l3;
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
  i0 = f270(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 52), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  j1 = i64_load((&M0), (u64)(i1));
  l5 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 12), i1);
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
  i3 = 16u;
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
  f183(i0, i1, i2, i3);
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
  f98(i0);
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
  f105(i0);
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

static u32 f161(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0;
  u64 l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
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
  i1 = 12917u;
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
  i0 = f97(i0);
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
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l2;
  i2 = l3;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = 104u;
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
  i0 = f103(i0);
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
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l3;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 56), i1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 60), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 80), j1);
  i0 = l3;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 88), i1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  i1 = l3;
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
  i0 = f271(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 92), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  j1 = i64_load((&M0), (u64)(i1));
  l5 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 12), i1);
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
  i3 = 16u;
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
  f165(i0, i1, i2, i3);
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
  f98(i0);
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
  i0 = i32_load((&M0), (u64)(i0 + 16));
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B18;}
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B18:;
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
  f105(i0);
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

static void f162(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l2 = 0, l7 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 96u;
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
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = l0;
  i2 = 64u;
  i1 += i2;
  i2 = l0;
  i3 = 56u;
  i2 += i3;
  i3 = l0;
  i4 = 48u;
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
  (*Z_envZ_get_resource_limitsZ_vjiii)(j0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i0 = i32_load8_u((&M0), (u64)(i0 + 8));
  i0 = !(i0);
  if (i0) {goto B17;}
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  j1 = 18446744073709551615ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  if (i0) {goto B18;}
  i0 = 0u;
  i1 = 9941u;
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
  B18:;
  i0 = p0;
  i1 = 32u;
  i0 += i1;
  l1 = i0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i1 = p0;
  i2 = 60u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l4 = i1;
  i0 = i0 == i1;
  if (i0) {goto B15;}
  L20: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l6 = i0;
    j0 = i64_load((&M0), (u64)(i0));
    j1 = l2;
    i0 = j0 == j1;
    if (i0) {goto B16;}
    i0 = l5;
    l4 = i0;
    i0 = l3;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L20;}
    goto B15;
  B17:;
  i0 = p0;
  i1 = 32u;
  i0 += i1;
  p2 = i0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i1 = p0;
  i2 = 60u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l4 = i1;
  i0 = i0 == i1;
  if (i0) {goto B11;}
  L21: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l6 = i0;
    j0 = i64_load((&M0), (u64)(i0));
    j1 = l2;
    i0 = j0 == j1;
    if (i0) {goto B12;}
    i0 = l5;
    l4 = i0;
    i0 = l3;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L21;}
    goto B11;
  B16:;
  i0 = l3;
  i1 = l4;
  i0 = i0 == i1;
  if (i0) {goto B15;}
  i0 = l6;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 12734u;
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
  goto B14;
  B15:;
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 40u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 15938991009778630656ull;
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
  l5 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B13;}
  i0 = l1;
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
  i0 = f161(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 12734u;
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
  i1 = l6;
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
  f163(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B3;
  B13:;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = l2;
  i64_store((&M0), (u64)(i0 + 88), j1);
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
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B27;}
  i0 = 0u;
  i1 = 13837u;
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
  i0 = l0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = l0;
  i1 = l0;
  i2 = 88u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = 104u;
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l5;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l5;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 56), i1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 60), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 80), j1);
  i0 = l5;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 88), i1);
  i0 = l0;
  i1 = 8u;
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
  f164(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = l5;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l0;
  i1 = l5;
  j1 = i64_load((&M0), (u64)(i1));
  l2 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = l5;
  i1 = i32_load((&M0), (u64)(i1 + 92));
  l6 = i1;
  i32_store((&M0), (u64)(i0 + 76), i1);
  i0 = p0;
  i1 = 60u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l4 = i0;
  i1 = p0;
  i2 = 64u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B7;}
  i0 = l4;
  j1 = l2;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l4;
  i1 = l6;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l4;
  i1 = l5;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l3;
  i1 = l4;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 80));
  l5 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l5;
  i0 = !(i0);
  if (i0) {goto B3;}
  goto B4;
  B12:;
  i0 = l3;
  i1 = l4;
  i0 = i0 == i1;
  if (i0) {goto B11;}
  i0 = l6;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l6;
  i0 = i32_load8_u((&M0), (u64)(i0 + 60));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B8;}
  goto B9;
  B11:;
  i0 = 0u;
  l6 = i0;
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 40u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 15938991009778630656ull;
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
  l5 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B9;}
  i0 = p2;
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
  i0 = f161(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l6;
  i0 = i32_load8_u((&M0), (u64)(i0 + 60));
  i1 = 1u;
  i0 &= i1;
  if (i0) {goto B8;}
  B9:;
  i0 = 0u;
  i1 = 9772u;
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
  l7 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  l2 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  j0 = l7;
  j1 = l2;
  j2 = 15426372072997126144ull;
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
  l5 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B37;}
  i0 = l0;
  i1 = 8u;
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
  i0 = f160(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 48));
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i0 = i0 == i1;
  if (i0) {goto B39;}
  i0 = 0u;
  i1 = 12734u;
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
  B39:;
  i0 = l5;
  j0 = i64_load((&M0), (u64)(i0 + 40));
  j1 = 1400ull;
  j0 += j1;
  l2 = j0;
  i0 = l6;
  if (i0) {goto B5;}
  goto B6;
  B37:;
  j0 = 1400ull;
  l2 = j0;
  i0 = l6;
  if (i0) {goto B5;}
  goto B6;
  B7:;
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  i1 = l0;
  i2 = 80u;
  i1 += i2;
  i2 = l0;
  i3 = 8u;
  i2 += i3;
  i3 = l0;
  i4 = 76u;
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
  f165(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 80));
  l5 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l5;
  if (i0) {goto B4;}
  goto B3;
  B6:;
  i0 = 0u;
  i1 = 13253u;
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
  i0 = p2;
  i1 = l6;
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
  f166(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  l6 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  i0 = l0;
  i1 = 36u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto B46;}
  L47: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l4 = i0;
    i0 = l5;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l4;
    i0 = !(i0);
    if (i0) {goto B48;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B48:;
    i0 = l6;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L47;}
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  goto B45;
  B46:;
  i0 = l6;
  l5 = i0;
  B45:;
  i0 = l3;
  i1 = l6;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B2;
  B4:;
  i0 = l5;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B51;}
  i0 = l5;
  i1 = 20u;
  i0 += i1;
  i1 = l4;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B51:;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B3:;
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  B2:;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  j1 = l2;
  i2 = l0;
  j2 = i64_load((&M0), (u64)(i2 + 56));
  i3 = l0;
  j3 = i64_load((&M0), (u64)(i3 + 48));
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
  (*Z_envZ_set_resource_limitsZ_vjjjj)(j0, j1, j2, j3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 96u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f163(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l7 = 0;
  u64 l5 = 0, l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13402u;
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
  i1 = 13513u;
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
  i0 = p1;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 60));
  i2 = 1u;
  i1 |= i2;
  i32_store((&M0), (u64)(i0 + 60), i1);
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 16));
  l3 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l6 = j0;
  i0 = 16u;
  l7 = i0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l7;
    i1 = 1u;
    i0 += i1;
    l7 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L5;}
  i0 = l3;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  i0 = l4;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = l7;
  i0 += i1;
  l7 = i0;
  B6:;
  i0 = l7;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B8;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l7 = i0;
  goto B7;
  B8:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l7 = i0;
  g0 = i0;
  B7:;
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l7;
  i2 = l2;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
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
  i0 = f272(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 92));
  j1 = 0ull;
  i2 = l7;
  i3 = l2;
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
  i0 = l2;
  i1 = 513u;
  i0 = i0 >= i1;
  if (i0) {goto B14;}
  j0 = l6;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 >= j1;
  if (i0) {goto B13;}
  goto B12;
  B14:;
  i0 = l7;
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
  f98(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = l6;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B12;}
  B13:;
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l6;
  j3 = 1ull;
  j2 += j3;
  j3 = l6;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  goto Bfunc;
  B12:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f164(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0, l7 = 0;
  u64 l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 60));
  i2 = 1u;
  i1 |= i2;
  i32_store((&M0), (u64)(i0 + 60), i1);
  i0 = p1;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  i1 = i32_load((&M0), (u64)(i1));
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 16));
  l3 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l6 = i0;
  i0 = 16u;
  l7 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l7;
    i1 = 1u;
    i0 += i1;
    l7 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L0;}
  i0 = l3;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  i0 = l4;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = l7;
  i0 += i1;
  l7 = i0;
  B1:;
  i0 = l7;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l7 = i0;
  goto B2;
  B3:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l7 = i0;
  g0 = i0;
  B2:;
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l7;
  i2 = l2;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
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
  i0 = f272(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = l6;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 15938991009778630656ull;
  i3 = p0;
  i3 = i32_load((&M0), (u64)(i3 + 8));
  j3 = i64_load((&M0), (u64)(i3));
  i4 = p1;
  j4 = i64_load((&M0), (u64)(i4));
  l5 = j4;
  i5 = l7;
  i6 = l2;
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
  i32_store((&M0), (u64)(i0 + 92), i1);
  i0 = l2;
  i1 = 513u;
  i0 = i0 >= i1;
  if (i0) {goto B9;}
  j0 = l5;
  i1 = l6;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 >= j1;
  if (i0) {goto B8;}
  goto B7;
  B9:;
  i0 = l7;
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
  f98(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = l5;
  i1 = l6;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B7;}
  B8:;
  i0 = l6;
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
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  goto Bfunc;
  B7:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f165(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f103(i0);
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
  f111(i0);
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
  l0 = i0;
  goto B6;
  B7:;
  i0 = l3;
  l0 = i0;
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
  i1 = l0;
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
    i0 = i32_load((&M0), (u64)(i0 + 16));
    p2 = i0;
    i0 = !(i0);
    if (i0) {goto B12;}
    i0 = p1;
    i1 = 20u;
    i0 += i1;
    i1 = p2;
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B12:;
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B11:;
    i0 = l0;
    i1 = l3;
    i0 = i0 != i1;
    if (i0) {goto L10;}
  B9:;
  i0 = l0;
  i0 = !(i0);
  if (i0) {goto B15;}
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B15:;
  FUNC_EPILOGUE;
}

static void f166(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l7 = 0;
  u64 l5 = 0, l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13402u;
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
  i1 = 13513u;
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
  i0 = p1;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 60));
  i2 = 4294967294u;
  i1 &= i2;
  i32_store((&M0), (u64)(i0 + 60), i1);
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 16));
  l3 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l6 = j0;
  i0 = 16u;
  l7 = i0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l7;
    i1 = 1u;
    i0 += i1;
    l7 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L5;}
  i0 = l3;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  i0 = l4;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = l7;
  i0 += i1;
  l7 = i0;
  B6:;
  i0 = l7;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B8;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l7 = i0;
  goto B7;
  B8:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l7 = i0;
  g0 = i0;
  B7:;
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l7;
  i2 = l2;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
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
  i0 = f272(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 92));
  j1 = 0ull;
  i2 = l7;
  i3 = l2;
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
  i0 = l2;
  i1 = 513u;
  i0 = i0 >= i1;
  if (i0) {goto B14;}
  j0 = l6;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 >= j1;
  if (i0) {goto B13;}
  goto B12;
  B14:;
  i0 = l7;
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
  f98(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = l6;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B12;}
  B13:;
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l6;
  j3 = 1ull;
  j2 += j3;
  j3 = l6;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  goto Bfunc;
  B12:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f167(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l2 = 0, l7 = 0, l8 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 96u;
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
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = l0;
  i2 = 64u;
  i1 += i2;
  i2 = l0;
  i3 = 56u;
  i2 += i3;
  i3 = l0;
  i4 = 48u;
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
  (*Z_envZ_get_resource_limitsZ_vjiii)(j0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i0 = i32_load8_u((&M0), (u64)(i0 + 8));
  i0 = !(i0);
  if (i0) {goto B17;}
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  j1 = 18446744073709551614ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  if (i0) {goto B18;}
  i0 = 0u;
  i1 = 10313u;
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
  B18:;
  i0 = p0;
  i1 = 32u;
  i0 += i1;
  l1 = i0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i1 = p0;
  i2 = 60u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l4 = i1;
  i0 = i0 == i1;
  if (i0) {goto B15;}
  L20: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l6 = i0;
    j0 = i64_load((&M0), (u64)(i0));
    j1 = l2;
    i0 = j0 == j1;
    if (i0) {goto B16;}
    i0 = l5;
    l4 = i0;
    i0 = l3;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L20;}
    goto B15;
  B17:;
  i0 = p0;
  i1 = 32u;
  i0 += i1;
  p2 = i0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i1 = p0;
  i2 = 60u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l4 = i1;
  i0 = i0 == i1;
  if (i0) {goto B11;}
  L21: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l6 = i0;
    j0 = i64_load((&M0), (u64)(i0));
    j1 = l2;
    i0 = j0 == j1;
    if (i0) {goto B12;}
    i0 = l5;
    l4 = i0;
    i0 = l3;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L21;}
    goto B11;
  B16:;
  i0 = l3;
  i1 = l4;
  i0 = i0 == i1;
  if (i0) {goto B15;}
  i0 = l6;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 12734u;
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
  goto B14;
  B15:;
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 40u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 15938991009778630656ull;
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
  l5 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B13;}
  i0 = l1;
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
  i0 = f161(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 12734u;
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
  i1 = l6;
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
  f168(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B3;
  B13:;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = l2;
  i64_store((&M0), (u64)(i0 + 88), j1);
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
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B27;}
  i0 = 0u;
  i1 = 13837u;
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
  i0 = l0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = l0;
  i1 = l0;
  i2 = 88u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = 104u;
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l5;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l5;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 56), i1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 60), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 80), j1);
  i0 = l5;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 88), i1);
  i0 = l0;
  i1 = 8u;
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
  f169(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = l5;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l0;
  i1 = l5;
  j1 = i64_load((&M0), (u64)(i1));
  l2 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = l5;
  i1 = i32_load((&M0), (u64)(i1 + 92));
  l6 = i1;
  i32_store((&M0), (u64)(i0 + 76), i1);
  i0 = p0;
  i1 = 60u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l4 = i0;
  i1 = p0;
  i2 = 64u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B7;}
  i0 = l4;
  j1 = l2;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l4;
  i1 = l6;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l4;
  i1 = l5;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l3;
  i1 = l4;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 80));
  l5 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l5;
  i0 = !(i0);
  if (i0) {goto B3;}
  goto B4;
  B12:;
  i0 = l3;
  i1 = l4;
  i0 = i0 == i1;
  if (i0) {goto B11;}
  i0 = l6;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l6;
  i0 = i32_load8_u((&M0), (u64)(i0 + 60));
  i1 = 2u;
  i0 &= i1;
  if (i0) {goto B8;}
  goto B9;
  B11:;
  i0 = 0u;
  l6 = i0;
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 40u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 15938991009778630656ull;
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
  l5 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B9;}
  i0 = p2;
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
  i0 = f161(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l6;
  i0 = i32_load8_u((&M0), (u64)(i0 + 60));
  i1 = 2u;
  i0 &= i1;
  if (i0) {goto B8;}
  B9:;
  i0 = 0u;
  i1 = 10108u;
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
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  j0 = 0ull;
  l2 = j0;
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  l7 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  l8 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  j0 = l7;
  j1 = l8;
  j2 = 15426372072997126144ull;
  j3 = l8;
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
  l5 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B37;}
  i0 = l0;
  i1 = 8u;
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
  i0 = f160(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 48));
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i0 = i0 == i1;
  if (i0) {goto B39;}
  i0 = 0u;
  i1 = 12734u;
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
  B39:;
  i0 = l5;
  j0 = i64_load((&M0), (u64)(i0 + 8));
  l2 = j0;
  B37:;
  i0 = l6;
  if (i0) {goto B42;}
  i0 = 0u;
  i1 = 13253u;
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
  B42:;
  i0 = p2;
  i1 = l6;
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
  f170(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  l6 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  i0 = l0;
  i1 = 36u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  L45: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l4 = i0;
    i0 = l5;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l4;
    i0 = !(i0);
    if (i0) {goto B46;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B46:;
    i0 = l6;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L45;}
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  goto B5;
  B7:;
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  i1 = l0;
  i2 = 80u;
  i1 += i2;
  i2 = l0;
  i3 = 8u;
  i2 += i3;
  i3 = l0;
  i4 = 76u;
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
  f165(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 80));
  l5 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l5;
  if (i0) {goto B4;}
  goto B3;
  B6:;
  i0 = l6;
  l5 = i0;
  B5:;
  i0 = l3;
  i1 = l6;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B2;
  B4:;
  i0 = l5;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B50;}
  i0 = l5;
  i1 = 20u;
  i0 += i1;
  i1 = l4;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B50:;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B3:;
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  B2:;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 64));
  j2 = l2;
  i3 = l0;
  j3 = i64_load((&M0), (u64)(i3 + 48));
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
  (*Z_envZ_set_resource_limitsZ_vjjjj)(j0, j1, j2, j3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 96u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f168(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l7 = 0;
  u64 l5 = 0, l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13402u;
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
  i1 = 13513u;
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
  i0 = p1;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 60));
  i2 = 2u;
  i1 |= i2;
  i32_store((&M0), (u64)(i0 + 60), i1);
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 16));
  l3 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l6 = j0;
  i0 = 16u;
  l7 = i0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l7;
    i1 = 1u;
    i0 += i1;
    l7 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L5;}
  i0 = l3;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  i0 = l4;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = l7;
  i0 += i1;
  l7 = i0;
  B6:;
  i0 = l7;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B8;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l7 = i0;
  goto B7;
  B8:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l7 = i0;
  g0 = i0;
  B7:;
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l7;
  i2 = l2;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
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
  i0 = f272(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 92));
  j1 = 0ull;
  i2 = l7;
  i3 = l2;
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
  i0 = l2;
  i1 = 513u;
  i0 = i0 >= i1;
  if (i0) {goto B14;}
  j0 = l6;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 >= j1;
  if (i0) {goto B13;}
  goto B12;
  B14:;
  i0 = l7;
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
  f98(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = l6;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B12;}
  B13:;
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l6;
  j3 = 1ull;
  j2 += j3;
  j3 = l6;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  goto Bfunc;
  B12:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f169(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0, l7 = 0;
  u64 l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 60));
  i2 = 2u;
  i1 |= i2;
  i32_store((&M0), (u64)(i0 + 60), i1);
  i0 = p1;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  i1 = i32_load((&M0), (u64)(i1));
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 16));
  l3 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l6 = i0;
  i0 = 16u;
  l7 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l7;
    i1 = 1u;
    i0 += i1;
    l7 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L0;}
  i0 = l3;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  i0 = l4;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = l7;
  i0 += i1;
  l7 = i0;
  B1:;
  i0 = l7;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l7 = i0;
  goto B2;
  B3:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l7 = i0;
  g0 = i0;
  B2:;
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l7;
  i2 = l2;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
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
  i0 = f272(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = l6;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 15938991009778630656ull;
  i3 = p0;
  i3 = i32_load((&M0), (u64)(i3 + 8));
  j3 = i64_load((&M0), (u64)(i3));
  i4 = p1;
  j4 = i64_load((&M0), (u64)(i4));
  l5 = j4;
  i5 = l7;
  i6 = l2;
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
  i32_store((&M0), (u64)(i0 + 92), i1);
  i0 = l2;
  i1 = 513u;
  i0 = i0 >= i1;
  if (i0) {goto B9;}
  j0 = l5;
  i1 = l6;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 >= j1;
  if (i0) {goto B8;}
  goto B7;
  B9:;
  i0 = l7;
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
  f98(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = l5;
  i1 = l6;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B7;}
  B8:;
  i0 = l6;
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
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  goto Bfunc;
  B7:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f170(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l7 = 0;
  u64 l5 = 0, l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13402u;
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
  i1 = 13513u;
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
  i0 = p1;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 60));
  i2 = 4294967293u;
  i1 &= i2;
  i32_store((&M0), (u64)(i0 + 60), i1);
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 16));
  l3 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l6 = j0;
  i0 = 16u;
  l7 = i0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l7;
    i1 = 1u;
    i0 += i1;
    l7 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L5;}
  i0 = l3;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  i0 = l4;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = l7;
  i0 += i1;
  l7 = i0;
  B6:;
  i0 = l7;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B8;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l7 = i0;
  goto B7;
  B8:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l7 = i0;
  g0 = i0;
  B7:;
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l7;
  i2 = l2;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
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
  i0 = f272(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 92));
  j1 = 0ull;
  i2 = l7;
  i3 = l2;
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
  i0 = l2;
  i1 = 513u;
  i0 = i0 >= i1;
  if (i0) {goto B14;}
  j0 = l6;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 >= j1;
  if (i0) {goto B13;}
  goto B12;
  B14:;
  i0 = l7;
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
  f98(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = l6;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B12;}
  B13:;
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l6;
  j3 = 1ull;
  j2 += j3;
  j3 = l6;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  goto Bfunc;
  B12:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f171(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l2 = 0, l7 = 0, l8 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 96u;
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
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = l0;
  i2 = 64u;
  i1 += i2;
  i2 = l0;
  i3 = 56u;
  i2 += i3;
  i3 = l0;
  i4 = 48u;
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
  (*Z_envZ_get_resource_limitsZ_vjiii)(j0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i0 = i32_load8_u((&M0), (u64)(i0 + 8));
  i0 = !(i0);
  if (i0) {goto B17;}
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  j1 = 18446744073709551614ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  if (i0) {goto B18;}
  i0 = 0u;
  i1 = 10647u;
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
  B18:;
  i0 = p0;
  i1 = 32u;
  i0 += i1;
  l1 = i0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i1 = p0;
  i2 = 60u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l4 = i1;
  i0 = i0 == i1;
  if (i0) {goto B15;}
  L20: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l6 = i0;
    j0 = i64_load((&M0), (u64)(i0));
    j1 = l2;
    i0 = j0 == j1;
    if (i0) {goto B16;}
    i0 = l5;
    l4 = i0;
    i0 = l3;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L20;}
    goto B15;
  B17:;
  i0 = p0;
  i1 = 32u;
  i0 += i1;
  p2 = i0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i1 = p0;
  i2 = 60u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l4 = i1;
  i0 = i0 == i1;
  if (i0) {goto B11;}
  L21: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l6 = i0;
    j0 = i64_load((&M0), (u64)(i0));
    j1 = l2;
    i0 = j0 == j1;
    if (i0) {goto B12;}
    i0 = l5;
    l4 = i0;
    i0 = l3;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L21;}
    goto B11;
  B16:;
  i0 = l3;
  i1 = l4;
  i0 = i0 == i1;
  if (i0) {goto B15;}
  i0 = l6;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 12734u;
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
  goto B14;
  B15:;
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 40u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 15938991009778630656ull;
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
  l5 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B13;}
  i0 = l1;
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
  i0 = f161(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 12734u;
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
  i1 = l6;
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
  f172(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B3;
  B13:;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = l2;
  i64_store((&M0), (u64)(i0 + 88), j1);
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
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B27;}
  i0 = 0u;
  i1 = 13837u;
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
  i0 = l0;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = l0;
  i1 = l0;
  i2 = 88u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = 104u;
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l5;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l5;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 56), i1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 60), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l5;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 80), j1);
  i0 = l5;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 88), i1);
  i0 = l0;
  i1 = 8u;
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
  f173(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = l5;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l0;
  i1 = l5;
  j1 = i64_load((&M0), (u64)(i1));
  l2 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = l5;
  i1 = i32_load((&M0), (u64)(i1 + 92));
  l6 = i1;
  i32_store((&M0), (u64)(i0 + 76), i1);
  i0 = p0;
  i1 = 60u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l4 = i0;
  i1 = p0;
  i2 = 64u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B7;}
  i0 = l4;
  j1 = l2;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l4;
  i1 = l6;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l4;
  i1 = l5;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l3;
  i1 = l4;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 80));
  l5 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l5;
  i0 = !(i0);
  if (i0) {goto B3;}
  goto B4;
  B12:;
  i0 = l3;
  i1 = l4;
  i0 = i0 == i1;
  if (i0) {goto B11;}
  i0 = l6;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l6;
  i0 = i32_load8_u((&M0), (u64)(i0 + 60));
  i1 = 4u;
  i0 &= i1;
  if (i0) {goto B8;}
  goto B9;
  B11:;
  i0 = 0u;
  l6 = i0;
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 40u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 15938991009778630656ull;
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
  l5 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B9;}
  i0 = p2;
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
  i0 = f161(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l6;
  i0 = i32_load8_u((&M0), (u64)(i0 + 60));
  i1 = 4u;
  i0 &= i1;
  if (i0) {goto B8;}
  B9:;
  i0 = 0u;
  i1 = 10469u;
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
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  j0 = 0ull;
  l2 = j0;
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  l7 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  l8 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  j0 = l7;
  j1 = l8;
  j2 = 15426372072997126144ull;
  j3 = l8;
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
  l5 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B37;}
  i0 = l0;
  i1 = 8u;
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
  i0 = f160(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 48));
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i0 = i0 == i1;
  if (i0) {goto B39;}
  i0 = 0u;
  i1 = 12734u;
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
  B39:;
  i0 = l5;
  j0 = i64_load((&M0), (u64)(i0 + 24));
  l2 = j0;
  B37:;
  i0 = l6;
  if (i0) {goto B42;}
  i0 = 0u;
  i1 = 13253u;
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
  B42:;
  i0 = p2;
  i1 = l6;
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
  f174(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  l6 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  i0 = l0;
  i1 = 36u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  i1 = l6;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  L45: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l4 = i0;
    i0 = l5;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l4;
    i0 = !(i0);
    if (i0) {goto B46;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B46:;
    i0 = l6;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L45;}
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  goto B5;
  B7:;
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  i1 = l0;
  i2 = 80u;
  i1 += i2;
  i2 = l0;
  i3 = 8u;
  i2 += i3;
  i3 = l0;
  i4 = 76u;
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
  f165(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 80));
  l5 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l5;
  if (i0) {goto B4;}
  goto B3;
  B6:;
  i0 = l6;
  l5 = i0;
  B5:;
  i0 = l3;
  i1 = l6;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B2;
  B4:;
  i0 = l5;
  i0 = i32_load((&M0), (u64)(i0 + 16));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B50;}
  i0 = l5;
  i1 = 20u;
  i0 += i1;
  i1 = l4;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B50:;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B3:;
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  B2:;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 64));
  i2 = l0;
  j2 = i64_load((&M0), (u64)(i2 + 56));
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
  (*Z_envZ_set_resource_limitsZ_vjjjj)(j0, j1, j2, j3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 96u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f172(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l7 = 0;
  u64 l5 = 0, l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13402u;
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
  i1 = 13513u;
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
  i0 = p1;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 60));
  i2 = 4u;
  i1 |= i2;
  i32_store((&M0), (u64)(i0 + 60), i1);
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 16));
  l3 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l6 = j0;
  i0 = 16u;
  l7 = i0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l7;
    i1 = 1u;
    i0 += i1;
    l7 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L5;}
  i0 = l3;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  i0 = l4;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = l7;
  i0 += i1;
  l7 = i0;
  B6:;
  i0 = l7;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B8;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l7 = i0;
  goto B7;
  B8:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l7 = i0;
  g0 = i0;
  B7:;
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l7;
  i2 = l2;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
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
  i0 = f272(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 92));
  j1 = 0ull;
  i2 = l7;
  i3 = l2;
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
  i0 = l2;
  i1 = 513u;
  i0 = i0 >= i1;
  if (i0) {goto B14;}
  j0 = l6;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 >= j1;
  if (i0) {goto B13;}
  goto B12;
  B14:;
  i0 = l7;
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
  f98(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = l6;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B12;}
  B13:;
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l6;
  j3 = 1ull;
  j2 += j3;
  j3 = l6;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  goto Bfunc;
  B12:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f173(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0, l7 = 0;
  u64 l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 60));
  i2 = 4u;
  i1 |= i2;
  i32_store((&M0), (u64)(i0 + 60), i1);
  i0 = p1;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  i1 = i32_load((&M0), (u64)(i1));
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 16));
  l3 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l6 = i0;
  i0 = 16u;
  l7 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l7;
    i1 = 1u;
    i0 += i1;
    l7 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L0;}
  i0 = l3;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  i0 = l4;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = l7;
  i0 += i1;
  l7 = i0;
  B1:;
  i0 = l7;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l7 = i0;
  goto B2;
  B3:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l7 = i0;
  g0 = i0;
  B2:;
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l7;
  i2 = l2;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
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
  i0 = f272(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = l6;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 15938991009778630656ull;
  i3 = p0;
  i3 = i32_load((&M0), (u64)(i3 + 8));
  j3 = i64_load((&M0), (u64)(i3));
  i4 = p1;
  j4 = i64_load((&M0), (u64)(i4));
  l5 = j4;
  i5 = l7;
  i6 = l2;
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
  i32_store((&M0), (u64)(i0 + 92), i1);
  i0 = l2;
  i1 = 513u;
  i0 = i0 >= i1;
  if (i0) {goto B9;}
  j0 = l5;
  i1 = l6;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 >= j1;
  if (i0) {goto B8;}
  goto B7;
  B9:;
  i0 = l7;
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
  f98(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = l5;
  i1 = l6;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B7;}
  B8:;
  i0 = l6;
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
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  goto Bfunc;
  B7:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f174(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l7 = 0;
  u64 l5 = 0, l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 88));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13402u;
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
  i1 = 13513u;
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
  i0 = p1;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 60));
  i2 = 4294967291u;
  i1 &= i2;
  i32_store((&M0), (u64)(i0 + 60), i1);
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 16));
  l3 = i1;
  i0 -= i1;
  l4 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l6 = j0;
  i0 = 16u;
  l7 = i0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l7;
    i1 = 1u;
    i0 += i1;
    l7 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L5;}
  i0 = l3;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  i0 = l4;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = l7;
  i0 += i1;
  l7 = i0;
  B6:;
  i0 = l7;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B8;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l7 = i0;
  goto B7;
  B8:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l7 = i0;
  g0 = i0;
  B7:;
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = l7;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l7;
  i2 = l2;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
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
  i0 = f272(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 92));
  j1 = 0ull;
  i2 = l7;
  i3 = l2;
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
  i0 = l2;
  i1 = 513u;
  i0 = i0 >= i1;
  if (i0) {goto B14;}
  j0 = l6;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 >= j1;
  if (i0) {goto B13;}
  goto B12;
  B14:;
  i0 = l7;
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
  f98(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = l6;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B12;}
  B13:;
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l6;
  j3 = 1ull;
  j2 += j3;
  j3 = l6;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  goto Bfunc;
  B12:;
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f175(u32 p0, u32 p1) {
  u32 l0 = 0, l2 = 0, l3 = 0, l4 = 0;
  u64 l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
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
  i0 = p0;
  i1 = 72u;
  i0 += i1;
  l0 = i0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l1 = j0;
  i0 = p0;
  i1 = 96u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = p0;
  i2 = 100u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l3 = i1;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    p1 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l4 = i0;
    j0 = i64_load((&M0), (u64)(i0));
    j1 = l1;
    i0 = j0 == j1;
    if (i0) {goto B4;}
    i0 = p1;
    l3 = i0;
    i0 = l2;
    i1 = p1;
    i0 = i0 != i1;
    if (i0) {goto L5;}
    goto B3;
  B4:;
  i0 = l2;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = l4;
  i1 = 92u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  i0 = 0u;
  i1 = 12734u;
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
  B6:;
  i0 = l4;
  if (i0) {goto B1;}
  goto B2;
  B3:;
  i0 = l0;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 80u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 12531438729690087424ull;
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
  i0 = f176(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 92));
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  i0 = 0u;
  i1 = 12734u;
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
  f177(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto Bfunc;
  B2:;
  i0 = 0u;
  l4 = i0;
  i0 = 0u;
  i1 = 10828u;
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
  i1 = 13253u;
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
  i0 = l0;
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
  f177(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  Bfunc:;
  FUNC_EPILOGUE;
}

static u32 f176(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0;
  u64 l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
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
  i1 = 12917u;
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
  i0 = f97(i0);
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
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l2;
  i2 = l3;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = 104u;
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
  i0 = f103(i0);
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
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l3;
  i1 = 1u;
  i32_store8((&M0), (u64)(i0 + 56), i1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 60), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 68), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 80), j1);
  i0 = l3;
  i1 = 0u;
  i32_store16((&M0), (u64)(i0 + 88), i1);
  i0 = l3;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 92), i1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  i1 = l3;
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
  i0 = f274(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = 4294967295u;
  i32_store((&M0), (u64)(i0 + 100), i1);
  i0 = l3;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 96), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  j1 = i64_load((&M0), (u64)(i1));
  l5 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 12), i1);
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
  i3 = 16u;
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
  f275(i0, i1, i2, i3);
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
  f98(i0);
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
  i0 = i32_load8_u((&M0), (u64)(i0 + 60));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B18;}
  i0 = p1;
  i1 = 68u;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B18:;
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
  f105(i0);
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

static void f177(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l3 = 0, l5 = 0;
  u64 l4 = 0, l6 = 0;
  f64 l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1, j2, j3, j4, j5;
  f64 d1, d2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 64u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i1 = 92u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13402u;
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
  i1 = 13513u;
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
  i0 = l1;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  d1 = f64_load((&M0), (u64)(i1));
  l2 = d1;
  d1 = (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(d1);
  d2 = l2;
  i3 = p1;
  i4 = 56u;
  i3 += i4;
  l3 = i3;
  i3 = i32_load8_u((&M0), (u64)(i3));
  d1 = i3 ? d1 : d2;
  f64_store((&M0), (u64)(i0), d1);
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l4 = j0;
  i0 = l1;
  i1 = 56u;
  i0 += i1;
  l5 = i0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = 48u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 24u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = 28u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = 36u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = 44u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = 52u;
  i0 += i1;
  i1 = l5;
  i1 = i32_load8_u((&M0), (u64)(i1));
  i32_store8((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l3;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 64u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = p1;
  i1 = i32_load8_u((&M0), (u64)(i1 + 60));
  l3 = i1;
  i2 = 1u;
  i1 >>= (i2 & 31);
  i2 = l3;
  i3 = 1u;
  i2 &= i3;
  i0 = i2 ? i0 : i1;
  l5 = i0;
  i1 = 65u;
  i0 += i1;
  l3 = i0;
  i0 = l5;
  j0 = (u64)(i0);
  l6 = j0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    j0 = l6;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L5;}
  i0 = l3;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B6;
  B7:;
  i0 = l0;
  i1 = l3;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  B6:;
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l0;
  i2 = l3;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = 24u;
  i0 += i1;
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
  i0 = f276(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 96));
  j1 = 0ull;
  i2 = l0;
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
  (*Z_envZ_db_update_i64Z_vijii)(i0, j1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B11;}
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
  f98(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  j0 = l4;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B13;}
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l4;
  j3 = 1ull;
  j2 += j3;
  j3 = l4;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&M0), (u64)(i0), j1);
  B13:;
  i0 = l1;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  d1 = f64_load((&M0), (u64)(i1));
  l2 = d1;
  d1 = (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(d1);
  d2 = l2;
  i3 = p1;
  i4 = 56u;
  i3 += i4;
  i3 = i32_load8_u((&M0), (u64)(i3));
  d1 = i3 ? d1 : d2;
  f64_store((&M0), (u64)(i0 + 16), d1);
  i0 = l1;
  i1 = l1;
  i2 = 16u;
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
  i0 = f120(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = !(i0);
  if (i0) {goto B14;}
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 100));
  l3 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B16;}
  i0 = p1;
  i1 = 100u;
  i0 += i1;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  i2 = p0;
  j2 = i64_load((&M0), (u64)(i2 + 8));
  j3 = 12531438729690087424ull;
  i4 = l1;
  i5 = 8u;
  i4 += i5;
  j5 = l4;
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
  i1 = (*Z_envZ_db_idx_double_find_primaryZ_ijjjij)(j1, j2, j3, i4, j5);
  i2 = g3;
  i3 = 1u;
  i2 += i3;
  g3 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i1;
  i32_store((&M0), (u64)(i0), i1);
  B16:;
  i0 = l3;
  j1 = 0ull;
  i2 = l1;
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
  (*Z_envZ_db_idx_double_updateZ_viji)(i0, j1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
  i0 = l1;
  i1 = 64u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f178(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l2 = 0, l4 = 0, l5 = 0;
  u64 l1 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 96u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B0;}
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  l1 = j0;
  j1 = 496ull;
  j0 &= j1;
  i0 = !(j0);
  if (i0) {goto B1;}
  j0 = l1;
  j1 = 15872ull;
  j0 &= j1;
  i0 = !(j0);
  if (i0) {goto B1;}
  j0 = l1;
  j1 = 507904ull;
  j0 &= j1;
  i0 = !(j0);
  if (i0) {goto B1;}
  j0 = l1;
  j1 = 16252928ull;
  j0 &= j1;
  i0 = !(j0);
  if (i0) {goto B1;}
  j0 = l1;
  j1 = 520093696ull;
  j0 &= j1;
  i0 = !(j0);
  if (i0) {goto B1;}
  j0 = l1;
  j1 = 16642998272ull;
  j0 &= j1;
  i0 = !(j0);
  if (i0) {goto B1;}
  j0 = l1;
  j1 = 532575944704ull;
  j0 &= j1;
  i0 = !(j0);
  if (i0) {goto B1;}
  j0 = l1;
  j1 = 17042430230528ull;
  j0 &= j1;
  i0 = !(j0);
  if (i0) {goto B1;}
  j0 = l1;
  j1 = 545357767376896ull;
  j0 &= j1;
  i0 = !(j0);
  if (i0) {goto B1;}
  j0 = l1;
  j1 = 17451448556060672ull;
  j0 &= j1;
  i0 = !(j0);
  if (i0) {goto B1;}
  j0 = l1;
  j1 = 558446353793941504ull;
  j0 &= j1;
  i0 = !(j0);
  if (i0) {goto B1;}
  j0 = l1;
  j1 = 576460752303423487ull;
  i0 = j0 > j1;
  if (i0) {goto B0;}
  B1:;
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
  j0 = f179(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = j0;
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 != j1;
  if (i0) {goto B5;}
  i0 = 0u;
  l2 = i0;
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = l0;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 52), i1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  l3 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l0;
  j1 = l3;
  i64_store((&M0), (u64)(i0 + 24), j1);
  j0 = l3;
  j1 = l3;
  j2 = 11071153799887323136ull;
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
  l4 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 <= (s32)i1);
  if (i0) {goto B4;}
  i0 = l0;
  i1 = 16u;
  i0 += i1;
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
  i0 = f180(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i0 = i0 == i1;
  if (i0) {goto B8;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = l2;
  j0 = i64_load((&M0), (u64)(i0 + 8));
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B2;}
  goto B3;
  B5:;
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  j1 = l1;
  i0 = j0 == j1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 11780u;
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
  goto B0;
  B4:;
  i0 = 0u;
  i1 = 11332u;
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
  j0 = i64_load((&M0), (u64)(i0 + 8));
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B2;}
  B3:;
  i0 = 0u;
  i1 = 11475u;
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
  i0 = l2;
  j0 = i64_load((&M0), (u64)(i0 + 16));
  j1 = 0ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 11626u;
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
  i0 = l2;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13957u;
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
  i1 = 14033u;
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
  B16:;
  i0 = l2;
  i0 = i32_load((&M0), (u64)(i0 + 36));
  i1 = l0;
  i2 = 64u;
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
  if (i0) {goto B19;}
  i0 = l0;
  i1 = 16u;
  i0 += i1;
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
  i0 = f180(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B19:;
  i0 = l0;
  i1 = 16u;
  i0 += i1;
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
  f181(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 40));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = l0;
  i1 = 44u;
  i0 += i1;
  l5 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  p1 = i0;
  i1 = l4;
  i0 = i0 == i1;
  if (i0) {goto B24;}
  L25: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p1;
    i1 = 4294967272u;
    i0 += i1;
    p1 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l2 = i0;
    i0 = p1;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l2;
    i0 = !(i0);
    if (i0) {goto B26;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B26:;
    i0 = l4;
    i1 = p1;
    i0 = i0 != i1;
    if (i0) {goto L25;}
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p1 = i0;
  goto B23;
  B24:;
  i0 = l4;
  p1 = i0;
  B23:;
  i0 = l5;
  i1 = l4;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  l4 = i0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  l1 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l0;
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  l3 = j1;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l0;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  j1 = l3;
  i64_store((&M0), (u64)(i0 + 88), j1);
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
  j1 = (*Z_envZ_current_receiverZ_jv)();
  i2 = g3;
  i3 = 1u;
  i2 += i3;
  g3 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = j0 == j1;
  if (i0) {goto B29;}
  i0 = 0u;
  i1 = 13837u;
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
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 68), i1);
  i0 = l0;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 64), i1);
  i0 = l0;
  i1 = l0;
  i2 = 88u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 72), i1);
  i0 = 64u;
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = p1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = p1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = p1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = p1;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = l0;
  i1 = 64u;
  i0 += i1;
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
  f182(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  l1 = j1;
  i64_store((&M0), (u64)(i0 + 64), j1);
  i0 = l0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 52));
  p0 = i1;
  i32_store((&M0), (u64)(i0 + 60), i1);
  i0 = l0;
  i1 = 44u;
  i0 += i1;
  l5 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = l4;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B36;}
  i0 = l2;
  j1 = l1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l2;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = l2;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l5;
  i1 = l2;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 80));
  p1 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = p1;
  if (i0) {goto B35;}
  goto B34;
  B36:;
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  i1 = l0;
  i2 = 80u;
  i1 += i2;
  i2 = l0;
  i3 = 64u;
  i2 += i3;
  i3 = l0;
  i4 = 60u;
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
  f183(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 80));
  p1 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 80), i1);
  i0 = p1;
  i0 = !(i0);
  if (i0) {goto B34;}
  B35:;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B34:;
  i0 = p2;
  j0 = i64_load((&M0), (u64)(i0));
  j1 = 0ull;
  j2 = 0ull;
  j3 = 0ull;
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
  (*Z_envZ_set_resource_limitsZ_vjjjj)(j0, j1, j2, j3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 40));
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B40;}
  i0 = l0;
  i1 = 44u;
  i0 += i1;
  p0 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  p1 = i0;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B42;}
  L43: 
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
    if (i0) {goto B44;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B44:;
    i0 = l2;
    i1 = p1;
    i0 = i0 != i1;
    if (i0) {goto L43;}
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p1 = i0;
  goto B41;
  B42:;
  i0 = l2;
  p1 = i0;
  B41:;
  i0 = p0;
  i1 = l2;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B40:;
  i0 = l0;
  i1 = 96u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u64 f179(u32 p0) {
  u32 l1 = 0;
  u64 l0 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 59u;
  i2 = 0u;
  i3 = p0;
  j3 = i64_load((&M0), (u64)(i3));
  l0 = j3;
  j4 = 576460752303423488ull;
  i3 = j3 < j4;
  i1 = i3 ? i1 : i2;
  p0 = i1;
  j2 = l0;
  j3 = 558446353793941504ull;
  j2 &= j3;
  i2 = !(j2);
  l1 = i2;
  i0 = i2 ? i0 : i1;
  i1 = 54u;
  i2 = p0;
  i3 = l1;
  i1 = i3 ? i1 : i2;
  p0 = i1;
  j2 = l0;
  j3 = 17451448556060672ull;
  j2 &= j3;
  i2 = !(j2);
  l1 = i2;
  i0 = i2 ? i0 : i1;
  i1 = 49u;
  i2 = p0;
  i3 = l1;
  i1 = i3 ? i1 : i2;
  p0 = i1;
  j2 = l0;
  j3 = 545357767376896ull;
  j2 &= j3;
  i2 = !(j2);
  l1 = i2;
  i0 = i2 ? i0 : i1;
  i1 = 44u;
  i2 = p0;
  i3 = l1;
  i1 = i3 ? i1 : i2;
  p0 = i1;
  j2 = l0;
  j3 = 17042430230528ull;
  j2 &= j3;
  i2 = !(j2);
  l1 = i2;
  i0 = i2 ? i0 : i1;
  i1 = 39u;
  i2 = p0;
  i3 = l1;
  i1 = i3 ? i1 : i2;
  p0 = i1;
  j2 = l0;
  j3 = 532575944704ull;
  j2 &= j3;
  i2 = !(j2);
  l1 = i2;
  i0 = i2 ? i0 : i1;
  i1 = 34u;
  i2 = p0;
  i3 = l1;
  i1 = i3 ? i1 : i2;
  p0 = i1;
  j2 = l0;
  j3 = 16642998272ull;
  j2 &= j3;
  i2 = !(j2);
  l1 = i2;
  i0 = i2 ? i0 : i1;
  i1 = 29u;
  i2 = p0;
  i3 = l1;
  i1 = i3 ? i1 : i2;
  p0 = i1;
  j2 = l0;
  j3 = 520093696ull;
  j2 &= j3;
  i2 = !(j2);
  l1 = i2;
  i0 = i2 ? i0 : i1;
  i1 = 24u;
  i2 = p0;
  i3 = l1;
  i1 = i3 ? i1 : i2;
  p0 = i1;
  j2 = l0;
  j3 = 16252928ull;
  j2 &= j3;
  i2 = !(j2);
  l1 = i2;
  i0 = i2 ? i0 : i1;
  i1 = 19u;
  i2 = p0;
  i3 = l1;
  i1 = i3 ? i1 : i2;
  p0 = i1;
  j2 = l0;
  j3 = 507904ull;
  j2 &= j3;
  i2 = !(j2);
  l1 = i2;
  i0 = i2 ? i0 : i1;
  i1 = 14u;
  i2 = p0;
  i3 = l1;
  i1 = i3 ? i1 : i2;
  p0 = i1;
  j2 = l0;
  j3 = 15872ull;
  j2 &= j3;
  i2 = !(j2);
  l1 = i2;
  i0 = i2 ? i0 : i1;
  i1 = 9u;
  i2 = p0;
  i3 = l1;
  i1 = i3 ? i1 : i2;
  p0 = i1;
  j2 = l0;
  j3 = 496ull;
  j2 &= j3;
  i2 = !(j2);
  l1 = i2;
  i0 = i2 ? i0 : i1;
  i1 = 4u;
  i2 = p0;
  i3 = l1;
  i1 = i3 ? i1 : i2;
  j2 = l0;
  j3 = 15ull;
  j2 &= j3;
  l2 = j2;
  i2 = !(j2);
  i0 = i2 ? i0 : i1;
  p0 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  j0 = 1ull;
  i1 = p0;
  j1 = (u64)(i1);
  j0 <<= (j1 & 63);
  j1 = 18446744073709551600ull;
  j0 += j1;
  j1 = l0;
  j0 &= j1;
  i1 = 64u;
  i2 = p0;
  i1 -= i2;
  j1 = (u64)(i1);
  j0 <<= (j1 & 63);
  j1 = l2;
  i2 = 63u;
  i3 = p0;
  i2 -= i3;
  j2 = (u64)(i2);
  j1 <<= (j2 & 63);
  j0 += j1;
  l0 = j0;
  B0:;
  j0 = l0;
  FUNC_EPILOGUE;
  return j0;
}

static u32 f180(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0;
  u64 l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
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
  i1 = 12917u;
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
  i0 = f97(i0);
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
  i0 = 48u;
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
  i0 = f103(i0);
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
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l3;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = l1;
  i1 = l3;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 44), i1);
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
  f278(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = 4294967295u;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = l3;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  j1 = i64_load((&M0), (u64)(i1));
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
  f279(i0, i1, i2, i3);
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
  f98(i0);
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
  f105(i0);
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

static void f181(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0, l7 = 0, l8 = 0;
  u64 l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j0, j1, j2, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 16u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 14093u;
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
  i1 = 14203u;
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
  l1 = i0;
  i1 = p0;
  i2 = 28u;
  i1 += i2;
  l2 = i1;
  i1 = i32_load((&M0), (u64)(i1));
  l3 = i1;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  i0 = l3;
  l4 = i0;
  i0 = l3;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  l5 = j1;
  i0 = j0 == j1;
  if (i0) {goto B11;}
  i0 = l1;
  i1 = 24u;
  i0 += i1;
  l6 = i0;
  i0 = l3;
  l7 = i0;
  L12: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i1 = l7;
    i0 = i0 == i1;
    if (i0) {goto B10;}
    i0 = l7;
    i1 = 4294967248u;
    i0 += i1;
    l8 = i0;
    i0 = l7;
    i1 = 4294967272u;
    i0 += i1;
    l4 = i0;
    l7 = i0;
    i0 = l8;
    i0 = i32_load((&M0), (u64)(i0));
    j0 = i64_load((&M0), (u64)(i0));
    j1 = l5;
    i0 = j0 != j1;
    if (i0) {goto L12;}
  B11:;
  i0 = l1;
  i1 = l4;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  i0 = 4294967272u;
  l8 = i0;
  i0 = l4;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B8;}
  goto B7;
  B10:;
  i0 = l1;
  l4 = i0;
  B9:;
  i0 = 0u;
  i1 = 14289u;
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
  l8 = i0;
  i0 = l4;
  i1 = l2;
  i1 = i32_load((&M0), (u64)(i1));
  l3 = i1;
  i0 = i0 != i1;
  if (i0) {goto B7;}
  B8:;
  i0 = l4;
  i1 = l8;
  i0 += i1;
  l6 = i0;
  goto B6;
  B7:;
  i0 = l4;
  l7 = i0;
  L14: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l7;
    i0 = i32_load((&M0), (u64)(i0));
    l6 = i0;
    i0 = l7;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l7;
    i1 = l8;
    i0 += i1;
    l1 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l4 = i0;
    i0 = l1;
    i1 = l6;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l4;
    i0 = !(i0);
    if (i0) {goto B15;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B15:;
    i0 = l7;
    i1 = 4294967288u;
    i0 += i1;
    i1 = l7;
    i2 = 16u;
    i1 += i2;
    i1 = i32_load((&M0), (u64)(i1));
    i32_store((&M0), (u64)(i0), i1);
    i0 = l7;
    i1 = 4294967280u;
    i0 += i1;
    i1 = l7;
    i2 = 8u;
    i1 += i2;
    j1 = i64_load((&M0), (u64)(i1));
    i64_store((&M0), (u64)(i0), j1);
    i0 = l3;
    i1 = l7;
    i2 = 24u;
    i1 += i2;
    l7 = i1;
    i0 = i0 != i1;
    if (i0) {goto L14;}
  i0 = l7;
  i1 = 4294967272u;
  i0 += i1;
  l6 = i0;
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l4 = i0;
  i1 = 24u;
  i0 += i1;
  i1 = l7;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  B6:;
  L17: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = l8;
    i0 += i1;
    l4 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l7 = i0;
    i0 = l4;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l7;
    i0 = !(i0);
    if (i0) {goto B18;}
    i0 = l7;
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B18:;
    i0 = l6;
    i1 = l4;
    i0 = i0 != i1;
    if (i0) {goto L17;}
  B5:;
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i1 = l6;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 36));
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
  i0 = p1;
  i1 = 40u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l7 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B22;}
  i0 = p0;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 11071153799887323136ull;
  i3 = l0;
  i4 = 8u;
  i3 += i4;
  i4 = p1;
  j4 = i64_load((&M0), (u64)(i4));
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
  i0 = (*Z_envZ_db_idx64_find_primaryZ_ijjjij)(j0, j1, j2, i3, j4);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l7 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B21;}
  B22:;
  i0 = l7;
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
  (*Z_envZ_db_idx64_removeZ_vi)(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B21:;
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f182(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l5 = 0, l7 = 0, l8 = 0, l9 = 0;
  u64 l3 = 0, l4 = 0, l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 48u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = p1;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  i1 = i32_load((&M0), (u64)(i1));
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  i0 = l0;
  l2 = i0;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l2;
  j1 = 6138663577826885632ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l2;
  j1 = 6138663577826885632ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l2;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l2;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = 0u;
  i0 = i32_load8_u((&M0), (u64)(i0 + 8264));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = 0u;
  j0 = i64_load((&M0), (u64)(i0 + 8256));
  l3 = j0;
  goto B0;
  B1:;
  i0 = l2;
  i1 = 8u;
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
  j0 = f142(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = j0;
  i0 = 0u;
  i1 = 1u;
  i32_store((&M0), (u64)(i0 + 8264), i1);
  i0 = 0u;
  j1 = l3;
  i64_store((&M0), (u64)(i0 + 8256), j1);
  i0 = l2;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  l8 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = l2;
  i1 = 36u;
  i0 += i1;
  l9 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  i1 = l8;
  i0 = i0 == i1;
  if (i0) {goto B4;}
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l7 = i0;
    i0 = l5;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l7;
    i0 = !(i0);
    if (i0) {goto B6;}
    i0 = l7;
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B6:;
    i0 = l8;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L5;}
  i0 = l2;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  goto B3;
  B4:;
  i0 = l8;
  l5 = i0;
  B3:;
  i0 = l9;
  i1 = l8;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  j0 = l3;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l4 = j0;
  i0 = 0u;
  l5 = i0;
  L11: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l4;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B10;}
    j0 = l4;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j0 = l4;
    j1 = 65280ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 == j1;
    if (i0) {goto B12;}
    j0 = l6;
    l4 = j0;
    i0 = l5;
    l7 = i0;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i0 = l7;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L11;}
    goto B9;
    B12:;
    j0 = l6;
    l4 = j0;
    L13: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l4;
      j1 = 65280ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B10;}
      j0 = l4;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l4 = j0;
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
      if (i0) {goto L13;}
    i0 = l8;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i0 = l8;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L11;}
    goto B9;
  B10:;
  i0 = 0u;
  i1 = 14420u;
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
  i0 = p1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  j1 = l3;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l2;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l2;
  j1 = 6138663577826885632ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l2;
  j1 = 6138663577826885632ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l2;
  j1 = 18446744073709551615ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l2;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = 0u;
  i0 = i32_load8_u((&M0), (u64)(i0 + 8264));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B16;}
  i0 = 0u;
  j0 = i64_load((&M0), (u64)(i0 + 8256));
  l3 = j0;
  goto B15;
  B16:;
  i0 = l2;
  i1 = 8u;
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
  j0 = f142(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = j0;
  i0 = 0u;
  i1 = 1u;
  i32_store((&M0), (u64)(i0 + 8264), i1);
  i0 = 0u;
  j1 = l3;
  i64_store((&M0), (u64)(i0 + 8256), j1);
  i0 = l2;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  l8 = i0;
  i0 = !(i0);
  if (i0) {goto B15;}
  i0 = l2;
  i1 = 36u;
  i0 += i1;
  l9 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  i1 = l8;
  i0 = i0 == i1;
  if (i0) {goto B19;}
  L20: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l7 = i0;
    i0 = l5;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l7;
    i0 = !(i0);
    if (i0) {goto B21;}
    i0 = l7;
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B21:;
    i0 = l8;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L20;}
  i0 = l2;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  goto B18;
  B19:;
  i0 = l8;
  l5 = i0;
  B18:;
  i0 = l9;
  i1 = l8;
  i32_store((&M0), (u64)(i0), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B15:;
  j0 = l3;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l4 = j0;
  i0 = 0u;
  l5 = i0;
  L26: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l4;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B25;}
    j0 = l4;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j0 = l4;
    j1 = 65280ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 == j1;
    if (i0) {goto B27;}
    j0 = l6;
    l4 = j0;
    i0 = l5;
    l7 = i0;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i0 = l7;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L26;}
    goto B24;
    B27:;
    j0 = l6;
    l4 = j0;
    L28: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l4;
      j1 = 65280ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B25;}
      j0 = l4;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l4 = j0;
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
      if (i0) {goto L28;}
    i0 = l8;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i0 = l8;
    i1 = 6u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L26;}
    goto B24;
  B25:;
  i0 = 0u;
  i1 = 14420u;
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
  i0 = p1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = p1;
  i1 = 32u;
  i0 += i1;
  j1 = l3;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  l7 = i0;
  i1 = 4294967248u;
  i0 += i1;
  l5 = i0;
  g0 = i0;
  i0 = l2;
  i1 = l5;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = l2;
  i1 = l5;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l2;
  i1 = l7;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l2;
  i1 = 8u;
  i0 += i1;
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
  i0 = f280(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  j2 = 15426372072997126144ull;
  i3 = p0;
  i3 = i32_load((&M0), (u64)(i3 + 8));
  j3 = i64_load((&M0), (u64)(i3));
  i4 = p1;
  j4 = i64_load((&M0), (u64)(i4));
  l4 = j4;
  i5 = l5;
  i6 = 48u;
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
  i32_store((&M0), (u64)(i0 + 52), i1);
  j0 = l4;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B32;}
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l4;
  j3 = 1ull;
  j2 += j3;
  j3 = l4;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&M0), (u64)(i0), j1);
  B32:;
  i0 = l2;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f183(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f103(i0);
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
  f111(i0);
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
    f105(i0);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static void f184(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l3 = 0, l4 = 0, l5 = 0;
  u64 l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 112u;
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
  j1 = l1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  j0 = l1;
  j1 = l1;
  j2 = 3592979018984456192ull;
  i3 = p1;
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
  i0 = f185(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  i0 = i32_load((&M0), (u64)(i0 + 48));
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B4;}
  i0 = 0u;
  i1 = 12734u;
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
  i1 = p0;
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
  f186(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 24));
  p2 = i0;
  if (i0) {goto B1;}
  goto B0;
  B2:;
  i0 = p1;
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
  i1 = 13837u;
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
  i0 = 64u;
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = p0;
  i1 = 40u;
  i0 += i1;
  l3 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 32u;
  i0 += i1;
  l4 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  l5 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = p0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  i1 = p2;
  i1 = i32_load((&M0), (u64)(i1));
  p1 = i1;
  i2 = p2;
  i2 = i32_load((&M0), (u64)(i2 + 4));
  i3 = p1;
  i2 -= i3;
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
  f118(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i2 = 24u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l4;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i2 = 16u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l5;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 56));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 48));
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l0;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i2 = 40u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 104), i1);
  i0 = l0;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 96), i1);
  i0 = l0;
  i1 = 48u;
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
  i0 = l0;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i2 = 8u;
  i1 |= i2;
  i32_store((&M0), (u64)(i0 + 100), i1);
  i0 = l0;
  i1 = 96u;
  i0 += i1;
  i1 = p0;
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
  i0 = f187(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 3592979018984456192ull;
  j3 = l2;
  i4 = p0;
  j4 = i64_load((&M0), (u64)(i4));
  l1 = j4;
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
  p2 = i1;
  i32_store((&M0), (u64)(i0 + 52), i1);
  j0 = l1;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  p1 = i1;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B16;}
  i0 = p1;
  j1 = 18446744073709551614ull;
  j2 = l1;
  j3 = 1ull;
  j2 += j3;
  j3 = l1;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&M0), (u64)(i0), j1);
  B16:;
  i0 = l0;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 96), i1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  l1 = j1;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l0;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 44), i1);
  i0 = l0;
  i1 = 28u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  p1 = i0;
  i1 = l0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B19;}
  i0 = p1;
  j1 = l1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p1;
  i1 = p2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 96), i1);
  i0 = p1;
  i1 = p0;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l3;
  i1 = p1;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 96));
  p0 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 96), i1);
  i0 = p0;
  if (i0) {goto B18;}
  goto B17;
  B19:;
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  i1 = l0;
  i2 = 96u;
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
  f188(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 96));
  p0 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 96), i1);
  i0 = p0;
  i0 = !(i0);
  if (i0) {goto B17;}
  B18:;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B17:;
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 24));
  p2 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  B1:;
  i0 = l0;
  i1 = 28u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  p0 = i0;
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B23;}
  L24: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 4294967272u;
    i0 += i1;
    p0 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    p1 = i0;
    i0 = p0;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = p1;
    i0 = !(i0);
    if (i0) {goto B25;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B25:;
    i0 = p2;
    i1 = p0;
    i0 = i0 != i1;
    if (i0) {goto L24;}
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p0 = i0;
  goto B22;
  B23:;
  i0 = p2;
  p0 = i0;
  B22:;
  i0 = l3;
  i1 = p2;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = l0;
  i1 = 112u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f185(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0, 
      l8 = 0, l11 = 0;
  u64 l9 = 0, l10 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 96u;
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
  i1 = 96u;
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
  l4 = i0;
  g0 = i0;
  i0 = 0u;
  l5 = i0;
  goto B3;
  B5:;
  i0 = 0u;
  i1 = 12917u;
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = 1u;
  l5 = i0;
  B3:;
  i0 = p1;
  i1 = l4;
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
  i0 = 64u;
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l6;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l6;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = l6;
  i1 = 40u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l6;
  i1 = 32u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l6;
  i1 = 24u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l3;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B11;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = l6;
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
  i0 = l4;
  i1 = 8u;
  i0 += i1;
  l2 = i0;
  i0 = l3;
  i1 = 4294967288u;
  i0 += i1;
  i1 = 31u;
  i0 = i0 > i1;
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  l7 = i0;
  i0 = l6;
  i1 = 16u;
  i0 += i1;
  l8 = i0;
  i0 = l1;
  i1 = 64u;
  i0 += i1;
  i1 = l2;
  i2 = 32u;
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
  j0 = 0ull;
  l9 = j0;
  i0 = 16u;
  l3 = i0;
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  l0 = i0;
  i0 = 0u;
  l2 = i0;
  j0 = 0ull;
  l10 = j0;
  L18: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = 64u;
    i0 += i1;
    i1 = l2;
    i0 += i1;
    l11 = i0;
    i0 = l3;
    i1 = 2u;
    i0 = i0 < i1;
    if (i0) {goto B19;}
    j0 = l10;
    j1 = 8ull;
    j0 <<= (j1 & 63);
    j1 = l9;
    i2 = l11;
    j2 = i64_load8_u((&M0), (u64)(i2));
    j1 |= j2;
    l9 = j1;
    j2 = 56ull;
    j1 >>= (j2 & 63);
    j0 |= j1;
    l10 = j0;
    i0 = l3;
    i1 = 4294967295u;
    i0 += i1;
    l3 = i0;
    j0 = l9;
    j1 = 8ull;
    j0 <<= (j1 & 63);
    l9 = j0;
    i0 = l2;
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    i1 = 32u;
    i0 = i0 != i1;
    if (i0) {goto L18;}
    goto B17;
    B19:;
    i0 = l3;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto B20;}
    i0 = 0u;
    i1 = 14645u;
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
    B20:;
    i0 = l0;
    j1 = l10;
    i64_store((&M0), (u64)(i0 + 8), j1);
    i0 = l0;
    j1 = l9;
    i2 = l11;
    j2 = i64_load8_u((&M0), (u64)(i2));
    j1 |= j2;
    i64_store((&M0), (u64)(i0), j1);
    i0 = 16u;
    l3 = i0;
    i0 = l0;
    i1 = 16u;
    i0 += i1;
    l0 = i0;
    j0 = 0ull;
    l9 = j0;
    j0 = 0ull;
    l10 = j0;
    i0 = l2;
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    i1 = 32u;
    i0 = i0 != i1;
    if (i0) {goto L18;}
  B17:;
  i0 = l3;
  i1 = 16u;
  i0 = i0 == i1;
  if (i0) {goto B22;}
  i0 = l3;
  i1 = 2u;
  i0 = i0 < i1;
  if (i0) {goto B23;}
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  j1 = l9;
  j2 = l10;
  i3 = l3;
  i4 = 3u;
  i3 <<= (i4 & 31);
  i4 = 4294967288u;
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
  (*Z_envZ___ashlti3Z_vijji)(i0, j1, j2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  l10 = j0;
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0 + 8));
  l9 = j0;
  B23:;
  i0 = l0;
  j1 = l9;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  j1 = l10;
  i64_store((&M0), (u64)(i0 + 8), j1);
  B22:;
  i0 = l8;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l8;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 40));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l8;
  i1 = 24u;
  i0 += i1;
  i1 = l1;
  i2 = 32u;
  i1 += i2;
  i2 = 24u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l8;
  i1 = 16u;
  i0 += i1;
  i1 = l1;
  i2 = 32u;
  i1 += i2;
  i2 = 16u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l6;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 52), i1);
  i0 = l1;
  i1 = l6;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l6;
  j1 = i64_load((&M0), (u64)(i1));
  l9 = j1;
  i64_store((&M0), (u64)(i0 + 64), j1);
  i0 = l1;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i1 = p0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B27;}
  i0 = l3;
  j1 = l9;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l3;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l3;
  i1 = l6;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l2;
  i1 = l3;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l5;
  if (i0) {goto B26;}
  goto B25;
  B27:;
  i0 = l7;
  i1 = l1;
  i2 = 32u;
  i1 += i2;
  i2 = l1;
  i3 = 64u;
  i2 += i3;
  i3 = l1;
  i4 = 28u;
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
  f188(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i0 = !(i0);
  if (i0) {goto B25;}
  B26:;
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
  f98(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B25:;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  l3 = i0;
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l3;
  i0 = !(i0);
  if (i0) {goto B30;}
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B30:;
  i0 = l1;
  i1 = 96u;
  i0 += i1;
  g0 = i0;
  i0 = l6;
  Bfunc:;
  FUNC_EPILOGUE;
  return i0;
}

static void f186(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l3 = 0;
  u64 l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 32u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 48));
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13402u;
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
  i1 = 13513u;
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
  i0 = p1;
  j0 = i64_load((&M0), (u64)(i0));
  l2 = j0;
  i0 = l1;
  i1 = p2;
  i1 = i32_load((&M0), (u64)(i1));
  p2 = i1;
  i1 = i32_load((&M0), (u64)(i1));
  l3 = i1;
  i2 = p2;
  i2 = i32_load((&M0), (u64)(i2 + 4));
  i3 = l3;
  i2 -= i3;
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
  f118(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 40u;
  i0 += i1;
  i1 = l1;
  i2 = 24u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = 32u;
  i0 += i1;
  i1 = l1;
  i2 = 16u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  j0 = l2;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B6;}
  i0 = 0u;
  i1 = 13642u;
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
  B6:;
  i0 = l0;
  p2 = i0;
  i1 = 4294967248u;
  i0 += i1;
  l0 = i0;
  g0 = i0;
  i0 = l1;
  i1 = p2;
  i2 = 4294967288u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
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
  i2 = 4294967256u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = l3;
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
  i0 = f187(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 52));
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
  if (i0) {goto B11;}
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l2;
  j3 = 1ull;
  j2 += j3;
  j3 = l2;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&M0), (u64)(i0), j1);
  B11:;
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f187(u32 p0, u32 p1) {
  u32 l0 = 0, l3 = 0;
  u64 l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 32u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = l0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  l1 = j1;
  j2 = 32ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 11), j1);
  i0 = l0;
  j1 = l1;
  j2 = 40ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 10), j1);
  i0 = l0;
  j1 = l1;
  j2 = 48ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 9), j1);
  i0 = l0;
  j1 = l1;
  j2 = 56ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  l2 = j1;
  j2 = 32ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 3), j1);
  i0 = l0;
  j1 = l2;
  j2 = 40ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 2), j1);
  i0 = l0;
  j1 = l2;
  j2 = 48ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 1), j1);
  i0 = l0;
  j1 = l2;
  j2 = 56ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0), j1);
  i0 = l0;
  j1 = l1;
  i1 = (u32)(j1);
  l3 = i1;
  i32_store8((&M0), (u64)(i0 + 15), i1);
  i0 = l0;
  i1 = l3;
  i2 = 8u;
  i1 >>= (i2 & 31);
  i32_store8((&M0), (u64)(i0 + 14), i1);
  i0 = l0;
  i1 = l3;
  i2 = 16u;
  i1 >>= (i2 & 31);
  i32_store8((&M0), (u64)(i0 + 13), i1);
  i0 = l0;
  i1 = l3;
  i2 = 24u;
  i1 >>= (i2 & 31);
  i32_store8((&M0), (u64)(i0 + 12), i1);
  i0 = l0;
  j1 = l2;
  i1 = (u32)(j1);
  l3 = i1;
  i32_store8((&M0), (u64)(i0 + 7), i1);
  i0 = l0;
  i1 = l3;
  i2 = 8u;
  i1 >>= (i2 & 31);
  i32_store8((&M0), (u64)(i0 + 6), i1);
  i0 = l0;
  i1 = l3;
  i2 = 16u;
  i1 >>= (i2 & 31);
  i32_store8((&M0), (u64)(i0 + 5), i1);
  i0 = l0;
  i1 = l3;
  i2 = 24u;
  i1 >>= (i2 & 31);
  i32_store8((&M0), (u64)(i0 + 4), i1);
  i0 = l0;
  i1 = p1;
  i2 = 24u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  l1 = j1;
  i64_store8((&M0), (u64)(i0 + 23), j1);
  i0 = l0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  l2 = j1;
  j2 = 32ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 27), j1);
  i0 = l0;
  j1 = l2;
  j2 = 40ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 26), j1);
  i0 = l0;
  j1 = l2;
  j2 = 48ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 25), j1);
  i0 = l0;
  j1 = l2;
  j2 = 56ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 24), j1);
  i0 = l0;
  j1 = l1;
  j2 = 32ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 19), j1);
  i0 = l0;
  j1 = l1;
  j2 = 40ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 18), j1);
  i0 = l0;
  j1 = l1;
  j2 = 48ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 17), j1);
  i0 = l0;
  j1 = l1;
  j2 = 56ull;
  j1 >>= (j2 & 63);
  i64_store8((&M0), (u64)(i0 + 16), j1);
  i0 = l0;
  j1 = l1;
  i1 = (u32)(j1);
  p1 = i1;
  i2 = 8u;
  i1 >>= (i2 & 31);
  i32_store8((&M0), (u64)(i0 + 22), i1);
  i0 = l0;
  i1 = p1;
  i2 = 16u;
  i1 >>= (i2 & 31);
  i32_store8((&M0), (u64)(i0 + 21), i1);
  i0 = l0;
  i1 = p1;
  i2 = 24u;
  i1 >>= (i2 & 31);
  i32_store8((&M0), (u64)(i0 + 20), i1);
  i0 = l0;
  j1 = l2;
  i1 = (u32)(j1);
  p1 = i1;
  i32_store8((&M0), (u64)(i0 + 31), i1);
  i0 = l0;
  i1 = p1;
  i2 = 8u;
  i1 >>= (i2 & 31);
  i32_store8((&M0), (u64)(i0 + 30), i1);
  i0 = l0;
  i1 = p1;
  i2 = 16u;
  i1 >>= (i2 & 31);
  i32_store8((&M0), (u64)(i0 + 29), i1);
  i0 = l0;
  i1 = p1;
  i2 = 24u;
  i1 >>= (i2 & 31);
  i32_store8((&M0), (u64)(i0 + 28), i1);
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  p1 = i1;
  i0 -= i1;
  i1 = 31u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13772u;
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
  p1 = i0;
  B0:;
  i0 = p1;
  i1 = l0;
  i2 = 32u;
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
  p1 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 32u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  g0 = i0;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f188(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f103(i0);
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
  f111(i0);
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
    f105(i0);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static void f189(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  u64 l5 = 0, l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 112u;
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
  i0 = p1;
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = 0u;
  i1 = 11935u;
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
  i0 = p0;
  i1 = 496u;
  i0 += i1;
  l1 = i0;
  i0 = p0;
  i1 = 520u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  i1 = p0;
  i2 = 524u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l3 = i1;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  L7: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    p1 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    l4 = i0;
    j0 = i64_load((&M0), (u64)(i0 + 8));
    j1 = 4995142087184830980ull;
    i0 = j0 == j1;
    if (i0) {goto B6;}
    i0 = p1;
    l3 = i0;
    i0 = l2;
    i1 = p1;
    i0 = i0 != i1;
    if (i0) {goto L7;}
    goto B5;
  B6:;
  i0 = l2;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  i0 = l4;
  i0 = i32_load((&M0), (u64)(i0 + 64));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B4;}
  i0 = 0u;
  i1 = 12734u;
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
  goto B4;
  B5:;
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0));
  i1 = p0;
  i2 = 504u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 13377137154988703744ull;
  j3 = 4995142087184830980ull;
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
  if (i0) {goto B3;}
  i0 = l1;
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
  i0 = f143(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = i32_load((&M0), (u64)(i0 + 64));
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B4;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = 0u;
  i1 = 12033u;
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
  i0 = l0;
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 8ull;
  j1 >>= (j2 & 63);
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  i1 = 12216u;
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
  f190(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  j0 = i64_load((&M0), (u64)(i0 + 32));
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 12224u;
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
  j0 = i64_load((&M0), (u64)(i0 + 24));
  j1 = 0ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 12386u;
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
  B16:;
  i0 = p0;
  j0 = i64_load((&M0), (u64)(i0));
  l5 = j0;
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
  i2 = 496u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 == j1;
  if (i0) {goto B18;}
  i0 = 0u;
  i1 = 13837u;
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
  B18:;
  i0 = 80u;
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  j1 = 4602678819172646912ull;
  i64_store((&M0), (u64)(i0 + 56), j1);
  i0 = p1;
  j1 = 4602678819172646912ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = p1;
  i1 = l1;
  i32_store((&M0), (u64)(i0 + 64), i1);
  i0 = p1;
  j1 = 4995142087184830980ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p1;
  j1 = 100000000000000ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  j1 = 1296126464ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = p1;
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = p1;
  i1 = p0;
  i2 = 344u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i2 = p0;
  i3 = 352u;
  i2 += i3;
  j2 = i64_load((&M0), (u64)(i2));
  j1 -= j2;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = p1;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  j2 = 1000ull;
  j1 = I64_DIV_S(j1, j2);
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = l0;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i2 = 64u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 12), i1);
  i0 = l0;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  i1 = 8u;
  i0 += i1;
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
  i0 = f191(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = p0;
  i2 = 504u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  j2 = 13377137154988703744ull;
  j3 = l5;
  i4 = p1;
  j4 = i64_load((&M0), (u64)(i4 + 8));
  l6 = j4;
  i5 = l0;
  i6 = 48u;
  i5 += i6;
  i6 = 64u;
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
  l2 = i1;
  i32_store((&M0), (u64)(i0 + 68), i1);
  j0 = l6;
  i1 = p0;
  i2 = 512u;
  i1 += i2;
  l3 = i1;
  j1 = i64_load((&M0), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B24;}
  i0 = l3;
  j1 = 18446744073709551614ull;
  j2 = l6;
  j3 = 1ull;
  j2 += j3;
  j3 = l6;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&M0), (u64)(i0), j1);
  B24:;
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l0;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  l6 = j1;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l0;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 44), i1);
  i0 = p0;
  i1 = 524u;
  i0 += i1;
  l4 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i1 = p0;
  i2 = 528u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B27;}
  i0 = l3;
  j1 = l6;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l3;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = l3;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l4;
  i1 = l3;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  p1 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = p1;
  if (i0) {goto B26;}
  goto B25;
  B27:;
  i0 = p0;
  i1 = 520u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
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
  f192(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  p1 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 8), i1);
  i0 = p1;
  i0 = !(i0);
  if (i0) {goto B25;}
  B26:;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B25:;
  i0 = l0;
  j1 = 3617214756542218240ull;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = 16u;
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i1 = 8u;
  i0 += i1;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p1;
  i1 = l0;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  i1 = 60u;
  i0 += i1;
  l2 = i0;
  i1 = p1;
  i2 = 16u;
  i1 += i2;
  l3 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  i1 = 16u;
  i0 += i1;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 6138663591592764928ull;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 56), i1);
  i0 = l0;
  i1 = p0;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  i1 = 12496u;
  i2 = p2;
  i3 = l0;
  i4 = 8u;
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
  f193(i0, i1, i2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 56));
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B32;}
  i0 = l2;
  i1 = p1;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B32:;
  i0 = l0;
  i1 = 112u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f190(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l3 = 0;
  u64 l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 48u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
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
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  l1 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  l2 = j1;
  i64_store((&M0), (u64)(i0 + 16), j1);
  j0 = l1;
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
  p2 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B3;}
  i0 = l0;
  i1 = 8u;
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
  i0 = f194(i0, i1);
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
  if (i0) {goto B5;}
  i0 = 0u;
  i1 = 12734u;
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
  i0 = p0;
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = p2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  i0 = l0;
  i1 = 36u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  p0 = i0;
  i1 = p1;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  L8: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 4294967272u;
    i0 += i1;
    p0 = i0;
    i0 = i32_load((&M0), (u64)(i0));
    p2 = i0;
    i0 = p0;
    i1 = 0u;
    i32_store((&M0), (u64)(i0), i1);
    i0 = p2;
    i0 = !(i0);
    if (i0) {goto B9;}
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
    f105(i0);
    i0 = g3;
    i1 = 1u;
    i0 += i1;
    g3 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B9:;
    i0 = p1;
    i1 = p0;
    i0 = i0 != i1;
    if (i0) {goto L8;}
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p0 = i0;
  goto B0;
  B3:;
  i0 = 0u;
  i1 = 12600u;
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
  i1 = 8u;
  i0 += i1;
  i1 = 0u;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  i1 = 0u;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  B2:;
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  goto Bfunc;
  B1:;
  i0 = p1;
  p0 = i0;
  B0:;
  i0 = l3;
  i1 = p1;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  g0 = i0;
  Bfunc:;
  FUNC_EPILOGUE;
}

static u32 f191(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
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
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l1 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13772u;
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
  l1 = i0;
  B0:;
  i0 = l1;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l2 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l2;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 13772u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  B3:;
  i0 = l2;
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
  i0 = l1;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = p1;
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
  i0 = f269(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = p1;
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
  i0 = f269(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f192(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f103(i0);
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
  f111(i0);
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
    f105(i0);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static void f193(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0;
  u64 l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 80u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = p0;
  i2 = p1;
  i3 = p2;
  i4 = p3;
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
  f195(i0, i1, i2, i3, i4);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 56), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 48), j1);
  i0 = l0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p1 = i0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1 + 24));
  p2 = i1;
  i0 -= i1;
  p3 = i0;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l1 = j0;
  i0 = 16u;
  p0 = i0;
  L1: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 1u;
    i0 += i1;
    p0 = i0;
    j0 = l1;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l1 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L1;}
  i0 = p2;
  i1 = p1;
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = p3;
  i1 = 4294967280u;
  i0 &= i1;
  i1 = p0;
  i0 += i1;
  p0 = i0;
  B2:;
  i0 = p0;
  i1 = l0;
  i2 = 40u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  p1 = i1;
  i0 += i1;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1 + 36));
  p2 = i1;
  i0 -= i1;
  p0 = i0;
  i0 = p1;
  i1 = p2;
  i0 -= i1;
  j0 = (u64)(i0);
  l1 = j0;
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 1u;
    i0 += i1;
    p0 = i0;
    j0 = l1;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l1 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L3;}
  i0 = p0;
  i0 = !(i0);
  if (i0) {goto B5;}
  i0 = l0;
  i1 = 48u;
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
  f196(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 52));
  p1 = i0;
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 48));
  p0 = i0;
  goto B4;
  B5:;
  i0 = 0u;
  p1 = i0;
  i0 = 0u;
  p0 = i0;
  B4:;
  i0 = l0;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 68), i1);
  i0 = l0;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 64), i1);
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 72), i1);
  i0 = l0;
  i1 = 64u;
  i0 += i1;
  i1 = l0;
  i2 = 8u;
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
  i0 = f197(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 48));
  p0 = i0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1 + 52));
  i2 = p0;
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
  (*Z_envZ_send_inlineZ_vii)(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 48));
  p0 = i0;
  i0 = !(i0);
  if (i0) {goto B9;}
  i0 = l0;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 52), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B9:;
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 36));
  p0 = i0;
  i0 = !(i0);
  if (i0) {goto B11;}
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  i1 = p0;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 24));
  p0 = i0;
  i0 = !(i0);
  if (i0) {goto B13;}
  i0 = l0;
  i1 = 28u;
  i0 += i1;
  i1 = p0;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  i0 = l0;
  i1 = 80u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f194(u32 p0, u32 p1) {
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
  i1 = 12917u;
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
  i0 = f97(i0);
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
  i0 = f103(i0);
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
  f257(i0, i1);
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
  f258(i0, i1, i2, i3);
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
  f98(i0);
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
  f105(i0);
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

static void f195(u32 p0, u32 p1, u32 p2, u32 p3, u32 p4) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 80u;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  i0 = l0;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = 0u;
  l1 = i0;
  i0 = 0u;
  l2 = i0;
  i0 = 0u;
  l3 = i0;
  i0 = p1;
  i1 = 12u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1 + 8));
  i0 -= i1;
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = l4;
  i1 = 4u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  l1 = i0;
  i1 = 268435456u;
  i0 = i0 >= i1;
  if (i0) {goto B0;}
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
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
  i1 = f103(i1);
  i2 = g3;
  i3 = 1u;
  i2 += i3;
  g3 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i1;
  i2 = l1;
  i3 = 4u;
  i2 <<= (i3 & 31);
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l0;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = p1;
  i1 = 12u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l4 = i1;
  i0 -= i1;
  l2 = i0;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B3;}
  i0 = l3;
  i1 = l4;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = l3;
  i2 = l2;
  i1 += i2;
  l2 = i1;
  i32_store((&M0), (u64)(i0 + 36), i1);
  goto B1;
  B3:;
  i0 = l3;
  l2 = i0;
  B1:;
  i0 = p0;
  i1 = p1;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = p0;
  j1 = 11913481165836648448ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p0;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = p0;
  i1 = 20u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  i1 = l1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 28), j1);
  i0 = p0;
  i1 = 36u;
  i0 += i1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = p2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = p3;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l0;
  i1 = p4;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  i1 = 24u;
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
  f196(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 32u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  p1 = i0;
  i0 = l0;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 28));
  p0 = i1;
  i32_store((&M0), (u64)(i0 + 52), i1);
  i0 = l0;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 48), i1);
  i0 = l0;
  i1 = p1;
  i32_store((&M0), (u64)(i0 + 56), i1);
  i0 = l0;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 64), i1);
  i0 = l0;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 72), i1);
  i0 = l0;
  i1 = 72u;
  i0 += i1;
  i1 = l0;
  i2 = 64u;
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
  f281(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&M0), (u64)(i0 + 32));
  p0 = i0;
  i0 = !(i0);
  if (i0) {goto B7;}
  i0 = l0;
  i1 = p0;
  i32_store((&M0), (u64)(i0 + 36), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B7:;
  i0 = l0;
  i1 = 80u;
  i0 += i1;
  g0 = i0;
  goto Bfunc;
  B0:;
  i0 = l0;
  i1 = 32u;
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
  f111(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f196(u32 p0, u32 p1) {
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
  i0 = f103(i0);
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
  f111(i0);
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
  f105(i0);
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

static u32 f197(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l0 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13772u;
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
  l0 = i0;
  B0:;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l0 = i0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 13772u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  B3:;
  i0 = l1;
  i1 = l2;
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
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = p1;
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
  i0 = f282(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = p1;
  i2 = 28u;
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
  i0 = f283(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
  return i0;
}

static void f198(u64 p0, u64 p1) {
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
  i0 = f97(i0);
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
  i1 = 13028u;
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
  i1 = 13028u;
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
  i1 = 32u;
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
  f178(i0, i1, i2);
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

static void f199(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  l0 = i0;
  l1 = i0;
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
  if (i0) {goto B0;}
  i0 = l2;
  i1 = 512u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B2;
  B3:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  B2:;
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
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f200(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  l0 = i0;
  l1 = i0;
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
  if (i0) {goto B0;}
  i0 = l2;
  i1 = 512u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B2;
  B3:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  B2:;
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
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f201(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  l0 = i0;
  l1 = i0;
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
  if (i0) {goto B0;}
  i0 = l2;
  i1 = 512u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B2;
  B3:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  B2:;
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
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f202(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  l0 = i0;
  l1 = i0;
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
  if (i0) {goto B0;}
  i0 = l2;
  i1 = 512u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B2;
  B3:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  B2:;
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
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f203(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  l0 = i0;
  l1 = i0;
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
  if (i0) {goto B0;}
  i0 = l2;
  i1 = 512u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B2;
  B3:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  B2:;
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
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f204(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 32u;
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
  i0 = f97(i0);
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
  i1 = 20u;
  i0 += i1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = 24u;
  i0 += i1;
  i1 = l0;
  i2 = l2;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  j1 = p1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
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
  f349(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f205(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
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
  i0 = f97(i0);
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
  i32_store((&M0), (u64)(i0 + 88), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 72), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  B7:;
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
  i32_store((&M0), (u64)(i0 + 84), i1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 64), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 56), j1);
  i0 = l1;
  i1 = 80u;
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
  i0 = f206(i0, i1);
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
  i2 = 80u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 80));
  i64_store((&M0), (u64)(i0 + 8), j1);
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
  j1 = i64_load((&M0), (u64)(i1 + 8));
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
  j1 = i64_load((&M0), (u64)(i1 + 96));
  p0 = j1;
  i64_store((&M0), (u64)(i0 + 40), j1);
  i0 = l1;
  j1 = p0;
  i64_store((&M0), (u64)(i0 + 112), j1);
  i0 = l1;
  i1 = 24u;
  i0 += i1;
  i1 = l1;
  i2 = 72u;
  i1 += i2;
  i2 = l1;
  i3 = 56u;
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
  f184(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 56));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B12;}
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 60), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B12:;
  i0 = l1;
  i1 = 128u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f206(u32 p0, u32 p1) {
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
    i1 = 14514u;
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
  f196(i0, i1);
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
  i1 = 13028u;
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

static void f207(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
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
  i0 = f97(i0);
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
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l0;
  i2 = l2;
  i1 += i2;
  l3 = i1;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 40u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 36));
  l0 = i0;
  B7:;
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
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l3;
  i1 = l0;
  i0 = i0 != i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 32u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 36));
  l0 = i0;
  B10:;
  i0 = l1;
  i1 = 23u;
  i0 += i1;
  i1 = l0;
  i2 = 1u;
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
  i2 = 1u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l3;
  i1 = l0;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0 + 36));
  l0 = i0;
  B13:;
  i0 = l1;
  i1 = 22u;
  i0 += i1;
  i1 = l0;
  i2 = 1u;
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
  i2 = 1u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  i1 = l1;
  i2 = 8u;
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
  i0 = f206(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B17;}
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 12), i1);
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
  f105(i0);
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
  FUNC_EPILOGUE;
}

static void f208(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0;
  u64 l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
  i0 -= i1;
  l0 = i0;
  l1 = i0;
  i0 = l0;
  g0 = i0;
  i0 = 0u;
  l2 = i0;
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
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = l3;
  i1 = 512u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  goto B2;
  B3:;
  i0 = l0;
  i1 = l3;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l2 = i0;
  g0 = i0;
  B2:;
  i0 = l2;
  i1 = l3;
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
  i0 = l2;
  i1 = l3;
  i0 += i1;
  l4 = i0;
  j0 = 0ull;
  l5 = j0;
  i0 = l2;
  l3 = i0;
  i0 = 0u;
  l0 = i0;
  L6: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = l4;
    i0 = i0 < i1;
    if (i0) {goto B7;}
    i0 = 0u;
    i1 = 14514u;
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
    j0 = l5;
    i1 = l3;
    i1 = i32_load8_u((&M0), (u64)(i1));
    l6 = i1;
    i2 = 127u;
    i1 &= i2;
    i2 = l0;
    i3 = 255u;
    i2 &= i3;
    l0 = i2;
    i1 <<= (i2 & 31);
    j1 = (u64)(i1);
    j0 |= j1;
    l5 = j0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i0 = l0;
    i1 = 7u;
    i0 += i1;
    l0 = i0;
    i0 = l6;
    i1 = 128u;
    i0 &= i1;
    if (i0) {goto L6;}
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 40), j1);
  j0 = l5;
  i0 = (u32)(j0);
  l0 = i0;
  i0 = l4;
  i1 = l3;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 40));
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l3;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l4;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i1 = l0;
  i2 = l1;
  i3 = 744u;
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
  f189(i0, i1, i2);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f209(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 736u;
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
  i0 = f97(i0);
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
  i1 = 8u;
  i0 += i1;
  i1 = l0;
  i2 = l2;
  i1 += i2;
  l2 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
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
  f350(i0);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 736u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f210(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 760u;
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
  i1 = 13028u;
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
  i1 = 752u;
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
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = l2;
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = 744u;
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
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 24u;
  i0 = i0 != i1;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13028u;
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
  B16:;
  i0 = l1;
  i1 = 736u;
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
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 32u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  j2 = i64_load((&M0), (u64)(i2 + 752));
  i3 = l1;
  j3 = i64_load((&M0), (u64)(i3 + 744));
  i4 = l1;
  j4 = i64_load((&M0), (u64)(i4 + 736));
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
  f159(i0, i1, j2, j3, j4);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f211(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 752u;
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
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 744), i1);
  i0 = l1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 736), i1);
  i0 = l1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 767), i1);
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 8u;
  i0 = i0 != i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 13028u;
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
  l4 = i0;
  i0 = l1;
  i1 = 767u;
  i0 += i1;
  i1 = l3;
  i2 = 1u;
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
  i1 = 9u;
  i0 += i1;
  l3 = i0;
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 767));
  i0 = !(i0);
  if (i0) {goto B13;}
  i0 = l2;
  i1 = 4294967287u;
  i0 += i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = 1u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l0;
  i1 = 17u;
  i0 += i1;
  l3 = i0;
  B13:;
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l4;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 752u;
  i1 += i2;
  i2 = l1;
  i3 = 736u;
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
  f162(i0, i1, i2);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f212(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 752u;
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
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 744), i1);
  i0 = l1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 736), i1);
  i0 = l1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 767), i1);
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 8u;
  i0 = i0 != i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 13028u;
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
  l4 = i0;
  i0 = l1;
  i1 = 767u;
  i0 += i1;
  i1 = l3;
  i2 = 1u;
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
  i1 = 9u;
  i0 += i1;
  l3 = i0;
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 767));
  i0 = !(i0);
  if (i0) {goto B13;}
  i0 = l2;
  i1 = 4294967287u;
  i0 += i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = 1u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l0;
  i1 = 17u;
  i0 += i1;
  l3 = i0;
  B13:;
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l4;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 752u;
  i1 += i2;
  i2 = l1;
  i3 = 736u;
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
  f167(i0, i1, i2);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f213(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 752u;
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
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 744), i1);
  i0 = l1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 736), i1);
  i0 = l1;
  i1 = 0u;
  i32_store8((&M0), (u64)(i0 + 767), i1);
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 8u;
  i0 = i0 != i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 13028u;
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
  l4 = i0;
  i0 = l1;
  i1 = 767u;
  i0 += i1;
  i1 = l3;
  i2 = 1u;
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
  i1 = 9u;
  i0 += i1;
  l3 = i0;
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 767));
  i0 = !(i0);
  if (i0) {goto B13;}
  i0 = l2;
  i1 = 4294967287u;
  i0 += i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B14;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = 1u;
  i32_store8((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l0;
  i1 = 17u;
  i0 += i1;
  l3 = i0;
  B13:;
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l4;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 752u;
  i1 += i2;
  i2 = l1;
  i3 = 736u;
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
  f171(i0, i1, i2);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f214(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l5 = 0, l6 = 0, l7 = 0, l9 = 0;
  u64 l4 = 0, l8 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 816u;
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  goto B1;
  B3:;
  i0 = 0u;
  l3 = i0;
  goto B0;
  B2:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l3 = i0;
  g0 = i0;
  B1:;
  i0 = l3;
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
  j0 = 0ull;
  l4 = j0;
  i0 = l1;
  i1 = 776u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = 16u;
  l0 = i0;
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  i1 = 16u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l2;
  i1 = 31u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = l3;
  i1 = l2;
  i0 += i1;
  l5 = i0;
  i0 = l1;
  i1 = 48u;
  i0 += i1;
  i1 = l3;
  i2 = 32u;
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
  i1 = 32u;
  i0 += i1;
  l6 = i0;
  i0 = l1;
  i1 = 784u;
  i0 += i1;
  l7 = i0;
  i0 = 0u;
  l2 = i0;
  j0 = 0ull;
  l8 = j0;
  L11: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = 48u;
    i0 += i1;
    i1 = l2;
    i0 += i1;
    l9 = i0;
    i0 = l0;
    i1 = 2u;
    i0 = i0 < i1;
    if (i0) {goto B12;}
    j0 = l8;
    j1 = 8ull;
    j0 <<= (j1 & 63);
    j1 = l4;
    i2 = l9;
    j2 = i64_load8_u((&M0), (u64)(i2));
    j1 |= j2;
    l4 = j1;
    j2 = 56ull;
    j1 >>= (j2 & 63);
    j0 |= j1;
    l8 = j0;
    i0 = l0;
    i1 = 4294967295u;
    i0 += i1;
    l0 = i0;
    j0 = l4;
    j1 = 8ull;
    j0 <<= (j1 & 63);
    l4 = j0;
    i0 = l2;
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    i1 = 32u;
    i0 = i0 != i1;
    if (i0) {goto L11;}
    goto B10;
    B12:;
    i0 = l0;
    i1 = 1u;
    i0 = i0 == i1;
    if (i0) {goto B13;}
    i0 = 0u;
    i1 = 14645u;
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
    i0 = l7;
    j1 = l8;
    i64_store((&M0), (u64)(i0 + 8), j1);
    i0 = l7;
    j1 = l4;
    i2 = l9;
    j2 = i64_load8_u((&M0), (u64)(i2));
    j1 |= j2;
    i64_store((&M0), (u64)(i0), j1);
    i0 = 16u;
    l0 = i0;
    i0 = l7;
    i1 = 16u;
    i0 += i1;
    l7 = i0;
    j0 = 0ull;
    l4 = j0;
    j0 = 0ull;
    l8 = j0;
    i0 = l2;
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    i1 = 32u;
    i0 = i0 != i1;
    if (i0) {goto L11;}
  B10:;
  i0 = l0;
  i1 = 16u;
  i0 = i0 == i1;
  if (i0) {goto B15;}
  i0 = l0;
  i1 = 2u;
  i0 = i0 < i1;
  if (i0) {goto B16;}
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  j1 = l4;
  j2 = l8;
  i3 = l0;
  i4 = 3u;
  i3 <<= (i4 & 31);
  i4 = 4294967288u;
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
  (*Z_envZ___ashlti3Z_vijji)(i0, j1, j2, i3);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 24u;
  i0 += i1;
  j0 = i64_load((&M0), (u64)(i0));
  l8 = j0;
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0 + 16));
  l4 = j0;
  B16:;
  i0 = l7;
  j1 = l4;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l7;
  j1 = l8;
  i64_store((&M0), (u64)(i0 + 8), j1);
  B15:;
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  i1 = 24u;
  i0 += i1;
  i1 = l1;
  i2 = 784u;
  i1 += i2;
  i2 = 24u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  i1 = 16u;
  i0 += i1;
  i1 = l1;
  i2 = 784u;
  i1 += i2;
  i2 = 16u;
  i1 += i2;
  j1 = i64_load((&M0), (u64)(i1));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 792));
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 784));
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l5;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l6;
  i32_store((&M0), (u64)(i0 + 36), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l5;
  i32_store((&M0), (u64)(i0 + 40), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 48u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
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
  i0 = l1;
  i1 = 752u;
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
  f348(i0);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 816u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f215(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 784u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 776), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 776u;
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
  i64_store((&M0), (u64)(i0 + 768), j1);
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
  i1 = 13028u;
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
  i1 = 768u;
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 752u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 24u;
  i0 = i0 != i1;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13028u;
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
  B16:;
  i0 = l1;
  i1 = 32u;
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
  i1 = 752u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 32u;
  i0 = i0 != i1;
  if (i0) {goto B19;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = l1;
  i1 = 736u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 40u;
  i0 = i0 != i1;
  if (i0) {goto B22;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l4 = i0;
  i0 = l1;
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 48u;
  i0 = i0 != i1;
  if (i0) {goto B25;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  i1 = l3;
  i2 = 1u;
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
  i0 = i32_load8_u((&M0), (u64)(i0 + 32));
  l2 = i0;
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 49u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l4;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 776u;
  i1 += i2;
  i2 = l1;
  i3 = 768u;
  i2 += i3;
  i3 = l1;
  i4 = 752u;
  i3 += i4;
  i4 = l1;
  i5 = 736u;
  i4 += i5;
  i5 = l2;
  i6 = 0u;
  i5 = i5 != i6;
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
  f332(i0, i1, i2, i3, i4, i5);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 784u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f216(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 736u;
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
  i1 = 13028u;
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
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 736u;
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
  f458(i0, i1);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f217(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 760u;
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
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 744), j1);
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
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 13028u;
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
  i1 = 744u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 40));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l0;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  i3 = 744u;
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
  f366(i0, i1, i2);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f218(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 760u;
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
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 744), j1);
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
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 13028u;
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
  i1 = 744u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 40));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l0;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  i3 = 744u;
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
  f373(i0, i1, i2);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f219(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 760u;
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
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 744), j1);
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
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 13028u;
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
  i1 = 744u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 40));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l0;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  i3 = 744u;
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
  f381(i0, i1, i2);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f220(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 784u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 776), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 776u;
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
  i64_store((&M0), (u64)(i0 + 768), j1);
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
  i1 = 13028u;
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
  i1 = 768u;
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 752u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 24u;
  i0 = i0 != i1;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13028u;
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
  B16:;
  i0 = l1;
  i1 = 32u;
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
  i1 = 752u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 32u;
  i0 = i0 != i1;
  if (i0) {goto B19;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = l2;
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = 736u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 40u;
  i0 = i0 != i1;
  if (i0) {goto B22;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = l1;
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 776u;
  i1 += i2;
  i2 = l1;
  i3 = 768u;
  i2 += i3;
  i3 = l1;
  i4 = 752u;
  i3 += i4;
  i4 = l1;
  i5 = 736u;
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
  f419(i0, i1, i2, i3, i4);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 784u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f221(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 760u;
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
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 744), j1);
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
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 13028u;
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
  i1 = 744u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 40));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l0;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  i3 = 744u;
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
  f425(i0, i1, i2);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f222(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 744u;
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
  f433(i0, i1);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f223(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 784u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 776), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 776u;
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
  i64_store((&M0), (u64)(i0 + 768), j1);
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
  i1 = 13028u;
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
  i1 = 768u;
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 752u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 24u;
  i0 = i0 != i1;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13028u;
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
  B16:;
  i0 = l1;
  i1 = 32u;
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
  i1 = 752u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 32u;
  i0 = i0 != i1;
  if (i0) {goto B19;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = l2;
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = 736u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 40u;
  i0 = i0 != i1;
  if (i0) {goto B22;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = l1;
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 776u;
  i1 += i2;
  i2 = l1;
  i3 = 768u;
  i2 += i3;
  i3 = l1;
  i4 = 752u;
  i3 += i4;
  i4 = l1;
  i5 = 736u;
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
  f435(i0, i1, i2, i3, i4);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 784u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f224(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 784u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 776), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 776u;
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
  i64_store((&M0), (u64)(i0 + 768), j1);
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
  i1 = 13028u;
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
  i1 = 768u;
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 752u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 24u;
  i0 = i0 != i1;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13028u;
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
  B16:;
  i0 = l1;
  i1 = 32u;
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
  i1 = 752u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 32u;
  i0 = i0 != i1;
  if (i0) {goto B19;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = l2;
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = 736u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 40u;
  i0 = i0 != i1;
  if (i0) {goto B22;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = l1;
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 776u;
  i1 += i2;
  i2 = l1;
  i3 = 768u;
  i2 += i3;
  i3 = l1;
  i4 = 752u;
  i3 += i4;
  i4 = l1;
  i5 = 736u;
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
  f439(i0, i1, i2, i3, i4);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 784u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f225(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 760u;
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
  i1 = 13028u;
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
  i1 = 752u;
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = l2;
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = 736u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 24u;
  i0 = i0 != i1;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13028u;
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
  B16:;
  i0 = l1;
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 32u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  j2 = i64_load((&M0), (u64)(i2 + 752));
  i3 = l1;
  i4 = 736u;
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
  f442(i0, i1, j2, i3);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f226(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 760u;
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
  i1 = 13028u;
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
  i1 = 752u;
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = l2;
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = 736u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 24u;
  i0 = i0 != i1;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13028u;
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
  B16:;
  i0 = l1;
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 32u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  j2 = i64_load((&M0), (u64)(i2 + 752));
  i3 = l1;
  i4 = 736u;
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
  f446(i0, i1, j2, i3);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f227(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 760u;
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
  i1 = 13028u;
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
  i1 = 752u;
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = l2;
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = 736u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 24u;
  i0 = i0 != i1;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13028u;
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
  B16:;
  i0 = l1;
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 32u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  j2 = i64_load((&M0), (u64)(i2 + 752));
  i3 = l1;
  i4 = 736u;
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
  f450(i0, i1, j2, i3);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f228(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 760u;
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
  i1 = 13028u;
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
  i1 = 752u;
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = l2;
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = 736u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 24u;
  i0 = i0 != i1;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13028u;
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
  B16:;
  i0 = l1;
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 32u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  j2 = i64_load((&M0), (u64)(i2 + 752));
  i3 = l1;
  i4 = 736u;
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
  f453(i0, i1, j2, i3);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f229(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 744u;
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
  f456(i0, i1);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f230(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 744u;
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
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  l4 = i0;
  i0 = l2;
  i1 = 4294967294u;
  i0 &= i1;
  i1 = 8u;
  i0 = i0 != i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 742u;
  i0 += i1;
  i1 = l4;
  i2 = 2u;
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
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 10u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 744u;
  i1 += i2;
  i2 = l1;
  i2 = i32_load16_u((&M0), (u64)(i2 + 742));
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
  f460(i0, i1, i2);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f231(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 744u;
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
  f461(i0, i1);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f232(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 760u;
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
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 744), j1);
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
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 13028u;
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
  i1 = 744u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 40));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l0;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  i3 = 744u;
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
  f464(i0, i1, i2);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f233(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 760u;
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
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 744), j1);
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
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 13028u;
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
  i1 = 744u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 40));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l0;
  i2 = 24u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  i3 = 744u;
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
  f466(i0, i1, i2);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f234(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 744u;
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
  f468(i0, i1);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f235(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 784u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 776), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 776u;
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
  i64_store((&M0), (u64)(i0 + 768), j1);
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
  i1 = 13028u;
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
  i1 = 768u;
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 752u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 24u;
  i0 = i0 != i1;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13028u;
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
  B16:;
  i0 = l1;
  i1 = 32u;
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
  i1 = 752u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 32u;
  i0 = i0 != i1;
  if (i0) {goto B19;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = l2;
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = 736u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 40u;
  i0 = i0 != i1;
  if (i0) {goto B22;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = l1;
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 48u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 776u;
  i1 += i2;
  i2 = l1;
  i3 = 768u;
  i2 += i3;
  i3 = l1;
  i4 = 752u;
  i3 += i4;
  i4 = l1;
  i5 = 736u;
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
  f333(i0, i1, i2, i3, i4);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 784u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f236(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 760u;
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
  i64_store((&M0), (u64)(i0 + 752), j1);
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
  i1 = 13028u;
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
  i1 = 752u;
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = l2;
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = 736u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 24u;
  i0 = i0 != i1;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13028u;
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
  B16:;
  i0 = l1;
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 32u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  i3 = 752u;
  i2 += i3;
  i3 = l1;
  i4 = 736u;
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
  f304(i0, i1, i2, i3);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f237(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 760u;
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
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = 8u;
  i0 = i0 != i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 13028u;
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
  l4 = i0;
  i0 = l1;
  i1 = 752u;
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
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 4294967292u;
  i0 &= i1;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 748u;
  i0 += i1;
  i1 = l3;
  i2 = 4u;
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
  i1 = 16u;
  i0 += i1;
  i1 = l4;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l0;
  i2 = 20u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l4;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  i3 = 752u;
  i2 += i3;
  i3 = l1;
  i3 = i32_load((&M0), (u64)(i3 + 748));
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
  f303(i0, i1, i2, i3);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f238(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 13028u;
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
  i1 = 736u;
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
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 744u;
  i1 += i2;
  i2 = l1;
  j2 = i64_load((&M0), (u64)(i2 + 736));
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
  f309(i0, i1, j2);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f239(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 744u;
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
  f334(i0, i1);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f240(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l4 = 0, l5 = 0;
  u64 l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 832u;
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
  i0 = f97(i0);
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
  i32_store((&M0), (u64)(i0 + 820), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 816), i1);
  i0 = l1;
  i1 = l0;
  i2 = l2;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 824), i1);
  j0 = 0ull;
  l3 = j0;
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 808), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0 + 820));
  l0 = i0;
  B7:;
  i0 = l1;
  i1 = 808u;
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
  i32_store((&M0), (u64)(i0 + 820), i1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 768), i1);
  i0 = 0u;
  l4 = i0;
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i1 = l1;
    i2 = 816u;
    i1 += i2;
    i2 = 8u;
    i1 += i2;
    i1 = i32_load((&M0), (u64)(i1));
    i0 = i0 < i1;
    if (i0) {goto B11;}
    i0 = 0u;
    i1 = 14514u;
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
    i0 = i32_load((&M0), (u64)(i0 + 820));
    l0 = i0;
    B11:;
    i0 = l0;
    i0 = i32_load8_u((&M0), (u64)(i0));
    l2 = i0;
    i0 = l1;
    i1 = l0;
    i2 = 1u;
    i1 += i2;
    l5 = i1;
    i32_store((&M0), (u64)(i0 + 820), i1);
    j0 = l3;
    i1 = l2;
    i2 = 127u;
    i1 &= i2;
    i2 = l4;
    i3 = 255u;
    i2 &= i3;
    l0 = i2;
    i1 <<= (i2 & 31);
    j1 = (u64)(i1);
    j0 |= j1;
    l3 = j0;
    i0 = l0;
    i1 = 7u;
    i0 += i1;
    l4 = i0;
    i0 = l5;
    l0 = i0;
    i0 = l2;
    i1 = 128u;
    i0 &= i1;
    if (i0) {goto L10;}
  i0 = l1;
  j1 = l3;
  i64_store32((&M0), (u64)(i0 + 768), j1);
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  i1 = 4u;
  i0 |= i1;
  l0 = i0;
  i0 = l1;
  i1 = 824u;
  i0 += i1;
  l2 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l5;
  i0 -= i1;
  i1 = 32u;
  i0 = i0 > i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0 + 820));
  l5 = i0;
  B13:;
  i0 = l0;
  i1 = l5;
  i2 = 33u;
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
  i1 = l5;
  i2 = 33u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 820), i1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 760), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l1;
  i1 = 816u;
  i0 += i1;
  i1 = l1;
  i2 = 752u;
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
  i0 = f241(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1 + 820));
  l0 = i1;
  i0 -= i1;
  i1 = 1u;
  i0 = i0 > i1;
  if (i0) {goto B17;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0 + 820));
  l0 = i0;
  B17:;
  i0 = l1;
  i1 = 750u;
  i0 += i1;
  i1 = l0;
  i2 = 2u;
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
  i1 = i32_load((&M0), (u64)(i1 + 820));
  i2 = 2u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 820), i1);
  i0 = l1;
  i1 = 24u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  i2 = 816u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 816));
  l3 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  j1 = l3;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 808u;
  i1 += i2;
  i2 = l1;
  i3 = 768u;
  i2 += i3;
  i3 = l1;
  i4 = 752u;
  i3 += i4;
  i4 = l1;
  i4 = i32_load16_u((&M0), (u64)(i4 + 750));
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
  f517(i0, i1, i2, i3, i4);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 752));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B23;}
  i0 = l1;
  i1 = 752u;
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B23:;
  i0 = l1;
  i1 = 832u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f241(u32 p0, u32 p1) {
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
  i0 = f206(i0, i1);
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
  i0 = f103(i0);
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
  f131(i0, i1);
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
  f131(i0, i1);
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
  f128(i0);
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
  f105(i0);
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

static void f242(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 744u;
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
  f522(i0, i1);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f243(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  if (i0) {goto B4;}
  i0 = l2;
  i1 = 512u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B2;
  B4:;
  i0 = 0u;
  l0 = i0;
  i0 = l1;
  i1 = 744u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  l4 = i0;
  goto B1;
  B3:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  B2:;
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
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l4 = i0;
  i0 = l1;
  i1 = 744u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B0;}
  B1:;
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l4;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 744));
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
  f155(i0, j1);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f244(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  if (i0) {goto B4;}
  i0 = l2;
  i1 = 512u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  i0 = f97(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B2;
  B4:;
  i0 = 0u;
  l0 = i0;
  i0 = l1;
  i1 = 750u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  l4 = i0;
  goto B1;
  B3:;
  i0 = l0;
  i1 = l2;
  i2 = 15u;
  i1 += i2;
  i2 = 4294967280u;
  i1 &= i2;
  i0 -= i1;
  l0 = i0;
  g0 = i0;
  B2:;
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
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l4 = i0;
  i0 = l1;
  i1 = 750u;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 1u;
  i0 = i0 > i1;
  if (i0) {goto B0;}
  B1:;
  i0 = 0u;
  i1 = 13028u;
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
  i1 = l0;
  i2 = 2u;
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
  i1 = 16u;
  i0 += i1;
  i1 = l4;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i2 = 2u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l4;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = l1;
  i0 = i32_load16_u((&M0), (u64)(i0 + 750));
  l2 = i0;
  i0 = l0;
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
  f157(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = l2;
  i32_store16((&M0), (u64)(i0 + 448), i1);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f245(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  u64 l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 784u;
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
  i0 = f97(i0);
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
  i32_store((&M0), (u64)(i0 + 772), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 768), i1);
  i0 = l1;
  i1 = l0;
  i2 = l2;
  i1 += i2;
  l3 = i1;
  i32_store((&M0), (u64)(i0 + 776), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 776u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  l3 = i0;
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 772));
  l0 = i0;
  B7:;
  i0 = l1;
  i1 = 760u;
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
  i32_store((&M0), (u64)(i0 + 772), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 752), j1);
  i0 = l3;
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0 + 772));
  l0 = i0;
  B10:;
  i0 = l1;
  i1 = 752u;
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
  i32_store((&M0), (u64)(i0 + 772), i1);
  i0 = l1;
  i1 = 0u;
  i32_store((&M0), (u64)(i0 + 744), i1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  i1 = l1;
  i2 = 736u;
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
  i0 = f246(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 16u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  i2 = 768u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 768));
  l4 = j1;
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  j1 = l4;
  i64_store((&M0), (u64)(i0 + 16), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  i3 = 752u;
  i2 += i3;
  i3 = l1;
  i4 = 736u;
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
  f533(i0, i1, i2, i3);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i0 = i32_load((&M0), (u64)(i0 + 736));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B17;}
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 740), i1);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B17:;
  i0 = l1;
  i1 = 784u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f246(u32 p0, u32 p1) {
  u32 l0 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
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
    i1 = 14514u;
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
    l0 = i1;
    i32_store((&M0), (u64)(i0), i1);
    j0 = l1;
    i1 = l5;
    i2 = 127u;
    i1 &= i2;
    i2 = l4;
    i3 = 255u;
    i2 &= i3;
    l4 = i2;
    i1 <<= (i2 & 31);
    j1 = (u64)(i1);
    j0 |= j1;
    l1 = j0;
    i0 = l4;
    i1 = 7u;
    i0 += i1;
    l4 = i0;
    i0 = l0;
    l0 = i0;
    i0 = l5;
    i1 = 128u;
    i0 &= i1;
    if (i0) {goto L0;}
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0 + 4));
  l3 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1));
  l5 = i1;
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  l4 = i0;
  j1 = l1;
  i1 = (u32)(j1);
  l0 = i1;
  i0 = i0 >= i1;
  if (i0) {goto B5;}
  i0 = p1;
  i1 = l0;
  i2 = l4;
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
  f247(i0, i1);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&M0), (u64)(i0));
  l5 = i0;
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&M0), (u64)(i1));
  l3 = i1;
  i0 = i0 != i1;
  if (i0) {goto B4;}
  goto B3;
  B5:;
  i0 = l4;
  i1 = l0;
  i0 = i0 <= i1;
  if (i0) {goto B7;}
  i0 = p1;
  i1 = 4u;
  i0 += i1;
  i1 = l5;
  i2 = l0;
  i3 = 3u;
  i2 <<= (i3 & 31);
  i1 += i2;
  l3 = i1;
  i32_store((&M0), (u64)(i0), i1);
  B7:;
  i0 = l5;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  B4:;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l4 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l2 = i0;
  L8: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i0 = i32_load((&M0), (u64)(i0));
    i1 = l0;
    i0 -= i1;
    i1 = 7u;
    i0 = i0 > i1;
    if (i0) {goto B9;}
    i0 = 0u;
    i1 = 13028u;
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
    i0 = l4;
    i0 = i32_load((&M0), (u64)(i0));
    l0 = i0;
    B9:;
    i0 = l5;
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
    i0 = l4;
    i1 = l4;
    i1 = i32_load((&M0), (u64)(i1));
    i2 = 8u;
    i1 += i2;
    l0 = i1;
    i32_store((&M0), (u64)(i0), i1);
    i0 = l3;
    i1 = l5;
    i2 = 8u;
    i1 += i2;
    l5 = i1;
    i0 = i0 != i1;
    if (i0) {goto L8;}
  B3:;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f247(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  l0 = i0;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l1 = i1;
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  i1 = p1;
  i0 = i0 >= i1;
  if (i0) {goto B4;}
  i0 = l1;
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1));
  l2 = i1;
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  l1 = i0;
  i1 = p1;
  i0 += i1;
  l3 = i0;
  i1 = 536870912u;
  i0 = i0 >= i1;
  if (i0) {goto B2;}
  i0 = 536870911u;
  l4 = i0;
  i0 = l0;
  i1 = l2;
  i0 -= i1;
  l0 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  i1 = 268435454u;
  i0 = i0 > i1;
  if (i0) {goto B5;}
  i0 = l3;
  i1 = l0;
  i2 = 2u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  l4 = i1;
  i2 = l4;
  i3 = l3;
  i2 = i2 < i3;
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = l4;
  i1 = 536870912u;
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  B5:;
  i0 = l4;
  i1 = 3u;
  i0 <<= (i1 & 31);
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
  i0 = f103(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  goto B0;
  B4:;
  i0 = l1;
  l4 = i0;
  i0 = p1;
  l0 = i0;
  L7: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    j1 = 0ull;
    i64_store((&M0), (u64)(i0), j1);
    i0 = l4;
    i1 = 8u;
    i0 += i1;
    l4 = i0;
    i0 = l0;
    i1 = 4294967295u;
    i0 += i1;
    l0 = i0;
    if (i0) {goto L7;}
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i1 = l1;
  i2 = p1;
  i3 = 3u;
  i2 <<= (i3 & 31);
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  goto Bfunc;
  B3:;
  i0 = 0u;
  l4 = i0;
  i0 = 0u;
  l0 = i0;
  goto B0;
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
  f111(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
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
  B0:;
  i0 = l0;
  i1 = l4;
  i2 = 3u;
  i1 <<= (i2 & 31);
  i0 += i1;
  l2 = i0;
  i0 = l0;
  i1 = l1;
  i2 = 3u;
  i1 <<= (i2 & 31);
  i0 += i1;
  l1 = i0;
  l4 = i0;
  i0 = p1;
  l0 = i0;
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    j1 = 0ull;
    i64_store((&M0), (u64)(i0), j1);
    i0 = l4;
    i1 = 8u;
    i0 += i1;
    l4 = i0;
    i0 = l0;
    i1 = 4294967295u;
    i0 += i1;
    l0 = i0;
    if (i0) {goto L10;}
  i0 = l1;
  i1 = p1;
  i2 = 3u;
  i1 <<= (i2 & 31);
  i0 += i1;
  l3 = i0;
  i0 = l1;
  i1 = p0;
  i2 = 4u;
  i1 += i2;
  l5 = i1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = p0;
  i2 = i32_load((&M0), (u64)(i2));
  l4 = i2;
  i1 -= i2;
  l0 = i1;
  i0 -= i1;
  p1 = i0;
  i0 = l0;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B11;}
  i0 = p1;
  i1 = l4;
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
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0));
  l4 = i0;
  B11:;
  i0 = p0;
  i1 = p1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l5;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B13;}
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f248(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 744u;
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
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  l4 = i0;
  i0 = l2;
  i1 = 8u;
  i0 = i0 != i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 40u;
  i0 += i1;
  i1 = l4;
  i2 = 1u;
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
  i0 = i32_load8_u((&M0), (u64)(i0 + 40));
  l2 = i0;
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l0;
  i2 = 9u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 744u;
  i1 += i2;
  i2 = l2;
  i3 = 0u;
  i2 = i2 != i3;
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
  f545(i0, i1, i2);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f249(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
  u64 l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 832u;
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
  i0 = f97(i0);
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
  i32_store((&M0), (u64)(i0 + 820), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 816), i1);
  i0 = l1;
  i1 = l0;
  i2 = l2;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 824), i1);
  i0 = l1;
  i1 = 816u;
  i0 += i1;
  i1 = l1;
  i2 = 744u;
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
  i0 = f250(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 24u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1 + 824));
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l0;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 816));
  l3 = j1;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  j1 = l3;
  i64_store((&M0), (u64)(i0 + 24), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
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
  i0 = l0;
  i1 = 272u;
  i0 += i1;
  i1 = l1;
  i2 = 744u;
  i1 += i2;
  i2 = 72u;
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
  i1 = 338u;
  i0 += i1;
  i0 = i32_load16_u((&M0), (u64)(i0));
  i1 = 2u;
  i0 = i0 > i1;
  if (i0) {goto B11;}
  i0 = 0u;
  i1 = 9150u;
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
  i0 = l1;
  i1 = 744u;
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
  f112(i0);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 832u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static u32 f250(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l0 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13028u;
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
  l0 = i0;
  B0:;
  i0 = p1;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l0 = i0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  B3:;
  i0 = l2;
  i1 = l1;
  i2 = 4u;
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
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 12u;
  i0 += i1;
  l0 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B6;}
  i0 = 0u;
  i1 = 13028u;
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
  l1 = i0;
  B6:;
  i0 = l0;
  i1 = l1;
  i2 = 4u;
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
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  B9:;
  i0 = l2;
  i1 = l1;
  i2 = 4u;
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
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  l0 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B12;}
  i0 = 0u;
  i1 = 13028u;
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
  l1 = i0;
  B12:;
  i0 = l0;
  i1 = l1;
  i2 = 4u;
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
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B15;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  B15:;
  i0 = l2;
  i1 = l1;
  i2 = 4u;
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
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 28u;
  i0 += i1;
  l0 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B18;}
  i0 = 0u;
  i1 = 13028u;
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
  l1 = i0;
  B18:;
  i0 = l0;
  i1 = l1;
  i2 = 4u;
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
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 32u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B21;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  B21:;
  i0 = l2;
  i1 = l1;
  i2 = 4u;
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
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 36u;
  i0 += i1;
  l0 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B24;}
  i0 = 0u;
  i1 = 13028u;
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
  l1 = i0;
  B24:;
  i0 = l0;
  i1 = l1;
  i2 = 4u;
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
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 40u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B27;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  B27:;
  i0 = l2;
  i1 = l1;
  i2 = 4u;
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
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 44u;
  i0 += i1;
  l0 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B30;}
  i0 = 0u;
  i1 = 13028u;
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
  l1 = i0;
  B30:;
  i0 = l0;
  i1 = l1;
  i2 = 4u;
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
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 48u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B33;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  B33:;
  i0 = l2;
  i1 = l1;
  i2 = 4u;
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
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 52u;
  i0 += i1;
  l0 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B36;}
  i0 = 0u;
  i1 = 13028u;
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
  l1 = i0;
  B36:;
  i0 = l0;
  i1 = l1;
  i2 = 4u;
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
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 56u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B39;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  B39:;
  i0 = l2;
  i1 = l1;
  i2 = 4u;
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
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 60u;
  i0 += i1;
  l0 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B42;}
  i0 = 0u;
  i1 = 13028u;
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
  l1 = i0;
  B42:;
  i0 = l0;
  i1 = l1;
  i2 = 4u;
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
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 64u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 1u;
  i0 = i0 > i1;
  if (i0) {goto B45;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  B45:;
  i0 = l2;
  i1 = l1;
  i2 = 2u;
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
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 2u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 66u;
  i0 += i1;
  p1 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 1u;
  i0 = i0 > i1;
  if (i0) {goto B48;}
  i0 = 0u;
  i1 = 13028u;
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
  l1 = i0;
  B48:;
  i0 = p1;
  i1 = l1;
  i2 = 2u;
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
  p1 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 2u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f251(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 744u;
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
  f354(i0, i1);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f252(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 744u;
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
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  l4 = i0;
  i0 = l2;
  i1 = 8u;
  i0 = i0 != i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 743u;
  i0 += i1;
  i1 = l4;
  i2 = 1u;
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
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 9u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 743));
  l2 = i0;
  i0 = l0;
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
  i0 = l1;
  j0 = i64_load((&M0), (u64)(i0 + 744));
  i1 = l2;
  i2 = 0u;
  i1 = i1 != i2;
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
  (*Z_envZ_set_privilegedZ_vji)(j0, i1);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f253(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l0;
  i1 = l2;
  i0 += i1;
  l3 = i0;
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 744u;
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
  i1 = 8u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 744u;
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
  f175(i0, i1);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f254(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 751u;
  i0 += i1;
  i1 = l0;
  i2 = 1u;
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
  i1 = 16u;
  i0 += i1;
  i1 = l0;
  i2 = l2;
  i1 += i2;
  l2 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i2 = 1u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 28), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 32), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 24));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = 40u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
  i3 = l1;
  i4 = 8u;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = l1;
  i0 = i32_load8_u((&M0), (u64)(i0 + 751));
  l2 = i0;
  i0 = l0;
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
  i0 = l0;
  i1 = 472u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load8_u((&M0), (u64)(i0));
  l4 = i0;
  i1 = 255u;
  i0 = i0 == i1;
  if (i0) {goto B15;}
  i0 = l4;
  i1 = 255u;
  i0 &= i1;
  i1 = 1u;
  i0 += i1;
  i1 = l2;
  i0 = i0 != i1;
  if (i0) {goto B14;}
  goto B13;
  B15:;
  i0 = 0u;
  i1 = 10929u;
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
  i0 = i32_load8_u((&M0), (u64)(i0));
  i1 = 255u;
  i0 &= i1;
  i1 = 1u;
  i0 += i1;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B13;}
  B14:;
  i0 = 0u;
  i1 = 11090u;
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
  i1 = 2u;
  i0 = i0 >= i1;
  if (i0) {goto B12;}
  goto B11;
  B13:;
  i0 = l2;
  i1 = 2u;
  i0 = i0 < i1;
  if (i0) {goto B11;}
  B12:;
  i0 = 0u;
  i1 = 11201u;
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
  i1 = 472u;
  i0 += i1;
  i1 = l2;
  i32_store8((&M0), (u64)(i0), i1);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f255(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 768u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 760), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 760u;
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
  i64_store((&M0), (u64)(i0 + 752), j1);
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
  i1 = 13028u;
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
  i1 = 752u;
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l1;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 736), j1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 16u;
  i0 = i0 != i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = l2;
  i0 += i1;
  l2 = i0;
  i0 = l1;
  i1 = 736u;
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
  i64_store((&M0), (u64)(i0 + 32), j1);
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  l3 = i0;
  i0 = l4;
  i1 = 24u;
  i0 = i0 != i1;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13028u;
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
  B16:;
  i0 = l1;
  i1 = 32u;
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
  i1 = 736u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 32));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 32u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l2;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 760u;
  i1 += i2;
  i2 = l1;
  i3 = 752u;
  i2 += i3;
  i3 = l1;
  i4 = 736u;
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
  f285(i0, i1, i2, i3);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 768u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f256(u64 p0, u64 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = g0;
  i1 = 752u;
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
  i0 = f97(i0);
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
  i64_store((&M0), (u64)(i0 + 744), j1);
  i0 = l2;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  i1 = 744u;
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
  i64_store((&M0), (u64)(i0 + 736), j1);
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
  i1 = 13028u;
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
  i1 = 736u;
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
  i1 = 8u;
  i0 += i1;
  i1 = l3;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l1;
  i1 = l0;
  i32_store((&M0), (u64)(i0 + 16), i1);
  i0 = l1;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  i32_store((&M0), (u64)(i0 + 20), i1);
  i0 = l1;
  i1 = l3;
  i32_store((&M0), (u64)(i0 + 24), i1);
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&M0), (u64)(i1 + 16));
  i64_store((&M0), (u64)(i0), j1);
  i0 = l1;
  i1 = 32u;
  i0 += i1;
  j1 = p0;
  j2 = p1;
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
  i0 = f137(i0, j1, j2, i3);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l1;
  i2 = 744u;
  i1 += i2;
  i2 = l1;
  i3 = 736u;
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
  f300(i0, i1, i2);
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
  i0 = f144(i0);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l1;
  i1 = 752u;
  i0 += i1;
  g0 = i0;
  FUNC_EPILOGUE;
}

static void f257(u32 p0, u32 p1) {
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
  i1 = 13028u;
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
  i1 = 13028u;
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
  i1 = 13028u;
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
  i1 = 13028u;
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
  i1 = 13028u;
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

static void f258(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f103(i0);
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
  f111(i0);
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
    f105(i0);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static u32 f259(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0;
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
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l1 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13028u;
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
  l1 = i0;
  B0:;
  i0 = p1;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l2 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l2;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  B3:;
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
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
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  i1 = p1;
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
  i0 = f260(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i1 = p1;
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
  i0 = f260(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f260(u32 p0, u32 p1) {
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
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l1 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13028u;
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
  l1 = i0;
  B0:;
  i0 = p1;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l2 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  j1 = 0ull;
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l2;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l2 = i0;
  B3:;
  i0 = l0;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
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
  j1 = i64_load((&M0), (u64)(i1 + 8));
  i64_store((&M0), (u64)(i0 + 8), j1);
  i0 = l1;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l2 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  l1 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l2;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B6;}
  i0 = 0u;
  i1 = 13028u;
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
  l2 = i0;
  B6:;
  i0 = l1;
  i1 = l2;
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
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  g0 = i0;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f261(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
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
  i0 = f250(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 72u;
  i0 += i1;
  l0 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l1 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B1;}
  i0 = 0u;
  i1 = 13028u;
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
  l1 = i0;
  B1:;
  i0 = l0;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 80u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B4;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  B4:;
  i0 = l2;
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
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 88u;
  i0 += i1;
  l1 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13028u;
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
  l0 = i0;
  B7:;
  i0 = l1;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 96u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  B10:;
  i0 = l2;
  i1 = l0;
  i2 = 4u;
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
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 4u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 104u;
  i0 += i1;
  l1 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13028u;
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
  l0 = i0;
  B13:;
  i0 = l1;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 112u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  B16:;
  i0 = l2;
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
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 120u;
  i0 += i1;
  l1 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B19;}
  i0 = 0u;
  i1 = 13028u;
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
  l0 = i0;
  B19:;
  i0 = l1;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 128u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B22;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  B22:;
  i0 = l2;
  i1 = l0;
  i2 = 4u;
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
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 4u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 136u;
  i0 += i1;
  l1 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B25;}
  i0 = 0u;
  i1 = 13028u;
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
  l0 = i0;
  B25:;
  i0 = l1;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 144u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B28;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  B28:;
  i0 = l2;
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
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 152u;
  i0 += i1;
  l1 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 1u;
  i0 = i0 > i1;
  if (i0) {goto B31;}
  i0 = 0u;
  i1 = 13028u;
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
  l0 = i0;
  B31:;
  i0 = l1;
  i1 = l0;
  i2 = 2u;
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
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 2u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 160u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B34;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  B34:;
  i0 = l2;
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
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 168u;
  i0 += i1;
  p1 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B37;}
  i0 = 0u;
  i1 = 13028u;
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
  l0 = i0;
  B37:;
  i0 = p1;
  i1 = l0;
  i2 = 4u;
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
  p1 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 4u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f262(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f103(i0);
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
  f111(i0);
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
    f105(i0);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static u32 f263(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l0 = i1;
  i0 -= i1;
  i1 = 1u;
  i0 = i0 > i1;
  if (i0) {goto B0;}
  i0 = 0u;
  i1 = 13028u;
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
  l0 = i0;
  B0:;
  i0 = p1;
  i1 = l0;
  i2 = 2u;
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
  i2 = 2u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 4u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  B3:;
  i0 = l2;
  i1 = l1;
  i2 = 4u;
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
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  if (i0) {goto B6;}
  i0 = 0u;
  i1 = 13028u;
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
  l1 = i0;
  B6:;
  i0 = l0;
  i1 = l1;
  i2 = 4u;
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
  i2 = 4u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = 13028u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l1 = i0;
  B9:;
  i0 = l2;
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
  i0 = l0;
  i1 = l0;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l1 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  p1 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l1;
  i0 = i0 != i1;
  if (i0) {goto B12;}
  i0 = 0u;
  i1 = 13028u;
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
  l1 = i0;
  B12:;
  i0 = p1;
  i1 = l1;
  i2 = 1u;
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
  p1 = i0;
  i1 = p1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 1u;
  i1 += i2;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f264(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f103(i0);
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
  f111(i0);
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
    f105(i0);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static void f265(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f103(i0);
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
  f111(i0);
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
    f105(i0);
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
  f105(i0);
  i0 = g3;
  i1 = 1u;
  i0 += i1;
  g3 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static u32 f266(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
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
  i0 = f267(i0, i1);
  i1 = g3;
  i2 = 1u;
  i1 += i2;
  g3 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 72u;
  i0 += i1;
  l0 = i0;
  i0 = p0;
  i0 = i32_load((&M0), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&M0), (u64)(i1 + 4));
  l1 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B1;}
  i0 = 0u;
  i1 = 13772u;
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
  l1 = i0;
  B1:;
  i0 = l1;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 80u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B4;}
  i0 = 0u;
  i1 = 13772u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  B4:;
  i0 = l0;
  i1 = l2;
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
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 88u;
  i0 += i1;
  l1 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B7;}
  i0 = 0u;
  i1 = 13772u;
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
  l0 = i0;
  B7:;
  i0 = l0;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 96u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B10;}
  i0 = 0u;
  i1 = 13772u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  B10:;
  i0 = l0;
  i1 = l2;
  i2 = 4u;
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
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 4u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 104u;
  i0 += i1;
  l1 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = 13772u;
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
  l0 = i0;
  B13:;
  i0 = l0;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 112u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B16;}
  i0 = 0u;
  i1 = 13772u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  B16:;
  i0 = l0;
  i1 = l2;
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
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 120u;
  i0 += i1;
  l1 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B19;}
  i0 = 0u;
  i1 = 13772u;
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
  l0 = i0;
  B19:;
  i0 = l0;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 128u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B22;}
  i0 = 0u;
  i1 = 13772u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  B22:;
  i0 = l0;
  i1 = l2;
  i2 = 4u;
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
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 4u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 136u;
  i0 += i1;
  l1 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B25;}
  i0 = 0u;
  i1 = 13772u;
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
  l0 = i0;
  B25:;
  i0 = l0;
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
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  i1 = l1;
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 144u;
  i0 += i1;
  l2 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B28;}
  i0 = 0u;
  i1 = 13772u;
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
  i0 = i32_load((&M0), (u64)(i0));
  l0 = i0;
  B28:;
  i0 = l0;
  i1 = l2;
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
  i1 = i32_load((&M0), (u64)(i1));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&M0), (u64)(i0), i1);
  i0 = p1;
  i1 = 152u;
  i0 += i1;
  l1 = i0;
  i0 = l3;
  i0 = i32_load((&M0), (u64)(i0));
  i1 = l0;
  i0 -= i1;
  i1 = 1u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B31;}
  i0 = 0u;
  i1 = 13772u;
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
  l0 = i0;
  B31:;
