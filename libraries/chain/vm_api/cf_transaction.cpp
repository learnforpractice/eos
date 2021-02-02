#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include "vm_api_proxy.hpp"

size_t vm_api_proxy::read_transaction(char *data, size_t data_size) {
    legacy_span<char> _data(data, data_size);
    return _interface->read_transaction(std::move(_data));
}

size_t vm_api_proxy::transaction_size() {
    return _interface->transaction_size();
}

int32_t vm_api_proxy::expiration() {
    return _interface->expiration();
}

int32_t vm_api_proxy::tapos_block_num() {
    return _interface->tapos_block_num();
}

int32_t vm_api_proxy::tapos_block_prefix() {
    return _interface->tapos_block_prefix();
}

int32_t vm_api_proxy::get_action( uint32_t type, uint32_t index, char *buffer, size_t buffer_size) {
    legacy_span<char> _buffer(buffer, buffer_size);
    return _interface->get_action(type, index, std::move(_buffer));
}
