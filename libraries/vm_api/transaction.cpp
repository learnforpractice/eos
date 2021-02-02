#include <vm_api_proxy.hpp>

extern "C" {

void send_inline( const char *data, size_t size ) {
    return get_vm_api()->send_inline(data, size);
}

void send_context_free_inline( const char *data, size_t size ) {
    return get_vm_api()->send_context_free_inline(data, size);    
}

void send_deferred( const __uint128 *sender_id, uint64_t payer, const char *data, size_t size, uint32_t replace_existing) {
    return get_vm_api()->send_deferred(sender_id, payer, data, size, replace_existing);
}

bool cancel_deferred( const __uint128 *sender_id ) {
   return get_vm_api()->cancel_deferred(sender_id);
}

}
