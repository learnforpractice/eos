#include <vm_config.h>
#include "vmlua.c.bin"

#include <string.h>

#include <vm_api4c.h>

void eosio_assert(uint32_t test, const char* msg);

void *get_memory_ptr(uint32_t offset, uint32_t size) {
  int test = offset + size <= M0.size && offset + size >= offset;
  // if (!test) {
  //   vm_print_stacktrace();
  // }
  eosio_assert(test, "memory access out of bound!");
  return M0.data + offset;
}

void WASM_RT_ADD_PREFIX(init)(void) {
  init_func_types();
  init_globals();
  init_memory();
  init_table();
  init_exports();
  memcpy(&(M0.data[0]), &g1, 4);
  __wasm_call_ctors();
}

static void *_offset_to_ptr(u32 offset, u32 size) {
  return get_memory_ptr(offset, size);
}

static void *_offset_to_char_ptr(u32 offset) {
  return get_memory_ptr(offset, 64);
}

void *vm_lua_get_memory() {
  return M0.data;
}

size_t vm_lua_get_memory_size() {
  return M0.size;
}

size_t vm_lua_backup_memory(void *backup, size_t size) {
  size_t copy_size = 0;
  if (size > M0.size) {
    copy_size = M0.size;
  } else {
    copy_size = size;
  }
  memcpy(backup, M0.data, copy_size);
  return copy_size;
}

void vm_lua_init()
{
//  init_vm_api4c();
  // set_memory_converter(_offset_to_ptr, _offset_to_char_ptr);
  // WASM_RT_ADD_PREFIX(init)();
}

int vm_lua_init_contract(const char* script, size_t script_len) {
    u32 ptr_offset = realloc_0(0, script_len);
    char *ptr = get_memory_ptr(ptr_offset, script_len);
    memcpy(ptr, script, script_len);
    return lua_init_contract(ptr_offset, script_len);
}

int vm_lua_apply(uint64_t receiver, uint64_t code, uint64_t action) {
  return lua_apply(receiver, code, action);
}
