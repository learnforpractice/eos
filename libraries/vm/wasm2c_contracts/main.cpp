#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <array>
#include <map>

#include "eosio.system.h"
#include <vm_api/vm_api.h>
#include "wasm-rt-impl.h"
#include <vm_api4c.h>

using namespace std;

extern "C" {
    void WASM_RT_ADD_PREFIX(eosio_native_init)(void);
    wasm_rt_memory_t* eosio_native_get_memory();
    void eosio_native_init_globals();
    u32 eosio_native_execute(u32 trx_offset, u32 trx_size, u32 sender_offset, u32 sender_size);
    u32 eosio_native_malloc(u32 size);
}

void vm_on_trap(wasm_rt_trap_t code) {
   get_vm_api()->eosio_assert(0, "vm runtime error");
   switch (code) {
      case WASM_RT_TRAP_NONE:
         get_vm_api()->eosio_assert(0, "vm no error");
         break;
      case WASM_RT_TRAP_OOB:
         get_vm_api()->eosio_assert(0, "vm error out of bounds");
         break;
      case WASM_RT_TRAP_INT_OVERFLOW:
         get_vm_api()->eosio_assert(0, "vm error int overflow");
         break;
      case WASM_RT_TRAP_DIV_BY_ZERO:
         get_vm_api()->eosio_assert(0, "vm error divide by zeror");
         break;
      case WASM_RT_TRAP_INVALID_CONVERSION:
         get_vm_api()->eosio_assert(0, "vm error invalid conversion");
         break;
      case WASM_RT_TRAP_UNREACHABLE:
         get_vm_api()->eosio_assert(0, "vm error unreachable");
         break;
      case WASM_RT_TRAP_CALL_INDIRECT:
         get_vm_api()->eosio_assert(0, "vm error call indirect");
         break;
      case WASM_RT_TRAP_EXHAUSTION:
         get_vm_api()->eosio_assert(0, "vm error exhaustion");
         break;
      default:
         get_vm_api()->eosio_assert(0, "vm unknown error");
         break;
   }
}

static void *offset_to_ptr_s(u32 offset, u32 size) {
    wasm_rt_memory_t *memory = eosio_native_get_memory();
//    printf("++++++offset %u, size %u\n", offset, size);
    get_vm_api()->eosio_assert(memory != nullptr, "memory should not be null");
    get_vm_api()->eosio_assert(offset + size <= memory->size && offset + size >= offset, "memory access out of bounds");
    return memory->data + offset;
}

static void *offset_to_char_ptr_s(u32 offset) {
    wasm_rt_memory_t *memory = eosio_native_get_memory();
    get_vm_api()->eosio_assert(memory != nullptr, "memory should not be null");
    for (int i=offset;i<memory->size;i++) {
        if (memory->data[i] == '\0') {
            return memory->data + offset; 
        }
    }
    get_vm_api()->eosio_assert(0, "not a valid c string!");
    return NULL;
}

extern "C" void sandboxed_contracts_init() {
    wasm2c_set_memory_converter(offset_to_ptr_s, offset_to_char_ptr_s);
    WASM_RT_ADD_PREFIX(eosio_native_init)();
}