#include <eosio/eosio.hpp>

extern "C" void apply(uint64_t receiver, uint64_t first_receiver, uint64_t action) {
    eosio::print("hello,world");
}
