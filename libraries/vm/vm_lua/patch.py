
patch_init = ('''void WASM_RT_ADD_PREFIX(init)(void) {
  init_func_types();
  init_globals();
  init_memory();
  init_table();
  init_exports();
}
''', '')

patch_set_jmp1 = ('''static void longjmp_ex(u32, u32);
static u32 setjmp_ex(u32);
''',

r'''
// static u32 setjmp_ex(u32);
// static void longjmp_ex(u32, u32);
void *get_memory_ptr(u32 offset, u32 size);
void vm_checktime(void);
void print_hex(char *data, size_t size);

#include <setjmp.h>

void setjmp_push(jmp_buf buf);
void setjmp_pop(jmp_buf buf);

#define setjmp_ex(i0) \
  i0; \
  { \
    jmp_buf buf; \
    int n = _setjmp(buf); \
    if (n == 0) { \
      setjmp_push(buf); \
    } \
    i0 = n; \
  }

#define longjmp_ex(p0, p1) \
{ \
  jmp_buf buf; \
  setjmp_pop(buf); \
  _longjmp(buf, p1); \
}
''')

patch_set_jmp2 = ('''static void longjmp_ex(u32 p0, u32 p1) {
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
''',

r'''
''')

header_patch = (
'''#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
''',

'''#include <math.h>
#include <string.h>
void vm_checktime(void);
'''
)

def patch_micropython(data, origin, patch):
    index = data.find(origin)
    print(index)
    assert index >= 0
    data = data[:index] + patch + data[index+len(origin):]
    return data

with open('vmlua.c.bin', 'r') as f:
    data = f.read()
    origin, patch = header_patch
    data = patch_micropython(data, origin, patch)

    origin, patch = patch_init
    data = patch_micropython(data, origin, patch)

    data = data.replace('fmodl', '_fmodl')
    data = data.replace('copysignl', '_copysignl')
    data = data.replace('scalbnl', '_scalbnl')
    data = data.replace('strnlen', '_strnlen')
    data = data.replace('frexpl', '_frexpl')
    data = data.replace('setjmp', 'setjmp_ex')
    data = data.replace('longjmp', 'longjmp_ex')

    origin, patch = patch_set_jmp1
    data = patch_micropython(data, origin, patch)

    origin, patch = patch_set_jmp2
    data = patch_micropython(data, origin, patch)

    origin = 'wasm_rt_allocate_memory((&M0), 1, 65536);'
    if data.find(origin) < 0:
      origin = 'wasm_rt_allocate_memory((&M0), 2, 65536);'
    if data.find(origin) < 0:
      origin = 'wasm_rt_allocate_memory((&M0), 1, 528);'
    patch = 'memset(&M0, 0, sizeof(M0));wasm_rt_allocate_memory((&M0), 2, VM_MAX_MEMORY_SIZE/65536);'
    data = patch_micropython(data, origin, patch)
    data = data.replace('__fpclassifyl', '____fpclassifyl')

with open('vmlua.c.bin', 'w') as f:
    f.write(data)
