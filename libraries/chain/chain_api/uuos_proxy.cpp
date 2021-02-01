#include <fc/log/logger.hpp>
#include <eosio/chain/config.hpp>
#include <eosio/chain/abi_def.hpp>
#include <fc/io/json.hpp>

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

