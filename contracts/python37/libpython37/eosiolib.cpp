#if 0
#include "datastream.hpp"
#include "memory.hpp"
#include "privileged.hpp"
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <eosiolib/types.hpp>
#include <eosiolib/system.h>
#include <eosiolib/print.h>

#define PAGE_SIZE 65536U

extern "C" {

WASM_IMPORT void send_deferred(const uint128_t& sender_id, uint64_t payer, const char *serialized_transaction, size_t size, uint32_t replace_existing);
WASM_IMPORT int cancel_deferred(const uint128_t& sender_id);

void _send_deferred(const uint128_t *sender_id, uint64_t payer, const char *serialized_transaction, size_t size, uint32_t replace_existing) {
   send_deferred(*sender_id, payer, serialized_transaction, size, replace_existing);
}

int _cancel_deferred(const uint128_t *sender_id) {
   return cancel_deferred(*sender_id);
}

//void* __dso_handle = 0;

static int pos = 0;
static char *memory_start = nullptr;
static char *code_buffer = nullptr;
#define MAX_MEMORY_SIZE (2*1024*1024)
#define MAX_CODE_SIZE (128*1024)
#define MAX_FROZEN_CODE_SIZE (64*1024)

void *get_current_memory() {
   return memory_start+pos;
}

void *get_code_memory() {
   return memory_start+MAX_MEMORY_SIZE;
}

void *get_frozen_code_memory() {
   return memory_start+MAX_MEMORY_SIZE+MAX_CODE_SIZE;
}

void* malloc(size_t size)
{
//   prints("+++malloc");printi(size);prints("\n");
   void *ptr;
   if (!memory_start) {
      const uint32_t current_pages = __builtin_wasm_current_memory();
      if (current_pages < 48) {
         __builtin_wasm_grow_memory(48 - current_pages);
      }
//      const uint32_t current_pages = __builtin_wasm_current_memory();
      memory_start = (char *)(8*PAGE_SIZE);
   }
   if (size % sizeof(uint32_t) == 0) {
   } else {
      size = (size+sizeof(uint32_t)-1)/sizeof(uint32_t)*sizeof(uint32_t);
   }
   if (pos + size >= MAX_MEMORY_SIZE) {
      return nullptr;
   }
   ptr = &memory_start[pos];
//   printi(pos);prints(" ");printi(size);prints("\n");
   pos += size;
   return ptr;
}

void* calloc(size_t count, size_t size)
{
   void *ptr = malloc(count*size);
   if (!ptr) {
      return nullptr;
   }
   memset(ptr, 0, count*size);
   return ptr;
}

void* realloc(void* ptr, size_t size)
{
   if (size == 0) {
      return nullptr;
   }
   void* new_ptr = calloc(size, 1);
   if (ptr != nullptr) {
      memcpy(new_ptr, ptr, size);
   }
   return new_ptr;
}

void free(void* ptr)
{
//   eosio::memory_heap.free(ptr);
}

}

#if 0
#define EOSIO_ENTRY eosio_system_apply
#include "../eosio.token/eosio.token.cpp"

#undef EOSIO_ENTRY
#define EOSIO_ENTRY eosio_token_apply
#include "../eosio.system/eosio.system.cpp"

#undef EOSIO_ENTRY
#define EOSIO_ENTRY eosio_msig_apply
#include "../eosio.msig/eosio.msig.cpp"
#endif

void *
operator new(std::size_t size)
{
    if (size == 0)
        size = 1;
    return malloc(size);
}

void
operator delete(void* ptr)
{
}

