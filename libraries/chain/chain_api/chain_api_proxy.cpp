#include <eosio/chain/controller.hpp>
#include "chain_api_proxy.hpp"

chain_api_proxy::chain_api_proxy(eosio::chain::controller* c) {
    this->c = c;
}

chain_api_proxy::~chain_api_proxy() {

}

eosio::chain::controller* chain_api_proxy::chain() {
    return this->c;
}
