#include "vm_api4c.hpp"

static vm_api4c_proxy *s_proxy;

extern "C" void int_vm_api4c_proxy(vm_api4c_proxy *proxy) {
    s_proxy = proxy;
}

extern "C" vm_api4c_proxy *get_vm_api4c_proxy(vm_api4c_proxy *proxy) {
    return s_proxy;
}

