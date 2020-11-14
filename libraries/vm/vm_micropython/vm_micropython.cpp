#include <stdint.h>
#include <stdlib.h>
#include <wasm-rt-impl.h>
#include <setjmp.h>
#include <vector>
#include <vm_api.h>

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

static std::vector<std::vector<uint8_t>> setjmp_stack;

extern "C" void print_hex(char *data, size_t size) {
  for (int i=0;i<size;i++) {
    printf("%02x", data[i]);
  }
  printf("\n");
}

extern "C" {
#include <stacktrace.h>
void vm_print_stacktrace(void) {
   print_stacktrace();
}

void setjmp_clear_stack() {
  setjmp_stack.clear();
}

void setjmp_push(jmp_buf buf) {
  std::vector<uint8_t> _buf(sizeof(jmp_buf));
  memcpy(_buf.data(), buf, sizeof(jmp_buf));
  setjmp_stack.push_back(_buf);
}

void setjmp_pop(jmp_buf buf) {
  if (setjmp_stack.empty()) {
    get_vm_api()->eosio_assert(false, "setjmp stack empty!");
    return;
  }

  std::vector<uint8_t>& _buf = setjmp_stack.back();
  memcpy(buf, _buf.data(), sizeof(jmp_buf));
  setjmp_stack.pop_back();
}

void setjmp_discard_top() {
  if (setjmp_stack.empty()) {
    get_vm_api()->eosio_assert(false, "setjmp stack empty!");
    return;
  }
  setjmp_stack.pop_back();
}

}

