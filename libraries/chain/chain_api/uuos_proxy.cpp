#include <fc/log/logger.hpp>
#include <eosio/chain/config.hpp>
#include <eosio/chain/abi_def.hpp>
#include <fc/io/json.hpp>
#include <dlfcn.h>

#include "uuos_proxy.hpp"
#include "chain_macro.hpp"
#include "native_object.hpp"

#include "../vm_api/vm_api_proxy.hpp"

using namespace eosio::chain;

uuos_proxy::uuos_proxy() {
    this->_vm_api_proxy = std::make_shared<vm_api_proxy>();
}

uuos_proxy::~uuos_proxy() {
}

vm_api_proxy *uuos_proxy::get_vm_api_proxy() {
    return _vm_api_proxy.get();
}

chain_proxy* uuos_proxy::chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir) {
    chain_proxy *proxy = new chain_proxy(config, _genesis, protocol_features_dir, snapshot_dir);
    return proxy;
}

void uuos_proxy::chain_free(chain_proxy* c) {
    if (!c) {
        return;
    }
    delete c;
}

void uuos_proxy::set_log_level(string& logger_name, int level) {
    fc::logger::get(logger_name).set_log_level(fc::log_level(level));
}

void uuos_proxy::set_block_interval_ms(int ms) {
    eosio::chain::config::set_block_interval_ms(ms);
}

void uuos_proxy::pack_abi(string& abi, vector<char>& packed_obj)
{
    try {
        auto _abi = fc::json::from_string(abi).as<abi_def>();
        packed_obj = fc::raw::pack<abi_def>(_abi);
    } CATCH_AND_LOG_EXCEPTION(this);
}

void uuos_proxy::pack_native_object(int type, string& msg, vector<char>& packed_obj) {
    pack_native_object_(type, msg, packed_obj);
}

void uuos_proxy::unpack_native_object(int type, string& packed_obj, string& result) {
    unpack_native_object_(type, packed_obj, result);
}

string& uuos_proxy::get_last_error() {
    return last_error;
}

void uuos_proxy::set_last_error(string& error) {
    last_error = error;
}

uint64_t uuos_proxy::s2n(string& s) {
    return eosio::chain::name(s).to_uint64_t();
}

string uuos_proxy::n2s(uint64_t n) {
    return eosio::chain::name(n).to_string();
}

bool uuos_proxy::set_native_contract(uint64_t contract, const string& native_contract_lib) {
    if (native_contract_lib.size() == 0) {
        auto itr = debug_contracts.find(contract);
        if (itr != debug_contracts.end()) {
            dlclose(itr->second.handle);
            debug_contracts.erase(itr);
        }
        return true;
    } else {
        void* handle = dlopen(native_contract_lib.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if (!handle) {
            elog("++++++++${s} load failed!", ("s", native_contract_lib));
            return false;
        }
        fn_native_apply native_apply = (fn_native_apply)dlsym(handle, "native_apply");
        if (native_apply == nullptr) {
            elog("++++++++native_apply entry not found!");
            return false;
        }
        debug_contracts[contract] = {native_contract_lib, handle, native_apply};
        return true;
    }
}

string uuos_proxy::get_native_contract(uint64_t contract) {
    auto itr = debug_contracts.find(contract);
    if (itr == debug_contracts.end()) {
        return "";
    }
    return itr->second.path;
}

bool uuos_proxy::call_native_contract(uint64_t receiver, uint64_t first_receiver, uint64_t action) {
    auto itr = debug_contracts.find(receiver);
    if (itr == debug_contracts.end()) {
        return false;
    }
    itr->second.apply(receiver, first_receiver, action);
    return true;
}

void uuos_proxy::enable_native_contracts(bool debug) {
    this->native_contracts_enabled = debug;
}

bool uuos_proxy::is_native_contracts_enabled() {
    return this->native_contracts_enabled;
}

