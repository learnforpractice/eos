#include "test_uuos.hpp"
__attribute__((eosio_wasm_import))
extern "C" void set_action_return_value(const unsigned char* code, uint32_t size);

using namespace eosio;

extern "C" void apply(uint64_t receiver, uint64_t code, uint64_t action) {
   for (int i=0;i<10;i++) {
      eosio::print("hello,world", "\n");
   }
   set_action_return_value((unsigned char*)"abcdefgh", 8);
}
