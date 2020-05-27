#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wasm-rt.h"
#include <eosiolib/types.h>
#include <vm_api/vm_api.h>
//#include <chain_api.hpp>

#include "src/interp.h"
#include "vm_api4c.h"
#include <env.h>

using namespace wabt::interp;

#define PAGE_SIZE (65536)

#ifndef WASM_RT_MODULE_PREFIX
#define WASM_RT_MODULE_PREFIX
#endif

#define WASM_RT_PASTE_(x, y) x ## y
#define WASM_RT_PASTE(x, y) WASM_RT_PASTE_(x, y)
#define WASM_RT_ADD_PREFIX(x) WASM_RT_PASTE(WASM_RT_MODULE_PREFIX, x)

extern "C" {
void *offset_to_ptr(u32 offset, u32 size);
void *offset_to_char_ptr(u32 offset);


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
    if (get_vm_api()->is_in_apply_context) {
        get_vm_api()->prints_l(s, size);
    } else {
        vmelog("%s", s);
    }
}

static void _printui(u64 u) {
    get_vm_api()->printui(u);
}

u32 _get_code_size(u64 account);
u32 _get_code(u64 account, u32 memory_offset, u32 buffer_size);
u64 _s2n(u32 in_offset, u32 in_len);

/* import: 'env' 'prints' */
void _prints(u32 a);
void _printi(u64 a);

u64 _s2n(u32 in_offset, u32 in_len);
u32 _n2s(u64 n, u32 out_offset, u32 length);


#include "db.cpp"

static void _set_copy_memory_range(u32 start, u32 end) {
    get_vm_api()->set_copy_memory_range((int)start, (int)end);
}

#include "action.cpp"
#include "chain.cpp"
#include "crypto.cpp"
#include "permission.cpp"
#include "system.cpp"
#include "transaction.cpp"
#include "token.cpp"


//softfloat.cpp
void init_softfloat();
void init_eosio_injection();

void init_privileged();
void init_compiler_builtins();

void _printhex(u32 data_offset, u32 data_len) {
    uint8_t *data = (uint8_t *)offset_to_ptr(data_offset, data_len);
    get_vm_api()->printhex(data, data_len);
}

u32 _call_native(u32 main_type, u32 sub_type, u32 input_offset, u32 input_size, u32 output_offset, u32 output_size) {
    uint8_t *input = (uint8_t *)offset_to_ptr(input_offset, input_size);
    uint8_t *output = (uint8_t *)offset_to_ptr(output_offset, output_size);
    // return get_vm_api()->call_native(main_type, sub_type, input, input_size, output, output_size);
}

void init_vm_api4c() {
    static bool initialized = false;
    if (initialized) {
        return;
    }
    initialized = true;

    Z_envZ_call_nativeZ_iiiiiii = _call_native;
    Z_envZ_printhexZ_vii = _printhex;

    Z_envZ_printiZ_vj = _printi;
    Z_envZ_printsZ_vi = _prints;
    Z_envZ_n2sZ_ijii = _n2s;

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


//permission.cpp
    Z_envZ_check_transaction_authorizationZ_iiiiiii = check_transaction_authorization;
    Z_envZ_check_permission_authorizationZ_ijjiiiij = check_permission_authorization;
    Z_envZ_get_permission_last_usedZ_jjj = get_permission_last_used;
    Z_envZ_get_account_creation_timeZ_jj = get_account_creation_time;

//privileged.cpp
    init_privileged();

//compiler_builtins.cpp
    init_compiler_builtins();

//system.cpp
    Z_envZ_eosio_assertZ_vii = eosio_assert;
    Z_envZ_eosio_assert_codeZ_vij = eosio_assert_code;
    Z_envZ_eosio_assert_messageZ_viii = eosio_assert_message;
    Z_envZ_current_timeZ_jv = current_time;
    Z_envZ_call_contractZ_vjii = call_contract;
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

    Z_envZ_token_openZ_vjjj = token_open;
    Z_envZ_token_retireZ_vjjii = token_retire;
    Z_envZ_token_closeZ_vjj = token_close;

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

}
