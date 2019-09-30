/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#include "native_contract_test.hpp"
#include <eosio/print.hpp>

using namespace eosio;

int add(int n) {
   return n+1;
}

static int recursive_depth = 0;

int recursive_test1();
int recursive_test2();

int recursive_test1() {
//   eosio::print("hello,world", recursive_depth, "\n");
   recursive_depth = add(recursive_depth);
   if (false) {//recursive_depth == 0) {
      return recursive_depth;
   }
   return recursive_test2();
}

int recursive_test2() {
//   eosio::print("hello,world", recursive_depth, "\n");
   recursive_depth = add(recursive_depth);
   if (false) {//recursive_depth == 0) {
      return recursive_depth;
   }
   return recursive_test1();
}


void native_contract_test::anyaction( name test )
{
   #if 1
   if (test == "staticvar"_n) {
      static int n = 0;
      eosio::print("hello,world\n");
      n += 1;
      eosio::print(n, "\n");
      eosio::check(n == 1, "bad value");
   } else if (test == "checktime"_n) {
      while(true);
   } else if (test == "memtest1"_n) {
      size_t current_page = __builtin_wasm_current_memory();
      check(__builtin_wasm_grow_memory(50) == current_page, "fail to grow");
   } else if (test == "memtest2"_n) {
      size_t current_page = __builtin_wasm_current_memory();
      check(__builtin_wasm_grow_memory(528) == current_page, "fail to grow");
   } else 
   #endif
   if (test == "recursive"_n) {
      eosio::print("hello,world ", recursive_depth, "\n");
      int n = recursive_test1();
      eosio::print("hello,world ", n, "\n");
      eosio::check(false, "bad bad bad!");
   }

}
