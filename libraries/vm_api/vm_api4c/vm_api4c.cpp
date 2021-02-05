#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wasm-rt.h"
#include <capi/types.h>

#include <uuos.hpp>

#include "vm_api4c.h"

#define EOSIO_THROW(msg) get_vm_api_proxy()->eosio_assert(false, msg)
#define EOSIO_ASSERT(test, msg) get_vm_api_proxy()->eosio_assert(test, msg)


#define PAGE_SIZE (65536)

#ifndef WASM_RT_MODULE_PREFIX
#define WASM_RT_MODULE_PREFIX
#endif

#define WASM_RT_PASTE_(x, y) x ## y
#define WASM_RT_PASTE(x, y) WASM_RT_PASTE_(x, y)
#define WASM_RT_ADD_PREFIX(x) WASM_RT_PASTE(WASM_RT_MODULE_PREFIX, x)

void *offset_to_ptr(u32 offset, u32 size);
void *offset_to_char_ptr(u32 offset);


static void _abort() {
    get_vm_api()->eosio_exit(0);
}

extern "C" void vm_print_stacktrace(void);

static u32 _memcpy(u32 dest_offset, u32 src_offset, u32 size) {
    int overlap;
    if (dest_offset > src_offset) {
        overlap = dest_offset - src_offset < size;
    } else {
        overlap = src_offset - dest_offset < size;
    }

    if (overlap) {
        printf("++++++++++++dest_offset: %u, src_offset: %u, size: %u\n", dest_offset, src_offset, size);
        vm_print_stacktrace();
        get_vm_api()->eosio_assert(false, "memcpy can only accept non-aliasing pointers");
    }
    
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

extern "C" {

u32 _get_code_size(u64 account);
u32 _get_code(u64 account, u32 memory_offset, u32 buffer_size);
u64 _s2n(u32 in_offset, u32 in_len);

u64 _s2n(u32 in_offset, u32 in_len);
u32 _n2s(u64 n, u32 out_offset, u32 length);

/* import: 'env' 'call_native' */
u32 (*Z_envZ_call_nativeZ_iiiiiii)(u32, u32, u32, u32, u32, u32);

/* import: 'env' 's2n' */
u64 (*Z_envZ_s2nZ_jii)(u32, u32);
/* import: 'env' 'n2s' */
u32 (*Z_envZ_n2sZ_ijii)(u64, u32, u32);

u32 (*Z_envZ_memcpyZ_iiii)(u32, u32, u32);
/* import: 'env' 'memset' */
u32 (*Z_envZ_memsetZ_iiii)(u32, u32, u32);
/* import: 'env' 'memmove' */
u32 (*Z_envZ_memmoveZ_iiii)(u32, u32, u32);
/* import: 'env' 'get_code' */
u32 (*Z_envZ_get_codeZ_ijii)(u64, u32, u32);
/* import: 'env' 'get_code_size' */
u32 (*Z_envZ_get_code_sizeZ_ij)(u64);
u32 (*Z_envZ_get_code_hashZ_ijii)(u64, u32, u32);
/* import: 'env' 'token_create' */
void (*Z_envZ_token_createZ_vjjj)(u64, u64, u64);

/* import: 'env' 'get_active_producers' */
u32 (*Z_envZ_get_active_producersZ_iii)(u32, u32);

/* import: 'env' 'set_copy_memory_range' */
void (*Z_envZ_set_copy_memory_rangeZ_vii)(u32, u32);

/* import: 'env' 'abort' */
void (*Z_envZ_abortZ_vv)(void);


#include "db.cpp"

#include "action.cpp"
#include "chain.cpp"
#include "crypto.cpp"
#include "permission.cpp"
#include "system.cpp"
#include "transaction.cpp"
#include "print.cpp"


//softfloat.cpp
void init_softfloat();
void init_eosio_injection();

void init_privileged();
void init_compiler_builtins();

void init_vm_api4c() {
    static bool initialized = false;
    if (initialized) {
        return;
    }
    initialized = true;

    init_print();

    Z_envZ_n2sZ_ijii = _n2s;

    Z_envZ_get_codeZ_ijii = _get_code;
    Z_envZ_get_code_sizeZ_ij = _get_code_size;
    Z_envZ_s2nZ_jii = _s2n;

    init_action();

//chain.cpp
    Z_envZ_get_active_producersZ_iii = get_active_producers;
//crypto.cpp
    init_crypto();

//permission.cpp
    Z_envZ_check_transaction_authorizationZ_iiiiiii = check_transaction_authorization;
    Z_envZ_check_permission_authorizationZ_ijjiiiij = check_permission_authorization;
    Z_envZ_get_permission_last_usedZ_jjj = get_permission_last_used;
    Z_envZ_get_account_creation_timeZ_jj = get_account_creation_time;

//privileged.cpp
    init_privileged();

//compiler_builtins.cpp
    // init_compiler_builtins();

//system.cpp
    Z_envZ_eosio_assertZ_vii = eosio_assert;
    Z_envZ_eosio_assert_codeZ_vij = eosio_assert_code;
    Z_envZ_eosio_assert_messageZ_viii = eosio_assert_message;
    Z_envZ_current_timeZ_jv = current_time;
    Z_envZ_call_contractZ_vjii = call_contract;
    Z_envZ_call_contract_get_resultsZ_iii = call_contract_get_results;
    // Z_envZ_get_code_hashZ_ijii = get_code_hash;

// transaction.cpp
    Z_envZ_send_deferredZ_vijiii = send_deferred;
    Z_envZ_cancel_deferredZ_ii = cancel_deferred;
    Z_envZ_read_transactionZ_iii = read_transaction;
    Z_envZ_transaction_sizeZ_iv = transaction_size;
    Z_envZ_tapos_block_numZ_iv = tapos_block_num;
    Z_envZ_tapos_block_prefixZ_iv = tapos_block_prefix;
    Z_envZ_expirationZ_iv = expiration;
    Z_envZ_get_actionZ_iiiii = get_action;
    Z_envZ_get_context_free_dataZ_iiii = get_context_free_data;

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
//    printf("++++%s %d\n", __FUNCTION__, __LINE__);
}

static fn_offset_to_ptr g_offset_to_ptr = 0;
static fn_offset_to_char_ptr g_offset_to_char_ptr = 0;

void set_memory_converter(fn_offset_to_ptr f1, fn_offset_to_char_ptr f2) {
    g_offset_to_ptr = f1;
    g_offset_to_char_ptr = f2;
}

void *offset_to_ptr(u32 offset, u32 size) {
    return g_offset_to_ptr(offset, size);
}

void *offset_to_char_ptr(u32 offset) {
    return g_offset_to_char_ptr(offset);
}

void wasm_rt_on_trap(int code) {
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

}
