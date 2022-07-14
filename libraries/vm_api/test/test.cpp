#include <stdint.h>

extern "C" void apply(uint64_t a, uint64_t b, uint64_t c);

extern "C" int native_apply(uint64_t a, uint64_t b, uint64_t c) {
    apply(a, b, c);
    return 1;
}
