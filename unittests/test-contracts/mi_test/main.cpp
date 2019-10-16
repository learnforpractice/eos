/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#include "mi.hpp"

using namespace eosio;

__attribute__((eosio_wasm_import))
extern "C" int evm_execute(const unsigned char* code, size_t size);

class A {
   [[eosio::action]]
   void init( int version, int core ) {

   }
};

extern "C" void apply(uint64_t receiver, uint64_t code, uint64_t action) {
   uint64_t payer = receiver;
   vector<index_type> index_types = {idx64};
   uint64_t scope = "scope"_n.value;
   uint64_t table = "table"_n.value;
   multi_index mi(code, scope, table, index_types);

//test store
{
   vector<vector<char>> secondary_values;
   uint64_t primary_key = "hello"_n.value;
   uint64_t secondary_key = "world"_n.value;


   vector<char> secondary_value((char*)&secondary_key, (char*)&secondary_key + 8);
   secondary_values.push_back(secondary_value);
   mi.store(primary_key, (void *)"hello", 5, secondary_values, payer);

   uint64_t primary_key2;
   int itr = mi.idx_find(0, primary_key2, &secondary_key, sizeof(secondary_key));
   check(primary_key == primary_key2, "bad secondary");
}

//test modify
{
   vector<vector<char>> secondary_values;
   uint64_t primary_key = "hello"_n.value;
   uint64_t secondary_key = "earth"_n.value;

   vector<char> secondary_value((char*)&secondary_key, (char*)&secondary_key + 8);
   secondary_values.push_back(secondary_value);
   int itr = mi.find(primary_key);
   mi.modify(itr, primary_key, (void *)"goodbye", 7, secondary_values, payer);

   char buffer[256];
   int data_size = mi.get(itr, buffer, 256);
   check(memcmp(buffer, "goodbye", 7) == 0, "bad data");

   uint64_t primary_key2;
   itr = mi.idx_find(0, primary_key2, &secondary_key, sizeof(secondary_key));
   check(primary_key == primary_key2, "bad secondary");
}



}

