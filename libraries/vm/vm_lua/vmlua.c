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

void lua_init()
{
  init_vm_api4c();
  set_memory_converter(_offset_to_ptr, _offset_to_char_ptr);
  WASM_RT_ADD_PREFIX(init)();
}

int vmlua_run_script() {
    static int initialized = 0;
    if (!initialized) {
        initialized = 1;
        lua_init();
    }
    const char *script = "function main ()\n"
    "	print(\"bar\")\n"
    "	return \"foo\"\n"
    "end\n"
    "return main()";

    size_t script_len = strlen(script);
    u32 ptr_offset = realloc_0(0, script_len + 1);
    char *ptr = get_memory_ptr(ptr_offset, script_len + 1);
    memcpy(ptr, script, script_len);
    ptr[script_len] = '\0';
    return run_lua(ptr_offset);
}
