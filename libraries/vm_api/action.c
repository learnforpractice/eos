#include <capi/types.h>
#include <vm_api_proxy.hpp>

uint32_t read_action_data( void* msg, uint32_t len ) {
    return get_vm_api()->read_action_data(msg, len);
}

uint32_t action_data_size() {
    return get_vm_api()->action_data_size();
}

void require_recipient( capi_name name ) {
    get_vm_api()->require_recipient(name);
}

void require_auth( capi_name name ) {
    get_vm_api()->require_auth(name);
}

bool has_auth( capi_name name ) {
    return get_vm_api()->has_auth(name);
}

void require_auth2( capi_name name, capi_name permission ) {
    get_vm_api()->require_auth2(name, permission);
}

bool is_account( capi_name name ) {
    return get_vm_api()->is_account(name);
}

void send_inline(char *serialized_action, size_t size) {
    get_vm_api()->send_inline(*serialized_action, size);
}

void send_context_free_inline(char *serialized_action, size_t size) {
    get_vm_api()->send_context_free_inline(*serialized_action, size);
}

uint64_t  publication_time() {
    return get_vm_api()->publication_time();
}

capi_name current_receiver() {
    return get_vm_api()->current_receiver();
}
