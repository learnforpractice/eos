#include "uuos.hpp"

static uuos_proxy *s_proxy;

extern "C" void init_uuos_proxy(uuos_proxy *proxy) {
    s_proxy = proxy;
}

extern "C" uuos_proxy* get_uuos_proxy() {
    return s_proxy;
}
