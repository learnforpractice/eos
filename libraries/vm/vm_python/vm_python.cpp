#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <vm_api/vm_api.h>
#include <chain_api.hpp>

#include <map>
#include <vector>
#include <memory>

#include "wasm-rt-impl.h"
#include "vm_api4c.h"

#include <python_vm_config.h>
#include <stacktrace.h>

#include "vm_python.h"

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef float f32;
typedef double f64;

#ifndef WASM_RT_MODULE_PREFIX
#define WASM_RT_MODULE_PREFIX
#endif

#define WASM_RT_PASTE_(x, y) x ## y
#define WASM_RT_PASTE(x, y) WASM_RT_PASTE_(x, y)
#define WASM_RT_ADD_PREFIX(x) WASM_RT_PASTE(WASM_RT_MODULE_PREFIX, x)


static vm_python_info g_python_info = {};

extern "C" {
   extern wasm_rt_memory_t* get_wasm_rt_memory(void);
   /* export: 'apply' */
   extern void (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(u64, u64, u64);
   /* export: 'call' */
   extern void (*WASM_RT_ADD_PREFIX(Z_callZ_vjjjj))(u64, u64, u64, u64);
   extern u32 (*WASM_RT_ADD_PREFIX(Z_get_current_memory))(void);

   //pythonvm.c.bin
   extern void WASM_RT_ADD_PREFIX(python_vm_init)(void);

   void init_globals(void);

   void python_vm_apply(uint64_t receiver, uint64_t code, uint64_t action) {
       wasm_rt_call_stack_depth = 0;
       init_globals();
      (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(receiver, code, action);
   }

   void python_vm_call(uint64_t func_name, uint64_t receiver, uint64_t code, uint64_t action) {
       wasm_rt_call_stack_depth = 0;
        init_globals();
      (*WASM_RT_ADD_PREFIX(Z_callZ_vjjjj))(func_name, receiver, code, action);
   }

   uint32_t wasm2c_get_current_memory(void) {
       return (*WASM_RT_ADD_PREFIX(Z_get_current_memory))();
   }


   void *offset_to_ptr(u32 offset, u32 size);
   void *offset_to_char_ptr(u32 offset);
   
   /* import: 'env' 'find_frozen_code' */
   extern u32 (*Z_envZ_find_frozen_codeZ_iiiii)(u32, u32, u32, u32);

   void export_vm_apply(uint64_t receiver, uint64_t code, uint64_t action);
   void export_vm_call(uint64_t func_name, uint64_t receiver, uint64_t code, uint64_t action);
   
   uint8_t *vm_grow_memory(uint32_t delta);
   void vm_load_memory(uint32_t offset_start, uint32_t length);



void find_frozen_code(const char *name, const char **code, int *size);
int __find_frozen_code(const char *name, size_t length, char* code, size_t code_size) {
    int size = 0;
    int _is_package = 0;
    const char *_code;
    find_frozen_code(name, &_code, &size);
    if (size < 0) {
        size = -size;
        _is_package = 1;
    }
    if (size == 0 || code_size < size) {
        return 0;
    }
    memcpy(code, _code, size);
    if (_is_package) {
        return -size;
    }
    return size;
}

uint8_t *vm_grow_memory(uint32_t delta) {
    EOSIO_THROW("grow memory not supported in python vm!");
    return nullptr;
}

uint8_t *python_vm_get_memory() {
   return g_python_info.memory_start;
}

size_t python_vm_get_memory_size() {
    return g_python_info.memory_size;
}

void vm_load_memory(uint32_t offset_start, uint32_t length) {
    g_python_info.load_memory(offset_start, length);
}

static void *offset_to_ptr_s(u32 offset, u32 size) {
    wasm_rt_memory_t *memory = get_wasm_rt_memory();
    vm_load_memory(offset, size);
    return memory->data + offset;
}

#define MAX_C_STRING_SIZE 4096
static void *offset_to_char_ptr_s(u32 offset) {
    wasm_rt_memory_t *memory = get_wasm_rt_memory();
    int count = 0;
    for (int i=offset;i<memory->size;i++) {
        vm_load_memory(i, 1);
        if (memory->data[i] == '\0') {
            return memory->data + offset; 
        }
        count += 1;
        EOSIO_ASSERT(count < MAX_C_STRING_SIZE, "c string to large then 1024 bytes long");
    }
    EOSIO_ASSERT(0, "not a valid c string!");
    return NULL;
}

u32 _find_frozen_code(u32 name_offset, u32 name_length, u32 code_offset, u32 code_size) {
    const char *name = (const char *)offset_to_ptr(name_offset, name_length);
    char *code = (char *)offset_to_ptr(code_offset, code_size);
//    printf("import %s\n", name);
    return __find_frozen_code(name, name_length, code, code_size);
}

extern "C" void print_function(const char *name, int line, int stack_pos) {
   printf("++++++++++++++%s %d %d\n", name, line, stack_pos);
}

extern "C" void vm_print_stacktrace(void) {
   print_stacktrace();
}


int vm_python2_setcode(uint64_t account) {
   return 0;
}

uint16_t vm_python_get_version() {
    return (uint16_t)(1<<8) | (uint16_t)0;
}

void vm_python2_init(struct vm_python_info *python_info) {
//   printf("+++++++vm_python2_init\n");
   EOSIO_ASSERT(python_info->memory_size%VM_PAGE_SIZE == 0, "wrong memory size");

   python_info->apply = python_vm_apply;
   python_info->call = python_vm_call;
   python_info->vmtype = 1;
   python_info->vmversion = 0;

   g_python_info = *python_info;

   set_memory_converter(offset_to_ptr_s, offset_to_char_ptr_s);

   Z_envZ_find_frozen_codeZ_iiiii = _find_frozen_code;
   init_vm_api4c();

   WASM_RT_ADD_PREFIX(python_vm_init)();
}

void vm_python2_deinit() {
   printf("vm_example: deinit\n");
}

}
