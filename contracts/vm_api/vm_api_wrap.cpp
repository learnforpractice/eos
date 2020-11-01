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
        pack_value(&itr, 4, output, output_size); \
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
        pack_value(&itr, 4, output, output_size); \
    } \
        break;                                 \
    case enum_db_##IDX##_previous:             \
    { \
        int32_t itr = get_vm_api()->db_##IDX##_previous(*(int32_t *)args[0].ptr, (uint64_t *)args[1].ptr); \
        pack_value(&itr, 4, output, output_size); \
    } \
        break; \
    case enum_db_##IDX##_find_primary:         \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        uint64_t primary = *(uint64_t *)args[4].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_find_primary(code, scope, table, (IDX_TYPE *)args[3].ptr, primary); \
        pack_value(&itr, 4, output, output_size); \
    } \
        break;                                 \
    case enum_db_##IDX##_find_secondary:       \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_find_primary(code, scope, table, (IDX_TYPE *)args[3].ptr, *(uint64_t *)args[4].ptr); \
        pack_value(&itr, 4, output, output_size); \
    } \
        break;                                 \
    case enum_db_##IDX##_lowerbound:           \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_lowerbound(code, scope, table, (IDX_TYPE *)args[3].ptr, (uint64_t *)args[4].ptr); \
        pack_value(&itr, 4, output, output_size); \
    } \
        break;                                 \
    case enum_db_##IDX##_upperbound:           \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_upperbound(code, scope, table, (IDX_TYPE *)args[3].ptr, (uint64_t *)args[4].ptr); \
        pack_value(&itr, 4, output, output_size); \
    } \
        break;                                 \
    case enum_db_##IDX##_end:                  \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_end(code, scope, table); \
        pack_value(&itr, 4, output, output_size); \
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
        pack_value(&itr, 4, output, output_size); \
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
        pack_value(&itr, 4, output, output_size); \
    } \
        break;                                 \
    case enum_db_##IDX##_previous:             \
    { \
        int32_t itr = get_vm_api()->db_##IDX##_previous(*(int32_t *)args[0].ptr, (uint64_t *)args[1].ptr); \
        pack_value(&itr, 4, output, output_size); \
    } \
        break; \
    case enum_db_##IDX##_find_primary:         \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        uint64_t primary = *(uint64_t *)args[4].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_find_primary(code, scope, table, (ARR_TYPE *)args[3].ptr, 2, primary); \
        pack_value(&itr, 4, output, output_size); \
    } \
        break;                                 \
    case enum_db_##IDX##_find_secondary:       \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_find_primary(code, scope, table, (ARR_TYPE *)args[3].ptr, 2, *(uint64_t *)args[4].ptr); \
        pack_value(&itr, 4, output, output_size); \
    } \
        break;                                 \
    case enum_db_##IDX##_lowerbound:           \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_lowerbound(code, scope, table, (ARR_TYPE *)args[3].ptr, 2, (uint64_t *)args[4].ptr); \
        pack_value(&itr, 4, output, output_size); \
    } \
        break;                                 \
    case enum_db_##IDX##_upperbound:           \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_upperbound(code, scope, table, (ARR_TYPE *)args[3].ptr, 2, (uint64_t *)args[4].ptr); \
        pack_value(&itr, 4, output, output_size); \
    } \
        break;                                 \
    case enum_db_##IDX##_end:                  \
    { \
        uint64_t code = *(uint64_t *)args[0].ptr;   \
        uint64_t scope = *(uint64_t *)args[1].ptr;    \
        uint64_t table = *(uint64_t *)args[2].ptr; \
        int32_t itr = get_vm_api()->db_##IDX##_end(code, scope, table); \
        pack_value(&itr, 4, output, output_size); \
    } \
        break;


static uint64_t align8(uint64_t size) {
   return (size + 7) & ~(uint64_t)7;
}

int verify_args(void *input, size_t input_size) {
    int n = 0;
    int pos = 0;

    while(pos + sizeof(uint64_t) < input_size) {
        uint64_t arg_size = ((uint64_t *)((char*)input+pos))[0];
        uint64_t aligned_arg_size = align8(arg_size);

        pos += sizeof(uint64_t);
        pos += aligned_arg_size;
        n += 1;
    }

    if (pos == input_size) {
        return n;
    }
    return 0;
}

int parse_args(void *input, size_t input_size, vm_api_arg *args, size_t args_count) {
    uint64_t pos = 0;
    if (input == nullptr || input_size <= sizeof(uint64_t)) {
        return 0;
    }

    int n = verify_args(input, input_size);
    if (n > args_count) {
        return 0;
    }

    for (int i=0;i<n;i++) {
        uint64_t arg_size = ((uint64_t *)((char*)input+pos))[0];
        uint64_t aligned_arg_size = align8(arg_size);

        args[i].ptr = (char *)input + pos + sizeof(uint64_t);
        args[i].size = arg_size;
        pos += sizeof(uint64_t);
        pos += aligned_arg_size;
    }
    return n;
}

static int pack_value(const void *value, size_t value_size, void *output, size_t output_size) {
    size_t aligned_value_size = align8(value_size);
    if (output_size < aligned_value_size + sizeof(uint64_t)) {
        return 0;
    }

    *((uint64_t *)output) = value_size;
    memcpy((char *)output + 8, value, value_size);
    return aligned_value_size + 8;
}

static void print_hex(char *data, size_t size) {
  for (int i=0;i<size;i++) {
    printf("%02x", data[i]);
  }
  printf("\n");
}

extern "C" int call_vm_api(vm_api_function_type function_type,  void *input, size_t input_size, void *output, size_t output_size) {
    vm_api_arg args[MAX_VM_API_ARGS];
    int args_count = parse_args(input, input_size, args, MAX_VM_API_ARGS);
    switch(function_type) {
        case enum_read_action_data:
        {
            uint32_t size = get_vm_api()->read_action_data(args[0].ptr, args[0].size);
            pack_value(&size, sizeof(size), output, output_size);
        }
            break;
        case enum_action_data_size:
        {
            uint32_t size = get_vm_api()->action_data_size();
            pack_value(&size, sizeof(size), output, output_size);
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
            get_vm_api()->require_recipient(*(uint64_t*)args[0].ptr);
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
            get_vm_api()->require_auth(*(uint64_t*)args[0].ptr);
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
            get_vm_api()->require_auth2(*(uint64_t*)args[0].ptr, *(uint64_t*)args[1].ptr);
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
            
            bool ret;
            if (function_type == enum_is_account) {
                ret = get_vm_api()->is_account(*(uint64_t*)args[0].ptr);
            } else if (function_type == enum_has_auth) {
                ret = get_vm_api()->has_auth(*(uint64_t*)args[0].ptr);
            }
            pack_value(&ret, 1, output, output_size);
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
            pack_value(&pub_time, sizeof(pub_time), output, output_size);            
        }
            break;
        case enum_current_receiver:
        {
            uint64_t receiver = get_vm_api()->current_receiver();
            pack_value(&receiver, sizeof(receiver), output, output_size);            
        }
            break;
        case enum_get_active_producers:
        {
            uint32_t ret = get_vm_api()->get_active_producers((uint64_t *)args[0].ptr, args[0].size);
            pack_value(&ret, 4, output, output_size);
        }
            break;
        case enum_assert_sha256:
        {
            get_vm_api()->eosio_assert(args_count == 2, "bad assert_sha256 argument count!");
            printf("+++++++%p %d %p, %d\n", args[0].ptr, args[0].size, args[1].ptr, args[1].size);
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
            get_vm_api()->eosio_assert(args_count == 2, "bad sha256 argument count!");
            printf("+++++++%p %d %p, %d\n", args[0].ptr, args[0].size, args[1].ptr, args[1].size);
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
            pack_value(&ret, sizeof(ret), output, output_size);
        }
            break;
        case enum_sha3:
        {
            get_vm_api()->sha3((char *)args[0].ptr, args[0].size, (char *)args[1].ptr, args[1].size);
        }
            break;
        case enum_db_store_i64:
        {
            uint64_t scope = *(uint64_t *)args[0].ptr;
            uint64_t table = *(uint64_t *)args[1].ptr;
            uint64_t payer = *(uint64_t *)args[2].ptr;
            uint64_t id = *(uint64_t *)args[3].ptr;
            int32_t itr = get_vm_api()->db_store_i64(scope, table, payer, id, (char *)args[4].ptr, args[4].size);
            pack_value(&itr, 4, output, output_size);
        }
            break;
        case enum_db_update_i64:
        {
            int32_t iterator = *(int32_t *)args[0].ptr;
            uint64_t payer = *(uint64_t *)args[1].ptr;
            get_vm_api()->db_update_i64(iterator, payer, (char *)args[2].ptr, args[2].size);
        }
            break;
        case enum_db_remove_i64:
        {
            get_vm_api()->db_remove_i64(*(int32_t*)args[0].ptr);
        }
            break;
        case enum_db_get_i64:
        {
            int32_t itr = *(int32_t *)args[0].ptr;
            get_vm_api()->db_get_i64(itr, (char *)args[1].ptr, args[1].size);
        }
            break;
        case enum_db_next_i64:
        {

        }
            break;
        case enum_db_previous_i64:
            break;
        case enum_db_find_i64:
            break;
        case enum_db_lowerbound_i64:
            break;
        case enum_db_upperbound_i64:
            break;
        case enum_db_end_i64:
            break;
        case enum_db_store_i256:
            break;
        case enum_db_update_i256:
            break;
        case enum_db_remove_i256:
            break;
        case enum_db_get_i256:
            break;
        case enum_db_find_i256:
            break;
        case enum_db_previous_i256:
            break;
        case enum_db_next_i256:
            break;
        case enum_db_upperbound_i256:
            break;
        case enum_db_lowerbound_i256:
            break;
        case enum_db_end_i256:
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
    }
    return 0;
}
