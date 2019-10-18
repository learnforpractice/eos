/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#include "mi.hpp"
#include <eosio/print.hpp>
using namespace eosio;

__attribute__((eosio_wasm_import))
extern "C" int evm_execute(const unsigned char* code, size_t size);

class A {
   [[eosio::action]]
   void init( int version, int core ) {

   }
};

extern "C" void apply(uint64_t receiver, uint64_t code, uint64_t action) {
//test store
{
   uint64_t payer = receiver;
   vector<index_type> index_types = {idx64};
   uint64_t scope = "scope"_n.value;
   uint64_t table = "table"_n.value;
   multi_index mi(code, scope, table, index_types);

   vm_buffer secondary_values[1];
   uint64_t primary_key = "hello"_n.value;
   uint64_t secondary_key = "world"_n.value;

   secondary_values[0].size = 8;
   secondary_values[0].data = (char *)&secondary_key;
   mi.store(primary_key, (void *)"hello", 5, secondary_values, 1, payer);

   uint64_t primary_key2;
   int itr = mi.idx_find(0, primary_key2, &secondary_key, sizeof(secondary_key));
   check(primary_key == primary_key2, "bad secondary");
}

//test modify
{
   uint64_t payer = receiver;
   vector<index_type> index_types = {idx64};
   uint64_t scope = "scope"_n.value;
   uint64_t table = "table"_n.value;
   multi_index mi(code, scope, table, index_types);

   vm_buffer secondary_values[1];
   uint64_t primary_key = "hello"_n.value;
   uint64_t secondary_key = "earth"_n.value;

   secondary_values[0].size = 8;
   secondary_values[0].data = (char *)&secondary_key;

   int itr = mi.find(primary_key);
   mi.modify(itr, primary_key, (void *)"goodbye", 7, secondary_values, 1, payer);

   vm_buffer vb;
   int data_size = mi.get(itr, &vb);
   check(memcmp(vb.data, "goodbye", 7) == 0, "bad data");

   uint64_t primary_key2;
   itr = mi.idx_find(0, primary_key2, &secondary_key, sizeof(secondary_key));
   check(primary_key == primary_key2, "bad secondary");
}

{
   uint64_t payer = receiver;
   vector<index_type> index_types = {idx64, idx128, idx256, idx_double, idx_long_double};
   uint64_t scope = "scope"_n.value;
   uint64_t table = "table"_n.value;
   multi_index mi(code, scope, table, index_types);

   uint64_t primary_key = "goodbye"_n.value;
   uint64_t secondary_key = "world"_n.value;

   
   vm_buffer secondary_values[5];

//   value64 = 64;
   secondary_values[0].size = 8;
   secondary_values[0].data = (char *)&secondary_key;


   uint128_t value128 = 128;
   secondary_values[1].size = 16;
   secondary_values[1].data = (char *)&value128;

   uint128_t value256[2];
   secondary_values[2].size = 32;
   secondary_values[2].data = (char *)&value256;   

   double valuedouble = 10.0;
   secondary_values[3].size = 8;
   secondary_values[3].data = (char *)&valuedouble;

   long double valuelongdouble = 100.0;
   secondary_values[3].size = 16;
   secondary_values[3].data = (char *)&valuelongdouble;


   mi.store(primary_key, (void *)"world", 5, secondary_values, 5, payer);

   uint64_t primary_key2;
   int itr = mi.idx_find(0, primary_key2, &secondary_key, sizeof(secondary_key));
   check(primary_key == primary_key2, "bad secondary");
}


}

