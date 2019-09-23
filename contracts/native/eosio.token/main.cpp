typedef unsigned __int128 uint128_t;

#include <stdint.h>
#include <eosio/db.h>
#include <eosio/multi_index.hpp>

extern "C" void apply(uint64_t receiver, uint64_t first_receiver, uint64_t action);
void native_apply(uint64_t receiver, uint64_t first_receiver, uint64_t action) {
    apply(receiver, first_receiver, action);
}

