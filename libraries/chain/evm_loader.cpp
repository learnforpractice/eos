#include "evm_loader.hpp"
#include <fc/exception/exception.hpp>

#include <dlfcn.h>


static evm_interface interface{};


void load_evm(const char *evm_path) {
    void *handle = dlopen(evm_path, RTLD_LAZY | RTLD_LOCAL);
    FC_ASSERT(handle != NULL, "loading ${path} failed!", ("path", evm_path));

    fn_evm_init evm_init = (fn_evm_init)dlsym(handle, "evm_init");
    FC_ASSERT(evm_init, "evm_init not found!");
    evm_init();

    interface.execute = (fn_evm_execute)dlsym(handle, "evm_execute");
    FC_ASSERT(interface.execute, "execute not found!");

    interface.recover_key = (fn_evm_recover_key)dlsym(handle, "evm_recover_key");
    FC_ASSERT(interface.recover_key, "recover_key not found!");

    interface.get_account_id = (fn_evm_get_account_id)dlsym(handle, "evm_get_account_id");
    FC_ASSERT(interface.get_account_id, "get_account_id not found!");
};

evm_interface& evm_get_interface() {
    return interface;
}



