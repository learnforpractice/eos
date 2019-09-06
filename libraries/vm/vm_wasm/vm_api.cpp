
#include <stdlib.h>
#include <stdint.h>
#include <stdexcept>

#include "vm_api.h"
static struct vm_api* s_api = nullptr;

extern "C" void register_vm_api_ro(struct vm_api* api) {
   if (!api) {
      throw std::runtime_error("vm_api pointer can not be NULL!");
   }
   s_api = api;
}

