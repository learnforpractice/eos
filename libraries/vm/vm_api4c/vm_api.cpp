#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wasm-rt.h"
#include <eosiolib/types.h>
#include <eosiolib_native/vm_api.h>
#include <chain_api.hpp>

#include "src/interp.h"

using namespace wabt::interp;

#define PAGE_SIZE (65536)

#ifndef WASM_RT_MODULE_PREFIX
#define WASM_RT_MODULE_PREFIX
#endif

#define WASM_RT_PASTE_(x, y) x ## y
#define WASM_RT_PASTE(x, y) WASM_RT_PASTE_(x, y)
#define WASM_RT_ADD_PREFIX(x) WASM_RT_PASTE(WASM_RT_MODULE_PREFIX, x)

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

Memory *_vm_memory = nullptr;

extern "C" {
void *offset_to_ptr(u32 offset, u32 size);
void *offset_to_char_ptr(u32 offset);

/* import: 'env' 'abort' */
void (*Z_envZ_abortZ_vv)(void);
/* import: 'env' 'memcpy' */
u32 (*Z_envZ_memcpyZ_iiii)(u32, u32, u32);
/* import: 'env' 'memmove' */
u32 (*Z_envZ_memmoveZ_iiii)(u32, u32, u32);
/* import: 'env' 'memset' */
u32 (*Z_envZ_memsetZ_iiii)(u32, u32, u32);
/* import: 'env' 'printi' */
void (*Z_envZ_printiZ_vj)(u64);
/* import: 'env' 'printn' */
void (*Z_envZ_printnZ_vj)(u64);
/* import: 'env' 'prints' */
void (*Z_envZ_printsZ_vi)(u32);
/* import: 'env' 'prints_l' */
void (*Z_envZ_prints_lZ_vii)(u32, u32);
/* import: 'env' 'printui' */
void (*Z_envZ_printuiZ_vj)(u64);

static void _abort() {
    get_vm_api()->eosio_abort();
}

static u32 _memcpy(u32 dest_offset, u32 src_offset, u32 size) {
    char *dest = (char *)offset_to_ptr(dest_offset, size);
    char *src = (char *)offset_to_ptr(src_offset, size);
    ::memcpy(dest, src, size);
    return dest_offset;
}

static u32 _memmove(u32 dest_offset, u32 src_offset, u32 size) {
    char *dest = (char *)offset_to_ptr(dest_offset, size);
    char *src = (char *)offset_to_ptr(src_offset, size);
    ::memmove(dest, src, size);
    return dest_offset;
}

static u32 _memset(u32 ptr_offset, u32 v, u32 count) {
    char *ptr = (char *)offset_to_ptr(ptr_offset, count);
    ::memset(ptr, v, count);
    return ptr_offset;
}

/*
static void _printi(u64 v) {
    get_vm_api()->printi(v);
}

static void _prints(u32 s_offset) {
    char *s = (char *)offset_to_char_ptr(s_offset);
    get_vm_api()->prints(s);
}
*/

static void _printn(u64 n) {
    get_vm_api()->printn(n);
}



static void _prints_l(u32 s_offset, u32 size) {
    char *s = (char *)offset_to_ptr(s_offset, size);
    get_vm_api()->prints_l(s, size);
}

static void _printui(u64 u) {
    get_vm_api()->printui(u);
}

uint8_t *vm_allocate_memory(uint32_t initial_pages, uint32_t max_pages) {
  if (_vm_memory) {
      return (uint8_t *)_vm_memory->data.data();
  }

  wabt::Limits limits(initial_pages, max_pages);
  _vm_memory = new Memory(limits);

  vmdlog("initial_pages %d, max_pages %d\n", initial_pages, max_pages);

  _vm_memory->data.resize(initial_pages * PAGE_SIZE);
  _vm_memory->base_address = _vm_memory->data.data();
  memset(_vm_memory->data.data(), 0,  _vm_memory->data.size());
  return (uint8_t *)_vm_memory->data.data();
}

uint8_t *vm_grow_memory(uint32_t delta) {
  uint64_t old_pages = _vm_memory->page_limits.initial;
  uint64_t new_pages = _vm_memory->page_limits.initial + delta;
  if (new_pages < old_pages || new_pages > _vm_memory->page_limits.max) {
      get_vm_api()->eosio_assert(0, "memory exceeded in grow!");
  }
  _vm_memory->page_limits.initial = new_pages;
  _vm_memory->data.resize(new_pages * PAGE_SIZE);
  _vm_memory->base_address = _vm_memory->data.data();
  memset(_vm_memory->data.data() + old_pages * PAGE_SIZE, 0, delta * PAGE_SIZE);
  return (uint8_t *)_vm_memory->data.data();
}

void vm_load_memory(uint32_t offset_start, uint32_t length) {
    LoadDataToWritableMemory(_vm_memory, offset_start, length);
}

//vm_python2.cpp
void vm_load_memory(uint32_t offset_start, uint32_t length);

void pythonvm_get_memory(char **start, uint32_t *size);

u32 _get_code_size(u64 account);
u32 _get_code(u64 account, u32 memory_offset, u32 buffer_size);
u64 _s2n(u32 in_offset, u32 in_len);

/* import: 'env' 'prints' */
void _prints(u32 a);
void _printi(u64 a);

u64 _s2n(u32 in_offset, u32 in_len);
u32 _n2s(u64 n, u32 out_offset, u32 length);


#include "db.cpp"

extern wasm_rt_memory_t* get_wasm_rt_memory(void);

void *offset_to_ptr(u32 offset, u32 size) {
    wasm_rt_memory_t *memory = get_wasm_rt_memory();
    vm_load_memory(offset, size);
//    printf("++++++offset %u, size %u\n", offset, size);
//    get_vm_api()->eosio_assert(offset + size <= memory->size && offset + size >= offset, "memory access out of bounds");
    return memory->data + offset;
}

void pythonvm_get_memory(char **start, uint32_t *size) {
    wasm_rt_memory_t *memory = get_wasm_rt_memory();
    *start = (char *)memory->data;
    *size = memory->size;
}

void *offset_to_char_ptr(u32 offset) {
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

/* import: 'env' 'get_code' */
u32 (*Z_envZ_get_codeZ_ijii)(u64, u32, u32);
/* import: 'env' 'get_code_size' */
u32 (*Z_envZ_get_code_sizeZ_ij)(u64);

extern void WASM_RT_ADD_PREFIX(init)(void);
static void _set_copy_memory_range(u32 start, u32 end) {
    get_vm_api()->set_copy_memory_range((int)start, (int)end);
}

void (*Z_envZ_set_copy_memory_rangeZ_vii)(u32, u32);
u64 (*Z_envZ_s2nZ_jii)(u32, u32);

#include "action.cpp"
#include "chain.cpp"

u32 (*Z_envZ_read_action_dataZ_iii)(u32, u32);
u32 (*Z_envZ_action_data_sizeZ_iv)(void);
void (*Z_envZ_require_recipientZ_vj)(u64);
void (*Z_envZ_require_authZ_vj)(u64);
void (*Z_envZ_require_auth2Z_vjj)(u64, u64);
u32 (*Z_envZ_has_authZ_ij)(u64);
u32 (*Z_envZ_is_accountZ_ij)(u64);
void (*Z_envZ_send_inlineZ_vii)(u32, u32);
void (*Z_envZ_send_context_free_inlineZ_vii)(u32, u32);
u64 (*Z_envZ_publication_timeZ_jv)(void);
u64 (*Z_envZ_current_receiverZ_jv)(void);

/* import: 'env' 'get_active_producers' */
u32 (*Z_envZ_get_active_producersZ_iii)(u32, u32);

//crypto.cpp
/* import: 'env' 'assert_sha256' */
void (*Z_envZ_assert_sha256Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_sha1' */
void (*Z_envZ_assert_sha1Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_sha512' */
void (*Z_envZ_assert_sha512Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_ripemd160' */
void (*Z_envZ_assert_ripemd160Z_viii)(u32, u32, u32);
/* import: 'env' 'sha256' */
void (*Z_envZ_sha256Z_viii)(u32, u32, u32);
/* import: 'env' 'sha1' */
void (*Z_envZ_sha1Z_viii)(u32, u32, u32);
/* import: 'env' 'sha512' */
void (*Z_envZ_sha512Z_viii)(u32, u32, u32);
/* import: 'env' 'ripemd160' */
void (*Z_envZ_ripemd160Z_viii)(u32, u32, u32);
/* import: 'env' 'assert_recover_key' */
void (*Z_envZ_assert_recover_keyZ_viiiii)(u32, u32, u32, u32, u32);
/* import: 'env' 'recover_key' */
u32 (*Z_envZ_recover_keyZ_iiiiii)(u32, u32, u32, u32, u32);

u32 (*Z_envZ_from_base58Z_iiiii)(u32, u32, u32, u32);
u32 (*Z_envZ_to_base58Z_iiiii)(u32, u32, u32, u32);


#include "crypto.cpp"

/* import: 'env' 'check_transaction_authorization' */
u32 (*Z_envZ_check_transaction_authorizationZ_iiiiiii)(u32, u32, u32, u32, u32, u32);
/* import: 'env' 'check_permission_authorization' */
u32 (*Z_envZ_check_permission_authorizationZ_ijjiiiij)(u64, u64, u32, u32, u32, u32, u64);
/* import: 'env' 'get_permission_last_used' */
u64 (*Z_envZ_get_permission_last_usedZ_jjj)(u64, u64);
/* import: 'env' 'get_account_creation_time' */
u64 (*Z_envZ_get_account_creation_timeZ_jj)(u64);
#include "permission.cpp"


//system.cpp
/* import: 'env' 'eosio_assert' */
void (*Z_envZ_eosio_assertZ_vii)(u32, u32);
/* import: 'env' 'eosio_assert_code' */
void (*Z_envZ_eosio_assert_codeZ_vij)(u32, u64);
/* import: 'env' 'eosio_assert_message' */
void (*Z_envZ_eosio_assert_messageZ_viii)(u32, u32, u32);
/* import: 'env' 'current_time' */
u64 (*Z_envZ_current_timeZ_jv)(void);
/* import: 'env' 'call_contract' */
void (*Z_envZ_call_contractZ_vjjjjjii)(u64, u64, u64, u64, u64, u32, u32);
/* import: 'env' 'call_contract_get_results' */
u32 (*Z_envZ_call_contract_get_resultsZ_iii)(u32, u32);

#include "system.cpp"



/* import: 'env' 'send_deferred' */
void (*Z_envZ_send_deferredZ_vijiii)(u32, u64, u32, u32, u32);
/* import: 'env' 'cancel_deferred' */
u32 (*Z_envZ_cancel_deferredZ_ii)(u32);
/* import: 'env' 'read_transaction' */
u32 (*Z_envZ_read_transactionZ_iii)(u32, u32);
/* import: 'env' 'transaction_size' */
u32 (*Z_envZ_transaction_sizeZ_iv)(void);
/* import: 'env' 'tapos_block_num' */
u32 (*Z_envZ_tapos_block_numZ_iv)(void);
/* import: 'env' 'tapos_block_prefix' */
u32 (*Z_envZ_tapos_block_prefixZ_iv)(void);
/* import: 'env' 'expiration' */
u32 (*Z_envZ_expirationZ_iv)(void);
/* import: 'env' 'get_action' */
u32 (*Z_envZ_get_actionZ_iiiii)(u32, u32, u32, u32);
/* import: 'env' 'get_context_free_data' */
u32 (*Z_envZ_get_context_free_dataZ_iiii)(u32, u32, u32);
#include "transaction.cpp"

//token.cpp
/* import: 'env' 'token_create' */
void (*Z_envZ_token_createZ_vjjj)(u64, u64, u64);
/* import: 'env' 'token_issue' */
void (*Z_envZ_token_issueZ_vjjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'token_transfer' */
void (*Z_envZ_token_transferZ_vjjjjii)(u64, u64, u64, u64, u32, u32);
#include "token.cpp"


//softfloat.cpp
void init_softfloat();
void init_eosio_injection();

void (*Z_envZ_wasm_syscallZ_vv)(void);
u32 (*Z_envZ_n2sZ_ijii)(u64, u32, u32);

void init_vm_api4c() {
    Z_envZ_printiZ_vj = _printi;
    Z_envZ_printsZ_vi = _prints;
    Z_envZ_n2sZ_ijii = _n2s;
    Z_envZ_wasm_syscallZ_vv = wasm_syscall;

    Z_envZ_get_codeZ_ijii = _get_code;
    Z_envZ_get_code_sizeZ_ij = _get_code_size;
    Z_envZ_set_copy_memory_rangeZ_vii = _set_copy_memory_range;
    Z_envZ_s2nZ_jii = _s2n;

    Z_envZ_read_action_dataZ_iii = read_action_data;
    Z_envZ_action_data_sizeZ_iv = action_data_size;
    Z_envZ_require_recipientZ_vj = require_recipient;
    Z_envZ_require_authZ_vj = require_auth;
    Z_envZ_require_auth2Z_vjj = require_auth2;
    Z_envZ_has_authZ_ij = has_auth;
    Z_envZ_is_accountZ_ij = is_account;
    Z_envZ_send_inlineZ_vii = send_inline;
    Z_envZ_send_context_free_inlineZ_vii = send_context_free_inline;
    Z_envZ_publication_timeZ_jv = publication_time;
    Z_envZ_current_receiverZ_jv = current_receiver;

//chain.cpp
    Z_envZ_get_active_producersZ_iii = get_active_producers;
//crypto.cpp
    Z_envZ_assert_sha256Z_viii = assert_sha256;
    Z_envZ_assert_sha1Z_viii = assert_sha1;
    Z_envZ_assert_sha512Z_viii = assert_sha512;
    Z_envZ_assert_ripemd160Z_viii = assert_ripemd160;
    Z_envZ_sha256Z_viii = sha256;
    Z_envZ_sha1Z_viii = sha1;
    Z_envZ_sha512Z_viii = sha512;
    Z_envZ_ripemd160Z_viii = ripemd160;
    Z_envZ_assert_recover_keyZ_viiiii = assert_recover_key;
    Z_envZ_recover_keyZ_iiiiii = recover_key;

    Z_envZ_from_base58Z_iiiii = from_base58;
    Z_envZ_to_base58Z_iiiii = to_base58;


//permission.cpp
    Z_envZ_check_transaction_authorizationZ_iiiiiii = check_transaction_authorization;
    Z_envZ_check_permission_authorizationZ_ijjiiiij = check_permission_authorization;
    Z_envZ_get_permission_last_usedZ_jjj = get_permission_last_used;
    Z_envZ_get_account_creation_timeZ_jj = get_account_creation_time;

//system.cpp
    Z_envZ_eosio_assertZ_vii = eosio_assert;
    Z_envZ_eosio_assert_codeZ_vij = eosio_assert_code;
    Z_envZ_eosio_assert_messageZ_viii = eosio_assert_message;
    Z_envZ_current_timeZ_jv = current_time;
    Z_envZ_call_contractZ_vjjjjjii = call_contract;
    Z_envZ_call_contract_get_resultsZ_iii = call_contract_get_results;

//transaction.cpp
    Z_envZ_send_deferredZ_vijiii = send_deferred;
    Z_envZ_cancel_deferredZ_ii = cancel_deferred;
    Z_envZ_read_transactionZ_iii = read_transaction;
    Z_envZ_transaction_sizeZ_iv = transaction_size;
    Z_envZ_tapos_block_numZ_iv = tapos_block_num;
    Z_envZ_tapos_block_prefixZ_iv = tapos_block_prefix;
    Z_envZ_expirationZ_iv = expiration;
    Z_envZ_get_actionZ_iiiii = get_action;
    Z_envZ_get_context_free_dataZ_iiii = get_context_free_data;

//token.cpp
    Z_envZ_token_createZ_vjjj = token_create;
    Z_envZ_token_issueZ_vjjjii = token_issue;
    Z_envZ_token_transferZ_vjjjjii = token_transfer;


    Z_envZ_abortZ_vv = _abort;
    Z_envZ_memcpyZ_iiii = _memcpy;
    Z_envZ_memmoveZ_iiii = _memmove;
    Z_envZ_memsetZ_iiii = _memset;
//    Z_envZ_printiZ_vj = _printi;
    Z_envZ_printnZ_vj = _printn;

//    Z_envZ_printsZ_vi = _prints;

    Z_envZ_prints_lZ_vii = _prints_l;
    Z_envZ_printuiZ_vj = _printui;

    init_db();
//softfloat.cpp
    init_softfloat();
    init_eosio_injection();
    printf("++++%s %d\n", __FUNCTION__, __LINE__);
    WASM_RT_ADD_PREFIX(init)();
}


}
