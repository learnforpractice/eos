#include <capi/types.h>
#include <vm_api_proxy.hpp>

extern "C" {

int32_t db_store_i64(uint64_t scope, capi_name table, capi_name payer, uint64_t id,  const void* data, uint32_t len) {
    return get_vm_api()->db_store_i64(scope, table, payer, id, data, len);
}

void db_update_i64(int32_t iterator, capi_name payer, const void* data, uint32_t len) {
    get_vm_api()->db_update_i64(iterator, payer, data, len);
}

void db_remove_i64(int32_t iterator) {
    get_vm_api()->db_remove_i64(iterator);
}

int32_t db_get_i64(int32_t iterator, void* data, uint32_t len) {
    return get_vm_api()->db_get_i64(iterator, data, len);
}

int32_t db_next_i64(int32_t iterator, uint64_t* primary) {
    return get_vm_api()->db_next_i64(iterator, primary);
}

int32_t db_previous_i64(int32_t iterator, uint64_t* primary) {
    return get_vm_api()->db_previous_i64(iterator, primary);
}

int32_t db_find_i64(capi_name code, uint64_t scope, capi_name table, uint64_t id) {
    return get_vm_api()->db_find_i64(code, scope, table, id);
}

int32_t db_lowerbound_i64(capi_name code, uint64_t scope, capi_name table, uint64_t id) {
    return get_vm_api()->db_lowerbound_i64(code, scope, table, id);
}

int32_t db_upperbound_i64(capi_name code, uint64_t scope, capi_name table, uint64_t id) {
    return get_vm_api()->db_upperbound_i64(code, scope, table, id);
}

int32_t db_end_i64(capi_name code, uint64_t scope, capi_name table) {
    return get_vm_api()->db_end_i64(code, scope, table);
}

int32_t db_idx64_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const uint64_t* secondary) {
    return get_vm_api()->db_idx64_store(scope, table, payer, id, secondary);
}

void db_idx64_update(int32_t iterator, capi_name payer, const uint64_t* secondary) {
    get_vm_api()->db_idx64_update(iterator, payer, secondary);
}

void db_idx64_remove(int32_t iterator) {
    get_vm_api()->db_idx64_remove(iterator);
}

int32_t db_idx64_next(int32_t iterator, uint64_t* primary) {
    return get_vm_api()->db_idx64_next(iterator, primary);
}

int32_t db_idx64_previous(int32_t iterator, uint64_t* primary) {
    return get_vm_api()->db_idx64_previous(iterator, primary);
}

int32_t db_idx64_find_primary(capi_name code, uint64_t scope, capi_name table, uint64_t* secondary, uint64_t primary) {
    return get_vm_api()->db_idx64_find_primary(code, scope, table, secondary, primary);
}

int32_t db_idx64_find_secondary(capi_name code, uint64_t scope, capi_name table, const uint64_t* secondary, uint64_t* primary) {
    return get_vm_api()->db_idx64_find_secondary(code, scope, table, secondary, primary);
}

int32_t db_idx64_lowerbound(capi_name code, uint64_t scope, capi_name table, uint64_t* secondary, uint64_t* primary) {
    return get_vm_api()->db_idx64_lowerbound(code, scope, table, secondary, primary);
}

int32_t db_idx64_upperbound(capi_name code, uint64_t scope, capi_name table, uint64_t* secondary, uint64_t* primary) {
    return get_vm_api()->db_idx64_upperbound(code, scope, table, secondary, primary);
}

int32_t db_idx64_end(capi_name code, uint64_t scope, capi_name table) {
    return get_vm_api()->db_idx64_end(code, scope, table);
}

int32_t db_idx128_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const uint128_t* secondary) {
    return get_vm_api()->db_idx128_store(scope, table, payer, id, secondary);
}

void db_idx128_update(int32_t iterator, capi_name payer, const uint128_t* secondary) {
    get_vm_api()->db_idx128_update(iterator, payer, secondary);
}

void db_idx128_remove(int32_t iterator) {
    get_vm_api()->db_idx128_remove(iterator);
}

int32_t db_idx128_next(int32_t iterator, uint64_t* primary) {
    return get_vm_api()->db_idx128_next(iterator, primary);
}

int32_t db_idx128_previous(int32_t iterator, uint64_t* primary) {
    return get_vm_api()->db_idx128_previous(iterator, primary);
}

int32_t db_idx128_find_primary(capi_name code, uint64_t scope, capi_name table, uint128_t* secondary, uint64_t primary) {
    return get_vm_api()->db_idx128_find_primary(code, scope, table, secondary, primary);
}

int32_t db_idx128_find_secondary(capi_name code, uint64_t scope, capi_name table, const uint128_t* secondary, uint64_t* primary) {
    return get_vm_api()->db_idx128_find_secondary(code, scope, table, secondary, primary);
}

int32_t db_idx128_lowerbound(capi_name code, uint64_t scope, capi_name table, uint128_t* secondary, uint64_t* primary) {
    return get_vm_api()->db_idx128_lowerbound(code, scope, table, secondary, primary);
}

int32_t db_idx128_upperbound(capi_name code, uint64_t scope, capi_name table, uint128_t* secondary, uint64_t* primary) {
    return get_vm_api()->db_idx128_upperbound(code, scope, table, secondary, primary);
}

int32_t db_idx128_end(capi_name code, uint64_t scope, capi_name table) {
    return get_vm_api()->db_idx128_end(code, scope, table);
}

int32_t db_idx256_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const uint128_t* data, uint32_t data_len ) {
    return get_vm_api()->db_idx256_store(scope, table, payer, id, data, data_len);
}

void db_idx256_update(int32_t iterator, capi_name payer, const uint128_t* data, uint32_t data_len) {
    get_vm_api()->db_idx256_update(iterator, payer, data, data_len);
}

void db_idx256_remove(int32_t iterator) {
    get_vm_api()->db_idx256_remove(iterator);
}

int32_t db_idx256_next(int32_t iterator, uint64_t* primary) {
    return get_vm_api()->db_idx256_next(iterator, primary);
}

int32_t db_idx256_previous(int32_t iterator, uint64_t* primary) {
    return get_vm_api()->db_idx256_previous(iterator, primary);
}

int32_t db_idx256_find_primary(capi_name code, uint64_t scope, capi_name table, uint128_t* data, uint32_t data_len, uint64_t primary) {
    return get_vm_api()->db_idx256_find_primary(code, scope, table, data, data_len, primary);
}

int32_t db_idx256_find_secondary(capi_name code, uint64_t scope, capi_name table, const uint128_t* data, uint32_t data_len, uint64_t* primary) {
    return get_vm_api()->db_idx256_find_secondary(code, scope, table, data, data_len, primary);
}

int32_t db_idx256_lowerbound(capi_name code, uint64_t scope, capi_name table, uint128_t* data, uint32_t data_len, uint64_t* primary) {
    return get_vm_api()->db_idx256_lowerbound(code, scope, table, data, data_len, primary);
}

int32_t db_idx256_upperbound(capi_name code, uint64_t scope, capi_name table, uint128_t* data, uint32_t data_len, uint64_t* primary) {
    return get_vm_api()->db_idx256_upperbound(code, scope, table, data, data_len, primary);
}

int32_t db_idx256_end(capi_name code, uint64_t scope, capi_name table) {
    return get_vm_api()->db_idx256_end(code, scope, table);
}

int32_t db_idx_double_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const double* secondary) {
    return get_vm_api()->db_idx_double_store(scope, table, payer, id, secondary);
}

void db_idx_double_update(int32_t iterator, capi_name payer, const double* secondary) {
    get_vm_api()->db_idx_double_update(iterator, payer, secondary);
}

void db_idx_double_remove(int32_t iterator) {
    get_vm_api()->db_idx_double_remove(iterator);
}

int32_t db_idx_double_next(int32_t iterator, uint64_t* primary) {
    return get_vm_api()->db_idx_double_next(iterator, primary);
}

int32_t db_idx_double_previous(int32_t iterator, uint64_t* primary) {
    return get_vm_api()->db_idx_double_previous(iterator, primary);
}

int32_t db_idx_double_find_primary(capi_name code, uint64_t scope, capi_name table, double* secondary, uint64_t primary) {
    return get_vm_api()->db_idx_double_find_primary(code, scope, table, secondary, primary);
}

int32_t db_idx_double_find_secondary(capi_name code, uint64_t scope, capi_name table, const double* secondary, uint64_t* primary) {
    return get_vm_api()->db_idx_double_find_secondary(code, scope, table, secondary, primary);
}

int32_t db_idx_double_lowerbound(capi_name code, uint64_t scope, capi_name table, double* secondary, uint64_t* primary) {
    return get_vm_api()->db_idx_double_lowerbound(code, scope, table, secondary, primary);
}

int32_t db_idx_double_upperbound(capi_name code, uint64_t scope, capi_name table, double* secondary, uint64_t* primary) {
    return get_vm_api()->db_idx_double_upperbound(code, scope, table, secondary, primary);
}

int32_t db_idx_double_end(capi_name code, uint64_t scope, capi_name table) {
    return get_vm_api()->db_idx_double_end(code, scope, table);
}

int32_t db_idx_long_double_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const long double* secondary) {
    return get_vm_api()->db_idx_long_double_store(scope, table, payer, id, secondary);
}

void db_idx_long_double_update(int32_t iterator, capi_name payer, const long double* secondary) {
    get_vm_api()->db_idx_long_double_update(iterator, payer, secondary);
}

void db_idx_long_double_remove(int32_t iterator) {
    get_vm_api()->db_idx_long_double_remove(iterator);
}

int32_t db_idx_long_double_next(int32_t iterator, uint64_t* primary) {
    return get_vm_api()->db_idx_long_double_next(iterator, primary);
}

int32_t db_idx_long_double_previous(int32_t iterator, uint64_t* primary) {
    return get_vm_api()->db_idx_long_double_previous(iterator, primary);
}

int32_t db_idx_long_double_find_primary(capi_name code, uint64_t scope, capi_name table, long double* secondary, uint64_t primary) {
    return get_vm_api()->db_idx_long_double_find_primary(code, scope, table, secondary, primary);
}

int32_t db_idx_long_double_find_secondary(capi_name code, uint64_t scope, capi_name table, const long double* secondary, uint64_t* primary) {
    return get_vm_api()->db_idx_long_double_find_secondary(code, scope, table, secondary, primary);
}

int32_t db_idx_long_double_lowerbound(capi_name code, uint64_t scope, capi_name table, long double* secondary, uint64_t* primary) {
    return get_vm_api()->db_idx_long_double_lowerbound(code, scope, table, secondary, primary);
}

int32_t db_idx_long_double_upperbound(capi_name code, uint64_t scope, capi_name table, long double* secondary, uint64_t* primary) {
    return get_vm_api()->db_idx_long_double_upperbound(code, scope, table, secondary, primary);
}

int32_t db_idx_long_double_end(capi_name code, uint64_t scope, capi_name table) {
    return get_vm_api()->db_idx_long_double_end(code, scope, table);
}

}