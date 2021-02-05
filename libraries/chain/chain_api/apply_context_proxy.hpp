#pragma once
#include <string>
#include <memory>

class vm_api_proxy;

namespace eosio { namespace chain {

class apply_context;

}}


class apply_context_proxy {

public:
    apply_context_proxy();
    virtual ~apply_context_proxy();
    virtual void set_context(eosio::chain::apply_context* ctx);
    virtual eosio::chain::apply_context& get_context();
    virtual vm_api_proxy *get_vm_api_proxy();
    virtual void checktime();

    virtual void timer_set_expiration_callback(void(*func)(void*), void* user);
    virtual bool timer_expired(void(*func)(void*), void* user);

    virtual bool contracts_console();

private:
    eosio::chain::apply_context* ctx = nullptr;
    std::unique_ptr<vm_api_proxy> _vm_api_proxy;
};
