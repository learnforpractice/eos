
#include <stdlib.h>
#include <stdint.h>
#include <stdexcept>

#include "vm_api.h"
#include "stacktrace.h"
static struct vm_api* s_api = nullptr;

void vm_register_api(struct vm_api* api) {
   if (!api) {
      throw std::runtime_error("vm_api pointer can not be NULL!");
   }
   s_api = api;
}

struct vm_api* get_vm_api() {
   if (!s_api) {
//      print_stacktrace();
      throw std::runtime_error("vm api not specified!!!");
   }
   return s_api;
}

static constexpr uint64_t char_to_symbol( char c ) {
   if( c >= 'a' && c <= 'z' )
      return (c - 'a') + 6;
   if( c >= '1' && c <= '5' )
      return (c - '1') + 1;
   return 0;
}


extern "C" uint64_t s2n( const char *str, size_t str_size ) {
   uint64_t n = 0;
   int i = 0;

   for ( ; str[i] && i < str_size && i < 12; ++i) {
      // NOTE: char_to_symbol() returns char type, and without this explicit
      // expansion to uint64 type, the compilation fails at the point of usage
      // of string_to_name(), where the usage requires constant (compile time) expression.
      uint64_t s = char_to_symbol(str[i]);
      if (s == 0) {
         return 0;
      }
      n |= (s & 0x1f) << (64 - 5 * (i + 1));
   }

   // The for-loop encoded up to 60 high bits into uint64 'name' variable,
   // if (strlen(str) > 12) then encode str[12] into the low (remaining)
   // 4 bits of 'name'
   if (i == 12)
      n |= char_to_symbol(str[12]) & 0x0F;
   return n;
}

extern "C" int n2s(uint64_t value, char* begin, size_t str_size ) {
   static const char* charmap = ".12345abcdefghijklmnopqrstuvwxyz";
   constexpr uint64_t mask = 0xF800000000000000ull;

   auto v = value;
   int n = 0;
   for( auto i = 0; i < str_size && i < 13; ++i, v <<= 5 ) {
      if( v == 0 ) return n;

      auto indx = (v & mask) >> (i == 12 ? 60 : 59);
      *begin = charmap[indx];
      ++begin;
      n += 1;
   }

   return n;
}
