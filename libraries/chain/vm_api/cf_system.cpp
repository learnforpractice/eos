#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include "vm_api_proxy.hpp"

void vm_api_proxy::eosio_abort() {
    _interface->abort();
}

void vm_api_proxy::eosio_assert(bool condition, const char *msg) {
    null_terminated_ptr _msg(msg);
    _interface->eosio_assert(condition, _msg);
}

void vm_api_proxy::eosio_assert_message( bool condition, const char *msg, size_t msg_size ) {
    legacy_span<const char> _msg((void *)msg, msg_size);
    _interface->eosio_assert_message(condition, std::move(_msg));
}

void vm_api_proxy::eosio_assert_code( bool condition, uint64_t error_code ) {
    _interface->eosio_assert_code(condition, error_code);
}

void vm_api_proxy::eosio_exit( int32_t code ) {
    _interface->eosio_exit(code);
}
