#include <stdint.h>
extern "C" void apply(uint64_t receiver, uint64_t first_receiver, uint64_t action);

void native_apply(uint64_t receiver, uint64_t first_receiver, uint64_t action) {
    apply(receiver, first_receiver, action);
}
