#include <stdint.h>
#include "micropython_vm_config.h"
#include <wasm-rt-impl.h>
#include <vm_api4c.h>

static void *get_memory_ptr(uint32_t offset, uint32_t size);
void vm_checktime(void);
void eosio_assert( uint32_t test, const char* msg );
void vm_print_stacktrace(void);

#include "micropython_eosio.c.bin"
#include <stdio.h>

static void *get_memory_ptr(uint32_t offset, uint32_t size) {
  int test = offset + size <= M0.size && offset + size >= offset;
  // if (!test) {
  //   vm_print_stacktrace();
  //   printf("++++++++offset %u, size %u\n", offset, size);
  // }
  if (!test) {
    eosio_assert(0, "memory access out of bound!");
  }
  return M0.data + offset;
}

static void *_offset_to_ptr(u32 offset, u32 size) {
  return get_memory_ptr(offset, size);
}

static void *_offset_to_char_ptr(u32 offset) {
  return get_memory_ptr(offset, 64);
}

static char g_backup_memory[64*1024];
void micropython_eosio_init() {
  static int initialized = 0;
  if (!initialized) {
    initialized = 1;
    init_eosio();
    memcpy(&(M0.data[0]), &g1, 4);
    memcpy(g_backup_memory, M0.data, 64*1024);
  } else {
    wasm_rt_allocate_memory((&M0), 1, PYTHON_VM_MAX_MEMORY_SIZE/65536);
    init_globals();
    memcpy(M0.data, g_backup_memory, 64*1024);
//    memcpy(&(M0.data[0]), &g1, 4);
  }
}

extern uint32_t wasm_rt_call_stack_depth;

int micropython_eosio_apply(uint64_t receiver, uint64_t account, uint64_t action) {
  return 0;
  if (receiver != 7684013976526520320) {//hello
      return 0;
  }
  set_memory_converter(_offset_to_ptr, _offset_to_char_ptr);
  micropython_eosio_init();
  wasm_rt_call_stack_depth = 0;
  int trap_code = wasm_rt_impl_try();
  if (trap_code == 0) {
    apply(receiver, account, action);
    return 1;
  } else {
    printf("++++micropython_contract_apply:trap code: %d\n", trap_code);
    wasm_rt_on_trap(trap_code);
    return 0;
  }
  return 0;
}
