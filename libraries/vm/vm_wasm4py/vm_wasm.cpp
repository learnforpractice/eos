#include <stdio.h>
#include <stdlib.h>

#include <eosio/chain/wasm_interface.hpp>
#include "vm_interface.hpp"

using namespace eosio::chain;

static wasm_interface* wif = nullptr;

extern "C" void wasm4py_init() {
    if (wif) {
        return;
    }
    wif = new wasm_interface(eosio::chain::wasm_interface::vm_type::wabt);
}

extern "C" void wasm4py_deinit() {
    if (!wif) {
        return;
    }
    delete wif;
    wif = nullptr;
}

extern "C" void wasm4py_validate(const bytes& code) {
    wif->validate(code);
}

extern "C" bool wasm4py_apply(const eosio::chain::digest_type& code_id, const fc::raw::shared_string& code) {
    return wif->apply(code_id, code);
}

extern "C" bool wasm4py_call(uint64_t contract, uint64_t func_name, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    return wif->call(contract, func_name, arg1, arg2, arg3);
}
