#pragma once
#include "vm_api4c.h"

class vm_api4c_proxy {
    /* import: 'env' 'get_active_producers' */
    virtual u32 get_active_producers(u32, u32);
    /* import: 'env' 'db_store_i64' */
    virtual u32 db_store_i64(u64, u64, u64, u64, u32, u32);
    /* import: 'env' 'db_update_i64' */
    virtual void db_update_i64(u32, u64, u32, u32);
    /* import: 'env' 'db_remove_i64' */
    virtual void db_remove_i64(u32);
    /* import: 'env' 'db_get_i64' */
    virtual u32 db_get_i64(u32, u32, u32);
    /* import: 'env' 'db_next_i64' */
    virtual u32 db_next_i64(u32, u32);
    /* import: 'env' 'db_previous_i64' */
    virtual u32 db_previous_i64(u32, u32);
    /* import: 'env' 'db_find_i64' */
    virtual u32 db_find_i64(u64, u64, u64, u64);
    /* import: 'env' 'db_lowerbound_i64' */
    virtual u32 db_lowerbound_i64(u64, u64, u64, u64);
    /* import: 'env' 'db_upperbound_i64' */
    virtual u32 db_upperbound_i64(u64, u64, u64, u64);
    /* import: 'env' 'db_end_i64' */
    virtual u32 db_end_i64(u64, u64, u64);
    /* import: 'env' 'db_idx64_store' */
    virtual u32 db_idx64_store(u64, u64, u64, u64, u32);
    /* import: 'env' 'db_idx64_update' */
    virtual void db_idx64_update(u32, u64, u32);
    /* import: 'env' 'db_idx64_remove' */
    virtual void db_idx64_remove(u32);
    /* import: 'env' 'db_idx64_next' */
    virtual u32 db_idx64_next(u32, u32);
    /* import: 'env' 'db_idx64_previous' */
    virtual u32 db_idx64_previous(u32, u32);
    /* import: 'env' 'db_idx64_find_primary' */
    virtual u32 db_idx64_find_primary(u64, u64, u64, u32, u64);
    /* import: 'env' 'db_idx64_find_secondary' */
    virtual u32 db_idx64_find_secondary(u64, u64, u64, u32, u32);
    /* import: 'env' 'db_idx64_lowerbound' */
    virtual u32 db_idx64_lowerbound(u64, u64, u64, u32, u32);
    /* import: 'env' 'db_idx64_upperbound' */
    virtual u32 db_idx64_upperbound(u64, u64, u64, u32, u32);
    /* import: 'env' 'db_idx64_end' */
    virtual u32 db_idx64_end(u64, u64, u64);
    /* import: 'env' 'db_idx128_store' */
    virtual u32 db_idx128_store(u64, u64, u64, u64, u32);
    /* import: 'env' 'db_idx128_update' */
    virtual void db_idx128_update(u32, u64, u32);
    /* import: 'env' 'db_idx128_remove' */
    virtual void db_idx128_remove(u32);
    /* import: 'env' 'db_idx128_next' */
    virtual u32 db_idx128_next(u32, u32);
    /* import: 'env' 'db_idx128_previous' */
    virtual u32 db_idx128_previous(u32, u32);
    /* import: 'env' 'db_idx128_find_primary' */
    virtual u32 db_idx128_find_primary(u64, u64, u64, u32, u64);
    /* import: 'env' 'db_idx128_find_secondary' */
    virtual u32 db_idx128_find_secondary(u64, u64, u64, u32, u32);
    /* import: 'env' 'db_idx128_lowerbound' */
    virtual u32 db_idx128_lowerbound(u64, u64, u64, u32, u32);
    /* import: 'env' 'db_idx128_upperbound' */
    virtual u32 db_idx128_upperbound(u64, u64, u64, u32, u32);
    /* import: 'env' 'db_idx128_end' */
    virtual u32 db_idx128_end(u64, u64, u64);
    /* import: 'env' 'db_idx256_store' */
    virtual u32 db_idx256_store(u64, u64, u64, u64, u32, u32);
    /* import: 'env' 'db_idx256_update' */
    virtual void db_idx256_update(u32, u64, u32, u32);
    /* import: 'env' 'db_idx256_remove' */
    virtual void db_idx256_remove(u32);
    /* import: 'env' 'db_idx256_next' */
    virtual u32 db_idx256_next(u32, u32);
    /* import: 'env' 'db_idx256_previous' */
    virtual u32 db_idx256_previous(u32, u32);
    /* import: 'env' 'db_idx256_find_primary' */
    virtual u32 db_idx256_find_primary(u64, u64, u64, u32, u32, u64);
    /* import: 'env' 'db_idx256_find_secondary' */
    virtual u32 db_idx256_find_secondary(u64, u64, u64, u32, u32, u32);
    /* import: 'env' 'db_idx256_lowerbound' */
    virtual u32 db_idx256_lowerbound(u64, u64, u64, u32, u32, u32);
    /* import: 'env' 'db_idx256_upperbound' */
    virtual u32 db_idx256_upperbound(u64, u64, u64, u32, u32, u32);
    /* import: 'env' 'db_idx256_end' */
    virtual u32 db_idx256_end(u64, u64, u64);
    /* import: 'env' 'db_idx_double_store' */
    virtual u32 db_idx_double_store(u64, u64, u64, u64, u32);
    /* import: 'env' 'db_idx_double_update' */
    virtual void db_idx_double_update(u32, u64, u32);
    /* import: 'env' 'db_idx_double_remove' */
    virtual void db_idx_double_remove(u32);
    /* import: 'env' 'db_idx_double_next' */
    virtual u32 db_idx_double_next(u32, u32);
    /* import: 'env' 'db_idx_double_previous' */
    virtual u32 db_idx_double_previous(u32, u32);
    /* import: 'env' 'db_idx_double_find_primary' */
    virtual u32 db_idx_double_find_primary(u64, u64, u64, u32, u64);
    /* import: 'env' 'db_idx_double_find_secondary' */
    virtual u32 db_idx_double_find_secondary(u64, u64, u64, u32, u32);
    /* import: 'env' 'db_idx_double_lowerbound' */
    virtual u32 db_idx_double_lowerbound(u64, u64, u64, u32, u32);
    /* import: 'env' 'db_idx_double_upperbound' */
    virtual u32 db_idx_double_upperbound(u64, u64, u64, u32, u32);
    /* import: 'env' 'db_idx_double_end' */
    virtual u32 db_idx_double_end(u64, u64, u64);
    /* import: 'env' 'db_idx_long_double_store' */
    virtual u32 db_idx_long_double_store(u64, u64, u64, u64, u32);
    /* import: 'env' 'db_idx_long_double_update' */
    virtual void db_idx_long_double_update(u32, u64, u32);
    /* import: 'env' 'db_idx_long_double_remove' */
    virtual void db_idx_long_double_remove(u32);
    /* import: 'env' 'db_idx_long_double_next' */
    virtual u32 db_idx_long_double_next(u32, u32);
    /* import: 'env' 'db_idx_long_double_previous' */
    virtual u32 db_idx_long_double_previous(u32, u32);
    /* import: 'env' 'db_idx_long_double_find_primary' */
    virtual u32 db_idx_long_double_find_primary(u64, u64, u64, u32, u64);
    /* import: 'env' 'db_idx_long_double_find_secondary' */
    virtual u32 db_idx_long_double_find_secondary(u64, u64, u64, u32, u32);
    /* import: 'env' 'db_idx_long_double_lowerbound' */
    virtual u32 db_idx_long_double_lowerbound(u64, u64, u64, u32, u32);
    /* import: 'env' 'db_idx_long_double_upperbound' */
    virtual u32 db_idx_long_double_upperbound(u64, u64, u64, u32, u32);
    /* import: 'env' 'db_idx_long_double_end' */
    virtual u32 db_idx_long_double_end(u64, u64, u64);
    /* import: 'env' 'check_transaction_authorization' */
    virtual u32 check_transaction_authorization(u32, u32, u32, u32, u32, u32);
    /* import: 'env' 'check_permission_authorization' */
    virtual u32 check_permission_authorization(u64, u64, u32, u32, u32, u32, u64);
    /* import: 'env' 'get_permission_last_used' */
    virtual u64 get_permission_last_used(u64, u64);
    /* import: 'env' 'get_account_creation_time' */
    virtual u64 get_account_creation_time(u64);
    /* import: 'env' 'assert_sha256' */
    virtual void assert_sha256(u32, u32, u32);
    /* import: 'env' 'assert_sha1' */
    virtual void assert_sha1(u32, u32, u32);
    /* import: 'env' 'assert_sha512' */
    virtual void assert_sha512(u32, u32, u32);
    /* import: 'env' 'assert_ripemd160' */
    virtual void assert_ripemd160(u32, u32, u32);
    /* import: 'env' 'sha256' */
    virtual void sha256(u32, u32, u32);
    /* import: 'env' 'sha1' */
    virtual void sha1(u32, u32, u32);
    /* import: 'env' 'sha512' */
    virtual void sha512(u32, u32, u32);
    /* import: 'env' 'ripemd160' */
    virtual void ripemd160(u32, u32, u32);
    /* import: 'env' 'recover_key' */
    virtual u32 recover_key(u32, u32, u32, u32, u32);
    /* import: 'env' 'assert_recover_key' */
    virtual void assert_recover_key(u32, u32, u32, u32, u32);
    /* import: 'env' 'read_action_data' */
    virtual u32 read_action_data(u32, u32);
    /* import: 'env' 'action_data_size' */
    virtual u32 action_data_size(void);
    /* import: 'env' 'require_recipient' */
    virtual void require_recipient(u64);
    /* import: 'env' 'require_auth' */
    virtual void require_auth(u64);
    /* import: 'env' 'has_auth' */
    virtual u32 has_auth(u64);
    /* import: 'env' 'require_auth2' */
    virtual void require_auth2(u64, u64);
    /* import: 'env' 'is_account' */
    virtual u32 is_account(u64);
    /* import: 'env' 'send_inline' */
    virtual void send_inline(u32, u32);
    /* import: 'env' 'send_context_free_inline' */
    virtual void send_context_free_inline(u32, u32);
    /* import: 'env' 'publication_time' */
    virtual u64 publication_time(void);
    /* import: 'env' 'current_receiver' */
    virtual u64 current_receiver(void);
    /* import: 'env' 'prints' */
    virtual void prints(u32);
    /* import: 'env' 'prints_l' */
    virtual void prints_l(u32, u32);
    /* import: 'env' 'printi' */
    virtual void printi(u64);
    /* import: 'env' 'printui' */
    virtual void printui(u64);
    /* import: 'env' 'printi128' */
    virtual void printi128(u32);
    /* import: 'env' 'printui128' */
    virtual void printui128(u32);
    /* import: 'env' 'printsf' */
    virtual void printsf(f32);
    /* import: 'env' 'printdf' */
    virtual void printdf(f64);
    /* import: 'env' 'printqf' */
    virtual void printqf(u32);
    /* import: 'env' 'printn' */
    virtual void printn(u64);
    /* import: 'env' 'printhex' */
    virtual void printhex(u32, u32);
    /* import: 'env' 'eosio_assert' */
    virtual void eosio_assert(u32, u32);
    /* import: 'env' 'eosio_assert_message' */
    virtual void eosio_assert_message(u32, u32, u32);
    /* import: 'env' 'eosio_assert_code' */
    virtual void eosio_assert_code(u32, u64);
    /* import: 'env' 'eosio_exit' */
    virtual void eosio_exit(u32);

};
