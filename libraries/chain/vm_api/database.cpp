#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include "vm_api_proxy.hpp"

int32_t vm_api_proxy::db_store_i64( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const void *data, uint32_t size ) {
    legacy_span<const char> buffer((void *)data, size);
    return _interface->db_store_i64(scope, table, payer, id, std::move(buffer));
}

void vm_api_proxy::db_update_i64( int32_t itr, uint64_t payer, const void *data, uint32_t size) {
    legacy_span<const char> buffer((void *)data, size);
    return _interface->db_update_i64(itr, payer, std::move(buffer));
}

void vm_api_proxy::db_remove_i64( int32_t itr ) {
    _interface->db_remove_i64(itr);
}

int32_t vm_api_proxy::db_get_i64( int32_t itr, void *data, uint32_t size ) {
    legacy_span<char> buffer(data, size);
    return _interface->db_get_i64(itr, std::move(buffer));
}

int32_t vm_api_proxy::db_next_i64( int32_t itr, uint64_t *primary ) {
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_next_i64(itr, std::move(_primary));
}

int32_t vm_api_proxy::db_previous_i64( int32_t itr, uint64_t *primary ) {
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_previous_i64(itr, std::move(_primary));
}

int32_t vm_api_proxy::db_find_i64( uint64_t code, uint64_t scope, uint64_t table, uint64_t id ) {
    return _interface->db_find_i64(code, scope, table, id);
}

int32_t vm_api_proxy::db_lowerbound_i64( uint64_t code, uint64_t scope, uint64_t table, uint64_t id ) {
    return _interface->db_lowerbound_i64(code, scope, table, id);
}

int32_t vm_api_proxy::db_upperbound_i64( uint64_t code, uint64_t scope, uint64_t table, uint64_t id ) {
    return _interface->db_upperbound_i64(code, scope, table, id);
}

int32_t vm_api_proxy::db_end_i64( uint64_t code, uint64_t scope, uint64_t table ) {
   return _interface->db_end_i64(code, scope, table);
}

/**
   * interface for uint64_t secondary
   */
int32_t vm_api_proxy::db_idx64_store( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const uint64_t *secondary) {
    legacy_ptr<const uint64_t> _secondary((void *)secondary);
    return _interface->db_idx64_store(scope, table, payer, id, std::move(_secondary));
}

void vm_api_proxy::db_idx64_update( int32_t iterator, uint64_t payer, const uint64_t *secondary ) {
    legacy_ptr<const uint64_t> _secondary((void *)secondary);
    return _interface->db_idx64_update(iterator, payer, std::move(_secondary));
}

void vm_api_proxy::db_idx64_remove( int32_t iterator ) {
    _interface->db_idx64_remove(iterator);
}

int32_t vm_api_proxy::db_idx64_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const uint64_t *secondary, uint64_t *primary ) {
    legacy_ptr<const uint64_t> _secondary((void *)secondary);
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx64_find_secondary(code, scope, table, std::move(_secondary), std::move(_primary));
}

int32_t vm_api_proxy::db_idx64_find_primary( uint64_t code, uint64_t scope, uint64_t table, uint64_t *secondary, uint64_t primary ) {
    legacy_ptr<uint64_t> _secondary(secondary);
    return _interface->db_idx64_find_primary(code, scope, table, std::move(_secondary), primary);
}

int32_t vm_api_proxy::db_idx64_lowerbound( uint64_t code, uint64_t scope, uint64_t table, uint64_t *secondary, uint64_t *primary ) {
    legacy_ptr<uint64_t> _secondary(secondary);
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx64_lowerbound(code, scope, table, std::move(_secondary), std::move(_primary));
}

int32_t vm_api_proxy::db_idx64_upperbound( uint64_t code, uint64_t scope, uint64_t table, uint64_t *secondary, uint64_t *primary ) {
    legacy_ptr<uint64_t> _secondary(secondary);
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx64_upperbound(code, scope, table, std::move(_secondary), std::move(_primary));
}

int32_t vm_api_proxy::db_idx64_end( uint64_t code, uint64_t scope, uint64_t table ) {
   return _interface->db_idx64_end(code, scope, table);
}

int32_t vm_api_proxy::db_idx64_next( int32_t iterator, uint64_t *primary ) {
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx64_next(iterator, std::move(_primary));
}

int32_t vm_api_proxy::db_idx64_previous( int32_t iterator, uint64_t *primary ) {
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx64_previous(iterator, std::move(_primary));
}

/**
   * interface for uint128_t secondary
   */
int32_t vm_api_proxy::db_idx128_store( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const uint128_t *secondary ) {
    legacy_ptr<const uint128_t> _secondary((void *)secondary);
    return _interface->db_idx128_store( scope, table, payer, id, std::move(_secondary) );
}

void vm_api_proxy::db_idx128_update( int32_t iterator, uint64_t payer, const uint128_t *secondary ) {
    legacy_ptr<const uint128_t> _secondary((void *)secondary);
    return _interface->db_idx128_update( iterator, payer, std::move(_secondary) );
}

void vm_api_proxy::db_idx128_remove( int32_t iterator ) {
   return _interface->db_idx128_remove( iterator );
}

int32_t vm_api_proxy::db_idx128_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const uint128_t *secondary, uint64_t *primary ) {
    legacy_ptr<const uint128_t> _secondary((void *)secondary);
    legacy_ptr<uint64_t> _primary(primary);
   return _interface->db_idx128_find_secondary(code, scope, table, std::move(_secondary), std::move(_primary));
}

int32_t vm_api_proxy::db_idx128_find_primary( uint64_t code, uint64_t scope, uint64_t table, uint128_t *secondary, uint64_t primary ) {
    legacy_ptr<uint128_t> _secondary(secondary);
    return _interface->db_idx128_find_primary(code, scope, table, std::move(_secondary), primary);
}

int32_t vm_api_proxy::db_idx128_lowerbound( uint64_t code, uint64_t scope, uint64_t table, uint128_t *secondary, uint64_t *primary ) {
    legacy_ptr<uint128_t> _secondary(secondary);
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx128_lowerbound(code, scope, table, std::move(_secondary), std::move(_primary));
}

int32_t vm_api_proxy::db_idx128_upperbound( uint64_t code, uint64_t scope, uint64_t table, uint128_t *secondary, uint64_t *primary ) {
    legacy_ptr<uint128_t> _secondary(secondary);
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx128_upperbound(code, scope, table, std::move(_secondary), std::move(_primary));
}

int32_t vm_api_proxy::db_idx128_end( uint64_t code, uint64_t scope, uint64_t table ) {
   return _interface->db_idx128_end(code, scope, table);
}

int32_t vm_api_proxy::db_idx128_next( int32_t iterator, uint64_t *primary ) {
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx128_next(iterator, std::move(_primary));
}

int32_t vm_api_proxy::db_idx128_previous( int32_t iterator, uint64_t *primary ) {
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx128_previous(iterator, std::move(_primary));
}

/**
   * interface for 256-bit interger secondary
   */
int32_t vm_api_proxy::db_idx256_store( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const __uint128 *data, uint32_t data_len ) {
    legacy_span<const uint128_t> _data((void *)data, data_len);
    return _interface->db_idx256_store(scope, table, payer, id, std::move(_data));
}

void vm_api_proxy::db_idx256_update( int32_t iterator, uint64_t payer, const __uint128 *data, uint32_t data_len ) {
    legacy_span<const uint128_t> _data((void *)data, data_len);
    return _interface->db_idx256_update(iterator, payer, std::move(_data));
}

void vm_api_proxy::db_idx256_remove( int32_t iterator ) {
   return _interface->db_idx256_remove(iterator);
}

int32_t vm_api_proxy::db_idx256_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const __uint128 *data, uint32_t data_len, uint64_t *primary ) {
    legacy_span<const uint128_t> _data((void *)data, data_len);
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx256_find_secondary(code, scope, table, std::move(_data), std::move(_primary));
}

int32_t vm_api_proxy::db_idx256_find_primary( uint64_t code, uint64_t scope, uint64_t table, __uint128 *data, uint32_t data_len, uint64_t primary ) {
    legacy_span<uint128_t> _data(data, data_len);
    return _interface->db_idx256_find_primary(code, scope, table, std::move(_data), primary);
}

int32_t vm_api_proxy::db_idx256_lowerbound( uint64_t code, uint64_t scope, uint64_t table, __uint128 *data, uint32_t data_len, uint64_t *primary ) {
    legacy_span<uint128_t> _data(data, data_len);
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx256_lowerbound(code, scope, table, std::move(_data), std::move(_primary));
}

int32_t vm_api_proxy::db_idx256_upperbound( uint64_t code, uint64_t scope, uint64_t table, __uint128 *data, uint32_t data_len, uint64_t *primary ) {
    legacy_span<uint128_t> _data(data, data_len);
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx256_upperbound(code, scope, table, std::move(_data), std::move(_primary));
}

int32_t vm_api_proxy::db_idx256_end( uint64_t code, uint64_t scope, uint64_t table ) {
    return _interface->db_idx256_end(code, scope, table);
}

int32_t vm_api_proxy::db_idx256_next( int32_t iterator, uint64_t *primary ) {
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx256_next(iterator, std::move(_primary));
}

int32_t vm_api_proxy::db_idx256_previous( int32_t iterator, uint64_t *primary ) {
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx256_previous(iterator, std::move(_primary));
}

/**
   * interface for double secondary
   */
int32_t vm_api_proxy::db_idx_double_store( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const double *secondary ) {
    legacy_ptr<const float64_t> _secondary((void *)secondary);
    return _interface->db_idx_double_store( scope, table, payer, id, std::move(_secondary) );
}

void vm_api_proxy::db_idx_double_update( int32_t iterator, uint64_t payer, const double *secondary ) {
    legacy_ptr<const float64_t> _secondary((void *)secondary);
    return _interface->db_idx_double_update( iterator, payer, std::move(_secondary) );
}

void vm_api_proxy::db_idx_double_remove( int32_t iterator ) {
   return _interface->db_idx_double_remove( iterator );
}

int32_t vm_api_proxy::db_idx_double_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const double *secondary, uint64_t *primary ) {
    legacy_ptr<const float64_t> _secondary((void *)secondary);
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx_double_find_secondary(code, scope, table, std::move(_secondary), std::move(_primary));
}

int32_t vm_api_proxy::db_idx_double_find_primary( uint64_t code, uint64_t scope, uint64_t table, double *secondary, uint64_t primary ) {
    legacy_ptr<float64_t> _secondary(secondary);
   return _interface->db_idx_double_find_primary(code, scope, table, std::move(_secondary), primary);
}

int32_t vm_api_proxy::db_idx_double_lowerbound( uint64_t code, uint64_t scope, uint64_t table, double *secondary, uint64_t *primary ) {
    legacy_ptr<float64_t> _secondary(secondary);
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx_double_lowerbound(code, scope, table, std::move(_secondary), std::move(_primary));
}

int32_t vm_api_proxy::db_idx_double_upperbound( uint64_t code, uint64_t scope, uint64_t table, double *secondary, uint64_t *primary ) {
    legacy_ptr<float64_t> _secondary(secondary);
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx_double_upperbound(code, scope, table, std::move(_secondary), std::move(_primary));
}

int32_t vm_api_proxy::db_idx_double_end( uint64_t code, uint64_t scope, uint64_t table ) {
   return _interface->db_idx_double_end(code, scope, table);
}

int32_t vm_api_proxy::db_idx_double_next( int32_t iterator, uint64_t *primary ) {
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx_double_next(iterator, std::move(_primary));
}

int32_t vm_api_proxy::db_idx_double_previous( int32_t iterator, uint64_t *primary ) {
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx_double_previous(iterator, std::move(_primary));
}

/**
   * interface for long double secondary
   */
int32_t vm_api_proxy::db_idx_long_double_store( uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const long double *secondary ) {
    legacy_ptr<const float128_t> _secondary((void *)secondary);
    return _interface->db_idx_long_double_store( scope, table, payer, id, std::move(_secondary) );
}

void vm_api_proxy::db_idx_long_double_update( int32_t iterator, uint64_t payer, const long double *secondary ) {
    legacy_ptr<const float128_t> _secondary((void *)secondary);
    return _interface->db_idx_long_double_update( iterator, payer, std::move(_secondary) );
}

void vm_api_proxy::db_idx_long_double_remove( int32_t iterator ) {
   return _interface->db_idx_long_double_remove( iterator );
}

int32_t vm_api_proxy::db_idx_long_double_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const long double *secondary, uint64_t *primary ) {
    legacy_ptr<const float128_t> _secondary((void *)secondary);
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx_long_double_find_secondary(code, scope, table, std::move(_secondary), std::move(_primary));
}

int32_t vm_api_proxy::db_idx_long_double_find_primary( uint64_t code, uint64_t scope, uint64_t table, long double *secondary, uint64_t primary ) {
    legacy_ptr<float128_t> _secondary(secondary);
    return _interface->db_idx_long_double_find_primary(code, scope, table, std::move(_secondary), primary);
}

int32_t vm_api_proxy::db_idx_long_double_lowerbound( uint64_t code, uint64_t scope, uint64_t table, long double *secondary, uint64_t *primary ) {
    legacy_ptr<float128_t> _secondary(secondary);
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx_long_double_lowerbound(code, scope, table, std::move(_secondary), std::move(_primary));
}

int32_t vm_api_proxy::db_idx_long_double_upperbound( uint64_t code, uint64_t scope, uint64_t table, long double *secondary, uint64_t *primary ) {
    legacy_ptr<float128_t> _secondary(secondary);
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx_long_double_upperbound(code, scope, table, std::move(_secondary), std::move(_primary));
}

int32_t vm_api_proxy::db_idx_long_double_end( uint64_t code, uint64_t scope, uint64_t table ) {
   return _interface->db_idx_long_double_end(code, scope, table);
}

int32_t vm_api_proxy::db_idx_long_double_next( int32_t iterator, uint64_t *primary ) {
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx_long_double_next(iterator, std::move(_primary));
}

int32_t vm_api_proxy::db_idx_long_double_previous( int32_t iterator, uint64_t *primary ) {
    legacy_ptr<uint64_t> _primary(primary);
    return _interface->db_idx_long_double_previous(iterator, std::move(_primary));
}

