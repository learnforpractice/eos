#include <eosio/chain_plugin/chain_plugin.hpp>
void *chain_new() {
    return (void *)new eosio::chain_plugin();
}
