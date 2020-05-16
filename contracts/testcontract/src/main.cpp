#include <stdint.h>
#include <eosio/print.hpp>

extern "C" {

void call(uint64_t receiver, uint64_t code, uint64_t action) {
    eosio::print("hello,world");
}

void apply(uint64_t receiver, uint64_t code, uint64_t action) {
    eosio::print("hello,world");
    call(receiver, code, action);
}

}
