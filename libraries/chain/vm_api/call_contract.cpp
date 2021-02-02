#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include "vm_api_proxy.hpp"

void vm_api_proxy::call_contract(uint64_t contract, const char *args, size_t args_size) {
    legacy_span<const char> buffer((void *)args, args_size);
    _interface->call_contract(contract, std::move(buffer));
}

size_t vm_api_proxy::call_contract_get_args(char *args, size_t args_size) {
   legacy_span<char> buffer(args, args_size);
   return _interface->call_contract_get_args(std::move(buffer));
}

size_t vm_api_proxy::call_contract_set_results(const char *result, size_t size) {
   legacy_span<const char> buffer((void *)result, size);
   return _interface->call_contract_set_results(std::move(buffer));
}

size_t vm_api_proxy::call_contract_get_results(char *result, size_t size) {
   legacy_span<char> buffer(result, size);
   return _interface->call_contract_get_results(std::move(buffer));
}

