#include <stdint.h>
#include <stdlib.h>
#include <wasm-rt-impl.h>
#include <setjmp.h>
#include <vm_api.h>

#include <vector>
#include <array>

extern "C" {
  int micropython_contract_init(int type, const char *py_src, size_t size);
  int micropython_contract_apply(uint64_t receiver, uint64_t code, uint64_t action);
  size_t micropython_get_memory_size();
  size_t micropython_backup_memory(void *backup, size_t size);
  size_t micropython_restore_memory(void *backup, size_t size);
}

extern "C" int vm_apply(uint64_t receiver, uint64_t code, uint64_t action) {
  int err = wasm_rt_impl_try();
  if (err == 0) {
    return 0;
  } else {
    return -1;
  }
}

//static std::vector<std::vector<uint8_t>> setjmp_stack;

extern "C" void print_hex(char *data, size_t size) {
  for (int i=0;i<size;i++) {
    printf("%02x", data[i]);
  }
  printf("\n");
}

