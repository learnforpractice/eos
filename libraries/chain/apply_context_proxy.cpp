#include <eosio/chain/controller.hpp>
#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/transaction_context.hpp>

#include <eosio/chain/apply_context_proxy.hpp>

#include <dlfcn.h>
#include <vm_api.h>

namespace eosio { namespace chain {


apply_context_proxy::apply_context_proxy() {

}

apply_context_proxy::~apply_context_proxy() {
    
}

void apply_context_proxy::set_context(apply_context* ctx) {
    this->ctx = ctx;
}

apply_context& apply_context_proxy::get_context() {
    get_vm_api()->eosio_assert(this->ctx != nullptr, "apply_context can not be null");
    return *this->ctx;
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

} } //eosio::chain