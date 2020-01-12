#include "test_uuos.hpp"
__attribute__((eosio_wasm_import))
extern "C" void set_action_return_value(const unsigned char* code, uint32_t size);

using namespace eosio;

extern "C" void apply(uint64_t receiver, uint64_t code, uint64_t action) {
   for (int i=0;i<10;i++) {
      eosio::print("hello,world", "\n");
   }
   double a = 0.0;
   for (int i=0;i<1000;i++) {
      a *= double(receiver);
   }
   set_action_return_value((unsigned char*)&a, 8);
}
