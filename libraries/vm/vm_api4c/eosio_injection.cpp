
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "vm_api4c.h"
#include <softfloat.hpp>
#include <vm_api/vm_api.h>
#include <env.h>

extern "C" {
static constexpr uint32_t inv_float_eps = 0x4B000000;
static constexpr uint64_t inv_double_eps = 0x4330000000000000;

#define DBL_EPSILON 2.22044604925031308085e-16


#define EOS_THROW(a, b) get_vm_api()->eosio_assert(0, b)

void vm_checktime() {
    static int counter = 0;
    counter += 1;
    if (counter < 10) {
        return;
    }
    counter = 0;

    if (get_vm_api()->is_in_apply_context) {
        get_vm_api()->checktime();
    }
}

static void call_depth_assert() {
    get_vm_api()->eosio_assert(0, "Exceeded call depth maximum");
}

static bool is_nan( const float32_t f ) {
    return f32_is_nan( f );
}
static bool is_nan( const float64_t f ) {
    return f64_is_nan( f );
}
static bool is_nan( const float128_t& f ) {
    return f128_is_nan( f );
}

// float binops
static float _eosio_f32_add( float a, float b ) {
    float32_t ret = f32_add( to_softfloat32(a), to_softfloat32(b) );
    return *reinterpret_cast<float*>(&ret);
}
static float _eosio_f32_sub( float a, float b ) {
    float32_t ret = f32_sub( to_softfloat32(a), to_softfloat32(b) );
    return *reinterpret_cast<float*>(&ret);
}
static f32 _eosio_f32_div( f32 a, f32 b ) {
    float32_t ret = f32_div( to_softfloat32(a), to_softfloat32(b) );
    return *reinterpret_cast<float*>(&ret);
}
static float _eosio_f32_mul( float a, float b ) {
    float32_t ret = f32_mul( to_softfloat32(a), to_softfloat32(b) );
    return *reinterpret_cast<float*>(&ret);
}
static float _eosio_f32_min( float af, float bf ) {
    float32_t a = to_softfloat32(af);
    float32_t b = to_softfloat32(bf);
    if (is_nan(a)) {
    return af;
    }
    if (is_nan(b)) {
    return bf;
    }
    if ( f32_sign_bit(a) != f32_sign_bit(b) ) {
    return f32_sign_bit(a) ? af : bf;
    }
    return f32_lt(a,b) ? af : bf;
}
static float _eosio_f32_max( float af, float bf ) {
    float32_t a = to_softfloat32(af);
    float32_t b = to_softfloat32(bf);
    if (is_nan(a)) {
    return af;
    }
    if (is_nan(b)) {
    return bf;
    }
    if ( f32_sign_bit(a) != f32_sign_bit(b) ) {
    return f32_sign_bit(a) ? bf : af;
    }
    return f32_lt( a, b ) ? bf : af;
}
static float _eosio_f32_copysign( float af, float bf ) {
    float32_t a = to_softfloat32(af);
    float32_t b = to_softfloat32(bf);
    uint32_t sign_of_a = a.v >> 31;
    uint32_t sign_of_b = b.v >> 31;
    a.v &= ~(1 << 31);             // clear the sign bit
    a.v = a.v | (sign_of_b << 31); // add the sign of b
    return from_softfloat32(a);
}
// float unops
static float _eosio_f32_abs( float af ) {
    float32_t a = to_softfloat32(af);
    a.v &= ~(1 << 31);
    return from_softfloat32(a);
}
static float _eosio_f32_neg( float af ) {
    float32_t a = to_softfloat32(af);
    uint32_t sign = a.v >> 31;
    a.v &= ~(1 << 31);
    a.v |= (!sign << 31);
    return from_softfloat32(a);
}
static float _eosio_f32_sqrt( float a ) {
    float32_t ret = f32_sqrt( to_softfloat32(a) );
    return from_softfloat32(ret);
}
// ceil, floor, trunc and nearest are lifted from libc
static float _eosio_f32_ceil( float af ) {
    float32_t a = to_softfloat32(af);
    int e = (int)(a.v >> 23 & 0xFF) - 0X7F;
    uint32_t m;
    if (e >= 23)
    return af;
    if (e >= 0) {
    m = 0x007FFFFF >> e;
    if ((a.v & m) == 0)
        return af;
    if (a.v >> 31 == 0)
        a.v += m;
    a.v &= ~m;
    } else {
    if (a.v >> 31)
        a.v = 0x80000000; // return -0.0f
    else if (a.v << 1)
        a.v = 0x3F800000; // return 1.0f
    }

    return from_softfloat32(a);
}
static float _eosio_f32_floor( float af ) {
    float32_t a = to_softfloat32(af);
    int e = (int)(a.v >> 23 & 0xFF) - 0X7F;
    uint32_t m;
    if (e >= 23)
    return af;
    if (e >= 0) {
    m = 0x007FFFFF >> e;
    if ((a.v & m) == 0)
        return af;
    if (a.v >> 31)
        a.v += m;
    a.v &= ~m;
    } else {
    if (a.v >> 31 == 0)
        a.v = 0;
    else if (a.v << 1)
        a.v = 0xBF800000; // return -1.0f
    }
    return from_softfloat32(a);
}
static float _eosio_f32_trunc( float af ) {
    float32_t a = to_softfloat32(af);
    int e = (int)(a.v >> 23 & 0xff) - 0x7f + 9;
    uint32_t m;
    if (e >= 23 + 9)
    return af;
    if (e < 9)
    e = 1;
    m = -1U >> e;
    if ((a.v & m) == 0)
    return af;
    a.v &= ~m;
    return from_softfloat32(a);
}
static float _eosio_f32_nearest( float af ) {
    float32_t a = to_softfloat32(af);
    int e = a.v>>23 & 0xff;
    int s = a.v>>31;
    float32_t y;
    if (e >= 0x7f+23)
    return af;
    if (s)
    y = f32_add( f32_sub( a, float32_t{inv_float_eps} ), float32_t{inv_float_eps} );
    else
    y = f32_sub( f32_add( a, float32_t{inv_float_eps} ), float32_t{inv_float_eps} );
    if (f32_eq( y, {0} ) )
    return s ? -0.0f : 0.0f;
    return from_softfloat32(y);
}

// float relops
static u32 _eosio_f32_eq( f32 a, f32 b ) {  return f32_eq( to_softfloat32(a), to_softfloat32(b) ); }
static u32 _eosio_f32_ne( f32 a, f32 b ) { return !f32_eq( to_softfloat32(a), to_softfloat32(b) ); }
static bool _eosio_f32_lt( float a, float b ) { return f32_lt( to_softfloat32(a), to_softfloat32(b) ); }
static u32 _eosio_f32_le( f32 a, f32 b ) { return f32_le( to_softfloat32(a), to_softfloat32(b) ); }
static u32 _eosio_f32_gt( f32 af, f32 bf ) {
    float32_t a = to_softfloat32(af);
    float32_t b = to_softfloat32(bf);
    if (is_nan(a))
    return false;
    if (is_nan(b))
    return false;
    return !f32_le( a, b );
}
static bool _eosio_f32_ge( float af, float bf ) {
    float32_t a = to_softfloat32(af);
    float32_t b = to_softfloat32(bf);
    if (is_nan(a))
    return false;
    if (is_nan(b))
    return false;
    return !f32_lt( a, b );
}

// double binops
static f64 _eosio_f64_add( f64 a, f64 b ) {
    float64_t ret = f64_add( to_softfloat64(a), to_softfloat64(b) );
    return from_softfloat64(ret);
}
static f64 _eosio_f64_sub( f64 a, f64 b ) {
    float64_t ret = f64_sub( to_softfloat64(a), to_softfloat64(b) );
    return from_softfloat64(ret);
}
static f64 _eosio_f64_div( f64 a, f64 b ) {
    float64_t ret = f64_div( to_softfloat64(a), to_softfloat64(b) );
    return from_softfloat64(ret);
}
static f64 _eosio_f64_mul( f64 a, f64 b ) {
    float64_t ret = f64_mul( to_softfloat64(a), to_softfloat64(b) );
    return from_softfloat64(ret);
}
static double _eosio_f64_min( double af, double bf ) {
    float64_t a = to_softfloat64(af);
    float64_t b = to_softfloat64(bf);
    if (is_nan(a))
    return af;
    if (is_nan(b))
    return bf;
    if (f64_sign_bit(a) != f64_sign_bit(b))
    return f64_sign_bit(a) ? af : bf;
    return f64_lt( a, b ) ? af : bf;
}
static double _eosio_f64_max( double af, double bf ) {
    float64_t a = to_softfloat64(af);
    float64_t b = to_softfloat64(bf);
    if (is_nan(a))
    return af;
    if (is_nan(b))
    return bf;
    if (f64_sign_bit(a) != f64_sign_bit(b))
    return f64_sign_bit(a) ? bf : af;
    return f64_lt( a, b ) ? bf : af;
}
static double _eosio_f64_copysign( double af, double bf ) {
    float64_t a = to_softfloat64(af);
    float64_t b = to_softfloat64(bf);
    uint64_t sign_of_a = a.v >> 63;
    uint64_t sign_of_b = b.v >> 63;
    a.v &= ~(uint64_t(1) << 63);             // clear the sign bit
    a.v = a.v | (sign_of_b << 63); // add the sign of b
    return from_softfloat64(a);
}

// double unops
static double _eosio_f64_abs( double af ) {
    float64_t a = to_softfloat64(af);
    a.v &= ~(uint64_t(1) << 63);
    return from_softfloat64(a);
}
static f64 _eosio_f64_neg( f64 af ) {
    float64_t a = to_softfloat64(af);
    uint64_t sign = a.v >> 63;
    a.v &= ~(uint64_t(1) << 63);
    a.v |= (uint64_t(!sign) << 63);
    return from_softfloat64(a);
}
static double _eosio_f64_sqrt( double a ) {
    float64_t ret = f64_sqrt( to_softfloat64(a) );
    return from_softfloat64(ret);
}
// ceil, floor, trunc and nearest are lifted from libc
static double _eosio_f64_ceil( double af ) {
    float64_t a = to_softfloat64( af );
    float64_t ret;
    int e = a.v >> 52 & 0x7ff;
    float64_t y;
    if (e >= 0x3ff+52 || f64_eq( a, { 0 } ))
    return af;
    /* y = int(x) - x, where int(x) is an integer neighbor of x */
    if (a.v >> 63)
    y = f64_sub( f64_add( f64_sub( a, float64_t{inv_double_eps} ), float64_t{inv_double_eps} ), a );
    else
    y = f64_sub( f64_sub( f64_add( a, float64_t{inv_double_eps} ), float64_t{inv_double_eps} ), a );
    /* special case because of non-nearest rounding modes */
    if (e <= 0x3ff-1) {
    return a.v >> 63 ? -0.0 : 1.0; //float64_t{0x8000000000000000} : float64_t{0xBE99999A3F800000}; //either -0.0 or 1
    }
    if (f64_lt( y, to_softfloat64(0) )) {
    ret = f64_add( f64_add( a, y ), to_softfloat64(1) ); // 0xBE99999A3F800000 } ); // plus 1
    return from_softfloat64(ret);
    }
    ret = f64_add( a, y );
    return from_softfloat64(ret);
}
static double _eosio_f64_floor( double af ) {
    float64_t a = to_softfloat64( af );
    float64_t ret;
    int e = a.v >> 52 & 0x7FF;
    float64_t y;
    double de = 1/DBL_EPSILON;
    if ( a.v == 0x8000000000000000) {
    return af;
    }
    if (e >= 0x3FF+52 || a.v == 0) {
    return af;
    }
    if (a.v >> 63)
    y = f64_sub( f64_add( f64_sub( a, float64_t{inv_double_eps} ), float64_t{inv_double_eps} ), a );
    else
    y = f64_sub( f64_sub( f64_add( a, float64_t{inv_double_eps} ), float64_t{inv_double_eps} ), a );
    if (e <= 0x3FF-1) {
    return a.v>>63 ? -1.0 : 0.0; //float64_t{0xBFF0000000000000} : float64_t{0}; // -1 or 0
    }
    if ( !f64_le( y, float64_t{0} ) ) {
    ret = f64_sub( f64_add(a,y), to_softfloat64(1.0));
    return from_softfloat64(ret);
    }
    ret = f64_add( a, y );
    return from_softfloat64(ret);
}
static double _eosio_f64_trunc( double af ) {
    float64_t a = to_softfloat64( af );
    int e = (int)(a.v >> 52 & 0x7ff) - 0x3ff + 12;
    uint64_t m;
    if (e >= 52 + 12)
    return af;
    if (e < 12)
    e = 1;
    m = -1ULL >> e;
    if ((a.v & m) == 0)
    return af;
    a.v &= ~m;
    return from_softfloat64(a);
}

static double _eosio_f64_nearest( double af ) {
    float64_t a = to_softfloat64( af );
    int e = (a.v >> 52 & 0x7FF);
    int s = a.v >> 63;
    float64_t y;
    if ( e >= 0x3FF+52 )
    return af;
    if ( s )
    y = f64_add( f64_sub( a, float64_t{inv_double_eps} ), float64_t{inv_double_eps} );
    else
    y = f64_sub( f64_add( a, float64_t{inv_double_eps} ), float64_t{inv_double_eps} );
    if ( f64_eq( y, float64_t{0} ) )
    return s ? -0.0 : 0.0;
    return from_softfloat64(y);
}

// double relops
static u32 _eosio_f64_eq( f64 a, f64 b ) { return f64_eq( to_softfloat64(a), to_softfloat64(b) ); }
static u32 _eosio_f64_ne( f64 a, f64 b ) { return !f64_eq( to_softfloat64(a), to_softfloat64(b) ); }
static u32 _eosio_f64_lt( f64 a, f64 b ) { return f64_lt( to_softfloat64(a), to_softfloat64(b) ); }
static u32 _eosio_f64_le( f64 a, f64 b ) { return f64_le( to_softfloat64(a), to_softfloat64(b) ); }
static u32 _eosio_f64_gt( f64 af, f64 bf ) {
    float64_t a = to_softfloat64(af);
    float64_t b = to_softfloat64(bf);
    if (is_nan(a))
    return false;
    if (is_nan(b))
    return false;
    return !f64_le( a, b );
}
static u32 _eosio_f64_ge( f64 af, f64 bf ) {
    float64_t a = to_softfloat64(af);
    float64_t b = to_softfloat64(bf);
    if (is_nan(a))
    return false;
    if (is_nan(b))
    return false;
    return !f64_lt( a, b );
}

// float and double conversions
static f64 _eosio_f32_promote( f32 a ) {
    return from_softfloat64(f32_to_f64( to_softfloat32(a)) );
}
static f32 _eosio_f64_demote( f64 a ) {
    return from_softfloat32(f64_to_f32( to_softfloat64(a)) );
}
static int32_t _eosio_f32_trunc_i32s( float af ) {
    float32_t a = to_softfloat32(af);
    if (_eosio_f32_ge(af, 2147483648.0f) || _eosio_f32_lt(af, -2147483648.0f))
    EOS_THROW(wasm_execution_error, "Error, f32.convert_s/i32 overflow" );
    if (is_nan(a))
    EOS_THROW(wasm_execution_error, "Error, f32.convert_s/i32 unrepresentable");
    return f32_to_i32( to_softfloat32(_eosio_f32_trunc( af )), 0, false );
}
static u32 _eosio_f64_trunc_i32s( f64 af ) {
    float64_t a = to_softfloat64(af);
    if (_eosio_f64_ge(af, 2147483648.0) || _eosio_f64_lt(af, -2147483648.0))
    EOS_THROW(wasm_execution_error, "Error, f64.convert_s/i32 overflow");
    if (is_nan(a))
    EOS_THROW(wasm_execution_error, "Error, f64.convert_s/i32 unrepresentable");
    return f64_to_i32( to_softfloat64(_eosio_f64_trunc( af )), 0, false );
}
static uint32_t _eosio_f32_trunc_i32u( float af ) {
    float32_t a = to_softfloat32(af);
    if (_eosio_f32_ge(af, 4294967296.0f) || _eosio_f32_le(af, -1.0f))
    EOS_THROW(wasm_execution_error, "Error, f32.convert_u/i32 overflow");
    if (is_nan(a))
    EOS_THROW(wasm_execution_error, "Error, f32.convert_u/i32 unrepresentable");
    return f32_to_ui32( to_softfloat32(_eosio_f32_trunc( af )), 0, false );
}
static u32 _eosio_f64_trunc_i32u( f64 af ) {
    float64_t a = to_softfloat64(af);
    if (_eosio_f64_ge(af, 4294967296.0) || _eosio_f64_le(af, -1.0))
    EOS_THROW(wasm_execution_error, "Error, f64.convert_u/i32 overflow");
    if (is_nan(a))
    EOS_THROW(wasm_execution_error, "Error, f64.convert_u/i32 unrepresentable");
    return f64_to_ui32( to_softfloat64(_eosio_f64_trunc( af )), 0, false );
}
static int64_t _eosio_f32_trunc_i64s( float af ) {
    float32_t a = to_softfloat32(af);
    if (_eosio_f32_ge(af, 9223372036854775808.0f) || _eosio_f32_lt(af, -9223372036854775808.0f))
    EOS_THROW(wasm_execution_error, "Error, f32.convert_s/i64 overflow");
    if (is_nan(a))
    EOS_THROW(wasm_execution_error, "Error, f32.convert_s/i64 unrepresentable");
    return f32_to_i64( to_softfloat32(_eosio_f32_trunc( af )), 0, false );
}
static u64 _eosio_f64_trunc_i64s( f64 af ) {
    float64_t a = to_softfloat64(af);
    if (_eosio_f64_ge(af, 9223372036854775808.0) || _eosio_f64_lt(af, -9223372036854775808.0))
    EOS_THROW(wasm_execution_error, "Error, f64.convert_s/i64 overflow");
    if (is_nan(a))
    EOS_THROW(wasm_execution_error, "Error, f64.convert_s/i64 unrepresentable");

    return f64_to_i64( to_softfloat64(_eosio_f64_trunc( af )), 0, false );
}
static uint64_t _eosio_f32_trunc_i64u( float af ) {
    float32_t a = to_softfloat32(af);
    if (_eosio_f32_ge(af, 18446744073709551616.0f) || _eosio_f32_le(af, -1.0f))
    EOS_THROW(wasm_execution_error, "Error, f32.convert_u/i64 overflow");
    if (is_nan(a))
    EOS_THROW(wasm_execution_error, "Error, f32.convert_u/i64 unrepresentable");
    return f32_to_ui64( to_softfloat32(_eosio_f32_trunc( af )), 0, false );
}
static uint64_t _eosio_f64_trunc_i64u( double af ) {
    float64_t a = to_softfloat64(af);
    if (_eosio_f64_ge(af, 18446744073709551616.0) || _eosio_f64_le(af, -1.0))
    EOS_THROW(wasm_execution_error, "Error, f64.convert_u/i64 overflow");
    if (is_nan(a))
    EOS_THROW(wasm_execution_error, "Error, f64.convert_u/i64 unrepresentable");
    return f64_to_ui64( to_softfloat64(_eosio_f64_trunc( af )), 0, false );
}
static float _eosio_i32_to_f32( int32_t a )  {
    return from_softfloat32(i32_to_f32( a ));
}
static float _eosio_i64_to_f32( int64_t a ) {
    return from_softfloat32(i64_to_f32( a ));
}
static float _eosio_ui32_to_f32( uint32_t a ) {
    return from_softfloat32(ui32_to_f32( a ));
}
static float _eosio_ui64_to_f32( uint64_t a ) {
    return from_softfloat32(ui64_to_f32( a ));
}
static double _eosio_i32_to_f64( int32_t a ) {
    return from_softfloat64(i32_to_f64( a ));
}
static double _eosio_i64_to_f64( int64_t a ) {
    return from_softfloat64(i64_to_f64( a ));
}
static double _eosio_ui32_to_f64( uint32_t a ) {
    return from_softfloat64(ui32_to_f64( a ));
}
static double _eosio_ui64_to_f64( uint64_t a ) {
    return from_softfloat64(ui64_to_f64( a ));
}

static int __unordtf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
    float128_t a = {{ la, ha }};
    float128_t b = {{ lb, hb }};
    if ( is_nan(a) || is_nan(b) ) {
        return 1;
    }
    return 0;
}

static int ___cmptf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb, int return_value_if_nan ) {
    float128_t a = {{ la, ha }};
    float128_t b = {{ lb, hb }};
    if ( __unordtf2(la, ha, lb, hb) ) {
        return return_value_if_nan;
    }

    if ( f128_lt( a, b ) ) {
        return -1;
    }

    if ( f128_eq( a, b ) ) {
        return 0;
    }
    return 1;
}

static u32 __gttf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
    return ___cmptf2(la, ha, lb, hb, 0);
}

static u32 __getf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
    return ___cmptf2(la, ha, lb, hb, -1);
}

static u32 __letf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
    return ___cmptf2(la, ha, lb, hb, 1);
}

static double __trunctfdf2( uint64_t l, uint64_t h ) {
    float128_t f = {{ l, h }};
    return from_softfloat64(f128_to_f64( f ));
}

static u32 __lttf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
    return ___cmptf2(la, ha, lb, hb, 0);
}

float __eosio_f32_mul( float a, float b ) {
    float32_t ret = ::f32_mul( to_softfloat32(a), to_softfloat32(b) );
    return *reinterpret_cast<float*>(&ret);
}

void init_eosio_injection()
{
    Z_eosio_injectionZ__eosio_f32_mulZ_fff = __eosio_f32_mul;

    Z_envZ___gttf2Z_ijjjj = __gttf2;
    Z_envZ___getf2Z_ijjjj = __getf2;
    Z_envZ___letf2Z_ijjjj = __letf2;
    Z_envZ___trunctfdf2Z_djj = __trunctfdf2;
    Z_envZ___lttf2Z_ijjjj = __lttf2;


    Z_eosio_injectionZ_checktimeZ_vv = vm_checktime ;
    Z_eosio_injectionZ_call_depth_assertZ_vv = call_depth_assert ;
    Z_eosio_injectionZ__eosio_f64_trunc_i64sZ_jd = _eosio_f64_trunc_i64s ;
    Z_eosio_injectionZ__eosio_f64_gtZ_idd = _eosio_f64_gt ;
    Z_eosio_injectionZ__eosio_f64_trunc_i32sZ_id = _eosio_f64_trunc_i32s ;
    Z_eosio_injectionZ__eosio_f64_neZ_idd = _eosio_f64_ne ;
    Z_eosio_injectionZ__eosio_f64_divZ_ddd = _eosio_f64_div ;
    Z_eosio_injectionZ__eosio_f64_mulZ_ddd = _eosio_f64_mul ;
    Z_eosio_injectionZ__eosio_f64_addZ_ddd = _eosio_f64_add ;
    Z_eosio_injectionZ__eosio_f64_leZ_idd = _eosio_f64_le ;
    Z_eosio_injectionZ__eosio_f64_eqZ_idd = _eosio_f64_eq ;

    Z_eosio_injectionZ__eosio_f64_subZ_ddd = _eosio_f64_sub ;
    Z_eosio_injectionZ__eosio_f64_ltZ_idd = _eosio_f64_lt ;
    Z_eosio_injectionZ__eosio_f64_negZ_dd = _eosio_f64_neg ;
    Z_eosio_injectionZ__eosio_f64_geZ_idd = _eosio_f64_ge ;
    Z_eosio_injectionZ__eosio_f64_trunc_i32uZ_id = _eosio_f64_trunc_i32u ;
    Z_eosio_injectionZ__eosio_f64_demoteZ_fd = _eosio_f64_demote ;
    Z_eosio_injectionZ__eosio_f32_promoteZ_df = _eosio_f32_promote ;
    Z_eosio_injectionZ__eosio_f32_eqZ_iff = _eosio_f32_eq ;
    Z_eosio_injectionZ__eosio_f32_divZ_fff = _eosio_f32_div ;
    Z_eosio_injectionZ__eosio_f32_leZ_iff = _eosio_f32_le ;
    Z_eosio_injectionZ__eosio_f32_neZ_iff = _eosio_f32_ne ;

    Z_eosio_injectionZ__eosio_f64_maxZ_ddd = _eosio_f64_max;

    Z_eosio_injectionZ__eosio_f64_absZ_dd = _eosio_f64_abs;
    Z_eosio_injectionZ__eosio_f64_truncZ_dd = _eosio_f64_trunc;

    Z_eosio_injectionZ__eosio_f32_gtZ_iff = _eosio_f32_gt;
}


}
