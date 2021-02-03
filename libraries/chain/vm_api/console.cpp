#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include <softfloat.hpp>

#include "vm_api_proxy.hpp"

// Kept as intrinsic rather than implementing on WASM side (using prints_l and strlen) because strlen is faster on native side.
// TODO predicate these for ignore
void vm_api_proxy::prints(const char *str) {
    null_terminated_ptr _str(str);
    _interface->prints(_str);
}

void vm_api_proxy::prints_l(const char *str, uint32_t str_size) {
    legacy_span<const char> _str((void *)str, str_size);
    _interface->prints_l(std::move(_str));
}

void vm_api_proxy::printi(int64_t val) {
    _interface->printi(val);
}

void vm_api_proxy::printui(uint64_t val) {
    _interface->printui(val);
}

void vm_api_proxy::printi128(const __int128 *val) {
    legacy_ptr<const __int128> _val((void *)val);
    _interface->printi128(std::move(_val));
}

void vm_api_proxy::printui128(const unsigned __int128 *val) {
    legacy_ptr<const unsigned __int128> _val((void *)val);
    _interface->printui128(std::move(_val));
}

void vm_api_proxy::printsf( float val ) {
    _interface->printsf(*(float32_t*)&val);
}

void vm_api_proxy::printdf( double val ) {
    _interface->printdf(*(float64_t*)&val);
}

void vm_api_proxy::printqf( const long double *val ) {
    legacy_ptr<const float128_t> _val((void *)val);
    _interface->printqf(std::move(_val));
}

void vm_api_proxy::printn(uint64_t value) {
    _interface->printn(name(value));
}

void vm_api_proxy::printhex( const void *data, uint32_t data_size ) {
    legacy_span<const char> _data((void *)data, data_size);
    _interface->printhex(std::move(_data));
}
