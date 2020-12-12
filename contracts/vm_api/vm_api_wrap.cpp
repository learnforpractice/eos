#include "vm_api_wrap.h"
#include "vm_api.h"
#include <string.h>
#include <stdio.h>

#include "vm_api_wrap/action.cpp"
#include "vm_api_wrap/chain.cpp"
#include "vm_api_wrap/crypto.cpp"
#include "vm_api_wrap/db.cpp"
#include "vm_api_wrap/permission.cpp"
#include "vm_api_wrap/print.cpp"
#include "vm_api_wrap/privileged.cpp"
#include "vm_api_wrap/system.cpp"
#include "vm_api_wrap/transaction.cpp"
#include "vm_api_wrap/vm_api.cpp"

struct function_info {
    int arg_count;
    const char *name;
    void (*fn)(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret);
};

static struct function_info s_vm_api_functions[enum_vm_api_function_count];
#define SET_FUNCTION_INFO(FN, COUNT) \
    s_vm_api_functions[enum_##FN] = {COUNT, #FN, FN};

void init_function_args() {
    static int init = false;
    if (init) {
        return;
    }
    memset(s_vm_api_functions, 0, sizeof(s_vm_api_functions));

    SET_FUNCTION_INFO(read_action_data, 1);
    SET_FUNCTION_INFO(action_data_size, 0);
    SET_FUNCTION_INFO(require_recipient, 1);
    SET_FUNCTION_INFO(require_auth, 1);
    SET_FUNCTION_INFO(require_auth2, 2);
    SET_FUNCTION_INFO(has_auth, 1);
    SET_FUNCTION_INFO(is_account, 1);
    SET_FUNCTION_INFO(send_inline, 1);
    SET_FUNCTION_INFO(send_context_free_inline, 1);
    SET_FUNCTION_INFO(publication_time, 0);
    SET_FUNCTION_INFO(current_receiver, 0);
    SET_FUNCTION_INFO(get_active_producers, 1);
    SET_FUNCTION_INFO(assert_sha256, 2);
    SET_FUNCTION_INFO(assert_sha1, 2);
    SET_FUNCTION_INFO(assert_sha512, 2);
    SET_FUNCTION_INFO(assert_ripemd160, 2);
    SET_FUNCTION_INFO(assert_recover_key, 3);
    SET_FUNCTION_INFO(sha256, 2);
    SET_FUNCTION_INFO(sha1, 2);
    SET_FUNCTION_INFO(sha512, 2);
    SET_FUNCTION_INFO(ripemd160, 2);
    SET_FUNCTION_INFO(recover_key, 3);
    SET_FUNCTION_INFO(db_store_i64, 5);
    SET_FUNCTION_INFO(db_update_i64, 3);
    SET_FUNCTION_INFO(db_remove_i64, 1);
    SET_FUNCTION_INFO(db_get_i64, 2);
    SET_FUNCTION_INFO(db_next_i64, 2);
    SET_FUNCTION_INFO(db_previous_i64, 2);
    SET_FUNCTION_INFO(db_find_i64, 4);
    SET_FUNCTION_INFO(db_lowerbound_i64, 4);
    SET_FUNCTION_INFO(db_upperbound_i64, 4);
    SET_FUNCTION_INFO(db_end_i64, 3);
    SET_FUNCTION_INFO(db_store_i256, 5);
    SET_FUNCTION_INFO(db_update_i256, 3);
    SET_FUNCTION_INFO(db_remove_i256, 1);
    SET_FUNCTION_INFO(db_get_i256, 2);
    SET_FUNCTION_INFO(db_find_i256, 4);
    SET_FUNCTION_INFO(db_previous_i256, 2);
    SET_FUNCTION_INFO(db_next_i256, 2);
    SET_FUNCTION_INFO(db_upperbound_i256, 4);
    SET_FUNCTION_INFO(db_lowerbound_i256, 4);
    SET_FUNCTION_INFO(db_end_i256, 3);
    SET_FUNCTION_INFO(db_get_table_row_count, 3);
    SET_FUNCTION_INFO(db_idx64_store, 5);
    SET_FUNCTION_INFO(db_idx64_update, 3);
    SET_FUNCTION_INFO(db_idx64_remove, 1);
    SET_FUNCTION_INFO(db_idx64_next, 2);
    SET_FUNCTION_INFO(db_idx64_previous, 2);
    SET_FUNCTION_INFO(db_idx64_find_primary, 5);
    SET_FUNCTION_INFO(db_idx64_find_secondary, 5);
    SET_FUNCTION_INFO(db_idx64_lowerbound, 5);
    SET_FUNCTION_INFO(db_idx64_upperbound, 5);
    SET_FUNCTION_INFO(db_idx64_end, 3);
    SET_FUNCTION_INFO(db_idx128_store, 5);
    SET_FUNCTION_INFO(db_idx128_update, 3);
    SET_FUNCTION_INFO(db_idx128_remove, 1);
    SET_FUNCTION_INFO(db_idx128_next, 2);
    SET_FUNCTION_INFO(db_idx128_previous, 2);
    SET_FUNCTION_INFO(db_idx128_find_primary, 5);
    SET_FUNCTION_INFO(db_idx128_find_secondary, 5);
    SET_FUNCTION_INFO(db_idx128_lowerbound, 5);
    SET_FUNCTION_INFO(db_idx128_upperbound, 5);
    SET_FUNCTION_INFO(db_idx128_end, 3);
    SET_FUNCTION_INFO(db_idx256_store, 5);
    SET_FUNCTION_INFO(db_idx256_update, 3);
    SET_FUNCTION_INFO(db_idx256_remove, 1);
    SET_FUNCTION_INFO(db_idx256_next, 2);
    SET_FUNCTION_INFO(db_idx256_previous, 2);
    SET_FUNCTION_INFO(db_idx256_find_primary, 5);
    SET_FUNCTION_INFO(db_idx256_find_secondary, 5);
    SET_FUNCTION_INFO(db_idx256_lowerbound, 5);
    SET_FUNCTION_INFO(db_idx256_upperbound, 5);
    SET_FUNCTION_INFO(db_idx256_end, 3);
    SET_FUNCTION_INFO(db_idx_double_store, 5);
    SET_FUNCTION_INFO(db_idx_double_update, 3);
    SET_FUNCTION_INFO(db_idx_double_remove, 1);
    SET_FUNCTION_INFO(db_idx_double_next, 2);
    SET_FUNCTION_INFO(db_idx_double_previous, 2);
    SET_FUNCTION_INFO(db_idx_double_find_primary, 5);
    SET_FUNCTION_INFO(db_idx_double_find_secondary, 5);
    SET_FUNCTION_INFO(db_idx_double_lowerbound, 5);
    SET_FUNCTION_INFO(db_idx_double_upperbound, 5);
    SET_FUNCTION_INFO(db_idx_double_end, 3);
    SET_FUNCTION_INFO(db_idx_long_double_store, 5);
    SET_FUNCTION_INFO(db_idx_long_double_update, 3);
    SET_FUNCTION_INFO(db_idx_long_double_remove, 1);
    SET_FUNCTION_INFO(db_idx_long_double_next, 2);
    SET_FUNCTION_INFO(db_idx_long_double_previous, 2);
    SET_FUNCTION_INFO(db_idx_long_double_find_primary, 5);
    SET_FUNCTION_INFO(db_idx_long_double_find_secondary, 5);
    SET_FUNCTION_INFO(db_idx_long_double_lowerbound, 5);
    SET_FUNCTION_INFO(db_idx_long_double_upperbound, 5);
    SET_FUNCTION_INFO(db_idx_long_double_end, 3);
    SET_FUNCTION_INFO(check_transaction_authorization, 3);
    SET_FUNCTION_INFO(check_permission_authorization, 5);
    SET_FUNCTION_INFO(get_permission_last_used, 2);
    SET_FUNCTION_INFO(get_account_creation_time, 1);
    SET_FUNCTION_INFO(prints, 1);
    SET_FUNCTION_INFO(prints_l, 1);
    SET_FUNCTION_INFO(printi, 1);
    SET_FUNCTION_INFO(printui, 1);
    SET_FUNCTION_INFO(printi128, 1);
    SET_FUNCTION_INFO(printui128, 1);
    SET_FUNCTION_INFO(printsf, 1);
    SET_FUNCTION_INFO(printdf, 1);
    SET_FUNCTION_INFO(printqf, 1);
    SET_FUNCTION_INFO(printn, 1);
    SET_FUNCTION_INFO(printhex, 1);
    SET_FUNCTION_INFO(set_resource_limits, 4);
    SET_FUNCTION_INFO(get_resource_limits, 4);
    SET_FUNCTION_INFO(set_proposed_producers, 1);
    SET_FUNCTION_INFO(set_proposed_producers_ex, 2);
    SET_FUNCTION_INFO(is_privileged, 1);
    SET_FUNCTION_INFO(set_privileged, 2);
    SET_FUNCTION_INFO(set_blockchain_parameters_packed, 1);
    SET_FUNCTION_INFO(get_blockchain_parameters_packed, 1);
    SET_FUNCTION_INFO(activate_feature, 1);
    SET_FUNCTION_INFO(eosio_abort, 0);
    SET_FUNCTION_INFO(eosio_assert, 2);
    SET_FUNCTION_INFO(eosio_assert_message, 2);
    SET_FUNCTION_INFO(eosio_assert_code, 2);
    SET_FUNCTION_INFO(eosio_exit, 1);
    SET_FUNCTION_INFO(current_time, 0);
    SET_FUNCTION_INFO(now, 0);
    SET_FUNCTION_INFO(checktime, 0);
    SET_FUNCTION_INFO(check_context_free, 1);
    SET_FUNCTION_INFO(send_deferred, 4);
    SET_FUNCTION_INFO(cancel_deferred, 1);
    SET_FUNCTION_INFO(read_transaction, 1);
    SET_FUNCTION_INFO(transaction_size, 0);
    SET_FUNCTION_INFO(tapos_block_num, 0);
    SET_FUNCTION_INFO(tapos_block_prefix, 0);
    SET_FUNCTION_INFO(expiration, 0);
    SET_FUNCTION_INFO(get_action, 3);
    SET_FUNCTION_INFO(assert_privileged, 0);
    SET_FUNCTION_INFO(assert_context_free, 0);
    SET_FUNCTION_INFO(get_context_free_data, 2);
    SET_FUNCTION_INFO(token_create, 3);
    SET_FUNCTION_INFO(token_issue, 4);
    SET_FUNCTION_INFO(token_transfer, 5);
    SET_FUNCTION_INFO(token_open, 3);
    SET_FUNCTION_INFO(token_retire, 3);
    SET_FUNCTION_INFO(token_close, 2);
    SET_FUNCTION_INFO(call_contract_get_args, 1);
    SET_FUNCTION_INFO(call_contract_set_results, 1);
    SET_FUNCTION_INFO(call_contract_get_results, 1);
    SET_FUNCTION_INFO(call_contract, 2);
    SET_FUNCTION_INFO(is_feature_activated, 1);
    SET_FUNCTION_INFO(preactivate_feature, 1);
    SET_FUNCTION_INFO(get_sender, 0);

    init = 1;
}

static void print_hex(char *data, size_t size) {
  for (int i=0;i<size;i++) {
    printf("%02x", data[i]);
  }
  printf("\n");
}

extern "C" int call_vm_api(vm_api_function_type function_type,  vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret) {
    static int initialized = 0;
    if (!initialized) {
        init_function_args();
        initialized = 1;
    }

    get_vm_api()->eosio_assert(function_type < enum_vm_api_function_count && function_type >= 0, "bad function type");

    printf("++++++++++function name:%d %s\n", function_type, s_vm_api_functions[function_type].name);

    auto& api = s_vm_api_functions[function_type];
    if (api.arg_count != args_count) {
        printf("++++++++++function_type:%d %d %d\n", function_type, s_vm_api_functions[function_type].arg_count, args_count);
        // print_hex((char *)input, input_size);
        get_vm_api()->eosio_assert(false, "bad argument count!");
        return 0;
    }
    
    api.fn(args, args_count, vm_ret);

}
