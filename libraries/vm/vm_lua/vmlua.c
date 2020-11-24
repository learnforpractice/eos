#include <vm_config.h>
#include "vmlua.c.bin"

#include <string.h>

void eosio_assert(uint32_t test, const char* msg);

void *get_memory_ptr(uint32_t offset, uint32_t size) {
  int test = offset + size <= M0.size && offset + size >= offset;
  // if (!test) {
  //   vm_print_stacktrace();
  // }
  eosio_assert(test, "memory access out of bound!");
  return M0.data + offset;
}

int vmlua_run_script() {
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
