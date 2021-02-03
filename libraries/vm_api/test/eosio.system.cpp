#include <stdint.h>

extern "C" void eosio_system_apply(uint64_t a, uint64_t b, uint64_t c);

extern "C" void native_apply(uint64_t a, uint64_t b, uint64_t c) {
    eosio_system_apply(a, b, c);
}

extern "C" int main(int argc, char **argv) {
    return 0;
}
