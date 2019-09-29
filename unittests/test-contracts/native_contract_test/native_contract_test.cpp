/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#include "native_contract_test.hpp"
#include <eosio/print.hpp>

using namespace eosio;

static int recursive_depth = 0;
void recursive_test() {
   eosio::print("hello,world\n");
   recursive_depth += 1;
   if (recursive_depth >= 100) {
      return;   
   }
   recursive_test();
}

void native_contract_test::anyaction( name test )
{
   if (test == "staticvar"_n) {
      static int n = 0;
      eosio::print("hello,world\n");
      n += 1;
      eosio::print(n, "\n");
      eosio::check(n == 1, "bad value");
   } else if (test == "checktime"_n) {
//      while(true);
   } else if (test == "memtest1"_n) {
      size_t current_page = __builtin_wasm_current_memory();
      check(__builtin_wasm_grow_memory(50) == current_page, "fail to grow");
   } else if (test == "memtest2"_n) {
      size_t current_page = __builtin_wasm_current_memory();
      check(__builtin_wasm_grow_memory(528) == current_page, "fail to grow");
   } else if (test == "recursive"_n) {
      recursive_test();
   }

}
