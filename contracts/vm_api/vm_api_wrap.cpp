#include "vm_api_wrap.h"
#include "vm_api.h"
#include <string.h>
#include <stdio.h>

#define DB_SECONDARY_INDEX_METHODS_SIMPLE(IDX, IDX_TYPE) \
    case enum_db_##IDX##_store:                \
    {                                                  \
        uint64_t scope = *(uint64_t *)args[0].ptr;   \
        uint64_t table = *(uint64_t *)args[1].ptr;    \
        uint64_t payer = *(uint64_t *)args[2].ptr; \
        uint64_t id = *(uint64_t *)args[3].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_store(scope, table, payer, id, (IDX_TYPE *)args[4].ptr ); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    }                                          \
        break;                                 \
    case enum_db_##IDX##_update:               \
        get_vm_api()->db_##IDX##_update(*(int32_t *)args[0].ptr, *(uint64_t *)args[1].ptr, (IDX_TYPE *)args[2].ptr); \
        break;                                 \
    case enum_db_##IDX##_remove:               \
    { \
        get_vm_api()->db_##IDX##_remove(*(int32_t *)args[0].ptr); \
    } \
        break;                                 \
    case enum_db_##IDX##_next:                 \
    { \
        int32_t itr = get_vm_api()->db_##IDX##_next(*(int32_t *)args[0].ptr, (uint64_t *)args[1].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
        break;                                 \
    case enum_db_##IDX##_previous:             \
    { \
        int32_t itr = get_vm_api()->db_##IDX##_previous(*(int32_t *)args[0].ptr, (uint64_t *)args[1].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
        break; \
    case enum_db_##IDX##_find_primary:         \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        uint64_t primary = *(uint64_t *)args[4].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_find_primary(code, scope, table, (IDX_TYPE *)args[3].ptr, primary); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
        break;                                 \
    case enum_db_##IDX##_find_secondary:       \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_find_primary(code, scope, table, (IDX_TYPE *)args[3].ptr, *(uint64_t *)args[4].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
        break;                                 \
    case enum_db_##IDX##_lowerbound:           \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_lowerbound(code, scope, table, (IDX_TYPE *)args[3].ptr, (uint64_t *)args[4].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
        break;                                 \
    case enum_db_##IDX##_upperbound:           \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_upperbound(code, scope, table, (IDX_TYPE *)args[3].ptr, (uint64_t *)args[4].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
        break;                                 \
    case enum_db_##IDX##_end:                  \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_end(code, scope, table); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
        break;                                 \


#define DB_SECONDARY_INDEX_METHODS_ARRAY(IDX, ARR_TYPE, ARR_SIZE) \
    case enum_db_##IDX##_store:                \
    {                                                  \
        uint64_t scope = *(uint64_t *)args[0].ptr;   \
        uint64_t table = *(uint64_t *)args[1].ptr;    \
        uint64_t payer = *(uint64_t *)args[2].ptr; \
        uint64_t id = *(uint64_t *)args[3].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_store(scope, table, payer, id, (ARR_TYPE *)args[4].ptr, 2); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    }                                          \
        break;                                 \
    case enum_db_##IDX##_update:               \
        get_vm_api()->db_##IDX##_update(*(int32_t *)args[0].ptr, *(uint64_t *)args[1].ptr, args[2].ptr, args[2].size); \
        break;                                 \
    case enum_db_##IDX##_remove:               \
    { \
        get_vm_api()->db_##IDX##_remove(*(int32_t *)args[0].ptr); \
    } \
        break;                                 \
    case enum_db_##IDX##_next:                 \
    { \
        int32_t itr = get_vm_api()->db_##IDX##_next(*(int32_t *)args[0].ptr, (uint64_t *)args[1].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
        break;                                 \
    case enum_db_##IDX##_previous:             \
    { \
        int32_t itr = get_vm_api()->db_##IDX##_previous(*(int32_t *)args[0].ptr, (uint64_t *)args[1].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
        break; \
    case enum_db_##IDX##_find_primary:         \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        uint64_t primary = *(uint64_t *)args[4].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_find_primary(code, scope, table, (ARR_TYPE *)args[3].ptr, 2, primary); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
        break;                                 \
    case enum_db_##IDX##_find_secondary:       \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_find_primary(code, scope, table, (ARR_TYPE *)args[3].ptr, 2, *(uint64_t *)args[4].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
        break;                                 \
    case enum_db_##IDX##_lowerbound:           \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_lowerbound(code, scope, table, (ARR_TYPE *)args[3].ptr, 2, (uint64_t *)args[4].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
        break;                                 \
    case enum_db_##IDX##_upperbound:           \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_upperbound(code, scope, table, (ARR_TYPE *)args[3].ptr, 2, (uint64_t *)args[4].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
        break;                                 \
    case enum_db_##IDX##_end:                  \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_end(code, scope, table); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
        break;

static int function_arg_count[enum_vm_api_function_count];

#define SET_FUNCTION_ARG_COUNT(FN, COUNT) \
    function_arg_count[enum_##FN] = COUNT;

void init_function_args() {
    static int init = false;
    if (init) {
        return;
    }
    memset(function_arg_count, 0, sizeof(function_arg_count));

    SET_FUNCTION_ARG_COUNT(read_action_data, 1);
    SET_FUNCTION_ARG_COUNT(require_recipient, 1);
    SET_FUNCTION_ARG_COUNT(require_auth, 1);
    SET_FUNCTION_ARG_COUNT(require_auth2, 2);
    SET_FUNCTION_ARG_COUNT(has_auth, 1);
    SET_FUNCTION_ARG_COUNT(is_account, 1);
    SET_FUNCTION_ARG_COUNT(send_inline, 1);
    SET_FUNCTION_ARG_COUNT(send_context_free_inline, 1);
    SET_FUNCTION_ARG_COUNT(publication_time, 0);
    SET_FUNCTION_ARG_COUNT(current_receiver, 0);
    SET_FUNCTION_ARG_COUNT(get_active_producers, 1);
    SET_FUNCTION_ARG_COUNT(assert_sha256, 2);
    SET_FUNCTION_ARG_COUNT(assert_sha1, 2);
    SET_FUNCTION_ARG_COUNT(assert_sha512, 2);
    SET_FUNCTION_ARG_COUNT(assert_ripemd160, 2);
    SET_FUNCTION_ARG_COUNT(assert_recover_key, 3);
    SET_FUNCTION_ARG_COUNT(sha256, 2);
    SET_FUNCTION_ARG_COUNT(sha1, 2);
    SET_FUNCTION_ARG_COUNT(sha512, 2);
    SET_FUNCTION_ARG_COUNT(ripemd160, 2);
    SET_FUNCTION_ARG_COUNT(recover_key, 3);
    SET_FUNCTION_ARG_COUNT(db_store_i64, 5);
    SET_FUNCTION_ARG_COUNT(db_update_i64, 3);
    SET_FUNCTION_ARG_COUNT(db_remove_i64, 1);
    SET_FUNCTION_ARG_COUNT(db_get_i64, 2);
    SET_FUNCTION_ARG_COUNT(db_next_i64, 2);
    SET_FUNCTION_ARG_COUNT(db_previous_i64, 2);
    SET_FUNCTION_ARG_COUNT(db_find_i64, 4);
    SET_FUNCTION_ARG_COUNT(db_lowerbound_i64, 4);
    SET_FUNCTION_ARG_COUNT(db_upperbound_i64, 4);
    SET_FUNCTION_ARG_COUNT(db_end_i64, 3);
    SET_FUNCTION_ARG_COUNT(db_store_i256, 5);
    SET_FUNCTION_ARG_COUNT(db_update_i256, 3);
    SET_FUNCTION_ARG_COUNT(db_remove_i256, 1);
    SET_FUNCTION_ARG_COUNT(db_get_i256, 2);
    SET_FUNCTION_ARG_COUNT(db_find_i256, 4);
    SET_FUNCTION_ARG_COUNT(db_previous_i256, 2);
    SET_FUNCTION_ARG_COUNT(db_next_i256, 2);
    SET_FUNCTION_ARG_COUNT(db_upperbound_i256, 4);
    SET_FUNCTION_ARG_COUNT(db_lowerbound_i256, 4);
    SET_FUNCTION_ARG_COUNT(db_end_i256, 3);
    SET_FUNCTION_ARG_COUNT(db_get_table_count, 3);
    SET_FUNCTION_ARG_COUNT(db_idx64_store, 5);
    SET_FUNCTION_ARG_COUNT(db_idx64_update, 3);
    SET_FUNCTION_ARG_COUNT(db_idx64_remove, 1);
    SET_FUNCTION_ARG_COUNT(db_idx64_next, 2);
    SET_FUNCTION_ARG_COUNT(db_idx64_previous, 2);
    SET_FUNCTION_ARG_COUNT(db_idx64_find_primary, 5);
    SET_FUNCTION_ARG_COUNT(db_idx64_find_secondary, 5);
    SET_FUNCTION_ARG_COUNT(db_idx64_lowerbound, 5);
    SET_FUNCTION_ARG_COUNT(db_idx64_upperbound, 5);
    SET_FUNCTION_ARG_COUNT(db_idx64_end, 3);
    SET_FUNCTION_ARG_COUNT(db_idx128_store, 5);
    SET_FUNCTION_ARG_COUNT(db_idx128_update, 3);
    SET_FUNCTION_ARG_COUNT(db_idx128_remove, 1);
    SET_FUNCTION_ARG_COUNT(db_idx128_next, 2);
    SET_FUNCTION_ARG_COUNT(db_idx128_previous, 2);
    SET_FUNCTION_ARG_COUNT(db_idx128_find_primary, 5);
    SET_FUNCTION_ARG_COUNT(db_idx128_find_secondary, 5);
    SET_FUNCTION_ARG_COUNT(db_idx128_lowerbound, 5);
    SET_FUNCTION_ARG_COUNT(db_idx128_upperbound, 5);
    SET_FUNCTION_ARG_COUNT(db_idx128_end, 3);
    SET_FUNCTION_ARG_COUNT(db_idx256_store, 5);
    SET_FUNCTION_ARG_COUNT(db_idx256_update, 3);
    SET_FUNCTION_ARG_COUNT(db_idx256_remove, 1);
    SET_FUNCTION_ARG_COUNT(db_idx256_next, 2);
    SET_FUNCTION_ARG_COUNT(db_idx256_previous, 2);
    SET_FUNCTION_ARG_COUNT(db_idx256_find_primary, 5);
    SET_FUNCTION_ARG_COUNT(db_idx256_find_secondary, 5);
    SET_FUNCTION_ARG_COUNT(db_idx256_lowerbound, 5);
    SET_FUNCTION_ARG_COUNT(db_idx256_upperbound, 5);
    SET_FUNCTION_ARG_COUNT(db_idx256_end, 3);
    SET_FUNCTION_ARG_COUNT(db_idx_double_store, 5);
    SET_FUNCTION_ARG_COUNT(db_idx_double_update, 3);
    SET_FUNCTION_ARG_COUNT(db_idx_double_remove, 1);
    SET_FUNCTION_ARG_COUNT(db_idx_double_next, 2);
    SET_FUNCTION_ARG_COUNT(db_idx_double_previous, 2);
    SET_FUNCTION_ARG_COUNT(db_idx_double_find_primary, 5);
    SET_FUNCTION_ARG_COUNT(db_idx_double_find_secondary, 5);
    SET_FUNCTION_ARG_COUNT(db_idx_double_lowerbound, 5);
    SET_FUNCTION_ARG_COUNT(db_idx_double_upperbound, 5);
    SET_FUNCTION_ARG_COUNT(db_idx_double_end, 3);
    SET_FUNCTION_ARG_COUNT(db_idx_long_double_store, 5);
    SET_FUNCTION_ARG_COUNT(db_idx_long_double_update, 3);
    SET_FUNCTION_ARG_COUNT(db_idx_long_double_remove, 1);
    SET_FUNCTION_ARG_COUNT(db_idx_long_double_next, 2);
    SET_FUNCTION_ARG_COUNT(db_idx_long_double_previous, 2);
    SET_FUNCTION_ARG_COUNT(db_idx_long_double_find_primary, 5);
    SET_FUNCTION_ARG_COUNT(db_idx_long_double_find_secondary, 5);
    SET_FUNCTION_ARG_COUNT(db_idx_long_double_lowerbound, 5);
    SET_FUNCTION_ARG_COUNT(db_idx_long_double_upperbound, 5);
    SET_FUNCTION_ARG_COUNT(db_idx_long_double_end, 3);
    SET_FUNCTION_ARG_COUNT(check_transaction_authorization, 3);
    SET_FUNCTION_ARG_COUNT(check_permission_authorization, 5);
    SET_FUNCTION_ARG_COUNT(get_permission_last_used, 2);
    SET_FUNCTION_ARG_COUNT(get_account_creation_time, 1);
    SET_FUNCTION_ARG_COUNT(prints, 1);
    SET_FUNCTION_ARG_COUNT(prints_l, 1);
    SET_FUNCTION_ARG_COUNT(printi, 1);
    SET_FUNCTION_ARG_COUNT(printui, 1);
    SET_FUNCTION_ARG_COUNT(printi128, 1);
    SET_FUNCTION_ARG_COUNT(printui128, 1);
    SET_FUNCTION_ARG_COUNT(printsf, 1);
    SET_FUNCTION_ARG_COUNT(printdf, 1);
    SET_FUNCTION_ARG_COUNT(printqf, 1);
    SET_FUNCTION_ARG_COUNT(printn, 1);
    SET_FUNCTION_ARG_COUNT(printhex, 1);
    SET_FUNCTION_ARG_COUNT(set_resource_limits, 4);
    SET_FUNCTION_ARG_COUNT(get_resource_limits, 4);
    SET_FUNCTION_ARG_COUNT(set_proposed_producers, 1);
    SET_FUNCTION_ARG_COUNT(set_proposed_producers_ex, 2);
    SET_FUNCTION_ARG_COUNT(is_privileged, 1);
    SET_FUNCTION_ARG_COUNT(set_privileged, 2);
    SET_FUNCTION_ARG_COUNT(set_blockchain_parameters_packed, 1);
    SET_FUNCTION_ARG_COUNT(get_blockchain_parameters_packed, 1);
    SET_FUNCTION_ARG_COUNT(activate_feature, 1);
    SET_FUNCTION_ARG_COUNT(eosio_abort, 0);
    SET_FUNCTION_ARG_COUNT(eosio_assert, 2);
    SET_FUNCTION_ARG_COUNT(eosio_assert_message, 2);
    SET_FUNCTION_ARG_COUNT(eosio_assert_code, 2);
    SET_FUNCTION_ARG_COUNT(eosio_exit, 1);
    SET_FUNCTION_ARG_COUNT(current_time, 0);
    SET_FUNCTION_ARG_COUNT(now, 0);
    SET_FUNCTION_ARG_COUNT(checktime, 0);
    SET_FUNCTION_ARG_COUNT(check_context_free, 1);
    SET_FUNCTION_ARG_COUNT(send_deferred, 4);
    SET_FUNCTION_ARG_COUNT(cancel_deferred, 1);
    SET_FUNCTION_ARG_COUNT(read_transaction, 1);
    SET_FUNCTION_ARG_COUNT(transaction_size, 0);
    SET_FUNCTION_ARG_COUNT(tapos_block_num, 0);
    SET_FUNCTION_ARG_COUNT(tapos_block_prefix, 0);
    SET_FUNCTION_ARG_COUNT(expiration, 0);
    SET_FUNCTION_ARG_COUNT(get_action, 3);
    SET_FUNCTION_ARG_COUNT(assert_privileged, 0);
    SET_FUNCTION_ARG_COUNT(assert_context_free, 0);
    SET_FUNCTION_ARG_COUNT(get_context_free_data, 2);
    SET_FUNCTION_ARG_COUNT(token_create, 3);
    SET_FUNCTION_ARG_COUNT(token_issue, 4);
    SET_FUNCTION_ARG_COUNT(token_transfer, 5);
    SET_FUNCTION_ARG_COUNT(token_open, 3);
    SET_FUNCTION_ARG_COUNT(token_retire, 3);
    SET_FUNCTION_ARG_COUNT(token_close, 2);
    SET_FUNCTION_ARG_COUNT(call_contract_get_args, 1);
    SET_FUNCTION_ARG_COUNT(call_contract_set_results, 1);
    SET_FUNCTION_ARG_COUNT(call_contract_get_results, 1);
    SET_FUNCTION_ARG_COUNT(call_contract, 2);
    SET_FUNCTION_ARG_COUNT(is_feature_activated, 1);
    SET_FUNCTION_ARG_COUNT(preactivate_feature, 1);
    SET_FUNCTION_ARG_COUNT(get_sender, 0);

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
    
    if (function_arg_count[function_type] != args_count) {
        // printf("++++++++++function_type:%d %d %d\n", function_type, function_arg_count[function_type], args_count);
        // print_hex((char *)input, input_size);
        get_vm_api()->eosio_assert(false, "bad argument count!");
        return 0;
    }

    switch(function_type) {
        case enum_read_action_data:
        {
            uint32_t size = get_vm_api()->read_action_data(args[0].ptr, args[0].size);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_u32;
            vm_ret->u32 = size;
        }
            break;
        case enum_action_data_size:
        {
            uint32_t size = get_vm_api()->action_data_size();
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_u32;
            vm_ret->u32 = size;
        }
            break;
        case enum_require_recipient:
        {
            if (args_count != 1) {
                return 0;
            }
            if (args[0].size != sizeof(uint64_t)) {
                return 0;
            }
            get_vm_api()->require_recipient(args[0].u64);
        }
            break;
        case enum_require_auth:
        {
            if (args_count != 1) {
                return 0;
            }
            if (args[0].size != sizeof(uint64_t)) {
                return 0;
            }
            get_vm_api()->require_auth(args[0].u64);
        }
            break;
        case enum_require_auth2:
        {
            if (args_count != 2) {
                return 0;
            }
            
            if (args[0].size != sizeof(uint64_t)) {
                return 0;
            }
            
            if (args[1].size != sizeof(uint64_t)) {
                return 0;
            }
            get_vm_api()->require_auth2(args[0].u64, args[1].u64);
        }
            break;
        case enum_has_auth:
        case enum_is_account:
        {
            if (args_count != 1) {
                return 0;
            }
            
            if (args[0].size != sizeof(uint64_t)) {
                return 0;
            }
            
            bool ret = false;
            if (function_type == enum_is_account) {
                ret = get_vm_api()->is_account(args[0].u64);
            } else if (function_type == enum_has_auth) {
                ret = get_vm_api()->has_auth(args[0].u64);
            }
            vm_ret->size = 1;
            vm_ret->u8 = ret;
            vm_ret->type = enum_arg_type_u8;
        }
            break;
        case enum_send_inline:
        case enum_send_context_free_inline:
        {
            if (args_count != 1) {
                return 0;
            }
            if (function_type == enum_send_inline) {
                get_vm_api()->send_inline((char *)args[0].ptr, args[0].size);
            } else {
                get_vm_api()->send_context_free_inline((char *)args[0].ptr, args[0].size);
            }
        }
            break;
        case enum_publication_time:
        {
            uint64_t pub_time = get_vm_api()->publication_time();
            vm_ret->size = 8;
            vm_ret->type = enum_arg_type_u64;
            vm_ret->u64 = pub_time;
        }
            break;
        case enum_current_receiver:
        {
            uint64_t receiver = get_vm_api()->current_receiver();
            vm_ret->size = 8;
            vm_ret->type = enum_arg_type_u64;
            vm_ret->u64 = receiver;
        }
            break;
        case enum_get_active_producers:
        {
            uint32_t ret = get_vm_api()->get_active_producers((uint64_t *)args[0].ptr, args[0].size);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = ret;
        }
            break;
        case enum_assert_sha256:
        {
            get_vm_api()->assert_sha256((char *)args[0].ptr, args[0].size, (checksum256 *)args[1].ptr);
        }
            break;
        case enum_assert_sha1:
        {
            get_vm_api()->assert_sha1((char *)args[0].ptr, args[0].size, (checksum160 *)args[1].ptr);
        }
            break;
        case enum_assert_sha512:
        {
            get_vm_api()->assert_sha512((char *)args[0].ptr, args[0].size, (checksum512 *)args[1].ptr);
        }
            break;
        case enum_assert_ripemd160:
        {
            get_vm_api()->assert_ripemd160((char *)args[0].ptr, args[0].size, (checksum160 *)args[1].ptr);
        }
            break;
        case enum_assert_recover_key:
        {
            get_vm_api()->assert_recover_key((checksum256 *)args[0].ptr, (char *)args[1].ptr, args[1].size, (char *)args[2].ptr, args[2].size);
        }
            break;
        case enum_sha256:
        {
            if (args[1].size < 32) {
                return 0;
            }
            get_vm_api()->sha256((char *)args[0].ptr, args[0].size, (checksum256 *)args[1].ptr);            
        }
            break;
        case enum_sha1:
        {
            if (args[1].size < 20) {
                return 0;
            }
            get_vm_api()->sha1((char *)args[0].ptr, args[0].size, (checksum160 *)args[1].ptr);
        }
            break;
        case enum_sha512:
        {
            if (args[1].size < 64) {
                return 0;
            }
            get_vm_api()->sha512((char *)args[0].ptr, args[0].size, (checksum512 *)args[1].ptr);
        }
            break;
        case enum_ripemd160:
        {
            get_vm_api()->ripemd160((char *)args[0].ptr, args[0].size, (checksum160 *)args[1].ptr);
        }
            break;
        case enum_recover_key:
        {
            int ret = get_vm_api()->recover_key((checksum256*)args[0].ptr, (char *)args[1].ptr, args[1].size, (char *)args[2].ptr, args[2].size);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = ret;
        }
            break;
        case enum_sha3:
        {
            get_vm_api()->sha3((char *)args[0].ptr, args[0].size, (char *)args[1].ptr, args[1].size);
        }
            break;
        case enum_db_store_i64:
        {
            uint64_t scope = args[0].u64;
            uint64_t table = args[1].u64;
            uint64_t payer = args[2].u64;
            uint64_t id = args[3].u64;
            int32_t itr = get_vm_api()->db_store_i64(scope, table, payer, id, (char *)args[4].ptr, args[4].size);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = itr;
        }
            break;
        case enum_db_update_i64:
        {
            int32_t iterator = args[0].i32;
            uint64_t payer = args[1].u64;
            get_vm_api()->db_update_i64(iterator, payer, (char *)args[2].ptr, args[2].size);
        }
            break;
        case enum_db_remove_i64:
        {
            get_vm_api()->db_remove_i64(args[0].i32);
        }
            break;
        case enum_db_get_i64:
        {
            int32_t itr = args[0].i32;
            int32_t size = get_vm_api()->db_get_i64(itr, (char *)args[1].ptr, args[1].size);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = size;
        }
            break;
        case enum_db_next_i64:
        {
            int32_t itr = args[0].i32;
            itr = get_vm_api()->db_next_i64(itr, (uint64_t *)args[1].ptr);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = itr;
        }
            break;
        case enum_db_previous_i64:
        {
            int32_t itr = args[0].i32;
            itr = get_vm_api()->db_previous_i64(itr, (uint64_t *)args[1].ptr);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = itr;
        }
            break;
        case enum_db_find_i64:
        {
            int32_t itr = get_vm_api()->db_find_i64(args[0].u64, args[1].u64, args[2].u64, args[3].u64);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = itr;
        }
            break;
        case enum_db_lowerbound_i64:
        {
            int32_t itr = get_vm_api()->db_lowerbound_i64(args[0].u64, args[1].u64, args[2].u64, args[3].u64);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = itr;
        }
            break;
        case enum_db_upperbound_i64:
        {
            int32_t itr = get_vm_api()->db_upperbound_i64(args[0].u64, args[1].u64, args[2].u64, args[3].u64);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = itr;
        }
            break;
        case enum_db_end_i64:
        {
            int32_t itr = get_vm_api()->db_end_i64(args[0].u64, args[1].u64, args[2].u64);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = itr;
        }
            break;
        case enum_db_store_i256:
        {
            uint64_t scope = args[0].u64;
            uint64_t table = args[1].u64;
            uint64_t payer = args[2].u64;
            int32_t itr = get_vm_api()->db_store_i256(scope, table, payer, args[3].ptr, args[3].size, (char *)args[4].ptr, args[4].size);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = itr;
        }
            break;
        case enum_db_update_i256:
        {
            int32_t iterator = args[0].i32;
            uint64_t payer = args[1].u64;
            get_vm_api()->db_update_i256(iterator, payer, (char *)args[2].ptr, args[2].size);
        }
            break;
        case enum_db_remove_i256:
        {
            get_vm_api()->db_remove_i256(args[0].i32);
        }
            break;
        case enum_db_get_i256:
        {
            int32_t itr = args[0].i32;
            int32_t size = get_vm_api()->db_get_i256(itr, (char *)args[1].ptr, args[1].size);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = size;
        }
            break;
        case enum_db_next_i256:
        {
            int32_t itr = args[0].i32;
            itr = get_vm_api()->db_next_i256(itr, args[1].ptr, args[1].size);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = itr;
        }
            break;
        case enum_db_previous_i256:
        {
            int32_t itr = args[0].i32;
            itr = get_vm_api()->db_previous_i256(itr, args[1].ptr, args[1].size);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = itr;
        }
            break;
        case enum_db_find_i256:
        {
            int32_t itr = get_vm_api()->db_find_i256(args[0].u64, args[1].u64, args[2].u64, args[3].ptr, args[3].size);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = itr;
        }
            break;
        case enum_db_lowerbound_i256:
        {
            int32_t itr = get_vm_api()->db_lowerbound_i256(args[0].u64, args[1].u64, args[2].u64, args[3].ptr, args[3].size);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = itr;
        }
            break;
        case enum_db_upperbound_i256:
        {
            int32_t itr = get_vm_api()->db_upperbound_i256(args[0].u64, args[1].u64, args[2].u64, args[3].ptr, args[3].size);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = itr;
        }
            break;
        case enum_db_end_i256:
        {
            int32_t itr = get_vm_api()->db_end_i256(args[0].u64, args[1].u64, args[2].u64);
            vm_ret->size = 4;
            vm_ret->type = enum_arg_type_i32;
            vm_ret->i32 = itr;
        }
            break;
        case enum_db_get_table_count:
            break;

        DB_SECONDARY_INDEX_METHODS_SIMPLE(idx64, uint64_t)
        DB_SECONDARY_INDEX_METHODS_SIMPLE(idx128, __uint128)
        DB_SECONDARY_INDEX_METHODS_ARRAY(idx256, __uint128, 2)
        DB_SECONDARY_INDEX_METHODS_SIMPLE(idx_double, float64_t)
        DB_SECONDARY_INDEX_METHODS_SIMPLE(idx_long_double, float128_t)

        case enum_check_transaction_authorization:
            break;
        case enum_check_permission_authorization:
            break;
        case enum_get_permission_last_used:
            break;
        case enum_get_account_creation_time:
            break;
        case enum_prints:
            break;
        case enum_prints_l:
            break;
        case enum_printi:
            break;
        case enum_printui:
            break;
        case enum_printi128:
            break;
        case enum_printui128:
            break;
        case enum_printsf:
            break;
        case enum_printdf:
            break;
        case enum_printqf:
            break;
        case enum_printn:
            break;
        case enum_printhex:
            break;
        case enum_set_resource_limits:
            break;
        case enum_get_resource_limits:
            break;
        case enum_set_proposed_producers:
            break;
        case enum_set_proposed_producers_ex:
            break;
        case enum_is_privileged:
            break;
        case enum_set_privileged:
            break;
        case enum_set_blockchain_parameters_packed:
            break;
        case enum_get_blockchain_parameters_packed:
            break;
        case enum_activate_feature:
            break;
        case enum_eosio_abort:
            break;
        case enum_eosio_assert:
            break;
        case enum_eosio_assert_message:
            break;
        case enum_eosio_assert_code:
            break;
        case enum_eosio_exit:
            break;
        case enum_current_time:
            break;
        case enum_now:
            break;
        case enum_checktime:
            break;
        case enum_check_context_free:
            break;
        case enum_send_deferred:
            break;
        case enum_cancel_deferred:
            break;
        case enum_read_transaction:
            break;
        case enum_transaction_size:
            break;
        case enum_tapos_block_num:
            break;
        case enum_tapos_block_prefix:
            break;
        case enum_expiration:
            break;
        case enum_get_action:
            break;
        case enum_assert_privileged:
            break;
        case enum_assert_context_free:
            break;
        case enum_get_context_free_data:
            break;
        case enum_token_create:
            break;
        case enum_token_issue:
            break;
        case enum_token_transfer:
            break;
        case enum_token_open:
            break;
        case enum_token_retire:
            break;
        case enum_token_close:
            break;
        case enum_call_contract_get_args:
            break;
        case enum_call_contract_set_results:
            break;
        case enum_call_contract_get_results:
            break;
        case enum_call_contract:
            break;
        case enum_is_feature_activated:
            break;
        case enum_preactivate_feature:
            break;
        case enum_get_sender:
            break;
        case enum_call_native:
            break;
        default:
            break;
    }
    return 0;
}
