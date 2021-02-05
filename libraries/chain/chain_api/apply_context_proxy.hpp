#pragma once
#include <string>
#include <eosio/chain/controller.hpp>

namespace eosio { namespace chain {

class apply_context;
class wasm_interface;
class controller;

class apply_context_proxy {

public:
    apply_context_proxy();
    virtual ~apply_context_proxy();
    virtual void set_context(apply_context* ctx);
    virtual apply_context& get_context();
    virtual void checktime();

    virtual void timer_set_expiration_callback(void(*func)(void*), void* user);
    virtual bool timer_expired(void(*func)(void*), void* user);

    virtual bool contracts_console();

private:
    apply_context* ctx = nullptr;
};

} } //eosio::chain
