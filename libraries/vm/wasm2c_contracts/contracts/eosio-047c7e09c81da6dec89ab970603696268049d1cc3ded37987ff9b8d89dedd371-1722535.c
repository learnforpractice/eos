#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "contracts/eosio-047c7e09c81da6dec89ab970603696268049d1cc3ded37987ff9b8d89dedd371-1722535.h"
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


static u32 func_types[48];

static void init_func_types(void) {
  func_types[0] = wasm_rt_register_func_type(3, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64);
  func_types[1] = wasm_rt_register_func_type(1, 0, WASM_RT_I32);
  func_types[2] = wasm_rt_register_func_type(2, 0, WASM_RT_I32, WASM_RT_I64);
  func_types[3] = wasm_rt_register_func_type(2, 0, WASM_RT_I32, WASM_RT_I32);
  func_types[4] = wasm_rt_register_func_type(3, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32);
  func_types[5] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32);
  func_types[6] = wasm_rt_register_func_type(6, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[7] = wasm_rt_register_func_type(5, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[8] = wasm_rt_register_func_type(5, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[9] = wasm_rt_register_func_type(3, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64);
  func_types[10] = wasm_rt_register_func_type(0, 0);
  func_types[11] = wasm_rt_register_func_type(0, 1, WASM_RT_I64);
  func_types[12] = wasm_rt_register_func_type(2, 0, WASM_RT_I64, WASM_RT_I64);
  func_types[13] = wasm_rt_register_func_type(1, 0, WASM_RT_I64);
  func_types[14] = wasm_rt_register_func_type(4, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32);
  func_types[15] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[16] = wasm_rt_register_func_type(5, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32);
  func_types[17] = wasm_rt_register_func_type(3, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[18] = wasm_rt_register_func_type(5, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[19] = wasm_rt_register_func_type(2, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64);
  func_types[20] = wasm_rt_register_func_type(2, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[21] = wasm_rt_register_func_type(6, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[22] = wasm_rt_register_func_type(4, 0, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[23] = wasm_rt_register_func_type(1, 1, WASM_RT_I32, WASM_RT_I32);
  func_types[24] = wasm_rt_register_func_type(5, 1, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[25] = wasm_rt_register_func_type(2, 0, WASM_RT_I64, WASM_RT_I32);
  func_types[26] = wasm_rt_register_func_type(1, 1, WASM_RT_I64, WASM_RT_I32);
  func_types[27] = wasm_rt_register_func_type(0, 1, WASM_RT_I32);
  func_types[28] = wasm_rt_register_func_type(3, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[29] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[30] = wasm_rt_register_func_type(4, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[31] = wasm_rt_register_func_type(5, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[32] = wasm_rt_register_func_type(6, 1, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[33] = wasm_rt_register_func_type(3, 1, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[34] = wasm_rt_register_func_type(4, 0, WASM_RT_I64, WASM_RT_I64, WASM_RT_I32, WASM_RT_I32);
  func_types[35] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64);
  func_types[36] = wasm_rt_register_func_type(5, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[37] = wasm_rt_register_func_type(4, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32);
  func_types[38] = wasm_rt_register_func_type(1, 1, WASM_RT_I64, WASM_RT_F64);
  func_types[39] = wasm_rt_register_func_type(2, 1, WASM_RT_I32, WASM_RT_I64, WASM_RT_I32);
  func_types[40] = wasm_rt_register_func_type(1, 1, WASM_RT_I64, WASM_RT_I64);
  func_types[41] = wasm_rt_register_func_type(3, 0, WASM_RT_I64, WASM_RT_I64, WASM_RT_I64);
  func_types[42] = wasm_rt_register_func_type(8, 0, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32, WASM_RT_I32);
  func_types[43] = wasm_rt_register_func_type(1, 1, WASM_RT_F64, WASM_RT_F64);
  func_types[44] = wasm_rt_register_func_type(2, 1, WASM_RT_F64, WASM_RT_F64, WASM_RT_F64);
  func_types[45] = wasm_rt_register_func_type(2, 1, WASM_RT_F64, WASM_RT_I32, WASM_RT_F64);
  func_types[46] = wasm_rt_register_func_type(2, 1, WASM_RT_F64, WASM_RT_F64, WASM_RT_I32);
  func_types[47] = wasm_rt_register_func_type(1, 1, WASM_RT_F64, WASM_RT_I64);
}

static u32 now(void);
static u32 _ZeqRK11checksum256S1_(u32, u32);
static u32 _ZeqRK11checksum160S1_(u32, u32);
static u32 _ZneRK11checksum160S1_(u32, u32);
static void _ZN5eosio12require_authERKNS_16permission_levelE(u32);
static void _ZN11eosiosystem15system_contract7onblockEN5eosio15block_timestampEy(u32, u32, u64);
static u32 f61(u32, u32);
static void f62(u32, u32);
static void _ZN11eosiosystem15system_contract24update_elected_producersEN5eosio15block_timestampE(u32, u32);
static void f64_0(u32, u32, u32);
static void f65(u32, u32);
static u32 f66(u32, u32);
static void f67(u32, u32);
static void f68(u32, u32, u32, u32);
static void f69(u32, u32, u32);
static void f70(u32, u32);
static u32 f71(u32);
static void f72(u32, u32, u32);
static void f73(u32, u32);
static u32 f74(u32, u32);
static void f75(u32, u32);
static u32 f76(u32, u32);
static u32 f77(u32, u32, u32, u32);
static u32 f78(u32, u32, u32, u32, u32);
static u32 f79(u32, u32, u32, u32, u32, u32);
static u32 f80(u32, u32, u32);
static u32 f81(u32, u32);
static u32 f82(u32, u32);
static u32 f83(u32, u32);
static void f84(u32, u32, u32, u32);
static u32 f85(u32, u32);
static u32 f86(u32, u32);
static void _ZN11eosiosystem15system_contract12claimrewardsERKy(u32, u32);
static u32 f88(u32, u64, u32);
static u32 f89(u32, u64, u32);
static void f90(u64, u64, u32, u32);
static void f91(u64, u64, u32, u32);
static void f92(u32, u32, u32);
static void f93(u32, u32);
static void f94(u32, u32);
static u32 f95(u32, u32);
static u32 f96(u32, u32);
static u32 f97(u32, u32);
static u32 f98(u32);
static void f99(u32, u32);
static void f100(u32, u32, u32, u32);
static void _ZN11eosiosystem15system_contract11buyrambytesEyym(u32, u64, u64, u32);
static u32 f102(u32, u32);
static void _ZN11eosiosystem14exchange_state7convertEN5eosio5assetENS1_11symbol_typeE(u32, u32, u32, u64);
static void _ZN11eosiosystem15system_contract6buyramEyyN5eosio5assetE(u32, u64, u64, u32);
static u32 f105(u32, u32);
static u32 f106(u32, u32);
static u32 f107(u32);
static void f108(u32, u32, u32, u32);
static u32 f109(u32, u64, u32);
static u32 f110(u32, u32);
static u32 f111(u32, u32);
static void _ZN11eosiosystem14exchange_state21convert_from_exchangeERNS0_9connectorEN5eosio5assetE(u32, u32, u32, u32);
static u32 f113(u32);
static void f114(u32, u32);
static void f115(u32, u32, u32, u32);
static void _ZN11eosiosystem15system_contract7sellramEyx(u32, u64, u64);
static void f117(u32, u32, u32);
static void _ZN11eosiosystem19validate_b1_vestingEx(u64);
static void _ZN11eosiosystem15system_contract8changebwEyyN5eosio5assetES2_b(u32, u64, u64, u32, u32, u32);
static u32 f120(u32, u32);
static u32 f121(u32, u32);
static u32 f122(u32);
static void f123(u32, u32, u32, u32);
static void f124(u32, u32);
static void f125(u32, u32);
static u32 f126(u32, u32);
static void f127(u32, u32, u32);
static void f128(u32, u32);
static u32 f129(u32);
static void f130(u32, u32);
static void f131(u32, u32, u32, u32);
static void f132(u32, u32, u32, u32, u32);
static void f133(u32, u32);
static u32 f134(u32);
static u32 f135(u32, u32);
static void f136(u32, u32);
static void f137(u32, u32, u32, u32);
static void f138(u32, u32, u32);
static void _ZN11eosiosystem15system_contract12update_votesEyyRKNSt3__16vectorIyNS1_9allocatorIyEEEEb(u32, u64, u64, u32, u32);
static void f140(u32, u32, u32);
static void _ZN11eosiosystem15system_contract23propagate_weight_changeERKNS_10voter_infoE(u32, u32);
static void f142(u32, u32, u32, u32);
static void f143(u32, u32, u32);
static void f144(u32, u32, u32);
static void f145(u32, u32, u32);
static void f146(u32, u32, u32);
static u32 f147(u32, u32);
static u32 f148(u32, u32);
static void f149(u32, u32, u32, u32, u32);
static void f150(u32, u32, u32, u32);
static u32 f151(u32, u64, u32);
static void f152(u32, u32, u32);
static void f153(u32, u32, u32);
static void f154(u32, u32, u32);
static u32 f155(u32, u32, u32);
static u32 f156(u32, u32);
static u32 f157(u32, u32);
static void f158(u32, u32);
static u32 f159(u32, u32);
static u32 f160(u32, u32);
static u32 f161(u32, u32);
static u32 f162(u32, u32);
static u32 f163(u32, u32);
static u32 f164(u32, u32);
static u32 f165(u32, u32);
static void _ZN11eosiosystem15system_contract10delegatebwEyyN5eosio5assetES2_b(u32, u64, u64, u32, u32, u32);
static void _ZN11eosiosystem15system_contract12undelegatebwEyyN5eosio5assetES2_(u32, u64, u64, u32, u32);
static void _ZN11eosiosystem15system_contract6refundEy(u32, u64);
static void _ZN11eosiosystem15system_contract11regproducerEyRKN5eosio10public_keyERKNSt3__112basic_stringIcNS5_11char_traitsIcEENS5_9allocatorIcEEEEt(u32, u64, u32, u32, u32);
static u32 f170(u32, u32);
static void f171(u32, u32, u64, u32);
static void f172(u32, u32);
static void _ZN11eosiosystem15system_contract9unregprodEy(u32, u64);
static void f174(u32, u32);
static f64 _ZN11eosiosystem10stake2voteEx(u64);
static void _ZN11eosiosystem15system_contract12voteproducerEyyRKNSt3__16vectorIyNS1_9allocatorIyEEEE(u32, u64, u64, u32);
static void _ZN11eosiosystem15system_contract8regproxyEyb(u32, u64, u32);
static void f178(u32, u32, u32);
static void f179(u32, u32);
static void _ZN11eosiosystem14exchange_state19convert_to_exchangeERNS0_9connectorEN5eosio5assetE(u32, u32, u32, u32);
static u32 _ZN11eosiosystem15system_contractC2Ey(u32, u64);
static u32 f182(u32, u32);
static u32 f183(u32, u32);
static void f184(u32, u32, u32, u32);
static u32 f185(u32, u32);
static void _ZN11eosiosystem15system_contract22get_default_parametersEv(u32);
static u32 _ZN11eosiosystem15system_contractD2Ev(u32);
static void f188(u32, u32, u64);
static void f189(u32, u32, u64, u32);
static void f190(u32, u32, u64, u32);
static u32 f191(u32, u32);
static u32 f192(u32, u32);
static void _ZN11eosiosystem15system_contract6setramEy(u32, u64);
static void _ZN11eosiosystem15system_contract9setparamsERKN5eosio21blockchain_parametersE(u32, u32);
static void _ZN11eosiosystem15system_contract7setprivEyh(u32, u64, u32);
static void _ZN11eosiosystem15system_contract11rmvproducerEy(u32, u64);
static void f197(u32, u32);
static void _ZN11eosiosystem15system_contract7bidnameEyyN5eosio5assetE(u32, u64, u64, u32);
static u64 f199(u64);
static void f200(u32, u32);
static void f201(u32);
static void f202(u32, u32);
static void _ZN11eosiosystem6native10newaccountEyy(u32, u64, u64);
static void f204(u32, u32);
static void apply(u64, u64, u64);
static u32 f206(u32, u32);
static void f207(u32);
static u32 f208(u32, u32);
static void f209(u32);
static void f210(u32);
static void f211(u32);
static void f212(u32);
static void f213(u32);
static u32 f214(u32, u32);
static u32 f215(u32, u32);
static u32 f216(u32, u32);
static u32 f217(u32, u32);
static u32 f218(u32, u32);
static u32 f219(u32, u32);
static u32 f220(u32, u32);
static u32 f221(u32, u32);
static u32 f222(u32, u32);
static u32 f223(u32, u32);
static u32 f224(u32, u32);
static u32 f225(u32, u32);
static u32 f226(u32, u32);
static void f227(u32, u32);
static void f228(u32, u32);
static void f229(u32, u32, u32);
static void f230(u32, u32);
static void f231(u32, u32, u32);
static void f232(u32, u32);
static void f233(u32, u32);
static u32 f234(u32);
static void f235(u32);
static void f236(u32);
static u32 f237(u32, u32);
static void f238(u32, u32, u32, u32, u32, u32, u32, u32);
static void f239(u32, u32);
static u32 f240(u32, u32, u32);
static void f241(u32);
static u32 f242(u32, u32);
static u32 f243(u32, u32, u32, u32, u32);
static f64 fabs_0(f64);
static f64 pow_0(f64, f64);
static f64 sqrt_0(f64);
static f64 scalbn_0(f64, u32);
static u32 memcmp_0(u32, u32, u32);
static u32 strlen_0(u32);
static void _ZN5eosio25set_blockchain_parametersERKNS_21blockchain_parametersE(u32);
static void _ZN5eosio25get_blockchain_parametersERNS_21blockchain_parametersE(u32);
static u32 malloc_0(u32);
static u32 f253(u32, u32);
static u32 f254(u32);
static void free_0(u32);
static void f256(void);

static u32 g0;

static void init_globals(void) {
  g0 = 250u;
}

static wasm_rt_memory_t memory;

static wasm_rt_table_t T0;

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

static void _ZN11eosiosystem15system_contract7onblockEN5eosio15block_timestampEy(u32 p0, u32 p1, u64 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l8 = 0, l9 = 0;
  u64 l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1, j2, j3, j4, j5;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 144u;
  i1 -= i2;
  l9 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  j0 = 0ull;
  l5 = j0;
  j0 = 59ull;
  l4 = j0;
  i0 = 16u;
  l3 = i0;
  j0 = 0ull;
  l6 = j0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l5;
    j1 = 4ull;
    i0 = j0 > j1;
    if (i0) {goto B5;}
    i0 = l3;
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
    l7 = j0;
    j0 = l5;
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
    l7 = j0;
    B2:;
    j0 = l7;
    j1 = 31ull;
    j0 &= j1;
    j1 = l4;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l7 = j0;
    B1:;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    j0 = l5;
    j1 = 1ull;
    j0 += j1;
    l5 = j0;
    j0 = l7;
    j1 = l6;
    j0 |= j1;
    l6 = j0;
    j0 = l4;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l4 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L0;}
  j0 = l6;
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
  i0 = p0;
  i1 = 264u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 1500000000000ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B7;}
  i0 = p0;
  i1 = 232u;
  i0 += i1;
  l3 = i0;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 0ull;
  i0 = j0 != j1;
  if (i0) {goto B8;}
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
  j1 = (*Z_envZ_current_timeZ_jv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i64_store((&memory), (u64)(i0), j1);
  B8:;
  i0 = p0;
  i1 = 76u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l8 = i0;
  i1 = p0;
  i2 = 72u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B10;}
  i0 = l8;
  i1 = 4294967272u;
  i0 += i1;
  l3 = i0;
  i0 = 0u;
  i1 = l1;
  i0 -= i1;
  l2 = i0;
  L11: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i0 = i32_load((&memory), (u64)(i0));
    j0 = i64_load((&memory), (u64)(i0));
    j1 = p2;
    i0 = j0 == j1;
    if (i0) {goto B10;}
    i0 = l3;
    l8 = i0;
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    l0 = i0;
    l3 = i0;
    i0 = l0;
    i1 = l2;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L11;}
  B10:;
  i0 = p0;
  i1 = 48u;
  i0 += i1;
  l3 = i0;
  i0 = l8;
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B14;}
  i0 = l8;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i1 = 92u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l3;
  i0 = i0 == i1;
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
  if (i0) {goto B13;}
  goto B12;
  B14:;
  i0 = p0;
  i1 = 48u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p0;
  i2 = 56u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 12531438729690087424ull;
  j3 = p2;
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
  if (i0) {goto B12;}
  i0 = l3;
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
  i0 = f61(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 92));
  i1 = l3;
  i0 = i0 == i1;
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
  B13:;
  i0 = p0;
  i1 = 256u;
  i0 += i1;
  l8 = i0;
  i1 = l8;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 1u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = 1u;
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
  i0 = l3;
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
  f62(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B12:;
  i0 = p1;
  i1 = p0;
  i2 = 224u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 -= i1;
  i1 = 121u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
  i0 = p0;
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
  _ZN11eosiosystem15system_contract24update_elected_producersEN5eosio15block_timestampE(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = p0;
  i2 = 296u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 -= i1;
  i1 = 172801u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
  i0 = l9;
  i1 = 56u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l9;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = l9;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = l9;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l5 = j1;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l9;
  j1 = l5;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l9;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0 + 60), i1);
  i0 = l9;
  i1 = l9;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l9;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 64), j1);
  i0 = l9;
  i1 = 8u;
  i0 += i1;
  i1 = l9;
  i2 = 16u;
  i1 += i2;
  i2 = l9;
  i3 = 64u;
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
  f64_0(i0, i1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l9;
  i0 = i32_load((&memory), (u64)(i0 + 12));
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B23;}
  i0 = l3;
  j0 = i64_load((&memory), (u64)(i0 + 16));
  j1 = 1ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B23;}
  i0 = l3;
  j0 = i64_load((&memory), (u64)(i0 + 24));
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
  j1 = (*Z_envZ_current_timeZ_jv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j2 = 18446743987309551616ull;
  j1 += j2;
  i0 = j0 >= j1;
  if (i0) {goto B23;}
  i0 = p0;
  i1 = 272u;
  i0 += i1;
  l3 = i0;
  j0 = i64_load((&memory), (u64)(i0));
  i0 = !(j0);
  if (i0) {goto B23;}
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
  i1 = l3;
  j1 = i64_load((&memory), (u64)(i1));
  j0 -= j1;
  j1 = 1209600000001ull;
  i0 = j0 < j1;
  if (i0) {goto B23;}
  i0 = p0;
  i1 = 296u;
  i0 += i1;
  i1 = p1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l9;
  i0 = i32_load((&memory), (u64)(i0 + 12));
  l3 = i0;
  i1 = 0u;
  i0 = i0 != i1;
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
  i0 = l3;
  i0 = i32_load((&memory), (u64)(i0 + 32));
  i1 = l9;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  l8 = i1;
  i0 = i0 == i1;
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
  i0 = l8;
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
  i0 = l9;
  j1 = 0ull;
  i2 = l3;
  i3 = 16u;
  i2 += i3;
  l0 = i2;
  j2 = i64_load((&memory), (u64)(i2));
  j1 -= j2;
  l5 = j1;
  i64_store((&memory), (u64)(i0 + 112), j1);
  i0 = l0;
  j1 = l5;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l3;
  j0 = i64_load((&memory), (u64)(i0));
  l5 = j0;
  i0 = 1u;
  i1 = 256u;
  i2 = g0;
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
  i0 = l9;
  i1 = l9;
  i2 = 64u;
  i1 += i2;
  i2 = 32u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 104), i1);
  i0 = l9;
  i1 = l9;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 100), i1);
  i0 = l9;
  i1 = l9;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l9;
  i1 = l9;
  i2 = 96u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 120), i1);
  i0 = l9;
  i1 = l3;
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 132), i1);
  i0 = l9;
  i1 = l3;
  i32_store((&memory), (u64)(i0 + 128), i1);
  i0 = l9;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 136), i1);
  i0 = l9;
  i1 = l3;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 140), i1);
  i0 = l9;
  i1 = 128u;
  i0 += i1;
  i1 = l9;
  i2 = 120u;
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
  f65(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l3;
  i0 = i32_load((&memory), (u64)(i0 + 36));
  j1 = 0ull;
  i2 = l9;
  i3 = 64u;
  i2 += i3;
  i3 = 32u;
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
  j0 = l5;
  i1 = l8;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B33;}
  i0 = l8;
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
  B33:;
  i0 = l9;
  j1 = 0ull;
  i2 = l0;
  j2 = i64_load((&memory), (u64)(i2));
  j1 -= j2;
  i64_store((&memory), (u64)(i0 + 128), j1);
  i0 = l9;
  i1 = 112u;
  i0 += i1;
  i1 = l9;
  i2 = 128u;
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
  i0 = memcmp_0(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = !(i0);
  if (i0) {goto B23;}
  i0 = l3;
  i1 = 40u;
  i0 += i1;
  l0 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B35;}
  i0 = l0;
  i1 = l8;
  j1 = i64_load((&memory), (u64)(i1));
  i2 = l8;
  j2 = i64_load((&memory), (u64)(i2 + 8));
  j3 = 11071153799887323136ull;
  i4 = l9;
  i5 = 120u;
  i4 += i5;
  j5 = l5;
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
  i1 = (*Z_envZ_db_idx64_find_primaryZ_ijjjij)(j1, j2, j3, i4, j5);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i1;
  i32_store((&memory), (u64)(i0), i1);
  B35:;
  i0 = l3;
  j1 = 0ull;
  i2 = l9;
  i3 = 128u;
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
  (*Z_envZ_db_idx64_updateZ_viji)(i0, j1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B23:;
  i0 = l9;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  l8 = i0;
  i0 = !(i0);
  if (i0) {goto B7;}
  i0 = l9;
  i1 = 52u;
  i0 += i1;
  p0 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i1 = l8;
  i0 = i0 == i1;
  if (i0) {goto B39;}
  L40: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    l3 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l0 = i0;
    i0 = l3;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l0;
    i0 = !(i0);
    if (i0) {goto B41;}
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
    f235(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B41:;
    i0 = l8;
    i1 = l3;
    i0 = i0 != i1;
    if (i0) {goto L40;}
  i0 = l9;
  i1 = 48u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  goto B38;
  B39:;
  i0 = l8;
  l3 = i0;
  B38:;
  i0 = p0;
  i1 = l8;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B7:;
  i0 = 0u;
  i1 = l9;
  i2 = 144u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f61(u32 p0, u32 p1) {
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
  i32_store((&memory), (u64)(i0 + 40), i1);
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
  i0 = 104u;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l4;
  i1 = 1u;
  i32_store8((&memory), (u64)(i0 + 56), i1);
  i0 = l4;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 60), i1);
  i0 = l4;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = l4;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 68), i1);
  i0 = l4;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 72), i1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = l4;
  i1 = 0u;
  i32_store16((&memory), (u64)(i0 + 88), i1);
  i0 = l4;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 92), i1);
  i0 = l6;
  i1 = 32u;
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
  i0 = f83(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = 4294967295u;
  i32_store((&memory), (u64)(i0 + 100), i1);
  i0 = l4;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 96), i1);
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
  i1 = i32_load((&memory), (u64)(i1 + 96));
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
  if (i0) {goto B15;}
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
  goto B14;
  B15:;
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
  f84(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
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
  i0 = i32_load8_u((&memory), (u64)(i0 + 60));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B17;}
  i0 = l2;
  i1 = 68u;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B17:;
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
  f235(i0);
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

static void f62(u32 p0, u32 p1) {
  u32 l2 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0, l3 = 0;
  f64 l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1, j2, j3, j4, j5;
  f64 d1, d2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 48u;
  i1 -= i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i1 = 92u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p0;
  i0 = i0 == i1;
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
  i0 = p1;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 72));
  i2 = 1u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 72), i1);
  i0 = l5;
  l6 = i0;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  d1 = f64_load((&memory), (u64)(i1));
  l1 = d1;
  d1 = (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(d1);
  d2 = l1;
  i3 = p1;
  i4 = 56u;
  i3 += i4;
  i3 = i32_load8_u((&memory), (u64)(i3));
  d1 = i3 ? d1 : d2;
  f64_store((&memory), (u64)(i0 + 24), d1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = 1u;
  i1 = 256u;
  i2 = g0;
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
  j0 = i64_load32_u((&memory), (u64)(i0 + 16));
  l3 = j0;
  i0 = 64u;
  l4 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l3;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l3 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L4;}
  i0 = p1;
  i1 = 64u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p1;
  i1 = i32_load8_u((&memory), (u64)(i1 + 60));
  l2 = i1;
  i2 = 1u;
  i1 >>= (i2 & 31);
  i2 = l2;
  i3 = 1u;
  i2 &= i3;
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i1 = l4;
  i0 += i1;
  l4 = i0;
  i0 = l2;
  j0 = (u64)(i0);
  l3 = j0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l3;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l3 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L5;}
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
  l5 = i0;
  goto B6;
  B7:;
  i0 = 0u;
  i1 = l5;
  i2 = l4;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B6:;
  i0 = l6;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l6;
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l6;
  i1 = l5;
  i2 = l4;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l6;
  i1 = 8u;
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
  i0 = f81(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 96));
  j1 = 0ull;
  i2 = l5;
  i3 = l4;
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
  i0 = l4;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B11;}
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
  B11:;
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B13;}
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
  B13:;
  i0 = l6;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  d1 = f64_load((&memory), (u64)(i1));
  l1 = d1;
  d1 = (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(d1);
  d2 = l1;
  i3 = p1;
  i4 = 56u;
  i3 += i4;
  i3 = i32_load8_u((&memory), (u64)(i3));
  d1 = i3 ? d1 : d2;
  f64_store((&memory), (u64)(i0 + 40), d1);
  i0 = l6;
  i1 = 24u;
  i0 += i1;
  i1 = l6;
  i2 = 40u;
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
  i0 = memcmp_0(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = !(i0);
  if (i0) {goto B14;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 100));
  l4 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B16;}
  i0 = p1;
  i1 = 100u;
  i0 += i1;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  i2 = p0;
  j2 = i64_load((&memory), (u64)(i2 + 8));
  j3 = 12531438729690087424ull;
  i4 = l6;
  i5 = 32u;
  i4 += i5;
  j5 = l0;
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
  i1 = (*Z_envZ_db_idx_double_find_primaryZ_ijjjij)(j1, j2, j3, i4, j5);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i1;
  i32_store((&memory), (u64)(i0), i1);
  B16:;
  i0 = l4;
  j1 = 0ull;
  i2 = l6;
  i3 = 40u;
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
  (*Z_envZ_db_idx_double_updateZ_viji)(i0, j1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
  i0 = 0u;
  i1 = l6;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void _ZN11eosiosystem15system_contract24update_elected_producersEN5eosio15block_timestampE(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l3 = 0, l4 = 0, l6 = 0;
  u64 l5 = 0;
  f64 l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1;
  f64 d0, d1, d2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 144u;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = 224u;
  i0 += i1;
  i1 = p1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = p0;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 136), i1);
  i0 = l6;
  i1 = 1176u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i1 = f234(i1);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i1;
  i32_store((&memory), (u64)(i0 + 124), i1);
  i0 = l6;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 120), i1);
  i0 = l6;
  i1 = p1;
  i2 = 1176u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 128), i1);
  i0 = l6;
  j1 = 18442240474082181119ull;
  i64_store((&memory), (u64)(i0 + 64), j1);
  i0 = l6;
  i1 = 8u;
  i0 += i1;
  i1 = l6;
  i2 = 136u;
  i1 += i2;
  i2 = l6;
  i3 = 64u;
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
  f69(i0, i1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 12));
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  i0 = l6;
  i1 = 64u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i0 = l6;
  i1 = 64u;
  i0 += i1;
  i1 = 48u;
  i0 += i1;
  l3 = i0;
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i0 = i32_load((&memory), (u64)(i0 + 124));
    l4 = i0;
    i1 = l6;
    i1 = i32_load((&memory), (u64)(i1 + 120));
    i0 -= i1;
    i1 = 56u;
    i0 = I32_DIV_S(i0, i1);
    i1 = 20u;
    i0 = i0 > i1;
    if (i0) {goto B2;}
    i0 = p1;
    d0 = f64_load((&memory), (u64)(i0 + 8));
    l2 = d0;
    d1 = 0;
    i0 = (*Z_eosio_injectionZ__eosio_f64_leZ_idd)(d0, d1);
    d1 = l2;
    d2 = l2;
    i1 = (*Z_eosio_injectionZ__eosio_f64_neZ_idd)(d1, d2);
    i0 |= i1;
    if (i0) {goto B2;}
    i0 = p1;
    i0 = i32_load8_u((&memory), (u64)(i0 + 56));
    i0 = !(i0);
    if (i0) {goto B2;}
    i0 = p1;
    j0 = i64_load((&memory), (u64)(i0));
    l5 = j0;
    i0 = l6;
    i1 = 24u;
    i0 += i1;
    i1 = p1;
    i2 = 16u;
    i1 += i2;
    i2 = 40u;
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
    j1 = l5;
    i64_store((&memory), (u64)(i0 + 64), j1);
    i0 = l1;
    i1 = l6;
    i2 = 24u;
    i1 += i2;
    i2 = 40u;
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
    i1 = i32_load16_u((&memory), (u64)(i1 + 88));
    i32_store16((&memory), (u64)(i0), i1);
    i0 = l4;
    i1 = l6;
    i2 = 120u;
    i1 += i2;
    i2 = 8u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    i0 = i0 >= i1;
    if (i0) {goto B7;}
    i0 = l6;
    i1 = l4;
    i2 = l6;
    i3 = 64u;
    i2 += i3;
    i3 = 56u;
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
    i1 = (*Z_envZ_memcpyZ_iiii)(i1, i2, i3);
    i2 = g0;
    i3 = 1u;
    i2 += i3;
    g0 = i2;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i2 = 56u;
    i1 += i2;
    i32_store((&memory), (u64)(i0 + 124), i1);
    goto B6;
    B7:;
    i0 = l6;
    i1 = 120u;
    i0 += i1;
    i1 = l6;
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
    f70(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B6:;
    i0 = l6;
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
    i0 = f71(i0);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i0 = i32_load((&memory), (u64)(i0 + 12));
    p1 = i0;
    if (i0) {goto L3;}
  B2:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 124));
  l4 = i0;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 120));
  p1 = i1;
  i0 -= i1;
  i1 = 56u;
  i0 = I32_DIV_S(i0, i1);
  i1 = p0;
  i2 = 280u;
  i1 += i2;
  i1 = i32_load16_u((&memory), (u64)(i1));
  i0 = i0 < i1;
  if (i0) {goto B12;}
  i0 = p1;
  i1 = l4;
  i2 = l6;
  i3 = 64u;
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
  f72(i0, i1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 124));
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 120));
  i0 -= i1;
  l4 = i0;
  i1 = 56u;
  i0 = I32_DIV_S(i0, i1);
  p1 = i0;
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B14;}
  i0 = p1;
  i1 = 89478486u;
  i0 = i0 >= i1;
  if (i0) {goto B11;}
  i0 = p1;
  i1 = 48u;
  i0 *= i1;
  p1 = i0;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i1 = p1;
  i0 += i1;
  l0 = i0;
  i0 = l4;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 28));
  i2 = l6;
  i2 = i32_load((&memory), (u64)(i2 + 24));
  p1 = i2;
  i1 -= i2;
  l3 = i1;
  i2 = 4294967248u;
  i1 = I32_DIV_S(i1, i2);
  i2 = 48u;
  i1 *= i2;
  i0 += i1;
  l1 = i0;
  i0 = l3;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B16;}
  i0 = l1;
  i1 = p1;
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
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  p1 = i0;
  B16:;
  i0 = l6;
  i1 = 32u;
  i0 += i1;
  i1 = l0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 28), i1);
  i0 = l6;
  i1 = l1;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = p1;
  i0 = !(i0);
  if (i0) {goto B14;}
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 120));
  p1 = i0;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 124));
  l1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B19;}
  i0 = l6;
  i1 = 32u;
  i0 += i1;
  l3 = i0;
  L20: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i0 = i32_load((&memory), (u64)(i0 + 28));
    l4 = i0;
    i1 = l3;
    i1 = i32_load((&memory), (u64)(i1));
    i0 = i0 == i1;
    if (i0) {goto B22;}
    i0 = l6;
    i1 = l4;
    i2 = p1;
    i3 = 48u;
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
    i1 = (*Z_envZ_memcpyZ_iiii)(i1, i2, i3);
    i2 = g0;
    i3 = 1u;
    i2 += i3;
    g0 = i2;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i2 = 48u;
    i1 += i2;
    i32_store((&memory), (u64)(i0 + 28), i1);
    goto B21;
    B22:;
    i0 = l6;
    i1 = 24u;
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
    f75(i0, i1);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B21:;
    i0 = l1;
    i1 = p1;
    i2 = 56u;
    i1 += i2;
    p1 = i1;
    i0 = i0 != i1;
    if (i0) {goto L20;}
  B19:;
  i0 = l6;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = 0u;
  p1 = i0;
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 28));
  l1 = i0;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 24));
  l4 = i1;
  i0 -= i1;
  i1 = 48u;
  i0 = I32_DIV_S(i0, i1);
  j0 = (u64)(i0);
  l5 = j0;
  L25: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p1;
    i1 = 1u;
    i0 += i1;
    p1 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L25;}
  i0 = l4;
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B28;}
  L29: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p1;
    i1 = 41u;
    i0 += i1;
    p1 = i0;
    i0 = l4;
    j0 = i64_load32_u((&memory), (u64)(i0 + 8));
    l5 = j0;
    L30: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = p1;
      i1 = 1u;
      i0 += i1;
      p1 = i0;
      j0 = l5;
      j1 = 7ull;
      j0 >>= (j1 & 63);
      l5 = j0;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto L30;}
    i0 = l4;
    i1 = 48u;
    i0 += i1;
    l4 = i0;
    i1 = l1;
    i0 = i0 != i1;
    if (i0) {goto L29;}
  i0 = p1;
  i0 = !(i0);
  if (i0) {goto B27;}
  B28:;
  i0 = l6;
  i1 = 8u;
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
  f73(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 12));
  l4 = i0;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  p1 = i0;
  goto B26;
  B27:;
  i0 = 0u;
  l4 = i0;
  i0 = 0u;
  p1 = i0;
  B26:;
  i0 = l6;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 68), i1);
  i0 = l6;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 72), i1);
  i0 = l6;
  i1 = 64u;
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
  i0 = f74(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  p1 = i0;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 12));
  i2 = p1;
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
  j0 = (*Z_envZ_set_proposed_producersZ_jii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j1 = 18446744073709551615ull;
  i0 = (u64)((s64)j0 <= (s64)j1);
  if (i0) {goto B33;}
  i0 = p0;
  i1 = 280u;
  i0 += i1;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 124));
  i2 = l6;
  i2 = i32_load((&memory), (u64)(i2 + 120));
  i1 -= i2;
  i2 = 56u;
  i1 = I32_DIV_S(i1, i2);
  i32_store16((&memory), (u64)(i0), i1);
  B33:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B35;}
  i0 = l6;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 12), i1);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B35:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  p1 = i0;
  i0 = !(i0);
  if (i0) {goto B37;}
  i0 = l6;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 28), i1);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B37:;
  i0 = l6;
  i0 = i32_load((&memory), (u64)(i0 + 120));
  p1 = i0;
  B12:;
  i0 = p1;
  i0 = !(i0);
  if (i0) {goto B39;}
  i0 = l6;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 124), i1);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B39:;
  i0 = 0u;
  i1 = l6;
  i2 = 144u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  goto Bfunc;
  B11:;
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

static void f64_0(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  u64 l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l7 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l7;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l7;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = 0u;
  p2 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = l5;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 11071153799887323136ull;
  i3 = l7;
  i4 = l7;
  i5 = 8u;
  i4 += i5;
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
  i0 = (*Z_envZ_db_idx64_lowerboundZ_ijjjii)(j0, j1, j2, i3, i4);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B0;}
  i0 = l7;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l2 = j0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 24));
  l3 = i1;
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = l6;
  i1 = 4294967272u;
  i0 += i1;
  p2 = i0;
  i0 = 0u;
  i1 = l3;
  i0 -= i1;
  l4 = i0;
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i0 = i32_load((&memory), (u64)(i0));
    j0 = i64_load((&memory), (u64)(i0));
    j1 = l2;
    i0 = j0 == j1;
    if (i0) {goto B2;}
    i0 = p2;
    l6 = i0;
    i0 = p2;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    p2 = i0;
    i0 = l5;
    i1 = l4;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L3;}
  B2:;
  i0 = l6;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  i0 = l6;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p2 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 32));
  i1 = l1;
  i0 = i0 == i1;
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
  goto B4;
  B5:;
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&memory), (u64)(i1));
  i2 = l1;
  j2 = i64_load((&memory), (u64)(i2 + 8));
  j3 = 11071153799887323136ull;
  j4 = l2;
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
  i1 = (*Z_envZ_db_find_i64Z_ijjjj)(j1, j2, j3, j4);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f66(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 32));
  i1 = l1;
  i0 = i0 == i1;
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
  B4:;
  i0 = p2;
  i1 = 40u;
  i0 += i1;
  i1 = l0;
  i32_store((&memory), (u64)(i0), i1);
  B0:;
  i0 = p0;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = p1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = 0u;
  i1 = l7;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f65(u32 p0, u32 p1) {
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
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i1 = 320u;
  i2 = g0;
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
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
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
  i1 = 320u;
  i2 = g0;
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
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 12));
  l0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f66(u32 p0, u32 p1) {
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l4;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l6;
  i1 = l6;
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l6;
  i1 = l4;
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l6;
  i1 = l4;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l6;
  i1 = l4;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 44), i1);
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
  f67(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = 4294967295u;
  i32_store((&memory), (u64)(i0 + 40), i1);
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
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l6;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1 + 36));
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
  if (i0) {goto B15;}
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
  goto B14;
  B15:;
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
  f68(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
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
  f235(i0);
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

static void f67(u32 p0, u32 p1) {
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
  i1 = 368u;
  i2 = g0;
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
  i1 = 368u;
  i2 = g0;
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
  i0 = i32_load((&memory), (u64)(i0 + 8));
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
  i1 = 368u;
  i2 = g0;
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
  i0 = i32_load((&memory), (u64)(i0 + 12));
  l0 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  p0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f68(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f234(i0);
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
  f241(i0);
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
    f235(i0);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static void f69(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  u64 l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l7 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l7;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l7;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = 0u;
  p2 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = l5;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 12531438729690087424ull;
  i3 = l7;
  i4 = l7;
  i5 = 8u;
  i4 += i5;
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
  i0 = (*Z_envZ_db_idx_double_lowerboundZ_ijjjii)(j0, j1, j2, i3, i4);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B0;}
  i0 = l7;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l2 = j0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 24));
  l3 = i1;
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = l6;
  i1 = 4294967272u;
  i0 += i1;
  p2 = i0;
  i0 = 0u;
  i1 = l3;
  i0 -= i1;
  l4 = i0;
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i0 = i32_load((&memory), (u64)(i0));
    j0 = i64_load((&memory), (u64)(i0));
    j1 = l2;
    i0 = j0 == j1;
    if (i0) {goto B2;}
    i0 = p2;
    l6 = i0;
    i0 = p2;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    p2 = i0;
    i0 = l5;
    i1 = l4;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L3;}
  B2:;
  i0 = l6;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  i0 = l6;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p2 = i0;
  i1 = 92u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l1;
  i0 = i0 == i1;
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
  goto B4;
  B5:;
  i0 = 0u;
  p2 = i0;
  i0 = l1;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = l1;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 12531438729690087424ull;
  j3 = l2;
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
  if (i0) {goto B4;}
  i0 = l1;
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
  i0 = f61(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 92));
  i1 = l1;
  i0 = i0 == i1;
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
  B4:;
  i0 = p2;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 100), i1);
  B0:;
  i0 = p0;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = p1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = 0u;
  i1 = l7;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f70(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l3 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  l2 = i1;
  i0 -= i1;
  i1 = 56u;
  i0 = I32_DIV_S(i0, i1);
  l0 = i0;
  i1 = 1u;
  i0 += i1;
  l5 = i0;
  i1 = 76695845u;
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  i0 = 76695844u;
  l4 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l2;
  i0 -= i1;
  i1 = 56u;
  i0 = I32_DIV_S(i0, i1);
  l1 = i0;
  i1 = 38347921u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = l5;
  i1 = l1;
  i2 = 1u;
  i1 <<= (i2 & 31);
  l4 = i1;
  i2 = l4;
  i3 = l5;
  i2 = i2 < i3;
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  B3:;
  i0 = l4;
  i1 = 56u;
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
  i0 = f234(i0);
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
  l3 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  goto B0;
  B2:;
  i0 = 0u;
  l4 = i0;
  i0 = 0u;
  l5 = i0;
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
  f241(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B0:;
  i0 = l5;
  i1 = l0;
  i2 = 56u;
  i1 *= i2;
  i0 += i1;
  i1 = p1;
  i2 = 56u;
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
  l0 = i0;
  i1 = l3;
  i2 = l2;
  i1 -= i2;
  l3 = i1;
  i2 = 4294967240u;
  i1 = I32_DIV_S(i1, i2);
  i2 = 56u;
  i1 *= i2;
  i0 += i1;
  p1 = i0;
  i0 = l5;
  i1 = l4;
  i2 = 56u;
  i1 *= i2;
  i0 += i1;
  l4 = i0;
  i0 = l0;
  i1 = 56u;
  i0 += i1;
  l5 = i0;
  i0 = l3;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B7;}
  i0 = p1;
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
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  B7:;
  i0 = p0;
  i1 = p1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i1 = l5;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B9:;
  FUNC_EPILOGUE;
}

static u32 f71(u32 p0) {
  u32 l0 = 0, l1 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0, l8 = 0;
  u64 l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l8 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 0u;
  l6 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 0u;
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
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l7 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 100));
  l5 = i0;
  i1 = 4294967295u;
  i0 = i0 != i1;
  if (i0) {goto B1;}
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = l5;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 12531438729690087424ull;
  i3 = l8;
  i4 = 8u;
  i3 += i4;
  i4 = l7;
  j4 = i64_load((&memory), (u64)(i4));
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
  i0 = (*Z_envZ_db_idx_double_find_primaryZ_ijjjij)(j0, j1, j2, i3, j4);
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
  i1 = l5;
  i32_store((&memory), (u64)(i0 + 100), i1);
  B1:;
  i0 = l8;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l5;
  i1 = l8;
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
  i0 = (*Z_envZ_db_idx_double_nextZ_iii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B3;}
  i0 = l8;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l2 = j0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l7 = i0;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1 + 24));
  l3 = i1;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  i0 = l7;
  i1 = 4294967272u;
  i0 += i1;
  l6 = i0;
  i0 = 0u;
  i1 = l3;
  i0 -= i1;
  l4 = i0;
  L6: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i0 = i32_load((&memory), (u64)(i0));
    j0 = i64_load((&memory), (u64)(i0));
    j1 = l2;
    i0 = j0 == j1;
    if (i0) {goto B5;}
    i0 = l6;
    l7 = i0;
    i0 = l6;
    i1 = 4294967272u;
    i0 += i1;
    l5 = i0;
    l6 = i0;
    i0 = l5;
    i1 = l4;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L6;}
  B5:;
  i0 = l7;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B8;}
  i0 = l7;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  i1 = 92u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l1;
  i0 = i0 == i1;
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
  goto B7;
  B8:;
  i0 = l1;
  i1 = l1;
  j1 = i64_load((&memory), (u64)(i1));
  i2 = l1;
  j2 = i64_load((&memory), (u64)(i2 + 8));
  j3 = 12531438729690087424ull;
  j4 = l2;
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
  i1 = (*Z_envZ_db_find_i64Z_ijjjj)(j1, j2, j3, j4);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i2 = g0;
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
  l6 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 92));
  i1 = l1;
  i0 = i0 == i1;
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
  B7:;
  i0 = l6;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 100), i1);
  B3:;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i1 = l6;
  i32_store((&memory), (u64)(i0), i1);
  i0 = 0u;
  i1 = l8;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f72(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l8 = 0, l10 = 0, 
      l11 = 0, l12 = 0, l13 = 0, l14 = 0;
  u64 l6 = 0, l7 = 0, l9 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 48u;
  i1 -= i2;
  l14 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = 0u;
    i1 = p1;
    i0 -= i1;
    l4 = i0;
    i0 = p1;
    i1 = 4294967288u;
    i0 += i1;
    l3 = i0;
    i0 = p1;
    i1 = 4294967184u;
    i0 += i1;
    l2 = i0;
    i0 = p1;
    i1 = 4294967240u;
    i0 += i1;
    l1 = i0;
    L3: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = p0;
      l5 = i0;
      L6: 
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        i0 = p1;
        i1 = l5;
        i0 -= i1;
        l8 = i0;
        i1 = 56u;
        i0 = I32_DIV_S(i0, i1);
        p0 = i0;
        i1 = 5u;
        i0 = i0 <= i1;
        if (i0) {goto B9;}
        i0 = l8;
        i1 = 391u;
        i0 = (u32)((s32)i0 <= (s32)i1);
        if (i0) {goto B10;}
        i0 = l5;
        i1 = l8;
        i2 = 112u;
        i1 = DIV_U(i1, i2);
        i2 = 56u;
        i1 *= i2;
        p0 = i1;
        i0 += i1;
        l10 = i0;
        i0 = l8;
        i1 = 55945u;
        i0 = (u32)((s32)i0 < (s32)i1);
        if (i0) {goto B12;}
        i0 = l5;
        i1 = l5;
        i2 = l8;
        i3 = 224u;
        i2 = DIV_U(i2, i3);
        i3 = 56u;
        i2 *= i3;
        l8 = i2;
        i1 += i2;
        i2 = l10;
        i3 = l10;
        i4 = l8;
        i3 += i4;
        i4 = l1;
        i5 = p2;
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
        i0 = f79(i0, i1, i2, i3, i4, i5);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        l12 = i0;
        goto B11;
        B12:;
        i0 = l5;
        i1 = l10;
        i2 = l1;
        i3 = p2;
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
        i0 = f77(i0, i1, i2, i3);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        l12 = i0;
        B11:;
        i0 = l5;
        j0 = i64_load((&memory), (u64)(i0));
        l6 = j0;
        i1 = l10;
        j1 = i64_load((&memory), (u64)(i1));
        l7 = j1;
        i0 = j0 < j1;
        if (i0) {goto B5;}
        j0 = l7;
        j1 = l6;
        i0 = j0 < j1;
        if (i0) {goto B15;}
        i0 = l5;
        i0 = i32_load16_u((&memory), (u64)(i0 + 48));
        i1 = l5;
        i2 = p0;
        i1 += i2;
        i1 = i32_load16_u((&memory), (u64)(i1 + 48));
        i0 = i0 < i1;
        if (i0) {goto B5;}
        B15:;
        i0 = l5;
        i1 = l2;
        i0 = i0 == i1;
        if (i0) {goto B16;}
        i0 = l5;
        i1 = p0;
        i0 += i1;
        i1 = 48u;
        i0 += i1;
        l8 = i0;
        i0 = l2;
        l13 = i0;
        L17: 
          (*Z_eosio_injectionZ_checktimeZ_vv)();
          i0 = l13;
          j0 = i64_load((&memory), (u64)(i0));
          l9 = j0;
          j1 = l7;
          i0 = j0 < j1;
          if (i0) {goto B8;}
          j0 = l7;
          j1 = l9;
          i0 = j0 < j1;
          if (i0) {goto B18;}
          i0 = l13;
          i1 = 48u;
          i0 += i1;
          i0 = i32_load16_u((&memory), (u64)(i0));
          i1 = l8;
          i1 = i32_load16_u((&memory), (u64)(i1));
          i0 = i0 < i1;
          if (i0) {goto B8;}
          B18:;
          i0 = l5;
          i1 = l13;
          i2 = 4294967240u;
          i1 += i2;
          l13 = i1;
          i0 = i0 != i1;
          if (i0) {goto L17;}
        B16:;
        i0 = l5;
        i1 = 56u;
        i0 += i1;
        p0 = i0;
        j0 = l6;
        i1 = l1;
        j1 = i64_load((&memory), (u64)(i1));
        l7 = j1;
        i0 = j0 < j1;
        if (i0) {goto B19;}
        j0 = l7;
        j1 = l6;
        i0 = j0 < j1;
        if (i0) {goto B20;}
        i0 = l5;
        i0 = i32_load16_u((&memory), (u64)(i0 + 48));
        i1 = l3;
        i1 = i32_load16_u((&memory), (u64)(i1));
        i0 = i0 < i1;
        if (i0) {goto B19;}
        B20:;
        i0 = p0;
        i1 = l1;
        i0 = i0 == i1;
        if (i0) {goto B2;}
        L22: 
          (*Z_eosio_injectionZ_checktimeZ_vv)();
          j0 = l6;
          i1 = p0;
          j1 = i64_load((&memory), (u64)(i1));
          l7 = j1;
          i0 = j0 < j1;
          if (i0) {goto B21;}
          j0 = l7;
          j1 = l6;
          i0 = j0 < j1;
          if (i0) {goto B23;}
          i0 = l5;
          i1 = 48u;
          i0 += i1;
          i0 = i32_load16_u((&memory), (u64)(i0));
          i1 = p0;
          i2 = 48u;
          i1 += i2;
          i1 = i32_load16_u((&memory), (u64)(i1));
          i0 = i0 < i1;
          if (i0) {goto B21;}
          B23:;
          i0 = p0;
          i1 = 56u;
          i0 += i1;
          p0 = i0;
          i1 = l4;
          i0 += i1;
          i1 = 4294967240u;
          i0 = i0 != i1;
          if (i0) {goto L22;}
          goto B2;
        B21:;
        i0 = l14;
        i1 = p0;
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
        i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        l10 = i0;
        i0 = p0;
        i1 = l1;
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
        i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        l8 = i0;
        i0 = l1;
        i1 = l10;
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
        i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        i0 = l8;
        i1 = 48u;
        i0 += i1;
        p0 = i0;
        i0 = i32_load16_u((&memory), (u64)(i0));
        l10 = i0;
        i0 = p0;
        i1 = l3;
        i1 = i32_load16_u((&memory), (u64)(i1));
        i32_store16((&memory), (u64)(i0), i1);
        i0 = l3;
        i1 = l10;
        i32_store16((&memory), (u64)(i0), i1);
        i0 = l8;
        i1 = 56u;
        i0 += i1;
        p0 = i0;
        B19:;
        i0 = p0;
        i1 = l1;
        i0 = i0 == i1;
        if (i0) {goto B2;}
        i0 = l1;
        l8 = i0;
        L27: 
          (*Z_eosio_injectionZ_checktimeZ_vv)();
          i0 = l5;
          j0 = i64_load((&memory), (u64)(i0));
          l7 = j0;
          i1 = p0;
          j1 = i64_load((&memory), (u64)(i1));
          l9 = j1;
          i0 = j0 < j1;
          if (i0) {goto B28;}
          L29: 
            (*Z_eosio_injectionZ_checktimeZ_vv)();
            j0 = l9;
            j1 = l7;
            i0 = j0 < j1;
            if (i0) {goto B30;}
            i0 = l5;
            i1 = 48u;
            i0 += i1;
            i0 = i32_load16_u((&memory), (u64)(i0));
            i1 = p0;
            i2 = 48u;
            i1 += i2;
            i1 = i32_load16_u((&memory), (u64)(i1));
            i0 = i0 < i1;
            if (i0) {goto B28;}
            B30:;
            j0 = l7;
            i1 = p0;
            i2 = 56u;
            i1 += i2;
            p0 = i1;
            j1 = i64_load((&memory), (u64)(i1));
            l9 = j1;
            i0 = j0 >= j1;
            if (i0) {goto L29;}
          B28:;
          L31: 
            (*Z_eosio_injectionZ_checktimeZ_vv)();
            j0 = l7;
            i1 = l8;
            i2 = 4294967240u;
            i1 += i2;
            l8 = i1;
            j1 = i64_load((&memory), (u64)(i1));
            l9 = j1;
            i0 = j0 < j1;
            if (i0) {goto L31;}
            j0 = l9;
            j1 = l7;
            i0 = j0 < j1;
            if (i0) {goto B32;}
            i0 = l5;
            i1 = 48u;
            i0 += i1;
            i0 = i32_load16_u((&memory), (u64)(i0));
            i1 = l8;
            i2 = 48u;
            i1 += i2;
            i1 = i32_load16_u((&memory), (u64)(i1));
            i0 = i0 < i1;
            if (i0) {goto L31;}
            B32:;
          i0 = p0;
          i1 = l8;
          i0 = i0 >= i1;
          if (i0) {goto B7;}
          i0 = l14;
          i1 = p0;
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
          i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
          i1 = g0;
          i2 = 1u;
          i1 += i2;
          g0 = i1;
          (*Z_eosio_injectionZ_checktimeZ_vv)();
          l10 = i0;
          i0 = p0;
          i1 = l8;
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
          i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
          i1 = g0;
          i2 = 1u;
          i1 += i2;
          g0 = i1;
          (*Z_eosio_injectionZ_checktimeZ_vv)();
          i0 = l8;
          i1 = l10;
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
          i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
          i1 = g0;
          i2 = 1u;
          i1 += i2;
          g0 = i1;
          (*Z_eosio_injectionZ_checktimeZ_vv)();
          l10 = i0;
          i0 = p0;
          i0 = i32_load16_u((&memory), (u64)(i0 + 48));
          l13 = i0;
          i0 = p0;
          i1 = l10;
          i2 = 48u;
          i1 += i2;
          l10 = i1;
          i1 = i32_load16_u((&memory), (u64)(i1));
          i32_store16((&memory), (u64)(i0 + 48), i1);
          i0 = l10;
          i1 = l13;
          i32_store16((&memory), (u64)(i0), i1);
          i0 = p0;
          i1 = 56u;
          i0 += i1;
          p0 = i0;
          goto L27;
        B10:;
        i0 = l5;
        i1 = l5;
        i2 = 56u;
        i1 += i2;
        i2 = l5;
        i3 = 112u;
        i2 += i3;
        l11 = i2;
        i3 = p2;
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
        i0 = f77(i0, i1, i2, i3);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        i0 = l5;
        i1 = 168u;
        i0 += i1;
        l8 = i0;
        i1 = p1;
        i0 = i0 == i1;
        if (i0) {goto B2;}
        i0 = 0u;
        l4 = i0;
        L37: 
          (*Z_eosio_injectionZ_checktimeZ_vv)();
          i0 = l11;
          p0 = i0;
          i0 = l8;
          l11 = i0;
          j0 = i64_load((&memory), (u64)(i0));
          l7 = j0;
          i1 = p0;
          j1 = i64_load((&memory), (u64)(i1));
          l9 = j1;
          i0 = j0 >= j1;
          if (i0) {goto B40;}
          i0 = p0;
          i1 = 48u;
          i0 += i1;
          l0 = i0;
          i0 = l11;
          i0 = i32_load16_u((&memory), (u64)(i0 + 48));
          l1 = i0;
          goto B39;
          B40:;
          j0 = l9;
          j1 = l7;
          i0 = j0 < j1;
          if (i0) {goto B38;}
          i0 = l11;
          i0 = i32_load16_u((&memory), (u64)(i0 + 48));
          l1 = i0;
          i1 = p0;
          i1 = i32_load16_u((&memory), (u64)(i1 + 48));
          i0 = i0 >= i1;
          if (i0) {goto B38;}
          i0 = p0;
          i1 = 48u;
          i0 += i1;
          l0 = i0;
          B39:;
          i0 = l14;
          i1 = l11;
          i2 = 8u;
          i1 += i2;
          i2 = 40u;
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
          p2 = i0;
          i0 = l11;
          i1 = p0;
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
          i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
          i1 = g0;
          i2 = 1u;
          i1 += i2;
          g0 = i1;
          (*Z_eosio_injectionZ_checktimeZ_vv)();
          i1 = 48u;
          i0 += i1;
          i1 = p0;
          i2 = 48u;
          i1 += i2;
          i1 = i32_load16_u((&memory), (u64)(i1));
          i32_store16((&memory), (u64)(i0), i1);
          i0 = p0;
          i1 = l5;
          i0 = i0 == i1;
          if (i0) {goto B45;}
          i0 = l4;
          l10 = i0;
          L46: 
            (*Z_eosio_injectionZ_checktimeZ_vv)();
            j0 = l7;
            i1 = l5;
            i2 = l10;
            i1 += i2;
            l8 = i1;
            i2 = 56u;
            i1 += i2;
            l13 = i1;
            j1 = i64_load((&memory), (u64)(i1));
            l9 = j1;
            i0 = j0 >= j1;
            if (i0) {goto B48;}
            i0 = l8;
            i1 = 104u;
            i0 += i1;
            l0 = i0;
            i0 = i32_load16_u((&memory), (u64)(i0));
            l12 = i0;
            goto B47;
            B48:;
            j0 = l9;
            j1 = l7;
            i0 = j0 < j1;
            if (i0) {goto B43;}
            i0 = l1;
            i1 = 65535u;
            i0 &= i1;
            i1 = l8;
            i2 = 104u;
            i1 += i2;
            i1 = i32_load16_u((&memory), (u64)(i1));
            l12 = i1;
            i0 = i0 >= i1;
            if (i0) {goto B44;}
            i0 = p0;
            i1 = 4294967288u;
            i0 += i1;
            l0 = i0;
            B47:;
            i0 = p0;
            i1 = 4294967240u;
            i0 += i1;
            p0 = i0;
            i0 = l8;
            i1 = 112u;
            i0 += i1;
            i1 = l13;
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
            i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
            i1 = g0;
            i2 = 1u;
            i1 += i2;
            g0 = i1;
            (*Z_eosio_injectionZ_checktimeZ_vv)();
            i0 = l8;
            i1 = 160u;
            i0 += i1;
            i1 = l12;
            i32_store16((&memory), (u64)(i0), i1);
            i0 = l10;
            i1 = 4294967240u;
            i0 += i1;
            l10 = i0;
            i1 = 4294967184u;
            i0 = i0 != i1;
            if (i0) {goto L46;}
          i0 = l5;
          p0 = i0;
          goto B43;
          B45:;
          i0 = l5;
          p0 = i0;
          goto B43;
          B44:;
          i0 = l8;
          i1 = 112u;
          i0 += i1;
          p0 = i0;
          B43:;
          i0 = p0;
          j1 = l7;
          i64_store((&memory), (u64)(i0), j1);
          i0 = p0;
          i1 = 8u;
          i0 += i1;
          i1 = p2;
          i2 = 40u;
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
          i1 = l1;
          i32_store16((&memory), (u64)(i0), i1);
          B38:;
          i0 = l4;
          i1 = 56u;
          i0 += i1;
          l4 = i0;
          i0 = l11;
          i1 = 56u;
          i0 += i1;
          l8 = i0;
          i1 = p1;
          i0 = i0 != i1;
          if (i0) {goto L37;}
          goto B2;
        B9:;
        i0 = p0;
        switch (i0) {
          case 0: goto B2;
          case 1: goto B2;
          case 2: goto B54;
          case 3: goto B53;
          case 4: goto B52;
          case 5: goto B51;
          default: goto B2;
        }
        B54:;
        i0 = l1;
        j0 = i64_load((&memory), (u64)(i0));
        l7 = j0;
        i1 = l5;
        j1 = i64_load((&memory), (u64)(i1));
        l9 = j1;
        i0 = j0 < j1;
        if (i0) {goto B55;}
        j0 = l9;
        j1 = l7;
        i0 = j0 < j1;
        if (i0) {goto B2;}
        i0 = l3;
        i0 = i32_load16_u((&memory), (u64)(i0));
        i1 = l5;
        i1 = i32_load16_u((&memory), (u64)(i1 + 48));
        i0 = i0 >= i1;
        if (i0) {goto B2;}
        B55:;
        i0 = l14;
        i1 = l5;
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
        i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        l8 = i0;
        i0 = l5;
        i1 = l1;
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
        i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        p0 = i0;
        i0 = l1;
        i1 = l8;
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
        i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        i0 = p0;
        i1 = 48u;
        i0 += i1;
        l8 = i0;
        i0 = i32_load16_u((&memory), (u64)(i0));
        p0 = i0;
        i0 = l8;
        i1 = l3;
        i1 = i32_load16_u((&memory), (u64)(i1));
        i32_store16((&memory), (u64)(i0), i1);
        i0 = l3;
        i1 = p0;
        i32_store16((&memory), (u64)(i0), i1);
        goto B2;
        B53:;
        i0 = l5;
        i1 = l5;
        i2 = 56u;
        i1 += i2;
        i2 = l1;
        i3 = p2;
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
        i0 = f77(i0, i1, i2, i3);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        goto B2;
        B52:;
        i0 = l5;
        i1 = l5;
        i2 = 56u;
        i1 += i2;
        i2 = l5;
        i3 = 112u;
        i2 += i3;
        i3 = l1;
        i4 = p2;
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
        i0 = f78(i0, i1, i2, i3, i4);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        goto B2;
        B51:;
        i0 = l5;
        i1 = l5;
        i2 = 56u;
        i1 += i2;
        i2 = l5;
        i3 = 112u;
        i2 += i3;
        i3 = l5;
        i4 = 168u;
        i3 += i4;
        i4 = l1;
        i5 = p2;
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
        i0 = f79(i0, i1, i2, i3, i4, i5);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        goto B2;
        B8:;
        i0 = l14;
        i1 = l5;
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
        i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        p0 = i0;
        i0 = l5;
        i1 = l13;
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
        i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        l8 = i0;
        i0 = l13;
        i1 = p0;
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
        i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        p0 = i0;
        i0 = l8;
        i0 = i32_load16_u((&memory), (u64)(i0 + 48));
        l0 = i0;
        i0 = l8;
        i1 = p0;
        i2 = 48u;
        i1 += i2;
        p0 = i1;
        i1 = i32_load16_u((&memory), (u64)(i1));
        i32_store16((&memory), (u64)(i0 + 48), i1);
        i0 = p0;
        i1 = l0;
        i32_store16((&memory), (u64)(i0), i1);
        i0 = l12;
        i1 = 1u;
        i0 += i1;
        l12 = i0;
        goto B4;
        B7:;
        i0 = p0;
        l5 = i0;
        goto L6;
      B5:;
      i0 = l1;
      l13 = i0;
      B4:;
      i0 = l5;
      i1 = 56u;
      i0 += i1;
      l8 = i0;
      i1 = l13;
      i0 = i0 >= i1;
      if (i0) {goto B65;}
      L66: 
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        i0 = l10;
        j0 = i64_load((&memory), (u64)(i0));
        l7 = j0;
        L68: 
          (*Z_eosio_injectionZ_checktimeZ_vv)();
          i0 = l8;
          j0 = i64_load((&memory), (u64)(i0));
          l9 = j0;
          j1 = l7;
          i0 = j0 < j1;
          if (i0) {goto B69;}
          j0 = l7;
          j1 = l9;
          i0 = j0 < j1;
          if (i0) {goto B67;}
          i0 = l8;
          i1 = 48u;
          i0 += i1;
          i0 = i32_load16_u((&memory), (u64)(i0));
          i1 = l10;
          i2 = 48u;
          i1 += i2;
          i1 = i32_load16_u((&memory), (u64)(i1));
          i0 = i0 >= i1;
          if (i0) {goto B67;}
          B69:;
          i0 = l8;
          i1 = 56u;
          i0 += i1;
          l8 = i0;
          goto L68;
        B67:;
        i0 = l13;
        i1 = 4294967240u;
        i0 += i1;
        p0 = i0;
        j0 = i64_load((&memory), (u64)(i0));
        l9 = j0;
        j1 = l7;
        i0 = j0 < j1;
        if (i0) {goto B70;}
        L72: 
          (*Z_eosio_injectionZ_checktimeZ_vv)();
          j0 = l7;
          j1 = l9;
          i0 = j0 < j1;
          if (i0) {goto B73;}
          i0 = p0;
          i1 = 48u;
          i0 += i1;
          i0 = i32_load16_u((&memory), (u64)(i0));
          i1 = l10;
          i2 = 48u;
          i1 += i2;
          i1 = i32_load16_u((&memory), (u64)(i1));
          i0 = i0 < i1;
          if (i0) {goto B71;}
          B73:;
          i0 = p0;
          i1 = 4294967240u;
          i0 += i1;
          p0 = i0;
          j0 = i64_load((&memory), (u64)(i0));
          l9 = j0;
          j1 = l7;
          i0 = j0 >= j1;
          if (i0) {goto L72;}
        B71:;
        i0 = p0;
        i1 = 56u;
        i0 += i1;
        l13 = i0;
        B70:;
        i0 = l8;
        i1 = p0;
        i0 = i0 > i1;
        if (i0) {goto B65;}
        i0 = l14;
        i1 = l8;
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
        i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        l0 = i0;
        i0 = l8;
        i1 = p0;
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
        i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        i0 = p0;
        i1 = l0;
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
        i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
        i1 = g0;
        i2 = 1u;
        i1 += i2;
        g0 = i1;
        (*Z_eosio_injectionZ_checktimeZ_vv)();
        i0 = l8;
        i1 = 48u;
        i0 += i1;
        l0 = i0;
        i0 = i32_load16_u((&memory), (u64)(i0));
        l11 = i0;
        i0 = l0;
        i1 = l13;
        i2 = 4294967288u;
        i1 += i2;
        l13 = i1;
        i1 = i32_load16_u((&memory), (u64)(i1));
        i32_store16((&memory), (u64)(i0), i1);
        i0 = l13;
        i1 = l11;
        i32_store16((&memory), (u64)(i0), i1);
        i0 = p0;
        i1 = l10;
        i2 = l10;
        i3 = l8;
        i2 = i2 == i3;
        i0 = i2 ? i0 : i1;
        l10 = i0;
        i0 = l8;
        i1 = 56u;
        i0 += i1;
        l8 = i0;
        i0 = l12;
        i1 = 1u;
        i0 += i1;
        l12 = i0;
        i0 = p0;
        l13 = i0;
        goto L66;
      B65:;
      i0 = l8;
      i1 = l10;
      i0 = i0 == i1;
      if (i0) {goto B77;}
      i0 = l10;
      j0 = i64_load((&memory), (u64)(i0));
      l7 = j0;
      i1 = l8;
      j1 = i64_load((&memory), (u64)(i1));
      l9 = j1;
      i0 = j0 < j1;
      if (i0) {goto B78;}
      j0 = l9;
      j1 = l7;
      i0 = j0 < j1;
      if (i0) {goto B77;}
      i0 = l10;
      i0 = i32_load16_u((&memory), (u64)(i0 + 48));
      i1 = l8;
      i1 = i32_load16_u((&memory), (u64)(i1 + 48));
      i0 = i0 >= i1;
      if (i0) {goto B77;}
      B78:;
      i0 = l14;
      i1 = l8;
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
      i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
      i1 = g0;
      i2 = 1u;
      i1 += i2;
      g0 = i1;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      p0 = i0;
      i0 = l8;
      i1 = l10;
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
      i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
      i1 = g0;
      i2 = 1u;
      i1 += i2;
      g0 = i1;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      l13 = i0;
      i0 = l10;
      i1 = p0;
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
      i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
      i1 = g0;
      i2 = 1u;
      i1 += i2;
      g0 = i1;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      p0 = i0;
      i0 = l13;
      i1 = 48u;
      i0 += i1;
      l10 = i0;
      i0 = i32_load16_u((&memory), (u64)(i0));
      l13 = i0;
      i0 = l10;
      i1 = p0;
      i2 = 48u;
      i1 += i2;
      p0 = i1;
      i1 = i32_load16_u((&memory), (u64)(i1));
      i32_store16((&memory), (u64)(i0), i1);
      i0 = p0;
      i1 = l13;
      i32_store16((&memory), (u64)(i0), i1);
      i0 = l12;
      i1 = 1u;
      i0 += i1;
      l12 = i0;
      B77:;
      i0 = l12;
      if (i0) {goto B83;}
      i0 = l5;
      i1 = l8;
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
      i0 = f80(i0, i1, i2);
      i1 = g0;
      i2 = 1u;
      i1 += i2;
      g0 = i1;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      l10 = i0;
      i0 = l8;
      i1 = 56u;
      i0 += i1;
      p0 = i0;
      i1 = p1;
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
      i0 = f80(i0, i1, i2);
      i1 = g0;
      i2 = 1u;
      i1 += i2;
      g0 = i1;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      if (i0) {goto B82;}
      i0 = l10;
      if (i0) {goto L3;}
      B83:;
      i0 = l8;
      i1 = l5;
      i0 -= i1;
      i1 = p1;
      i2 = l8;
      i1 -= i2;
      i0 = (u32)((s32)i0 >= (s32)i1);
      if (i0) {goto B1;}
      i0 = l5;
      i1 = l8;
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
      f72(i0, i1, i2);
      i0 = g0;
      i1 = 1u;
      i0 += i1;
      g0 = i0;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l8;
      i1 = 56u;
      i0 += i1;
      p0 = i0;
      goto L3;
      B82:;
    i0 = l8;
    p1 = i0;
    i0 = l5;
    p0 = i0;
    i0 = l10;
    i0 = !(i0);
    if (i0) {goto L0;}
    B2:;
    i0 = 0u;
    i1 = l14;
    i2 = 48u;
    i1 += i2;
    i32_store((&memory), (u64)(i0 + 4), i1);
    goto Bfunc;
    B1:;
    i0 = l8;
    i1 = 56u;
    i0 += i1;
    i1 = p1;
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
    f72(i0, i1, i2);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l8;
    p1 = i0;
    i0 = l5;
    p0 = i0;
    goto L0;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f73(u32 p0, u32 p1) {
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
  i0 = f234(i0);
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
  f241(i0);
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
  f235(i0);
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

static u32 f74(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l5 = 0, l6 = 0;
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
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1));
  i0 -= i1;
  i1 = 48u;
  i0 = I32_DIV_S(i0, i1);
  j0 = (u64)(i0);
  l4 = j0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l5 = i0;
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
    j0 = l4;
    i0 = (u32)(j0);
    l3 = i0;
    i0 = l6;
    j1 = l4;
    j2 = 7ull;
    j1 >>= (j2 & 63);
    l4 = j1;
    j2 = 0ull;
    i1 = j1 != j2;
    l0 = i1;
    i2 = 7u;
    i1 <<= (i2 & 31);
    i2 = l3;
    i3 = 127u;
    i2 &= i3;
    i1 |= i2;
    i32_store8((&memory), (u64)(i0 + 15), i1);
    i0 = l1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i0 -= i1;
    i1 = 0u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 320u;
    i2 = g0;
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
    l5 = i1;
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
  i1 = 4u;
  i0 += i1;
  l3 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 8u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i0 -= i1;
    i1 = 7u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 320u;
    i2 = g0;
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
    i1 = l3;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 8u;
    i1 += i2;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p0;
    i1 = l2;
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
    i0 = f76(i0, i1);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = 48u;
    i0 += i1;
    l2 = i0;
    i1 = l0;
    i0 = i0 == i1;
    if (i0) {goto B3;}
    i0 = l3;
    i0 = i32_load((&memory), (u64)(i0));
    l5 = i0;
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

static void f75(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l3 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  l2 = i1;
  i0 -= i1;
  i1 = 48u;
  i0 = I32_DIV_S(i0, i1);
  l0 = i0;
  i1 = 1u;
  i0 += i1;
  l5 = i0;
  i1 = 89478486u;
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  i0 = 89478485u;
  l4 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l2;
  i0 -= i1;
  i1 = 48u;
  i0 = I32_DIV_S(i0, i1);
  l1 = i0;
  i1 = 44739241u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = l5;
  i1 = l1;
  i2 = 1u;
  i1 <<= (i2 & 31);
  l4 = i1;
  i2 = l4;
  i3 = l5;
  i2 = i2 < i3;
  i0 = i2 ? i0 : i1;
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  B3:;
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
  i0 = f234(i0);
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
  l3 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  goto B0;
  B2:;
  i0 = 0u;
  l4 = i0;
  i0 = 0u;
  l5 = i0;
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
  f241(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B0:;
  i0 = l5;
  i1 = l0;
  i2 = 48u;
  i1 *= i2;
  i0 += i1;
  i1 = p1;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i1 = l3;
  i2 = l2;
  i1 -= i2;
  l3 = i1;
  i2 = 4294967248u;
  i1 = I32_DIV_S(i1, i2);
  i2 = 48u;
  i1 *= i2;
  i0 += i1;
  p1 = i0;
  i0 = l5;
  i1 = l4;
  i2 = 48u;
  i1 *= i2;
  i0 += i1;
  l4 = i0;
  i0 = l0;
  i1 = 48u;
  i0 += i1;
  l5 = i0;
  i0 = l3;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B7;}
  i0 = p1;
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
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  B7:;
  i0 = p0;
  i1 = p1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i1 = l5;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B9:;
  FUNC_EPILOGUE;
}

static u32 f76(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l5 = 0, l6 = 0;
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
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l5 = i0;
  i0 = p1;
  j0 = i64_load32_u((&memory), (u64)(i0));
  l4 = j0;
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
    j0 = l4;
    i0 = (u32)(j0);
    l3 = i0;
    i0 = l6;
    j1 = l4;
    j2 = 7ull;
    j1 >>= (j2 & 63);
    l4 = j1;
    j2 = 0ull;
    i1 = j1 != j2;
    l0 = i1;
    i2 = 7u;
    i1 <<= (i2 & 31);
    i2 = l3;
    i3 = 127u;
    i2 &= i3;
    i1 |= i2;
    i32_store8((&memory), (u64)(i0 + 15), i1);
    i0 = l1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i0 -= i1;
    i1 = 0u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 320u;
    i2 = g0;
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
    l5 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l0;
    if (i0) {goto L0;}
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i0 = 4u;
  l2 = i0;
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i0 -= i1;
    i1 = 0u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 320u;
    i2 = g0;
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
    l3 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = p1;
    i2 = l2;
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
    l5 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l2;
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    i1 = 37u;
    i0 = i0 != i1;
    if (i0) {goto L3;}
  i0 = 0u;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f77(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0, l3 = 0, l4 = 0, l5 = 0;
  u64 l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 48u;
  i1 -= i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 1u;
  l4 = i0;
  i0 = 1u;
  l3 = i0;
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l2 = j1;
  i0 = j0 < j1;
  if (i0) {goto B0;}
  i0 = 0u;
  l3 = i0;
  j0 = l2;
  j1 = l1;
  i0 = j0 < j1;
  if (i0) {goto B0;}
  i0 = p1;
  i0 = i32_load16_u((&memory), (u64)(i0 + 48));
  i1 = p0;
  i1 = i32_load16_u((&memory), (u64)(i1 + 48));
  i0 = i0 < i1;
  l3 = i0;
  B0:;
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0));
  l2 = j0;
  j1 = l1;
  i0 = j0 < j1;
  if (i0) {goto B1;}
  i0 = 0u;
  l4 = i0;
  j0 = l1;
  j1 = l2;
  i0 = j0 < j1;
  if (i0) {goto B1;}
  i0 = p2;
  i0 = i32_load16_u((&memory), (u64)(i0 + 48));
  i1 = p1;
  i1 = i32_load16_u((&memory), (u64)(i1 + 48));
  i0 = i0 < i1;
  l4 = i0;
  B1:;
  i0 = l3;
  i0 = !(i0);
  if (i0) {goto B5;}
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B4;}
  i0 = l5;
  i1 = p0;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i0 = p0;
  i1 = p2;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i1 = p1;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load16_u((&memory), (u64)(i0 + 48));
  p1 = i0;
  i0 = p0;
  i1 = p2;
  i1 = i32_load16_u((&memory), (u64)(i1 + 48));
  i32_store16((&memory), (u64)(i0 + 48), i1);
  i0 = p2;
  i1 = p1;
  i32_store16((&memory), (u64)(i0 + 48), i1);
  i0 = 1u;
  l3 = i0;
  goto B2;
  B5:;
  i0 = 0u;
  l3 = i0;
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B2;}
  i0 = l5;
  i1 = p1;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = p1;
  i1 = p2;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i1 = l4;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load16_u((&memory), (u64)(i0 + 48));
  l3 = i0;
  i0 = p1;
  i1 = p2;
  i1 = i32_load16_u((&memory), (u64)(i1 + 48));
  i32_store16((&memory), (u64)(i0 + 48), i1);
  i0 = p2;
  i1 = l3;
  i32_store16((&memory), (u64)(i0 + 48), i1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l2 = j1;
  i0 = j0 < j1;
  if (i0) {goto B12;}
  i0 = 1u;
  l3 = i0;
  j0 = l2;
  j1 = l1;
  i0 = j0 < j1;
  if (i0) {goto B2;}
  i0 = p1;
  i1 = 48u;
  i0 += i1;
  i0 = i32_load16_u((&memory), (u64)(i0));
  i1 = p0;
  i1 = i32_load16_u((&memory), (u64)(i1 + 48));
  i0 = i0 >= i1;
  if (i0) {goto B2;}
  B12:;
  i0 = l4;
  i1 = p0;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  i0 = p0;
  i1 = p1;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = p2;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  i0 = p0;
  i1 = 48u;
  i0 += i1;
  p0 = i0;
  i0 = i32_load16_u((&memory), (u64)(i0));
  p1 = i0;
  i0 = p0;
  i1 = p2;
  i2 = 48u;
  i1 += i2;
  p2 = i1;
  i1 = i32_load16_u((&memory), (u64)(i1));
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p2;
  i1 = p1;
  i32_store16((&memory), (u64)(i0), i1);
  goto B3;
  B4:;
  i0 = l5;
  i1 = p0;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = p0;
  i1 = p1;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = p1;
  i1 = l0;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  i0 = l4;
  i0 = i32_load16_u((&memory), (u64)(i0 + 48));
  p1 = i0;
  i0 = l4;
  i1 = p0;
  i1 = i32_load16_u((&memory), (u64)(i1 + 48));
  i32_store16((&memory), (u64)(i0 + 48), i1);
  i0 = p0;
  i1 = p1;
  i32_store16((&memory), (u64)(i0 + 48), i1);
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l2 = j1;
  i0 = j0 < j1;
  if (i0) {goto B19;}
  i0 = 1u;
  l3 = i0;
  j0 = l2;
  j1 = l1;
  i0 = j0 < j1;
  if (i0) {goto B2;}
  i0 = p2;
  i0 = i32_load16_u((&memory), (u64)(i0 + 48));
  i1 = p1;
  i2 = 65535u;
  i1 &= i2;
  i0 = i0 >= i1;
  if (i0) {goto B2;}
  B19:;
  i0 = l0;
  i1 = p0;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i0 = p0;
  i1 = p2;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i1 = p1;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 48u;
  i0 += i1;
  p0 = i0;
  i0 = i32_load16_u((&memory), (u64)(i0));
  p1 = i0;
  i0 = p0;
  i1 = p2;
  i2 = 48u;
  i1 += i2;
  p2 = i1;
  i1 = i32_load16_u((&memory), (u64)(i1));
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p2;
  i1 = p1;
  i32_store16((&memory), (u64)(i0), i1);
  B3:;
  i0 = 2u;
  l3 = i0;
  B2:;
  i0 = 0u;
  i1 = l5;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l3;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f78(u32 p0, u32 p1, u32 p2, u32 p3, u32 p4) {
  u32 l0 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 48u;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = p1;
  i2 = p2;
  i3 = p4;
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
  i0 = f77(i0, i1, i2, i3);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p4 = i0;
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1));
  l2 = j1;
  i0 = j0 >= j1;
  if (i0) {goto B3;}
  i0 = p2;
  i1 = 48u;
  i0 += i1;
  l3 = i0;
  goto B2;
  B3:;
  j0 = l2;
  j1 = l1;
  i0 = j0 < j1;
  if (i0) {goto B4;}
  i0 = p3;
  i0 = i32_load16_u((&memory), (u64)(i0 + 48));
  i1 = p2;
  i1 = i32_load16_u((&memory), (u64)(i1 + 48));
  i0 = i0 >= i1;
  if (i0) {goto B4;}
  i0 = p2;
  i1 = 48u;
  i0 += i1;
  l3 = i0;
  goto B2;
  B4:;
  i0 = p4;
  p3 = i0;
  goto B1;
  B2:;
  i0 = l6;
  i1 = p2;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = p2;
  i1 = p3;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  i1 = l0;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i1 = 48u;
  i0 += i1;
  l4 = i0;
  i0 = i32_load16_u((&memory), (u64)(i0));
  l5 = i0;
  i0 = l4;
  i1 = p3;
  i2 = 48u;
  i1 += i2;
  p3 = i1;
  i1 = i32_load16_u((&memory), (u64)(i1));
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p3;
  i1 = l5;
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1));
  l2 = j1;
  i0 = j0 >= j1;
  if (i0) {goto B9;}
  i0 = p1;
  i1 = 48u;
  i0 += i1;
  l5 = i0;
  goto B8;
  B9:;
  i0 = p4;
  i1 = 1u;
  i0 += i1;
  p3 = i0;
  j0 = l2;
  j1 = l1;
  i0 = j0 < j1;
  if (i0) {goto B1;}
  i0 = l3;
  i0 = i32_load16_u((&memory), (u64)(i0));
  i1 = p1;
  i1 = i32_load16_u((&memory), (u64)(i1 + 48));
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  i0 = p1;
  i1 = 48u;
  i0 += i1;
  l5 = i0;
  B8:;
  i0 = l0;
  i1 = p1;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = p2;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i1 = l0;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = i0;
  i0 = p1;
  i1 = 48u;
  i0 += i1;
  p2 = i0;
  i0 = i32_load16_u((&memory), (u64)(i0));
  l4 = i0;
  i0 = p2;
  i1 = p3;
  i2 = 48u;
  i1 += i2;
  p3 = i1;
  i1 = i32_load16_u((&memory), (u64)(i1));
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p3;
  i1 = l4;
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l2 = j1;
  i0 = j0 < j1;
  if (i0) {goto B13;}
  i0 = p4;
  i1 = 2u;
  i0 += i1;
  p3 = i0;
  j0 = l2;
  j1 = l1;
  i0 = j0 < j1;
  if (i0) {goto B1;}
  i0 = l5;
  i0 = i32_load16_u((&memory), (u64)(i0));
  i1 = p0;
  i1 = i32_load16_u((&memory), (u64)(i1 + 48));
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  B13:;
  i0 = l0;
  i1 = p0;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = i0;
  i0 = p0;
  i1 = p1;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = p3;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 48u;
  i0 += i1;
  p1 = i0;
  i0 = i32_load16_u((&memory), (u64)(i0));
  p3 = i0;
  i0 = p1;
  i1 = p2;
  i1 = i32_load16_u((&memory), (u64)(i1));
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p2;
  i1 = p3;
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p4;
  i1 = 3u;
  i0 += i1;
  p3 = i0;
  B1:;
  i0 = 0u;
  i1 = l6;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p3;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f79(u32 p0, u32 p1, u32 p2, u32 p3, u32 p4, u32 p5) {
  u32 l0 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l1 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 48u;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = p1;
  i2 = p2;
  i3 = p3;
  i4 = p5;
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
  i0 = f78(i0, i1, i2, i3, i4);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p5 = i0;
  i0 = p4;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1));
  l2 = j1;
  i0 = j0 >= j1;
  if (i0) {goto B3;}
  i0 = p3;
  i1 = 48u;
  i0 += i1;
  l3 = i0;
  goto B2;
  B3:;
  j0 = l2;
  j1 = l1;
  i0 = j0 < j1;
  if (i0) {goto B4;}
  i0 = p4;
  i0 = i32_load16_u((&memory), (u64)(i0 + 48));
  i1 = p3;
  i1 = i32_load16_u((&memory), (u64)(i1 + 48));
  i0 = i0 >= i1;
  if (i0) {goto B4;}
  i0 = p3;
  i1 = 48u;
  i0 += i1;
  l3 = i0;
  goto B2;
  B4:;
  i0 = p5;
  p4 = i0;
  goto B1;
  B2:;
  i0 = l6;
  i1 = p3;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = p3;
  i1 = p4;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p4;
  i1 = l0;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  i1 = 48u;
  i0 += i1;
  l4 = i0;
  i0 = i32_load16_u((&memory), (u64)(i0));
  l5 = i0;
  i0 = l4;
  i1 = p4;
  i2 = 48u;
  i1 += i2;
  p4 = i1;
  i1 = i32_load16_u((&memory), (u64)(i1));
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p4;
  i1 = l5;
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1));
  l2 = j1;
  i0 = j0 >= j1;
  if (i0) {goto B9;}
  i0 = p2;
  i1 = 48u;
  i0 += i1;
  l5 = i0;
  goto B8;
  B9:;
  i0 = p5;
  i1 = 1u;
  i0 += i1;
  p4 = i0;
  j0 = l2;
  j1 = l1;
  i0 = j0 < j1;
  if (i0) {goto B1;}
  i0 = l3;
  i0 = i32_load16_u((&memory), (u64)(i0));
  i1 = p2;
  i1 = i32_load16_u((&memory), (u64)(i1 + 48));
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  i0 = p2;
  i1 = 48u;
  i0 += i1;
  l5 = i0;
  B8:;
  i0 = l0;
  i1 = p2;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i1 = p3;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  i1 = l0;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i1 = 48u;
  i0 += i1;
  p4 = i0;
  i0 = i32_load16_u((&memory), (u64)(i0));
  l4 = i0;
  i0 = p4;
  i1 = p3;
  i2 = 48u;
  i1 += i2;
  p3 = i1;
  i1 = i32_load16_u((&memory), (u64)(i1));
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p3;
  i1 = l4;
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1));
  l2 = j1;
  i0 = j0 >= j1;
  if (i0) {goto B14;}
  i0 = p1;
  i1 = 48u;
  i0 += i1;
  l4 = i0;
  goto B13;
  B14:;
  i0 = p5;
  i1 = 2u;
  i0 += i1;
  p4 = i0;
  j0 = l2;
  j1 = l1;
  i0 = j0 < j1;
  if (i0) {goto B1;}
  i0 = l5;
  i0 = i32_load16_u((&memory), (u64)(i0));
  i1 = p1;
  i1 = i32_load16_u((&memory), (u64)(i1 + 48));
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  i0 = p1;
  i1 = 48u;
  i0 += i1;
  l4 = i0;
  B13:;
  i0 = l0;
  i1 = p1;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = p2;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = i0;
  i0 = p2;
  i1 = l0;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p4 = i0;
  i0 = p3;
  i1 = 48u;
  i0 += i1;
  p2 = i0;
  i0 = i32_load16_u((&memory), (u64)(i0));
  p1 = i0;
  i0 = p2;
  i1 = p4;
  i2 = 48u;
  i1 += i2;
  p4 = i1;
  i1 = i32_load16_u((&memory), (u64)(i1));
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p4;
  i1 = p1;
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l2 = j1;
  i0 = j0 < j1;
  if (i0) {goto B18;}
  i0 = p5;
  i1 = 3u;
  i0 += i1;
  p4 = i0;
  j0 = l2;
  j1 = l1;
  i0 = j0 < j1;
  if (i0) {goto B1;}
  i0 = l4;
  i0 = i32_load16_u((&memory), (u64)(i0));
  i1 = p0;
  i1 = i32_load16_u((&memory), (u64)(i1 + 48));
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  B18:;
  i0 = l0;
  i1 = p0;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p4 = i0;
  i0 = p0;
  i1 = p3;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p1 = i0;
  i0 = p3;
  i1 = p4;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = 48u;
  i0 += i1;
  p3 = i0;
  i0 = i32_load16_u((&memory), (u64)(i0));
  p4 = i0;
  i0 = p3;
  i1 = p2;
  i1 = i32_load16_u((&memory), (u64)(i1));
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p2;
  i1 = p4;
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p5;
  i1 = 4u;
  i0 += i1;
  p4 = i0;
  B1:;
  i0 = 0u;
  i1 = l6;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p4;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f80(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l3 = 0, l5 = 0, l6 = 0, l7 = 0, l8 = 0, l9 = 0, 
      l10 = 0, l11 = 0, l12 = 0, l13 = 0;
  u64 l2 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 48u;
  i1 -= i2;
  l13 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i1 = p0;
  i0 -= i1;
  i1 = 56u;
  i0 = I32_DIV_S(i0, i1);
  l10 = i0;
  i1 = 5u;
  i0 = i0 > i1;
  if (i0) {goto B6;}
  i0 = 1u;
  l5 = i0;
  i0 = l10;
  switch (i0) {
    case 0: goto B0;
    case 1: goto B0;
    case 2: goto B7;
    case 3: goto B1;
    case 4: goto B2;
    case 5: goto B3;
    default: goto B0;
  }
  B7:;
  i0 = p1;
  i1 = 4294967240u;
  i0 += i1;
  l10 = i0;
  j0 = i64_load((&memory), (u64)(i0));
  l2 = j0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l4 = j1;
  i0 = j0 >= j1;
  if (i0) {goto B5;}
  i0 = p1;
  i1 = 4294967288u;
  i0 += i1;
  p2 = i0;
  goto B4;
  B6:;
  i0 = p0;
  i1 = p0;
  i2 = 56u;
  i1 += i2;
  i2 = p0;
  i3 = 112u;
  i2 += i3;
  l8 = i2;
  i3 = p2;
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
  i0 = f77(i0, i1, i2, i3);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 168u;
  i0 += i1;
  l5 = i0;
  i1 = p1;
  i0 = i0 == i1;
  if (i0) {goto B11;}
  i0 = 0u;
  l7 = i0;
  i0 = 0u;
  l12 = i0;
  L12: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l8;
    l10 = i0;
    i0 = l5;
    l8 = i0;
    j0 = i64_load((&memory), (u64)(i0));
    l2 = j0;
    i1 = l10;
    j1 = i64_load((&memory), (u64)(i1));
    l4 = j1;
    i0 = j0 >= j1;
    if (i0) {goto B15;}
    i0 = l10;
    i1 = 48u;
    i0 += i1;
    l3 = i0;
    i0 = l8;
    i0 = i32_load16_u((&memory), (u64)(i0 + 48));
    l9 = i0;
    goto B14;
    B15:;
    j0 = l4;
    j1 = l2;
    i0 = j0 < j1;
    if (i0) {goto B13;}
    i0 = l8;
    i0 = i32_load16_u((&memory), (u64)(i0 + 48));
    l9 = i0;
    i1 = l10;
    i1 = i32_load16_u((&memory), (u64)(i1 + 48));
    i0 = i0 >= i1;
    if (i0) {goto B13;}
    i0 = l10;
    i1 = 48u;
    i0 += i1;
    l3 = i0;
    B14:;
    i0 = l13;
    i1 = l8;
    i2 = 8u;
    i1 += i2;
    i2 = 40u;
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
    l0 = i0;
    i0 = l8;
    i1 = l10;
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
    i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    l1 = i0;
    i1 = 48u;
    i0 += i1;
    i1 = l10;
    i2 = 48u;
    i1 += i2;
    i1 = i32_load16_u((&memory), (u64)(i1));
    i32_store16((&memory), (u64)(i0), i1);
    i0 = l10;
    i1 = p0;
    i0 = i0 == i1;
    if (i0) {goto B20;}
    i0 = l7;
    p2 = i0;
    L21: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l2;
      i1 = p0;
      i2 = p2;
      i1 += i2;
      l5 = i1;
      i2 = 56u;
      i1 += i2;
      l6 = i1;
      j1 = i64_load((&memory), (u64)(i1));
      l4 = j1;
      i0 = j0 >= j1;
      if (i0) {goto B23;}
      i0 = l5;
      i1 = 104u;
      i0 += i1;
      l3 = i0;
      i0 = i32_load16_u((&memory), (u64)(i0));
      l11 = i0;
      goto B22;
      B23:;
      j0 = l4;
      j1 = l2;
      i0 = j0 < j1;
      if (i0) {goto B19;}
      i0 = l9;
      i1 = 65535u;
      i0 &= i1;
      i1 = l5;
      i2 = 104u;
      i1 += i2;
      i1 = i32_load16_u((&memory), (u64)(i1));
      l11 = i1;
      i0 = i0 >= i1;
      if (i0) {goto B18;}
      i0 = l10;
      i1 = 4294967288u;
      i0 += i1;
      l3 = i0;
      B22:;
      i0 = l10;
      i1 = 4294967240u;
      i0 += i1;
      l10 = i0;
      i0 = l5;
      i1 = 112u;
      i0 += i1;
      i1 = l6;
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
      i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
      i1 = g0;
      i2 = 1u;
      i1 += i2;
      g0 = i1;
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l5;
      i1 = 160u;
      i0 += i1;
      i1 = l11;
      i32_store16((&memory), (u64)(i0), i1);
      i0 = p2;
      i1 = 4294967240u;
      i0 += i1;
      p2 = i0;
      i1 = 4294967184u;
      i0 = i0 != i1;
      if (i0) {goto L21;}
    B20:;
    i0 = p0;
    l10 = i0;
    goto B18;
    B19:;
    i0 = l5;
    i1 = 112u;
    i0 += i1;
    l10 = i0;
    B18:;
    i0 = l10;
    j1 = l2;
    i64_store((&memory), (u64)(i0), j1);
    i0 = l10;
    i1 = 8u;
    i0 += i1;
    i1 = l0;
    i2 = 40u;
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
    i1 = l9;
    i32_store16((&memory), (u64)(i0), i1);
    i0 = l12;
    i1 = 1u;
    i0 += i1;
    l12 = i0;
    i1 = 8u;
    i0 = i0 == i1;
    if (i0) {goto B10;}
    B13:;
    i0 = l7;
    i1 = 56u;
    i0 += i1;
    l7 = i0;
    i0 = l8;
    i1 = 56u;
    i0 += i1;
    l5 = i0;
    i1 = p1;
    i0 = i0 != i1;
    if (i0) {goto L12;}
  B11:;
  i0 = 1u;
  l5 = i0;
  goto B9;
  B10:;
  i0 = l1;
  i1 = 56u;
  i0 += i1;
  i1 = p1;
  i0 = i0 == i1;
  l10 = i0;
  i0 = 0u;
  l5 = i0;
  B9:;
  i0 = l10;
  i1 = l5;
  i0 |= i1;
  l5 = i0;
  goto B0;
  B5:;
  j0 = l4;
  j1 = l2;
  i0 = j0 < j1;
  if (i0) {goto B0;}
  i0 = p1;
  i1 = 4294967288u;
  i0 += i1;
  p2 = i0;
  i0 = i32_load16_u((&memory), (u64)(i0));
  i1 = p0;
  i1 = i32_load16_u((&memory), (u64)(i1 + 48));
  i0 = i0 >= i1;
  if (i0) {goto B0;}
  B4:;
  i0 = l13;
  i1 = p0;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  i0 = p0;
  i1 = l10;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l11 = i0;
  i0 = l10;
  i1 = l6;
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
  i0 = (*Z_envZ_memcpyZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l11;
  i1 = 48u;
  i0 += i1;
  l10 = i0;
  i0 = i32_load16_u((&memory), (u64)(i0));
  l6 = i0;
  i0 = l10;
  i1 = p2;
  i1 = i32_load16_u((&memory), (u64)(i1));
  i32_store16((&memory), (u64)(i0), i1);
  i0 = p2;
  i1 = l6;
  i32_store16((&memory), (u64)(i0), i1);
  goto B0;
  B3:;
  i0 = p0;
  i1 = p0;
  i2 = 56u;
  i1 += i2;
  i2 = p0;
  i3 = 112u;
  i2 += i3;
  i3 = p0;
  i4 = 168u;
  i3 += i4;
  i4 = p1;
  i5 = 4294967240u;
  i4 += i5;
  i5 = p2;
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
  i0 = f79(i0, i1, i2, i3, i4, i5);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B0;
  B2:;
  i0 = p0;
  i1 = p0;
  i2 = 56u;
  i1 += i2;
  i2 = p0;
  i3 = 112u;
  i2 += i3;
  i3 = p1;
  i4 = 4294967240u;
  i3 += i4;
  i4 = p2;
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
  i0 = f78(i0, i1, i2, i3, i4);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B0;
  B1:;
  i0 = p0;
  i1 = p0;
  i2 = 56u;
  i1 += i2;
  i2 = p1;
  i3 = 4294967240u;
  i2 += i3;
  i3 = p2;
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
  i0 = f77(i0, i1, i2, i3);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = 0u;
  i1 = l13;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l5;
  i1 = 1u;
  i0 &= i1;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f81(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = p1;
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
  i0 = f76(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  i0 = l1;
  i1 = p1;
  i1 = i32_load8_u((&memory), (u64)(i1 + 56));
  i32_store8((&memory), (u64)(i0 + 15), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 0u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i1 = l1;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 1u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = p1;
  i2 = 60u;
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
  i0 = f82(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 72u;
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
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 80u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 1u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 88u;
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
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 0u;
  i1 = l1;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f82(u32 p0, u32 p1) {
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
    i1 = 320u;
    i2 = g0;
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
  i1 = 320u;
  i2 = g0;
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

static u32 f83(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l3 = 0, l4 = 0, l5 = 0;
  u64 l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1;
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
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l4 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l4;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l4 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 0u;
  l1 = i0;
  j0 = 0ull;
  l2 = j0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l0 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = p0;
    i2 = 8u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    i0 = i0 < i1;
    i1 = 416u;
    i2 = g0;
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
    l4 = i0;
    i0 = i32_load8_u((&memory), (u64)(i0));
    l3 = i0;
    i0 = l0;
    i1 = l4;
    i2 = 1u;
    i1 += i2;
    l4 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l3;
    i1 = 127u;
    i0 &= i1;
    i1 = l1;
    i2 = 255u;
    i1 &= i2;
    l1 = i1;
    i0 <<= (i1 & 31);
    j0 = (u64)(i0);
    j1 = l2;
    j0 |= j1;
    l2 = j0;
    i0 = l1;
    i1 = 7u;
    i0 += i1;
    l1 = i0;
    i0 = l3;
    i1 = 7u;
    i0 >>= (i1 & 31);
    if (i0) {goto L4;}
  i0 = p1;
  j1 = l2;
  i64_store32((&memory), (u64)(i0 + 16), j1);
  i0 = 20u;
  l3 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l1 = i0;
  L6: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l0;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l4;
    i0 = i0 != i1;
    i1 = 368u;
    i2 = g0;
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
    i1 = l3;
    i0 += i1;
    i1 = l1;
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
    i0 = l1;
    i1 = l1;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 1u;
    i1 += i2;
    l4 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i1 = 53u;
    i0 = i0 != i1;
    if (i0) {goto L6;}
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l4;
  i0 = i0 != i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 15u;
  i0 += i1;
  i1 = p0;
  i2 = 4u;
  i1 += i2;
  l4 = i1;
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
  i0 = l4;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 1u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = l5;
  i1 = i32_load8_u((&memory), (u64)(i1 + 15));
  i2 = 0u;
  i1 = i1 != i2;
  i32_store8((&memory), (u64)(i0 + 56), i1);
  i0 = p0;
  i1 = p1;
  i2 = 60u;
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
  i0 = f85(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 72u;
  i0 += i1;
  i1 = l4;
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
  i0 = l4;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 4u;
  i1 += i2;
  l3 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l4;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l3;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 80u;
  i0 += i1;
  i1 = l4;
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
  i0 = l4;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l3 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l4;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l3;
  i0 -= i1;
  i1 = 1u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 88u;
  i0 += i1;
  i1 = l4;
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
  i0 = l4;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 2u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 0u;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l4;
  FUNC_EPILOGUE;
  return i0;
}

static void f84(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f234(i0);
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
  f241(i0);
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
    i0 = i32_load8_u((&memory), (u64)(i0 + 60));
    i1 = 1u;
    i0 &= i1;
    i0 = !(i0);
    if (i0) {goto B12;}
    i0 = p1;
    i1 = 68u;
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
    f235(i0);
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
    f235(i0);
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
  if (i0) {goto B15;}
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B15:;
  FUNC_EPILOGUE;
}

static u32 f85(u32 p0, u32 p1) {
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
  i0 = f86(i0, i1);
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
  f239(i0, i1);
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
  i0 = f234(i0);
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
  f239(i0, i1);
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
  f235(i0);
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
  f236(i0);
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

static u32 f86(u32 p0, u32 p1) {
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
    i1 = 416u;
    i2 = g0;
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
  f73(i0, i1);
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
  i1 = 368u;
  i2 = g0;
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

static void _ZN11eosiosystem15system_contract12claimrewardsERKy(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l8 = 0, l9 = 0, l18 = 0;
  u64 l4 = 0, l5 = 0, l7 = 0, l10 = 0, l11 = 0, l12 = 0, l13 = 0, l14 = 0, 
      l15 = 0, l16 = 0, l17 = 0;
  f64 l6 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4, j5;
  f64 d0, d1, d2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 272u;
  i1 -= i2;
  l18 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
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
  i0 = p0;
  i1 = 48u;
  i0 += i1;
  l0 = i0;
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1));
  i2 = 432u;
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
  i0 = f88(i0, j1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i0 = i32_load8_u((&memory), (u64)(i0 + 56));
  i1 = 464u;
  i2 = g0;
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
  i1 = 264u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 1499999999999ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 512u;
  i2 = g0;
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
  i0 = l18;
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
  j1 = (*Z_envZ_current_timeZ_jv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l11 = j1;
  i64_store((&memory), (u64)(i0 + 200), j1);
  j0 = l11;
  i1 = l1;
  j1 = i64_load((&memory), (u64)(i1 + 80));
  j0 -= j1;
  j1 = 86400000000ull;
  i0 = j0 > j1;
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
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 672u;
  l9 = i0;
  j0 = 0ull;
  l12 = j0;
  L6: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 10ull;
    i0 = j0 > j1;
    if (i0) {goto B11;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B10;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B9;
    B11:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 == j1;
    if (i0) {goto B8;}
    goto B7;
    B10:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B9:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B8:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B7:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j0 = l13;
    j1 = l12;
    j0 |= j1;
    l12 = j0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j1 = 13ull;
    i0 = j0 != j1;
    if (i0) {goto L6;}
  i0 = l18;
  i1 = 256u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  j1 = l12;
  i64_store((&memory), (u64)(i0 + 224), j1);
  i0 = l18;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 240), j1);
  i0 = l18;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 248), j1);
  i0 = l18;
  j1 = 5459781ull;
  i64_store((&memory), (u64)(i0 + 232), j1);
  i0 = l18;
  i1 = 224u;
  i0 += i1;
  j1 = 5459781ull;
  i2 = 432u;
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
  i0 = f89(i0, j1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = i64_load((&memory), (u64)(i0));
  l13 = j0;
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 248));
  l3 = i0;
  i0 = !(i0);
  if (i0) {goto B13;}
  i0 = l18;
  i1 = 252u;
  i0 += i1;
  l8 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l9 = i0;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B15;}
  L16: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l9;
    i1 = 4294967272u;
    i0 += i1;
    l9 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l2 = i0;
    i0 = l9;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l2;
    i0 = !(i0);
    if (i0) {goto B17;}
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
    f235(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B17:;
    i0 = l3;
    i1 = l9;
    i0 = i0 != i1;
    if (i0) {goto L16;}
  i0 = l18;
  i1 = 248u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l9 = i0;
  goto B14;
  B15:;
  i0 = l3;
  l9 = i0;
  B14:;
  i0 = l8;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l9;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  i0 = p0;
  i1 = 232u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  l11 = j0;
  i0 = !(j0);
  if (i0) {goto B24;}
  i0 = l18;
  j0 = i64_load((&memory), (u64)(i0 + 200));
  j1 = l11;
  j0 -= j1;
  l11 = j0;
  i0 = !(j0);
  if (i0) {goto B24;}
  j0 = l13;
  d0 = (f64)(s64)(j0);
  d1 = 0.04879;
  d0 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d0, d1);
  j1 = l11;
  d1 = (f64)(j1);
  d0 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d0, d1);
  d1 = 31449600000000;
  d0 = (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(d0, d1);
  j0 = (*Z_eosio_injectionZ__eosio_f64_trunc_i64sZ_jd)(d0);
  l17 = j0;
  j1 = 5ull;
  j0 = I64_DIV_S(j0, j1);
  l11 = j0;
  j1 = l17;
  j2 = 20ull;
  j1 = I64_DIV_S(j1, j2);
  l7 = j1;
  j0 -= j1;
  l5 = j0;
  j0 = l17;
  j1 = l11;
  j0 -= j1;
  l4 = j0;
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 672u;
  l9 = i0;
  j0 = 0ull;
  l12 = j0;
  L25: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 10ull;
    i0 = j0 > j1;
    if (i0) {goto B30;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B29;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B28;
    B30:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 == j1;
    if (i0) {goto B27;}
    goto B26;
    B29:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B28:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B27:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B26:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j0 = l13;
    j1 = l12;
    j0 |= j1;
    l12 = j0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j1 = 13ull;
    i0 = j0 != j1;
    if (i0) {goto L25;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 16u;
  l9 = i0;
  j0 = 0ull;
  l15 = j0;
  L31: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 4ull;
    i0 = j0 > j1;
    if (i0) {goto B36;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B35;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B34;
    B36:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B33;}
    goto B32;
    B35:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B34:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B33:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B32:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l15;
    j0 |= j1;
    l15 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L31;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 688u;
  l9 = i0;
  j0 = 0ull;
  l16 = j0;
  L37: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 5ull;
    i0 = j0 > j1;
    if (i0) {goto B42;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B41;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B40;
    B42:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B39;}
    goto B38;
    B41:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B40:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B39:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B38:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l16;
    j0 |= j1;
    l16 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L37;}
  i0 = l18;
  j1 = l16;
  i64_store((&memory), (u64)(i0 + 176), j1);
  i0 = l18;
  j1 = l15;
  i64_store((&memory), (u64)(i0 + 168), j1);
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l9 = i0;
  i1 = 12u;
  i0 += i1;
  i1 = l18;
  i2 = 168u;
  i1 += i2;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l9;
  i1 = 4u;
  i0 += i1;
  i1 = l18;
  i1 = i32_load((&memory), (u64)(i1 + 172));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 184), i1);
  i0 = l9;
  i1 = l18;
  i1 = i32_load((&memory), (u64)(i1 + 168));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = l9;
  i2 = 16u;
  i1 += i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 192), i1);
  i0 = l9;
  i1 = 8u;
  i0 += i1;
  i1 = l18;
  i1 = i32_load((&memory), (u64)(i1 + 176));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 188), i1);
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 16u;
  l9 = i0;
  j0 = 0ull;
  l15 = j0;
  L44: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 4ull;
    i0 = j0 > j1;
    if (i0) {goto B49;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B48;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B47;
    B49:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B46;}
    goto B45;
    B48:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B47:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B46:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B45:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l15;
    j0 |= j1;
    l15 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L44;}
  j0 = l17;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775807ull;
  i0 = j0 < j1;
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
  j0 = 5459781ull;
  l11 = j0;
  i0 = 0u;
  l9 = i0;
  L53: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B52;}
    j0 = l11;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l11 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B54;}
    L55: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l11;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l11 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B52;}
      i0 = l9;
      i1 = 1u;
      i0 += i1;
      l9 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L55;}
    B54:;
    i0 = 1u;
    l2 = i0;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L53;}
    goto B51;
  B52:;
  i0 = 0u;
  l2 = i0;
  B51:;
  i0 = l2;
  i1 = 704u;
  i2 = g0;
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
  i0 = l18;
  i1 = 160u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 152), j1);
  i0 = 736u;
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
  i0 = strlen_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l9 = i0;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B23;}
  i0 = l9;
  i1 = 11u;
  i0 = i0 >= i1;
  if (i0) {goto B60;}
  i0 = l18;
  i1 = l9;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&memory), (u64)(i0 + 152), i1);
  i0 = l18;
  i1 = 152u;
  i0 += i1;
  i1 = 1u;
  i0 |= i1;
  l2 = i0;
  i0 = l9;
  if (i0) {goto B59;}
  goto B58;
  B60:;
  i0 = l9;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = l18;
  i1 = l3;
  i2 = 1u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0 + 152), i1);
  i0 = l18;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 160), i1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 156), i1);
  B59:;
  i0 = l2;
  i1 = 736u;
  i2 = l9;
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
  B58:;
  i0 = l2;
  i1 = l9;
  i0 += i1;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 152));
  l9 = i0;
  i0 = l18;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 152), i1);
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 156));
  l2 = i0;
  i0 = l18;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 156), i1);
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 160));
  l3 = i0;
  i0 = l18;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 160), i1);
  i0 = l18;
  i1 = l18;
  i2 = 192u;
  i1 += i2;
  l8 = i1;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 216), i1);
  i0 = l8;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = l18;
  i1 = i32_load((&memory), (u64)(i1 + 184));
  i32_store((&memory), (u64)(i0 + 208), i1);
  i0 = l18;
  i1 = l18;
  i1 = i32_load((&memory), (u64)(i1 + 188));
  i32_store((&memory), (u64)(i0 + 212), i1);
  i0 = l18;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 188), i1);
  i0 = l18;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 184), i1);
  i0 = l18;
  j1 = l17;
  i64_store((&memory), (u64)(i0 + 232), j1);
  i0 = l18;
  j1 = l15;
  i64_store((&memory), (u64)(i0 + 224), j1);
  i0 = l18;
  i1 = 240u;
  i0 += i1;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 248), i1);
  i0 = l18;
  i1 = 252u;
  i0 += i1;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 256u;
  i0 += i1;
  l9 = i0;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  j0 = l12;
  j1 = 8516769789752901632ull;
  i2 = l18;
  i3 = 208u;
  i2 += i3;
  i3 = l18;
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
  f90(j0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l18;
  i0 = i32_load8_u((&memory), (u64)(i0 + 248));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B64;}
  i0 = l9;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B64:;
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 208));
  l9 = i0;
  i0 = !(i0);
  if (i0) {goto B66;}
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 212), i1);
  i0 = l9;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B66:;
  i0 = l18;
  i0 = i32_load8_u((&memory), (u64)(i0 + 152));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B68;}
  i0 = l18;
  i1 = 160u;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B68:;
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 672u;
  l9 = i0;
  j0 = 0ull;
  l12 = j0;
  L70: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 10ull;
    i0 = j0 > j1;
    if (i0) {goto B75;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B74;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B73;
    B75:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 == j1;
    if (i0) {goto B72;}
    goto B71;
    B74:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B73:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B72:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B71:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j0 = l13;
    j1 = l12;
    j0 |= j1;
    l12 = j0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j1 = 13ull;
    i0 = j0 != j1;
    if (i0) {goto L70;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 16u;
  l9 = i0;
  j0 = 0ull;
  l15 = j0;
  L76: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 4ull;
    i0 = j0 > j1;
    if (i0) {goto B81;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B80;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B79;
    B81:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B78;}
    goto B77;
    B80:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B79:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B78:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B77:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l15;
    j0 |= j1;
    l15 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L76;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 688u;
  l9 = i0;
  j0 = 0ull;
  l16 = j0;
  L82: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 5ull;
    i0 = j0 > j1;
    if (i0) {goto B87;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B86;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B85;
    B87:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B84;}
    goto B83;
    B86:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B85:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B84:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B83:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l16;
    j0 |= j1;
    l16 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L82;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 16u;
  l9 = i0;
  j0 = 0ull;
  l17 = j0;
  L88: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 4ull;
    i0 = j0 > j1;
    if (i0) {goto B93;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B92;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B91;
    B93:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B90;}
    goto B89;
    B92:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B91:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B90:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B89:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l17;
    j0 |= j1;
    l17 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L88;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l13 = j0;
  i0 = 784u;
  l9 = i0;
  j0 = 0ull;
  l14 = j0;
  L94: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = 0ull;
    l10 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 > j1;
    if (i0) {goto B95;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B97;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B96;
    B97:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B96:;
    i0 = l2;
    i1 = 31u;
    i0 &= i1;
    j0 = (u64)(i0);
    j1 = l13;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l10 = j0;
    B95:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l10;
    j1 = l14;
    j0 |= j1;
    l14 = j0;
    j0 = l13;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l13 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L94;}
  j0 = l4;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775807ull;
  i0 = j0 < j1;
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
  j0 = 5459781ull;
  l11 = j0;
  i0 = 0u;
  l9 = i0;
  L101: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B100;}
    j0 = l11;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l11 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B102;}
    L103: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l11;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l11 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B100;}
      i0 = l9;
      i1 = 1u;
      i0 += i1;
      l9 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L103;}
    B102:;
    i0 = 1u;
    l2 = i0;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L101;}
    goto B99;
  B100:;
  i0 = 0u;
  l2 = i0;
  B99:;
  i0 = l2;
  i1 = 704u;
  i2 = g0;
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
  i0 = l18;
  i1 = 128u;
  i0 += i1;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l18;
  i1 = 140u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l18;
  j1 = l14;
  i64_store((&memory), (u64)(i0 + 112), j1);
  i0 = l18;
  j1 = l17;
  i64_store((&memory), (u64)(i0 + 104), j1);
  i0 = l18;
  j1 = l4;
  i64_store((&memory), (u64)(i0 + 120), j1);
  i0 = l18;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 136), i1);
  i0 = l18;
  i1 = 136u;
  i0 += i1;
  l2 = i0;
  i0 = 800u;
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
  i0 = strlen_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l9 = i0;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B22;}
  i0 = l9;
  i1 = 11u;
  i0 = i0 >= i1;
  if (i0) {goto B108;}
  i0 = l18;
  i1 = 136u;
  i0 += i1;
  i1 = l9;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l2;
  i1 = 1u;
  i0 += i1;
  l3 = i0;
  i0 = l9;
  if (i0) {goto B107;}
  goto B106;
  B108:;
  i0 = l9;
  i1 = 16u;
  i0 += i1;
  i1 = 4294967280u;
  i0 &= i1;
  l8 = i0;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = l18;
  i1 = 136u;
  i0 += i1;
  i1 = l8;
  i2 = 1u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 144u;
  i0 += i1;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 140u;
  i0 += i1;
  i1 = l9;
  i32_store((&memory), (u64)(i0), i1);
  B107:;
  i0 = l3;
  i1 = 800u;
  i2 = l9;
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
  B106:;
  i0 = l3;
  i1 = l9;
  i0 += i1;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l9 = i0;
  j1 = l15;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l9;
  j1 = l16;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 168), i1);
  i0 = l18;
  i1 = l9;
  i2 = 16u;
  i1 += i2;
  l9 = i1;
  i32_store((&memory), (u64)(i0 + 176), i1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 172), i1);
  i0 = l18;
  i1 = l18;
  j1 = i64_load((&memory), (u64)(i1 + 104));
  i64_store((&memory), (u64)(i0 + 224), j1);
  i0 = l18;
  i1 = l18;
  i2 = 104u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 232), j1);
  i0 = l18;
  i1 = 224u;
  i0 += i1;
  i1 = 24u;
  i0 += i1;
  i1 = l18;
  i2 = 104u;
  i1 += i2;
  i2 = 24u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l18;
  i1 = l18;
  j1 = i64_load((&memory), (u64)(i1 + 120));
  i64_store((&memory), (u64)(i0 + 240), j1);
  i0 = l18;
  i1 = 224u;
  i0 += i1;
  i1 = 40u;
  i0 += i1;
  l9 = i0;
  i1 = l2;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = l2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 256), j1);
  i0 = l18;
  i1 = 136u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 140u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 104u;
  i0 += i1;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  j0 = l12;
  j1 = 14829575313431724032ull;
  i2 = l18;
  i3 = 168u;
  i2 += i3;
  i3 = l18;
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
  f91(j0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l18;
  i0 = i32_load8_u((&memory), (u64)(i0 + 256));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B113;}
  i0 = l9;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B113:;
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 168));
  l9 = i0;
  i0 = !(i0);
  if (i0) {goto B115;}
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 172), i1);
  i0 = l9;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B115:;
  i0 = l18;
  i1 = 136u;
  i0 += i1;
  i0 = i32_load8_u((&memory), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B117;}
  i0 = l18;
  i1 = 144u;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B117:;
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 672u;
  l9 = i0;
  j0 = 0ull;
  l12 = j0;
  L119: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 10ull;
    i0 = j0 > j1;
    if (i0) {goto B124;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B123;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B122;
    B124:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 == j1;
    if (i0) {goto B121;}
    goto B120;
    B123:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B122:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B121:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B120:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j0 = l13;
    j1 = l12;
    j0 |= j1;
    l12 = j0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j1 = 13ull;
    i0 = j0 != j1;
    if (i0) {goto L119;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 16u;
  l9 = i0;
  j0 = 0ull;
  l15 = j0;
  L125: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 4ull;
    i0 = j0 > j1;
    if (i0) {goto B130;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B129;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B128;
    B130:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B127;}
    goto B126;
    B129:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B128:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B127:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B126:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l15;
    j0 |= j1;
    l15 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L125;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 688u;
  l9 = i0;
  j0 = 0ull;
  l16 = j0;
  L131: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 5ull;
    i0 = j0 > j1;
    if (i0) {goto B136;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B135;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B134;
    B136:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B133;}
    goto B132;
    B135:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B134:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B133:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B132:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l16;
    j0 |= j1;
    l16 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L131;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 16u;
  l9 = i0;
  j0 = 0ull;
  l17 = j0;
  L137: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 4ull;
    i0 = j0 > j1;
    if (i0) {goto B142;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B141;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B140;
    B142:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B139;}
    goto B138;
    B141:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B140:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B139:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B138:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l17;
    j0 |= j1;
    l17 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L137;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 832u;
  l9 = i0;
  j0 = 0ull;
  l14 = j0;
  L143: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 9ull;
    i0 = j0 > j1;
    if (i0) {goto B148;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B147;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B146;
    B148:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B145;}
    goto B144;
    B147:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B146:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B145:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B144:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l14;
    j0 |= j1;
    l14 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L143;}
  j0 = l7;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775807ull;
  i0 = j0 < j1;
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
  j0 = 5459781ull;
  l11 = j0;
  i0 = 0u;
  l9 = i0;
  L152: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B151;}
    j0 = l11;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l11 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B153;}
    L154: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l11;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l11 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B151;}
      i0 = l9;
      i1 = 1u;
      i0 += i1;
      l9 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L154;}
    B153:;
    i0 = 1u;
    l2 = i0;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L152;}
    goto B150;
  B151:;
  i0 = 0u;
  l2 = i0;
  B150:;
  i0 = l2;
  i1 = 704u;
  i2 = g0;
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
  i0 = l18;
  i1 = 80u;
  i0 += i1;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l18;
  i1 = 92u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l18;
  j1 = l14;
  i64_store((&memory), (u64)(i0 + 64), j1);
  i0 = l18;
  j1 = l17;
  i64_store((&memory), (u64)(i0 + 56), j1);
  i0 = l18;
  j1 = l7;
  i64_store((&memory), (u64)(i0 + 72), j1);
  i0 = l18;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 88), i1);
  i0 = l18;
  i1 = 88u;
  i0 += i1;
  l2 = i0;
  i0 = 848u;
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
  i0 = strlen_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l9 = i0;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B21;}
  i0 = l9;
  i1 = 11u;
  i0 = i0 >= i1;
  if (i0) {goto B159;}
  i0 = l18;
  i1 = 88u;
  i0 += i1;
  i1 = l9;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l2;
  i1 = 1u;
  i0 += i1;
  l3 = i0;
  i0 = l9;
  if (i0) {goto B158;}
  goto B157;
  B159:;
  i0 = l9;
  i1 = 16u;
  i0 += i1;
  i1 = 4294967280u;
  i0 &= i1;
  l8 = i0;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = l18;
  i1 = 88u;
  i0 += i1;
  i1 = l8;
  i2 = 1u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 96u;
  i0 += i1;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 92u;
  i0 += i1;
  i1 = l9;
  i32_store((&memory), (u64)(i0), i1);
  B158:;
  i0 = l3;
  i1 = 848u;
  i2 = l9;
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
  B157:;
  i0 = l3;
  i1 = l9;
  i0 += i1;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l9 = i0;
  j1 = l15;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l9;
  j1 = l16;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 168), i1);
  i0 = l18;
  i1 = l9;
  i2 = 16u;
  i1 += i2;
  l9 = i1;
  i32_store((&memory), (u64)(i0 + 176), i1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 172), i1);
  i0 = l18;
  i1 = l18;
  j1 = i64_load((&memory), (u64)(i1 + 56));
  i64_store((&memory), (u64)(i0 + 224), j1);
  i0 = l18;
  i1 = l18;
  i2 = 56u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 232), j1);
  i0 = l18;
  i1 = 224u;
  i0 += i1;
  i1 = 24u;
  i0 += i1;
  i1 = l18;
  i2 = 56u;
  i1 += i2;
  i2 = 24u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l18;
  i1 = l18;
  j1 = i64_load((&memory), (u64)(i1 + 72));
  i64_store((&memory), (u64)(i0 + 240), j1);
  i0 = l18;
  i1 = 224u;
  i0 += i1;
  i1 = 40u;
  i0 += i1;
  l9 = i0;
  i1 = l2;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = l2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 256), j1);
  i0 = l18;
  i1 = 88u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 92u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 56u;
  i0 += i1;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  j0 = l12;
  j1 = 14829575313431724032ull;
  i2 = l18;
  i3 = 168u;
  i2 += i3;
  i3 = l18;
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
  f91(j0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l18;
  i0 = i32_load8_u((&memory), (u64)(i0 + 256));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B164;}
  i0 = l9;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B164:;
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 168));
  l9 = i0;
  i0 = !(i0);
  if (i0) {goto B166;}
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 172), i1);
  i0 = l9;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B166:;
  i0 = l18;
  i1 = 88u;
  i0 += i1;
  i0 = i32_load8_u((&memory), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B168;}
  i0 = l18;
  i1 = 96u;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B168:;
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 672u;
  l9 = i0;
  j0 = 0ull;
  l16 = j0;
  L170: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 10ull;
    i0 = j0 > j1;
    if (i0) {goto B175;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B174;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B173;
    B175:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 == j1;
    if (i0) {goto B172;}
    goto B171;
    B174:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B173:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B172:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B171:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j0 = l13;
    j1 = l16;
    j0 |= j1;
    l16 = j0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j1 = 13ull;
    i0 = j0 != j1;
    if (i0) {goto L170;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 16u;
  l9 = i0;
  j0 = 0ull;
  l17 = j0;
  L176: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 4ull;
    i0 = j0 > j1;
    if (i0) {goto B181;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B180;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B179;
    B181:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B178;}
    goto B177;
    B180:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B179:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B178:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B177:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l17;
    j0 |= j1;
    l17 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L176;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 688u;
  l9 = i0;
  j0 = 0ull;
  l14 = j0;
  L182: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 5ull;
    i0 = j0 > j1;
    if (i0) {goto B187;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B186;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B185;
    B187:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B184;}
    goto B183;
    B186:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B185:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B184:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B183:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l14;
    j0 |= j1;
    l14 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L182;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 16u;
  l9 = i0;
  j0 = 0ull;
  l12 = j0;
  L188: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 4ull;
    i0 = j0 > j1;
    if (i0) {goto B193;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B192;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B191;
    B193:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B190;}
    goto B189;
    B192:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B191:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B190:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B189:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l12;
    j0 |= j1;
    l12 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L188;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 880u;
  l9 = i0;
  j0 = 0ull;
  l15 = j0;
  L194: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 9ull;
    i0 = j0 > j1;
    if (i0) {goto B199;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B198;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B197;
    B199:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B196;}
    goto B195;
    B198:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B197:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B196:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B195:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l15;
    j0 |= j1;
    l15 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L194;}
  j0 = l5;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775807ull;
  i0 = j0 < j1;
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
  j0 = 5459781ull;
  l11 = j0;
  i0 = 0u;
  l9 = i0;
  L203: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B202;}
    j0 = l11;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l11 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B204;}
    L205: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l11;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l11 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B202;}
      i0 = l9;
      i1 = 1u;
      i0 += i1;
      l9 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L205;}
    B204:;
    i0 = 1u;
    l2 = i0;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L203;}
    goto B201;
  B202:;
  i0 = 0u;
  l2 = i0;
  B201:;
  i0 = l2;
  i1 = 704u;
  i2 = g0;
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
  i0 = l18;
  i1 = 32u;
  i0 += i1;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l18;
  i1 = 44u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l18;
  j1 = l15;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l18;
  j1 = l12;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l18;
  j1 = l5;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l18;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l18;
  i1 = 40u;
  i0 += i1;
  l2 = i0;
  i0 = 896u;
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
  i0 = strlen_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l9 = i0;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B20;}
  i0 = l9;
  i1 = 11u;
  i0 = i0 >= i1;
  if (i0) {goto B210;}
  i0 = l18;
  i1 = 40u;
  i0 += i1;
  i1 = l9;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l2;
  i1 = 1u;
  i0 += i1;
  l3 = i0;
  i0 = l9;
  if (i0) {goto B209;}
  goto B208;
  B210:;
  i0 = l9;
  i1 = 16u;
  i0 += i1;
  i1 = 4294967280u;
  i0 &= i1;
  l8 = i0;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = l18;
  i1 = 40u;
  i0 += i1;
  i1 = l8;
  i2 = 1u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 48u;
  i0 += i1;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 44u;
  i0 += i1;
  i1 = l9;
  i32_store((&memory), (u64)(i0), i1);
  B209:;
  i0 = l3;
  i1 = 896u;
  i2 = l9;
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
  B208:;
  i0 = l3;
  i1 = l9;
  i0 += i1;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l9 = i0;
  j1 = l17;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l9;
  j1 = l14;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 168), i1);
  i0 = l18;
  i1 = l9;
  i2 = 16u;
  i1 += i2;
  l9 = i1;
  i32_store((&memory), (u64)(i0 + 176), i1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 172), i1);
  i0 = l18;
  i1 = l18;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i64_store((&memory), (u64)(i0 + 224), j1);
  i0 = l18;
  i1 = l18;
  i2 = 8u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 232), j1);
  i0 = l18;
  i1 = 224u;
  i0 += i1;
  i1 = 24u;
  i0 += i1;
  i1 = l18;
  i2 = 8u;
  i1 += i2;
  i2 = 24u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l18;
  i1 = l18;
  j1 = i64_load((&memory), (u64)(i1 + 24));
  i64_store((&memory), (u64)(i0 + 240), j1);
  i0 = l18;
  i1 = 224u;
  i0 += i1;
  i1 = 40u;
  i0 += i1;
  l9 = i0;
  i1 = l2;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = l2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 256), j1);
  i0 = l18;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 44u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 8u;
  i0 += i1;
  i1 = 40u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  j0 = l16;
  j1 = 14829575313431724032ull;
  i2 = l18;
  i3 = 168u;
  i2 += i3;
  i3 = l18;
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
  f91(j0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l18;
  i0 = i32_load8_u((&memory), (u64)(i0 + 256));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B215;}
  i0 = l9;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B215:;
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 168));
  l9 = i0;
  i0 = !(i0);
  if (i0) {goto B217;}
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 172), i1);
  i0 = l9;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B217:;
  i0 = l18;
  i1 = 40u;
  i0 += i1;
  i0 = i32_load8_u((&memory), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B219;}
  i0 = l18;
  i1 = 48u;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B219:;
  i0 = p0;
  i1 = 232u;
  i0 += i1;
  i1 = l18;
  j1 = i64_load((&memory), (u64)(i1 + 200));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  i1 = 240u;
  i0 += i1;
  l9 = i0;
  i1 = l9;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = l5;
  j1 += j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  i1 = 248u;
  i0 += i1;
  l9 = i0;
  i1 = l9;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = l7;
  j1 += j2;
  i64_store((&memory), (u64)(i0), j1);
  B24:;
  j0 = 0ull;
  l13 = j0;
  j0 = 0ull;
  l14 = j0;
  i0 = p0;
  i1 = 256u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l9 = i0;
  i0 = !(i0);
  if (i0) {goto B221;}
  i0 = l1;
  j0 = i64_load32_u((&memory), (u64)(i0 + 72));
  i1 = p0;
  i2 = 248u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j0 *= j1;
  i1 = l9;
  j1 = (u64)(i1);
  j0 = I64_DIV_S(j0, j1);
  l14 = j0;
  B221:;
  i0 = p0;
  i1 = 240u;
  i0 += i1;
  l2 = i0;
  j0 = i64_load((&memory), (u64)(i0));
  l10 = j0;
  i0 = p0;
  i1 = 288u;
  i0 += i1;
  d0 = f64_load((&memory), (u64)(i0));
  l6 = d0;
  d1 = 0;
  i0 = (*Z_eosio_injectionZ__eosio_f64_leZ_idd)(d0, d1);
  d1 = l6;
  d2 = l6;
  i1 = (*Z_eosio_injectionZ__eosio_f64_neZ_idd)(d1, d2);
  i0 |= i1;
  if (i0) {goto B222;}
  j0 = l10;
  d0 = (f64)(s64)(j0);
  i1 = l1;
  d1 = f64_load((&memory), (u64)(i1 + 8));
  d0 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d0, d1);
  d1 = l6;
  d0 = (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(d0, d1);
  j0 = (*Z_eosio_injectionZ__eosio_f64_trunc_i64sZ_jd)(d0);
  l13 = j0;
  B222:;
  i0 = p0;
  i1 = 256u;
  i0 += i1;
  i1 = l9;
  i2 = l1;
  i2 = i32_load((&memory), (u64)(i2 + 72));
  i1 -= i2;
  i32_store((&memory), (u64)(i0), i1);
  j0 = 0ull;
  l11 = j0;
  i0 = l2;
  j1 = l10;
  j2 = 0ull;
  j3 = l13;
  j4 = l13;
  j5 = 1000000ull;
  i4 = (u64)((s64)j4 < (s64)j5);
  j2 = i4 ? j2 : j3;
  l7 = j2;
  j1 -= j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  i1 = 248u;
  i0 += i1;
  l9 = i0;
  i1 = l9;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = l14;
  j1 -= j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l18;
  i1 = l18;
  i2 = 200u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 224), i1);
  i0 = l0;
  i1 = l1;
  i2 = l18;
  i3 = 224u;
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
  f92(i0, i1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = l14;
  j1 = 1ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B226;}
  j0 = 59ull;
  l10 = j0;
  i0 = 672u;
  l9 = i0;
  j0 = 0ull;
  l12 = j0;
  L227: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 10ull;
    i0 = j0 > j1;
    if (i0) {goto B232;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B231;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B230;
    B232:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 == j1;
    if (i0) {goto B229;}
    goto B228;
    B231:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B230:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B229:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B228:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j0 = l13;
    j1 = l12;
    j0 |= j1;
    l12 = j0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j1 = 13ull;
    i0 = j0 != j1;
    if (i0) {goto L227;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 832u;
  l9 = i0;
  j0 = 0ull;
  l15 = j0;
  L233: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 9ull;
    i0 = j0 > j1;
    if (i0) {goto B238;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B237;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B236;
    B238:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B235;}
    goto B234;
    B237:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B236:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B235:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B234:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l15;
    j0 |= j1;
    l15 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L233;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 688u;
  l9 = i0;
  j0 = 0ull;
  l16 = j0;
  L239: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 5ull;
    i0 = j0 > j1;
    if (i0) {goto B244;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B243;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B242;
    B244:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B241;}
    goto B240;
    B243:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B242:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B241:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B240:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l16;
    j0 |= j1;
    l16 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L239;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 832u;
  l9 = i0;
  j0 = 0ull;
  l17 = j0;
  L245: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 9ull;
    i0 = j0 > j1;
    if (i0) {goto B250;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B249;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B248;
    B250:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B247;}
    goto B246;
    B249:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B248:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B247:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B246:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l17;
    j0 |= j1;
    l17 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L245;}
  j0 = l14;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775807ull;
  i0 = j0 < j1;
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
  j0 = 5459781ull;
  l11 = j0;
  i0 = 0u;
  l9 = i0;
  L254: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B253;}
    j0 = l11;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l11 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B255;}
    L256: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l11;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l11 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B253;}
      i0 = l9;
      i1 = 1u;
      i0 += i1;
      l9 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L256;}
    B255:;
    i0 = 1u;
    l2 = i0;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L254;}
    goto B252;
  B253:;
  i0 = 0u;
  l2 = i0;
  B252:;
  i0 = l2;
  i1 = 704u;
  i2 = g0;
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
  i0 = l18;
  i1 = 216u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 208), j1);
  i0 = 928u;
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
  i0 = strlen_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l9 = i0;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B225;}
  i0 = l9;
  i1 = 11u;
  i0 = i0 >= i1;
  if (i0) {goto B261;}
  i0 = l18;
  i1 = l9;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&memory), (u64)(i0 + 208), i1);
  i0 = l18;
  i1 = 208u;
  i0 += i1;
  i1 = 1u;
  i0 |= i1;
  l2 = i0;
  i0 = l9;
  if (i0) {goto B260;}
  goto B259;
  B261:;
  i0 = l9;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = l18;
  i1 = l3;
  i2 = 1u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0 + 208), i1);
  i0 = l18;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 216), i1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 212), i1);
  B260:;
  i0 = l2;
  i1 = 928u;
  i2 = l9;
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
  B259:;
  i0 = l2;
  i1 = l9;
  i0 += i1;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 208));
  l2 = i0;
  i0 = l18;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 208), i1);
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 212));
  l3 = i0;
  i0 = l18;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 212), i1);
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 216));
  p0 = i0;
  i0 = l18;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 216), i1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l11 = j0;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l9 = i0;
  j1 = l15;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l9;
  j1 = l16;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 168), i1);
  i0 = l18;
  i1 = l9;
  i2 = 16u;
  i1 += i2;
  l9 = i1;
  i32_store((&memory), (u64)(i0 + 176), i1);
  i0 = l18;
  j1 = l17;
  i64_store((&memory), (u64)(i0 + 224), j1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 172), i1);
  i0 = l18;
  j1 = l11;
  i64_store((&memory), (u64)(i0 + 232), j1);
  i0 = l18;
  j1 = l14;
  i64_store((&memory), (u64)(i0 + 240), j1);
  i0 = l18;
  i1 = 248u;
  i0 += i1;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l18;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 256), i1);
  i0 = l18;
  i1 = 260u;
  i0 += i1;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 264u;
  i0 += i1;
  l9 = i0;
  i1 = p0;
  i32_store((&memory), (u64)(i0), i1);
  j0 = l12;
  j1 = 14829575313431724032ull;
  i2 = l18;
  i3 = 168u;
  i2 += i3;
  i3 = l18;
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
  f91(j0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l18;
  i0 = i32_load8_u((&memory), (u64)(i0 + 256));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B266;}
  i0 = l9;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B266:;
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 168));
  l9 = i0;
  i0 = !(i0);
  if (i0) {goto B268;}
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 172), i1);
  i0 = l9;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B268:;
  i0 = l18;
  i0 = i32_load8_u((&memory), (u64)(i0 + 208));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B226;}
  i0 = l18;
  i1 = 216u;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B226:;
  j0 = l7;
  j1 = 1ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B271;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 672u;
  l9 = i0;
  j0 = 0ull;
  l12 = j0;
  L272: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 10ull;
    i0 = j0 > j1;
    if (i0) {goto B277;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B276;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B275;
    B277:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 == j1;
    if (i0) {goto B274;}
    goto B273;
    B276:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B275:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B274:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B273:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j0 = l13;
    j1 = l12;
    j0 |= j1;
    l12 = j0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j1 = 13ull;
    i0 = j0 != j1;
    if (i0) {goto L272;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 880u;
  l9 = i0;
  j0 = 0ull;
  l15 = j0;
  L278: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 9ull;
    i0 = j0 > j1;
    if (i0) {goto B283;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B282;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B281;
    B283:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B280;}
    goto B279;
    B282:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B281:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B280:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B279:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l15;
    j0 |= j1;
    l15 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L278;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 688u;
  l9 = i0;
  j0 = 0ull;
  l16 = j0;
  L284: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 5ull;
    i0 = j0 > j1;
    if (i0) {goto B289;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B288;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B287;
    B289:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B286;}
    goto B285;
    B288:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B287:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B286:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B285:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l16;
    j0 |= j1;
    l16 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L284;}
  j0 = 0ull;
  l11 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 880u;
  l9 = i0;
  j0 = 0ull;
  l17 = j0;
  L290: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    j1 = 9ull;
    i0 = j0 > j1;
    if (i0) {goto B295;}
    i0 = l9;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l2 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B294;}
    i0 = l2;
    i1 = 165u;
    i0 += i1;
    l2 = i0;
    goto B293;
    B295:;
    j0 = 0ull;
    l13 = j0;
    j0 = l11;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B292;}
    goto B291;
    B294:;
    i0 = l2;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l2;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l2 = i0;
    B293:;
    i0 = l2;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l13 = j0;
    B292:;
    j0 = l13;
    j1 = 31ull;
    j0 &= j1;
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l13 = j0;
    B291:;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    j0 = l11;
    j1 = 1ull;
    j0 += j1;
    l11 = j0;
    j0 = l13;
    j1 = l17;
    j0 |= j1;
    l17 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L290;}
  j0 = l7;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775807ull;
  i0 = j0 < j1;
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
  j0 = 5459781ull;
  l11 = j0;
  i0 = 0u;
  l9 = i0;
  L299: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l11;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B298;}
    j0 = l11;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l11 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B300;}
    L301: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l11;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l11 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B298;}
      i0 = l9;
      i1 = 1u;
      i0 += i1;
      l9 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L301;}
    B300:;
    i0 = 1u;
    l2 = i0;
    i0 = l9;
    i1 = 1u;
    i0 += i1;
    l9 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L299;}
    goto B297;
  B298:;
  i0 = 0u;
  l2 = i0;
  B297:;
  i0 = l2;
  i1 = 704u;
  i2 = g0;
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
  i0 = l18;
  i1 = 216u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 208), j1);
  i0 = 960u;
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
  i0 = strlen_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l9 = i0;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B224;}
  i0 = l9;
  i1 = 11u;
  i0 = i0 >= i1;
  if (i0) {goto B306;}
  i0 = l18;
  i1 = l9;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&memory), (u64)(i0 + 208), i1);
  i0 = l18;
  i1 = 208u;
  i0 += i1;
  i1 = 1u;
  i0 |= i1;
  l2 = i0;
  i0 = l9;
  if (i0) {goto B305;}
  goto B304;
  B306:;
  i0 = l9;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = l18;
  i1 = l3;
  i2 = 1u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0 + 208), i1);
  i0 = l18;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 216), i1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 212), i1);
  B305:;
  i0 = l2;
  i1 = 960u;
  i2 = l9;
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
  B304:;
  i0 = l2;
  i1 = l9;
  i0 += i1;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 208));
  l2 = i0;
  i0 = l18;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 208), i1);
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 212));
  l3 = i0;
  i0 = l18;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 212), i1);
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 216));
  p0 = i0;
  i0 = l18;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 216), i1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l11 = j0;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l9 = i0;
  j1 = l15;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l9;
  j1 = l16;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 168), i1);
  i0 = l18;
  i1 = l9;
  i2 = 16u;
  i1 += i2;
  l9 = i1;
  i32_store((&memory), (u64)(i0 + 176), i1);
  i0 = l18;
  j1 = l17;
  i64_store((&memory), (u64)(i0 + 224), j1);
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 172), i1);
  i0 = l18;
  j1 = l11;
  i64_store((&memory), (u64)(i0 + 232), j1);
  i0 = l18;
  j1 = l7;
  i64_store((&memory), (u64)(i0 + 240), j1);
  i0 = l18;
  i1 = 248u;
  i0 += i1;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l18;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 256), i1);
  i0 = l18;
  i1 = 260u;
  i0 += i1;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l18;
  i1 = 264u;
  i0 += i1;
  l9 = i0;
  i1 = p0;
  i32_store((&memory), (u64)(i0), i1);
  j0 = l12;
  j1 = 14829575313431724032ull;
  i2 = l18;
  i3 = 168u;
  i2 += i3;
  i3 = l18;
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
  f91(j0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l18;
  i0 = i32_load8_u((&memory), (u64)(i0 + 256));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B311;}
  i0 = l9;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B311:;
  i0 = l18;
  i0 = i32_load((&memory), (u64)(i0 + 168));
  l9 = i0;
  i0 = !(i0);
  if (i0) {goto B313;}
  i0 = l18;
  i1 = l9;
  i32_store((&memory), (u64)(i0 + 172), i1);
  i0 = l9;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B313:;
  i0 = l18;
  i0 = i32_load8_u((&memory), (u64)(i0 + 208));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B271;}
  i0 = l18;
  i1 = 216u;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B271:;
  i0 = 0u;
  i1 = l18;
  i2 = 272u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  goto Bfunc;
  B225:;
  i0 = l18;
  i1 = 208u;
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
  f236(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B224:;
  i0 = l18;
  i1 = 208u;
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
  f236(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B23:;
  i0 = l18;
  i1 = 152u;
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
  f236(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
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
  f236(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B21:;
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
  f236(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B20:;
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
  f236(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
}

static u32 f88(u32 p0, u64 p1, u32 p2) {
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
  i1 = 92u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p0;
  i0 = i0 == i1;
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
  goto B2;
  B3:;
  i0 = 0u;
  l3 = i0;
  i0 = p0;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 12531438729690087424ull;
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
  i0 = f61(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 92));
  i1 = p0;
  i0 = i0 == i1;
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

static u32 f89(u32 p0, u64 p1, u32 p2) {
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
  i0 = f97(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 40));
  i1 = p0;
  i0 = i0 == i1;
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

static void f90(u64 p0, u64 p1, u32 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 80u;
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
  i1 = f234(i1);
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
  i1 = 24u;
  i0 += i1;
  i1 = 24u;
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
  i1 = 56u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = 60u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p3;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p3;
  i1 = i32_load8_u((&memory), (u64)(i1 + 24));
  l4 = i1;
  i2 = 1u;
  i1 >>= (i2 & 31);
  i2 = l4;
  i3 = 1u;
  i2 &= i3;
  i0 = i2 ? i0 : i1;
  p2 = i0;
  i1 = 24u;
  i0 += i1;
  l4 = i0;
  i0 = p2;
  j0 = (u64)(i0);
  p0 = j0;
  i0 = p3;
  i1 = 24u;
  i0 += i1;
  l2 = i0;
  i0 = l5;
  i1 = 24u;
  i0 += i1;
  i1 = 28u;
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
  f73(i0, i1);
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
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = l5;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 72), i1);
  i0 = p2;
  i1 = l4;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i1 = p3;
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
  i0 = p2;
  i1 = l4;
  i2 = 8u;
  i1 += i2;
  l3 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i1 = p3;
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
  i0 = p2;
  i1 = l4;
  i2 = 16u;
  i1 += i2;
  l3 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i1 = p3;
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
  i0 = l5;
  i1 = l4;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 68), i1);
  i0 = l5;
  i1 = 64u;
  i0 += i1;
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
  i0 = f82(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = 64u;
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
  f94(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 64));
  l4 = i0;
  i1 = l5;
  i1 = i32_load((&memory), (u64)(i1 + 68));
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
  i0 = i32_load((&memory), (u64)(i0 + 64));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B18;}
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 68), i1);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B18:;
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 52));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B20;}
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B20:;
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 40));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B22;}
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B22:;
  i0 = l5;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B24;}
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B24:;
  i0 = 0u;
  i1 = l5;
  i2 = 80u;
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
  f241(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f91(u64 p0, u64 p1, u32 p2, u32 p3) {
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
  i1 = f234(i1);
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
  f73(i0, i1);
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
  f93(i0, i1);
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
  f94(i0, i1);
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
  f235(i0);
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
  f235(i0);
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
  f235(i0);
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
  f235(i0);
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
  f241(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f92(u32 p0, u32 p1, u32 p2) {
  u32 l2 = 0, l4 = 0, l5 = 0;
  u64 l0 = 0, l3 = 0;
  f64 l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1, j2, j3, j4, j5;
  f64 d1, d2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 48u;
  i1 -= i2;
  l4 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i1 = 92u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p0;
  i0 = i0 == i1;
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
  i0 = l4;
  l5 = i0;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  d1 = f64_load((&memory), (u64)(i1));
  l1 = d1;
  d1 = (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(d1);
  d2 = l1;
  i3 = p1;
  i4 = 56u;
  i3 += i4;
  i3 = i32_load8_u((&memory), (u64)(i3));
  d1 = i3 ? d1 : d2;
  f64_store((&memory), (u64)(i0 + 24), d1);
  i0 = p1;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1));
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = p1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 72), i1);
  i0 = 1u;
  i1 = 256u;
  i2 = g0;
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
  j0 = i64_load32_u((&memory), (u64)(i0 + 16));
  l3 = j0;
  i0 = 64u;
  p2 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 1u;
    i0 += i1;
    p2 = i0;
    j0 = l3;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l3 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L4;}
  i0 = p1;
  i1 = 64u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p1;
  i1 = i32_load8_u((&memory), (u64)(i1 + 60));
  l2 = i1;
  i2 = 1u;
  i1 >>= (i2 & 31);
  i2 = l2;
  i3 = 1u;
  i2 &= i3;
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i1 = p2;
  i0 += i1;
  p2 = i0;
  i0 = l2;
  j0 = (u64)(i0);
  l3 = j0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 1u;
    i0 += i1;
    p2 = i0;
    j0 = l3;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l3 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L5;}
  i0 = p2;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
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
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  goto B6;
  B7:;
  i0 = 0u;
  i1 = l4;
  i2 = p2;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l4 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B6:;
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
  i1 = l4;
  i2 = p2;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l5;
  i1 = 8u;
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
  i0 = f81(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 96));
  j1 = 0ull;
  i2 = l4;
  i3 = p2;
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
  i0 = p2;
  i1 = 513u;
  i0 = i0 < i1;
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
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B13;}
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
  B13:;
  i0 = l5;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  d1 = f64_load((&memory), (u64)(i1));
  l1 = d1;
  d1 = (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(d1);
  d2 = l1;
  i3 = p1;
  i4 = 56u;
  i3 += i4;
  i3 = i32_load8_u((&memory), (u64)(i3));
  d1 = i3 ? d1 : d2;
  f64_store((&memory), (u64)(i0 + 40), d1);
  i0 = l5;
  i1 = 24u;
  i0 += i1;
  i1 = l5;
  i2 = 40u;
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
  i0 = memcmp_0(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = !(i0);
  if (i0) {goto B14;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 100));
  p2 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B16;}
  i0 = p1;
  i1 = 100u;
  i0 += i1;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  i2 = p0;
  j2 = i64_load((&memory), (u64)(i2 + 8));
  j3 = 12531438729690087424ull;
  i4 = l5;
  i5 = 32u;
  i4 += i5;
  j5 = l0;
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
  i1 = (*Z_envZ_db_idx_double_find_primaryZ_ijjjij)(j1, j2, j3, i4, j5);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i1;
  i32_store((&memory), (u64)(i0), i1);
  B16:;
  i0 = p2;
  j1 = 0ull;
  i2 = l5;
  i3 = 40u;
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
  (*Z_envZ_db_idx_double_updateZ_viji)(i0, j1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
  i0 = 0u;
  i1 = l5;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f93(u32 p0, u32 p1) {
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
  i1 = 320u;
  i2 = g0;
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
  i1 = 320u;
  i2 = g0;
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
  i1 = 320u;
  i2 = g0;
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
  i1 = 320u;
  i2 = g0;
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
  i0 = f82(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  FUNC_EPILOGUE;
}

static void f94(u32 p0, u32 p1) {
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
  f73(i0, i1);
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
  i1 = 320u;
  i2 = g0;
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
  i1 = 320u;
  i2 = g0;
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
  i0 = f95(i0, i1);
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
  i0 = f96(i0, i1);
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

static u32 f95(u32 p0, u32 p1) {
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
    i1 = 320u;
    i2 = g0;
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
    i1 = 320u;
    i2 = g0;
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
    i1 = 320u;
    i2 = g0;
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

static u32 f96(u32 p0, u32 p1) {
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
    i1 = 320u;
    i2 = g0;
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
  i1 = 320u;
  i2 = g0;
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

static u32 f97(u32 p0, u32 p1) {
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
  i0 = f234(i0);
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
  i0 = f98(i0);
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
  f99(i0, i1);
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
  f100(i0, i1, i2, i3);
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
  f235(i0);
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

static u32 f98(u32 p0) {
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
  i1 = 704u;
  i2 = g0;
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
  i1 = 704u;
  i2 = g0;
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

static void f99(u32 p0, u32 p1) {
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
  i1 = 368u;
  i2 = g0;
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
  i1 = 368u;
  i2 = g0;
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
  i1 = 368u;
  i2 = g0;
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
  i1 = 368u;
  i2 = g0;
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
  i1 = 368u;
  i2 = g0;
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

static void f100(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f234(i0);
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
  f241(i0);
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
    f235(i0);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static void _ZN11eosiosystem15system_contract11buyrambytesEyym(u32 p0, u64 p1, u64 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l5 = 0, l6 = 0;
  u64 l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 144u;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = 332u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i1 = p0;
  i2 = 328u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l0 = i1;
  i0 = i0 == i1;
  if (i0) {goto B0;}
  i0 = l3;
  i1 = 4294967272u;
  i0 += i1;
  l5 = i0;
  i0 = 0u;
  i1 = l0;
  i0 -= i1;
  l1 = i0;
  L1: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l5;
    i0 = i32_load((&memory), (u64)(i0));
    j0 = i64_load((&memory), (u64)(i0 + 8));
    j1 = 4995142087184830980ull;
    i0 = j0 == j1;
    if (i0) {goto B0;}
    i0 = l5;
    l3 = i0;
    i0 = l5;
    i1 = 4294967272u;
    i0 += i1;
    l2 = i0;
    l5 = i0;
    i0 = l2;
    i1 = l1;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L1;}
  B0:;
  i0 = p0;
  i1 = 304u;
  i0 += i1;
  l2 = i0;
  i0 = l3;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = l3;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 64));
  i1 = l2;
  i0 = i0 == i1;
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
  goto B2;
  B3:;
  i0 = 0u;
  l5 = i0;
  i0 = p0;
  i1 = 304u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p0;
  i2 = 312u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 13377137154988703744ull;
  j3 = 4995142087184830980ull;
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
  l3 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B2;}
  i0 = l2;
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
  i0 = f102(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 64));
  i1 = l2;
  i0 = i0 == i1;
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
  B2:;
  i0 = l6;
  i1 = 80u;
  i0 += i1;
  i1 = l5;
  i2 = 64u;
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
  j1 = 1296126464ull;
  i64_store((&memory), (u64)(i0 + 56), j1);
  i0 = l6;
  i1 = p3;
  j1 = (u64)(i1);
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = 1u;
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
  j0 = 5062994ull;
  l4 = j0;
  i0 = 0u;
  l5 = i0;
  L12: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l4;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B11;}
    j0 = l4;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B13;}
    L14: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l4;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l4 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B11;}
      i0 = l5;
      i1 = 1u;
      i0 += i1;
      l5 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L14;}
    B13:;
    i0 = 1u;
    l2 = i0;
    i0 = l5;
    i1 = 1u;
    i0 += i1;
    l5 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L12;}
    goto B10;
  B11:;
  i0 = 0u;
  l2 = i0;
  B10:;
  i0 = l2;
  i1 = 704u;
  i2 = g0;
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
  i1 = 16u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l6;
  i2 = 48u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l6;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1 + 48));
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l6;
  i1 = 64u;
  i0 += i1;
  i1 = l6;
  i2 = 80u;
  i1 += i2;
  i2 = l6;
  i3 = 16u;
  i2 += i3;
  j3 = 1397703940ull;
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
  _ZN11eosiosystem14exchange_state7convertEN5eosio5assetENS1_11symbol_typeE(i0, i1, i2, j3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = 32u;
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
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l6;
  j1 = l4;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  j1 = p1;
  j2 = p2;
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
  _ZN11eosiosystem15system_contract6buyramEyyN5eosio5assetE(i0, j1, j2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = l6;
  i2 = 144u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f102(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  u64 l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 48u;
  i1 -= i2;
  l7 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l7;
  l6 = i0;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 44), i1);
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
  i32_store((&memory), (u64)(i0 + 40), i1);
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
  i0 = l6;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l6;
  i1 = l6;
  i2 = 32u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l6;
  i1 = l6;
  i2 = 44u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = 80u;
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
  i0 = f234(i0);
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
  i0 = f113(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = l6;
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
  f114(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l6;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  l3 = j1;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l6;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1 + 68));
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
  i3 = 8u;
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
  f115(i0, i1, i2, i3);
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
  f235(i0);
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

static void _ZN11eosiosystem14exchange_state7convertEN5eosio5assetENS1_11symbol_typeE(u32 p0, u32 p1, u32 p2, u64 p3) {
  u32 l4 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0, l1 = 0, l3 = 0;
  f64 l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3;
  f64 d0, d1, d2, d3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 112u;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i1 = 48u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  l3 = j0;
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l0 = j0;
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i0 = j0 != j1;
  if (i0) {goto B9;}
  j0 = l3;
  j1 = p3;
  i0 = j0 != j1;
  if (i0) {goto B8;}
  i0 = l6;
  i1 = 80u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l4 = i0;
  i1 = p2;
  i2 = 8u;
  i1 += i2;
  l5 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0));
  l3 = j0;
  i0 = l6;
  i1 = 16u;
  i0 += i1;
  i1 = 12u;
  i0 += i1;
  i1 = l6;
  i2 = 80u;
  i1 += i2;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = 16u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  j1 = l3;
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 84));
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 80));
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l6;
  i1 = 96u;
  i0 += i1;
  i1 = p1;
  i2 = p1;
  i3 = 16u;
  i2 += i3;
  i3 = l6;
  i4 = 16u;
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
  _ZN11eosiosystem14exchange_state21convert_from_exchangeERNS0_9connectorEN5eosio5assetE(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = l6;
  i2 = 96u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p2;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1 + 96));
  i64_store((&memory), (u64)(i0), j1);
  goto B0;
  B9:;
  j0 = l0;
  j1 = l3;
  i0 = j0 != j1;
  if (i0) {goto B7;}
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0));
  l3 = j0;
  d0 = (f64)(s64)(j0);
  i1 = p1;
  i2 = 16u;
  i1 += i2;
  l4 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = l3;
  j1 += j2;
  d1 = (f64)(s64)(j1);
  d0 = (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(d0, d1);
  d1 = 1;
  d0 = (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(d0, d1);
  i1 = p1;
  i2 = 32u;
  i1 += i2;
  d1 = f64_load((&memory), (u64)(i1));
  d2 = 1000;
  d1 = (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(d1, d2);
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  d0 = pow_0(d0, d1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = d0;
  i0 = l4;
  j1 = l3;
  i2 = l4;
  j2 = i64_load((&memory), (u64)(i2));
  j1 += j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p1;
  j1 = l0;
  d1 = (f64)(s64)(j1);
  d2 = 1;
  d3 = l2;
  d2 = (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(d2, d3);
  d1 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d1, d2);
  d1 = (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(d1);
  j1 = (*Z_eosio_injectionZ__eosio_f64_trunc_i64sZ_jd)(d1);
  l0 = j1;
  i2 = p1;
  j2 = i64_load((&memory), (u64)(i2));
  j1 += j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  j0 = l0;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775807ull;
  i0 = j0 < j1;
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
  j0 = l1;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l3 = j0;
  i0 = 0u;
  l4 = i0;
  L13: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B6;}
    j0 = l3;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l3 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B14;}
    L15: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B6;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L15;}
    B14:;
    i0 = 1u;
    l5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L13;}
    goto B5;
  B8:;
  j0 = l1;
  j1 = p3;
  i0 = j0 != j1;
  if (i0) {goto B4;}
  i0 = l6;
  i1 = 64u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l4 = i0;
  i1 = p2;
  i2 = 8u;
  i1 += i2;
  l5 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0));
  l3 = j0;
  i0 = l6;
  i1 = 32u;
  i0 += i1;
  i1 = 12u;
  i0 += i1;
  i1 = l6;
  i2 = 64u;
  i1 += i2;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = 32u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  j1 = l3;
  i64_store((&memory), (u64)(i0 + 64), j1);
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 68));
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 64));
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l6;
  i1 = 96u;
  i0 += i1;
  i1 = p1;
  i2 = p1;
  i3 = 40u;
  i2 += i3;
  i3 = l6;
  i4 = 32u;
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
  _ZN11eosiosystem14exchange_state21convert_from_exchangeERNS0_9connectorEN5eosio5assetE(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l5;
  i1 = l6;
  i2 = 96u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p2;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1 + 96));
  i64_store((&memory), (u64)(i0), j1);
  goto B0;
  B7:;
  j0 = l0;
  j1 = l1;
  i0 = j0 != j1;
  if (i0) {goto B3;}
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0));
  l3 = j0;
  d0 = (f64)(s64)(j0);
  i1 = p1;
  i2 = 40u;
  i1 += i2;
  l4 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = l3;
  j1 += j2;
  d1 = (f64)(s64)(j1);
  d0 = (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(d0, d1);
  d1 = 1;
  d0 = (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(d0, d1);
  i1 = p1;
  i2 = 56u;
  i1 += i2;
  d1 = f64_load((&memory), (u64)(i1));
  d2 = 1000;
  d1 = (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(d1, d2);
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  d0 = pow_0(d0, d1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = d0;
  i0 = l4;
  j1 = l3;
  i2 = l4;
  j2 = i64_load((&memory), (u64)(i2));
  j1 += j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p1;
  j1 = l0;
  d1 = (f64)(s64)(j1);
  d2 = 1;
  d3 = l2;
  d2 = (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(d2, d3);
  d1 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d1, d2);
  d1 = (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(d1);
  j1 = (*Z_eosio_injectionZ__eosio_f64_trunc_i64sZ_jd)(d1);
  l0 = j1;
  i2 = p1;
  j2 = i64_load((&memory), (u64)(i2));
  j1 += j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  l1 = j0;
  j0 = l0;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775807ull;
  i0 = j0 < j1;
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
  j0 = l1;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l3 = j0;
  i0 = 0u;
  l4 = i0;
  L19: 
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
    l3 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B20;}
    L21: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B2;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L21;}
    B20:;
    i0 = 1u;
    l5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L19;}
    goto B1;
  B6:;
  i0 = 0u;
  l5 = i0;
  B5:;
  i0 = l5;
  i1 = 704u;
  i2 = g0;
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
  i0 = p2;
  i1 = 8u;
  i0 += i1;
  j1 = l1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p2;
  j1 = l0;
  i64_store((&memory), (u64)(i0), j1);
  goto B0;
  B4:;
  i0 = 0u;
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
  goto B0;
  B3:;
  i0 = 0u;
  i1 = 1280u;
  i2 = g0;
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
  goto B0;
  B2:;
  i0 = 0u;
  l5 = i0;
  B1:;
  i0 = l5;
  i1 = 704u;
  i2 = g0;
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
  i0 = p2;
  i1 = 8u;
  i0 += i1;
  j1 = l1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p2;
  j1 = l0;
  i64_store((&memory), (u64)(i0), j1);
  B0:;
  i0 = p2;
  i1 = 8u;
  i0 += i1;
  l4 = i0;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = p3;
  i0 = j0 != j1;
  if (i0) {goto B27;}
  i0 = p0;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  goto B26;
  B27:;
  i0 = l6;
  i1 = 48u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1));
  l3 = j1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = l6;
  i1 = 8u;
  i0 += i1;
  j1 = l3;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l6;
  j1 = l0;
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 52));
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l6;
  i1 = l6;
  i1 = i32_load((&memory), (u64)(i1 + 48));
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = p1;
  i2 = l6;
  j3 = p3;
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
  _ZN11eosiosystem14exchange_state7convertEN5eosio5assetENS1_11symbol_typeE(i0, i1, i2, j3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B26:;
  i0 = 0u;
  i1 = l6;
  i2 = 112u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void _ZN11eosiosystem15system_contract6buyramEyyN5eosio5assetE(u32 p0, u64 p1, u64 p2, u32 p3) {
  u32 l4 = 0, l5 = 0, l6 = 0, l13 = 0;
  u64 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l7 = 0, l8 = 0, l9 = 0, l10 = 0, 
      l11 = 0, l12 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 160u;
  i1 -= i2;
  l13 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
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
  j0 = 0ull;
  l8 = j0;
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  j1 = 0ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 1056u;
  i2 = g0;
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
  j1 = l0;
  j2 = 199ull;
  j1 += j2;
  j2 = 200ull;
  j1 = I64_DIV_S(j1, j2);
  l2 = j1;
  j0 -= j1;
  l3 = j0;
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l1 = j0;
  j0 = 59ull;
  l7 = j0;
  i0 = 672u;
  p3 = i0;
  j0 = 0ull;
  l9 = j0;
  L2: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l8;
    j1 = 10ull;
    i0 = j0 > j1;
    if (i0) {goto B7;}
    i0 = p3;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l4 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B6;}
    i0 = l4;
    i1 = 165u;
    i0 += i1;
    l4 = i0;
    goto B5;
    B7:;
    j0 = 0ull;
    l10 = j0;
    j0 = l8;
    j1 = 11ull;
    i0 = j0 == j1;
    if (i0) {goto B4;}
    goto B3;
    B6:;
    i0 = l4;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l4;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    B5:;
    i0 = l4;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l10 = j0;
    B4:;
    j0 = l10;
    j1 = 31ull;
    j0 &= j1;
    j1 = l7;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l10 = j0;
    B3:;
    i0 = p3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    j0 = l7;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l7 = j0;
    j0 = l10;
    j1 = l9;
    j0 |= j1;
    l9 = j0;
    j0 = l8;
    j1 = 1ull;
    j0 += j1;
    l8 = j0;
    j1 = 13ull;
    i0 = j0 != j1;
    if (i0) {goto L2;}
  j0 = 0ull;
  l8 = j0;
  j0 = 59ull;
  l7 = j0;
  i0 = 688u;
  p3 = i0;
  j0 = 0ull;
  l11 = j0;
  L8: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l8;
    j1 = 5ull;
    i0 = j0 > j1;
    if (i0) {goto B13;}
    i0 = p3;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l4 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B12;}
    i0 = l4;
    i1 = 165u;
    i0 += i1;
    l4 = i0;
    goto B11;
    B13:;
    j0 = 0ull;
    l10 = j0;
    j0 = l8;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B10;}
    goto B9;
    B12:;
    i0 = l4;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l4;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    B11:;
    i0 = l4;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l10 = j0;
    B10:;
    j0 = l10;
    j1 = 31ull;
    j0 &= j1;
    j1 = l7;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l10 = j0;
    B9:;
    i0 = p3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    j0 = l8;
    j1 = 1ull;
    j0 += j1;
    l8 = j0;
    j0 = l10;
    j1 = l11;
    j0 |= j1;
    l11 = j0;
    j0 = l7;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l7 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L8;}
  j0 = 0ull;
  l8 = j0;
  j0 = 59ull;
  l7 = j0;
  i0 = 1088u;
  p3 = i0;
  j0 = 0ull;
  l12 = j0;
  L14: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l8;
    j1 = 8ull;
    i0 = j0 > j1;
    if (i0) {goto B19;}
    i0 = p3;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l4 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B18;}
    i0 = l4;
    i1 = 165u;
    i0 += i1;
    l4 = i0;
    goto B17;
    B19:;
    j0 = 0ull;
    l10 = j0;
    j0 = l8;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B16;}
    goto B15;
    B18:;
    i0 = l4;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l4;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    B17:;
    i0 = l4;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l10 = j0;
    B16:;
    j0 = l10;
    j1 = 31ull;
    j0 &= j1;
    j1 = l7;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l10 = j0;
    B15:;
    i0 = p3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    j0 = l8;
    j1 = 1ull;
    j0 += j1;
    l8 = j0;
    j0 = l10;
    j1 = l12;
    j0 |= j1;
    l12 = j0;
    j0 = l7;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l7 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L14;}
  i0 = l13;
  i1 = 152u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l13;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 144), j1);
  i0 = 1104u;
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
  i0 = strlen_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = i0;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B21;}
  i0 = p3;
  i1 = 11u;
  i0 = i0 >= i1;
  if (i0) {goto B25;}
  i0 = l13;
  i1 = p3;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&memory), (u64)(i0 + 144), i1);
  i0 = l13;
  i1 = 144u;
  i0 += i1;
  i1 = 1u;
  i0 |= i1;
  l4 = i0;
  i0 = p3;
  if (i0) {goto B24;}
  goto B23;
  B25:;
  i0 = p3;
  i1 = 16u;
  i0 += i1;
  i1 = 4294967280u;
  i0 &= i1;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = l13;
  i1 = l5;
  i2 = 1u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0 + 144), i1);
  i0 = l13;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 152), i1);
  i0 = l13;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 148), i1);
  B24:;
  i0 = l4;
  i1 = 1104u;
  i2 = p3;
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
  B23:;
  i0 = l4;
  i1 = p3;
  i0 += i1;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 144));
  l4 = i0;
  i0 = l13;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 144), i1);
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 148));
  l5 = i0;
  i0 = l13;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 148), i1);
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 152));
  l6 = i0;
  i0 = l13;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 152), i1);
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = i0;
  j1 = p1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p3;
  j1 = l11;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l13;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l13;
  i1 = p3;
  i2 = 16u;
  i1 += i2;
  p3 = i1;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l13;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 64), j1);
  i0 = l13;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 28), i1);
  i0 = l13;
  j1 = l12;
  i64_store((&memory), (u64)(i0 + 72), j1);
  i0 = l13;
  j1 = l3;
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = l13;
  i1 = 88u;
  i0 += i1;
  j1 = l1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l13;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l13;
  i1 = 100u;
  i0 += i1;
  i1 = l5;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l13;
  i1 = 104u;
  i0 += i1;
  p3 = i0;
  i1 = l6;
  i32_store((&memory), (u64)(i0), i1);
  j0 = l9;
  j1 = 14829575313431724032ull;
  i2 = l13;
  i3 = 24u;
  i2 += i3;
  i3 = l13;
  i4 = 64u;
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
  f91(j0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l13;
  i0 = i32_load8_u((&memory), (u64)(i0 + 96));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B30;}
  i0 = p3;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B30:;
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  p3 = i0;
  i0 = !(i0);
  if (i0) {goto B32;}
  i0 = l13;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 28), i1);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B32:;
  i0 = l13;
  i0 = i32_load8_u((&memory), (u64)(i0 + 144));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B34;}
  i0 = l13;
  i1 = 152u;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B34:;
  j0 = l0;
  j1 = 1ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B36;}
  j0 = 0ull;
  l8 = j0;
  j0 = 59ull;
  l7 = j0;
  i0 = 672u;
  p3 = i0;
  j0 = 0ull;
  l9 = j0;
  L37: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l8;
    j1 = 10ull;
    i0 = j0 > j1;
    if (i0) {goto B42;}
    i0 = p3;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l4 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B41;}
    i0 = l4;
    i1 = 165u;
    i0 += i1;
    l4 = i0;
    goto B40;
    B42:;
    j0 = 0ull;
    l10 = j0;
    j0 = l8;
    j1 = 11ull;
    i0 = j0 == j1;
    if (i0) {goto B39;}
    goto B38;
    B41:;
    i0 = l4;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l4;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    B40:;
    i0 = l4;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l10 = j0;
    B39:;
    j0 = l10;
    j1 = 31ull;
    j0 &= j1;
    j1 = l7;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l10 = j0;
    B38:;
    i0 = p3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    j0 = l7;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l7 = j0;
    j0 = l10;
    j1 = l9;
    j0 |= j1;
    l9 = j0;
    j0 = l8;
    j1 = 1ull;
    j0 += j1;
    l8 = j0;
    j1 = 13ull;
    i0 = j0 != j1;
    if (i0) {goto L37;}
  j0 = 0ull;
  l8 = j0;
  j0 = 59ull;
  l7 = j0;
  i0 = 688u;
  p3 = i0;
  j0 = 0ull;
  l11 = j0;
  L43: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l8;
    j1 = 5ull;
    i0 = j0 > j1;
    if (i0) {goto B48;}
    i0 = p3;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l4 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B47;}
    i0 = l4;
    i1 = 165u;
    i0 += i1;
    l4 = i0;
    goto B46;
    B48:;
    j0 = 0ull;
    l10 = j0;
    j0 = l8;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B45;}
    goto B44;
    B47:;
    i0 = l4;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l4;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    B46:;
    i0 = l4;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l10 = j0;
    B45:;
    j0 = l10;
    j1 = 31ull;
    j0 &= j1;
    j1 = l7;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l10 = j0;
    B44:;
    i0 = p3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    j0 = l8;
    j1 = 1ull;
    j0 += j1;
    l8 = j0;
    j0 = l10;
    j1 = l11;
    j0 |= j1;
    l11 = j0;
    j0 = l7;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l7 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L43;}
  j0 = 0ull;
  l8 = j0;
  j0 = 59ull;
  l10 = j0;
  i0 = 1120u;
  p3 = i0;
  j0 = 0ull;
  l12 = j0;
  L49: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = 0ull;
    l7 = j0;
    j0 = l8;
    j1 = 11ull;
    i0 = j0 > j1;
    if (i0) {goto B50;}
    i0 = p3;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l4 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B52;}
    i0 = l4;
    i1 = 165u;
    i0 += i1;
    l4 = i0;
    goto B51;
    B52:;
    i0 = l4;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l4;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l4 = i0;
    B51:;
    i0 = l4;
    i1 = 31u;
    i0 &= i1;
    j0 = (u64)(i0);
    j1 = l10;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l7 = j0;
    B50:;
    i0 = p3;
    i1 = 1u;
    i0 += i1;
    p3 = i0;
    j0 = l8;
    j1 = 1ull;
    j0 += j1;
    l8 = j0;
    j0 = l7;
    j1 = l12;
    j0 |= j1;
    l12 = j0;
    j0 = l10;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l10 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L49;}
  i0 = l13;
  i1 = 152u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l13;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 144), j1);
  i0 = 1136u;
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
  i0 = strlen_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = i0;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B20;}
  i0 = p3;
  i1 = 11u;
  i0 = i0 >= i1;
  if (i0) {goto B56;}
  i0 = l13;
  i1 = p3;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&memory), (u64)(i0 + 144), i1);
  i0 = l13;
  i1 = 144u;
  i0 += i1;
  i1 = 1u;
  i0 |= i1;
  l4 = i0;
  i0 = p3;
  if (i0) {goto B55;}
  goto B54;
  B56:;
  i0 = p3;
  i1 = 16u;
  i0 += i1;
  i1 = 4294967280u;
  i0 &= i1;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = l13;
  i1 = l5;
  i2 = 1u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0 + 144), i1);
  i0 = l13;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 152), i1);
  i0 = l13;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 148), i1);
  B55:;
  i0 = l4;
  i1 = 1136u;
  i2 = p3;
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
  B54:;
  i0 = l4;
  i1 = p3;
  i0 += i1;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 144));
  l4 = i0;
  i0 = l13;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 144), i1);
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 148));
  l5 = i0;
  i0 = l13;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 148), i1);
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 152));
  l6 = i0;
  i0 = l13;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 152), i1);
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = i0;
  j1 = p1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p3;
  j1 = l11;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l13;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l13;
  i1 = p3;
  i2 = 16u;
  i1 += i2;
  p3 = i1;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l13;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 64), j1);
  i0 = l13;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 28), i1);
  i0 = l13;
  j1 = l12;
  i64_store((&memory), (u64)(i0 + 72), j1);
  i0 = l13;
  j1 = l2;
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = l13;
  i1 = 88u;
  i0 += i1;
  j1 = l1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l13;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l13;
  i1 = 100u;
  i0 += i1;
  i1 = l5;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l13;
  i1 = 104u;
  i0 += i1;
  p3 = i0;
  i1 = l6;
  i32_store((&memory), (u64)(i0), i1);
  j0 = l9;
  j1 = 14829575313431724032ull;
  i2 = l13;
  i3 = 24u;
  i2 += i3;
  i3 = l13;
  i4 = 64u;
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
  f91(j0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l13;
  i0 = i32_load8_u((&memory), (u64)(i0 + 96));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B61;}
  i0 = p3;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B61:;
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  p3 = i0;
  i0 = !(i0);
  if (i0) {goto B63;}
  i0 = l13;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 28), i1);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B63:;
  i0 = l13;
  i0 = i32_load8_u((&memory), (u64)(i0 + 144));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B36;}
  i0 = l13;
  i1 = 152u;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B36:;
  i0 = p0;
  i1 = 304u;
  i0 += i1;
  l4 = i0;
  j1 = 4995142087184830980ull;
  i2 = 1248u;
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
  i0 = f109(i0, j1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 64));
  i1 = l4;
  i0 = i0 == i1;
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
  i0 = l4;
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
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l8 = j0;
  i0 = l13;
  j1 = l1;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l13;
  i1 = 16u;
  i0 += i1;
  j1 = l1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l13;
  j1 = l3;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l13;
  j1 = l3;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l13;
  i1 = 64u;
  i0 += i1;
  i1 = p3;
  i2 = l13;
  i3 = 8u;
  i2 += i3;
  j3 = 1296126464ull;
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
  _ZN11eosiosystem14exchange_state7convertEN5eosio5assetENS1_11symbol_typeE(i0, i1, i2, j3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l13;
  j0 = i64_load((&memory), (u64)(i0 + 64));
  l10 = j0;
  j0 = l8;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i0 = j0 == j1;
  i1 = 256u;
  i2 = g0;
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
  i0 = l13;
  i1 = l13;
  i2 = 64u;
  i1 += i2;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l13;
  i1 = l13;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 28), i1);
  i0 = l13;
  i1 = l13;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l13;
  i1 = 24u;
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
  i0 = f110(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  i0 = i32_load((&memory), (u64)(i0 + 68));
  j1 = 0ull;
  i2 = l13;
  i3 = 64u;
  i2 += i3;
  i3 = 64u;
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
  j0 = l8;
  i1 = p0;
  i2 = 320u;
  i1 += i2;
  p3 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B74;}
  i0 = p3;
  j1 = 18446744073709551614ull;
  j2 = l8;
  j3 = 1ull;
  j2 += j3;
  j3 = l8;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&memory), (u64)(i0), j1);
  B74:;
  j0 = l10;
  j1 = 0ull;
  i0 = (u64)((s64)j0 > (s64)j1);
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
  i0 = p0;
  i1 = 208u;
  i0 += i1;
  p3 = i0;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = l10;
  j1 += j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  i1 = 216u;
  i0 += i1;
  p3 = i0;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = l3;
  j1 += j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l13;
  i1 = 56u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l13;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = l13;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = l13;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l8 = j1;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l13;
  j1 = p2;
  i64_store((&memory), (u64)(i0 + 32), j1);
  j0 = l8;
  j1 = p2;
  j2 = 15426372072997126144ull;
  j3 = p2;
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
  p3 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B77;}
  i0 = l13;
  i1 = 24u;
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
  i0 = f105(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  i1 = l13;
  i2 = 24u;
  i1 += i2;
  i0 = i0 == i1;
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
  i0 = 1u;
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
  i0 = p3;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  i1 = l13;
  i2 = 24u;
  i1 += i2;
  i0 = i0 == i1;
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
  i0 = l13;
  j0 = i64_load((&memory), (u64)(i0 + 24));
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
  i0 = p3;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1 + 40));
  j2 = l10;
  j1 += j2;
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0));
  l8 = j0;
  i0 = 1u;
  i1 = 256u;
  i2 = g0;
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
  i0 = l13;
  i1 = l13;
  i2 = 64u;
  i1 += i2;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 152), i1);
  i0 = l13;
  i1 = l13;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 148), i1);
  i0 = l13;
  i1 = l13;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 144), i1);
  i0 = l13;
  i1 = 144u;
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
  i0 = f106(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  i0 = i32_load((&memory), (u64)(i0 + 52));
  j1 = p2;
  i2 = l13;
  i3 = 64u;
  i2 += i3;
  i3 = 48u;
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
  j0 = l8;
  i1 = l13;
  i2 = 40u;
  i1 += i2;
  l4 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B76;}
  i0 = l4;
  j1 = 18446744073709551614ull;
  j2 = l8;
  j3 = 1ull;
  j2 += j3;
  j3 = l8;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&memory), (u64)(i0), j1);
  goto B76;
  B77:;
  i0 = l13;
  j0 = i64_load((&memory), (u64)(i0 + 24));
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
  i0 = 64u;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p3 = i0;
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
  i0 = f107(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = p3;
  i1 = l13;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = p3;
  j1 = l10;
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = p3;
  j1 = p2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l13;
  i1 = l13;
  i2 = 64u;
  i1 += i2;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 152), i1);
  i0 = l13;
  i1 = l13;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 148), i1);
  i0 = l13;
  i1 = l13;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 144), i1);
  i0 = l13;
  i1 = 144u;
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
  i0 = f106(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  i1 = l13;
  i2 = 32u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 15426372072997126144ull;
  j3 = p2;
  i4 = p3;
  j4 = i64_load((&memory), (u64)(i4));
  l8 = j4;
  i5 = l13;
  i6 = 64u;
  i5 += i6;
  i6 = 48u;
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
  p0 = i1;
  i32_store((&memory), (u64)(i0 + 52), i1);
  j0 = l8;
  i1 = l13;
  i2 = 40u;
  i1 += i2;
  l4 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B94;}
  i0 = l4;
  j1 = 18446744073709551614ull;
  j2 = l8;
  j3 = 1ull;
  j2 += j3;
  j3 = l8;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&memory), (u64)(i0), j1);
  B94:;
  i0 = l13;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 144), i1);
  i0 = l13;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1));
  l8 = j1;
  i64_store((&memory), (u64)(i0 + 64), j1);
  i0 = l13;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 140), i1);
  i0 = l13;
  i1 = 52u;
  i0 += i1;
  l5 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = l13;
  i2 = 56u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B96;}
  i0 = l4;
  j1 = l8;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l4;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l13;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 144), i1);
  i0 = l4;
  i1 = p3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = l4;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B95;
  B96:;
  i0 = l13;
  i1 = 48u;
  i0 += i1;
  i1 = l13;
  i2 = 144u;
  i1 += i2;
  i2 = l13;
  i3 = 64u;
  i2 += i3;
  i3 = l13;
  i4 = 140u;
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
  f108(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B95:;
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 144));
  l4 = i0;
  i0 = l13;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 144), i1);
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B76;}
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B76:;
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1 + 40));
  i2 = p3;
  j2 = i64_load((&memory), (u64)(i2 + 8));
  i3 = p3;
  j3 = i64_load((&memory), (u64)(i3 + 24));
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
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  p0 = i0;
  i0 = !(i0);
  if (i0) {goto B100;}
  i0 = l13;
  i1 = 52u;
  i0 += i1;
  l5 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  p3 = i0;
  i1 = p0;
  i0 = i0 == i1;
  if (i0) {goto B102;}
  L103: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p3;
    i1 = 4294967272u;
    i0 += i1;
    p3 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l4 = i0;
    i0 = p3;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l4;
    i0 = !(i0);
    if (i0) {goto B104;}
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
    f235(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B104:;
    i0 = p0;
    i1 = p3;
    i0 = i0 != i1;
    if (i0) {goto L103;}
  i0 = l13;
  i1 = 48u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p3 = i0;
  goto B101;
  B102:;
  i0 = p0;
  p3 = i0;
  B101:;
  i0 = l5;
  i1 = p0;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B100:;
  i0 = 0u;
  i1 = l13;
  i2 = 160u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  goto Bfunc;
  B21:;
  i0 = l13;
  i1 = 144u;
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
  f236(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B20:;
  i0 = l13;
  i1 = 144u;
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
  f236(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
}

static u32 f105(u32 p0, u32 p1) {
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
  i32_store((&memory), (u64)(i0 + 40), i1);
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
  i0 = 64u;
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
  i0 = f234(i0);
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
  i0 = f107(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = l4;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l6;
  i1 = 32u;
  i0 += i1;
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
  i0 = f111(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 52), i1);
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
  i1 = i32_load((&memory), (u64)(i1 + 52));
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
  f108(i0, i1, i2, i3);
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
  f235(i0);
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

static u32 f106(u32 p0, u32 p1) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 32u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 40u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f107(u32 p0) {
  u32 l1 = 0, l2 = 0;
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p0;
  i1 = 16u;
  i0 += i1;
  l1 = i0;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = 1u;
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
  i0 = l1;
  j0 = i64_load((&memory), (u64)(i0));
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
  i1 = 704u;
  i2 = g0;
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
  i1 = 32u;
  i0 += i1;
  l1 = i0;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = 1u;
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
  i1 = 704u;
  i2 = g0;
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
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f108(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f234(i0);
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
  f241(i0);
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
    f235(i0);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static u32 f109(u32 p0, u64 p1, u32 p2) {
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
  i0 = i32_load((&memory), (u64)(i0 + 64));
  i1 = p0;
  i0 = i0 == i1;
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
  goto B2;
  B3:;
  i0 = 0u;
  l3 = i0;
  i0 = p0;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 13377137154988703744ull;
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
  i0 = f102(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 64));
  i1 = p0;
  i0 = i0 == i1;
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

static u32 f110(u32 p0, u32 p1) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 32u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 40u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 48u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 56u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f111(u32 p0, u32 p1) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 24u;
  i0 += i1;
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 32u;
  i0 += i1;
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 40u;
  i0 += i1;
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void _ZN11eosiosystem14exchange_state21convert_from_exchangeERNS0_9connectorEN5eosio5assetE(u32 p0, u32 p1, u32 p2, u32 p3) {
  u64 l0 = 0, l2 = 0;
  f64 l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1, j2;
  f64 d0, d1, d2, d3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i0 = j0 == j1;
  i1 = 1328u;
  i2 = g0;
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
  i0 = p2;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0));
  l2 = j0;
  d0 = (f64)(s64)(j0);
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = l2;
  j1 -= j2;
  d1 = (f64)(s64)(j1);
  d0 = (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(d0, d1);
  d1 = 1;
  d0 = (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(d0, d1);
  d1 = 1000;
  i2 = p2;
  d2 = f64_load((&memory), (u64)(i2 + 16));
  d1 = (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(d1, d2);
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  d0 = pow_0(d0, d1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = d0;
  i0 = p1;
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = l2;
  j1 -= j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  j1 = l0;
  d1 = (f64)(s64)(j1);
  d2 = l1;
  d3 = -1;
  d2 = (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(d2, d3);
  d1 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d1, d2);
  j1 = (*Z_eosio_injectionZ__eosio_f64_trunc_i64sZ_jd)(d1);
  l2 = j1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p2;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = l2;
  j1 -= j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  l0 = j1;
  i64_store((&memory), (u64)(i0 + 8), j1);
  j0 = l2;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775807ull;
  i0 = j0 < j1;
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
  j0 = l0;
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l2 = j0;
  i0 = 0u;
  p2 = i0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l2;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B4;}
    j0 = l2;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l2 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B6;}
    L7: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l2;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l2 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B4;}
      i0 = p2;
      i1 = 1u;
      i0 += i1;
      p2 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L7;}
    B6:;
    i0 = 1u;
    p1 = i0;
    i0 = p2;
    i1 = 1u;
    i0 += i1;
    p2 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L5;}
    goto B3;
  B4:;
  i0 = 0u;
  p1 = i0;
  B3:;
  i0 = p1;
  i1 = 704u;
  i2 = g0;
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
  FUNC_EPILOGUE;
}

static u32 f113(u32 p0) {
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
  i1 = 704u;
  i2 = g0;
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
  i1 = 704u;
  i2 = g0;
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
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = p0;
  i1 = 32u;
  i0 += i1;
  j1 = 4602678819172646912ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  i1 = 48u;
  i0 += i1;
  l1 = i0;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = 1u;
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
  i0 = l1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l0 = j0;
  i0 = 0u;
  l1 = i0;
  L17: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l0;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B16;}
    j0 = l0;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l0 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B18;}
    L19: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l0;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l0 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B16;}
      i0 = l1;
      i1 = 1u;
      i0 += i1;
      l1 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L19;}
    B18:;
    i0 = 1u;
    l2 = i0;
    i0 = l1;
    i1 = 1u;
    i0 += i1;
    l1 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L17;}
    goto B15;
  B16:;
  i0 = 0u;
  l2 = i0;
  B15:;
  i0 = l2;
  i1 = 704u;
  i2 = g0;
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
  i1 = 56u;
  i0 += i1;
  j1 = 4602678819172646912ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f114(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 24u;
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
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 32u;
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
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 40u;
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
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 48u;
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
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l1;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 56u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 8));
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 68), i1);
  FUNC_EPILOGUE;
}

static void f115(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f234(i0);
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
  f241(i0);
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
    f235(i0);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static void _ZN11eosiosystem15system_contract7sellramEyx(u32 p0, u64 p1, u64 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l13 = 0;
  u64 l7 = 0, l8 = 0, l9 = 0, l10 = 0, l11 = 0, l12 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 176u;
  i1 -= i2;
  l13 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l13;
  j1 = p2;
  i64_store((&memory), (u64)(i0 + 104), j1);
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
  j1 = 0ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 1360u;
  i2 = g0;
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
  i0 = l13;
  i1 = 96u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l13;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 72), j1);
  i0 = l13;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = l13;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 88), j1);
  i0 = l13;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  p2 = j1;
  i64_store((&memory), (u64)(i0 + 64), j1);
  i0 = 0u;
  l2 = i0;
  j0 = p2;
  j1 = p1;
  j2 = 15426372072997126144ull;
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
  l1 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B2;}
  i0 = l13;
  i1 = 64u;
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
  i0 = f105(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  i1 = l13;
  i2 = 64u;
  i1 += i2;
  i0 = i0 == i1;
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
  B2:;
  i0 = l2;
  i1 = 0u;
  i0 = i0 != i1;
  l4 = i0;
  i1 = 1392u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 40));
  i1 = l13;
  j1 = i64_load((&memory), (u64)(i1 + 104));
  i0 = (u64)((s64)j0 >= (s64)j1);
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
  i0 = l13;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0 + 56), j1);
  i0 = l13;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = 1u;
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
  i0 = l13;
  j0 = i64_load((&memory), (u64)(i0 + 56));
  j1 = 8ull;
  j0 >>= (j1 & 63);
  p2 = j0;
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = 0u;
    l1 = i0;
    j0 = p2;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B9;}
    j0 = p2;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    p2 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B11;}
    L12: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = p2;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      p2 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B9;}
      i0 = l6;
      i1 = 1u;
      i0 += i1;
      l6 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L12;}
    B11:;
    i0 = 1u;
    l1 = i0;
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l6 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L10;}
  B9:;
  i0 = l1;
  i1 = 704u;
  i2 = g0;
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
  i1 = 332u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = p0;
  i2 = 328u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l3 = i1;
  i0 = i0 == i1;
  if (i0) {goto B14;}
  i0 = l5;
  i1 = 4294967272u;
  i0 += i1;
  l6 = i0;
  i0 = 0u;
  i1 = l3;
  i0 -= i1;
  l0 = i0;
  L15: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i0 = i32_load((&memory), (u64)(i0));
    j0 = i64_load((&memory), (u64)(i0 + 8));
    j1 = 4995142087184830980ull;
    i0 = j0 == j1;
    if (i0) {goto B14;}
    i0 = l6;
    l5 = i0;
    i0 = l6;
    i1 = 4294967272u;
    i0 += i1;
    l1 = i0;
    l6 = i0;
    i0 = l1;
    i1 = l0;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L15;}
  B14:;
  i0 = p0;
  i1 = 304u;
  i0 += i1;
  l1 = i0;
  i0 = l5;
  i1 = l3;
  i0 = i0 == i1;
  if (i0) {goto B17;}
  i0 = l5;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 64));
  i1 = l1;
  i0 = i0 == i1;
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
  goto B16;
  B17:;
  i0 = 0u;
  l6 = i0;
  i0 = p0;
  i1 = 304u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p0;
  i2 = 312u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 13377137154988703744ull;
  j3 = 4995142087184830980ull;
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
  if (i0) {goto B16;}
  i0 = l1;
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
  i0 = f102(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 64));
  i1 = l1;
  i0 = i0 == i1;
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
  B16:;
  i0 = l13;
  i1 = l13;
  i2 = 104u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 132), i1);
  i0 = l13;
  i1 = l13;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 128), i1);
  i0 = l6;
  i1 = 0u;
  i0 = i0 != i1;
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
  i0 = l1;
  i1 = l6;
  i2 = l13;
  i3 = 128u;
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
  f117(i0, i1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = 208u;
  i0 += i1;
  l6 = i0;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1));
  i2 = l13;
  j2 = i64_load((&memory), (u64)(i2 + 104));
  j1 -= j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  i1 = 216u;
  i0 += i1;
  l6 = i0;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1));
  i2 = l13;
  j2 = i64_load((&memory), (u64)(i2 + 48));
  j1 -= j2;
  p2 = j1;
  i64_store((&memory), (u64)(i0), j1);
  j0 = p2;
  j1 = 63ull;
  j0 >>= (j1 & 63);
  i0 = (u32)(j0);
  i1 = 1u;
  i0 ^= i1;
  i1 = 1440u;
  i2 = g0;
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
  i0 = l2;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  i1 = l13;
  i2 = 64u;
  i1 += i2;
  i0 = i0 == i1;
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
  i0 = l13;
  j0 = i64_load((&memory), (u64)(i0 + 64));
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
  i0 = l2;
  i1 = l2;
  j1 = i64_load((&memory), (u64)(i1 + 40));
  i2 = l13;
  j2 = i64_load((&memory), (u64)(i2 + 104));
  j1 -= j2;
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = l2;
  j0 = i64_load((&memory), (u64)(i0));
  p2 = j0;
  i0 = 1u;
  i1 = 256u;
  i2 = g0;
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
  i0 = l13;
  i1 = l13;
  i2 = 128u;
  i1 += i2;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l13;
  i1 = l13;
  i2 = 128u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l13;
  i1 = l13;
  i2 = 128u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l13;
  i1 = 16u;
  i0 += i1;
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
  i0 = f106(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l2;
  i0 = i32_load((&memory), (u64)(i0 + 52));
  j1 = p1;
  i2 = l13;
  i3 = 128u;
  i2 += i3;
  i3 = 48u;
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
  j0 = p2;
  i1 = l13;
  i2 = 80u;
  i1 += i2;
  l6 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B32;}
  i0 = l6;
  j1 = 18446744073709551614ull;
  j2 = p2;
  j3 = 1ull;
  j2 += j3;
  j3 = p2;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&memory), (u64)(i0), j1);
  B32:;
  i0 = l2;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = l2;
  i2 = 40u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i2 = l2;
  j2 = i64_load((&memory), (u64)(i2 + 8));
  i3 = l2;
  j3 = i64_load((&memory), (u64)(i3 + 24));
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
  j0 = 0ull;
  p2 = j0;
  j0 = 59ull;
  l7 = j0;
  i0 = 672u;
  l6 = i0;
  j0 = 0ull;
  l8 = j0;
  L34: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = p2;
    j1 = 10ull;
    i0 = j0 > j1;
    if (i0) {goto B39;}
    i0 = l6;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l1 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B38;}
    i0 = l1;
    i1 = 165u;
    i0 += i1;
    l1 = i0;
    goto B37;
    B39:;
    j0 = 0ull;
    l9 = j0;
    j0 = p2;
    j1 = 11ull;
    i0 = j0 == j1;
    if (i0) {goto B36;}
    goto B35;
    B38:;
    i0 = l1;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l1;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l1 = i0;
    B37:;
    i0 = l1;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l9 = j0;
    B36:;
    j0 = l9;
    j1 = 31ull;
    j0 &= j1;
    j1 = l7;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l9 = j0;
    B35:;
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l6 = i0;
    j0 = l7;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l7 = j0;
    j0 = l9;
    j1 = l8;
    j0 |= j1;
    l8 = j0;
    j0 = p2;
    j1 = 1ull;
    j0 += j1;
    p2 = j0;
    j1 = 13ull;
    i0 = j0 != j1;
    if (i0) {goto L34;}
  j0 = 0ull;
  p2 = j0;
  j0 = 59ull;
  l7 = j0;
  i0 = 1088u;
  l6 = i0;
  j0 = 0ull;
  l10 = j0;
  L40: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = p2;
    j1 = 8ull;
    i0 = j0 > j1;
    if (i0) {goto B45;}
    i0 = l6;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l1 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B44;}
    i0 = l1;
    i1 = 165u;
    i0 += i1;
    l1 = i0;
    goto B43;
    B45:;
    j0 = 0ull;
    l9 = j0;
    j0 = p2;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B42;}
    goto B41;
    B44:;
    i0 = l1;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l1;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l1 = i0;
    B43:;
    i0 = l1;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l9 = j0;
    B42:;
    j0 = l9;
    j1 = 31ull;
    j0 &= j1;
    j1 = l7;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l9 = j0;
    B41:;
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l6 = i0;
    j0 = p2;
    j1 = 1ull;
    j0 += j1;
    p2 = j0;
    j0 = l9;
    j1 = l10;
    j0 |= j1;
    l10 = j0;
    j0 = l7;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l7 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L40;}
  j0 = 0ull;
  p2 = j0;
  j0 = 59ull;
  l7 = j0;
  i0 = 688u;
  l6 = i0;
  j0 = 0ull;
  l11 = j0;
  L46: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = p2;
    j1 = 5ull;
    i0 = j0 > j1;
    if (i0) {goto B51;}
    i0 = l6;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l1 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B50;}
    i0 = l1;
    i1 = 165u;
    i0 += i1;
    l1 = i0;
    goto B49;
    B51:;
    j0 = 0ull;
    l9 = j0;
    j0 = p2;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B48;}
    goto B47;
    B50:;
    i0 = l1;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l1;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l1 = i0;
    B49:;
    i0 = l1;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l9 = j0;
    B48:;
    j0 = l9;
    j1 = 31ull;
    j0 &= j1;
    j1 = l7;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l9 = j0;
    B47:;
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l6 = i0;
    j0 = p2;
    j1 = 1ull;
    j0 += j1;
    p2 = j0;
    j0 = l9;
    j1 = l11;
    j0 |= j1;
    l11 = j0;
    j0 = l7;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l7 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L46;}
  j0 = 0ull;
  p2 = j0;
  j0 = 59ull;
  l7 = j0;
  i0 = 1088u;
  l6 = i0;
  j0 = 0ull;
  l12 = j0;
  L52: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = p2;
    j1 = 8ull;
    i0 = j0 > j1;
    if (i0) {goto B57;}
    i0 = l6;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l1 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B56;}
    i0 = l1;
    i1 = 165u;
    i0 += i1;
    l1 = i0;
    goto B55;
    B57:;
    j0 = 0ull;
    l9 = j0;
    j0 = p2;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B54;}
    goto B53;
    B56:;
    i0 = l1;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l1;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l1 = i0;
    B55:;
    i0 = l1;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l9 = j0;
    B54:;
    j0 = l9;
    j1 = 31ull;
    j0 &= j1;
    j1 = l7;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l9 = j0;
    B53:;
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l6 = i0;
    j0 = p2;
    j1 = 1ull;
    j0 += j1;
    p2 = j0;
    j0 = l9;
    j1 = l12;
    j0 |= j1;
    l12 = j0;
    j0 = l7;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l7 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L52;}
  i0 = l13;
  i1 = 16u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l13;
  i2 = 48u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l13;
  i1 = l13;
  j1 = i64_load((&memory), (u64)(i1 + 48));
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l13;
  i1 = 8u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l13;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = 1504u;
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
  i0 = strlen_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B59;}
  i0 = l6;
  i1 = 11u;
  i0 = i0 >= i1;
  if (i0) {goto B63;}
  i0 = l13;
  i1 = l6;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l13;
  i1 = 1u;
  i0 |= i1;
  l1 = i0;
  i0 = l6;
  if (i0) {goto B62;}
  goto B61;
  B63:;
  i0 = l6;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i0 = l13;
  i1 = l2;
  i2 = 1u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l13;
  i1 = l1;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l13;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B62:;
  i0 = l1;
  i1 = 1504u;
  i2 = l6;
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
  B61:;
  i0 = l1;
  i1 = l6;
  i0 += i1;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l13;
  i1 = 32u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  i1 = l13;
  i2 = 16u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l13;
  i1 = l13;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i0 = l13;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l5 = i0;
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l0 = i0;
  i0 = l13;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l13;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 8), i1);
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  j1 = l10;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l6;
  j1 = l11;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l13;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  p0 = i1;
  i32_store((&memory), (u64)(i0 + 120), i1);
  i0 = l13;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 112), i1);
  i0 = l13;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 136), j1);
  i0 = l13;
  j1 = l12;
  i64_store((&memory), (u64)(i0 + 128), j1);
  i0 = l13;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 116), i1);
  i0 = l13;
  i1 = 152u;
  i0 += i1;
  i1 = l1;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l13;
  i1 = l13;
  j1 = i64_load((&memory), (u64)(i1 + 32));
  i64_store((&memory), (u64)(i0 + 144), j1);
  i0 = l13;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 160), i1);
  i0 = l13;
  i1 = 164u;
  i0 += i1;
  i1 = l0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l13;
  i1 = 168u;
  i0 += i1;
  l6 = i0;
  i1 = l5;
  i32_store((&memory), (u64)(i0), i1);
  j0 = l8;
  j1 = 14829575313431724032ull;
  i2 = l13;
  i3 = 112u;
  i2 += i3;
  i3 = l13;
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
  f91(j0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l13;
  i0 = i32_load8_u((&memory), (u64)(i0 + 160));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B68;}
  i0 = l6;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B68:;
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 112));
  l6 = i0;
  i0 = !(i0);
  if (i0) {goto B70;}
  i0 = l13;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 116), i1);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B70:;
  i0 = l13;
  i0 = i32_load8_u((&memory), (u64)(i0));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B72;}
  i0 = l13;
  i1 = 8u;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B72:;
  i0 = l13;
  j0 = i64_load((&memory), (u64)(i0 + 48));
  p2 = j0;
  j1 = 200ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B74;}
  j0 = p2;
  j1 = 200ull;
  j0 = DIV_U(j0, j1);
  l12 = j0;
  j0 = 0ull;
  p2 = j0;
  j0 = 59ull;
  l7 = j0;
  i0 = 672u;
  l6 = i0;
  j0 = 0ull;
  l8 = j0;
  L75: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = p2;
    j1 = 10ull;
    i0 = j0 > j1;
    if (i0) {goto B80;}
    i0 = l6;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l1 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B79;}
    i0 = l1;
    i1 = 165u;
    i0 += i1;
    l1 = i0;
    goto B78;
    B80:;
    j0 = 0ull;
    l9 = j0;
    j0 = p2;
    j1 = 11ull;
    i0 = j0 == j1;
    if (i0) {goto B77;}
    goto B76;
    B79:;
    i0 = l1;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l1;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l1 = i0;
    B78:;
    i0 = l1;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l9 = j0;
    B77:;
    j0 = l9;
    j1 = 31ull;
    j0 &= j1;
    j1 = l7;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l9 = j0;
    B76:;
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l6 = i0;
    j0 = l7;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l7 = j0;
    j0 = l9;
    j1 = l8;
    j0 |= j1;
    l8 = j0;
    j0 = p2;
    j1 = 1ull;
    j0 += j1;
    p2 = j0;
    j1 = 13ull;
    i0 = j0 != j1;
    if (i0) {goto L75;}
  j0 = 0ull;
  p2 = j0;
  j0 = 59ull;
  l7 = j0;
  i0 = 688u;
  l6 = i0;
  j0 = 0ull;
  l10 = j0;
  L81: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = p2;
    j1 = 5ull;
    i0 = j0 > j1;
    if (i0) {goto B86;}
    i0 = l6;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l1 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B85;}
    i0 = l1;
    i1 = 165u;
    i0 += i1;
    l1 = i0;
    goto B84;
    B86:;
    j0 = 0ull;
    l9 = j0;
    j0 = p2;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B83;}
    goto B82;
    B85:;
    i0 = l1;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l1;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l1 = i0;
    B84:;
    i0 = l1;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    l9 = j0;
    B83:;
    j0 = l9;
    j1 = 31ull;
    j0 &= j1;
    j1 = l7;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l9 = j0;
    B82:;
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l6 = i0;
    j0 = p2;
    j1 = 1ull;
    j0 += j1;
    p2 = j0;
    j0 = l9;
    j1 = l10;
    j0 |= j1;
    l10 = j0;
    j0 = l7;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l7 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L81;}
  j0 = 0ull;
  p2 = j0;
  j0 = 59ull;
  l9 = j0;
  i0 = 1120u;
  l6 = i0;
  j0 = 0ull;
  l11 = j0;
  L87: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = 0ull;
    l7 = j0;
    j0 = p2;
    j1 = 11ull;
    i0 = j0 > j1;
    if (i0) {goto B88;}
    i0 = l6;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l1 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B90;}
    i0 = l1;
    i1 = 165u;
    i0 += i1;
    l1 = i0;
    goto B89;
    B90:;
    i0 = l1;
    i1 = 208u;
    i0 += i1;
    i1 = 0u;
    i2 = l1;
    i3 = 4294967247u;
    i2 += i3;
    i3 = 255u;
    i2 &= i3;
    i3 = 5u;
    i2 = i2 < i3;
    i0 = i2 ? i0 : i1;
    l1 = i0;
    B89:;
    i0 = l1;
    i1 = 31u;
    i0 &= i1;
    j0 = (u64)(i0);
    j1 = l9;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    l7 = j0;
    B88:;
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l6 = i0;
    j0 = p2;
    j1 = 1ull;
    j0 += j1;
    p2 = j0;
    j0 = l7;
    j1 = l11;
    j0 |= j1;
    l11 = j0;
    j0 = l9;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l9 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L87;}
  i0 = 1u;
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
  j0 = 5459781ull;
  p2 = j0;
  i0 = 0u;
  l6 = i0;
  L94: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = p2;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B93;}
    j0 = p2;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    p2 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B95;}
    L96: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = p2;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      p2 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B93;}
      i0 = l6;
      i1 = 1u;
      i0 += i1;
      l6 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L96;}
    B95:;
    i0 = 1u;
    l1 = i0;
    i0 = l6;
    i1 = 1u;
    i0 += i1;
    l6 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L94;}
    goto B92;
  B93:;
  i0 = 0u;
  l1 = i0;
  B92:;
  i0 = l1;
  i1 = 704u;
  i2 = g0;
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
  i0 = l13;
  i1 = 120u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l13;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 112), j1);
  i0 = 1520u;
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
  i0 = strlen_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B58;}
  i0 = l6;
  i1 = 11u;
  i0 = i0 >= i1;
  if (i0) {goto B101;}
  i0 = l13;
  i1 = l6;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&memory), (u64)(i0 + 112), i1);
  i0 = l13;
  i1 = 112u;
  i0 += i1;
  i1 = 1u;
  i0 |= i1;
  l1 = i0;
  i0 = l6;
  if (i0) {goto B100;}
  goto B99;
  B101:;
  i0 = l6;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l1 = i0;
  i0 = l13;
  i1 = l2;
  i2 = 1u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0 + 112), i1);
  i0 = l13;
  i1 = l1;
  i32_store((&memory), (u64)(i0 + 120), i1);
  i0 = l13;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 116), i1);
  B100:;
  i0 = l1;
  i1 = 1520u;
  i2 = l6;
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
  B99:;
  i0 = l1;
  i1 = l6;
  i0 += i1;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 112));
  l1 = i0;
  i0 = l13;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 112), i1);
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 116));
  l2 = i0;
  i0 = l13;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 116), i1);
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 120));
  l5 = i0;
  i0 = l13;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 120), i1);
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  j1 = p1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l6;
  j1 = l10;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l13;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l13;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l13;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 128), j1);
  i0 = l13;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l13;
  j1 = l11;
  i64_store((&memory), (u64)(i0 + 136), j1);
  i0 = l13;
  j1 = l12;
  i64_store((&memory), (u64)(i0 + 144), j1);
  i0 = l13;
  i1 = 152u;
  i0 += i1;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l13;
  i1 = l1;
  i32_store((&memory), (u64)(i0 + 160), i1);
  i0 = l13;
  i1 = 164u;
  i0 += i1;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l13;
  i1 = 168u;
  i0 += i1;
  l6 = i0;
  i1 = l5;
  i32_store((&memory), (u64)(i0), i1);
  j0 = l8;
  j1 = 14829575313431724032ull;
  i2 = l13;
  i3 = 16u;
  i2 += i3;
  i3 = l13;
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
  f91(j0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l13;
  i0 = i32_load8_u((&memory), (u64)(i0 + 160));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B106;}
  i0 = l6;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B106:;
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 16));
  l6 = i0;
  i0 = !(i0);
  if (i0) {goto B108;}
  i0 = l13;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 20), i1);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B108:;
  i0 = l13;
  i0 = i32_load8_u((&memory), (u64)(i0 + 112));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B74;}
  i0 = l13;
  i1 = 120u;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B74:;
  i0 = l13;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  l2 = i0;
  i0 = !(i0);
  if (i0) {goto B111;}
  i0 = l13;
  i1 = 92u;
  i0 += i1;
  l5 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B113;}
  L114: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l6;
    i1 = 4294967272u;
    i0 += i1;
    l6 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l1 = i0;
    i0 = l6;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l1;
    i0 = !(i0);
    if (i0) {goto B115;}
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
    f235(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B115:;
    i0 = l2;
    i1 = l6;
    i0 = i0 != i1;
    if (i0) {goto L114;}
  i0 = l13;
  i1 = 88u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l6 = i0;
  goto B112;
  B113:;
  i0 = l2;
  l6 = i0;
  B112:;
  i0 = l5;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B111:;
  i0 = 0u;
  i1 = l13;
  i2 = 176u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  goto Bfunc;
  B59:;
  i0 = l13;
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
  f236(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B58:;
  i0 = l13;
  i1 = 112u;
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
  f236(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
}

static void f117(u32 p0, u32 p1, u32 p2) {
  u32 l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  u64 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 48u;
  i1 -= i2;
  l5 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 64));
  i1 = p0;
  i0 = i0 == i1;
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
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l0 = j0;
  i0 = l5;
  l4 = i0;
  j1 = 1296126464ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l4;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  j1 = i64_load((&memory), (u64)(i1));
  l1 = j1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  j0 = l1;
  j1 = 4611686018427387903ull;
  j0 += j1;
  j1 = 9223372036854775807ull;
  i0 = j0 < j1;
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
  j0 = 5062994ull;
  l1 = j0;
  i0 = 0u;
  l2 = i0;
  L6: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l1;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B5;}
    j0 = l1;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l1 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B7;}
    L8: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l1;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l1 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B5;}
      i0 = l2;
      i1 = 1u;
      i0 += i1;
      l2 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L8;}
    B7:;
    i0 = 1u;
    l3 = i0;
    i0 = l2;
    i1 = 1u;
    i0 += i1;
    l2 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L6;}
    goto B4;
  B5:;
  i0 = 0u;
  l3 = i0;
  B4:;
  i0 = l3;
  i1 = 704u;
  i2 = g0;
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
  i1 = l4;
  i2 = 16u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l4;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l4;
  i1 = 32u;
  i0 += i1;
  i1 = p1;
  i2 = l4;
  j3 = 1397703940ull;
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
  _ZN11eosiosystem14exchange_state7convertEN5eosio5assetENS1_11symbol_typeE(i0, i1, i2, j3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1 + 32));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l2;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  i2 = 32u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  j0 = l0;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 256u;
  i2 = g0;
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
  i1 = l5;
  l3 = i1;
  i2 = 4294967232u;
  i1 += i2;
  l2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l4;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l4;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 32), i1);
  i0 = l4;
  i1 = l3;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l4;
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
  i0 = f110(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 68));
  j1 = 0ull;
  i2 = l2;
  i3 = 64u;
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
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B14;}
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
  B14:;
  i0 = 0u;
  i1 = l4;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void _ZN11eosiosystem19validate_b1_vestingEx(u64 p0) {
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1, j2;
  f64 d1, d2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j0 = 1000000000000ull;
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
  j1 = (*Z_envZ_current_timeZ_jv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j2 = 1000000ull;
  j1 = DIV_U(j1, j2);
  j2 = 4294967295ull;
  j1 &= j2;
  j2 = 18446744072181740416ull;
  j1 += j2;
  d1 = (f64)(s64)(j1);
  d2 = 1000000000000;
  d1 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d1, d2);
  d2 = 314496000;
  d1 = (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(d1, d2);
  j1 = (*Z_eosio_injectionZ__eosio_f64_trunc_i64sZ_jd)(d1);
  j0 -= j1;
  j1 = p0;
  i0 = (u64)((s64)j0 <= (s64)j1);
  i1 = 1536u;
  i2 = g0;
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
  FUNC_EPILOGUE;
}

static void _ZN11eosiosystem15system_contract8changebwEyyN5eosio5assetES2_b(u32 p0, u64 p1, u64 p2, u32 p3, u32 p4, u32 p5) {
  u32 l0 = 0, l3 = 0, l4 = 0, l10 = 0;
  u64 l1 = 0, l2 = 0, l5 = 0, l6 = 0, l7 = 0, l8 = 0, l9 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4;
  f64 d1, d2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 192u;
  i1 -= i2;
  l10 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l10;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 152), j1);
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
  i0 = 1u;
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
  i0 = 0u;
  l4 = i0;
  j0 = 5459781ull;
  l6 = j0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B3;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B5;}
    L6: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B3;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L6;}
    B5:;
    i0 = 1u;
    l0 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L4;}
    goto B2;
  B3:;
  i0 = 0u;
  l0 = i0;
  B2:;
  i0 = l0;
  i1 = 704u;
  i2 = g0;
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
  l5 = j0;
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0));
  l8 = j0;
  i0 = !(j0);
  if (i0) {goto B11;}
  i0 = 1u;
  i1 = 1648u;
  i2 = g0;
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
  l6 = j0;
  i0 = p5;
  if (i0) {goto B10;}
  goto B9;
  B11:;
  i0 = 1u;
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
    l0 = i0;
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
  l0 = i0;
  B14:;
  i0 = l0;
  i1 = 704u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 0ull;
  i0 = j0 != j1;
  i1 = 1648u;
  i2 = g0;
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
  l6 = j0;
  i0 = p5;
  i0 = !(i0);
  if (i0) {goto B9;}
  B10:;
  i0 = l10;
  j1 = p2;
  i64_store((&memory), (u64)(i0 + 152), j1);
  j0 = p2;
  l6 = j0;
  B9:;
  i0 = l10;
  i1 = 144u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 128), j1);
  i0 = l10;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 136), j1);
  i0 = l10;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l7 = j1;
  i64_store((&memory), (u64)(i0 + 112), j1);
  i0 = l10;
  j1 = l6;
  i64_store((&memory), (u64)(i0 + 120), j1);
  j0 = l7;
  j1 = l6;
  j2 = 5377987680120340480ull;
  j3 = p2;
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
  l4 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B23;}
  i0 = l10;
  i1 = 112u;
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
  i0 = f120(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  i1 = l10;
  i2 = 112u;
  i1 += i2;
  i0 = i0 == i1;
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
  i0 = 1u;
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
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  i1 = l10;
  i2 = 112u;
  i1 += i2;
  i0 = i0 == i1;
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
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 112));
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
  i0 = l0;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l6 = j0;
  j0 = l5;
  i1 = l0;
  i2 = 24u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 1680u;
  i2 = g0;
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
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  j2 = l8;
  j1 += j2;
  l7 = j1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  j0 = l7;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 1728u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 16));
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  i1 = 1760u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i2 = 40u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 1680u;
  i2 = g0;
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
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1 + 32));
  i2 = p4;
  j2 = i64_load((&memory), (u64)(i2));
  j1 += j2;
  l7 = j1;
  i64_store((&memory), (u64)(i0 + 32), j1);
  j0 = l7;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 1728u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 32));
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  i1 = 1760u;
  i2 = g0;
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
  j0 = l6;
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i0 = j0 == j1;
  i1 = 256u;
  i2 = g0;
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
  i2 = 16u;
  i1 += i2;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 104), i1);
  i0 = l10;
  i1 = l10;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 100), i1);
  i0 = l10;
  i1 = l10;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l10;
  i1 = 96u;
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
  i0 = f121(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 52));
  j1 = 0ull;
  i2 = l10;
  i3 = 16u;
  i2 += i3;
  i3 = 48u;
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
  j0 = l6;
  i1 = l10;
  i2 = 128u;
  i1 += i2;
  l4 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B22;}
  i0 = l4;
  j1 = 18446744073709551614ull;
  j2 = l6;
  j3 = 1ull;
  j2 += j3;
  j3 = l6;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&memory), (u64)(i0), j1);
  goto B22;
  B23:;
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 152));
  l7 = j0;
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 112));
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
  i0 = 64u;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
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
  i0 = f122(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = l0;
  i1 = l10;
  i2 = 112u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l0;
  j1 = p2;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = l10;
  j1 = i64_load((&memory), (u64)(i1 + 152));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l0;
  i1 = 28u;
  i0 += i1;
  i1 = p3;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = 24u;
  i0 += i1;
  i1 = p3;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = 20u;
  i0 += i1;
  i1 = p3;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = p3;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  i1 = p4;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l0;
  i1 = p4;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l10;
  i1 = l10;
  i2 = 16u;
  i1 += i2;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 104), i1);
  i0 = l10;
  i1 = l10;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 100), i1);
  i0 = l10;
  i1 = l10;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l10;
  i1 = 96u;
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
  i0 = f121(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = l10;
  i2 = 112u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 5377987680120340480ull;
  j3 = l7;
  i4 = l0;
  j4 = i64_load((&memory), (u64)(i4 + 8));
  l6 = j4;
  i5 = l10;
  i6 = 16u;
  i5 += i6;
  i6 = 48u;
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
  p5 = i1;
  i32_store((&memory), (u64)(i0 + 52), i1);
  j0 = l6;
  i1 = l10;
  i2 = 128u;
  i1 += i2;
  l4 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B46;}
  i0 = l4;
  j1 = 18446744073709551614ull;
  j2 = l6;
  j3 = 1ull;
  j2 += j3;
  j3 = l6;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&memory), (u64)(i0), j1);
  B46:;
  i0 = l10;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l10;
  i1 = l0;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  l6 = j1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l10;
  i1 = p5;
  i32_store((&memory), (u64)(i0 + 80), i1);
  i0 = l10;
  i1 = 112u;
  i0 += i1;
  i1 = 28u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = l10;
  i2 = 144u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B48;}
  i0 = l4;
  j1 = l6;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l4;
  i1 = p5;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l4;
  i1 = l0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = 140u;
  i0 += i1;
  i1 = l4;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B47;
  B48:;
  i0 = l10;
  i1 = 136u;
  i0 += i1;
  i1 = l10;
  i2 = 96u;
  i1 += i2;
  i2 = l10;
  i3 = 16u;
  i2 += i3;
  i3 = l10;
  i4 = 80u;
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
  f123(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B47:;
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 96));
  l4 = i0;
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B22;}
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B22:;
  i0 = 1u;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L54: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B53;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B55;}
    L56: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B53;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L56;}
    B55:;
    i0 = 1u;
    p5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L54;}
    goto B52;
  B53:;
  i0 = 0u;
  p5 = i0;
  B52:;
  i0 = p5;
  i1 = 704u;
  i2 = g0;
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
  j0 = 1397703940ull;
  i1 = l0;
  i2 = 24u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 16));
  j1 = 63ull;
  j0 >>= (j1 & 63);
  i0 = (u32)(j0);
  i1 = 1u;
  i0 ^= i1;
  i1 = 1792u;
  i2 = g0;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L63: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B62;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B64;}
    L65: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B62;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L65;}
    B64:;
    i0 = 1u;
    p5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L63;}
    goto B61;
  B62:;
  i0 = 0u;
  p5 = i0;
  B61:;
  i0 = p5;
  i1 = 704u;
  i2 = g0;
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
  j0 = 1397703940ull;
  i1 = l0;
  i2 = 40u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 32));
  j1 = 63ull;
  j0 >>= (j1 & 63);
  i0 = (u32)(j0);
  i1 = 1u;
  i0 ^= i1;
  i1 = 1840u;
  i2 = g0;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L72: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B71;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B73;}
    L74: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B71;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L74;}
    B73:;
    i0 = 1u;
    p5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L72;}
    goto B70;
  B71:;
  i0 = 0u;
  p5 = i0;
  B70:;
  i0 = p5;
  i1 = 704u;
  i2 = g0;
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
  i1 = 24u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  i1 = 16u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  i0 = !(j0);
  i0 = !(i0);
  if (i0) {goto B77;}
  i0 = 1u;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L81: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B80;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B82;}
    L83: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B80;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L83;}
    B82:;
    i0 = 1u;
    p5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L81;}
    goto B79;
  B80:;
  i0 = 0u;
  p5 = i0;
  B79:;
  i0 = p5;
  i1 = 704u;
  i2 = g0;
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
  i1 = 40u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  i1 = 32u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 0ull;
  i0 = j0 != j1;
  if (i0) {goto B77;}
  i0 = l0;
  i1 = 0u;
  i0 = i0 != i1;
  l4 = i0;
  i1 = 1888u;
  i2 = g0;
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
  i0 = i32_load((&memory), (u64)(i0 + 52));
  i1 = l10;
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
  i0 = (*Z_envZ_db_next_i64Z_iii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B88;}
  i0 = l10;
  i1 = 112u;
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
  i0 = f120(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B88:;
  i0 = l10;
  i1 = 112u;
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
  f124(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B77:;
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 136));
  p5 = i0;
  i0 = !(i0);
  if (i0) {goto B92;}
  i0 = l10;
  i1 = 140u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = p5;
  i0 = i0 == i1;
  if (i0) {goto B94;}
  L95: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l4 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l0 = i0;
    i0 = l4;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l0;
    i0 = !(i0);
    if (i0) {goto B96;}
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
    f235(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B96:;
    i0 = p5;
    i1 = l4;
    i0 = i0 != i1;
    if (i0) {goto L95;}
  i0 = l10;
  i1 = 136u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B93;
  B94:;
  i0 = p5;
  l4 = i0;
  B93:;
  i0 = l3;
  i1 = p5;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B92:;
  i0 = l10;
  i1 = 112u;
  i0 += i1;
  i1 = 32u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 128), j1);
  i0 = l10;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 136), j1);
  i0 = l10;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  l6 = j1;
  i64_store((&memory), (u64)(i0 + 112), j1);
  i0 = l10;
  j1 = p2;
  i64_store((&memory), (u64)(i0 + 120), j1);
  j0 = l6;
  j1 = p2;
  j2 = 15426372072997126144ull;
  j3 = p2;
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
  l4 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 <= (s32)i1);
  if (i0) {goto B100;}
  i0 = l10;
  i1 = 112u;
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
  i0 = f105(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  i1 = l10;
  i2 = 112u;
  i1 += i2;
  i0 = i0 == i1;
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
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 152));
  l7 = j0;
  i0 = 1u;
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
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  i1 = l10;
  i2 = 112u;
  i1 += i2;
  i0 = i0 == i1;
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
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 112));
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
  i0 = l0;
  j0 = i64_load((&memory), (u64)(i0));
  l6 = j0;
  j0 = l5;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 1680u;
  i2 = g0;
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
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i2 = p3;
  j2 = i64_load((&memory), (u64)(i2));
  j1 += j2;
  l5 = j1;
  i64_store((&memory), (u64)(i0 + 8), j1);
  j0 = l5;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 1728u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  i1 = 1760u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i2 = 32u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 1680u;
  i2 = g0;
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
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1 + 24));
  i2 = p4;
  j2 = i64_load((&memory), (u64)(i2));
  j1 += j2;
  l5 = j1;
  i64_store((&memory), (u64)(i0 + 24), j1);
  j0 = l5;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 1728u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 24));
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  i1 = 1760u;
  i2 = g0;
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
  j0 = l6;
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 256u;
  i2 = g0;
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
  i2 = 16u;
  i1 += i2;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 104), i1);
  i0 = l10;
  i1 = l10;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 100), i1);
  i0 = l10;
  i1 = l10;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l10;
  i1 = 96u;
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
  i0 = f106(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i0 = i32_load((&memory), (u64)(i0 + 52));
  j1 = l7;
  j2 = 0ull;
  j3 = l7;
  j4 = p2;
  i3 = j3 == j4;
  j1 = i3 ? j1 : j2;
  i2 = l10;
  i3 = 16u;
  i2 += i3;
  i3 = 48u;
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
  j0 = l6;
  i1 = l10;
  i2 = 112u;
  i1 += i2;
  i2 = 16u;
  i1 += i2;
  l4 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B99;}
  i0 = l4;
  j1 = 18446744073709551614ull;
  j2 = l6;
  j3 = 1ull;
  j2 += j3;
  j3 = l6;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&memory), (u64)(i0), j1);
  goto B99;
  B100:;
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 152));
  l5 = j0;
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 112));
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
  i0 = 64u;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
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
  i0 = f107(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = l0;
  i1 = l10;
  i2 = 112u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l0;
  j1 = p2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l0;
  i1 = 20u;
  i0 += i1;
  i1 = p3;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  i1 = p3;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = 12u;
  i0 += i1;
  i1 = p3;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = p3;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l0;
  i1 = 32u;
  i0 += i1;
  i1 = p4;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l0;
  i1 = p4;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l10;
  i1 = l10;
  i2 = 16u;
  i1 += i2;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 104), i1);
  i0 = l10;
  i1 = l10;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 100), i1);
  i0 = l10;
  i1 = l10;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l10;
  i1 = 96u;
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
  i0 = f106(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l0;
  i1 = l10;
  i2 = 112u;
  i1 += i2;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 15426372072997126144ull;
  j3 = l5;
  i4 = l0;
  j4 = i64_load((&memory), (u64)(i4));
  l6 = j4;
  i5 = l10;
  i6 = 16u;
  i5 += i6;
  i6 = 48u;
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
  p5 = i1;
  i32_store((&memory), (u64)(i0 + 52), i1);
  j0 = l6;
  i1 = l10;
  i2 = 112u;
  i1 += i2;
  i2 = 16u;
  i1 += i2;
  l4 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B123;}
  i0 = l4;
  j1 = 18446744073709551614ull;
  j2 = l6;
  j3 = 1ull;
  j2 += j3;
  j3 = l6;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&memory), (u64)(i0), j1);
  B123:;
  i0 = l10;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l10;
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1));
  l6 = j1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l10;
  i1 = p5;
  i32_store((&memory), (u64)(i0 + 80), i1);
  i0 = l10;
  i1 = 140u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = l10;
  i2 = 144u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B125;}
  i0 = l4;
  j1 = l6;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l4;
  i1 = p5;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l4;
  i1 = l0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = l4;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B124;
  B125:;
  i0 = l10;
  i1 = 136u;
  i0 += i1;
  i1 = l10;
  i2 = 96u;
  i1 += i2;
  i2 = l10;
  i3 = 16u;
  i2 += i3;
  i3 = l10;
  i4 = 80u;
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
  f108(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B124:;
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 96));
  l4 = i0;
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 96), i1);
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B99;}
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B99:;
  i0 = 1u;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L131: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B130;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B132;}
    L133: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B130;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L133;}
    B132:;
    i0 = 1u;
    p5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L131;}
    goto B129;
  B130:;
  i0 = 0u;
  p5 = i0;
  B129:;
  i0 = p5;
  i1 = 704u;
  i2 = g0;
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
  j0 = 1397703940ull;
  i1 = l0;
  i2 = 16u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
  j1 = 63ull;
  j0 >>= (j1 & 63);
  i0 = (u32)(j0);
  i1 = 1u;
  i0 ^= i1;
  i1 = 1936u;
  i2 = g0;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L140: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B139;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B141;}
    L142: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B139;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L142;}
    B141:;
    i0 = 1u;
    p5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L140;}
    goto B138;
  B139:;
  i0 = 0u;
  p5 = i0;
  B138:;
  i0 = p5;
  i1 = 704u;
  i2 = g0;
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
  j0 = 1397703940ull;
  i1 = l0;
  i2 = 32u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 24));
  j1 = 63ull;
  j0 >>= (j1 & 63);
  i0 = (u32)(j0);
  i1 = 1u;
  i0 ^= i1;
  i1 = 1984u;
  i2 = g0;
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
  j0 = p2;
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1 + 40));
  i2 = l0;
  i3 = 8u;
  i2 += i3;
  j2 = i64_load((&memory), (u64)(i2));
  i3 = l0;
  j3 = i64_load((&memory), (u64)(i3 + 24));
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
  i0 = 1u;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L150: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B149;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B151;}
    L152: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B149;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L152;}
    B151:;
    i0 = 1u;
    p5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L150;}
    goto B148;
  B149:;
  i0 = 0u;
  p5 = i0;
  B148:;
  i0 = p5;
  i1 = 704u;
  i2 = g0;
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
  i1 = 16u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  i0 = !(j0);
  i0 = !(i0);
  if (i0) {goto B155;}
  i0 = 1u;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L159: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B158;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B160;}
    L161: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B158;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L161;}
    B160:;
    i0 = 1u;
    p5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L159;}
    goto B157;
  B158:;
  i0 = 0u;
  p5 = i0;
  B157:;
  i0 = p5;
  i1 = 704u;
  i2 = g0;
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
  i1 = 32u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  i1 = 24u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 0ull;
  i0 = j0 != j1;
  if (i0) {goto B155;}
  i0 = l0;
  i1 = 40u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 0ull;
  i0 = j0 != j1;
  if (i0) {goto B155;}
  i0 = l0;
  i1 = 0u;
  i0 = i0 != i1;
  l4 = i0;
  i1 = 1888u;
  i2 = g0;
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
  i0 = i32_load((&memory), (u64)(i0 + 52));
  i1 = l10;
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
  i0 = (*Z_envZ_db_next_i64Z_iii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B166;}
  i0 = l10;
  i1 = 112u;
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
  i0 = f105(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B166:;
  i0 = l10;
  i1 = 112u;
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
  f125(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B155:;
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 136));
  p5 = i0;
  i0 = !(i0);
  if (i0) {goto B170;}
  i0 = l10;
  i1 = 140u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = p5;
  i0 = i0 == i1;
  if (i0) {goto B172;}
  L173: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l4 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l0 = i0;
    i0 = l4;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l0;
    i0 = !(i0);
    if (i0) {goto B174;}
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
    f235(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B174:;
    i0 = p5;
    i1 = l4;
    i0 = i0 != i1;
    if (i0) {goto L173;}
  i0 = l10;
  i1 = 136u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B171;
  B172:;
  i0 = p5;
  l4 = i0;
  B171:;
  i0 = l3;
  i1 = p5;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B170:;
  j0 = 0ull;
  l6 = j0;
  j0 = 59ull;
  l5 = j0;
  i0 = 2032u;
  l4 = i0;
  j0 = 0ull;
  l7 = j0;
  L177: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    j1 = 10ull;
    i0 = j0 > j1;
    if (i0) {goto B182;}
    i0 = l4;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l0 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B181;}
    i0 = l0;
    i1 = 165u;
    i0 += i1;
    l0 = i0;
    goto B180;
    B182:;
    j0 = 0ull;
    p2 = j0;
    j0 = l6;
    j1 = 11ull;
    i0 = j0 == j1;
    if (i0) {goto B179;}
    goto B178;
    B181:;
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
    B180:;
    i0 = l0;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    p2 = j0;
    B179:;
    j0 = p2;
    j1 = 31ull;
    j0 &= j1;
    j1 = l5;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    p2 = j0;
    B178:;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l5;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l5 = j0;
    j0 = p2;
    j1 = l7;
    j0 |= j1;
    l7 = j0;
    j0 = l6;
    j1 = 1ull;
    j0 += j1;
    l6 = j0;
    j1 = 13ull;
    i0 = j0 != j1;
    if (i0) {goto L177;}
  j0 = l7;
  j1 = p1;
  i0 = j0 == j1;
  if (i0) {goto B184;}
  i0 = l10;
  j1 = 18446744073709551615ull;
  i64_store((&memory), (u64)(i0 + 128), j1);
  i0 = 0u;
  l0 = i0;
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 136), i1);
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 152));
  l6 = j0;
  i0 = l10;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  p2 = j1;
  i64_store((&memory), (u64)(i0 + 112), j1);
  i0 = l10;
  j1 = l6;
  i64_store((&memory), (u64)(i0 + 120), j1);
  i0 = l10;
  i1 = 140u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = 144u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  j0 = p2;
  j1 = l6;
  j2 = 13445401747262537728ull;
  j3 = l6;
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
  l4 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B185;}
  i0 = l10;
  i1 = 112u;
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
  i0 = f126(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  i1 = l10;
  i2 = 112u;
  i1 += i2;
  i0 = i0 == i1;
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
  B185:;
  i0 = l10;
  i1 = 96u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = p3;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l10;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 96), j1);
  i0 = l10;
  i1 = 80u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = p4;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l10;
  i1 = p4;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = l0;
  i0 = !(i0);
  if (i0) {goto B197;}
  i0 = l10;
  i1 = l10;
  i2 = 80u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l10;
  i1 = l10;
  i2 = 96u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = 1u;
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
  i0 = l10;
  i1 = 112u;
  i0 += i1;
  i1 = l0;
  i2 = l10;
  i3 = 16u;
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
  f127(i0, i1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 1u;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L203: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B202;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B204;}
    L205: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B202;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L205;}
    B204:;
    i0 = 1u;
    p5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L203;}
    goto B201;
  B202:;
  i0 = 0u;
  p5 = i0;
  B201:;
  i0 = p5;
  i1 = 704u;
  i2 = g0;
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
  j0 = 1397703940ull;
  i1 = l0;
  i2 = 24u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 16));
  j1 = 63ull;
  j0 >>= (j1 & 63);
  i0 = (u32)(j0);
  i1 = 1u;
  i0 ^= i1;
  i1 = 2048u;
  i2 = g0;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L212: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B211;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B213;}
    L214: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B211;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L214;}
    B213:;
    i0 = 1u;
    p5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L212;}
    goto B210;
  B211:;
  i0 = 0u;
  p5 = i0;
  B210:;
  i0 = p5;
  i1 = 704u;
  i2 = g0;
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
  j0 = 1397703940ull;
  i1 = l0;
  i2 = 40u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 32));
  j1 = 63ull;
  j0 >>= (j1 & 63);
  i0 = (u32)(j0);
  i1 = 1u;
  i0 ^= i1;
  i1 = 2080u;
  i2 = g0;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L221: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B220;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B222;}
    L223: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B220;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L223;}
    B222:;
    i0 = 1u;
    p5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L221;}
    goto B219;
  B220:;
  i0 = 0u;
  p5 = i0;
  B219:;
  i0 = p5;
  i1 = 704u;
  i2 = g0;
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
  i1 = 24u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  i1 = 16u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  i0 = !(j0);
  i0 = !(i0);
  if (i0) {goto B190;}
  i0 = 1u;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L227: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B196;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B228;}
    L229: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B196;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L229;}
    B228:;
    i0 = 1u;
    p5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L227;}
    goto B195;
  B197:;
  i0 = 1u;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L233: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B232;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B234;}
    L235: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B232;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L235;}
    B234:;
    i0 = 1u;
    l0 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L233;}
    goto B231;
  B232:;
  i0 = 0u;
  l0 = i0;
  B231:;
  i0 = l0;
  i1 = 704u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 104));
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 96));
  j1 = 0ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B191;}
  i0 = 1u;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L239: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B194;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B240;}
    L241: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B194;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L241;}
    B240:;
    i0 = 1u;
    l0 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L239;}
    goto B193;
  B196:;
  i0 = 0u;
  p5 = i0;
  B195:;
  i0 = p5;
  i1 = 704u;
  i2 = g0;
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
  i1 = 40u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  i1 = 32u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 0ull;
  i0 = j0 != j1;
  if (i0) {goto B190;}
  i0 = 1u;
  i1 = 1888u;
  i2 = g0;
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
  i0 = i32_load((&memory), (u64)(i0 + 52));
  i1 = l10;
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
  i0 = (*Z_envZ_db_next_i64Z_iii)(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B246;}
  i0 = l10;
  i1 = 112u;
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
  i0 = f126(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B246:;
  i0 = l10;
  i1 = 112u;
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
  f128(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B192;
  B194:;
  i0 = 0u;
  l0 = i0;
  B193:;
  i0 = l0;
  i1 = 704u;
  i2 = g0;
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
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 80));
  j1 = 18446744073709551615ull;
  i0 = (u64)((s64)j0 <= (s64)j1);
  if (i0) {goto B191;}
  B192:;
  i0 = l10;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l10;
  i1 = l10;
  j1 = i64_load((&memory), (u64)(i1 + 152));
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l10;
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
  i0 = (*Z_envZ_cancel_deferredZ_ii)(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B189;
  B191:;
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 152));
  l6 = j0;
  i0 = l10;
  i1 = l10;
  i2 = 96u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l10;
  i1 = l10;
  i2 = 152u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = l10;
  i2 = 80u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l10;
  j1 = l6;
  i64_store((&memory), (u64)(i0 + 176), j1);
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 112));
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
  i0 = l10;
  i1 = l10;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l10;
  i1 = l10;
  i2 = 112u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l10;
  i1 = l10;
  i2 = 176u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = 64u;
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
  i0 = f234(i0);
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
  i0 = f129(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = l10;
  i2 = 112u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l10;
  i1 = 16u;
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
  f130(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l10;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 168), i1);
  i0 = l10;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1));
  l6 = j1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l10;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1 + 52));
  p5 = i1;
  i32_store((&memory), (u64)(i0 + 164), i1);
  i0 = l10;
  i1 = 140u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i1 = l10;
  i2 = 144u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B259;}
  i0 = l0;
  j1 = l6;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l0;
  i1 = p5;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 168), i1);
  i0 = l0;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = l0;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B258;
  B259:;
  i0 = l10;
  i1 = 136u;
  i0 += i1;
  i1 = l10;
  i2 = 168u;
  i1 += i2;
  i2 = l10;
  i3 = 16u;
  i2 += i3;
  i3 = l10;
  i4 = 164u;
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
  f131(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B258:;
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 168));
  l4 = i0;
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 168), i1);
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B190;}
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B190:;
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
  l6 = j0;
  i0 = l10;
  i1 = 44u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = 48u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 28), i1);
  i0 = l10;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0 + 32), i1);
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 36), i1);
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 40), i1);
  i0 = l10;
  j1 = l6;
  j2 = 1000000ull;
  j1 = DIV_U(j1, j2);
  i1 = (u32)(j1);
  i2 = 60u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 52), i1);
  i0 = l10;
  i1 = 56u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = 60u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = l10;
  i1 = 68u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = 72u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = 52u;
  i0 += i1;
  p5 = i0;
  j0 = 0ull;
  l6 = j0;
  j0 = 59ull;
  l5 = j0;
  i0 = 688u;
  l4 = i0;
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 152));
  l8 = j0;
  j0 = 0ull;
  l7 = j0;
  L263: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    j1 = 5ull;
    i0 = j0 > j1;
    if (i0) {goto B268;}
    i0 = l4;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l0 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B267;}
    i0 = l0;
    i1 = 165u;
    i0 += i1;
    l0 = i0;
    goto B266;
    B268:;
    j0 = 0ull;
    p2 = j0;
    j0 = l6;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B265;}
    goto B264;
    B267:;
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
    B266:;
    i0 = l0;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    p2 = j0;
    B265:;
    j0 = p2;
    j1 = 31ull;
    j0 &= j1;
    j1 = l5;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    p2 = j0;
    B264:;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l6;
    j1 = 1ull;
    j0 += j1;
    l6 = j0;
    j0 = p2;
    j1 = l7;
    j0 |= j1;
    l7 = j0;
    j0 = l5;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l5 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L263;}
  i0 = l10;
  j1 = l7;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l10;
  j1 = l8;
  i64_store((&memory), (u64)(i0), j1);
  j0 = 0ull;
  l6 = j0;
  j0 = 59ull;
  l5 = j0;
  i0 = 2112u;
  l4 = i0;
  j0 = 0ull;
  l7 = j0;
  L269: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    j1 = 5ull;
    i0 = j0 > j1;
    if (i0) {goto B274;}
    i0 = l4;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l0 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B273;}
    i0 = l0;
    i1 = 165u;
    i0 += i1;
    l0 = i0;
    goto B272;
    B274:;
    j0 = 0ull;
    p2 = j0;
    j0 = l6;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B271;}
    goto B270;
    B273:;
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
    B272:;
    i0 = l0;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    p2 = j0;
    B271:;
    j0 = p2;
    j1 = 31ull;
    j0 &= j1;
    j1 = l5;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    p2 = j0;
    B270:;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l6;
    j1 = 1ull;
    j0 += j1;
    l6 = j0;
    j0 = p2;
    j1 = l7;
    j0 |= j1;
    l7 = j0;
    j0 = l5;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l5 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L269;}
  i0 = l10;
  j1 = l7;
  i64_store((&memory), (u64)(i0 + 176), j1);
  i0 = p5;
  i1 = l10;
  i2 = p0;
  i3 = l10;
  i4 = 176u;
  i3 += i4;
  i4 = l10;
  i5 = 152u;
  i4 += i5;
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
  f132(i0, i1, i2, i3, i4);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 152));
  l6 = j0;
  i0 = l10;
  i1 = 36u;
  i0 += i1;
  i1 = 259200u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l10;
  j1 = l6;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l10;
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
  i0 = (*Z_envZ_cancel_deferredZ_ii)(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l10;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l10;
  i1 = l10;
  j1 = i64_load((&memory), (u64)(i1 + 152));
  l6 = j1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l10;
  i1 = 176u;
  i0 += i1;
  i1 = l10;
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
  f133(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l10;
  j1 = l6;
  i2 = l10;
  i2 = i32_load((&memory), (u64)(i2 + 176));
  l4 = i2;
  i3 = l10;
  i3 = i32_load((&memory), (u64)(i3 + 180));
  i4 = l4;
  i3 -= i4;
  i4 = 1u;
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
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 176));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B279;}
  i0 = l10;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 180), i1);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B279:;
  i0 = l10;
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
  i0 = f134(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B189:;
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 96));
  l6 = j0;
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 88));
  i1 = l10;
  j1 = i64_load((&memory), (u64)(i1 + 104));
  l1 = j1;
  i0 = j0 == j1;
  i1 = 1680u;
  i2 = g0;
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
  j0 = l6;
  i1 = l10;
  j1 = i64_load((&memory), (u64)(i1 + 80));
  j0 += j1;
  l2 = j0;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 1728u;
  i2 = g0;
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
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  i1 = 1760u;
  i2 = g0;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L288: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B287;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B289;}
    L290: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B287;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L290;}
    B289:;
    i0 = 1u;
    l0 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L288;}
    goto B286;
  B287:;
  i0 = 0u;
  l0 = i0;
  B286:;
  i0 = l0;
  i1 = 704u;
  i2 = g0;
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
  j0 = 1397703940ull;
  j1 = l1;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j1 = 1ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B293;}
  j0 = 0ull;
  l6 = j0;
  j0 = 59ull;
  l5 = j0;
  i0 = 672u;
  l4 = i0;
  j0 = 0ull;
  l7 = j0;
  L294: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    j1 = 10ull;
    i0 = j0 > j1;
    if (i0) {goto B299;}
    i0 = l4;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l0 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B298;}
    i0 = l0;
    i1 = 165u;
    i0 += i1;
    l0 = i0;
    goto B297;
    B299:;
    j0 = 0ull;
    p2 = j0;
    j0 = l6;
    j1 = 11ull;
    i0 = j0 == j1;
    if (i0) {goto B296;}
    goto B295;
    B298:;
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
    B297:;
    i0 = l0;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    p2 = j0;
    B296:;
    j0 = p2;
    j1 = 31ull;
    j0 &= j1;
    j1 = l5;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    p2 = j0;
    B295:;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l5;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l5 = j0;
    j0 = p2;
    j1 = l7;
    j0 |= j1;
    l7 = j0;
    j0 = l6;
    j1 = 1ull;
    j0 += j1;
    l6 = j0;
    j1 = 13ull;
    i0 = j0 != j1;
    if (i0) {goto L294;}
  j0 = 0ull;
  l6 = j0;
  j0 = 59ull;
  l5 = j0;
  i0 = 688u;
  l4 = i0;
  j0 = 0ull;
  l8 = j0;
  L300: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    j1 = 5ull;
    i0 = j0 > j1;
    if (i0) {goto B305;}
    i0 = l4;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l0 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B304;}
    i0 = l0;
    i1 = 165u;
    i0 += i1;
    l0 = i0;
    goto B303;
    B305:;
    j0 = 0ull;
    p2 = j0;
    j0 = l6;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B302;}
    goto B301;
    B304:;
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
    B303:;
    i0 = l0;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    p2 = j0;
    B302:;
    j0 = p2;
    j1 = 31ull;
    j0 &= j1;
    j1 = l5;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    p2 = j0;
    B301:;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l6;
    j1 = 1ull;
    j0 += j1;
    l6 = j0;
    j0 = p2;
    j1 = l8;
    j0 |= j1;
    l8 = j0;
    j0 = l5;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l5 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L300;}
  j0 = 0ull;
  l6 = j0;
  j0 = 59ull;
  l5 = j0;
  i0 = 2032u;
  l4 = i0;
  j0 = 0ull;
  l9 = j0;
  L306: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    j1 = 10ull;
    i0 = j0 > j1;
    if (i0) {goto B311;}
    i0 = l4;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l0 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B310;}
    i0 = l0;
    i1 = 165u;
    i0 += i1;
    l0 = i0;
    goto B309;
    B311:;
    j0 = 0ull;
    p2 = j0;
    j0 = l6;
    j1 = 11ull;
    i0 = j0 == j1;
    if (i0) {goto B308;}
    goto B307;
    B310:;
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
    B309:;
    i0 = l0;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    p2 = j0;
    B308:;
    j0 = p2;
    j1 = 31ull;
    j0 &= j1;
    j1 = l5;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    p2 = j0;
    B307:;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l5;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l5 = j0;
    j0 = p2;
    j1 = l9;
    j0 |= j1;
    l9 = j0;
    j0 = l6;
    j1 = 1ull;
    j0 += j1;
    l6 = j0;
    j1 = 13ull;
    i0 = j0 != j1;
    if (i0) {goto L306;}
  i0 = l10;
  i1 = 184u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 176), j1);
  i0 = 2128u;
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
  i0 = strlen_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i1 = 4294967280u;
  i0 = i0 >= i1;
  if (i0) {goto B183;}
  i0 = l4;
  i1 = 11u;
  i0 = i0 >= i1;
  if (i0) {goto B315;}
  i0 = l10;
  i1 = l4;
  i2 = 1u;
  i1 <<= (i2 & 31);
  i32_store8((&memory), (u64)(i0 + 176), i1);
  i0 = l10;
  i1 = 176u;
  i0 += i1;
  i1 = 1u;
  i0 |= i1;
  l0 = i0;
  i0 = l4;
  if (i0) {goto B314;}
  goto B313;
  B315:;
  i0 = l4;
  i1 = 16u;
  i0 += i1;
  i1 = 4294967280u;
  i0 &= i1;
  p5 = i0;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l0 = i0;
  i0 = l10;
  i1 = p5;
  i2 = 1u;
  i1 |= i2;
  i32_store((&memory), (u64)(i0 + 176), i1);
  i0 = l10;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 184), i1);
  i0 = l10;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 180), i1);
  B314:;
  i0 = l0;
  i1 = 2128u;
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
  B313:;
  i0 = l0;
  i1 = l4;
  i0 += i1;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 176));
  l0 = i0;
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 176), i1);
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 180));
  p5 = i0;
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 180), i1);
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 184));
  l3 = i0;
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 184), i1);
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  j1 = p1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l4;
  j1 = l8;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l10;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = l4;
  i2 = 16u;
  i1 += i2;
  l4 = i1;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l10;
  j1 = p1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l10;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l10;
  j1 = l9;
  i64_store((&memory), (u64)(i0 + 24), j1);
  i0 = l10;
  j1 = l2;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l10;
  i1 = 40u;
  i0 += i1;
  j1 = l1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l10;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l10;
  i1 = 52u;
  i0 += i1;
  i1 = p5;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = 56u;
  i0 += i1;
  l4 = i0;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  j0 = l7;
  j1 = 14829575313431724032ull;
  i2 = l10;
  i3 = l10;
  i4 = 16u;
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
  f91(j0, j1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l10;
  i0 = i32_load8_u((&memory), (u64)(i0 + 48));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B320;}
  i0 = l4;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B320:;
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B322;}
  i0 = l10;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 4), i1);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B322:;
  i0 = l10;
  i0 = i32_load8_u((&memory), (u64)(i0 + 176));
  i1 = 1u;
  i0 &= i1;
  i0 = !(i0);
  if (i0) {goto B293;}
  i0 = l10;
  i1 = 184u;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B293:;
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0 + 136));
  p5 = i0;
  i0 = !(i0);
  if (i0) {goto B184;}
  i0 = l10;
  i1 = 140u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = p5;
  i0 = i0 == i1;
  if (i0) {goto B326;}
  L327: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l4 = i0;
    i0 = i32_load((&memory), (u64)(i0));
    l0 = i0;
    i0 = l4;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l0;
    i0 = !(i0);
    if (i0) {goto B328;}
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
    f235(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B328:;
    i0 = p5;
    i1 = l4;
    i0 = i0 != i1;
    if (i0) {goto L327;}
  i0 = l10;
  i1 = 136u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B325;
  B326:;
  i0 = p5;
  l4 = i0;
  B325:;
  i0 = l3;
  i1 = p5;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B184:;
  i0 = l10;
  i1 = 16u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = p3;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  l6 = j1;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l10;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 16), j1);
  j0 = l6;
  i1 = p4;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i0 = j0 == j1;
  i1 = 1680u;
  i2 = g0;
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
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i2 = p4;
  j2 = i64_load((&memory), (u64)(i2));
  j1 += j2;
  l6 = j1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  j0 = l6;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 1728u;
  i2 = g0;
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
  j0 = l6;
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  i1 = 1760u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 152));
  l6 = j0;
  i0 = p0;
  i1 = 36u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p5 = i0;
  i1 = p0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  p4 = i1;
  i0 = i0 == i1;
  if (i0) {goto B334;}
  i0 = p5;
  i1 = 4294967272u;
  i0 += i1;
  l4 = i0;
  i0 = 0u;
  i1 = p4;
  i0 -= i1;
  p3 = i0;
  L335: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0));
    j0 = i64_load((&memory), (u64)(i0));
    j1 = l6;
    i0 = j0 == j1;
    if (i0) {goto B334;}
    i0 = l4;
    p5 = i0;
    i0 = l4;
    i1 = 4294967272u;
    i0 += i1;
    l0 = i0;
    l4 = i0;
    i0 = l0;
    i1 = p3;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L335;}
  B334:;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i0 = p5;
  i1 = p4;
  i0 = i0 == i1;
  if (i0) {goto B339;}
  i0 = p5;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p5 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = l0;
  i0 = i0 == i1;
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
  i0 = p5;
  if (i0) {goto B338;}
  goto B337;
  B339:;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p0;
  i2 = 16u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 15938991009778630656ull;
  j3 = l6;
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
  l4 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B337;}
  i0 = l0;
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
  i0 = f135(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p5 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = l0;
  i0 = i0 == i1;
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
  B338:;
  i0 = l10;
  i1 = l10;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 112), i1);
  i0 = 1u;
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
  i0 = l0;
  i1 = p5;
  i2 = l10;
  i3 = 112u;
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
  f138(i0, i1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B336;
  B337:;
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 152));
  l6 = j0;
  i0 = l10;
  i1 = l10;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 84), i1);
  i0 = l10;
  i1 = l10;
  i2 = 152u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 80), i1);
  i0 = l10;
  j1 = l6;
  i64_store((&memory), (u64)(i0 + 96), j1);
  i0 = l0;
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
  i0 = l10;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 112), i1);
  i0 = l10;
  i1 = l10;
  i2 = 80u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 116), i1);
  i0 = l10;
  i1 = l10;
  i2 = 96u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 120), i1);
  i0 = 104u;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p5 = i0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p5;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p5;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = p5;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = p5;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = p5;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = p5;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = p5;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0 + 56), i1);
  i0 = p5;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 60), i1);
  i0 = p5;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = p5;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 72), j1);
  i0 = p5;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0 + 80), j1);
  i0 = 1u;
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
  j0 = 5459781ull;
  l6 = j0;
  i0 = 0u;
  l4 = i0;
  L352: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B351;}
    j0 = l6;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l6 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B353;}
    L354: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l6;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l6 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B351;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L354;}
    B353:;
    i0 = 1u;
    p3 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L352;}
    goto B350;
  B351:;
  i0 = 0u;
  p3 = i0;
  B350:;
  i0 = p3;
  i1 = 704u;
  i2 = g0;
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
  i0 = p5;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 88), i1);
  i0 = l10;
  i1 = 112u;
  i0 += i1;
  i1 = p5;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f136(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l10;
  i1 = p5;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l10;
  i1 = p5;
  j1 = i64_load((&memory), (u64)(i1));
  l6 = j1;
  i64_store((&memory), (u64)(i0 + 112), j1);
  i0 = l10;
  i1 = p5;
  i1 = i32_load((&memory), (u64)(i1 + 92));
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 176), i1);
  i0 = p0;
  i1 = 36u;
  i0 += i1;
  p3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = p0;
  i2 = 40u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B358;}
  i0 = l4;
  j1 = l6;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l4;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l4;
  i1 = p5;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p3;
  i1 = l4;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B357;
  B358:;
  i0 = p0;
  i1 = 32u;
  i0 += i1;
  i1 = l10;
  i2 = l10;
  i3 = 112u;
  i2 += i3;
  i3 = l10;
  i4 = 176u;
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
  f137(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B357:;
  i0 = l10;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i0 = l10;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B336;}
  i0 = l4;
  i0 = i32_load((&memory), (u64)(i0 + 16));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B360;}
  i0 = l4;
  i1 = 20u;
  i0 += i1;
  i1 = l0;
  i32_store((&memory), (u64)(i0), i1);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B360:;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B336:;
  i0 = p5;
  j0 = i64_load((&memory), (u64)(i0 + 32));
  j1 = 63ull;
  j0 >>= (j1 & 63);
  i0 = (u32)(j0);
  i1 = 1u;
  i0 ^= i1;
  i1 = 2144u;
  i2 = g0;
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
  j0 = 0ull;
  l6 = j0;
  j0 = 59ull;
  l5 = j0;
  i0 = 2192u;
  l4 = i0;
  i0 = l10;
  j0 = i64_load((&memory), (u64)(i0 + 152));
  l8 = j0;
  j0 = 0ull;
  l7 = j0;
  L364: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l6;
    j1 = 1ull;
    i0 = j0 > j1;
    if (i0) {goto B369;}
    i0 = l4;
    i0 = i32_load8_s((&memory), (u64)(i0));
    l0 = i0;
    i1 = 4294967199u;
    i0 += i1;
    i1 = 255u;
    i0 &= i1;
    i1 = 25u;
    i0 = i0 > i1;
    if (i0) {goto B368;}
    i0 = l0;
    i1 = 165u;
    i0 += i1;
    l0 = i0;
    goto B367;
    B369:;
    j0 = 0ull;
    p2 = j0;
    j0 = l6;
    j1 = 11ull;
    i0 = j0 <= j1;
    if (i0) {goto B366;}
    goto B365;
    B368:;
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
    B367:;
    i0 = l0;
    j0 = (u64)(i0);
    j1 = 56ull;
    j0 <<= (j1 & 63);
    j1 = 56ull;
    j0 = (u64)((s64)j0 >> (j1 & 63));
    p2 = j0;
    B366:;
    j0 = p2;
    j1 = 31ull;
    j0 &= j1;
    j1 = l5;
    j2 = 4294967295ull;
    j1 &= j2;
    j0 <<= (j1 & 63);
    p2 = j0;
    B365:;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l6;
    j1 = 1ull;
    j0 += j1;
    l6 = j0;
    j0 = p2;
    j1 = l7;
    j0 |= j1;
    l7 = j0;
    j0 = l5;
    j1 = 18446744073709551611ull;
    j0 += j1;
    l5 = j0;
    j1 = 18446744073709551610ull;
    i0 = j0 != j1;
    if (i0) {goto L364;}
  j0 = l8;
  j1 = l7;
  i0 = j0 != j1;
  if (i0) {goto B370;}
  i0 = p5;
  i1 = 32u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  l6 = j0;
  j0 = 1000000000000ull;
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
  j1 = (*Z_envZ_current_timeZ_jv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j2 = 1000000ull;
  j1 = DIV_U(j1, j2);
  j2 = 4294967295ull;
  j1 &= j2;
  j2 = 18446744072181740416ull;
  j1 += j2;
  d1 = (f64)(s64)(j1);
  d2 = 1000000000000;
  d1 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d1, d2);
  d2 = 314496000;
  d1 = (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(d1, d2);
  j1 = (*Z_eosio_injectionZ__eosio_f64_trunc_i64sZ_jd)(d1);
  j0 -= j1;
  j1 = l6;
  i0 = (u64)((s64)j0 <= (s64)j1);
  i1 = 1536u;
  i2 = g0;
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
  B370:;
  i0 = p5;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l6 = j0;
  i0 = p5;
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p5;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  i0 = i0 != i1;
  if (i0) {goto B374;}
  j0 = l6;
  i0 = !(j0);
  if (i0) {goto B373;}
  B374:;
  i0 = p0;
  i1 = l10;
  j1 = i64_load((&memory), (u64)(i1 + 152));
  j2 = l6;
  i3 = p5;
  i4 = 16u;
  i3 += i4;
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
  _ZN11eosiosystem15system_contract12update_votesEyyRKNSt3__16vectorIyNS1_9allocatorIyEEEEb(i0, j1, j2, i3, i4);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B373:;
  i0 = 0u;
  i1 = l10;
  i2 = 192u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  goto Bfunc;
  B183:;
  i0 = l10;
  i1 = 176u;
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
  f236(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
}

static u32 f120(u32 p0, u32 p1) {
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
  i32_store((&memory), (u64)(i0 + 40), i1);
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
  i0 = 64u;
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
  i0 = f234(i0);
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
  i0 = f122(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = l4;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l6;
  i1 = 32u;
  i0 += i1;
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
  i0 = f165(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 52), i1);
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l6;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  l3 = j1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l6;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1 + 52));
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
  f123(i0, i1, i2, i3);
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
  f235(i0);
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

static u32 f121(u32 p0, u32 p1) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 32u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 40u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f122(u32 p0) {
  u32 l1 = 0, l2 = 0;
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  l1 = i0;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = 1u;
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
  i0 = l1;
  j0 = i64_load((&memory), (u64)(i0));
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
  i1 = 704u;
  i2 = g0;
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
  i1 = 40u;
  i0 += i1;
  l1 = i0;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = 1u;
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
  i1 = 704u;
  i2 = g0;
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

static void f123(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f234(i0);
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
  f241(i0);
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
    f235(i0);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static void f124(u32 p0, u32 p1) {
  u32 l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 2704u;
  i2 = g0;
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
  i1 = 2752u;
  i2 = g0;
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
  i1 = 2816u;
  i2 = g0;
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
    f235(i0);
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
    f235(i0);
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
  i0 = i32_load((&memory), (u64)(i0 + 52));
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

static void f125(u32 p0, u32 p1) {
  u32 l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 2704u;
  i2 = g0;
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
  i1 = 2752u;
  i2 = g0;
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
  i1 = 2816u;
  i2 = g0;
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
    f235(i0);
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
    f235(i0);
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
  i0 = i32_load((&memory), (u64)(i0 + 52));
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

static u32 f126(u32 p0, u32 p1) {
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
  i32_store((&memory), (u64)(i0 + 40), i1);
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
  i0 = 64u;
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
  i0 = f234(i0);
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
  i0 = f129(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = l4;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l6;
  i1 = 32u;
  i0 += i1;
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
  i0 = f164(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l4;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 52), i1);
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
  i1 = i32_load((&memory), (u64)(i1 + 52));
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
  f131(i0, i1, i2, i3);
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
  f235(i0);
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

static void f127(u32 p0, u32 p1, u32 p2) {
  u32 l1 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0, l2 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 16u;
  i0 -= i1;
  l6 = i0;
  l5 = i0;
  i0 = 0u;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  i1 = p0;
  i0 = i0 == i1;
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
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = 1u;
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
  j0 = 5459781ull;
  l2 = j0;
  i0 = 0u;
  l3 = i0;
  L6: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l2;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B5;}
    j0 = l2;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l2 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B7;}
    L8: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l2;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l2 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B5;}
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      l3 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L8;}
    B7:;
    i0 = 1u;
    l4 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L6;}
    goto B4;
  B5:;
  i0 = 0u;
  l4 = i0;
  B4:;
  i0 = l4;
  i1 = 704u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j1 = 0ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B12;}
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l1 = i0;
  i0 = 1u;
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
  j0 = 5459781ull;
  l2 = j0;
  i0 = 0u;
  l3 = i0;
  L16: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l2;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B15;}
    j0 = l2;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l2 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B17;}
    L18: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l2;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l2 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B15;}
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      l3 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L18;}
    B17:;
    i0 = 1u;
    l4 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L16;}
    goto B14;
  B15:;
  i0 = 0u;
  l4 = i0;
  B14:;
  i0 = l4;
  i1 = 704u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j1 = 18446744073709551615ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  if (i0) {goto B11;}
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
  j1 = (*Z_envZ_current_timeZ_jv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j2 = 1000000ull;
  j1 = DIV_U(j1, j2);
  i64_store32((&memory), (u64)(i0 + 8), j1);
  B11:;
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  i1 = p1;
  i2 = 24u;
  i1 += i2;
  l1 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 2880u;
  i2 = g0;
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
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i2 = l3;
  j2 = i64_load((&memory), (u64)(i2));
  j1 -= j2;
  l2 = j1;
  i64_store((&memory), (u64)(i0 + 16), j1);
  j0 = l2;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 2928u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 16));
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  i1 = 2960u;
  i2 = g0;
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
  j0 = 5459781ull;
  l2 = j0;
  i0 = 0u;
  l3 = i0;
  L28: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l2;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B27;}
    j0 = l2;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l2 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B29;}
    L30: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l2;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l2 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B27;}
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      l3 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L30;}
    B29:;
    i0 = 1u;
    l4 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L28;}
    goto B26;
  B27:;
  i0 = 0u;
  l4 = i0;
  B26:;
  i0 = l4;
  i1 = 704u;
  i2 = g0;
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
  i1 = 24u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0));
  l2 = j0;
  j1 = 0ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B34;}
  i0 = 1u;
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
  j0 = 5459781ull;
  l2 = j0;
  i0 = 0u;
  l3 = i0;
  L38: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l2;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B37;}
    j0 = l2;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l2 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B39;}
    L40: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l2;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l2 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B37;}
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      l3 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L40;}
    B39:;
    i0 = 1u;
    l4 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L38;}
    goto B36;
  B37:;
  i0 = 0u;
  l4 = i0;
  B36:;
  i0 = l4;
  i1 = 704u;
  i2 = g0;
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
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l3;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  goto B33;
  B34:;
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i1 = l1;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l3;
  j1 = 0ull;
  j2 = l2;
  j1 -= j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = 1u;
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
  j0 = 5459781ull;
  l2 = j0;
  i0 = 0u;
  l3 = i0;
  L45: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l2;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B44;}
    j0 = l2;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l2 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B46;}
    L47: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l2;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l2 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B44;}
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      l3 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L47;}
    B46:;
    i0 = 1u;
    l4 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L45;}
    goto B43;
  B44:;
  i0 = 0u;
  l4 = i0;
  B43:;
  i0 = l4;
  i1 = 704u;
  i2 = g0;
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
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  B33:;
  i0 = l1;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l3 = i0;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  i1 = p1;
  i2 = 40u;
  i1 += i2;
  l1 = i1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 2880u;
  i2 = g0;
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
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1 + 32));
  i2 = l3;
  j2 = i64_load((&memory), (u64)(i2));
  j1 -= j2;
  l2 = j1;
  i64_store((&memory), (u64)(i0 + 32), j1);
  j0 = l2;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 2928u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 32));
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  i1 = 2960u;
  i2 = g0;
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
  j0 = 5459781ull;
  l2 = j0;
  i0 = 0u;
  l3 = i0;
  L55: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l2;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B54;}
    j0 = l2;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l2 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B56;}
    L57: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l2;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l2 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B54;}
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      l3 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L57;}
    B56:;
    i0 = 1u;
    l4 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L55;}
    goto B53;
  B54:;
  i0 = 0u;
  l4 = i0;
  B53:;
  i0 = l4;
  i1 = 704u;
  i2 = g0;
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
  i1 = 40u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  i1 = 32u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  l2 = j0;
  j1 = 0ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B61;}
  i0 = 1u;
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
  j0 = 5459781ull;
  l2 = j0;
  i0 = 0u;
  l3 = i0;
  L65: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l2;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B64;}
    j0 = l2;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l2 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B66;}
    L67: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l2;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l2 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B64;}
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      l3 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L67;}
    B66:;
    i0 = 1u;
    l4 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L65;}
    goto B63;
  B64:;
  i0 = 0u;
  l4 = i0;
  B63:;
  i0 = l4;
  i1 = 704u;
  i2 = g0;
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
  i0 = p2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l3;
  i1 = 8u;
  i0 += i1;
  l1 = i0;
  goto B60;
  B61:;
  i0 = p2;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i1 = l1;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l3;
  j1 = 0ull;
  j2 = l2;
  j1 -= j2;
  i64_store((&memory), (u64)(i0), j1);
  i0 = 1u;
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
  j0 = 5459781ull;
  l2 = j0;
  i0 = 0u;
  l3 = i0;
  L72: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l2;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B71;}
    j0 = l2;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l2 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B73;}
    L74: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l2;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l2 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B71;}
      i0 = l3;
      i1 = 1u;
      i0 += i1;
      l3 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L74;}
    B73:;
    i0 = 1u;
    p2 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L72;}
    goto B70;
  B71:;
  i0 = 0u;
  p2 = i0;
  B70:;
  i0 = p2;
  i1 = 704u;
  i2 = g0;
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
  i1 = 32u;
  i0 += i1;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  B60:;
  i0 = l1;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  j0 = l0;
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 == j1;
  i1 = 256u;
  i2 = g0;
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
  i1 = l6;
  p2 = i1;
  i2 = 4294967248u;
  i1 += i2;
  l3 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l5;
  i1 = l3;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l5;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = p2;
  i2 = 4294967292u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
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
  i0 = f163(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 52));
  j1 = 0ull;
  i2 = l3;
  i3 = 44u;
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
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B79;}
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
  B79:;
  i0 = 0u;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f128(u32 p0, u32 p1) {
  u32 l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  i1 = p0;
  i0 = i0 == i1;
  i1 = 2704u;
  i2 = g0;
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
  i1 = 2752u;
  i2 = g0;
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
  i1 = 2816u;
  i2 = g0;
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
    f235(i0);
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
    f235(i0);
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
  i0 = i32_load((&memory), (u64)(i0 + 52));
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

static u32 f129(u32 p0) {
  u32 l1 = 0, l2 = 0;
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  l1 = i0;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = 1u;
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
  i0 = l1;
  j0 = i64_load((&memory), (u64)(i0));
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
  i1 = 704u;
  i2 = g0;
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
  i1 = 40u;
  i0 += i1;
  l1 = i0;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = 1u;
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
  i1 = 704u;
  i2 = g0;
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

static void f130(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  u64 l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7, 
      i8;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 16u;
  i0 -= i1;
  l7 = i0;
  l6 = i0;
  i0 = 0u;
  i1 = l7;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l1 = i1;
  i1 = i32_load((&memory), (u64)(i1));
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i0 = l1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l2 = i0;
  i0 = 1u;
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
  j0 = 5459781ull;
  l3 = j0;
  i0 = 0u;
  l4 = i0;
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
    l3 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B4;}
    L5: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B2;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L5;}
    B4:;
    i0 = 1u;
    l5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L3;}
    goto B1;
  B2:;
  i0 = 0u;
  l5 = i0;
  B1:;
  i0 = l5;
  i1 = 704u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 0ull;
  i0 = (u64)((s64)j0 >= (s64)j1);
  if (i0) {goto B8;}
  i0 = l1;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  j0 = i64_load((&memory), (u64)(i0));
  l3 = j0;
  i0 = p1;
  i1 = 24u;
  i0 += i1;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p1;
  j1 = 0ull;
  j2 = l3;
  j1 -= j2;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = 1u;
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
  j0 = 5459781ull;
  l3 = j0;
  i0 = 0u;
  l4 = i0;
  L12: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B11;}
    j0 = l3;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l3 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B13;}
    L14: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B11;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L14;}
    B13:;
    i0 = 1u;
    l5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L12;}
    goto B10;
  B11:;
  i0 = 0u;
  l5 = i0;
  B10:;
  i0 = l5;
  i1 = 704u;
  i2 = g0;
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
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  B8:;
  i0 = l1;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l2 = i0;
  i0 = 1u;
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
  j0 = 5459781ull;
  l3 = j0;
  i0 = 0u;
  l4 = i0;
  L19: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B18;}
    j0 = l3;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l3 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B20;}
    L21: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B18;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L21;}
    B20:;
    i0 = 1u;
    l5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L19;}
    goto B17;
  B18:;
  i0 = 0u;
  l5 = i0;
  B17:;
  i0 = l5;
  i1 = 704u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 0ull;
  i0 = (u64)((s64)j0 >= (s64)j1);
  if (i0) {goto B24;}
  i0 = l1;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  j0 = i64_load((&memory), (u64)(i0));
  l3 = j0;
  i0 = p1;
  i1 = 40u;
  i0 += i1;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p1;
  j1 = 0ull;
  j2 = l3;
  j1 -= j2;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = 1u;
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
  j0 = 5459781ull;
  l3 = j0;
  i0 = 0u;
  l4 = i0;
  L28: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B27;}
    j0 = l3;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l3 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B29;}
    L30: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B27;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L30;}
    B29:;
    i0 = 1u;
    l5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L28;}
    goto B26;
  B27:;
  i0 = 0u;
  l5 = i0;
  B26:;
  i0 = l5;
  i1 = 704u;
  i2 = g0;
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
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l4;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  B24:;
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
  j1 = (*Z_envZ_current_timeZ_jv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j2 = 1000000ull;
  j1 = DIV_U(j1, j2);
  i64_store32((&memory), (u64)(i0 + 8), j1);
  i0 = 0u;
  i1 = l7;
  l5 = i1;
  i2 = 4294967248u;
  i1 += i2;
  l4 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = l5;
  i2 = 4294967292u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8), i1);
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
  i0 = f163(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 13445401747262537728ull;
  i3 = p0;
  i3 = i32_load((&memory), (u64)(i3 + 8));
  j3 = i64_load((&memory), (u64)(i3));
  i4 = p1;
  j4 = i64_load((&memory), (u64)(i4));
  l3 = j4;
  i5 = l4;
  i6 = 44u;
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
  i32_store((&memory), (u64)(i0 + 52), i1);
  j0 = l3;
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B35;}
  i0 = l0;
  i1 = 16u;
  i0 += i1;
  j1 = 18446744073709551614ull;
  j2 = l3;
  j3 = 1ull;
  j2 += j3;
  j3 = l3;
  j4 = 18446744073709551613ull;
  i3 = j3 > j4;
  j1 = i3 ? j1 : j2;
  i64_store((&memory), (u64)(i0), j1);
  B35:;
  i0 = 0u;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f131(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f234(i0);
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
  f241(i0);
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
    f235(i0);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  FUNC_EPILOGUE;
}

static void f132(u32 p0, u32 p1, u32 p2, u32 p3, u32 p4) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  l3 = i1;
  i0 -= i1;
  i1 = 40u;
  i0 = I32_DIV_S(i0, i1);
  l0 = i0;
  i1 = 1u;
  i0 += i1;
  l2 = i0;
  i1 = 107374183u;
  i0 = i0 >= i1;
  if (i0) {goto B1;}
  i0 = 107374182u;
  l1 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l3;
  i0 -= i1;
  i1 = 40u;
  i0 = I32_DIV_S(i0, i1);
  l3 = i0;
  i1 = 53687090u;
  i0 = i0 > i1;
  if (i0) {goto B3;}
  i0 = l2;
  i1 = l3;
  i2 = 1u;
  i1 <<= (i2 & 31);
  l3 = i1;
  i2 = l3;
  i3 = l2;
  i2 = i2 < i3;
  i0 = i2 ? i0 : i1;
  l1 = i0;
  i0 = !(i0);
  if (i0) {goto B2;}
  B3:;
  i0 = l1;
  i1 = 40u;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  goto B0;
  B2:;
  i0 = 0u;
  l1 = i0;
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
  f241(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  B0:;
  i0 = l2;
  i1 = l0;
  i2 = 40u;
  i1 *= i2;
  i0 += i1;
  l3 = i0;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l3;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l3;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l3;
  i1 = 24u;
  i0 += i1;
  p3 = i0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
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
  i1 = f234(i1);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i1;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = p3;
  i1 = p2;
  i2 = 16u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p2;
  i1 = 12u;
  i0 += i1;
  i1 = p1;
  i2 = 12u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = p2;
  i1 = 8u;
  i0 += i1;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = p2;
  i1 = 4u;
  i0 += i1;
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = p2;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = 20u;
  i0 += i1;
  i1 = l0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 28), i1);
  i0 = l3;
  i1 = 32u;
  i0 += i1;
  p2 = i0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = 36u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = 28u;
  i0 += i1;
  i1 = 8u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  f73(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l3;
  i1 = i32_load((&memory), (u64)(i1 + 28));
  p2 = i1;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i0 = p2;
  i1 = p4;
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
  i1 = l1;
  i2 = 40u;
  i1 *= i2;
  i0 += i1;
  p4 = i0;
  i0 = l3;
  i1 = 40u;
  i0 += i1;
  l0 = i0;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p1 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  p2 = i1;
  i0 = i0 == i1;
  if (i0) {goto B11;}
  i0 = 0u;
  i1 = p2;
  i0 -= i1;
  p3 = i0;
  i0 = p1;
  i1 = 4294967276u;
  i0 += i1;
  p2 = i0;
  L12: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 4294967264u;
    i0 += i1;
    i1 = p2;
    i2 = 4294967284u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = l3;
    i1 = 4294967256u;
    i0 += i1;
    i1 = p2;
    i2 = 4294967276u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    p1 = i0;
    j1 = 0ull;
    i64_store((&memory), (u64)(i0), j1);
    i0 = l3;
    i1 = 4294967280u;
    i0 += i1;
    l1 = i0;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = p2;
    i2 = 4294967292u;
    i1 += i2;
    l2 = i1;
    i1 = i32_load((&memory), (u64)(i1));
    i32_store((&memory), (u64)(i0), i1);
    i0 = l3;
    i1 = 4294967276u;
    i0 += i1;
    i1 = p2;
    i1 = i32_load((&memory), (u64)(i1));
    i32_store((&memory), (u64)(i0), i1);
    i0 = l1;
    i1 = p2;
    i2 = 4u;
    i1 += i2;
    p1 = i1;
    i1 = i32_load((&memory), (u64)(i1));
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l3;
    i1 = 4294967284u;
    i0 += i1;
    p1 = i0;
    j1 = 0ull;
    i64_store((&memory), (u64)(i0), j1);
    i0 = l2;
    j1 = 0ull;
    i64_store((&memory), (u64)(i0), j1);
    i0 = l3;
    i1 = 4294967292u;
    i0 += i1;
    l1 = i0;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = p2;
    i2 = 8u;
    i1 += i2;
    l2 = i1;
    i1 = i32_load((&memory), (u64)(i1));
    i32_store((&memory), (u64)(i0), i1);
    i0 = l3;
    i1 = 4294967288u;
    i0 += i1;
    i1 = p2;
    i2 = 12u;
    i1 += i2;
    i1 = i32_load((&memory), (u64)(i1));
    i32_store((&memory), (u64)(i0), i1);
    i0 = l1;
    i1 = p2;
    i2 = 16u;
    i1 += i2;
    p1 = i1;
    i1 = i32_load((&memory), (u64)(i1));
    i32_store((&memory), (u64)(i0), i1);
    i0 = p1;
    i1 = 0u;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l2;
    j1 = 0ull;
    i64_store((&memory), (u64)(i0), j1);
    i0 = l3;
    i1 = 4294967256u;
    i0 += i1;
    l3 = i0;
    i0 = p2;
    i1 = 4294967256u;
    i0 += i1;
    p2 = i0;
    i1 = p3;
    i0 += i1;
    i1 = 4294967276u;
    i0 = i0 != i1;
    if (i0) {goto L12;}
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p2 = i0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l1 = i0;
  goto B10;
  B11:;
  i0 = p2;
  l1 = i0;
  B10:;
  i0 = p0;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i1 = l0;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = p4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p2;
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B13;}
  i0 = 0u;
  i1 = l1;
  i0 -= i1;
  p1 = i0;
  i0 = p2;
  i1 = 4294967272u;
  i0 += i1;
  l3 = i0;
  L14: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l3;
    i1 = 12u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    p2 = i0;
    i0 = !(i0);
    if (i0) {goto B15;}
    i0 = l3;
    i1 = 16u;
    i0 += i1;
    i1 = p2;
    i32_store((&memory), (u64)(i0), i1);
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
    f235(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B15:;
    i0 = l3;
    i0 = i32_load((&memory), (u64)(i0));
    p2 = i0;
    i0 = !(i0);
    if (i0) {goto B17;}
    i0 = l3;
    i1 = 4u;
    i0 += i1;
    i1 = p2;
    i32_store((&memory), (u64)(i0), i1);
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
    f235(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B17:;
    i0 = l3;
    i1 = 4294967256u;
    i0 += i1;
    l3 = i0;
    i1 = p1;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L14;}
  B13:;
  i0 = l1;
  i0 = !(i0);
  if (i0) {goto B19;}
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B19:;
  FUNC_EPILOGUE;
}

static void f133(u32 p0, u32 p1) {
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
  i0 = f159(i0, i1);
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
  f73(i0, i1);
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
  i0 = f160(i0, i1);
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
  i0 = f161(i0, i1);
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
  i0 = f161(i0, i1);
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
  i0 = f162(i0, i1);
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

static u32 f134(u32 p0) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 48));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B0;}
  i0 = p0;
  i1 = 52u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = 0u;
  i1 = l0;
  i0 -= i1;
  l1 = i0;
  i0 = l4;
  i1 = 4294967284u;
  i0 += i1;
  l4 = i0;
  L3: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0));
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto B4;}
    i0 = l4;
    i1 = 4u;
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
    f235(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B4:;
    i0 = l4;
    i1 = 4294967280u;
    i0 += i1;
    l4 = i0;
    i1 = l1;
    i0 += i1;
    i1 = 4294967284u;
    i0 = i0 != i1;
    if (i0) {goto L3;}
  i0 = p0;
  i1 = 48u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B1;
  B2:;
  i0 = l0;
  l4 = i0;
  B1:;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B0:;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 36));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B7;}
  i0 = p0;
  i1 = 40u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B9;}
  i0 = 0u;
  i1 = l0;
  i0 -= i1;
  l1 = i0;
  i0 = l4;
  i1 = 4294967272u;
  i0 += i1;
  l4 = i0;
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 12u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto B11;}
    i0 = l4;
    i1 = 16u;
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
    f235(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B11:;
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0));
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto B13;}
    i0 = l4;
    i1 = 4u;
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
    f235(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B13:;
    i0 = l4;
    i1 = 4294967256u;
    i0 += i1;
    l4 = i0;
    i1 = l1;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L10;}
  i0 = p0;
  i1 = 36u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B8;
  B9:;
  i0 = l0;
  l4 = i0;
  B8:;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B7:;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 24));
  l0 = i0;
  i0 = !(i0);
  if (i0) {goto B16;}
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B18;}
  i0 = 0u;
  i1 = l0;
  i0 -= i1;
  l1 = i0;
  i0 = l4;
  i1 = 4294967272u;
  i0 += i1;
  l4 = i0;
  L19: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 12u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto B20;}
    i0 = l4;
    i1 = 16u;
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
    f235(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B20:;
    i0 = l4;
    i0 = i32_load((&memory), (u64)(i0));
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto B22;}
    i0 = l4;
    i1 = 4u;
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
    f235(i0);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    B22:;
    i0 = l4;
    i1 = 4294967256u;
    i0 += i1;
    l4 = i0;
    i1 = l1;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L19;}
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l4 = i0;
  goto B17;
  B18:;
  i0 = l0;
  l4 = i0;
  B17:;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B16:;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f135(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0;
  u64 l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 48u;
  i1 -= i2;
  l7 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l7;
  l6 = i0;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 44), i1);
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
  i32_store((&memory), (u64)(i0 + 40), i1);
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
  i0 = l6;
  i1 = l6;
  i2 = 32u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l6;
  i1 = l6;
  i2 = 44u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l6;
  i1 = p0;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = 104u;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i1 = p0;
  i2 = l6;
  i3 = 8u;
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
  i0 = f155(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  i0 = l6;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l6;
  i1 = l2;
  j1 = i64_load((&memory), (u64)(i1));
  l3 = j1;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l6;
  i1 = l2;
  i1 = i32_load((&memory), (u64)(i1 + 92));
  p1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = 28u;
  i0 += i1;
  l1 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  i1 = p0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i0 = i0 >= i1;
  if (i0) {goto B15;}
  i0 = l5;
  j1 = l3;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = l5;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l6;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l5;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l1;
  i1 = l5;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B14;
  B15:;
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  i1 = l6;
  i2 = 24u;
  i1 += i2;
  i2 = l6;
  i3 = 8u;
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
  f137(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
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
  i0 = i32_load((&memory), (u64)(i0 + 16));
  l5 = i0;
  i0 = !(i0);
  if (i0) {goto B17;}
  i0 = l2;
  i1 = 20u;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B17:;
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
  f235(i0);
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

static void f136(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l6 = 0, l7 = 0;
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
  l7 = i0;
  l6 = i0;
  i0 = 0u;
  i1 = l7;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l4 = i1;
  i1 = i32_load((&memory), (u64)(i1));
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p1;
  i1 = l4;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i0 = p1;
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  l1 = i1;
  i0 -= i1;
  l3 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l5 = j0;
  i0 = 16u;
  l4 = i0;
  L0: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    j0 = l5;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l5 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L0;}
  i0 = l1;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B1;}
  i0 = l3;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = l4;
  i0 += i1;
  l4 = i0;
  B1:;
  i0 = l4;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B3;}
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
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  goto B2;
  B3:;
  i0 = 0u;
  i1 = l7;
  i2 = l2;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l4 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B2:;
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l6;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l6;
  i1 = l4;
  i2 = l2;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8), i1);
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
  i0 = f147(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 15938991009778630656ull;
  i3 = p0;
  i3 = i32_load((&memory), (u64)(i3 + 8));
  j3 = i64_load((&memory), (u64)(i3));
  i4 = p1;
  j4 = i64_load((&memory), (u64)(i4));
  l5 = j4;
  i5 = l4;
  i6 = l2;
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
  i32_store((&memory), (u64)(i0 + 92), i1);
  i0 = l2;
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
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B7:;
  j0 = l5;
  i1 = l0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B9;}
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
  B9:;
  i0 = 0u;
  i1 = l6;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f137(u32 p0, u32 p1, u32 p2, u32 p3) {
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
  i0 = f234(i0);
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
  f241(i0);
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
    i0 = i32_load((&memory), (u64)(i0 + 16));
    l2 = i0;
    i0 = !(i0);
    if (i0) {goto B12;}
    i0 = p1;
    i1 = 20u;
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
    f235(i0);
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
    f235(i0);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B15:;
  FUNC_EPILOGUE;
}

static void f138(u32 p0, u32 p1, u32 p2) {
  u32 l1 = 0, l2 = 0, l3 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 16u;
  i0 -= i1;
  l6 = i0;
  l5 = i0;
  i0 = 0u;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = p0;
  i0 = i0 == i1;
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
  i0 = p1;
  i1 = p1;
  j1 = i64_load((&memory), (u64)(i1 + 32));
  i2 = p2;
  i2 = i32_load((&memory), (u64)(i2));
  j2 = i64_load((&memory), (u64)(i2));
  j1 += j2;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = 1u;
  i1 = 256u;
  i2 = g0;
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
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  l1 = i1;
  i0 -= i1;
  l3 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l4 = j0;
  i0 = 16u;
  p2 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 1u;
    i0 += i1;
    p2 = i0;
    j0 = l4;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L4;}
  i0 = l1;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  i0 = l3;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = p2;
  i0 += i1;
  p2 = i0;
  B5:;
  i0 = p2;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
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
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  goto B6;
  B7:;
  i0 = 0u;
  i1 = l6;
  i2 = l2;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  p2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B6:;
  i0 = l5;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l5;
  i1 = p2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = p2;
  i2 = l2;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
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
  i0 = f147(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 92));
  j1 = 0ull;
  i2 = p2;
  i3 = l2;
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
  i0 = l2;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B11;}
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
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B13;}
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
  B13:;
  i0 = 0u;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void _ZN11eosiosystem15system_contract12update_votesEyyRKNSt3__16vectorIyNS1_9allocatorIyEEEEb(u32 p0, u64 p1, u64 p2, u32 p3, u32 p4) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0, 
      l8 = 0, l9 = 0, l10 = 0, l11 = 0, l12 = 0, l13 = 0, l15 = 0, l16 = 0;
  f64 l14 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  f64 d0, d1, d2, d3, d4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 80u;
  i1 -= i2;
  l16 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l16;
  j1 = p2;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = p3;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p3;
  i1 = i32_load((&memory), (u64)(i1));
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  l15 = i0;
  j0 = p2;
  i0 = !(j0);
  l5 = i0;
  if (i0) {goto B1;}
  i0 = l15;
  i0 = !(i0);
  i1 = 2208u;
  i2 = g0;
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
  j0 = p2;
  j1 = p1;
  i0 = j0 != j1;
  i1 = 2272u;
  i2 = g0;
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
  goto B0;
  B1:;
  i0 = l15;
  i1 = 31u;
  i0 = i0 < i1;
  i1 = 2304u;
  i2 = g0;
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
  i1 = 4u;
  i0 += i1;
  l4 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p3;
  i1 = i32_load((&memory), (u64)(i1));
  l12 = i1;
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  i1 = 2u;
  i0 = i0 < i1;
  if (i0) {goto B0;}
  i0 = 0u;
  l15 = i0;
  i0 = 1u;
  l3 = i0;
  L6: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l12;
    i1 = l15;
    i0 += i1;
    l12 = i0;
    j0 = i64_load((&memory), (u64)(i0));
    i1 = l12;
    i2 = 8u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i0 = j0 < j1;
    i1 = 2352u;
    i2 = g0;
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
    i0 = l15;
    i1 = 8u;
    i0 += i1;
    l15 = i0;
    i0 = l3;
    i1 = 1u;
    i0 += i1;
    l3 = i0;
    i1 = l4;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = p3;
    i2 = i32_load((&memory), (u64)(i2));
    l12 = i2;
    i1 -= i2;
    i2 = 3u;
    i1 = (u32)((s32)i1 >> (i2 & 31));
    i0 = i0 < i1;
    if (i0) {goto L6;}
  B0:;
  i0 = p0;
  i1 = 36u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i1 = p0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l13 = i1;
  i0 = i0 == i1;
  if (i0) {goto B8;}
  i0 = l3;
  i1 = 4294967272u;
  i0 += i1;
  l15 = i0;
  i0 = 0u;
  i1 = l13;
  i0 -= i1;
  l4 = i0;
  L9: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l15;
    i0 = i32_load((&memory), (u64)(i0));
    j0 = i64_load((&memory), (u64)(i0));
    j1 = p1;
    i0 = j0 == j1;
    if (i0) {goto B8;}
    i0 = l15;
    l3 = i0;
    i0 = l15;
    i1 = 4294967272u;
    i0 += i1;
    l12 = i0;
    l15 = i0;
    i0 = l12;
    i1 = l4;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L9;}
  B8:;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l0 = i0;
  i0 = l3;
  i1 = l13;
  i0 = i0 == i1;
  if (i0) {goto B11;}
  i0 = l3;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = l0;
  i0 = i0 == i1;
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
  i0 = l16;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 28), i1);
  i0 = l16;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l16;
  i1 = 24u;
  i0 += i1;
  i1 = 4u;
  i0 |= i1;
  l11 = i0;
  goto B10;
  B11:;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p0;
  i2 = 16u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 15938991009778630656ull;
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
  l15 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 <= (s32)i1);
  if (i0) {goto B13;}
  i0 = l0;
  i1 = l15;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f135(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = l0;
  i0 = i0 == i1;
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
  i0 = l16;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 28), i1);
  i0 = l16;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l16;
  i1 = 24u;
  i0 += i1;
  i1 = 4u;
  i0 |= i1;
  l11 = i0;
  goto B10;
  B13:;
  i0 = 0u;
  l2 = i0;
  i0 = l16;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 28), i1);
  i0 = l16;
  i1 = l0;
  i32_store((&memory), (u64)(i0 + 24), i1);
  i0 = l16;
  i1 = 24u;
  i0 += i1;
  i1 = 4u;
  i0 |= i1;
  l11 = i0;
  B10:;
  i0 = l2;
  i1 = 0u;
  i0 = i0 != i1;
  i1 = 2400u;
  i2 = g0;
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
  l15 = i0;
  i0 = l5;
  if (i0) {goto B18;}
  i0 = l2;
  i0 = i32_load8_u((&memory), (u64)(i0 + 56));
  i1 = 1u;
  i0 ^= i1;
  l15 = i0;
  B18:;
  i0 = l15;
  i1 = 1u;
  i0 &= i1;
  i1 = 2448u;
  i2 = g0;
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
  d0 = f64_load((&memory), (u64)(i0 + 40));
  l14 = d0;
  d1 = 0;
  i0 = (*Z_eosio_injectionZ__eosio_f64_gtZ_idd)(d0, d1);
  d1 = l14;
  d2 = l14;
  i1 = (*Z_eosio_injectionZ__eosio_f64_neZ_idd)(d1, d2);
  i0 |= i1;
  if (i0) {goto B20;}
  i0 = p0;
  i1 = 264u;
  i0 += i1;
  l15 = i0;
  i1 = l15;
  j1 = i64_load((&memory), (u64)(i1));
  i2 = l2;
  i3 = 32u;
  i2 += i3;
  j2 = i64_load((&memory), (u64)(i2));
  j1 += j2;
  p1 = j1;
  i64_store((&memory), (u64)(i0), j1);
  j0 = p1;
  j1 = 1500000000000ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  if (i0) {goto B20;}
  i0 = p0;
  i1 = 272u;
  i0 += i1;
  l15 = i0;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 0ull;
  i0 = j0 != j1;
  if (i0) {goto B20;}
  i0 = l15;
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
  j1 = (*Z_envZ_current_timeZ_jv)();
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i64_store((&memory), (u64)(i0), j1);
  B20:;
  i0 = l16;
  i1 = l2;
  i2 = 32u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  d1 = (f64)(s64)(j1);
  d2 = 2;
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
  j3 = (*Z_envZ_current_timeZ_jv)();
  i4 = g0;
  i5 = 1u;
  i4 += i5;
  g0 = i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j4 = 1000000ull;
  j3 = DIV_U(j3, j4);
  j4 = 4294967295ull;
  j3 &= j4;
  j4 = 18446744072762866816ull;
  j3 += j4;
  j4 = 604800ull;
  j3 = I64_DIV_S(j3, j4);
  d3 = (f64)(s64)(j3);
  d4 = 52;
  d3 = (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(d3, d4);
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
  d2 = pow_0(d2, d3);
  i3 = g0;
  i4 = 1u;
  i3 += i4;
  g0 = i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  d1 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d1, d2);
  l14 = d1;
  f64_store((&memory), (u64)(i0 + 16), d1);
  i0 = l2;
  i0 = i32_load8_u((&memory), (u64)(i0 + 56));
  i0 = !(i0);
  if (i0) {goto B24;}
  i0 = l16;
  d1 = l14;
  i2 = l2;
  d2 = f64_load((&memory), (u64)(i2 + 48));
  d1 = (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(d1, d2);
  f64_store((&memory), (u64)(i0 + 16), d1);
  B24:;
  i0 = l16;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l16;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = l2;
  i1 = 40u;
  i0 += i1;
  d0 = f64_load((&memory), (u64)(i0));
  l14 = d0;
  d1 = 0;
  i0 = (*Z_eosio_injectionZ__eosio_f64_leZ_idd)(d0, d1);
  d1 = l14;
  d2 = l14;
  i1 = (*Z_eosio_injectionZ__eosio_f64_neZ_idd)(d1, d2);
  i0 |= i1;
  if (i0) {goto B25;}
  i0 = l2;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  p1 = j0;
  i0 = !(j0);
  if (i0) {goto B28;}
  i0 = p0;
  i1 = 36u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i1 = p0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l13 = i1;
  i0 = i0 == i1;
  if (i0) {goto B29;}
  i0 = l3;
  i1 = 4294967272u;
  i0 += i1;
  l15 = i0;
  i0 = 0u;
  i1 = l13;
  i0 -= i1;
  l4 = i0;
  L30: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l15;
    i0 = i32_load((&memory), (u64)(i0));
    j0 = i64_load((&memory), (u64)(i0));
    j1 = p1;
    i0 = j0 == j1;
    if (i0) {goto B29;}
    i0 = l15;
    l3 = i0;
    i0 = l15;
    i1 = 4294967272u;
    i0 += i1;
    l12 = i0;
    l15 = i0;
    i0 = l12;
    i1 = l4;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L30;}
  B29:;
  i0 = l3;
  i1 = l13;
  i0 = i0 == i1;
  if (i0) {goto B27;}
  i0 = l3;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l15 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = l0;
  i0 = i0 == i1;
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
  goto B26;
  B28:;
  i0 = l2;
  i0 = i32_load((&memory), (u64)(i0 + 16));
  l1 = i0;
  i1 = l2;
  i2 = 20u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l10 = i1;
  i0 = i0 == i1;
  if (i0) {goto B25;}
  i0 = l16;
  i1 = 65u;
  i0 += i1;
  l8 = i0;
  i0 = l16;
  i1 = 48u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l6 = i0;
  i0 = l16;
  i1 = 48u;
  i0 += i1;
  i1 = 16u;
  i0 += i1;
  l7 = i0;
  i0 = 0u;
  l15 = i0;
  i0 = 0u;
  l13 = i0;
  L32: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l13;
    i1 = l15;
    i2 = 24u;
    i1 *= i2;
    l15 = i1;
    i0 += i1;
    l9 = i0;
    i0 = l15;
    i0 = !(i0);
    if (i0) {goto B33;}
    i0 = l15;
    i1 = 24u;
    i0 = I32_DIV_S(i0, i1);
    l15 = i0;
    i0 = l1;
    j0 = i64_load((&memory), (u64)(i0));
    p1 = j0;
    i0 = l13;
    l12 = i0;
    L34: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l12;
      i1 = l15;
      i2 = 1u;
      i1 >>= (i2 & 31);
      l3 = i1;
      i2 = 24u;
      i1 *= i2;
      i0 += i1;
      l4 = i0;
      i1 = 24u;
      i0 += i1;
      l5 = i0;
      i1 = l12;
      i2 = l4;
      j2 = i64_load((&memory), (u64)(i2));
      j3 = p1;
      i2 = j2 < j3;
      l4 = i2;
      i0 = i2 ? i0 : i1;
      l12 = i0;
      i0 = l5;
      i1 = l13;
      i2 = l4;
      i0 = i2 ? i0 : i1;
      l13 = i0;
      i0 = l15;
      i1 = 4294967295u;
      i0 += i1;
      i1 = l3;
      i0 -= i1;
      i1 = l3;
      i2 = l4;
      i0 = i2 ? i0 : i1;
      l15 = i0;
      if (i0) {goto L34;}
    B33:;
    i0 = l13;
    i1 = l9;
    i0 = i0 == i1;
    if (i0) {goto B37;}
    i0 = l1;
    j0 = i64_load((&memory), (u64)(i0));
    p1 = j0;
    i1 = l13;
    j1 = i64_load((&memory), (u64)(i1));
    i0 = j0 < j1;
    if (i0) {goto B36;}
    goto B35;
    B37:;
    i0 = l1;
    j0 = i64_load((&memory), (u64)(i0));
    p1 = j0;
    B36:;
    i0 = l6;
    j1 = 0ull;
    i64_store((&memory), (u64)(i0), j1);
    i0 = l7;
    i1 = 0u;
    i32_store8((&memory), (u64)(i0), i1);
    i0 = l16;
    j1 = p1;
    i64_store((&memory), (u64)(i0 + 48), j1);
    i0 = l8;
    i1 = 6u;
    i0 += i1;
    i1 = l16;
    i2 = 41u;
    i1 += i2;
    i2 = 6u;
    i1 += i2;
    i1 = i32_load8_u((&memory), (u64)(i1));
    i32_store8((&memory), (u64)(i0), i1);
    i0 = l8;
    i1 = 4u;
    i0 += i1;
    i1 = l16;
    i2 = 41u;
    i1 += i2;
    i2 = 4u;
    i1 += i2;
    i1 = i32_load16_u((&memory), (u64)(i1));
    i32_store16((&memory), (u64)(i0), i1);
    i0 = l8;
    i1 = l16;
    i1 = i32_load((&memory), (u64)(i1 + 41));
    i32_store((&memory), (u64)(i0), i1);
    i0 = l16;
    i1 = 72u;
    i0 += i1;
    i1 = l16;
    i2 = l13;
    i3 = l16;
    i4 = 48u;
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
    f142(i0, i1, i2, i3);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l16;
    i0 = i32_load((&memory), (u64)(i0 + 72));
    l13 = i0;
    i0 = l11;
    i0 = i32_load((&memory), (u64)(i0));
    l2 = i0;
    B35:;
    i0 = l13;
    i1 = 16u;
    i0 += i1;
    i1 = 0u;
    i32_store8((&memory), (u64)(i0), i1);
    i0 = l13;
    i1 = l13;
    d1 = f64_load((&memory), (u64)(i1 + 8));
    i2 = l2;
    d2 = f64_load((&memory), (u64)(i2 + 40));
    d1 = (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(d1, d2);
    f64_store((&memory), (u64)(i0 + 8), d1);
    i0 = l1;
    i1 = 8u;
    i0 += i1;
    l1 = i0;
    i1 = l10;
    i0 = i0 == i1;
    if (i0) {goto B25;}
    i0 = l16;
    i0 = i32_load((&memory), (u64)(i0 + 4));
    l15 = i0;
    i0 = l16;
    i0 = i32_load((&memory), (u64)(i0));
    l13 = i0;
    goto L32;
  B27:;
  i0 = 0u;
  l15 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p0;
  i2 = 16u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 15938991009778630656ull;
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
  l12 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B26;}
  i0 = l0;
  i1 = l12;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f135(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l15 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = l0;
  i0 = i0 == i1;
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
  B26:;
  i0 = l15;
  i1 = 0u;
  i0 = i0 != i1;
  l12 = i0;
  i1 = 2512u;
  i2 = g0;
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
  i0 = l16;
  i1 = l16;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l12;
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
  i0 = l0;
  i1 = l15;
  i2 = l16;
  i3 = 48u;
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
  f140(i0, i1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = l15;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  _ZN11eosiosystem15system_contract23propagate_weight_changeERKNS_10voter_infoE(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B25:;
  i0 = l16;
  j0 = i64_load((&memory), (u64)(i0 + 32));
  p1 = j0;
  i0 = !(j0);
  if (i0) {goto B49;}
  i0 = p0;
  i1 = 36u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i1 = p0;
  i2 = 32u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l13 = i1;
  i0 = i0 == i1;
  if (i0) {goto B50;}
  i0 = l3;
  i1 = 4294967272u;
  i0 += i1;
  l15 = i0;
  i0 = 0u;
  i1 = l13;
  i0 -= i1;
  l4 = i0;
  L51: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l15;
    i0 = i32_load((&memory), (u64)(i0));
    j0 = i64_load((&memory), (u64)(i0));
    j1 = p1;
    i0 = j0 == j1;
    if (i0) {goto B50;}
    i0 = l15;
    l3 = i0;
    i0 = l15;
    i1 = 4294967272u;
    i0 += i1;
    l12 = i0;
    l15 = i0;
    i0 = l12;
    i1 = l4;
    i0 += i1;
    i1 = 4294967272u;
    i0 = i0 != i1;
    if (i0) {goto L51;}
  B50:;
  i0 = l3;
  i1 = l13;
  i0 = i0 == i1;
  if (i0) {goto B48;}
  i0 = l3;
  i1 = 4294967272u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l15 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = l0;
  i0 = i0 == i1;
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
  goto B47;
  B49:;
  i0 = l16;
  d0 = f64_load((&memory), (u64)(i0 + 16));
  l14 = d0;
  d1 = 0;
  i0 = (*Z_eosio_injectionZ__eosio_f64_ltZ_idd)(d0, d1);
  d1 = l14;
  d2 = l14;
  i1 = (*Z_eosio_injectionZ__eosio_f64_neZ_idd)(d1, d2);
  i0 |= i1;
  if (i0) {goto B46;}
  i0 = p3;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = p3;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l9 = i1;
  i0 = i0 == i1;
  if (i0) {goto B46;}
  i0 = l16;
  i1 = 65u;
  i0 += i1;
  l1 = i0;
  i0 = l16;
  i1 = 48u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l10 = i0;
  i0 = l16;
  i1 = 48u;
  i0 += i1;
  i1 = 16u;
  i0 += i1;
  l11 = i0;
  L53: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l16;
    i0 = i32_load((&memory), (u64)(i0));
    l13 = i0;
    i1 = l16;
    i1 = i32_load((&memory), (u64)(i1 + 4));
    i2 = 24u;
    i1 *= i2;
    l15 = i1;
    i0 += i1;
    l8 = i0;
    i0 = l15;
    i0 = !(i0);
    if (i0) {goto B54;}
    i0 = l15;
    i1 = 24u;
    i0 = I32_DIV_S(i0, i1);
    l15 = i0;
    i0 = l2;
    j0 = i64_load((&memory), (u64)(i0));
    p1 = j0;
    i0 = l13;
    l12 = i0;
    L55: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l12;
      i1 = l15;
      i2 = 1u;
      i1 >>= (i2 & 31);
      l3 = i1;
      i2 = 24u;
      i1 *= i2;
      i0 += i1;
      l4 = i0;
      i1 = 24u;
      i0 += i1;
      l5 = i0;
      i1 = l12;
      i2 = l4;
      j2 = i64_load((&memory), (u64)(i2));
      j3 = p1;
      i2 = j2 < j3;
      l4 = i2;
      i0 = i2 ? i0 : i1;
      l12 = i0;
      i0 = l5;
      i1 = l13;
      i2 = l4;
      i0 = i2 ? i0 : i1;
      l13 = i0;
      i0 = l15;
      i1 = 4294967295u;
      i0 += i1;
      i1 = l3;
      i0 -= i1;
      i1 = l3;
      i2 = l4;
      i0 = i2 ? i0 : i1;
      l15 = i0;
      if (i0) {goto L55;}
    B54:;
    i0 = l13;
    i1 = l8;
    i0 = i0 == i1;
    if (i0) {goto B58;}
    i0 = l2;
    j0 = i64_load((&memory), (u64)(i0));
    p1 = j0;
    i1 = l13;
    j1 = i64_load((&memory), (u64)(i1));
    i0 = j0 < j1;
    if (i0) {goto B57;}
    goto B56;
    B58:;
    i0 = l2;
    j0 = i64_load((&memory), (u64)(i0));
    p1 = j0;
    B57:;
    i0 = l10;
    j1 = 0ull;
    i64_store((&memory), (u64)(i0), j1);
    i0 = l11;
    i1 = 0u;
    i32_store8((&memory), (u64)(i0), i1);
    i0 = l16;
    j1 = p1;
    i64_store((&memory), (u64)(i0 + 48), j1);
    i0 = l1;
    i1 = 6u;
    i0 += i1;
    i1 = l16;
    i2 = 41u;
    i1 += i2;
    i2 = 6u;
    i1 += i2;
    i1 = i32_load8_u((&memory), (u64)(i1));
    i32_store8((&memory), (u64)(i0), i1);
    i0 = l1;
    i1 = 4u;
    i0 += i1;
    i1 = l16;
    i2 = 41u;
    i1 += i2;
    i2 = 4u;
    i1 += i2;
    i1 = i32_load16_u((&memory), (u64)(i1));
    i32_store16((&memory), (u64)(i0), i1);
    i0 = l1;
    i1 = l16;
    i1 = i32_load((&memory), (u64)(i1 + 41));
    i32_store((&memory), (u64)(i0), i1);
    i0 = l16;
    i1 = 72u;
    i0 += i1;
    i1 = l16;
    i2 = l13;
    i3 = l16;
    i4 = 48u;
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
    f142(i0, i1, i2, i3);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l16;
    i0 = i32_load((&memory), (u64)(i0 + 72));
    l13 = i0;
    i0 = l16;
    d0 = f64_load((&memory), (u64)(i0 + 16));
    l14 = d0;
    B56:;
    i0 = l13;
    i1 = 16u;
    i0 += i1;
    i1 = 1u;
    i32_store8((&memory), (u64)(i0), i1);
    i0 = l13;
    d1 = l14;
    i2 = l13;
    d2 = f64_load((&memory), (u64)(i2 + 8));
    d1 = (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(d1, d2);
    f64_store((&memory), (u64)(i0 + 8), d1);
    i0 = l2;
    i1 = 8u;
    i0 += i1;
    l2 = i0;
    i1 = l9;
    i0 = i0 != i1;
    if (i0) {goto L53;}
    goto B46;
  B48:;
  i0 = 0u;
  l15 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p0;
  i2 = 16u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  j2 = 15938991009778630656ull;
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
  l12 = i0;
  i1 = 0u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B47;}
  i0 = l0;
  i1 = l12;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i0 = f135(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l15 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = l0;
  i0 = i0 == i1;
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
  B47:;
  i0 = l15;
  i1 = 0u;
  i0 = i0 != i1;
  l3 = i0;
  i1 = 2544u;
  i2 = g0;
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
  l12 = i0;
  i0 = p4;
  i0 = !(i0);
  if (i0) {goto B64;}
  i0 = l15;
  i0 = i32_load8_u((&memory), (u64)(i0 + 56));
  l12 = i0;
  B64:;
  i0 = l12;
  i1 = 2576u;
  i2 = g0;
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
  i0 = l16;
  d0 = f64_load((&memory), (u64)(i0 + 16));
  l14 = d0;
  d1 = 0;
  i0 = (*Z_eosio_injectionZ__eosio_f64_ltZ_idd)(d0, d1);
  d1 = l14;
  d2 = l14;
  i1 = (*Z_eosio_injectionZ__eosio_f64_neZ_idd)(d1, d2);
  i0 |= i1;
  if (i0) {goto B46;}
  i0 = l16;
  i1 = l16;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l3;
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
  i0 = l0;
  i1 = l15;
  i2 = l16;
  i3 = 48u;
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
  f143(i0, i1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = l15;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  _ZN11eosiosystem15system_contract23propagate_weight_changeERKNS_10voter_infoE(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B46:;
  i0 = l16;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 24u;
  i0 *= i1;
  l15 = i0;
  i0 = !(i0);
  if (i0) {goto B69;}
  i0 = l16;
  i0 = i32_load((&memory), (u64)(i0));
  l13 = i0;
  i1 = l15;
  i0 += i1;
  l1 = i0;
  i0 = p0;
  i1 = 48u;
  i0 += i1;
  l5 = i0;
  i0 = p4;
  i0 = !(i0);
  if (i0) {goto B70;}
  i0 = p0;
  i1 = 72u;
  i0 += i1;
  l8 = i0;
  i0 = p0;
  i1 = 76u;
  i0 += i1;
  l9 = i0;
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  l10 = i0;
  L71: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l13;
    j0 = i64_load((&memory), (u64)(i0));
    p1 = j0;
    i0 = l9;
    i0 = i32_load((&memory), (u64)(i0));
    l3 = i0;
    i1 = l8;
    i1 = i32_load((&memory), (u64)(i1));
    l2 = i1;
    i0 = i0 == i1;
    if (i0) {goto B72;}
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    l15 = i0;
    i0 = 0u;
    i1 = l2;
    i0 -= i1;
    l4 = i0;
    L73: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l15;
      i0 = i32_load((&memory), (u64)(i0));
      j0 = i64_load((&memory), (u64)(i0));
      j1 = p1;
      i0 = j0 == j1;
      if (i0) {goto B72;}
      i0 = l15;
      l3 = i0;
      i0 = l15;
      i1 = 4294967272u;
      i0 += i1;
      l12 = i0;
      l15 = i0;
      i0 = l12;
      i1 = l4;
      i0 += i1;
      i1 = 4294967272u;
      i0 = i0 != i1;
      if (i0) {goto L73;}
    B72:;
    i0 = l3;
    i1 = l2;
    i0 = i0 == i1;
    if (i0) {goto B77;}
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    l15 = i0;
    i1 = 92u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i0 = i0 == i1;
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
    i0 = l15;
    if (i0) {goto B76;}
    goto B75;
    B77:;
    i0 = l5;
    j0 = i64_load((&memory), (u64)(i0));
    i1 = l10;
    j1 = i64_load((&memory), (u64)(i1));
    j2 = 12531438729690087424ull;
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
    l15 = i0;
    i1 = 0u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto B75;}
    i0 = l5;
    i1 = l15;
    i2 = g0;
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
    l15 = i0;
    i0 = i32_load((&memory), (u64)(i0 + 92));
    i1 = l5;
    i0 = i0 == i1;
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
    B76:;
    i0 = 1u;
    l12 = i0;
    i0 = l15;
    i0 = i32_load8_u((&memory), (u64)(i0 + 56));
    if (i0) {goto B82;}
    i0 = l13;
    i1 = 16u;
    i0 += i1;
    i0 = i32_load8_u((&memory), (u64)(i0));
    i1 = 1u;
    i0 ^= i1;
    l12 = i0;
    B82:;
    i0 = l12;
    i1 = 1u;
    i0 &= i1;
    i1 = 2592u;
    i2 = g0;
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
    i0 = l16;
    i1 = p0;
    i32_store((&memory), (u64)(i0 + 52), i1);
    i0 = l16;
    i1 = l13;
    i32_store((&memory), (u64)(i0 + 48), i1);
    i0 = 1u;
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
    i0 = l5;
    i1 = l15;
    i2 = l16;
    i3 = 48u;
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
    f144(i0, i1, i2);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    goto B74;
    B75:;
    i0 = l13;
    i1 = 16u;
    i0 += i1;
    i0 = i32_load8_u((&memory), (u64)(i0));
    i1 = 1u;
    i0 ^= i1;
    i1 = 2640u;
    i2 = g0;
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
    B74:;
    i0 = l13;
    i1 = 24u;
    i0 += i1;
    l13 = i0;
    i1 = l1;
    i0 = i0 != i1;
    if (i0) {goto L71;}
    goto B69;
  B70:;
  i0 = p0;
  i1 = 72u;
  i0 += i1;
  l8 = i0;
  i0 = p0;
  i1 = 76u;
  i0 += i1;
  l9 = i0;
  i0 = p0;
  i1 = 56u;
  i0 += i1;
  l10 = i0;
  L87: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l13;
    j0 = i64_load((&memory), (u64)(i0));
    p1 = j0;
    i0 = l9;
    i0 = i32_load((&memory), (u64)(i0));
    l3 = i0;
    i1 = l8;
    i1 = i32_load((&memory), (u64)(i1));
    l2 = i1;
    i0 = i0 == i1;
    if (i0) {goto B88;}
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    l15 = i0;
    i0 = 0u;
    i1 = l2;
    i0 -= i1;
    l4 = i0;
    L89: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      i0 = l15;
      i0 = i32_load((&memory), (u64)(i0));
      j0 = i64_load((&memory), (u64)(i0));
      j1 = p1;
      i0 = j0 == j1;
      if (i0) {goto B88;}
      i0 = l15;
      l3 = i0;
      i0 = l15;
      i1 = 4294967272u;
      i0 += i1;
      l12 = i0;
      l15 = i0;
      i0 = l12;
      i1 = l4;
      i0 += i1;
      i1 = 4294967272u;
      i0 = i0 != i1;
      if (i0) {goto L89;}
    B88:;
    i0 = l3;
    i1 = l2;
    i0 = i0 == i1;
    if (i0) {goto B93;}
    i0 = l3;
    i1 = 4294967272u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    l15 = i0;
    i1 = 92u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i0 = i0 == i1;
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
    i0 = l15;
    if (i0) {goto B92;}
    goto B91;
    B93:;
    i0 = l5;
    j0 = i64_load((&memory), (u64)(i0));
    i1 = l10;
    j1 = i64_load((&memory), (u64)(i1));
    j2 = 12531438729690087424ull;
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
    l15 = i0;
    i1 = 0u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto B91;}
    i0 = l5;
    i1 = l15;
    i2 = g0;
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
    l15 = i0;
    i0 = i32_load((&memory), (u64)(i0 + 92));
    i1 = l5;
    i0 = i0 == i1;
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
    B92:;
    i0 = 1u;
    i1 = 2592u;
    i2 = g0;
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
    i0 = l16;
    i1 = p0;
    i32_store((&memory), (u64)(i0 + 52), i1);
    i0 = l16;
    i1 = l13;
    i32_store((&memory), (u64)(i0 + 48), i1);
    i0 = 1u;
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
    i0 = l5;
    i1 = l15;
    i2 = l16;
    i3 = 48u;
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
    f144(i0, i1, i2);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    goto B90;
    B91:;
    i0 = l13;
    i1 = 16u;
    i0 += i1;
    i0 = i32_load8_u((&memory), (u64)(i0));
    i1 = 1u;
    i0 ^= i1;
    i1 = 2640u;
    i2 = g0;
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
    B90:;
    i0 = l13;
    i1 = 24u;
    i0 += i1;
    l13 = i0;
    i1 = l1;
    i0 = i0 != i1;
    if (i0) {goto L87;}
  B69:;
  i0 = l16;
  i0 = i32_load((&memory), (u64)(i0 + 28));
  l15 = i0;
  i0 = l16;
  i1 = p3;
  i32_store((&memory), (u64)(i0 + 52), i1);
  i0 = l16;
  i1 = l16;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 48), i1);
  i0 = l16;
  i1 = l16;
  i2 = 32u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 56), i1);
  i0 = l15;
  i1 = 0u;
  i0 = i0 != i1;
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
  i0 = l0;
  i1 = l15;
  i2 = l16;
  i3 = 48u;
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
  f145(i0, i1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = l16;
  i1 = 8u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i0 = !(i0);
  if (i0) {goto B104;}
  i0 = l16;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B104:;
  i0 = 0u;
  i1 = l16;
  i2 = 80u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f140(u32 p0, u32 p1, u32 p2) {
  u32 l1 = 0, l2 = 0, l3 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  f64 d1, d2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 16u;
  i0 -= i1;
  l6 = i0;
  l5 = i0;
  i0 = 0u;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = p0;
  i0 = i0 == i1;
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
  i0 = p1;
  i1 = p1;
  d1 = f64_load((&memory), (u64)(i1 + 48));
  i2 = p2;
  i2 = i32_load((&memory), (u64)(i2));
  i2 = i32_load((&memory), (u64)(i2 + 4));
  d2 = f64_load((&memory), (u64)(i2 + 40));
  d1 = (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(d1, d2);
  f64_store((&memory), (u64)(i0 + 48), d1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = 1u;
  i1 = 256u;
  i2 = g0;
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
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  l1 = i1;
  i0 -= i1;
  l3 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l4 = j0;
  i0 = 16u;
  p2 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 1u;
    i0 += i1;
    p2 = i0;
    j0 = l4;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L4;}
  i0 = l1;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  i0 = l3;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = p2;
  i0 += i1;
  p2 = i0;
  B5:;
  i0 = p2;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
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
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  goto B6;
  B7:;
  i0 = 0u;
  i1 = l6;
  i2 = l2;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  p2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B6:;
  i0 = l5;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l5;
  i1 = p2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = p2;
  i2 = l2;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
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
  i0 = f147(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 92));
  j1 = 0ull;
  i2 = p2;
  i3 = l2;
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
  i0 = l2;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B11;}
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
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B13;}
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
  B13:;
  i0 = 0u;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void _ZN11eosiosystem15system_contract23propagate_weight_changeERKNS_10voter_infoE(u32 p0, u32 p1) {
  u32 l1 = 0, l2 = 0, l4 = 0, l6 = 0, l7 = 0;
  u64 l0 = 0;
  f64 l3 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j3, j4;
  f64 d0, d1, d2, d3, d4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 32u;
  i1 -= i2;
  l7 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0 + 8));
  i0 = !(j0);
  i1 = p1;
  i1 = i32_load8_u((&memory), (u64)(i1 + 56));
  i1 = !(i1);
  i0 |= i1;
  i1 = 2448u;
  i2 = g0;
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
  j1 = i64_load((&memory), (u64)(i1 + 32));
  d1 = (f64)(s64)(j1);
  d2 = 2;
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
  j3 = (*Z_envZ_current_timeZ_jv)();
  i4 = g0;
  i5 = 1u;
  i4 += i5;
  g0 = i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  j4 = 1000000ull;
  j3 = DIV_U(j3, j4);
  j4 = 4294967295ull;
  j3 &= j4;
  j4 = 18446744072762866816ull;
  j3 += j4;
  j4 = 604800ull;
  j3 = I64_DIV_S(j3, j4);
  d3 = (f64)(s64)(j3);
  d4 = 52;
  d3 = (*Z_eosio_injectionZ__eosio_f64_divZ_ddd)(d3, d4);
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
  d2 = pow_0(d2, d3);
  i3 = g0;
  i4 = 1u;
  i3 += i4;
  g0 = i3;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  d1 = (*Z_eosio_injectionZ__eosio_f64_mulZ_ddd)(d1, d2);
  l5 = d1;
  f64_store((&memory), (u64)(i0 + 24), d1);
  i0 = p1;
  i0 = i32_load8_u((&memory), (u64)(i0 + 56));
  i0 = !(i0);
  if (i0) {goto B3;}
  i0 = l7;
  d1 = l5;
  i2 = p1;
  d2 = f64_load((&memory), (u64)(i2 + 48));
  d1 = (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(d1, d2);
  l5 = d1;
  f64_store((&memory), (u64)(i0 + 24), d1);
  B3:;
  d0 = l5;
  i1 = p1;
  d1 = f64_load((&memory), (u64)(i1 + 40));
  d0 = (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(d0, d1);
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
  d0 = fabs_0(d0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = d0;
  d1 = 1;
  i0 = (*Z_eosio_injectionZ__eosio_f64_leZ_idd)(d0, d1);
  d1 = l3;
  d2 = l3;
  i1 = (*Z_eosio_injectionZ__eosio_f64_neZ_idd)(d1, d2);
  i0 |= i1;
  if (i0) {goto B4;}
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = !(j0);
  if (i0) {goto B6;}
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  l2 = i0;
  j1 = l0;
  i2 = 2576u;
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
  i0 = f151(i0, j1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l6 = i0;
  i0 = l7;
  i1 = p1;
  i32_store((&memory), (u64)(i0 + 20), i1);
  i0 = l7;
  i1 = l7;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l2;
  i1 = l6;
  i2 = l7;
  i3 = 16u;
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
  f152(i0, i1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
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
  _ZN11eosiosystem15system_contract23propagate_weight_changeERKNS_10voter_infoE(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B4;
  B6:;
  i0 = l7;
  d1 = l5;
  i2 = p1;
  i3 = 40u;
  i2 += i3;
  d2 = f64_load((&memory), (u64)(i2));
  d1 = (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(d1, d2);
  f64_store((&memory), (u64)(i0 + 16), d1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 16));
  l6 = i0;
  i1 = p1;
  i2 = 20u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B4;}
  i0 = p0;
  i1 = 48u;
  i0 += i1;
  l2 = i0;
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = l6;
    j1 = i64_load((&memory), (u64)(i1));
    i2 = 2672u;
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
    i0 = f88(i0, j1, i2);
    i1 = g0;
    i2 = 1u;
    i1 += i2;
    g0 = i1;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    l4 = i0;
    i0 = l7;
    i1 = p0;
    i32_store((&memory), (u64)(i0 + 12), i1);
    i0 = l7;
    i1 = l7;
    i2 = 16u;
    i1 += i2;
    i32_store((&memory), (u64)(i0 + 8), i1);
    i0 = l2;
    i1 = l4;
    i2 = l7;
    i3 = 8u;
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
    f153(i0, i1, i2);
    i0 = g0;
    i1 = 1u;
    i0 += i1;
    g0 = i0;
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l1;
    i1 = l6;
    i2 = 8u;
    i1 += i2;
    l6 = i1;
    i0 = i0 != i1;
    if (i0) {goto L10;}
  B4:;
  i0 = l7;
  i1 = l7;
  i2 = 24u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = p1;
  i2 = l7;
  i3 = 16u;
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
  f154(i0, i1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = l7;
  i2 = 32u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f142(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l5 = 0, l6 = 0, l7 = 0, l8 = 0;
  u64 l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l8 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l1 = i1;
  i2 = 24u;
  i1 *= i2;
  i0 += i1;
  l2 = i0;
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  i0 = p3;
  j0 = i64_load((&memory), (u64)(i0));
  l3 = j0;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 >= j1;
  if (i0) {goto B5;}
  B6:;
  i0 = l0;
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = p2;
  i1 = 4294967272u;
  i0 += i1;
  l2 = i0;
  j0 = i64_load((&memory), (u64)(i0));
  l4 = j0;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1));
  l3 = j1;
  i0 = j0 < j1;
  if (i0) {goto B2;}
  j0 = l3;
  j1 = l4;
  i0 = j0 >= j1;
  if (i0) {goto B1;}
  i0 = l0;
  p2 = i0;
  i0 = l2;
  i1 = l0;
  i0 -= i1;
  i1 = 24u;
  i0 = I32_DIV_S(i0, i1);
  l5 = i0;
  if (i0) {goto B4;}
  goto B3;
  B5:;
  i0 = l2;
  i1 = p2;
  i0 -= i1;
  i1 = 24u;
  i0 = I32_DIV_S(i0, i1);
  l5 = i0;
  i0 = !(i0);
  if (i0) {goto B7;}
  L8: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = l5;
    i2 = 1u;
    i1 >>= (i2 & 31);
    l7 = i1;
    i2 = 24u;
    i1 *= i2;
    i0 += i1;
    l6 = i0;
    j0 = i64_load((&memory), (u64)(i0));
    j1 = l3;
    i0 = j0 >= j1;
    if (i0) {goto B9;}
    i0 = l6;
    i1 = 24u;
    i0 += i1;
    p2 = i0;
    i0 = l5;
    i1 = 4294967295u;
    i0 += i1;
    i1 = l7;
    i0 -= i1;
    l7 = i0;
    B9:;
    i0 = l7;
    l5 = i0;
    if (i0) {goto L8;}
  B7:;
  i0 = p2;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = p2;
  l2 = i0;
  j0 = l3;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 < j1;
  if (i0) {goto B2;}
  goto B1;
  B4:;
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = l5;
    i2 = 1u;
    i1 >>= (i2 & 31);
    l7 = i1;
    i2 = 24u;
    i1 *= i2;
    i0 += i1;
    l6 = i0;
    j0 = i64_load((&memory), (u64)(i0));
    j1 = l3;
    i0 = j0 >= j1;
    if (i0) {goto B11;}
    i0 = l6;
    i1 = 24u;
    i0 += i1;
    p2 = i0;
    i0 = l5;
    i1 = 4294967295u;
    i0 += i1;
    i1 = l7;
    i0 -= i1;
    l7 = i0;
    B11:;
    i0 = l7;
    l5 = i0;
    if (i0) {goto L10;}
  B3:;
  i0 = p2;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B2;}
  i0 = p2;
  l2 = i0;
  j0 = l3;
  i1 = p2;
  j1 = i64_load((&memory), (u64)(i1));
  i0 = j0 >= j1;
  if (i0) {goto B1;}
  B2:;
  i0 = l8;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l1;
  i0 = i0 != i1;
  if (i0) {goto B12;}
  i0 = p0;
  i1 = p1;
  i2 = l8;
  i3 = 8u;
  i2 += i3;
  i3 = 1u;
  i4 = p3;
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
  f149(i0, i1, i2, i3, i4);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  goto B0;
  B12:;
  i0 = p1;
  i1 = p2;
  i2 = 1u;
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
  f150(i0, i1, i2, i3);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = p2;
  i3 = l0;
  i2 -= i3;
  i3 = 24u;
  i2 = I32_DIV_S(i2, i3);
  i3 = 24u;
  i2 *= i3;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto B0;
  B1:;
  i0 = p0;
  i1 = l0;
  i2 = l2;
  i3 = l0;
  i2 -= i3;
  i3 = 24u;
  i2 = I32_DIV_S(i2, i3);
  i3 = 24u;
  i2 *= i3;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  B0:;
  i0 = 0u;
  i1 = l8;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f143(u32 p0, u32 p1, u32 p2) {
  u32 l1 = 0, l2 = 0, l3 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  f64 d1, d2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 16u;
  i0 -= i1;
  l6 = i0;
  l5 = i0;
  i0 = 0u;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = p0;
  i0 = i0 == i1;
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
  i0 = p1;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1));
  d1 = f64_load((&memory), (u64)(i1));
  i2 = p1;
  d2 = f64_load((&memory), (u64)(i2 + 48));
  d1 = (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(d1, d2);
  f64_store((&memory), (u64)(i0 + 48), d1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = 1u;
  i1 = 256u;
  i2 = g0;
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
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  l1 = i1;
  i0 -= i1;
  l3 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l4 = j0;
  i0 = 16u;
  p2 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 1u;
    i0 += i1;
    p2 = i0;
    j0 = l4;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L4;}
  i0 = l1;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  i0 = l3;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = p2;
  i0 += i1;
  p2 = i0;
  B5:;
  i0 = p2;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
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
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  goto B6;
  B7:;
  i0 = 0u;
  i1 = l6;
  i2 = l2;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  p2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B6:;
  i0 = l5;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l5;
  i1 = p2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = p2;
  i2 = l2;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
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
  i0 = f147(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 92));
  j1 = 0ull;
  i2 = p2;
  i3 = l2;
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
  i0 = l2;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B11;}
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
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B13;}
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
  B13:;
  i0 = 0u;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f144(u32 p0, u32 p1, u32 p2) {
  u32 l2 = 0, l4 = 0, l5 = 0;
  u64 l0 = 0, l3 = 0;
  f64 l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1, j2, j3, j4, j5;
  f64 d1, d2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 48u;
  i1 -= i2;
  l4 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i1 = 92u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p0;
  i0 = i0 == i1;
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
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  l5 = i0;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1));
  d1 = f64_load((&memory), (u64)(i1 + 8));
  i2 = l5;
  d2 = f64_load((&memory), (u64)(i2));
  l1 = d2;
  d1 = (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(d1, d2);
  d2 = 0;
  d1 = (*Z_eosio_injectionZ__eosio_f64_maxZ_ddd)(d1, d2);
  f64_store((&memory), (u64)(i0), d1);
  i0 = l4;
  l5 = i0;
  d1 = l1;
  d1 = (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(d1);
  d2 = l1;
  i3 = p1;
  i4 = 56u;
  i3 += i4;
  i3 = i32_load8_u((&memory), (u64)(i3));
  d1 = i3 ? d1 : d2;
  f64_store((&memory), (u64)(i0 + 24), d1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 288u;
  i0 += i1;
  l2 = i0;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1));
  d1 = f64_load((&memory), (u64)(i1 + 8));
  i2 = l2;
  d2 = f64_load((&memory), (u64)(i2));
  d1 = (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(d1, d2);
  f64_store((&memory), (u64)(i0), d1);
  i0 = 1u;
  i1 = 256u;
  i2 = g0;
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
  j0 = i64_load32_u((&memory), (u64)(i0 + 16));
  l3 = j0;
  i0 = 64u;
  p2 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 1u;
    i0 += i1;
    p2 = i0;
    j0 = l3;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l3 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L4;}
  i0 = p1;
  i1 = 64u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p1;
  i1 = i32_load8_u((&memory), (u64)(i1 + 60));
  l2 = i1;
  i2 = 1u;
  i1 >>= (i2 & 31);
  i2 = l2;
  i3 = 1u;
  i2 &= i3;
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i1 = p2;
  i0 += i1;
  p2 = i0;
  i0 = l2;
  j0 = (u64)(i0);
  l3 = j0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 1u;
    i0 += i1;
    p2 = i0;
    j0 = l3;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l3 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L5;}
  i0 = p2;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
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
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  goto B6;
  B7:;
  i0 = 0u;
  i1 = l4;
  i2 = p2;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l4 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B6:;
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
  i1 = l4;
  i2 = p2;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l5;
  i1 = 8u;
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
  i0 = f81(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 96));
  j1 = 0ull;
  i2 = l4;
  i3 = p2;
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
  i0 = p2;
  i1 = 513u;
  i0 = i0 < i1;
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
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B13;}
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
  B13:;
  i0 = l5;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  d1 = f64_load((&memory), (u64)(i1));
  l1 = d1;
  d1 = (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(d1);
  d2 = l1;
  i3 = p1;
  i4 = 56u;
  i3 += i4;
  i3 = i32_load8_u((&memory), (u64)(i3));
  d1 = i3 ? d1 : d2;
  f64_store((&memory), (u64)(i0 + 40), d1);
  i0 = l5;
  i1 = 24u;
  i0 += i1;
  i1 = l5;
  i2 = 40u;
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
  i0 = memcmp_0(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = !(i0);
  if (i0) {goto B14;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 100));
  p2 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B16;}
  i0 = p1;
  i1 = 100u;
  i0 += i1;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  i2 = p0;
  j2 = i64_load((&memory), (u64)(i2 + 8));
  j3 = 12531438729690087424ull;
  i4 = l5;
  i5 = 32u;
  i4 += i5;
  j5 = l0;
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
  i1 = (*Z_envZ_db_idx_double_find_primaryZ_ijjjij)(j1, j2, j3, i4, j5);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i1;
  i32_store((&memory), (u64)(i0), i1);
  B16:;
  i0 = p2;
  j1 = 0ull;
  i2 = l5;
  i3 = 40u;
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
  (*Z_envZ_db_idx_double_updateZ_viji)(i0, j1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
  i0 = 0u;
  i1 = l5;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f145(u32 p0, u32 p1, u32 p2) {
  u32 l1 = 0, l2 = 0, l3 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 16u;
  i0 -= i1;
  l6 = i0;
  l5 = i0;
  i0 = 0u;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = p0;
  i0 = i0 == i1;
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
  i0 = p1;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1));
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = 16u;
  l3 = i0;
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  l4 = j0;
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  l2 = i0;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l1 = i1;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = l2;
  i1 = l1;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = l1;
  i2 = i32_load((&memory), (u64)(i2 + 4));
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
  f146(i0, i1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l4 = j0;
  B3:;
  i0 = p1;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1 + 8));
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 8), j1);
  j0 = l0;
  j1 = l4;
  i0 = j0 == j1;
  i1 = 256u;
  i2 = g0;
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
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  p2 = i0;
  i1 = l2;
  i1 = i32_load((&memory), (u64)(i1));
  l2 = i1;
  i0 -= i1;
  l1 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l4 = j0;
  L6: 
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
    if (i0) {goto L6;}
  i0 = l2;
  i1 = p2;
  i0 = i0 == i1;
  if (i0) {goto B7;}
  i0 = l1;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = l3;
  i0 += i1;
  l3 = i0;
  B7:;
  i0 = l3;
  i1 = 49u;
  i0 += i1;
  p2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B9;}
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
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  goto B8;
  B9:;
  i0 = 0u;
  i1 = l6;
  i2 = p2;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l3 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B8:;
  i0 = l5;
  i1 = l3;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l5;
  i1 = l3;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = l3;
  i2 = p2;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
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
  i0 = f147(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 92));
  j1 = 0ull;
  i2 = l3;
  i3 = p2;
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
  i0 = p2;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B13;}
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
  B13:;
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B15;}
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
  B15:;
  i0 = 0u;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f146(u32 p0, u32 p1, u32 p2) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i1 = p1;
  i0 -= i1;
  l0 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  l1 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 8));
  l5 = i1;
  i2 = p0;
  i2 = i32_load((&memory), (u64)(i2));
  l2 = i2;
  i1 -= i2;
  i2 = 3u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  i0 = i0 <= i1;
  if (i0) {goto B3;}
  i0 = l2;
  i0 = !(i0);
  if (i0) {goto B4;}
  i0 = p0;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 4), i1);
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  l5 = i0;
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  B4:;
  i0 = l1;
  i1 = 536870912u;
  i0 = i0 >= i1;
  if (i0) {goto B0;}
  i0 = 536870911u;
  l2 = i0;
  i0 = l5;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  i1 = 268435454u;
  i0 = i0 > i1;
  if (i0) {goto B6;}
  i0 = l1;
  l2 = i0;
  i0 = l5;
  i1 = 2u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  p2 = i0;
  i1 = l1;
  i0 = i0 < i1;
  if (i0) {goto B6;}
  i0 = p2;
  l2 = i0;
  i0 = p2;
  i1 = 536870912u;
  i0 = i0 >= i1;
  if (i0) {goto B0;}
  B6:;
  i0 = p0;
  i1 = l2;
  i2 = 3u;
  i1 <<= (i2 & 31);
  l1 = i1;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
    g0 = i2;
  }
  i1 = f234(i1);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l2 = i1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = l2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i2 = l1;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l0;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B2;}
  i0 = l2;
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
  i1 = 4u;
  i0 += i1;
  p0 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = l0;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto Bfunc;
  B3:;
  i0 = p1;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = l2;
  i1 -= i2;
  l0 = i1;
  i0 += i1;
  i1 = p2;
  i2 = l1;
  i3 = l0;
  i4 = 3u;
  i3 = (u32)((s32)i3 >> (i4 & 31));
  l0 = i3;
  i2 = i2 > i3;
  i0 = i2 ? i0 : i1;
  l5 = i0;
  i1 = p1;
  i0 -= i1;
  l3 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  l4 = i0;
  i0 = !(i0);
  if (i0) {goto B9;}
  i0 = l2;
  i1 = p1;
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
  i0 = (*Z_envZ_memmoveZ_iiii)(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B9:;
  i0 = l1;
  i1 = l0;
  i0 = i0 <= i1;
  if (i0) {goto B1;}
  i0 = p2;
  i1 = l5;
  i0 -= i1;
  p1 = i0;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B2;}
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  p0 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = l5;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = p1;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto Bfunc;
  B2:;
  goto Bfunc;
  B1:;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i1 = l2;
  i2 = l4;
  i3 = 3u;
  i2 <<= (i3 & 31);
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto Bfunc;
  B0:;
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
  f241(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  UNREACHABLE;
  Bfunc:;
  FUNC_EPILOGUE;
}

static u32 f147(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = p1;
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
  i0 = f148(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 32u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 40u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 48u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l1;
  i1 = p1;
  i1 = i32_load8_u((&memory), (u64)(i1 + 56));
  i32_store8((&memory), (u64)(i0 + 15), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 0u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i1 = l1;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 1u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 60u;
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
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 64u;
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
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 72u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 80u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 0u;
  i1 = l1;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f148(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l5 = 0, l6 = 0;
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
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1));
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l4 = j0;
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  l5 = i0;
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
    j0 = l4;
    i0 = (u32)(j0);
    l0 = i0;
    i0 = l6;
    j1 = l4;
    j2 = 7ull;
    j1 >>= (j2 & 63);
    l4 = j1;
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
    i1 = l5;
    i0 -= i1;
    i1 = 0u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 320u;
    i2 = g0;
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
    l5 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l1;
    if (i0) {goto L0;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
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
  l0 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 8u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i0 -= i1;
    i1 = 7u;
    i0 = (u32)((s32)i0 > (s32)i1);
    i1 = 320u;
    i2 = g0;
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
    l5 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l1;
    i1 = l3;
    i2 = 8u;
    i1 += i2;
    l3 = i1;
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

static void f149(u32 p0, u32 p1, u32 p2, u32 p3, u32 p4) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1));
  i0 -= i1;
  i1 = 24u;
  i0 = I32_DIV_S(i0, i1);
  l1 = i0;
  i0 = 178956970u;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 8));
  p2 = i1;
  i0 -= i1;
  i1 = p3;
  i2 = p2;
  i1 -= i2;
  i2 = p1;
  i2 = i32_load((&memory), (u64)(i2 + 4));
  l5 = i2;
  i1 += i2;
  i0 = i0 < i1;
  if (i0) {goto B0;}
  i0 = p2;
  i1 = 536870911u;
  i0 = i0 > i1;
  if (i0) {goto B2;}
  i0 = p2;
  i1 = 3u;
  i0 <<= (i1 & 31);
  i1 = 5u;
  i0 = DIV_U(i0, i1);
  p2 = i0;
  goto B1;
  B2:;
  i0 = 4294967295u;
  i1 = p2;
  i2 = 3u;
  i1 <<= (i2 & 31);
  i2 = p2;
  i3 = 2684354559u;
  i2 = i2 > i3;
  i0 = i2 ? i0 : i1;
  p2 = i0;
  B1:;
  i0 = l5;
  i1 = p3;
  i0 += i1;
  l5 = i0;
  i1 = p2;
  i2 = 178956970u;
  i3 = p2;
  i4 = 178956970u;
  i3 = i3 < i4;
  i1 = i3 ? i1 : i2;
  p2 = i1;
  i2 = l5;
  i3 = p2;
  i2 = i2 > i3;
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i1 = 178956971u;
  i0 = i0 >= i1;
  if (i0) {goto B0;}
  i0 = l2;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  p2 = i0;
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l3 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B4;}
  i0 = l4;
  p2 = i0;
  i0 = l3;
  i0 = !(i0);
  if (i0) {goto B4;}
  i0 = l3;
  l5 = i0;
  i0 = l4;
  p2 = i0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = l5;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = p2;
    i1 = 16u;
    i0 += i1;
    i1 = l5;
    i2 = 16u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = p2;
    i1 = 8u;
    i0 += i1;
    i1 = l5;
    i2 = 8u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = p2;
    i1 = 24u;
    i0 += i1;
    p2 = i0;
    i0 = l5;
    i1 = 24u;
    i0 += i1;
    l5 = i0;
    i1 = l0;
    i0 = i0 != i1;
    if (i0) {goto L5;}
  B4:;
  i0 = p2;
  i1 = p4;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p2;
  i1 = 16u;
  i0 += i1;
  i1 = p4;
  i2 = 16u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p2;
  i1 = p4;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p2;
  i1 = p3;
  i2 = 24u;
  i1 *= i2;
  i0 += i1;
  p2 = i0;
  i0 = l3;
  i0 = !(i0);
  if (i0) {goto B6;}
  i0 = l3;
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = 24u;
  i1 *= i2;
  i0 += i1;
  l5 = i0;
  i1 = l0;
  i0 = i0 == i1;
  if (i0) {goto B7;}
  L8: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = l0;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = p2;
    i1 = 16u;
    i0 += i1;
    i1 = l0;
    i2 = 16u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = p2;
    i1 = 8u;
    i0 += i1;
    i1 = l0;
    i2 = 8u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = p2;
    i1 = 24u;
    i0 += i1;
    p2 = i0;
    i0 = l0;
    i1 = 24u;
    i0 += i1;
    l0 = i0;
    i1 = l5;
    i0 = i0 != i1;
    if (i0) {goto L8;}
  B7:;
  i0 = p1;
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B6:;
  i0 = p1;
  i1 = l4;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p1;
  i1 = 4u;
  i0 += i1;
  i1 = p2;
  i2 = l4;
  i1 -= i2;
  i2 = 24u;
  i1 = I32_DIV_S(i1, i2);
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = l4;
  i2 = l1;
  i3 = 24u;
  i2 *= i3;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
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

static void f150(u32 p0, u32 p1, u32 p2, u32 p3) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0, l6 = 0, l7 = 0, 
      l8 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p2;
  i0 = !(i0);
  if (i0) {goto B2;}
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0));
  l0 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l1 = i1;
  i2 = 24u;
  i1 *= i2;
  i0 += i1;
  l2 = i0;
  i1 = p1;
  i0 -= i1;
  l8 = i0;
  i0 = !(i0);
  if (i0) {goto B1;}
  i0 = l8;
  i1 = 24u;
  i0 = I32_DIV_S(i0, i1);
  i1 = p2;
  i0 = i0 >= i1;
  if (i0) {goto B0;}
  i0 = l2;
  i1 = p1;
  i0 = i0 == i1;
  if (i0) {goto B3;}
  i0 = p2;
  i1 = 24u;
  i0 *= i1;
  l7 = i0;
  i0 = p1;
  l8 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l8;
    i1 = l7;
    i0 += i1;
    l6 = i0;
    i1 = l8;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = l6;
    i1 = 16u;
    i0 += i1;
    i1 = l8;
    i2 = 16u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = l6;
    i1 = 8u;
    i0 += i1;
    i1 = l8;
    i2 = 8u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = l2;
    i1 = l8;
    i2 = 24u;
    i1 += i2;
    l8 = i1;
    i0 = i0 != i1;
    if (i0) {goto L4;}
  B3:;
  i0 = p1;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p1;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  i1 = p3;
  i2 = 16u;
  i1 += i2;
  l8 = i1;
  i1 = i32_load8_u((&memory), (u64)(i1));
  i32_store8((&memory), (u64)(i0), i1);
  i0 = l2;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l0;
  i1 = l1;
  i2 = 24u;
  i1 *= i2;
  i0 += i1;
  l6 = i0;
  i1 = 16u;
  i0 += i1;
  i1 = l8;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l6;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l8 = i0;
  i1 = l8;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = p2;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  B2:;
  goto Bfunc;
  B1:;
  i0 = l2;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l2;
  i1 = 16u;
  i0 += i1;
  i1 = p3;
  i2 = 16u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l2;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l8 = i0;
  i1 = l8;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = p2;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto Bfunc;
  B0:;
  i0 = l2;
  i1 = p2;
  i2 = 4294967272u;
  i1 *= i2;
  l4 = i1;
  i0 += i1;
  l5 = i0;
  i0 = 0u;
  l8 = i0;
  i0 = l2;
  i1 = 0u;
  i2 = p2;
  i1 -= i2;
  i2 = 24u;
  i1 *= i2;
  i0 += i1;
  l3 = i0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l2;
    i1 = l8;
    i0 += i1;
    l6 = i0;
    i1 = l5;
    i2 = l8;
    i1 += i2;
    l7 = i1;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = l6;
    i1 = 16u;
    i0 += i1;
    i1 = l7;
    i2 = 16u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = l6;
    i1 = 8u;
    i0 += i1;
    i1 = l7;
    i2 = 8u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = l4;
    i1 = l8;
    i2 = 24u;
    i1 += i2;
    l8 = i1;
    i0 += i1;
    if (i0) {goto L5;}
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l8 = i0;
  i1 = l8;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = p2;
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = p1;
  i0 = i0 == i1;
  if (i0) {goto B6;}
  i0 = p2;
  i1 = 4294967272u;
  i0 *= i1;
  l7 = i0;
  i1 = p1;
  i0 -= i1;
  l2 = i0;
  i0 = l0;
  i1 = l1;
  i2 = 24u;
  i1 *= i2;
  i0 += i1;
  i1 = 4294967288u;
  i0 += i1;
  l8 = i0;
  L7: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l8;
    i1 = 4294967280u;
    i0 += i1;
    i1 = l8;
    i2 = l7;
    i1 += i2;
    l6 = i1;
    i2 = 4294967280u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = l8;
    i1 = 4294967288u;
    i0 += i1;
    i1 = l6;
    i2 = 4294967288u;
    i1 += i2;
    j1 = i64_load((&memory), (u64)(i1));
    i64_store((&memory), (u64)(i0), j1);
    i0 = l8;
    i1 = l6;
    i1 = i32_load8_u((&memory), (u64)(i1));
    i32_store8((&memory), (u64)(i0), i1);
    i0 = l8;
    i1 = 4294967272u;
    i0 += i1;
    l8 = i0;
    i1 = l2;
    i0 += i1;
    i1 = 4294967288u;
    i0 = i0 != i1;
    if (i0) {goto L7;}
  B6:;
  i0 = p1;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p1;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p1;
  i1 = 16u;
  i0 += i1;
  i1 = p3;
  i2 = 16u;
  i1 += i2;
  i1 = i32_load8_u((&memory), (u64)(i1));
  i32_store8((&memory), (u64)(i0), i1);
  Bfunc:;
  FUNC_EPILOGUE;
}

static u32 f151(u32 p0, u64 p1, u32 p2) {
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
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = p0;
  i0 = i0 == i1;
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
  goto B2;
  B3:;
  i0 = 0u;
  l3 = i0;
  i0 = p0;
  j0 = i64_load((&memory), (u64)(i0));
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  j2 = 15938991009778630656ull;
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
  i0 = f135(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = p0;
  i0 = i0 == i1;
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

static void f152(u32 p0, u32 p1, u32 p2) {
  u32 l1 = 0, l2 = 0, l3 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  f64 d1, d2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 16u;
  i0 -= i1;
  l6 = i0;
  l5 = i0;
  i0 = 0u;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = p0;
  i0 = i0 == i1;
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
  i0 = p1;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1));
  d1 = f64_load((&memory), (u64)(i1));
  i2 = p2;
  i3 = 4u;
  i2 += i3;
  i2 = i32_load((&memory), (u64)(i2));
  i3 = 40u;
  i2 += i3;
  d2 = f64_load((&memory), (u64)(i2));
  d1 = (*Z_eosio_injectionZ__eosio_f64_subZ_ddd)(d1, d2);
  i2 = p1;
  d2 = f64_load((&memory), (u64)(i2 + 48));
  d1 = (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(d1, d2);
  f64_store((&memory), (u64)(i0 + 48), d1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = 1u;
  i1 = 256u;
  i2 = g0;
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
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  l1 = i1;
  i0 -= i1;
  l3 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l4 = j0;
  i0 = 16u;
  p2 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 1u;
    i0 += i1;
    p2 = i0;
    j0 = l4;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L4;}
  i0 = l1;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  i0 = l3;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = p2;
  i0 += i1;
  p2 = i0;
  B5:;
  i0 = p2;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
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
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  goto B6;
  B7:;
  i0 = 0u;
  i1 = l6;
  i2 = l2;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  p2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B6:;
  i0 = l5;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l5;
  i1 = p2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = p2;
  i2 = l2;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
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
  i0 = f147(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 92));
  j1 = 0ull;
  i2 = p2;
  i3 = l2;
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
  i0 = l2;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B11;}
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
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B13;}
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
  B13:;
  i0 = 0u;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f153(u32 p0, u32 p1, u32 p2) {
  u32 l2 = 0, l4 = 0, l5 = 0;
  u64 l0 = 0, l3 = 0;
  f64 l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1, j2, j3, j4, j5;
  f64 d1, d2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 48u;
  i1 -= i2;
  l4 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i1 = 92u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p0;
  i0 = i0 == i1;
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
  i0 = p1;
  i1 = 8u;
  i0 += i1;
  l5 = i0;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1));
  d1 = f64_load((&memory), (u64)(i1));
  i2 = l5;
  d2 = f64_load((&memory), (u64)(i2));
  l1 = d2;
  d1 = (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(d1, d2);
  f64_store((&memory), (u64)(i0), d1);
  i0 = l4;
  l5 = i0;
  d1 = l1;
  d1 = (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(d1);
  d2 = l1;
  i3 = p1;
  i4 = 56u;
  i3 += i4;
  i3 = i32_load8_u((&memory), (u64)(i3));
  d1 = i3 ? d1 : d2;
  f64_store((&memory), (u64)(i0 + 24), d1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 288u;
  i0 += i1;
  l2 = i0;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1));
  d1 = f64_load((&memory), (u64)(i1));
  i2 = l2;
  d2 = f64_load((&memory), (u64)(i2));
  d1 = (*Z_eosio_injectionZ__eosio_f64_addZ_ddd)(d1, d2);
  f64_store((&memory), (u64)(i0), d1);
  i0 = 1u;
  i1 = 256u;
  i2 = g0;
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
  j0 = i64_load32_u((&memory), (u64)(i0 + 16));
  l3 = j0;
  i0 = 64u;
  p2 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 1u;
    i0 += i1;
    p2 = i0;
    j0 = l3;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l3 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L4;}
  i0 = p1;
  i1 = 64u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  i1 = p1;
  i1 = i32_load8_u((&memory), (u64)(i1 + 60));
  l2 = i1;
  i2 = 1u;
  i1 >>= (i2 & 31);
  i2 = l2;
  i3 = 1u;
  i2 &= i3;
  i0 = i2 ? i0 : i1;
  l2 = i0;
  i1 = p2;
  i0 += i1;
  p2 = i0;
  i0 = l2;
  j0 = (u64)(i0);
  l3 = j0;
  L5: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 1u;
    i0 += i1;
    p2 = i0;
    j0 = l3;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l3 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L5;}
  i0 = p2;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
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
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l4 = i0;
  goto B6;
  B7:;
  i0 = 0u;
  i1 = l4;
  i2 = p2;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  l4 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B6:;
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 12), i1);
  i0 = l5;
  i1 = l4;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
  i1 = l4;
  i2 = p2;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 16), i1);
  i0 = l5;
  i1 = 8u;
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
  i0 = f81(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 96));
  j1 = 0ull;
  i2 = l4;
  i3 = p2;
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
  i0 = p2;
  i1 = 513u;
  i0 = i0 < i1;
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
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B13;}
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
  B13:;
  i0 = l5;
  i1 = p1;
  i2 = 8u;
  i1 += i2;
  d1 = f64_load((&memory), (u64)(i1));
  l1 = d1;
  d1 = (*Z_eosio_injectionZ__eosio_f64_negZ_dd)(d1);
  d2 = l1;
  i3 = p1;
  i4 = 56u;
  i3 += i4;
  i3 = i32_load8_u((&memory), (u64)(i3));
  d1 = i3 ? d1 : d2;
  f64_store((&memory), (u64)(i0 + 40), d1);
  i0 = l5;
  i1 = 24u;
  i0 += i1;
  i1 = l5;
  i2 = 40u;
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
  i0 = memcmp_0(i0, i1, i2);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = !(i0);
  if (i0) {goto B14;}
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 100));
  p2 = i0;
  i1 = 4294967295u;
  i0 = (u32)((s32)i0 > (s32)i1);
  if (i0) {goto B16;}
  i0 = p1;
  i1 = 100u;
  i0 += i1;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1));
  i2 = p0;
  j2 = i64_load((&memory), (u64)(i2 + 8));
  j3 = 12531438729690087424ull;
  i4 = l5;
  i5 = 32u;
  i4 += i5;
  j5 = l0;
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
  i1 = (*Z_envZ_db_idx_double_find_primaryZ_ijjjij)(j1, j2, j3, i4, j5);
  i2 = g0;
  i3 = 1u;
  i2 += i3;
  g0 = i2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i1;
  i32_store((&memory), (u64)(i0), i1);
  B16:;
  i0 = p2;
  j1 = 0ull;
  i2 = l5;
  i3 = 40u;
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
  (*Z_envZ_db_idx_double_updateZ_viji)(i0, j1, i2);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B14:;
  i0 = 0u;
  i1 = l5;
  i2 = 48u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void f154(u32 p0, u32 p1, u32 p2) {
  u32 l1 = 0, l2 = 0, l3 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5;
  u64 j0, j1, j2, j3, j4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i0 = i32_load((&memory), (u64)(i0 + 4));
  i1 = 16u;
  i0 -= i1;
  l6 = i0;
  l5 = i0;
  i0 = 0u;
  i1 = l6;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 88));
  i1 = p0;
  i0 = i0 == i1;
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
  i0 = p1;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1));
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = p1;
  j0 = i64_load((&memory), (u64)(i0));
  l0 = j0;
  i0 = 1u;
  i1 = 256u;
  i2 = g0;
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
  i1 = 20u;
  i0 += i1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 16));
  l1 = i1;
  i0 -= i1;
  l3 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  j0 = (u64)(i0);
  l4 = j0;
  i0 = 16u;
  p2 = i0;
  L4: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p2;
    i1 = 1u;
    i0 += i1;
    p2 = i0;
    j0 = l4;
    j1 = 7ull;
    j0 >>= (j1 & 63);
    l4 = j0;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto L4;}
  i0 = l1;
  i1 = l2;
  i0 = i0 == i1;
  if (i0) {goto B5;}
  i0 = l3;
  i1 = 4294967288u;
  i0 &= i1;
  i1 = p2;
  i0 += i1;
  p2 = i0;
  B5:;
  i0 = p2;
  i1 = 49u;
  i0 += i1;
  l2 = i0;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B7;}
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
  i0 = malloc_0(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p2 = i0;
  goto B6;
  B7:;
  i0 = 0u;
  i1 = l6;
  i2 = l2;
  i3 = 15u;
  i2 += i3;
  i3 = 4294967280u;
  i2 &= i3;
  i1 -= i2;
  p2 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  B6:;
  i0 = l5;
  i1 = p2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = l5;
  i1 = p2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l5;
  i1 = p2;
  i2 = l2;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 8), i1);
  i0 = l5;
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
  i0 = f147(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0 + 92));
  j1 = 0ull;
  i2 = p2;
  i3 = l2;
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
  i0 = l2;
  i1 = 513u;
  i0 = i0 < i1;
  if (i0) {goto B11;}
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
  free_0(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B11:;
  j0 = l0;
  i1 = p0;
  j1 = i64_load((&memory), (u64)(i1 + 16));
  i0 = j0 < j1;
  if (i0) {goto B13;}
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
  B13:;
  i0 = 0u;
  i1 = l5;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static u32 f155(u32 p0, u32 p1, u32 p2) {
  u32 l1 = 0, l2 = 0;
  u64 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3;
  u64 j0, j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 8), j1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 40), j1);
  i0 = 0u;
  l1 = i0;
  i0 = p0;
  i1 = 24u;
  i0 += i1;
  i1 = 0u;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = p0;
  i1 = 0u;
  i32_store8((&memory), (u64)(i0 + 56), i1);
  i0 = p0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 60), i1);
  i0 = p0;
  i1 = 0u;
  i32_store((&memory), (u64)(i0 + 64), i1);
  i0 = p0;
  j1 = 0ull;
  i64_store((&memory), (u64)(i0 + 72), j1);
  i0 = p0;
  i1 = 80u;
  i0 += i1;
  l2 = i0;
  j1 = 1397703940ull;
  i64_store((&memory), (u64)(i0), j1);
  i0 = 1u;
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
  i0 = l2;
  j0 = i64_load((&memory), (u64)(i0));
  j1 = 8ull;
  j0 >>= (j1 & 63);
  l0 = j0;
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
  i1 = 704u;
  i2 = g0;
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
  i32_store((&memory), (u64)(i0 + 88), i1);
  i0 = p2;
  i0 = i32_load((&memory), (u64)(i0 + 4));
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
  i0 = f156(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i1 = p2;
  i1 = i32_load((&memory), (u64)(i1 + 8));
  i1 = i32_load((&memory), (u64)(i1));
  i32_store((&memory), (u64)(i0 + 92), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f156(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 16u;
  i1 -= i2;
  l1 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i1 = p1;
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
  i0 = f157(i0, i1);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  p0 = i0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 32u;
  i0 += i1;
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 40u;
  i0 += i1;
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 48u;
  i0 += i1;
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 = i0 != i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 15u;
  i0 += i1;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 1u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p1;
  i1 = l1;
  i1 = i32_load8_u((&memory), (u64)(i1 + 15));
  i2 = 0u;
  i1 = i1 != i2;
  i32_store8((&memory), (u64)(i0 + 56), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 60u;
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
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 64u;
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
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 72u;
  i0 += i1;
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 80u;
  i0 += i1;
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 0u;
  i1 = l1;
  i2 = 16u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f157(u32 p0, u32 p1) {
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
    i1 = 416u;
    i2 = g0;
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
  l3 = i0;
  i1 = p1;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l1 = i1;
  i2 = p1;
  i2 = i32_load((&memory), (u64)(i2));
  l2 = i2;
  i1 -= i2;
  i2 = 3u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  l5 = i1;
  i0 = i0 <= i1;
  if (i0) {goto B4;}
  i0 = p1;
  i1 = l3;
  i2 = l5;
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
  f158(i0, i1);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p1;
  i0 = i32_load((&memory), (u64)(i0));
  l2 = i0;
  i1 = p1;
  i2 = 4u;
  i1 += i2;
  i1 = i32_load((&memory), (u64)(i1));
  l1 = i1;
  i0 = i0 != i1;
  if (i0) {goto B3;}
  goto B2;
  B4:;
  i0 = l3;
  i1 = l5;
  i0 = i0 >= i1;
  if (i0) {goto B6;}
  i0 = p1;
  i1 = 4u;
  i0 += i1;
  i1 = l2;
  i2 = l3;
  i3 = 3u;
  i2 <<= (i3 & 31);
  i1 += i2;
  l1 = i1;
  i32_store((&memory), (u64)(i0), i1);
  B6:;
  i0 = l2;
  i1 = l1;
  i0 = i0 == i1;
  if (i0) {goto B2;}
  B3:;
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  l3 = i0;
  i0 = i32_load((&memory), (u64)(i0));
  l5 = i0;
  L7: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = p0;
    i1 = 8u;
    i0 += i1;
    i0 = i32_load((&memory), (u64)(i0));
    i1 = l5;
    i0 -= i1;
    i1 = 7u;
    i0 = i0 > i1;
    i1 = 368u;
    i2 = g0;
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
    i1 = l3;
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
    i0 = l3;
    i1 = l3;
    i1 = i32_load((&memory), (u64)(i1));
    i2 = 8u;
    i1 += i2;
    l5 = i1;
    i32_store((&memory), (u64)(i0), i1);
    i0 = l1;
    i1 = l2;
    i2 = 8u;
    i1 += i2;
    l2 = i1;
    i0 = i0 != i1;
    if (i0) {goto L7;}
  B2:;
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void f158(u32 p0, u32 p1) {
  u32 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0, l5 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  u64 j1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  l5 = i0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  l0 = i1;
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  i1 = p1;
  i0 = i0 >= i1;
  if (i0) {goto B4;}
  i0 = l0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1));
  l1 = i1;
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  l2 = i0;
  i1 = p1;
  i0 += i1;
  l0 = i0;
  i1 = 536870912u;
  i0 = i0 >= i1;
  if (i0) {goto B2;}
  i0 = 536870911u;
  l4 = i0;
  i0 = l5;
  i1 = l1;
  i0 -= i1;
  l5 = i0;
  i1 = 3u;
  i0 = (u32)((s32)i0 >> (i1 & 31));
  i1 = 268435454u;
  i0 = i0 > i1;
  if (i0) {goto B5;}
  i0 = l0;
  i1 = l5;
  i2 = 2u;
  i1 = (u32)((s32)i1 >> (i2 & 31));
  l4 = i1;
  i2 = l4;
  i3 = l0;
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
  i0 = f234(i0);
  i1 = g0;
  i2 = 1u;
  i1 += i2;
  g0 = i1;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  l5 = i0;
  goto B0;
  B4:;
  i0 = l0;
  l4 = i0;
  i0 = p1;
  l5 = i0;
  L7: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    j1 = 0ull;
    i64_store((&memory), (u64)(i0), j1);
    i0 = l4;
    i1 = 8u;
    i0 += i1;
    l4 = i0;
    i0 = l5;
    i1 = 4294967295u;
    i0 += i1;
    l5 = i0;
    if (i0) {goto L7;}
  i0 = p0;
  i1 = 4u;
  i0 += i1;
  i1 = l0;
  i2 = p1;
  i3 = 3u;
  i2 <<= (i3 & 31);
  i1 += i2;
  i32_store((&memory), (u64)(i0), i1);
  goto Bfunc;
  B3:;
  i0 = 0u;
  l4 = i0;
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
  f241(i0);
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
  i1 = l4;
  i2 = 3u;
  i1 <<= (i2 & 31);
  i0 += i1;
  l1 = i0;
  i0 = l5;
  i1 = l2;
  i2 = 3u;
  i1 <<= (i2 & 31);
  i0 += i1;
  l0 = i0;
  l4 = i0;
  i0 = p1;
  l5 = i0;
  L10: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    i0 = l4;
    j1 = 0ull;
    i64_store((&memory), (u64)(i0), j1);
    i0 = l4;
    i1 = 8u;
    i0 += i1;
    l4 = i0;
    i0 = l5;
    i1 = 4294967295u;
    i0 += i1;
    l5 = i0;
    if (i0) {goto L10;}
  i0 = l0;
  i1 = p1;
  i2 = 3u;
  i1 <<= (i2 & 31);
  i0 += i1;
  l2 = i0;
  i0 = l0;
  i1 = p0;
  i2 = 4u;
  i1 += i2;
  l3 = i1;
  i1 = i32_load((&memory), (u64)(i1));
  i2 = p0;
  i2 = i32_load((&memory), (u64)(i2));
  l4 = i2;
  i1 -= i2;
  l5 = i1;
  i0 -= i1;
  p1 = i0;
  i0 = l5;
  i1 = 1u;
  i0 = (u32)((s32)i0 < (s32)i1);
  if (i0) {goto B11;}
  i0 = p1;
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
  B11:;
  i0 = p0;
  i1 = p1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l3;
  i1 = l2;
  i32_store((&memory), (u64)(i0), i1);
  i0 = p0;
  i1 = 8u;
  i0 += i1;
  i1 = l1;
  i32_store((&memory), (u64)(i0), i1);
  i0 = l4;
  i0 = !(i0);
  if (i0) {goto B13;}
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
  f235(i0);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  B13:;
  Bfunc:;
  FUNC_EPILOGUE;
}

static u32 f159(u32 p0, u32 p1) {
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

static u32 f160(u32 p0, u32 p1) {
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
  i1 = 320u;
  i2 = g0;
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
  i1 = 320u;
  i2 = g0;
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
  i1 = 320u;
  i2 = g0;
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
    i1 = 320u;
    i2 = g0;
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
  i1 = 320u;
  i2 = g0;
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
    i1 = 320u;
    i2 = g0;
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

static u32 f161(u32 p0, u32 p1) {
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
    i1 = 320u;
    i2 = g0;
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
    i1 = 320u;
    i2 = g0;
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
    i1 = 320u;
    i2 = g0;
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
    i0 = f95(i0, i1);
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
    i0 = f96(i0, i1);
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

static u32 f162(u32 p0, u32 p1) {
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
    i1 = 320u;
    i2 = g0;
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
    i1 = 320u;
    i2 = g0;
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
    i0 = f96(i0, i1);
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

static u32 f163(u32 p0, u32 p1) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 3u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 32u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = (u32)((s32)i0 > (s32)i1);
  i1 = 320u;
  i2 = g0;
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
  i2 = 40u;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f164(u32 p0, u32 p1) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 3u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 24u;
  i0 += i1;
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 32u;
  i0 += i1;
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 40u;
  i0 += i1;
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static u32 f165(u32 p0, u32 p1) {
  u32 l0 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 24u;
  i0 += i1;
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 32u;
  i0 += i1;
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  l0 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  i0 = i32_load((&memory), (u64)(i0 + 8));
  i1 = l0;
  i0 -= i1;
  i1 = 7u;
  i0 = i0 > i1;
  i1 = 368u;
  i2 = g0;
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
  i1 = 40u;
  i0 += i1;
  i1 = p0;
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
  i0 = p0;
  i1 = p0;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 8u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = p0;
  FUNC_EPILOGUE;
  return i0;
}

static void _ZN11eosiosystem15system_contract10delegatebwEyyN5eosio5assetES2_b(u32 p0, u64 p1, u64 p2, u32 p3, u32 p4, u32 p5) {
  u32 l4 = 0, l5 = 0, l6 = 0;
  u64 l0 = 0, l1 = 0, l2 = 0, l3 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 64u;
  i1 -= i2;
  l6 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 1u;
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
  i0 = 0u;
  l4 = i0;
  j0 = 5459781ull;
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
    l3 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B4;}
    L5: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B2;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L5;}
    B4:;
    i0 = 1u;
    l5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L3;}
    goto B1;
  B2:;
  i0 = 0u;
  l5 = i0;
  B1:;
  i0 = l5;
  i1 = 704u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0 + 8));
  l1 = j0;
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0));
  l2 = j0;
  j1 = 63ull;
  j0 >>= (j1 & 63);
  i0 = (u32)(j0);
  i1 = 1u;
  i0 ^= i1;
  i1 = 2992u;
  i2 = g0;
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
  j0 = 5459781ull;
  l3 = j0;
  i0 = 0u;
  l4 = i0;
  L12: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B11;}
    j0 = l3;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l3 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B13;}
    L14: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B11;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L14;}
    B13:;
    i0 = 1u;
    l5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L12;}
    goto B10;
  B11:;
  i0 = 0u;
  l5 = i0;
  B10:;
  i0 = l5;
  i1 = 704u;
  i2 = g0;
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
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j0 = i64_load((&memory), (u64)(i0));
  l3 = j0;
  j1 = 63ull;
  j0 >>= (j1 & 63);
  i0 = (u32)(j0);
  i1 = 1u;
  i0 ^= i1;
  i1 = 2992u;
  i2 = g0;
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
  j1 = l0;
  i0 = j0 == j1;
  i1 = 1680u;
  i2 = g0;
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
  j1 = l3;
  j0 += j1;
  l1 = j0;
  j1 = 13835058055282163712ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 1728u;
  i2 = g0;
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
  j1 = 4611686018427387904ull;
  i0 = (u64)((s64)j0 < (s64)j1);
  i1 = 1760u;
  i2 = g0;
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
  j0 = 5459781ull;
  l3 = j0;
  i0 = 0u;
  l4 = i0;
  L24: 
    (*Z_eosio_injectionZ_checktimeZ_vv)();
    j0 = l3;
    i0 = (u32)(j0);
    i1 = 24u;
    i0 <<= (i1 & 31);
    i1 = 3221225471u;
    i0 += i1;
    i1 = 452984830u;
    i0 = i0 > i1;
    if (i0) {goto B23;}
    j0 = l3;
    j1 = 8ull;
    j0 >>= (j1 & 63);
    l3 = j0;
    j1 = 255ull;
    j0 &= j1;
    j1 = 0ull;
    i0 = j0 != j1;
    if (i0) {goto B25;}
    L26: 
      (*Z_eosio_injectionZ_checktimeZ_vv)();
      j0 = l3;
      j1 = 8ull;
      j0 >>= (j1 & 63);
      l3 = j0;
      j1 = 255ull;
      j0 &= j1;
      j1 = 0ull;
      i0 = j0 != j1;
      if (i0) {goto B23;}
      i0 = l4;
      i1 = 1u;
      i0 += i1;
      l4 = i0;
      i1 = 7u;
      i0 = (u32)((s32)i0 < (s32)i1);
      if (i0) {goto L26;}
    B25:;
    i0 = 1u;
    l5 = i0;
    i0 = l4;
    i1 = 1u;
    i0 += i1;
    l4 = i0;
    i1 = 7u;
    i0 = (u32)((s32)i0 < (s32)i1);
    if (i0) {goto L24;}
    goto B22;
  B23:;
  i0 = 0u;
  l5 = i0;
  B22:;
  i0 = l5;
  i1 = 704u;
  i2 = g0;
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
  j1 = 1397703940ull;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
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
  j1 = 0ull;
  i0 = (u64)((s64)j0 > (s64)j1);
  i1 = 2992u;
  i2 = g0;
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
  i1 = 48u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l4 = i0;
  i1 = p3;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l6;
  i1 = 32u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  l5 = i0;
  i1 = p4;
  i2 = 8u;
  i1 += i2;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l6;
  i1 = p3;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 48), j1);
  i0 = l6;
  i1 = p4;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0 + 32), j1);
  i0 = l6;
  i1 = 16u;
  i0 += i1;
  i1 = 8u;
  i0 += i1;
  i1 = l4;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l6;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1 + 48));
  i64_store((&memory), (u64)(i0 + 16), j1);
  i0 = l6;
  i1 = 8u;
  i0 += i1;
  i1 = l5;
  j1 = i64_load((&memory), (u64)(i1));
  i64_store((&memory), (u64)(i0), j1);
  i0 = l6;
  i1 = l6;
  j1 = i64_load((&memory), (u64)(i1 + 32));
  i64_store((&memory), (u64)(i0), j1);
  i0 = p0;
  j1 = p1;
  j2 = p2;
  i3 = l6;
  i4 = 16u;
  i3 += i4;
  i4 = l6;
  i5 = p5;
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
  _ZN11eosiosystem15system_contract8changebwEyyN5eosio5assetES2_b(i0, j1, j2, i3, i4, i5);
  i0 = g0;
  i1 = 1u;
  i0 += i1;
  g0 = i0;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = l6;
  i2 = 64u;
  i1 += i2;
  i32_store((&memory), (u64)(i0 + 4), i1);
  FUNC_EPILOGUE;
}

static void _ZN11eosiosystem15system_contract12undelegatebwEyyN5eosio5assetES2_(u32 p0, u64 p1, u64 p2, u32 p3, u32 p4) {
  u32 l5 = 0, l6 = 0, l7 = 0;
  u64 l0 = 0, l1 = 0, l2 = 0, l3 = 0, l4 = 0;
  FUNC_PROLOGUE;
  u32 i0, i1, i2, i3, i4, i5, i6, i7;
  u64 j0, j1, j2;
  (*Z_eosio_injectionZ_checktimeZ_vv)();
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&memory), (u64)(i1 + 4));
  i2 = 64u;
  i1 -= i2;
  l7 = i1;
  i32_store((&memory), (u64)(i0 + 4), i1);
  i0 = 1u;
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
  i0 = 0u;
  l5 = i0;
  j0 = 5459781ull;
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
    l6 = i0;
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
  l6 = i0;
  B1:;
  i0 = l6;
  i1 = 704u;
  i2 = g0;
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
  j0 = 1397703940ull;
  i1 = p4;
  j1 = i64_load((&memory), (u64)(i1 + 8));
  l2 = j1;
  i0 = j0 == j1;
  i1 = 1584u;
  i2 = g0;
  i2 = !(i2);
  if (i2) {
    (*Z_eosio_injectionZ_call_depth_assertZ_vv)();
  } else {
    i2 = 4294967295u;
    i3 = g0;
    i2 += i3;
  }
  }