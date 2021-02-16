#include "vm_api4c.hpp"
#include "action.cpp"
#include "chain.cpp"
#include "crypto.cpp"
#include "db.cpp"
#include "permission.cpp"
#include "print.cpp"
#include "privileged.cpp"
#include "transaction.cpp"
#include "system.cpp"

#include "env.cpp"

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

extern "C" void vm_checktime() {

}

void init_action() {
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
}

void init_crypto() {
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
}

static void init_db() {
   Z_envZ_db_lowerbound_i64Z_ijjjj = db_lowerbound_i64 ;
   Z_envZ_db_next_i64Z_iii = db_next_i64 ;
   Z_envZ_db_remove_i64Z_vi = db_remove_i64 ;
   Z_envZ_db_store_i64Z_ijjjjii = db_store_i64 ;
   Z_envZ_db_update_i64Z_vijii = db_update_i64 ;
   Z_envZ_db_upperbound_i64Z_ijjjj = db_upperbound_i64 ;
   Z_envZ_db_end_i64Z_ijjj = db_end_i64 ;
   // Z_envZ_db_get_countZ_ijjj = db_get_count ;

   Z_envZ_db_find_i64Z_ijjjj = db_find_i64 ;
   Z_envZ_db_get_i64Z_iiii = db_get_i64 ;

   Z_envZ_db_idx128_endZ_ijjj = db_idx128_end ;
   Z_envZ_db_idx128_find_primaryZ_ijjjij = db_idx128_find_primary ;
   Z_envZ_db_idx128_find_secondaryZ_ijjjii = db_idx128_find_secondary ;
   Z_envZ_db_idx128_lowerboundZ_ijjjii = db_idx128_lowerbound ;
   Z_envZ_db_idx128_nextZ_iii = db_idx128_next ;
   Z_envZ_db_idx128_previousZ_iii = db_idx128_previous ;
   Z_envZ_db_idx128_storeZ_ijjjji = db_idx128_store ;
   Z_envZ_db_idx128_updateZ_viji = db_idx128_update ;
   Z_envZ_db_idx128_removeZ_vi = db_idx128_remove;
   Z_envZ_db_idx128_upperboundZ_ijjjii = db_idx128_upperbound ;

   Z_envZ_db_idx256_storeZ_ijjjjii = db_idx256_store;
   Z_envZ_db_idx256_endZ_ijjj = db_idx256_end ;
   Z_envZ_db_idx256_find_primaryZ_ijjjiij = db_idx256_find_primary ;
   Z_envZ_db_idx256_find_secondaryZ_ijjjiii = db_idx256_find_secondary ;
   Z_envZ_db_idx256_lowerboundZ_ijjjiii = db_idx256_lowerbound ;
   Z_envZ_db_idx256_nextZ_iii = db_idx256_next ;
   Z_envZ_db_idx256_previousZ_iii = db_idx256_previous ;
   Z_envZ_db_idx256_removeZ_vi = db_idx256_remove ;
   Z_envZ_db_idx256_updateZ_vijii = db_idx256_update ;
   Z_envZ_db_idx256_upperboundZ_ijjjiii = db_idx256_upperbound ;

   Z_envZ_db_idx64_endZ_ijjj = db_idx64_end ;
   Z_envZ_db_idx64_find_primaryZ_ijjjij = db_idx64_find_primary ;
   Z_envZ_db_idx64_find_secondaryZ_ijjjii = db_idx64_find_secondary ;
   Z_envZ_db_idx64_lowerboundZ_ijjjii = db_idx64_lowerbound ;
   Z_envZ_db_idx64_nextZ_iii = db_idx64_next ;
   Z_envZ_db_idx64_previousZ_iii = db_idx64_previous ;
   Z_envZ_db_idx64_removeZ_vi = db_idx64_remove ;
   Z_envZ_db_idx64_storeZ_ijjjji = db_idx64_store ;
   Z_envZ_db_idx64_updateZ_viji = db_idx64_update ;
   Z_envZ_db_idx64_upperboundZ_ijjjii = db_idx64_upperbound ;

   Z_envZ_db_idx_double_endZ_ijjj = db_idx_double_end ;
   Z_envZ_db_idx_double_find_primaryZ_ijjjij = db_idx_double_find_primary ;
   Z_envZ_db_idx_double_find_secondaryZ_ijjjii = db_idx_double_find_secondary ;
   Z_envZ_db_idx_double_lowerboundZ_ijjjii = db_idx_double_lowerbound ;
   Z_envZ_db_idx_double_nextZ_iii = db_idx_double_next ;
   Z_envZ_db_idx_double_previousZ_iii = db_idx_double_previous ;
   Z_envZ_db_idx_double_removeZ_vi = db_idx_double_remove ;
   Z_envZ_db_idx_double_storeZ_ijjjji = db_idx_double_store ;
   Z_envZ_db_idx_double_updateZ_viji = db_idx_double_update ;
   Z_envZ_db_idx_double_upperboundZ_ijjjii = db_idx_double_upperbound ;

   Z_envZ_db_idx_long_double_endZ_ijjj = db_idx_long_double_end ;
   Z_envZ_db_idx_long_double_find_primaryZ_ijjjij = db_idx_long_double_find_primary ;
   Z_envZ_db_idx_long_double_find_secondaryZ_ijjjii = db_idx_long_double_find_secondary ;
   Z_envZ_db_idx_long_double_lowerboundZ_ijjjii = db_idx_long_double_lowerbound ;
   Z_envZ_db_idx_long_double_nextZ_iii = db_idx_long_double_next ;
   Z_envZ_db_idx_long_double_previousZ_iii = db_idx_long_double_previous ;
   Z_envZ_db_idx_long_double_removeZ_vi = db_idx_long_double_remove ;
   Z_envZ_db_idx_long_double_storeZ_ijjjji = db_idx_long_double_store ;
   Z_envZ_db_idx_long_double_updateZ_viji = db_idx_long_double_update ;
   Z_envZ_db_idx_long_double_upperboundZ_ijjjii = db_idx_long_double_upperbound ;

   Z_envZ_db_previous_i64Z_iii = db_previous_i64;

   // Z_envZ_db_find_i256Z_ijjjii = db_find_i256;
   // Z_envZ_db_update_i256Z_vijii = db_update_i256;
   // Z_envZ_db_store_i256Z_ijjjiiii = db_store_i256;
   // Z_envZ_db_get_i256Z_iiii = db_get_i256;
   // Z_envZ_db_remove_i256Z_vi = db_remove_i256;

   // Z_envZ_db_next_i256Z_iiii = db_next_i256;
   // Z_envZ_db_previous_i256Z_iiii = db_previous_i256;

   // Z_envZ_db_lowerbound_i256Z_ijjjii = db_lowerbound_i256;
   // Z_envZ_db_upperbound_i256Z_ijjjii = db_upperbound_i256;
   // Z_envZ_db_end_i256Z_ijjj = db_end_i256;

}


void init_print() {
    Z_envZ_printhexZ_vii = printhex;

    Z_envZ_printiZ_vj = printi;
    Z_envZ_printsZ_vi = prints;
    Z_envZ_printqfZ_vi = printqf;
}


void init_privileged() {
    Z_envZ_get_resource_limitsZ_vjiii = get_resource_limits;	
    Z_envZ_set_resource_limitsZ_vjjjj = set_resource_limits;	
    Z_envZ_set_proposed_producersZ_jii = set_proposed_producers;	
    Z_envZ_is_privilegedZ_ij = is_privileged;	
    Z_envZ_set_privilegedZ_vji = set_privileged;	
    Z_envZ_set_blockchain_parameters_packedZ_vii = set_blockchain_parameters_packed;	
    Z_envZ_get_blockchain_parameters_packedZ_iii = get_blockchain_parameters_packed;	
   //  Z_envZ_activate_featureZ_vj = activate_feature;
   //  Z_envZ_preactivate_featureZ_vi = preactivate_feature;
}

void init_vm_api4c() {
   init_action();
   init_crypto();
   init_db();
   init_print();
   init_privileged();

//system.cpp
    Z_envZ_eosio_assertZ_vii = eosio_assert;
    Z_envZ_eosio_assert_codeZ_vij = eosio_assert_code;
   //  Z_envZ_eosio_assert_messageZ_viii = eosio_assert_message;
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
}