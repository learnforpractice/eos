#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include <fc/uint128.hpp>
#include <compiler_builtins.hpp>
#include "vm_api_proxy.hpp"

void vm_api_proxy::__ashlti3(__int128* ret, uint64_t low, uint64_t high, uint32_t shift) {
      fc::uint128 i(high, low);
      i <<= shift;
      *ret = (unsigned __int128)i;
}

void vm_api_proxy::__ashrti3(__int128* ret, uint64_t low, uint64_t high, uint32_t shift) {
   // retain the signedness
   *ret = high;
   *ret <<= 64;
   *ret |= low;
   *ret >>= shift;
}

void vm_api_proxy::__lshlti3(__int128* ret, uint64_t low, uint64_t high, uint32_t shift) {
   fc::uint128 i(high, low);
   i <<= shift;
   *ret = (unsigned __int128)i;
}

void vm_api_proxy::__lshrti3(__int128* ret, uint64_t low, uint64_t high, uint32_t shift) {
   fc::uint128 i(high, low);
   i >>= shift;
   *ret = (unsigned __int128)i;
}

void vm_api_proxy::__divti3(__int128* ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb) {
   __int128 lhs = ha;
   __int128 rhs = hb;

   lhs <<= 64;
   lhs |=  la;

   rhs <<= 64;
   rhs |=  lb;

   EOS_ASSERT(rhs != 0, arithmetic_exception, "divide by zero");

   lhs /= rhs;

   *ret = lhs;
}

void vm_api_proxy::__udivti3(unsigned __int128 *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb) {
   unsigned __int128 lhs = ha;
   unsigned __int128 rhs = hb;

   lhs <<= 64;
   lhs |=  la;

   rhs <<= 64;
   rhs |=  lb;

   EOS_ASSERT(rhs != 0, arithmetic_exception, "divide by zero");

   lhs /= rhs;
   *ret = lhs;
}

void vm_api_proxy::__multi3(__int128* ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb) {
   __int128 lhs = ha;
   __int128 rhs = hb;

   lhs <<= 64;
   lhs |=  la;

   rhs <<= 64;
   rhs |=  lb;

   lhs *= rhs;
   *ret = lhs;
}

void vm_api_proxy::__modti3(__int128* ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb) {
   __int128 lhs = ha;
   __int128 rhs = hb;

   lhs <<= 64;
   lhs |=  la;

   rhs <<= 64;
   rhs |=  lb;

   EOS_ASSERT(rhs != 0, arithmetic_exception, "divide by zero");

   lhs %= rhs;
   *ret = lhs;
}

void vm_api_proxy::__umodti3(unsigned __int128 *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb) {
   unsigned __int128 lhs = ha;
   unsigned __int128 rhs = hb;

   lhs <<= 64;
   lhs |=  la;

   rhs <<= 64;
   rhs |=  lb;

   EOS_ASSERT(rhs != 0, arithmetic_exception, "divide by zero");

   lhs %= rhs;
   *ret = lhs;
}

// arithmetic long double
void vm_api_proxy::__addtf3( float128_t *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   float128_t a = {{ la, ha }};
   float128_t b = {{ lb, hb }};
   *ret = f128_add( a, b );
}

void vm_api_proxy::__subtf3( float128_t *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   float128_t a = {{ la, ha }};
   float128_t b = {{ lb, hb }};
   *ret = f128_sub( a, b );
}

void vm_api_proxy::__multf3( float128_t *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   float128_t a = {{ la, ha }};
   float128_t b = {{ lb, hb }};
   *ret = f128_mul( a, b );
}

void vm_api_proxy::__divtf3( float128_t *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   float128_t a = {{ la, ha }};
   float128_t b = {{ lb, hb }};
   *ret = f128_div( a, b );
}

void vm_api_proxy::__negtf2( float128_t *ret, uint64_t la, uint64_t ha ) {
   *ret = {{ la, (ha ^ (uint64_t)1 << 63) }};
}

// conversion long double
void vm_api_proxy::__extendsftf2( float128_t *ret, float f ) {
   *ret = f32_to_f128( to_softfloat32(f) );
}

void vm_api_proxy::__extenddftf2( float128_t *ret, double d ) {
   *ret = f64_to_f128( to_softfloat64(d) );
}

double vm_api_proxy::__trunctfdf2( uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   return from_softfloat64(f128_to_f64( f ));
}

float vm_api_proxy::__trunctfsf2( uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   return from_softfloat32(f128_to_f32( f ));
}

int32_t vm_api_proxy::__fixtfsi( uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   return f128_to_i32( f, 0, false );
}

int64_t vm_api_proxy::__fixtfdi( uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   return f128_to_i64( f, 0, false );
}

void vm_api_proxy::__fixtfti( __int128* ret, uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   *ret = ___fixtfti( f );
}

uint32_t vm_api_proxy::__fixunstfsi( uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   return f128_to_ui32( f, 0, false );
}

uint64_t vm_api_proxy::__fixunstfdi( uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   return f128_to_ui64( f, 0, false );
}

void vm_api_proxy::__fixunstfti( unsigned __int128 *ret, uint64_t l, uint64_t h ) {
   float128_t f = {{ l, h }};
   *ret = ___fixunstfti( f );
}

void vm_api_proxy::__fixsfti( __int128* ret, float a ) {
   *ret = ___fixsfti( to_softfloat32(a).v );
}

void vm_api_proxy::__fixdfti( __int128* ret, double a ) {
   *ret = ___fixdfti( to_softfloat64(a).v );
}

void vm_api_proxy::__fixunssfti( unsigned __int128 *ret, float a ) {
   *ret = ___fixunssfti( to_softfloat32(a).v );
}

void vm_api_proxy::__fixunsdfti( unsigned __int128 *ret, double a ) {
   *ret = ___fixunsdfti( to_softfloat64(a).v );
}

double vm_api_proxy::__floatsidf( int32_t i ) {
   return from_softfloat64(i32_to_f64(i));
}

void vm_api_proxy::__floatsitf( float128_t *ret, int32_t i ) {
   *ret = i32_to_f128(i);
}

void vm_api_proxy::__floatditf( float128_t *ret, uint64_t a ) {
   *ret = i64_to_f128( a );
}

void vm_api_proxy::__floatunsitf( float128_t *ret, uint32_t i ) {
   *ret = ui32_to_f128(i);
}

void vm_api_proxy::__floatunditf( float128_t *ret, uint64_t a ) {
   *ret = ui64_to_f128( a );
}

double vm_api_proxy::__floattidf( uint64_t l, uint64_t h ) {
   fc::uint128 v(h, l);
   unsigned __int128 val = (unsigned __int128)v;
   return ___floattidf( *(__int128*)&val );
}

double vm_api_proxy::__floatuntidf( uint64_t l, uint64_t h ) {
   fc::uint128 v(h, l);
   return ___floatuntidf( (unsigned __int128)v );
}

inline static int cmptf2_impl( vm_api_proxy& i, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb, int return_value_if_nan ) {
   float128_t a = {{ la, ha }};
   float128_t b = {{ lb, hb }};
   if ( i.__unordtf2(la, ha, lb, hb) )
      return return_value_if_nan;
   if ( f128_lt( a, b ) )
      return -1;
   if ( f128_eq( a, b ) )
      return 0;
   return 1;
}

int vm_api_proxy::__eqtf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   return cmptf2_impl(*this, la, ha, lb, hb, 1);
}

int vm_api_proxy::__netf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   return cmptf2_impl(*this, la, ha, lb, hb, 1);
}

int vm_api_proxy::__getf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   return cmptf2_impl(*this, la, ha, lb, hb, -1);
}

int vm_api_proxy::__gttf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   return cmptf2_impl(*this, la, ha, lb, hb, 0);
}

int vm_api_proxy::__letf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   return cmptf2_impl(*this, la, ha, lb, hb, 1);
}

int vm_api_proxy::__lttf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   return cmptf2_impl(*this, la, ha, lb, hb, 0);
}

int vm_api_proxy::__cmptf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   return cmptf2_impl(*this, la, ha, lb, hb, 1);
}

int vm_api_proxy::__unordtf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb ) {
   float128_t a = {{ la, ha }};
   float128_t b = {{ lb, hb }};
   if ( f128_is_nan(a) || f128_is_nan(b) )
      return 1;
   return 0;
}
