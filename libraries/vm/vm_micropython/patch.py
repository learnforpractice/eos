
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
void *get_memory_ptr(u32 offset, u32 size);
void vm_checktime(void);
void print_hex(char *data, size_t size);

#include <setjmp.h>
#include <stdio.h>

void setjmp_push(jmp_buf buf);
void setjmp_pop(jmp_buf buf);

#define setjmp_ex(i0) \
  i0; \
  { \
    jmp_buf buf; \
    int n = setjmp(buf); \
    if (n == 0) { \
      setjmp_push(buf); \
    } \
    i0 = n; \
  }

#define longjmp_ex(p0, p1) \
{ \
  jmp_buf buf; \
  setjmp_pop(buf); \
  longjmp(buf, p1); \
}
''')

patch_ticks_ms = ('''static void mp_js_write(u32 p0, u32 p1) {
  FUNC_PROLOGUE;
  FUNC_EPILOGUE;
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
  char *p0_ptr = get_memory_ptr(p0, len);
  prints_l(p0_ptr, len);
  FUNC_EPILOGUE;
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
    if data.find(origin) < 0:
      origin = 'wasm_rt_allocate_memory((&M0), 2, 65536);'
    patch = 'memset(&M0, 0, sizeof(M0));wasm_rt_allocate_memory((&M0), 2, PYTHON_VM_MAX_MEMORY_SIZE/65536);'
    data = patch_micropython(data, origin, patch)
    data = data.replace('__fpclassifyl', '____fpclassifyl')

with open('micropython.c.bin', 'w') as f:
    f.write(data)
