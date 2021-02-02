#include <vm_api_proxy.hpp>
extern "C" {
// Kept as intrinsic rather than implementing on WASM side (using prints_l and strlen) because strlen is faster on native side.
// TODO predicate these for ignore
void prints(const char *str) {
    get_vm_api()->prints(str);
}

void prints_l(const char *str, size_t str_size) {
    get_vm_api()->prints_l(str, str_size);
}

void printi(int64_t val) {
    get_vm_api()->printi(val);
}

void printui(uint64_t val) {
    get_vm_api()->printui(val);
}

void printi128(const __int128 *val) {
    get_vm_api()->printi128(val);
}

void printui128(const unsigned __int128 *val) {
    get_vm_api()->printui128(val);
}

void printsf( float32_t val ) {
    get_vm_api()->printsf(val);
}

void printdf( float64_t val ) {
    get_vm_api()->printdf(val);
}

void printqf( const float128_t *val ) {
    get_vm_api()->printqf(val);
}

void printn(uint64_t value) {
    get_vm_api()->printn(value);
}

void printhex( const char *data, size_t data_size ) {
    get_vm_api()->printhex(data, data_size);
}

}