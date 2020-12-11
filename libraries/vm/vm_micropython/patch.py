
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

pathc_setjmp = (r'''static u32 setjmp_ex(u32 p0) {
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
''')

header_patch = (
'''#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
''',

'''
#if 100*__GNUC__+__GNUC_MINOR__ >= 303
#define INFINITY  __builtin_inff()
#else
#define INFINITY  1e5000f
#endif

#include <string.h>
'''
)

nlr_pop_patch = (
r'''static void nlr_pop(void) {
  FUNC_PROLOGUE;
  u32 i0, i1;
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&M0), (u64)(i1 + 10264));
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0 + 10264), i1);
  FUNC_EPILOGUE;
}
''',

r'''
static void _nlr_pop(void) {
  FUNC_PROLOGUE;
  u32 i0, i1;
  i0 = 0u;
  i1 = 0u;
  i1 = i32_load((&M0), (u64)(i1 + 10264));
  i1 = i32_load((&M0), (u64)(i1));
  i32_store((&M0), (u64)(i0 + 10264), i1);
  FUNC_EPILOGUE;
}

void setjmp_discard_top();
static void nlr_pop(void) {
  _nlr_pop();
  setjmp_discard_top();
}

'''
)

frozen_stat_patch = (
r'''static u32 export_vm_frozen_stat(u32 p0) {
  FUNC_PROLOGUE;
  u32 i0;
  i0 = 0u;
  FUNC_EPILOGUE;
  return i0;
}
''',

r'''
u32 _vm_frozen_stat(u32 str_offset);
static u32 export_vm_frozen_stat(u32 p0) {
  return _vm_frozen_stat(p0);
}
'''
)

vm_load_frozen_module_patch = (
'''static u32 export_vm_load_frozen_module(u32 p0, u32 p1, u32 p2, u32 p3) {
  FUNC_PROLOGUE;
  u32 i0;
  i0 = 0u;
  FUNC_EPILOGUE;
  return i0;
}
''',

'''
u32 _load_frozen_module(u32 str_offset, u32 len, u32 content_offset, u32 content_size);
static u32 export_vm_load_frozen_module(u32 p0, u32 p1, u32 p2, u32 p3) {
  return _load_frozen_module(p0, p1, p2, p3);
}
'''
)

def patch_micropython(data, origin, patch):
    index = data.find(origin)
    print(index)
    assert index >= 0
    data = data[:index] + patch + data[index+len(origin):]
    return data

with open('micropython.c.bin', 'r') as f:
    data = f.read()
    origin, patch = header_patch
    data = patch_micropython(data, origin, patch)

    origin, patch = patch_set_jmp
    data = patch_micropython(data, origin, patch)

    origin, patch = patch_init
    data = patch_micropython(data, origin, patch)

    origin, patch = pathc_setjmp
    data = patch_micropython(data, origin, patch)

    origin, patch = nlr_pop_patch
    data = patch_micropython(data, origin, patch)

    # origin, patch = frozen_stat_patch
    # data = patch_micropython(data, origin, patch)

    # origin, patch = vm_load_frozen_module_patch
    # data = patch_micropython(data, origin, patch)

    origin = 'wasm_rt_allocate_memory((&M0), 1, 65536);'
    if data.find(origin) < 0:
      origin = 'wasm_rt_allocate_memory((&M0), 2, 65536);'
    if data.find(origin) < 0:
      origin = 'wasm_rt_allocate_memory((&M0), 1, 528);'
    patch = 'memset(&M0, 0, sizeof(M0));wasm_rt_allocate_memory((&M0), 2, PYTHON_VM_MAX_MEMORY_SIZE/65536);'
    data = patch_micropython(data, origin, patch)
    data = data.replace('__fpclassifyl', '____fpclassifyl')

with open('micropython.c.bin', 'w') as f:
    f.write(data)
