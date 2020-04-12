#include "evm_loader.hpp"
#include <fc/exception/exception.hpp>

#include <dlfcn.h>


static evm_interface interface{};

// extern "C" int call_native(int main_type, int type, const uint8_t *input, size_t input_size, uint8_t *output, size_t output_size);
// extern "C" int evm_execute(const uint8_t *raw_trx, uint32_t raw_trx_size, const char *sender_address, uint32_t sender_address_size);

void load_evm(const char *evm_path) {
    void *handle = dlopen(evm_path, RTLD_LAZY | RTLD_LOCAL);
    FC_ASSERT(handle != NULL, "loading ${path} failed!", ("path", evm_path));

    fn_evm_init evm_init = (fn_evm_init)dlsym(handle, "evm_init");
    FC_ASSERT(evm_init, "evm_init not found!");
    evm_init();

    interface.execute = (fn_evm_execute)dlsym(handle, "native_evm_execute");
    FC_ASSERT(interface.execute, "execute not found!");

    interface.recover_key = (fn_evm_recover_key)dlsym(handle, "evm_recover_key");
    FC_ASSERT(interface.recover_key, "recover_key not found!");

    interface.call_native = (fn_evm_call_native)dlsym(handle, "evm_call_native");
    FC_ASSERT(interface.call_native, "evm_call_native not found!");
};

evm_interface& evm_get_interface() {
    return interface;
}

int evm_execute(const char *raw_trx, uint32_t raw_trx_size, const char *sender_address, uint32_t sender_address_size) {
    return evm_get_interface().execute(raw_trx, raw_trx_size, sender_address, sender_address_size);
}
