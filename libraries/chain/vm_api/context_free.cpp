#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include <fc/uint128.hpp>
#include "vm_api_proxy.hpp"

int32_t vm_api_proxy::get_context_free_data(uint32_t index, char *data, uint32_t data_size) {
    legacy_span<char> buffer(data, data_size);
    return _interface->get_context_free_data(index, std::move(buffer));
}
