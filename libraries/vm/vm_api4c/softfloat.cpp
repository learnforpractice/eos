
#include "vm_api4c.h"
#include <string.h>
#include <softfloat.hpp>
#include <stdio.h>

extern "C" {
    /* import: 'env' '__addtf3' */
    void (*Z_envZ___addtf3Z_vijjjj)(u32, u64, u64, u64, u64);
    /* import: 'env' '__eqtf2' */
    u32 (*Z_envZ___eqtf2Z_ijjjj)(u64, u64, u64, u64);
    /* import: 'env' '__extenddftf2' */
    void (*Z_envZ___extenddftf2Z_vid)(u32, f64);
    /* import: 'env' '__fixtfsi' */
    u32 (*Z_envZ___fixtfsiZ_ijj)(u64, u64);
    /* import: 'env' '__fixunstfsi' */
    u32 (*Z_envZ___fixunstfsiZ_ijj)(u64, u64);
    /* import: 'env' '__floatsitf' */
    void (*Z_envZ___floatsitfZ_vii)(u32, u32);
    /* import: 'env' '__floatunsitf' */
    void (*Z_envZ___floatunsitfZ_vii)(u32, u32);
    /* import: 'env' '__multf3' */
    void (*Z_envZ___multf3Z_vijjjj)(u32, u64, u64, u64, u64);
    /* import: 'env' '__netf2' */
    u32 (*Z_envZ___netf2Z_ijjjj)(u64, u64, u64, u64);
    /* import: 'env' '__subtf3' */
    void (*Z_envZ___subtf3Z_vijjjj)(u32, u64, u64, u64, u64);
    /* import: 'env' '__unordtf2' */
    u32 (*Z_envZ___unordtf2Z_ijjjj)(u64, u64, u64, u64);

    /* import: 'env' '__divtf3' */
    void (*Z_envZ___divtf3Z_vijjjj)(u32, u64, u64, u64, u64);

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

static u32 __unordtf2( u64 la, u64 ha, u64 lb, u64 hb ) {
    float128_t a = {{ la, ha }};
    float128_t b = {{ lb, hb }};
    if ( is_nan(a) || is_nan(b) )
    return 1;
    return 0;
}

static int ___cmptf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb, int return_value_if_nan ) {
    float128_t a = {{ la, ha }};
    float128_t b = {{ lb, hb }};
    if ( __unordtf2(la, ha, lb, hb) )
    return return_value_if_nan;
    if ( f128_lt( a, b ) )
    return -1;
    if ( f128_eq( a, b ) )
    return 0;
    return 1;
}

static void __addtf3( u32 ret_offset, u64 la, u64 ha, u64 lb, u64 hb ) {
    float128_t *ret = (float128_t *)offset_to_ptr(ret_offset, sizeof(float128_t));
    float128_t a = {{ la, ha }};
    float128_t b = {{ lb, hb }};
    *ret = f128_add( a, b );
}

static u32 __eqtf2( u64 la, u64 ha, u64 lb, u64 hb ) {
    return ___cmptf2(la, ha, lb, hb, 1);
}

static void __extenddftf2( u32 ret_offset, f64 d ) {
    float128_t *ret = (float128_t *)offset_to_ptr(ret_offset, sizeof(float128_t));
    *ret = f64_to_f128( to_softfloat64(d) );
}

static u32 __fixtfsi( u64 l, u64 h ) {
    float128_t f = {{ l, h }};
    return f128_to_i32( f, 0, false );
}

static u32 __fixunstfsi( u64 l, u64 h ) {
    float128_t f = {{ l, h }};
    return f128_to_ui32( f, 0, false );
}

static void __floatsitf( u32 ret_offset, u32 i ) {
    float128_t *ret = (float128_t *)offset_to_ptr(ret_offset, sizeof(float128_t));
    *ret = i32_to_f128(i);
}

static void __floatunsitf( u32 ret_offset, uint32_t i ) {
    float128_t *ret = (float128_t *)offset_to_ptr(ret_offset, sizeof(float128_t));
    *ret = ui32_to_f128(i);
}

static void __multf3( u32 ret_offset, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
    float128_t *ret = (float128_t *)offset_to_ptr(ret_offset, sizeof(float128_t));
    float128_t a = {{ la, ha }};
    float128_t b = {{ lb, hb }};
    *ret = f128_mul( a, b );
}

static u32 __netf2( u64 la, u64 ha, u64 lb, u64 hb ) {
    return ___cmptf2(la, ha, lb, hb, 1);
}

static void __subtf3( u32 ret_offset, u64 la, u64 ha, u64 lb, u64 hb ) {
    float128_t *ret = (float128_t *)offset_to_ptr(ret_offset, sizeof(float128_t));
    float128_t a = {{ la, ha }};
    float128_t b = {{ lb, hb }};
    *ret = f128_sub( a, b );
}

/* import: 'env' '__divtf3' */
extern void __divtf3(u32 ret_offset, u64 la, u64 ha, u64 lb, u64 hb) {
    float128_t *ret = (float128_t *)offset_to_ptr(ret_offset, sizeof(float128_t));
    float128_t a = {{ la, ha }};
    float128_t b = {{ lb, hb }};
    *ret = f128_div( a, b );
}

extern "C" void init_softfloat() {
    Z_envZ___addtf3Z_vijjjj = __addtf3;
    Z_envZ___eqtf2Z_ijjjj = __eqtf2;
    Z_envZ___extenddftf2Z_vid = __extenddftf2;
    Z_envZ___fixtfsiZ_ijj = __fixtfsi;
    Z_envZ___fixunstfsiZ_ijj = __fixunstfsi;
    Z_envZ___floatsitfZ_vii = __floatsitf;
    Z_envZ___floatunsitfZ_vii = __floatunsitf;
    Z_envZ___multf3Z_vijjjj = __multf3;
    Z_envZ___netf2Z_ijjjj = __netf2;
    Z_envZ___subtf3Z_vijjjj = __subtf3;
    Z_envZ___unordtf2Z_ijjjj = __unordtf2;
    Z_envZ___divtf3Z_vijjjj = __divtf3;
}

