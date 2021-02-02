#include <vm_api_proxy.hpp>

extern "C" {

void eosio_abort() {
    get_vm_api()->eosio_abort();
}

void eosio_assert(bool condition, const char *msg) {
    get_vm_api()->eosio_assert(condition, msg);
}

void eosio_assert_message( bool condition, const char *msg, size_t msg_size ) {
    get_vm_api()->eosio_assert_message(condition, msg, msg_size);
}

void eosio_assert_code( bool condition, uint64_t error_code ) {
    get_vm_api()->eosio_assert_code(condition, error_code);
}

void eosio_exit( int32_t code ) {
    get_vm_api()->eosio_exit(code);
}

}
