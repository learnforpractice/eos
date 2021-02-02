#include <vm_api_proxy.hpp>

extern "C" {

int32_t db_store_i64( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const char *data, size_t size ) {
    return get_vm_api()->db_store_i64(scope, table, payer, id, data, size);
}

void db_update_i64( int32_t itr, uint64_t payer, const char *data, size_t size) {
    return get_vm_api()->db_update_i64(itr, payer, data, size);
}

void db_remove_i64( int32_t itr ) {
    get_vm_api()->db_remove_i64(itr);
}

int32_t db_get_i64( int32_t itr, char *data, size_t size ) {
    return get_vm_api()->db_get_i64(itr, data, size);
}

int32_t db_next_i64( int32_t itr, uint64_t *primary ) {
    return get_vm_api()->db_next_i64(itr, primary);
}

int32_t db_previous_i64( int32_t itr, uint64_t *primary ) {
    return get_vm_api()->db_previous_i64(itr, primary);
}

int32_t db_find_i64( uint64_t code, uint64_t scope, uint64_t table, uint64_t id ) {
    return get_vm_api()->db_find_i64(code, scope, table, id);
}

int32_t db_lowerbound_i64( uint64_t code, uint64_t scope, uint64_t table, uint64_t id ) {
    return get_vm_api()->db_lowerbound_i64(code, scope, table, id);
}

int32_t db_upperbound_i64( uint64_t code, uint64_t scope, uint64_t table, uint64_t id ) {
    return get_vm_api()->db_upperbound_i64(code, scope, table, id);
}

int32_t db_end_i64( uint64_t code, uint64_t scope, uint64_t table ) {
   return get_vm_api()->db_end_i64(code, scope, table);
}

/**
   * interface for uint64_t secondary
   */
int32_t db_idx64_store( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const uint64_t *secondary) {
    return get_vm_api()->db_idx64_store(scope, table, payer, id, secondary);
}

void db_idx64_update( int32_t iterator, uint64_t payer, const uint64_t *secondary ) {
    return get_vm_api()->db_idx64_update(iterator, payer, secondary);
}

void db_idx64_remove( int32_t iterator ) {
    get_vm_api()->db_idx64_remove(iterator);
}

int32_t db_idx64_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const uint64_t *secondary, uint64_t *primary ) {
    return get_vm_api()->db_idx64_find_secondary(code, scope, table, secondary, primary);
}

int32_t db_idx64_find_primary( uint64_t code, uint64_t scope, uint64_t table, uint64_t *secondary, uint64_t primary ) {
    return get_vm_api()->db_idx64_find_primary(code, scope, table, secondary, primary);
}

int32_t db_idx64_lowerbound( uint64_t code, uint64_t scope, uint64_t table, uint64_t *secondary, uint64_t *primary ) {
    return get_vm_api()->db_idx64_lowerbound(code, scope, table, secondary, primary);
}

int32_t db_idx64_upperbound( uint64_t code, uint64_t scope, uint64_t table, uint64_t *secondary, uint64_t *primary ) {
    return get_vm_api()->db_idx64_upperbound(code, scope, table, secondary, primary);
}

int32_t db_idx64_end( uint64_t code, uint64_t scope, uint64_t table ) {
   return get_vm_api()->db_idx64_end(code, scope, table);
}

int32_t db_idx64_next( int32_t iterator, uint64_t *primary ) {
    return get_vm_api()->db_idx64_next(iterator, primary);
}

int32_t db_idx64_previous( int32_t iterator, uint64_t *primary ) {
    return get_vm_api()->db_idx64_previous(iterator, primary);
}

/**
   * interface for __uint128 secondary
   */
int32_t db_idx128_store( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const __uint128 *secondary ) {
    return get_vm_api()->db_idx128_store( scope, table, payer, id, secondary );
}

void db_idx128_update( int32_t iterator, uint64_t payer, const __uint128 *secondary ) {
    return get_vm_api()->db_idx128_update( iterator, payer, secondary );
}

void db_idx128_remove( int32_t iterator ) {
   return get_vm_api()->db_idx128_remove( iterator );
}

int32_t db_idx128_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const __uint128 *secondary, uint64_t *primary ) {
    return get_vm_api()->db_idx128_find_secondary(code, scope, table, secondary, primary);
}

int32_t db_idx128_find_primary( uint64_t code, uint64_t scope, uint64_t table, __uint128 *secondary, uint64_t primary ) {
    return get_vm_api()->db_idx128_find_primary(code, scope, table, secondary, primary);
}

int32_t db_idx128_lowerbound( uint64_t code, uint64_t scope, uint64_t table, __uint128 *secondary, uint64_t *primary ) {
    return get_vm_api()->db_idx128_lowerbound(code, scope, table, secondary, primary);
}

int32_t db_idx128_upperbound( uint64_t code, uint64_t scope, uint64_t table, __uint128 *secondary, uint64_t *primary ) {
    return get_vm_api()->db_idx128_upperbound(code, scope, table, secondary, primary);
}

int32_t db_idx128_end( uint64_t code, uint64_t scope, uint64_t table ) {
   return get_vm_api()->db_idx128_end(code, scope, table);
}

int32_t db_idx128_next( int32_t iterator, uint64_t *primary ) {
    return get_vm_api()->db_idx128_next(iterator, primary);
}

int32_t db_idx128_previous( int32_t iterator, uint64_t *primary ) {
    return get_vm_api()->db_idx128_previous(iterator, primary);
}

/**
   * interface for 256-bit interger secondary
   */
int32_t db_idx256_store( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const __uint128 *data, size_t data_len ) {
    return get_vm_api()->db_idx256_store(scope, table, payer, id, data, data_len);
}

void db_idx256_update( int32_t iterator, uint64_t payer, const __uint128 *data, size_t data_len ) {
    return get_vm_api()->db_idx256_update(iterator, payer, data, data_len);
}

void db_idx256_remove( int32_t iterator ) {
   return get_vm_api()->db_idx256_remove(iterator);
}

int32_t db_idx256_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const __uint128 *data, size_t data_len, uint64_t *primary ) {
    return get_vm_api()->db_idx256_find_secondary(code, scope, table, data, data_len, primary);
}

int32_t db_idx256_find_primary( uint64_t code, uint64_t scope, uint64_t table, __uint128 *data, size_t data_len, uint64_t primary ) {
    return get_vm_api()->db_idx256_find_primary(code, scope, table, data, data_len, primary);
}

int32_t db_idx256_lowerbound( uint64_t code, uint64_t scope, uint64_t table, __uint128 *data, size_t data_len, uint64_t *primary ) {
    return get_vm_api()->db_idx256_lowerbound(code, scope, table, data, data_len, primary);
}

int32_t db_idx256_upperbound( uint64_t code, uint64_t scope, uint64_t table, __uint128 *data, size_t data_len, uint64_t *primary ) {
    return get_vm_api()->db_idx256_upperbound(code, scope, table, data, data_len, primary);
}

int32_t db_idx256_end( uint64_t code, uint64_t scope, uint64_t table ) {
    return get_vm_api()->db_idx256_end(code, scope, table);
}

int32_t db_idx256_next( int32_t iterator, uint64_t *primary ) {
    return get_vm_api()->db_idx256_next(iterator, primary);
}

int32_t db_idx256_previous( int32_t iterator, uint64_t *primary ) {
    return get_vm_api()->db_idx256_previous(iterator, primary);
}

/**
   * interface for double secondary
   */
int32_t db_idx_double_store( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const float64_t *secondary ) {
    return get_vm_api()->db_idx_double_store( scope, table, payer, id, secondary );
}

void db_idx_double_update( int32_t iterator, uint64_t payer, const float64_t *secondary ) {
    return get_vm_api()->db_idx_double_update( iterator, payer, secondary );
}

void db_idx_double_remove( int32_t iterator ) {
   return get_vm_api()->db_idx_double_remove( iterator );
}

int32_t db_idx_double_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const float64_t *secondary, uint64_t *primary ) {
    return get_vm_api()->db_idx_double_find_secondary(code, scope, table, secondary, primary);
}

int32_t db_idx_double_find_primary( uint64_t code, uint64_t scope, uint64_t table, float64_t *secondary, uint64_t primary ) {
   return get_vm_api()->db_idx_double_find_primary(code, scope, table, secondary, primary);
}

int32_t db_idx_double_lowerbound( uint64_t code, uint64_t scope, uint64_t table, float64_t *secondary, uint64_t *primary ) {
    return get_vm_api()->db_idx_double_lowerbound(code, scope, table, secondary, primary);
}

int32_t db_idx_double_upperbound( uint64_t code, uint64_t scope, uint64_t table, float64_t *secondary, uint64_t *primary ) {
    return get_vm_api()->db_idx_double_upperbound(code, scope, table, secondary, primary);
}

int32_t db_idx_double_end( uint64_t code, uint64_t scope, uint64_t table ) {
   return get_vm_api()->db_idx_double_end(code, scope, table);
}

int32_t db_idx_double_next( int32_t iterator, uint64_t *primary ) {
    return get_vm_api()->db_idx_double_next(iterator, primary);
}

int32_t db_idx_double_previous( int32_t iterator, uint64_t *primary ) {
    return get_vm_api()->db_idx_double_previous(iterator, primary);
}

/**
   * interface for long double secondary
   */
int32_t db_idx_long_double_store( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const float128_t *secondary ) {
    return get_vm_api()->db_idx_long_double_store( scope, table, payer, id, secondary );
}

void db_idx_long_double_update( int32_t iterator, uint64_t payer, const float128_t *secondary ) {
    return get_vm_api()->db_idx_long_double_update( iterator, payer, secondary );
}

void db_idx_long_double_remove( int32_t iterator ) {
   return get_vm_api()->db_idx_long_double_remove( iterator );
}

int32_t db_idx_long_double_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const float128_t *secondary, uint64_t *primary ) {
    return get_vm_api()->db_idx_long_double_find_secondary(code, scope, table, secondary, primary);
}

int32_t db_idx_long_double_find_primary( uint64_t code, uint64_t scope, uint64_t table, float128_t *secondary, uint64_t primary ) {
    return get_vm_api()->db_idx_long_double_find_primary(code, scope, table, secondary, primary);
}

int32_t db_idx_long_double_lowerbound( uint64_t code, uint64_t scope, uint64_t table, float128_t *secondary, uint64_t *primary ) {
    return get_vm_api()->db_idx_long_double_lowerbound(code, scope, table, secondary, primary);
}

int32_t db_idx_long_double_upperbound( uint64_t code, uint64_t scope, uint64_t table, float128_t *secondary, uint64_t *primary ) {
    return get_vm_api()->db_idx_long_double_upperbound(code, scope, table, secondary, primary);
}

int32_t db_idx_long_double_end( uint64_t code, uint64_t scope, uint64_t table ) {
   return get_vm_api()->db_idx_long_double_end(code, scope, table);
}

int32_t db_idx_long_double_next( int32_t iterator, uint64_t *primary ) {
    return get_vm_api()->db_idx_long_double_next(iterator, primary);
}

int32_t db_idx_long_double_previous( int32_t iterator, uint64_t *primary ) {
    return get_vm_api()->db_idx_long_double_previous(iterator, primary);
}

}
