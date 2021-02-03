#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include "vm_api_proxy.hpp"

void vm_api_proxy::send_inline( char *data, uint32_t size ) {
    legacy_span<const char> _data((void *)data, size);
    return _interface->send_inline(std::move(_data));
}

void vm_api_proxy::send_context_free_inline( char *data, uint32_t size ) {
    legacy_span<const char> _data((void *)data, size);
    return _interface->send_context_free_inline(std::move(_data));    
}

void vm_api_proxy::send_deferred( const uint128_t sender_id, uint64_t payer, const char *data, uint32_t size, uint32_t replace_existing) {
    legacy_ptr<const uint128_t> _sender_id((void *)&sender_id);
    legacy_span<const char> _data((void *)data, size);
    return _interface->send_deferred(std::move(_sender_id), name(payer), std::move(_data), replace_existing);
}

int32_t vm_api_proxy::cancel_deferred( const uint128_t sender_id ) {
   legacy_ptr<const uint128_t> _sender_id((void *)&sender_id);
   return _interface->cancel_deferred(std::move(_sender_id));
}
