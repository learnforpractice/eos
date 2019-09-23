#include <stdint.h>
extern "C" void eosio_system_apply(uint64_t receiver, uint64_t first_receiver, uint64_t action);

void native_apply(uint64_t receiver, uint64_t first_receiver, uint64_t action) {
    eosio_system_apply(receiver, first_receiver, action);
}
