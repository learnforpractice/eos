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

