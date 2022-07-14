#include <stdint.h>

extern "C" void eosio_token_apply(uint64_t a, uint64_t b, uint64_t c);

extern "C" int native_apply(uint64_t a, uint64_t b, uint64_t c) {
    eosio_token_apply(a, b, c);
    return 1;
}

