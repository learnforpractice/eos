#pragma once
void wasm_call(uint64_t contract, uint64_t func_name, const char *args, size_t args_size);
void vm_call(uint64_t contract, uint64_t func_name, const char *args, size_t args_size);
int call_contract_get_args(void* args, size_t size);
int call_contract_set_results(const void* result, size_t size1);
int call_contract_get_results(void* result, size_t size1);
void call_contract_cleanup();
