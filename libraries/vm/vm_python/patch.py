import sys

pythonvm_h_patch =r'''
//vm_python.cpp
void vm_load_memory(uint32_t offset_start, uint32_t length);
//eosio_injection.cpp
void vm_checktime();
void *memcpy(void *dest, const void *src, unsigned long n);

#if 100*__GNUC__+__GNUC_MINOR__ >= 303
#define NAN       __builtin_nanf("")
#define INFINITY  __builtin_inff()
#else
#define NAN       (0.0f/0.0f)
#define INFINITY  1e5000f
#endif

double trunc (double a);
double fabs(double x);
'''

patch_head = r'''
#include "pythonvm.h"

#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#define LIKELY(x) __builtin_expect(!!(x), 1)

#define TRAP(x) (wasm_rt_trap(WASM_RT_TRAP_##x), 0)

#define FUNC_PROLOGUE                                            \
  vm_checktime();                                                   \
  if (++wasm_rt_call_stack_depth > WASM_RT_MAX_CALL_STACK_DEPTH) \
    TRAP(EXHAUSTION)

#define FUNC_EPILOGUE --wasm_rt_call_stack_depth

#define UNREACHABLE TRAP(UNREACHABLE)

#define CALL_INDIRECT(table, t, ft, x, ...)          \
  (LIKELY((x) < table.size && table.data[x].func &&  \
          table.data[x].func_type == func_types[ft]) \
       ? ((t)table.data[x].func)(__VA_ARGS__)        \
       : TRAP(CALL_INDIRECT))

#define MEMCHECK(mem, a, t)  \
  if (UNLIKELY((a) + sizeof(t) > mem->size)) TRAP(OOB)

#define DEFINE_LOAD(name, t1, t2, t3)              \
  static inline t3 name(wasm_rt_memory_t* mem, u64 addr) {   \
    MEMCHECK(mem, addr, t1);                       \
    t1 result;                                     \
    vm_load_memory(addr, sizeof(t1));              \
    memcpy(&result, &mem->data[addr], sizeof(t1)); \
    return (t3)(t2)result;                         \
  }
  
#define DEFINE_STORE(name, t1, t2)                           \
  static inline void name(wasm_rt_memory_t* mem, u64 addr, t2 value) { \
    MEMCHECK(mem, addr, t1);                                 \
    vm_load_memory(addr, sizeof(t1));                        \
    t1 wrapped = (t1)value;                                  \
    memcpy(&mem->data[addr], &wrapped, sizeof(t1));          \
  }
'''

patch_end = r'''
/* export: 'apply' */
void (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(u64, u64, u64);
void (*WASM_RT_ADD_PREFIX(Z_callZ_vjjjj))(u64, u64, u64, u64);
u32 (*WASM_RT_ADD_PREFIX(Z_get_current_memory))(void);

static void init_exports(void) {
  /* export: 'apply' */
  WASM_RT_ADD_PREFIX(Z_applyZ_vjjj) = (&apply);
  WASM_RT_ADD_PREFIX(Z_callZ_vjjjj) = (&%s);
  (WASM_RT_ADD_PREFIX(Z_get_current_memory)) = (&%s);
}

void WASM_RT_ADD_PREFIX(python_vm_init)(void) {
  static int init = 0;
  if (!init) {
    init_func_types();
    init_table();
    init_exports();
    init = 1;
  }
  init_globals();
  init_memory();
  memcpy(&(M0.data[0]), &g1, 4);
  %s();
}
'''

def patch_pythonvm_c_bin():
    global patch_end
    with open('pythonvm.c.bin', 'r') as f:
        source = f.read()

    if source.find('void vm_load_memory(uint32_t offset_start, uint32_t length);') >= 0:
        print('pythonvm.c.bin already patched')
        return

    start_index = source.find('static void apply(u64 p0, u64 p1, u64 p2)')
    end_index = source.find('FUNC_EPILOGUE', start_index)
    apply_source = source[start_index:end_index]
    lines = apply_source.split('\n')
    for i in range(len(lines)):
        line = lines[i]
        if line.find('(j0, j1, j2, j3)') > 0:
            end = line.find('(')
            func_call = line[:end].strip()

            line = lines[i+1]
            end = line.find('(')
            func_pythonvm_init = line[:end].strip()

            line = lines[i+2]
            start = line.find('= ')
            end = line.find('(')
            get_current_memory = line[start+2:end]
            break

    patch_end = patch_end%(func_call, get_current_memory, func_pythonvm_init)

    start = source.find('DEFINE_LOAD(i32_load, u32, u32, u32);')
    end = source.rfind('''/* export: 'apply' */\nvoid (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(u64, u64, u64);''')
    source = source[start:end]
    source = source.replace('static void init_globals(void)', 'void init_globals(void)')


    with open('pythonvm.c.bin', 'w') as f:
        f.write(patch_head)
        f.write(source)
        f.write(patch_end)


def patch_pythonvm_h():
    pythonvm_h = open('pythonvm.h', 'r').read()
    if pythonvm_h.find('void vm_checktime(); //eosio_injection.cpp') >= 0:
        print('pythonvm.h already patched')
        return
    insert_pos = pythonvm_h.find('#ifndef WASM_RT_MODULE_PREFIX')


    with open('pythonvm.h', 'w') as f:
        f.write(pythonvm_h[:insert_pos])
        f.write(pythonvm_h_patch)
        f.write(pythonvm_h[insert_pos:])

patch_pythonvm_c_bin()
patch_pythonvm_h()