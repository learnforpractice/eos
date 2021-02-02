#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include "vm_api_proxy.hpp"

size_t vm_api_proxy::get_active_producers( uint64_t *producers, size_t size ) {
    legacy_span<account_name> _producers((void *)producers, size);
    return _interface->get_active_producers(std::move(_producers));
}

