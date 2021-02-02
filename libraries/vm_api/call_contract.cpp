#include <vm_api_proxy.hpp>

extern "C" {

void call_contract(uint64_t contract, const char *args, size_t args_size) {
    get_vm_api()->call_contract(contract, args, args_size);
}

size_t call_contract_get_args(char *args, size_t args_size) {
   return get_vm_api()->call_contract_get_args(args, args_size);
}

size_t call_contract_set_results(const char *result, size_t size) {
   return get_vm_api()->call_contract_set_results(result, size);
}

size_t call_contract_get_results(char *result, size_t size) {
   return get_vm_api()->call_contract_get_results(result, size);
}

}
