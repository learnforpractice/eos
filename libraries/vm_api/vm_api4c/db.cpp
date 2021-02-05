#include "vm_api4c.hpp"
#include <vm_api_proxy.hpp>

u32 vm_api4c_proxy::db_store_i64(u64 scope, u64 table, u64 payer, u64 id,  u32 data_offset, u32 len) {
   const void* data = (void *)offset_to_ptr(data_offset, len);
   return get_vm_api()->db_store_i64(scope, table, payer, id, (const char*)data, len);
}

void vm_api4c_proxy::db_update_i64(u32 iterator, u64 payer, u32 data_offset, u32 len) {
   const void* data = (void *)offset_to_ptr(data_offset, len);
   get_vm_api()->db_update_i64( iterator, payer, (const char*)data, len);
}

void vm_api4c_proxy::db_remove_i64(u32 iterator) {
   get_vm_api()->db_remove_i64(iterator);
}

u32 vm_api4c_proxy::db_get_i64(u32 iterator, u32 data_offset, u32 len) {
   void* data = (void *)offset_to_ptr(data_offset, len);
   return get_vm_api()->db_get_i64(iterator, data, len);
}

u32 vm_api4c_proxy::db_next_i64(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_next_i64(iterator, primary);
}

u32 vm_api4c_proxy::db_previous_i64(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_previous_i64(iterator, primary);
}

u32 vm_api4c_proxy::db_find_i64(u64 code, u64 scope, u64 table, u64 id) {
   return get_vm_api()->db_find_i64(code, scope, table, id);
}

u32 vm_api4c_proxy::db_lowerbound_i64(u64 code, u64 scope, u64 table, u64 id) {
   return get_vm_api()->db_lowerbound_i64(code, scope, table, id);
}

u32 vm_api4c_proxy::db_upperbound_i64(u64 code, u64 scope, u64 table, u64 id) {
   return get_vm_api()->db_upperbound_i64(code, scope, table, id);
}

u32 vm_api4c_proxy::db_end_i64(uint64_t code, uint64_t scope, uint64_t table) {
   return get_vm_api()->db_end_i64(code, scope, table);
}

// u32 vm_api4c_proxy::db_get_count(uint64_t code, uint64_t scope, uint64_t table) {
//    return get_vm_api()->db_get_table_row_count(code, scope, table);
// }

u32 vm_api4c_proxy::db_idx64_store(u64 scope, u64 table, u64 payer, u64 id, u32 secondary_offset) {
   const uint64_t* secondary = (uint64_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx64_store(scope, table, payer, id, secondary);
}

void vm_api4c_proxy::db_idx64_update(u32 iterator, u64 payer, u32 secondary_offset) {
   const uint64_t* secondary = (uint64_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   get_vm_api()->db_idx64_update(iterator, payer, secondary);
}

void vm_api4c_proxy::db_idx64_remove(u32 iterator) {
   get_vm_api()->db_idx64_remove(iterator);
}

u32 vm_api4c_proxy::db_idx64_next(u32 iterator, u32 primary_offset) {
   uint64_t *primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx64_next(iterator, primary);
}

u32 vm_api4c_proxy::db_idx64_previous(u32 iterator, u32 primary_offset) {
   uint64_t *primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx64_previous(iterator, primary);
}

u32 vm_api4c_proxy::db_idx64_find_primary(u64 code, u64 scope, u64 table, u32 secondary_offset, u64 primary) {
   uint64_t* secondary = (uint64_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx64_find_primary(code, scope, table, secondary, primary);
}

u32 vm_api4c_proxy::db_idx64_find_secondary(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   const uint64_t* secondary = (uint64_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx64_find_secondary(code, scope, table, secondary, primary);
}

u32 vm_api4c_proxy::db_idx64_lowerbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* secondary = (uint64_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx64_lowerbound(code, scope, table, secondary, primary);
}

u32 vm_api4c_proxy::db_idx64_upperbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* secondary = (uint64_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx64_upperbound(code, scope, table, secondary, primary);
}

u32 vm_api4c_proxy::db_idx64_end(u64 code, u64 scope, u64 table) {
   return get_vm_api()->db_idx64_end(code, scope, table);
}

u32 vm_api4c_proxy::db_idx128_store(u64 scope, u64 table, u64 payer, u64 id, u32 secondary_offset) {
   const uint128_t* secondary = (uint128_t *)offset_to_ptr(secondary_offset, sizeof(uint128_t));
   return get_vm_api()->db_idx128_store(scope, table, payer, id, secondary);
}

void vm_api4c_proxy::db_idx128_update(u32 iterator, u64 payer, u32 secondary_offset) {
   const uint128_t* secondary = (uint128_t *)offset_to_ptr(secondary_offset, sizeof(uint128_t));
   get_vm_api()->db_idx128_update(iterator, payer, secondary);
}

void vm_api4c_proxy::db_idx128_remove(u32 iterator) {
   get_vm_api()->db_idx128_remove(iterator);
}

u32 vm_api4c_proxy::db_idx128_next(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx128_next(iterator, primary);
}

u32 vm_api4c_proxy::db_idx128_previous(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx128_previous(iterator, primary) ;
}

u32 vm_api4c_proxy::db_idx128_find_primary(u64 code, u64 scope, u64 table, u32 secondary_offset, u64 primary) {
   uint128_t* secondary = (uint128_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx128_find_primary(code, scope, table, secondary, primary);
}

u32 vm_api4c_proxy::db_idx128_find_secondary(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   uint128_t* secondary = (uint128_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx128_find_secondary(code, scope, table, secondary, primary);
}

u32 vm_api4c_proxy::db_idx128_lowerbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   uint128_t* secondary = (uint128_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx128_lowerbound(code, scope, table, secondary, primary);
}

u32 vm_api4c_proxy::db_idx128_upperbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   uint128_t* secondary = (uint128_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx128_upperbound(code, scope, table, secondary, primary);
}

u32 vm_api4c_proxy::db_idx128_end(u64 code, u64 scope, u64 table) {
   return get_vm_api()->db_idx128_end(code, scope, table);
}

u32 vm_api4c_proxy::db_idx256_store(u64 scope, u64 table, u64 payer, u64 id, u32 data_offset, u32 data_len ) {
   uint128_t* data = (uint128_t *)offset_to_ptr(data_offset, sizeof(uint128_t));
   return get_vm_api()->db_idx256_store(scope, table, payer, id, data, data_len );
}

void vm_api4c_proxy::db_idx256_update(u32 iterator, u64 payer, u32 data_offset, u32 data_len) {
   uint128_t* data = (uint128_t *)offset_to_ptr(data_offset, sizeof(uint128_t));
   get_vm_api()->db_idx256_update(iterator, payer, data, data_len);
}

void vm_api4c_proxy::db_idx256_remove(u32 iterator) {
   get_vm_api()->db_idx256_remove(iterator);
}

u32 vm_api4c_proxy::db_idx256_next(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx256_next(iterator, primary);
}

u32 vm_api4c_proxy::db_idx256_previous(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx256_previous(iterator, primary);
}

u32 vm_api4c_proxy::db_idx256_find_primary(u64 code, u64 scope, u64 table, u32 data_offset, u32 data_len, u64 primary) {
   uint128_t* data = (uint128_t *)offset_to_ptr(data_offset, sizeof(uint128_t));
   return get_vm_api()->db_idx256_find_primary(code, scope, table, data, data_len, primary);
}

u32 vm_api4c_proxy::db_idx256_find_secondary(u64 code, u64 scope, u64 table, u32 data_offset, u32 data_len, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   const uint128_t* data = (uint128_t *)offset_to_ptr(data_offset, sizeof(uint128_t));
   return get_vm_api()->db_idx256_find_secondary(code, scope, table, data, data_len, primary);
}

u32 vm_api4c_proxy::db_idx256_lowerbound(u64 code, u64 scope, u64 table, u32 data_offset, u32 data_len, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   uint128_t* data = (uint128_t *)offset_to_ptr(data_offset, sizeof(uint128_t));
   return get_vm_api()->db_idx256_lowerbound(code, scope, table, data, data_len, primary);
}

u32 vm_api4c_proxy::db_idx256_upperbound(u64 code, u64 scope, u64 table, u32 data_offset, u32 data_len, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   uint128_t* data = (uint128_t *)offset_to_ptr(data_offset, sizeof(uint128_t));
   return get_vm_api()->db_idx256_upperbound(code, scope, table, data, data_len, primary);
}

u32 vm_api4c_proxy::db_idx256_end(u64 code, u64 scope, u64 table) {
   return get_vm_api()->db_idx256_end(code, scope, table);
}

u32 vm_api4c_proxy::db_idx_double_store(u64 scope, u64 table, u64 payer, u64 id, u32 secondary_offset) {
   const double* secondary = (double*)offset_to_ptr(secondary_offset, sizeof(double));
   return get_vm_api()->db_idx_double_store(scope, table, payer, id, (const double*)secondary);
}

void vm_api4c_proxy::db_idx_double_update(u32 iterator, u64 payer, u32 secondary_offset) {
   const double* secondary = (double*)offset_to_ptr(secondary_offset, sizeof(double));
   get_vm_api()->db_idx_double_update(iterator, payer, (const double*)secondary);
}

void vm_api4c_proxy::db_idx_double_remove(u32 iterator) {
   get_vm_api()->db_idx_double_remove(iterator);
}

u32 vm_api4c_proxy::db_idx_double_next(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx_double_next(iterator, primary);
}

u32 vm_api4c_proxy::db_idx_double_previous(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx_double_previous(iterator, primary);
}

u32 vm_api4c_proxy::db_idx_double_find_primary(u64 code, u64 scope, u64 table, u32 secondary_offset, u64 primary) {
   double* secondary = (double*)offset_to_ptr(secondary_offset, sizeof(double));
   return get_vm_api()->db_idx_double_find_primary(code, scope, table, (double*)secondary, primary);
}

u32 vm_api4c_proxy::db_idx_double_find_secondary(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   double* secondary = (double*)offset_to_ptr(secondary_offset, sizeof(double));
   return get_vm_api()->db_idx_double_find_secondary(code, scope, table, (const double*)secondary, primary);
}

u32 vm_api4c_proxy::db_idx_double_lowerbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   double* secondary = (double*)offset_to_ptr(secondary_offset, sizeof(double));
   return get_vm_api()->db_idx_double_lowerbound(code, scope, table, (double*)secondary, primary);
}

u32 vm_api4c_proxy::db_idx_double_upperbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   double* secondary = (double*)offset_to_ptr(secondary_offset, sizeof(double));
   return get_vm_api()->db_idx_double_upperbound(code, scope, table, (double*)secondary, primary);
}

u32 vm_api4c_proxy::db_idx_double_end(u64 code, u64 scope, u64 table) {
   return get_vm_api()->db_idx_double_end(code, scope, table);
}

u32 vm_api4c_proxy::db_idx_long_double_store(u64 scope, u64 table, u64 payer, u64 id, u32 secondary_offset) {
   long double *secondary = (long double *)offset_to_ptr(secondary_offset, sizeof(long double));
   return get_vm_api()->db_idx_long_double_store(scope, table, payer, id, (const long double*)secondary);
}

void vm_api4c_proxy::db_idx_long_double_update(u32 iterator, u64 payer, u32 secondary_offset) {
   long double *secondary = (long double *)offset_to_ptr(secondary_offset, sizeof(long double));
   get_vm_api()->db_idx_long_double_update(iterator, payer, (const long double*)secondary);
}

void vm_api4c_proxy::db_idx_long_double_remove(u32 iterator) {
   get_vm_api()->db_idx_long_double_remove(iterator);
}

u32 vm_api4c_proxy::db_idx_long_double_next(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx_long_double_next(iterator, primary);
}

u32 vm_api4c_proxy::db_idx_long_double_previous(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx_long_double_previous(iterator, primary);
}

u32 vm_api4c_proxy::db_idx_long_double_find_primary(u64 code, u64 scope, u64 table, u32 secondary_offset, u64 primary) {
   long double* secondary = (long double*)offset_to_ptr(secondary_offset, sizeof(long double));
   return get_vm_api()->db_idx_long_double_find_primary(code, scope, table, (long double*)secondary, primary);
}

u32 vm_api4c_proxy::db_idx_long_double_find_secondary(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   const long double* secondary = (long double*)offset_to_ptr(secondary_offset, sizeof(long double));
   return get_vm_api()->db_idx_long_double_find_secondary(code, scope, table, (const long double*)secondary, primary);
}

u32 vm_api4c_proxy::db_idx_long_double_lowerbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   const long double* secondary = (long double*)offset_to_ptr(secondary_offset, sizeof(long double));
   return get_vm_api()->db_idx_long_double_lowerbound(code, scope, table, (long double*)secondary, primary);
}

u32 vm_api4c_proxy::db_idx_long_double_upperbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   const long double* secondary = (long double*)offset_to_ptr(secondary_offset, sizeof(long double));
   return get_vm_api()->db_idx_long_double_upperbound(code, scope, table, (long double*)secondary, primary);
}

u32 vm_api4c_proxy::db_idx_long_double_end(u64 code, u64 scope, u64 table) {
   return get_vm_api()->db_idx_long_double_end(code, scope, table);
}

// u32 vm_api4c_proxy::db_get_table_row_count(u64 code, u64 scope, u64 table) {
//    return get_vm_api()->db_get_table_row_count(code, scope, table);
// }

// u32 vm_api4c_proxy::db_find_i256( u64 code, u64 scope, u64 table, u32 id_offset, u32 id_size ) {
//    void *id = (void *)offset_to_ptr(id_offset, id_size);
//    return get_vm_api()->db_find_i256(code, scope, table, id, id_size);
// }

// u32 vm_api4c_proxy::db_store_i256( u64 scope, u64 table, u64 payer, u32 id_offset, u32 id_size, u32 buffer_offset, u32 buffer_size ) {
//    void *id = (void *)offset_to_ptr(id_offset, id_size);
//    const char *buffer = (const char *)offset_to_ptr(buffer_offset, buffer_size);
//    return get_vm_api()->db_store_i256(scope, table, payer, id, id_size, buffer, buffer_size);
// }

// void vm_api4c_proxy::db_update_i256( u32 iterator, u64 payer, u32 buffer_offset, u32 buffer_size ) {
//    const char *buffer = (const char *)offset_to_ptr(buffer_offset, buffer_size);
//    return get_vm_api()->db_update_i256(iterator, payer, buffer, buffer_size);
// }

// void vm_api4c_proxy::db_remove_i256( u32 iterator ) {
//    return get_vm_api()->db_remove_i256(iterator);
// }

// u32 vm_api4c_proxy::db_get_i256( u32 iterator, u32 buffer_offset, u32 buffer_size ) {
//    char *buffer = (char *)offset_to_ptr(buffer_offset, buffer_size);
//    return get_vm_api()->db_get_i256(iterator, buffer, buffer_size);
// }

// u32 vm_api4c_proxy::db_upperbound_i256( u64 code, u64 scope, u64 table, u32 id_offset, u32 id_size ) {
//    void *id = offset_to_ptr(id_offset, id_size);
//    return get_vm_api()->db_upperbound_i256(code, scope, table, id, id_size);
// }

// u32 vm_api4c_proxy::db_lowerbound_i256( u64 code, u64 scope, u64 table, u32 id_offset, u32 id_size ) {
//    void *id = offset_to_ptr(id_offset, id_size);
//    return get_vm_api()->db_lowerbound_i256(code, scope, table, id, id_size);
// }

// u32 vm_api4c_proxy::db_next_i256(u32 itr, u32 primary_offset, u32 primary_size) {
//    void *primary = offset_to_ptr(primary_offset, primary_size);
//    return get_vm_api()->db_next_i256(itr, primary, primary_size);
// }

// u32 vm_api4c_proxy::db_previous_i256(u32 itr, u32 primary_offset, u32 primary_size) {
//    void *primary = offset_to_ptr(primary_offset, primary_size);
//    return get_vm_api()->db_previous_i256(itr, primary, primary_size);
// }

// u32 vm_api4c_proxy::db_end_i256(u64 code, u64 scope, u64 table) {
//    return get_vm_api()->db_end_i256(code, scope, table);
// }
