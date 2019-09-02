#include <stdint.h>

extern "C" void apply( uint64_t receiver, uint64_t code, uint64_t action );
int main() {
    apply(0, 0, 0);
}
