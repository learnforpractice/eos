//eosio-cpp -D__WASM -o a.wasm -I/usr/local/Cellar/eosio.cdt//1.7.0/opt/eosio.cdt/include/eosiolib all_intrinsic_api.cpp
#include <capi/eosio/types.h>
#include <capi/eosio/action.h>
#include <capi/eosio/chain.h>
#include <capi/eosio/crypto.h>
#include <capi/eosio/db.h>
#include <capi/eosio/permission.h>
#include <capi/eosio/print.h>
#include <capi/eosio/privileged.h>
#include <capi/eosio/system.h>
#include <capi/eosio/transaction.h>

extern "C" void apply(uint64_t a, uint64_t b, uint64_t c) {
            //chain.h
    get_active_producers( 0, 0 );
            //db.h
    db_store_i64(0, 0, 0, 0,  0, 0);
    db_update_i64(0, 0, 0, 0);
    db_remove_i64(0);
    db_get_i64(0, 0, 0);
    db_next_i64(0, 0);
    db_previous_i64(0, 0);
    db_find_i64(0, 0, 0, 0);
    db_lowerbound_i64(0, 0, 0, 0);
    db_upperbound_i64(0, 0, 0, 0);
    db_end_i64(0, 0, 0);
    db_idx64_store(0, 0, 0, 0, 0);
    db_idx64_update(0, 0, 0);
    db_idx64_remove(0);
    db_idx64_next(0, 0);
    db_idx64_previous(0, 0);
    db_idx64_find_primary(0, 0, 0, 0, 0);
    db_idx64_find_secondary(0, 0, 0, 0, 0);
    db_idx64_lowerbound(0, 0, 0, 0, 0);
    db_idx64_upperbound(0, 0, 0, 0, 0);
    db_idx64_end(0, 0, 0);
    db_idx128_store(0, 0, 0, 0, 0);
    db_idx128_update(0, 0, 0);
    db_idx128_remove(0);
    db_idx128_next(0, 0);
    db_idx128_previous(0, 0);
    db_idx128_find_primary(0, 0, 0, 0, 0);
    db_idx128_find_secondary(0, 0, 0, 0, 0);
    db_idx128_lowerbound(0, 0, 0, 0, 0);
    db_idx128_upperbound(0, 0, 0, 0, 0);
    db_idx128_end(0, 0, 0);
    db_idx256_store(0, 0, 0, 0, 0, 0 );
    db_idx256_update(0, 0, 0, 0);
    db_idx256_remove(0);
    db_idx256_next(0, 0);
    db_idx256_previous(0, 0);
    db_idx256_find_primary(0, 0, 0, 0, 0, 0);
    db_idx256_find_secondary(0, 0, 0, 0, 0, 0);
    db_idx256_lowerbound(0, 0, 0, 0, 0, 0);
    db_idx256_upperbound(0, 0, 0, 0, 0, 0);
    db_idx256_end(0, 0, 0);
    db_idx_double_store(0, 0, 0, 0, 0);
    db_idx_double_update(0, 0, 0);
    db_idx_double_remove(0);
    db_idx_double_next(0, 0);
    db_idx_double_previous(0, 0);
    db_idx_double_find_primary(0, 0, 0, 0, 0);
    db_idx_double_find_secondary(0, 0, 0, 0, 0);
    db_idx_double_lowerbound(0, 0, 0, 0, 0);
    db_idx_double_upperbound(0, 0, 0, 0, 0);
    db_idx_double_end(0, 0, 0);
    db_idx_long_double_store(0, 0, 0, 0, 0);
    db_idx_long_double_update(0, 0, 0);
    db_idx_long_double_remove(0);
    db_idx_long_double_next(0, 0);
    db_idx_long_double_previous(0, 0);
    db_idx_long_double_find_primary(0, 0, 0, 0, 0);
    db_idx_long_double_find_secondary(0, 0, 0, 0, 0);
    db_idx_long_double_lowerbound(0, 0, 0, 0, 0);
    db_idx_long_double_upperbound(0, 0, 0, 0, 0);
    db_idx_long_double_end(0, 0, 0);
            //permission.h
    check_transaction_authorization( 0, 0, 0, 0, 0, 0);
    check_permission_authorization( 0,
                                            0,
    0, 0,
    0, 0,
    0
                                        );

    get_permission_last_used( 0, 0 );
    get_account_creation_time( 0 );
            //crypto.h
    assert_sha256( 0, 0, 0 );
    assert_sha1( 0, 0, 0);
    assert_sha512( 0, 0, 0 );
    assert_ripemd160( 0, 0, 0);
    sha256( 0, 0, 0 );
    sha1( 0, 0, 0 );
    sha512( 0, 0, 0 );
    ripemd160( 0, 0, 0 );
    recover_key( 0, 0, 0, 0, 0 );
    assert_recover_key( 0, 0, 0, 0, 0 );
            //action.h
    read_action_data( 0, 0 );
    action_data_size();
    require_recipient( 0 );
    require_auth( 0 );
    has_auth( 0 );
    require_auth2( 0, 0 );
    is_account( 0 );
    send_inline(0, 0);
    send_context_free_inline(0, 0);
    publication_time();
    current_receiver();
//     set_action_return_value(0, 0);

            //print.h
    prints( 0 );
    prints_l( 0, 0);
    printi( 0 );
    printui(0);
    printi128( 0 );
    printui128( 0 );
    printsf(0);
    printdf(0);
    printqf(0);
    printn( 0 );
    printhex( 0, 0 );
            //system.h
    eosio_assert( 0, 0 );
    eosio_assert_message( 0, 0, 0 );
    eosio_assert_code( 0, 0 );
    eosio_exit( 0 );
    current_time();
    is_feature_activated( 0 );
    get_sender();
            //privileged.h
    get_resource_limits( 0, 0, 0, 0 );
    set_resource_limits( 0, 0, 0, 0 );
    set_proposed_producers( 0, 0);
    set_proposed_producers_ex( 0, 0, 0 );
    is_privileged( 0 );
    set_privileged( 0, 0 );
    set_blockchain_parameters_packed( 0, 0 );
    get_blockchain_parameters_packed( 0, 0 );
    preactivate_feature( 0 );

//     is_feature_active( 0 );
//     activate_feature( 0 );

//     set_resource_limit( 0, 0, 0 );
//     get_resource_limit( 0, 0 );
//     get_wasm_parameters_packed( 0, 0, 0 );
//     set_wasm_parameters_packed( 0, 0 );
//     get_parameters_packed( 0, 0, 0);
//     set_parameters_packed( 0, 0 );
//     get_kv_parameters_packed( 0, 0, 0 );
//     set_kv_parameters_packed( 0, 0 );

            //transaction.h
    send_deferred(0, 0, 0, 0, 0);
    cancel_deferred(0);
    read_transaction(0, 0);
    transaction_size();
    tapos_block_num();
    tapos_block_prefix();
    expiration();
    get_action( 0, 0, 0, 0 );
    get_context_free_data( 0, 0, 0 );

//     call_contract(0, 0, 0);
//     call_contract_get_args(0, 0);
//     call_contract_set_results(0, 0);
//     call_contract_get_results(0, 0);

}
