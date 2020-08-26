#include <eosio/chain/evm.hpp>
#include <eosio/chain/exceptions.hpp>

using namespace fc;

static fn_evm_call_native evm_call = nullptr;

void evm_init(const char* evm_lib) {
   if (!evm_call) {
      void *handle = dlopen(evm_lib, RTLD_LAZY | RTLD_GLOBAL);
      EOS_ASSERT(handle, assert_exception, "load evm lib failed!");
      evm_call = (fn_evm_call_native)dlsym(handle, "evm_call_native");
      EOS_ASSERT(evm_call, assert_exception, "load evm_call_native failed!");
   }
}

int evm_call_native(int type, const uint8_t *packed_args, size_t packed_args_size, uint8_t *output, size_t output_size) {
   return evm_call(type, packed_args, packed_args_size, output, output_size);
}
