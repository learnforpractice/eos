#include <string.h>
#include "wasm-rt-impl.h"
#include <vm_api/vm_api.h>

#include "native_contracts.hpp"
#include "native_contracts.h"

extern "C" {
    void eosio_native_init_globals();
    u32 eosio_native_malloc(u32 size);

    void *offset_to_ptr(u32 offset, u32 size);
    void *offset_to_char_ptr(u32 offset);

    /* export: 'eosio_system_get_delegated_balance' */
    extern u64 (*WASM_RT_ADD_PREFIX(Z_eosio_system_get_delegated_balanceZ_jj))(u64);
    /* export: 'eosio_system_get_rex_fund' */
    extern u64 (*WASM_RT_ADD_PREFIX(Z_eosio_system_get_rex_fundZ_jj))(u64);
    /* export: 'system_contract_is_vm_activated' */
    extern u32 (*WASM_RT_ADD_PREFIX(Z_system_contract_is_vm_activatedZ_iii))(u32, u32);


    /* export: 'token_create' */
    extern void (*WASM_RT_ADD_PREFIX(Z_token_createZ_vjjj))(u64, u64, u64);
    /* export: 'token_issue' */
    extern void (*WASM_RT_ADD_PREFIX(Z_token_issueZ_vjjjii))(u64, u64, u64, u32, u32);
    /* export: 'token_transfer' */
    extern void (*WASM_RT_ADD_PREFIX(Z_token_transferZ_vjjjjii))(u64, u64, u64, u64, u32, u32);
    /* export: 'token_open' */
    extern void (*WASM_RT_ADD_PREFIX(Z_token_openZ_vjjj))(u64, u64, u64);
    /* export: 'token_retire' */
    extern void (*WASM_RT_ADD_PREFIX(Z_token_retireZ_vjjii))(u64, u64, u32, u32);
    /* export: 'token_close' */
    extern void (*WASM_RT_ADD_PREFIX(Z_token_closeZ_vjj))(u64, u64);
    /* export: 'token_get_balance' */
    extern u64 (*WASM_RT_ADD_PREFIX(Z_token_get_balanceZ_jji))(u64, u32);
    /* export: 'token_set_balance' */
    extern void (*WASM_RT_ADD_PREFIX(Z_token_set_balanceZ_vjji))(u64, u64, u32);


    int64_t eosio_system_get_delegated_balance(uint64_t owner) {
        eosio_native_init_globals();
        return Z_eosio_system_get_delegated_balanceZ_jj(owner);
    }

    int64_t eosio_system_get_rex_fund(uint64_t owner) {
        eosio_native_init_globals();
        return Z_eosio_system_get_rex_fundZ_jj(owner);
    }

    bool system_contract_is_vm_activated( uint8_t vmtype, uint8_t vmversion ) {
        eosio_native_init_globals();
        return Z_system_contract_is_vm_activatedZ_iii(vmtype, vmversion);
    }


    void token_create( uint64_t issuer, int64_t maximum_supply, uint64_t sym) {
        Z_token_createZ_vjjj(issuer, maximum_supply, sym);
    }

    void token_issue( uint64_t to, int64_t quantity, uint64_t sym, const char* memo, size_t size2 ) {
        u32 memo_offset = eosio_native_malloc(size2);
        char *_memo = (char *)offset_to_ptr(memo_offset, (u32)size2);
        memcpy(_memo, memo, size2);
        Z_token_issueZ_vjjjii(to, quantity, sym, memo_offset, size2);
    }

    void token_transfer( uint64_t from, uint64_t to, int64_t quantity, uint64_t sym, const char* memo, size_t size2) {
        u32 memo_offset = eosio_native_malloc(size2);
        char *_memo = (char *)offset_to_ptr(memo_offset, (u32)size2);
        memcpy(_memo, memo, size2);
        Z_token_transferZ_vjjjjii(from, to, quantity, sym, memo_offset, size2);
    }

    void token_open( uint64_t owner, uint64_t _symbol, uint64_t ram_payer ) {
        Z_token_openZ_vjjj(owner, _symbol, ram_payer);
    }

    void token_retire( int64_t amount, uint64_t _symbol, const char *memo, size_t memo_size ) {
        u32 memo_offset = eosio_native_malloc(memo_size);
        char *_memo = (char *)offset_to_ptr(memo_offset, (u32)memo_size);
        memcpy(_memo, memo, memo_size);
        Z_token_retireZ_vjjii(amount, _symbol, memo_offset, memo_size);
    }

    void token_close( uint64_t owner, uint64_t _symbol ) {
        Z_token_closeZ_vjj(owner, _symbol);
    }

}

