
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

extern "C" struct vm_api* get_vm_api_ro() {
   if (!s_api) {
      throw std::runtime_error("vm api not specified!!!");
   }
   return s_api;
}
