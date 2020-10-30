
patch_init = ('''void WASM_RT_ADD_PREFIX(init)(void) {
  init_func_types();
  init_globals();
  init_memory();
  init_table();
  init_exports();
}
''',

r'''
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
}

int micropython_contract_init(const char *mpy, size_t size) {
  u32 offset = malloc(size);
  char *ptr = (char *)get_memory_ptr(offset);
  memcpy(ptr, mpy, size);
  return micropython_init_from_mpy(offset, size);
}

int micropython_contract_apply(uint64_t receiver, uint64_t code, uint64_t action) {
  return micropython_apply(receiver, code, action);
}

''')

patch_set_jmp = ('''static u32 setjmp_ex(u32);
static void longjmp_ex(u32, u32);
''',

r'''
// static u32 setjmp_ex(u32);
// static void longjmp_ex(u32, u32);
static void *get_memory_ptr(int offset);
#include <setjmp.h>
#include <stdio.h>

void vm_checktime(void) {

}

#define setjmp_ex(p0) \
  setjmp(*(jmp_buf*)get_memory_ptr(p0))

#define longjmp_ex(p0, p1) \
{ \
  void *ptr = get_memory_ptr(p0); \
  printf("++++++++longjmp:%p\n", ptr); \
  longjmp(*(jmp_buf*)ptr, p1); \
}
''')

patch_ticks_ms = ('''static void mp_js_write(u32 p0, u32 p1) {
  FUNC_PROLOGUE;
  FUNC_EPILOGUE;
}

static u32 mp_js_ticks_ms(void) {
  FUNC_PROLOGUE;
  u32 i0;
  i0 = 0u;
  FUNC_EPILOGUE;
  return i0;
}

static void mp_js_hook(void) {
  FUNC_PROLOGUE;
  FUNC_EPILOGUE;
}

static u32 setjmp_ex(u32 p0) {
  FUNC_PROLOGUE;
  u32 i0;
  i0 = 0u;
  FUNC_EPILOGUE;
  return i0;
}

static void longjmp_ex(u32 p0, u32 p1) {
  FUNC_PROLOGUE;
  FUNC_EPILOGUE;
}
''',

r'''
static void mp_js_write(u32 p0, u32 p1) {
  FUNC_PROLOGUE;
  char *p0_ptr = get_memory_ptr(p0);
  for (int i=0;i<p1;i++) {
    putchar(p0_ptr[i]);
  }
  FUNC_EPILOGUE;
}

#include <sys/time.h>

long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long us = te.tv_sec*1000000LL + te.tv_usec; // calculate milliseconds
//    printf("us: %lld\n", us);
    return us/1000;
}

static u32 mp_js_ticks_ms(void) {
  FUNC_PROLOGUE;
  u32 i0;
  i0 = 0u;
  FUNC_EPILOGUE;
  return current_timestamp();
}

static void mp_js_hook(void) {
  FUNC_PROLOGUE;
  FUNC_EPILOGUE;
}
''')

def patch_micropython(data, origin, patch):
    index = data.find(origin)
    print(index)
    assert index >= 0
    data = data[:index] + patch + data[index+len(origin):]
    return data

with open('micropython.c.bin', 'r') as f:
    data = f.read()
    origin, patch = patch_set_jmp

    data = patch_micropython(data, origin, patch)

    origin, patch = patch_init
    data = patch_micropython(data, origin, patch)

    origin, patch = patch_ticks_ms
    data = patch_micropython(data, origin, patch)

with open('micropython.c.bin', 'w') as f:
    f.write(data)
