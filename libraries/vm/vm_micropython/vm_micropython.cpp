#include <stdint.h>
#include <stdlib.h>
#include <wasm-rt-impl.h>

extern "C" int micropython_contract_init(const char *mpy, size_t size);
extern "C" int micropython_contract_apply(uint64_t receiver, uint64_t code, uint64_t action);

extern "C" int vm_init() {
    return 0;
}

extern "C" int vm_apply(uint64_t receiver, uint64_t code, uint64_t action) {
  int err = wasm_rt_impl_try();
  if (err == 0) {
    return 0;
  } else {
    return -1;
  }
}
