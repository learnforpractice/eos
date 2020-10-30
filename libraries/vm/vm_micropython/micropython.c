#include "micropython.c.bin"
u32 (*Z_envZ_memsetZ_iiii)(u32, u32, u32);
u32 (*Z_envZ_memcpyZ_iiii)(u32, u32, u32);
u32 (*Z_envZ_memmoveZ_iiii)(u32, u32, u32);
void (*Z_envZ_prints_lZ_vii)(u32, u32);
void (*Z_envZ_eosio_assertZ_vii)(u32, u32);

//void * memset ( void * ptr, int value, size_t num );
#include <string.h>

u32 _memset(u32 ptr_offset, u32 value, u32 num) {
  char *ptr = get_memory_ptr(ptr_offset);
  memset(ptr, value, num);
  return ptr_offset;
}

//void * memcpy ( void * destination, const void * source, size_t num );
u32 _memcpy(u32 dest_offset, u32 src_offset, u32 num) {
  char *src_ptr = get_memory_ptr(src_offset);
  char *dest_ptr = get_memory_ptr(dest_offset);
  memcpy(dest_ptr, src_ptr, num);
  return dest_offset;
}

//void * memmove ( void * destination, const void * source, size_t num )
u32 _memmove(u32 dest_offset, u32 src_offset, u32 num) {
  char *src_ptr = get_memory_ptr(src_offset);
  char *dest_ptr = get_memory_ptr(dest_offset);
  memmove(dest_ptr, src_ptr, num);
  return dest_offset;
}

void _prints(u32 a, u32 b) {

}

void _eosio_assert(u32 a, u32 b) {

}

void WASM_RT_ADD_PREFIX(init)(void) {
  Z_envZ_memsetZ_iiii = _memset;
  Z_envZ_memcpyZ_iiii = _memcpy;
  Z_envZ_memmoveZ_iiii = _memmove;
  Z_envZ_prints_lZ_vii = _prints;
  Z_envZ_eosio_assertZ_vii = _eosio_assert;

  init_func_types();
  init_globals();
  init_memory();
  init_table();
  init_exports();
  memcpy(&(M0.data[0]), &g1, 4);
}

const char *script = \
"import time\n"  \
"print(dir(time))\n" \
"import struct\n" \
"import hello\n" \
"print(hello.say_hello())\n" \
"print(dir(struct))\n" \
"t0 = time.ticks_ms()\n" \
"for i in range(1):\n" \
"    aa = struct.pack('QQ3s', 123, 456, 'abc')\n" \
"print('pack.py', time.ticks_ms()-t0)\n" \
;

#include "wasm-rt-impl.h"

int run_main(int argc, char **argv) {
  int code = wasm_rt_impl_try();
  if (code == 0) {
    init();
    printf("+++++++++hello,world\n");
    apply(0, 0, 0);
    int script_size = strlen(script);
    u32 ptr_offset = malloc(script_size+1);
    if (ptr_offset == 0) {
      printf("bad malloc\n");
      return -1;
    }
    char *ptr = get_memory_ptr(ptr_offset);
    memset(ptr, 0, script_size+1);
    memcpy(ptr, script, script_size);
//    printf("%ld, %s\n", ptr_offset, script);
    do_str(ptr_offset, 1);
    return 0;
  } else {
    printf("trap: %d\n", code);
    return -code;
  }
}

static void *get_memory_ptr(int offset) {
  return M0.data + offset;
}

int micropython_init() {
  init();
  mp_js_init(64*1024);
  return 1;
}

int micropython_contract_init(int type, const char *py_src, size_t size) {
  u32 offset = malloc(size);
  char *ptr = (char *)get_memory_ptr(offset);
  memcpy(ptr, py_src, size);
  return micropython_init_module(type, offset, size);
}

int micropython_contract_apply(uint64_t receiver, uint64_t code, uint64_t action) {
  int trap_code = wasm_rt_impl_try();
  if (trap_code == 0) {
    return micropython_apply(receiver, code, action);
  } else {
    printf("++++trap code: %d\n", trap_code);
  }
  return 0;
}

void *micropython_get_memory() {
  return M0.data;
}

size_t micropython_get_memory_size() {
  return M0.size;
}
