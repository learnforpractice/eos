#include <capi/types.h>
#include <vm_api_proxy.hpp>

extern "C" {

void  eosio_assert( uint32_t test, const char* msg ) {
    get_vm_api()->eosio_assert(test, msg);
}

void  eosio_assert_message( uint32_t test, const char* msg, uint32_t msg_len ) {
    get_vm_api()->eosio_assert_message(test, msg, msg_len);
}

void  eosio_assert_code( uint32_t test, uint64_t code ) {
    get_vm_api()->eosio_assert_code(test, code);
}

void eosio_exit( int32_t code ) {
    return get_vm_api()->eosio_exit(code);
}

uint64_t  current_time() {
    return get_vm_api()->current_time();
}

bool is_feature_activated( const capi_checksum256* feature_digest ) {
    return get_vm_api()->is_feature_activated(feature_digest);
}

capi_name get_sender() {
    return get_vm_api()->get_sender();
}


}
