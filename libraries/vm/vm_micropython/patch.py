
patch_init = ('''void WASM_RT_ADD_PREFIX(init)(void) {
  init_func_types();
  init_globals();
  init_memory();
  init_table();
  init_exports();
}
''', '')

patch_set_jmp = ('''static u32 setjmp_ex(u32);
static void longjmp_ex(u32, u32);
''',

r'''
// static u32 setjmp_ex(u32);
// static void longjmp_ex(u32, u32);
void *get_memory_ptr(int offset);
#include <setjmp.h>
#include <stdio.h>

void vm_checktime(void);

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
void prints_l( const char* cstr, uint32_t len);

static void mp_js_write(u32 p0, u32 len) {
  FUNC_PROLOGUE;
  char *p0_ptr = get_memory_ptr(p0);
  prints_l(p0_ptr, len);
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

    origin = 'wasm_rt_allocate_memory((&M0), 1, 65536);'
    patch = 'wasm_rt_allocate_memory((&M0), 2, 65536);'
    data = patch_micropython(data, origin, patch)

with open('micropython.c.bin', 'w') as f:
    f.write(data)
