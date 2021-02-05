#pragma once

#include "uuos_proxy.hpp"

extern "C" void uuos_init_chain();
extern "C" void init_new_chain_api();
extern "C" uuos_proxy *get_uuos_proxy();
extern "C" apply_context_proxy *get_apply_context_proxy();
extern "C" vm_api_proxy *get_vm_api_proxy();
