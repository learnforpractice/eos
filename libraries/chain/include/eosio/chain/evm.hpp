#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <dlfcn.h>

typedef int (*fn_evm_call_native)(int type, const uint8_t *packed_args, size_t packed_args_size, uint8_t *output, size_t output_size);

void evm_init(const char* evm_lib);

int evm_call_native(int type, const uint8_t *packed_args, size_t packed_args_size, uint8_t *output, size_t output_size);

