static void *get_memory_ptr(unsigned int offset, unsigned int size);
void vm_checktime(void);

#include "micropython_vm_config.h"
#include "micropython.c.bin"
#include "vm_api.c"

#include <wasm-rt-impl.h>
#include <vm_api4c.h>

#include <stdlib.h>
#include <stdio.h>
#include <vm_api.h>

uint32_t micropython_in_apply_context(void) {
  return get_vm_api()->is_in_apply_context;
}

uint32_t wasm_rt_call_stack_depth = 0;
uint32_t g_saved_call_stack_depth = 0;
jmp_buf g_jmp_buf;

u32 (*Z_envZ_call_vm_apiZ_iiiii)(u32, u32, u32, u32);

void (*Z_envZ_setjmp_discard_topZ_vv)(void);
u32 (*Z_envZ_vm_load_frozen_moduleZ_iiiii)(u32, u32, u32, u32);
u32 (*Z_envZ_vm_frozen_statZ_ii)(u32);
u32 (*Z_envZ_in_apply_contextZ_iv)(void);

u32 _call_vm_api(u32 function_type, u32 input_offset, u32 input_size, u32 output_offset);


void eosio_assert( uint32_t test, const char* msg );
void setjmp_discard_top(void);
size_t vm_load_frozen_module(const char *str, size_t len, char *content, size_t content_size);
uint32_t vm_frozen_stat(const char *str);

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
  Z_envZ_call_vm_apiZ_iiiii = _call_vm_api;

  Z_envZ_setjmp_discard_topZ_vv = _setjmp_discard_top;
  Z_envZ_vm_load_frozen_moduleZ_iiiii = _load_frozen_module;
  Z_envZ_vm_frozen_statZ_ii = _vm_frozen_stat;
  Z_envZ_in_apply_contextZ_iv = micropython_in_apply_context;

  init_func_types();
  init_globals();
  init_memory();
  init_table();
  init_exports();
  memcpy(&(M0.data[0]), &g1, 4);
  __wasm_call_ctors();
}

void vm_print_stacktrace(void);

static void *get_memory_ptr(uint32_t offset, uint32_t size) {
  int test = offset + size <= M0.size && offset + size >= offset;
  if (!test) {
    vm_print_stacktrace();
    printf("++++++++offset %u, size %u M0.size %u\n", offset, size, M0.size);
  }
  if (!test) {
    eosio_assert(0, "micropython.c: memory access out of bound!");
  }
  return M0.data + offset;
}

static void *_offset_to_ptr(u32 offset, u32 size) {
  return get_memory_ptr(offset, size);
}

static void *_offset_to_char_ptr(u32 offset) {
  return get_memory_ptr(offset, 64);
}

void init_frozen_module(const char *name) {
  size_t size = vm_load_frozen_module(name, strlen(name), NULL, 0);
  u32 init_script_offset = malloc_0(size);
  char *init_script = (char *)get_memory_ptr(init_script_offset, size);
  vm_load_frozen_module(name, strlen(name), init_script, size);
  micropython_init_module_from_mpy_with_name(0, init_script_offset, size);
}

void *micropython_get_memory();
size_t micropython_get_memory_size();

#include <memory.h>
#include <string.h>

void take_snapshot(const char *inital_memory, const char *current_memory) {
    const char *ptr1 = inital_memory;
    const char *ptr2 = current_memory;
    int block_size = 64;

    int initial_memory_size = 64*1024;
    int pos = PYTHON_VM_STACK_SIZE; //do not save stack data as it's temperately
    int total_size = 0;
    while(pos<initial_memory_size) {
        if (memcmp(ptr1+pos, ptr2+pos, block_size) == 0) {
            pos += block_size;
            continue;
        }
        int start = pos;
        pos += block_size;
        while (pos<initial_memory_size) {
            if (memcmp(ptr1+pos, ptr2+pos, block_size) == 0) {
                break;
            }
            pos += block_size;
        }

        int copy_size = pos-start;
        pos += block_size;
        total_size += copy_size;
        printf("++++++++start: %d, size: %d\n", start, copy_size);
    }
    printf("++++++++total_size: %d \n", total_size);
}

void *malloc(size_t size);

int micropython_init() {
  init_vm_api4c();
  set_memory_converter(_offset_to_ptr, _offset_to_char_ptr);
  printf("+++++++_offset_to_ptr %p\n", _offset_to_ptr);

  init();
  // u32 ptr = malloc_0(1);
  // printf("++++++++++++init_frozen_module, current ptr is %u\n", ptr);

  mp_js_init(64*1024);

  int trap_code = wasm_rt_impl_try();
  if (trap_code == 0) {
    micropython_init_frozen_modules();
    // init_frozen_module("_init.mpy");
  // ptr = malloc_0(1);
  // printf("++++++++++++init_frozen_module, current ptr is %u\n", ptr);

    return 1;
  } else {
    printf("++++init_frozen_module:trap code: %d\n", trap_code);
    wasm_rt_on_trap(trap_code);
    return 0;
  }
//  micropython_run_script(script_offset);
  return 1;
}

void setjmp_clear_stack();

void micropython_init_memory(size_t initial_pages) {
  initial_pages = 1;
  wasm_rt_allocate_memory((&M0), initial_pages, PYTHON_VM_MAX_MEMORY_SIZE/65536);
  printf("++++++++++micropython_init_memory: initial_pages %u, M0.size: %u\n", initial_pages, M0.size);
}

int micropython_contract_init(int type, const char *py_src, size_t size) {
  setjmp_clear_stack();

  init_globals();
  set_memory_converter(_offset_to_ptr, _offset_to_char_ptr);

  int trap_code = wasm_rt_impl_try();
  if (trap_code == 0) {
    u32 offset = malloc_0(size);
    char *ptr = (char *)get_memory_ptr(offset, size);
    memcpy(ptr, py_src, size);
  //  printf("++++++++++++memory start %p\n", ptr);
    int ret = micropython_init_module(type, offset, size);
    return ret;
  } else {
    printf("++++micropython_contract_init:trap code: %d\n", trap_code);
    wasm_rt_on_trap(trap_code);
  }
  return 0;
}

void (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(u64, u64, u64);

int micropython_contract_apply(uint64_t receiver, uint64_t code, uint64_t action) {
  setjmp_clear_stack();

  init_globals();
  set_memory_converter(_offset_to_ptr, _offset_to_char_ptr);

  // u32 ptr = malloc_0(1);
  // printf("++++++++++++micropython_contract_apply, current ptr is %ld\n", ptr);
  wasm_rt_call_stack_depth = 0;
  int trap_code = wasm_rt_impl_try();
  if (trap_code == 0) {
    int ret = 1;
    Z_applyZ_vjjj(receiver, code, action);
//    int ret = micropython_apply(receiver, code, action);

    // u32 ptr = malloc_0(1);
    // printf("++++++++++++micropython_contract_apply, current ptr is %u\n", ptr);
    return ret;
  } else {
    printf("++++micropython_contract_apply:trap code: %d\n", trap_code);
    wasm_rt_on_trap(trap_code);
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

