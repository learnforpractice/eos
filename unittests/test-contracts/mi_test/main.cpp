/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#include "mi.hpp"

using namespace eosio;

__attribute__((eosio_wasm_import))
extern "C" int evm_execute(const unsigned char* code, size_t size);

extern "C" void apply(uint64_t receiver, uint64_t code, uint64_t action) {
   vector<index_type> index_types = {idx64};
   uint64_t scope = "scope"_n.value;
   uint64_t table = "table"_n.value;
   multi_index mi(code, scope, table, index_types);
}
