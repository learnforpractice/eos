#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <eosiolib_native/vm_api.h>
#include <chain_api.hpp>

#include "vm_memory.h"

#include <map>
#include <vector>
#include <memory>

#include "wasm-rt-impl.h"
#include "vm_api4c.h"

#include <python_vm_config.h>
#include <stacktrace.h>


#define PAGE_SIZE (65536)

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


using namespace pythonvm;

struct vm_state_backup {
   std::vector<memory_segment>                                 memory_backup;
   int contract_memory_start;
   int contract_memory_end;
};

static std::map<std::array<uint8_t,32>, std::shared_ptr<struct vm_state_backup>>      _contract_state_backup;

vm_memory *_vm_memory = nullptr;


//std::map<fc::sha256, std::shared_ptr<struct vm_state_backup>>    _contract_state_backup2;

extern "C" {
   /* export: 'apply' */
   extern void (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(u64, u64, u64);
   /* export: 'call' */
   extern void (*WASM_RT_ADD_PREFIX(Z_callZ_vjjjj))(u64, u64, u64, u64);
   extern void (*WASM_RT_ADD_PREFIX(Z_python_initZ_vv))(void);
   //pythonvm.c.bin
   extern void WASM_RT_ADD_PREFIX(init)(void);

   void export_vm_apply(uint64_t receiver, uint64_t code, uint64_t action) {
      (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(receiver, code, action);
   }

   void export_vm_call(uint64_t func_name, uint64_t receiver, uint64_t code, uint64_t action) {
      (*WASM_RT_ADD_PREFIX(Z_callZ_vjjjj))(func_name, receiver, code, action);
   }

   void *offset_to_ptr(u32 offset, u32 size);
   void *offset_to_char_ptr(u32 offset);
   
   /* import: 'env' 'find_frozen_code' */
   u32 (*Z_envZ_find_frozen_codeZ_iiiii)(u32, u32, u32, u32);

   void export_vm_apply(uint64_t receiver, uint64_t code, uint64_t action);
   void export_vm_call(uint64_t func_name, uint64_t receiver, uint64_t code, uint64_t action);
   void pythonvm_get_memory(char **start, uint32_t *size);
   
   uint8_t *python_vm_allocate_memory(uint32_t initial_pages, uint32_t max_pages);
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



extern wasm_rt_memory_t* get_wasm_rt_memory(void);

uint8_t *python_vm_allocate_memory(uint32_t initial_pages, uint32_t max_pages) {
  if (_vm_memory) {
      return (uint8_t *)_vm_memory->data.data();
  }

  _vm_memory = new vm_memory(initial_pages, max_pages);

  vmdlog("initial_pages %d, max_pages %d\n", initial_pages, max_pages);

  _vm_memory->data.resize(initial_pages * PAGE_SIZE);
  _vm_memory->base_address = _vm_memory->data.data();
  memset(_vm_memory->data.data(), 0,  _vm_memory->data.size());
  return (uint8_t *)_vm_memory->data.data();
}

uint8_t *vm_grow_memory(uint32_t delta) {
    get_vm_api()->eosio_assert(0, "grow memory not supported in python vm!");
}

void vm_load_memory(uint32_t offset_start, uint32_t length) {
    _vm_memory->load_data_to_writable_memory(offset_start, length);
}

static void *offset_to_ptr_s(u32 offset, u32 size) {
    wasm_rt_memory_t *memory = get_wasm_rt_memory();
    vm_load_memory(offset, size);
//    printf("++++++offset %u, size %u\n", offset, size);
//    get_vm_api()->eosio_assert(offset + size <= memory->size && offset + size >= offset, "memory access out of bounds");
    return memory->data + offset;
}

static void *offset_to_char_ptr_s(u32 offset) {
    wasm_rt_memory_t *memory = get_wasm_rt_memory();
    for (int i=offset;i<memory->size;i++) {
        vm_load_memory(i, 1);
        if (memory->data[i] == '\0') {
            return memory->data + offset; 
        }
    }
    get_vm_api()->eosio_assert(0, "not a valid c string!");
    return NULL;
}

void pythonvm_get_memory(char **start, uint32_t *size) {
    wasm_rt_memory_t *memory = get_wasm_rt_memory();
    *start = (char *)memory->data;
    *size = memory->size;
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

void take_snapshoot(std::array<uint8_t,32>& code_id) {
   char *mem_start;
   uint32_t vm_memory_size;
   pythonvm_get_memory(&mem_start, &vm_memory_size);
   std::shared_ptr<vm_state_backup> backup = std::make_shared<vm_state_backup>();

   int total_count = 0;

   int pos = PYTHON_VM_STACK_SIZE/sizeof(uint64_t);
   uint64_t *ptr1 = (uint64_t *)_vm_memory->data_backup.data();
   uint64_t *ptr2 = (uint64_t *)mem_start;

   int contract_mem_start = 0;
   int contract_mem_end = 0;
   get_vm_api()->get_copy_memory_range(&contract_mem_start, &contract_mem_end);

//   vmdlog("++++contract_mem_start %d, contract_mem_end %d, vm_memory_size %d\n", contract_mem_start, contract_mem_end, vm_memory_size);
   get_vm_api()->eosio_assert(contract_mem_start > 0 && contract_mem_start<vm_memory_size, "bad start contract memory");
   get_vm_api()->eosio_assert(contract_mem_end > 0 && contract_mem_end<vm_memory_size, "bad end contract memory");
   get_vm_api()->eosio_assert(contract_mem_start < contract_mem_end, "bad memory range");

   contract_mem_start = contract_mem_start/8*8;
   contract_mem_end = (contract_mem_end+7)/8*8;

   vm_memory_size = contract_mem_start;
   vm_memory_size/=sizeof(uint64_t);

   //save diff memory
   while(pos<vm_memory_size) {
      if (ptr1[pos] == ptr2[pos]) {
         pos += 1;
         continue;
      }
      int start = pos;
      total_count += 1;
      pos += 1;
      while (pos<vm_memory_size && ptr1[pos] != ptr2[pos]) {
         pos += 1;
         total_count += 1;
      }
      memory_segment segment;
      segment.offset = start*sizeof(uint64_t);
      segment.data.resize((pos-start)*sizeof(uint64_t));
      memcpy(segment.data.data(), &ptr2[start], (pos-start)*sizeof(uint64_t));
      backup->memory_backup.emplace_back(std::move(segment));

      pos += 1;
   }

{
   memory_segment segment;
   segment.offset = contract_mem_start;
   segment.data.resize(contract_mem_end-contract_mem_start, 0x00);
   memcpy(segment.data.data(), (char *)ptr2 + contract_mem_start, contract_mem_end-contract_mem_start);
   backup->memory_backup.emplace_back(std::move(segment));
}
   backup->contract_memory_start = contract_mem_start;
   backup->contract_memory_end = contract_mem_end;

   _vm_memory->segments = &backup->memory_backup;
   _vm_memory->malloc_memory_start = contract_mem_end;

   _contract_state_backup[code_id] = backup;
}

int vm_python2_apply(uint64_t receiver, uint64_t account, uint64_t act) {
   std::array<uint8_t,32> code_id;
   get_chain_api()->get_code_id(receiver, code_id.data(), 32);

   if (_vm_memory->counter == 0xffffffff) {
      //reset counter make IsWriteMemoryInUse function properly 
      memset(_vm_memory->in_use.data(), 0, _vm_memory->in_use.size()*sizeof(_vm_memory->in_use[0]));
      memset(_vm_memory->segments_cache.data(), 0, _vm_memory->segments_cache.size()*sizeof(_vm_memory->segments_cache[0]));
//            memory->segments_cache.assign(memory->segments_cache.size(), {0, 0});
      _vm_memory->counter = 1;
   } else {
      _vm_memory->counter += 1;
   }


   auto itr = _contract_state_backup.find(code_id);
   if (itr == _contract_state_backup.end()) {
      _vm_memory->init_smart_contract = true;
      char *mem_start;
      uint32_t vm_memory_size;
      pythonvm_get_memory(&mem_start, &vm_memory_size);
      get_vm_api()->eosio_assert(vm_memory_size == _vm_memory->data_backup.size(), "something went wrong, memory size not the same!");
      memcpy(mem_start, _vm_memory->data_backup.data(), vm_memory_size);
      //do not allow access apply context in when python contract is initializing
      get_vm_api()->allow_access_apply_context = false;
//      export_vm_call(1, receiver, account, act);
      export_vm_call(1, receiver, account, 1);
      get_vm_api()->allow_access_apply_context = true;
      take_snapshoot(code_id);
   } else {
      _vm_memory->segments = &itr->second->memory_backup;
      _vm_memory->malloc_memory_start = itr->second->contract_memory_end;
   }
   _vm_memory->init_smart_contract = false;

   export_vm_apply(receiver, account, act);
   return 1;
}

int vm_python2_call(uint64_t account, uint64_t func) {
   printf("+++++vm_example: call\n");
   return 0;
}

int vm_python2_load(uint64_t account) {
   return 0;
}

int vm_python2_unload(uint64_t account) {
   return 0;
}

void vm_python2_init() {
   static int initialized = 0;
   printf("+++++++vm_python2_init %d\n", initialized);
   if (initialized) {
      return;
   }
   initialized = 1;

   set_memory_converter(offset_to_ptr_s, offset_to_char_ptr_s);

   Z_envZ_find_frozen_codeZ_iiiii = _find_frozen_code;
   init_vm_api4c();

   WASM_RT_ADD_PREFIX(init)();

   (*WASM_RT_ADD_PREFIX(Z_python_initZ_vv))();

   _vm_memory->init_smart_contract = true;
   _vm_memory->counter = 1;

//   pythonvm_get_memory(&mem_start, &size);
//   _vm_memory->data_backup.resize(size);
//   memcpy(_vm_memory->data_backup.data(), mem_start, size);

   _vm_memory->backup_memory();
   _vm_memory->init_cache();
//   export_vm_call(0, 0, 0, 0);
}

void vm_python2_deinit() {
   printf("vm_example: deinit\n");
}

}
