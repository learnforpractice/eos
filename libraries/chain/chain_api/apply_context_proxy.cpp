#include <eosio/chain/controller.hpp>
#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/transaction_context.hpp>

#include "apply_context_proxy.hpp"
#include "../vm_api/vm_api_proxy.hpp"

#include <dlfcn.h>

using namespace eosio::chain;

apply_context_proxy::apply_context_proxy() {
    this->_vm_api_proxy = std::make_unique<vm_api_proxy>();
}

apply_context_proxy::~apply_context_proxy() {
    
}

void apply_context_proxy::set_context(apply_context* ctx) {
    this->ctx = ctx;
    this->_vm_api_proxy->set_apply_context(ctx);
}

apply_context& apply_context_proxy::get_context() {
//    get_vm_api()->eosio_assert(this->ctx != nullptr, "apply_context can not be null");
    return *this->ctx;
}

vm_api_proxy *apply_context_proxy::get_vm_api_proxy() {
    return this->_vm_api_proxy.get();
}

void apply_context_proxy::timer_set_expiration_callback(void(*func)(void*), void* user) {
    get_context().trx_context.transaction_timer.set_expiration_callback(func, user);
}

bool apply_context_proxy::timer_expired(void(*func)(void*), void* user) {
    return get_context().trx_context.transaction_timer.expired;
}

void apply_context_proxy::checktime() {
    get_context().trx_context.checktime();
}

bool apply_context_proxy::contracts_console() {
    return get_context().control.contracts_console();
}
