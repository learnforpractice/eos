#include <vm_api_proxy.hpp>
#include <stacktrace.h>

static vm_api_proxy *s_proxy;

extern "C" void init_vm_api(vm_api_proxy *proxy) {
    s_proxy = proxy;
}

extern "C" vm_api_proxy* get_vm_api() {
    if (s_proxy == nullptr) {
        printf("+++++++vm_api_proxy not initialized!");
        print_stacktrace();
        exit(-1);
        return nullptr;
    }
    return s_proxy;
}
