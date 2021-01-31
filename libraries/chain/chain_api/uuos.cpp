#include "uuos_proxy.hpp"
#include <dlfcn.h>
static uuos_proxy *s_proxy = nullptr;

extern "C" void uuos_init_chain() {
    if (s_proxy) {
        return;
    }

    s_proxy = new uuos_proxy();

    const char *chain_api_lib = getenv("CHAIN_API_LIB");
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
}

extern "C" uuos_proxy *get_uuos_proxy() {
    return s_proxy;
}
