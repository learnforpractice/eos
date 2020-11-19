#include "micropython_vm_config.h"
#include "micropython.c.bin"
#include <wasm-rt-impl.h>

uint32_t wasm_rt_call_stack_depth = 0;
uint32_t g_saved_call_stack_depth = 0;
jmp_buf g_jmp_buf;

u32 (*Z_envZ_memsetZ_iiii)(u32, u32, u32);
u32 (*Z_envZ_memcpyZ_iiii)(u32, u32, u32);
u32 (*Z_envZ_memmoveZ_iiii)(u32, u32, u32);
u32 (*Z_envZ_call_vm_apiZ_iiiii)(u32, u32, u32, u32);
u64 (*Z_envZ_s2nZ_jii)(u32, u32);
u32 (*Z_envZ_n2sZ_ijii)(u64, u32, u32);

void (*Z_envZ_setjmp_discard_topZ_vv)(void);
u32 (*Z_envZ_vm_load_frozen_moduleZ_iiiii)(u32, u32, u32, u32);
u32 (*Z_envZ_vm_frozen_statZ_ii)(u32);

uint64_t s2n( const char *str, size_t str_size );
int n2s(uint64_t value, char *str, size_t str_size);
u32 _call_vm_api(u32 function_type, u32 input_offset, u32 input_size, u32 output_offset);


void eosio_assert( uint32_t test, const char* msg );
void setjmp_discard_top(void);
size_t vm_load_frozen_module(const char *str, size_t len, char *content, size_t content_size);
uint32_t vm_frozen_stat(const char *str);

static u64 _s2n(u32 str_offset, u32 str_size) {
  char *str = get_memory_ptr(str_offset, str_size);
  return s2n(str, str_size);
}

static u32 _n2s(u64 n, u32 str_offset, u32 str_size) {
  char *str = get_memory_ptr(str_offset, str_size);
  u32 ret = n2s(n, str, str_size);
//  printf("+++++++++++++_n2s: %s %d\n", str, ret);
  return ret;
}

//void * memset ( void * ptr, int value, size_t num );

static u32 _memset(u32 ptr_offset, u32 value, u32 num) {
  char *ptr = get_memory_ptr(ptr_offset, num);
  memset(ptr, value, num);
  return ptr_offset;
}

//void * memcpy ( void * destination, const void * source, size_t num );
static u32 _memcpy(u32 dest_offset, u32 src_offset, u32 num) {
  char *src_ptr = get_memory_ptr(src_offset, num);
  char *dest_ptr = get_memory_ptr(dest_offset, num);
  memcpy(dest_ptr, src_ptr, num);
  return dest_offset;
}

//void * memmove ( void * destination, const void * source, size_t num )
static u32 _memmove(u32 dest_offset, u32 src_offset, u32 num) {
  char *src_ptr = get_memory_ptr(src_offset, num);
  char *dest_ptr = get_memory_ptr(dest_offset, num);
  memmove(dest_ptr, src_ptr, num);
  return dest_offset;
}

static void _setjmp_discard_top() {
  setjmp_discard_top();
}

u32 _load_frozen_module(u32 str_offset, u32 len, u32 content_offset, u32 content_size) {
  char *src = get_memory_ptr(str_offset, len);
  char *content = get_memory_ptr(content_offset, content_size);
  return vm_load_frozen_module(src, len, content, content_size);
}

u32 _vm_frozen_stat(u32 str_offset) {
  char *src = get_memory_ptr(str_offset, 64);
  return vm_frozen_stat(src);
}

void WASM_RT_ADD_PREFIX(init)(void) {
  Z_envZ_memsetZ_iiii = _memset;
  Z_envZ_memcpyZ_iiii = _memcpy;
  Z_envZ_memmoveZ_iiii = _memmove;

  Z_envZ_call_vm_apiZ_iiiii = _call_vm_api;

  Z_envZ_s2nZ_jii = _s2n;
  Z_envZ_n2sZ_ijii = _n2s;

  Z_envZ_setjmp_discard_topZ_vv = _setjmp_discard_top;
  Z_envZ_vm_load_frozen_moduleZ_iiiii = _load_frozen_module;
  Z_envZ_vm_frozen_statZ_ii = _vm_frozen_stat;

  init_func_types();
  init_globals();
  init_memory();
  init_table();
  init_exports();
  memcpy(&(M0.data[0]), &g1, 4);
  __wasm_call_ctors();
}

void vm_print_stacktrace(void);

void *get_memory_ptr(uint32_t offset, uint32_t size) {
  int test = offset + size <= M0.size && offset + size >= offset;
  // if (!test) {
  //   vm_print_stacktrace();
  // }
  eosio_assert(test, "memory access out of bound!");
  return M0.data + offset;
}

static void *offset_to_ptr(u32 offset, u32 size) {
  return get_memory_ptr(offset, size);
}

static void *offset_to_char_ptr(u32 offset) {
  return get_memory_ptr(offset, 64);
}

#include <vm_api4c.h>

void init_frozen_module(const char *name) {
  size_t size = vm_load_frozen_module(name, strlen(name), NULL, 0);
  u32 init_script_offset = malloc(size);
  char *init_script = (char *)get_memory_ptr(init_script_offset, size);
  vm_load_frozen_module(name, strlen(name), init_script, size);
  micropython_init_module_from_mpy_with_name(0, init_script_offset, size);
}

int micropython_init() {
  init_vm_api4c();
  set_memory_converter(offset_to_ptr, offset_to_char_ptr);

  init();
  mp_js_init(64*1024);

  init_frozen_module("init.mpy");

//  micropython_run_script(script_offset);
  return 1;
}

void setjmp_clear_stack();
#define EOSIO_THROW(msg) eosio_assert(0, msg)

void wasm_rt_on_trap(wasm_rt_trap_t code) {
//   vm_print_stacktrace();
   wasm_rt_call_stack_depth = 0;
   switch (code) {
      case WASM_RT_TRAP_NONE:
         EOSIO_THROW("vm no error");
         break;
      case WASM_RT_TRAP_OOB:
         EOSIO_THROW("vm error out of bounds");
         break;
      case WASM_RT_TRAP_INT_OVERFLOW:
         EOSIO_THROW("vm error int overflow");
         break;
      case WASM_RT_TRAP_DIV_BY_ZERO:
         EOSIO_THROW("vm error divide by zeror");
         break;
      case WASM_RT_TRAP_INVALID_CONVERSION:
         EOSIO_THROW("vm error invalid conversion");
         break;
      case WASM_RT_TRAP_UNREACHABLE:
         EOSIO_THROW("vm error unreachable");
         break;
      case WASM_RT_TRAP_CALL_INDIRECT:
         EOSIO_THROW("vm error call indirect");
         break;
      case WASM_RT_TRAP_EXHAUSTION:
         EOSIO_THROW("vm error exhaustion");
         break;
      default:
         EOSIO_THROW("vm unknown error");
         break;
   }
}

void micropython_init_memory(size_t initial_pages) {
  wasm_rt_allocate_memory((&M0), initial_pages, PYTHON_VM_MAX_MEMORY_SIZE/65536);
}


int micropython_contract_init(int type, const char *py_src, size_t size) {
  setjmp_clear_stack();

  init_globals();
  
  int trap_code = wasm_rt_impl_try();
  if (trap_code == 0) {
    u32 offset = malloc(size);
    char *ptr = (char *)get_memory_ptr(offset, size);
    memcpy(ptr, py_src, size);
  //  printf("++++++++++++memory start %p\n", ptr);
    int ret = micropython_init_module(type, offset, size);
    return ret;
  } else {
    printf("++++micropython_contract_init:trap code: %d\n", trap_code);
    wasm_rt_on_trap((wasm_rt_trap_t)trap_code);
  }
  return 0;
}

int micropython_contract_apply(uint64_t receiver, uint64_t code, uint64_t action) {
  setjmp_clear_stack();

  init_globals();

  wasm_rt_call_stack_depth = 0;
  int trap_code = wasm_rt_impl_try();
  if (trap_code == 0) {
    return micropython_apply(receiver, code, action);
  } else {
    printf("++++micropython_contract_apply:trap code: %d\n", trap_code);
    wasm_rt_on_trap((wasm_rt_trap_t)trap_code);
    return trap_code;
  }
  return 0;
}

void *micropython_get_memory() {
  return M0.data;
}

size_t micropython_get_memory_size() {
  return M0.size;
}

size_t micropython_backup_memory(void *backup, size_t size) {
  size_t copy_size = 0;
  if (size > M0.size) {
    copy_size = M0.size;
  } else {
    copy_size = size;
  }
  memcpy(backup, M0.data, copy_size);
  return copy_size;
}

size_t micropython_restore_memory(void *backup, size_t size) {
  size_t copy_size = 0;
  if (size > M0.size) {
    copy_size = M0.size;
  } else {
    copy_size = size;
  }

  memcpy(M0.data, backup, copy_size);
  return copy_size;
}

