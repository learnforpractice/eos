#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>

#include "vm_api_proxy.hpp"

vm_api_proxy::vm_api_proxy() {

}

vm_api_proxy::~vm_api_proxy() {

}

void vm_api_proxy::set_apply_context(apply_context* ctx) {
    if (ctx) {
        _interface = std::make_unique<webassembly::interface>(*ctx);
    } else {
        _interface.reset();
    }
}

size_t vm_api_proxy::read_action_data(char* msg, size_t len) {
    legacy_span<char> s2(msg, len);
    _interface->read_action_data(std::move(s2));
}

size_t vm_api_proxy::action_data_size(void) {
    return _interface->action_data_size();
}
