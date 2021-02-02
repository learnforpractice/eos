#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include "vm_api_proxy.hpp"

size_t vm_api_proxy::read_action_data(char* msg, size_t len) {
    legacy_span<char> s2(msg, len);
    return _interface->read_action_data(std::move(s2));
}

size_t vm_api_proxy::action_data_size() {
    return _interface->action_data_size();
}

uint64_t vm_api_proxy::current_receiver() {
   return _interface->current_receiver().to_uint64_t();
}

void vm_api_proxy::set_action_return_value(const char *data, size_t data_size) {
   span<const char> packed_blob(data, data_size);
    _interface->set_action_return_value(std::move(packed_blob));
}
