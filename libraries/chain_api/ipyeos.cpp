#include <ipyeos_proxy.hpp>

static ipyeos_proxy *s_proxy;

extern "C" void init_ipyeos_proxy(ipyeos_proxy *proxy) {
    s_proxy = proxy;
}

extern "C" ipyeos_proxy* get_ipyeos_proxy() {
    return s_proxy;
}
