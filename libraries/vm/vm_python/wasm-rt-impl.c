/*
 * Copyright 2018 WebAssembly Community Group participants
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wasm-rt-impl.h"

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vm_api/vm_api.h>
#include <python_vm_config.h>

#define PAGE_SIZE 65536U
typedef struct FuncType {
  wasm_rt_type_t* params;
  wasm_rt_type_t* results;
  uint32_t param_count;
  uint32_t result_count;
} FuncType;

uint32_t wasm_rt_call_stack_depth;
uint32_t g_saved_call_stack_depth;

jmp_buf g_jmp_buf;
FuncType* g_func_types;
uint32_t g_func_type_count;

void vm_print_stacktrace(void);

void wasm_rt_trap(wasm_rt_trap_t code) {
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

static bool func_types_are_equal(FuncType* a, FuncType* b) {
  if (a->param_count != b->param_count || a->result_count != b->result_count)
    return 0;
  int i;
  for (i = 0; i < a->param_count; ++i)
    if (a->params[i] != b->params[i])
      return 0;
  for (i = 0; i < a->result_count; ++i)
    if (a->results[i] != b->results[i])
      return 0;
  return 1;
}

uint32_t wasm_rt_register_func_type(uint32_t param_count,
                                    uint32_t result_count,
                                    ...) {
  FuncType func_type;
  func_type.param_count = param_count;
  func_type.params = malloc(param_count * sizeof(wasm_rt_type_t));
  func_type.result_count = result_count;
  func_type.results = malloc(result_count * sizeof(wasm_rt_type_t));

  va_list args;
  va_start(args, result_count);

  uint32_t i;
  for (i = 0; i < param_count; ++i)
    func_type.params[i] = va_arg(args, wasm_rt_type_t);
  for (i = 0; i < result_count; ++i)
    func_type.results[i] = va_arg(args, wasm_rt_type_t);
  va_end(args);

  for (i = 0; i < g_func_type_count; ++i) {
    if (func_types_are_equal(&g_func_types[i], &func_type)) {
      free(func_type.params);
      free(func_type.results);
      return i + 1;
    }
  }

  uint32_t idx = g_func_type_count++;
  g_func_types = realloc(g_func_types, g_func_type_count * sizeof(FuncType));
  g_func_types[idx] = func_type;
  return idx + 1;
}

//vm_python.cpp
//uint8_t *python_vm_allocate_memory(uint32_t initial_pages, uint32_t max_pages);
uint8_t *python_vm_get_memory();
size_t python_vm_get_memory_size();

uint8_t *vm_grow_memory(uint32_t delta);

static wasm_rt_memory_t* g_memory = NULL;
void wasm_rt_allocate_memory(wasm_rt_memory_t* memory,
                             uint32_t initial_pages,
                             uint32_t max_pages) {
//  if (g_memory) {
//    return;
//  }

  vmdlog("initial_pages %d, max_pages %d\n", initial_pages, max_pages);
//  initial_pages = 160;//10M
  size_t memory_size = python_vm_get_memory_size();
  memory->pages = memory_size/PAGE_SIZE;//initial_pages;
  memory->max_pages = memory_size/PAGE_SIZE;//max_pages;
  memory->size = memory_size;//PYTHON_VM_PAGES * PAGE_SIZE;
  memory->data = python_vm_get_memory();
//  memory->data = calloc(memory->size, 1);
  g_memory = memory;
}

extern wasm_rt_memory_t* get_wasm_rt_memory(void) {
    return g_memory;
}

uint32_t wasm_rt_grow_memory(wasm_rt_memory_t* memory, uint32_t delta) {
  EOSIO_ASSERT(0, "grow_memory should never be called!");
  vmdlog("delta %d\n", delta);
//  return (uint32_t)-1;
  uint32_t old_pages = memory->pages;
  uint32_t new_pages = memory->pages + delta;
  if (new_pages < old_pages || new_pages > memory->max_pages) {
    return (uint32_t)-1;
  }
  memory->pages = new_pages;
  memory->size = new_pages * PAGE_SIZE;
  memory->data = vm_grow_memory(delta);
//  memory->data = realloc(memory->data, memory->size);
  memset(memory->data + old_pages * PAGE_SIZE, 0, delta * PAGE_SIZE);
  return old_pages;
}

void wasm_rt_allocate_table(wasm_rt_table_t* table,
                            uint32_t elements,
                            uint32_t max_elements) {
  table->size = elements;
  table->max_size = max_elements;
  table->data = calloc(table->size, sizeof(wasm_rt_elem_t));
}
