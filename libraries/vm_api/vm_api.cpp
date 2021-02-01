#include <vm_api_proxy.hpp>

static vm_api_proxy *s_proxy;

extern "C" void init_vm_api(vm_api_proxy *proxy) {
    s_proxy = proxy;
}

extern "C" vm_api_proxy* get_vm_api() {
    return s_proxy;
}
