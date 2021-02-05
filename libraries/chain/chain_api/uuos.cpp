#include <dlfcn.h>

#include "uuos_proxy.hpp"
#include "apply_context_proxy.hpp"
#include "../vm_api/vm_api_proxy.hpp"
#include <stacktrace.h>

static uuos_proxy *s_proxy = nullptr;

extern "C" void uuos_init_chain() {
    if (s_proxy) {
        return;
    }

    s_proxy = new uuos_proxy();

    const char *chain_api_lib = getenv("CHAIN_API_LIB");
    if (!chain_api_lib) {
        printf("++++CHAIN_API_LIB environment variable not set!\n");
        exit(-1);
    }

    void *handle = dlopen(chain_api_lib, RTLD_LAZY | RTLD_GLOBAL);
    if (handle == 0) {
        printf("loading %s failed! error: %s\n", chain_api_lib, dlerror());
        exit(-1);
        return;
    }

    fn_init_uuos_proxy init_uuos_proxy = (fn_init_uuos_proxy)dlsym(handle, "init_uuos_proxy");
    if (!init_uuos_proxy) {
        printf("loading init_uuos_proxy failed! error: %s\n", dlerror());
        exit(-1);
        return;
    }
    init_uuos_proxy(s_proxy);

    const char *vm_api_lib = getenv("VM_API_LIB");
    if (!vm_api_lib) {
        printf("++++VM_API_LIB environment variable not set!\n");
        exit(-1);
    }

    handle = dlopen(vm_api_lib, RTLD_LAZY | RTLD_GLOBAL);
    if (handle == 0) {
        printf("loading %s failed! error: %s\n", vm_api_lib, dlerror());
        exit(-1);
        return;
    }

    fn_init_vm_api init_vm_api = (fn_init_vm_api)dlsym(handle, "init_vm_api");
    if (!init_vm_api) {
        printf("loading init_vm_api failed! error: %s\n", dlerror());
        exit(-1);
        return;
    }

    init_vm_api(s_proxy->get_apply_context_proxy()->get_vm_api_proxy());
}

extern "C" uuos_proxy *get_uuos_proxy() {
    if (s_proxy == nullptr) {
        print_stacktrace();
        printf("uuos_proxy ptr is null\n");
        exit(-1);
    }
    return s_proxy;
}
