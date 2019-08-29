/* import: 'env' 'db_end_i64' */
u32 (*Z_envZ_db_end_i64Z_ijjj)(u64, u64, u64);

u32 (*Z_envZ_db_get_countZ_ijjj)(u64, u64, u64);

/* import: 'env' 'db_find_i64' */
u32 (*Z_envZ_db_find_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_get_i64' */
u32 (*Z_envZ_db_get_i64Z_iiii)(u32, u32, u32);
/* import: 'env' 'db_idx128_end' */
u32 (*Z_envZ_db_idx128_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx128_find_primary' */
u32 (*Z_envZ_db_idx128_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx128_find_secondary' */
u32 (*Z_envZ_db_idx128_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx128_lowerbound' */
u32 (*Z_envZ_db_idx128_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx128_next' */
u32 (*Z_envZ_db_idx128_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx128_previous' */
u32 (*Z_envZ_db_idx128_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx128_store' */
u32 (*Z_envZ_db_idx128_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx128_update' */
void (*Z_envZ_db_idx128_updateZ_viji)(u32, u64, u32);
void (*Z_envZ_db_idx128_removeZ_vi)(u32);
/* import: 'env' 'db_idx128_upperbound' */
u32 (*Z_envZ_db_idx128_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx256_end' */
u32 (*Z_envZ_db_idx256_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx256_find_primary' */
u32 (*Z_envZ_db_idx256_find_primaryZ_ijjjiij)(u64, u64, u64, u32, u32, u64);
/* import: 'env' 'db_idx256_find_secondary' */
u32 (*Z_envZ_db_idx256_find_secondaryZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx256_lowerbound' */
u32 (*Z_envZ_db_idx256_lowerboundZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx256_next' */
u32 (*Z_envZ_db_idx256_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx256_previous' */
u32 (*Z_envZ_db_idx256_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx256_remove' */
void (*Z_envZ_db_idx256_removeZ_vi)(u32);
/* import: 'env' 'db_idx256_update' */
void (*Z_envZ_db_idx256_updateZ_vijii)(u32, u64, u32, u32);
/* import: 'env' 'db_idx256_upperbound' */
u32 (*Z_envZ_db_idx256_upperboundZ_ijjjiii)(u64, u64, u64, u32, u32, u32);
/* import: 'env' 'db_idx64_end' */
u32 (*Z_envZ_db_idx64_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx64_find_primary' */
u32 (*Z_envZ_db_idx64_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx64_find_secondary' */
u32 (*Z_envZ_db_idx64_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx64_lowerbound' */
u32 (*Z_envZ_db_idx64_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx64_next' */
u32 (*Z_envZ_db_idx64_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx64_previous' */
u32 (*Z_envZ_db_idx64_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx64_remove' */
void (*Z_envZ_db_idx64_removeZ_vi)(u32);
/* import: 'env' 'db_idx64_store' */
u32 (*Z_envZ_db_idx64_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx64_update' */
void (*Z_envZ_db_idx64_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx64_upperbound' */
u32 (*Z_envZ_db_idx64_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_double_end' */
u32 (*Z_envZ_db_idx_double_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx_double_find_primary' */
u32 (*Z_envZ_db_idx_double_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx_double_find_secondary' */
u32 (*Z_envZ_db_idx_double_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_double_lowerbound' */
u32 (*Z_envZ_db_idx_double_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_double_next' */
u32 (*Z_envZ_db_idx_double_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx_double_previous' */
u32 (*Z_envZ_db_idx_double_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx_double_remove' */
void (*Z_envZ_db_idx_double_removeZ_vi)(u32);
/* import: 'env' 'db_idx_double_store' */
u32 (*Z_envZ_db_idx_double_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx_double_update' */
void (*Z_envZ_db_idx_double_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx_double_upperbound' */
u32 (*Z_envZ_db_idx_double_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_end' */
u32 (*Z_envZ_db_idx_long_double_endZ_ijjj)(u64, u64, u64);
/* import: 'env' 'db_idx_long_double_find_primary' */
u32 (*Z_envZ_db_idx_long_double_find_primaryZ_ijjjij)(u64, u64, u64, u32, u64);
/* import: 'env' 'db_idx_long_double_find_secondary' */
u32 (*Z_envZ_db_idx_long_double_find_secondaryZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_lowerbound' */
u32 (*Z_envZ_db_idx_long_double_lowerboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_idx_long_double_next' */
u32 (*Z_envZ_db_idx_long_double_nextZ_iii)(u32, u32);
/* import: 'env' 'db_idx_long_double_previous' */
u32 (*Z_envZ_db_idx_long_double_previousZ_iii)(u32, u32);
/* import: 'env' 'db_idx_long_double_remove' */
void (*Z_envZ_db_idx_long_double_removeZ_vi)(u32);
/* import: 'env' 'db_idx_long_double_store' */
u32 (*Z_envZ_db_idx_long_double_storeZ_ijjjji)(u64, u64, u64, u64, u32);
/* import: 'env' 'db_idx_long_double_update' */
void (*Z_envZ_db_idx_long_double_updateZ_viji)(u32, u64, u32);
/* import: 'env' 'db_idx_long_double_upperbound' */
u32 (*Z_envZ_db_idx_long_double_upperboundZ_ijjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'db_lowerbound_i64' */
u32 (*Z_envZ_db_lowerbound_i64Z_ijjjj)(u64, u64, u64, u64);
/* import: 'env' 'db_next_i64' */
u32 (*Z_envZ_db_next_i64Z_iii)(u32, u32);
/* import: 'env' 'db_remove_i64' */
void (*Z_envZ_db_remove_i64Z_vi)(u32);
/* import: 'env' 'db_store_i64' */
u32 (*Z_envZ_db_store_i64Z_ijjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'db_update_i64' */
void (*Z_envZ_db_update_i64Z_vijii)(u32, u64, u32, u32);
/* import: 'env' 'db_upperbound_i64' */
u32 (*Z_envZ_db_upperbound_i64Z_ijjjj)(u64, u64, u64, u64);
u32 (*Z_envZ_db_previous_i64Z_iii)(u32, u32);

u32 (*Z_envZ_db_get_table_countZ_ijjj)(u64, u64, u64);


static u32 db_store_i64(u64 scope, u64 table, u64 payer, u64 id,  u32 data_offset, u32 len) {
   const void* data = (void *)offset_to_ptr(data_offset, len);
   return get_vm_api()->db_store_i64(scope, table, payer, id, (const char*)data, len);
}

static void db_update_i64(u32 iterator, u64 payer, u32 data_offset, u32 len) {
   const void* data = (void *)offset_to_ptr(data_offset, len);
   get_vm_api()->db_update_i64( iterator, payer, (const char*)data, len);
}

static void db_remove_i64(u32 iterator) {
   get_vm_api()->db_remove_i64(iterator);
}

static u32 db_get_i64(u32 iterator, u32 data_offset, u32 len) {
   void* data = (void *)offset_to_ptr(data_offset, len);
   return get_vm_api()->db_get_i64(iterator, data, len);
}

static u32 db_next_i64(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_next_i64(iterator, primary);
}

static u32 db_previous_i64(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_previous_i64(iterator, primary);
}

static u32 db_find_i64(u64 code, u64 scope, u64 table, u64 id) {
   return get_vm_api()->db_find_i64(code, scope, table, id);
}

static u32 db_lowerbound_i64(u64 code, u64 scope, u64 table, u64 id) {
   return get_vm_api()->db_lowerbound_i64(code, scope, table, id);
}

static u32 db_upperbound_i64(u64 code, u64 scope, u64 table, u64 id) {
   return get_vm_api()->db_upperbound_i64(code, scope, table, id);
}

static u32 db_end_i64(uint64_t code, uint64_t scope, uint64_t table) {
   return get_vm_api()->db_end_i64(code, scope, table);
}

static u32 db_get_count(uint64_t code, uint64_t scope, uint64_t table) {
   return get_vm_api()->db_get_table_count(code, scope, table);
}

static u32 db_idx64_store(u64 scope, u64 table, u64 payer, u64 id, u32 secondary_offset) {
   const uint64_t* secondary = (uint64_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx64_store(scope, table, payer, id, secondary);
}

static void db_idx64_update(u32 iterator, u64 payer, u32 secondary_offset) {
   const uint64_t* secondary = (uint64_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   get_vm_api()->db_idx64_update(iterator, payer, secondary);
}

static void db_idx64_remove(u32 iterator) {
   get_vm_api()->db_idx64_remove(iterator);
}

static u32 db_idx64_next(u32 iterator, u32 primary_offset) {
   uint64_t *primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx64_next(iterator, primary);
}

static u32 db_idx64_previous(u32 iterator, u32 primary_offset) {
   uint64_t *primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx64_previous(iterator, primary);
}

static u32 db_idx64_find_primary(u64 code, u64 scope, u64 table, u32 secondary_offset, u64 primary) {
   uint64_t* secondary = (uint64_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx64_find_primary(code, scope, table, secondary, primary);
}

static u32 db_idx64_find_secondary(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   const uint64_t* secondary = (uint64_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx64_find_secondary(code, scope, table, secondary, primary);
}

static u32 db_idx64_lowerbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* secondary = (uint64_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx64_lowerbound(code, scope, table, secondary, primary);
}

static u32 db_idx64_upperbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* secondary = (uint64_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx64_upperbound(code, scope, table, secondary, primary);
}

static u32 db_idx64_end(u64 code, u64 scope, u64 table) {
   return get_vm_api()->db_idx64_end(code, scope, table);
}

static u32 db_idx128_store(u64 scope, u64 table, u64 payer, u64 id, u32 secondary_offset) {
   const uint128_t* secondary = (uint128_t *)offset_to_ptr(secondary_offset, sizeof(uint128_t));
   return get_vm_api()->db_idx128_store(scope, table, payer, id, secondary);
}

static void db_idx128_update(u32 iterator, u64 payer, u32 secondary_offset) {
   const uint128_t* secondary = (uint128_t *)offset_to_ptr(secondary_offset, sizeof(uint128_t));
   get_vm_api()->db_idx128_update(iterator, payer, secondary);
}

static void db_idx128_remove(u32 iterator) {
   get_vm_api()->db_idx128_remove(iterator);
}

static u32 db_idx128_next(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx128_next(iterator, primary);
}

static u32 db_idx128_previous(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx128_previous(iterator, primary) ;
}

static u32 db_idx128_find_primary(u64 code, u64 scope, u64 table, u32 secondary_offset, u64 primary) {
   uint128_t* secondary = (uint128_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx128_find_primary(code, scope, table, secondary, primary);
}

static u32 db_idx128_find_secondary(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   uint128_t* secondary = (uint128_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx128_find_secondary(code, scope, table, secondary, primary);
}

static u32 db_idx128_lowerbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   uint128_t* secondary = (uint128_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx128_lowerbound(code, scope, table, secondary, primary);
}

static u32 db_idx128_upperbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   uint128_t* secondary = (uint128_t *)offset_to_ptr(secondary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx128_upperbound(code, scope, table, secondary, primary);
}

static u32 db_idx128_end(u64 code, u64 scope, u64 table) {
   return get_vm_api()->db_idx128_end(code, scope, table);
}

static u32 db_idx256_store(u64 scope, u64 table, u64 payer, u64 id, u32 data_offset, u32 data_len ) {
   uint128_t* data = (uint128_t *)offset_to_ptr(data_offset, sizeof(uint128_t));
   return get_vm_api()->db_idx256_store(scope, table, payer, id, data, data_len );
}

static void db_idx256_update(u32 iterator, u64 payer, u32 data_offset, u32 data_len) {
   uint128_t* data = (uint128_t *)offset_to_ptr(data_offset, sizeof(uint128_t));
   get_vm_api()->db_idx256_update(iterator, payer, data, data_len);
}

static void db_idx256_remove(u32 iterator) {
   get_vm_api()->db_idx256_remove(iterator);
}

static u32 db_idx256_next(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx256_next(iterator, primary);
}

static u32 db_idx256_previous(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx256_previous(iterator, primary);
}

static u32 db_idx256_find_primary(u64 code, u64 scope, u64 table, u32 data_offset, u32 data_len, u64 primary) {
   uint128_t* data = (uint128_t *)offset_to_ptr(data_offset, sizeof(uint128_t));
   return get_vm_api()->db_idx256_find_primary(code, scope, table, data, data_len, primary);
}

static u32 db_idx256_find_secondary(u64 code, u64 scope, u64 table, u32 data_offset, u32 data_len, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   const uint128_t* data = (uint128_t *)offset_to_ptr(data_offset, sizeof(uint128_t));
   return get_vm_api()->db_idx256_find_secondary(code, scope, table, data, data_len, primary);
}

static u32 db_idx256_lowerbound(u64 code, u64 scope, u64 table, u32 data_offset, u32 data_len, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   uint128_t* data = (uint128_t *)offset_to_ptr(data_offset, sizeof(uint128_t));
   return get_vm_api()->db_idx256_lowerbound(code, scope, table, data, data_len, primary);
}

static u32 db_idx256_upperbound(u64 code, u64 scope, u64 table, u32 data_offset, u32 data_len, u32 primary_offset) {
   uint64_t* primary = (uint64_t *)offset_to_ptr(primary_offset, sizeof(uint64_t));
   uint128_t* data = (uint128_t *)offset_to_ptr(data_offset, sizeof(uint128_t));
   return get_vm_api()->db_idx256_upperbound(code, scope, table, data, data_len, primary);
}

static u32 db_idx256_end(u64 code, u64 scope, u64 table) {
   return get_vm_api()->db_idx256_end(code, scope, table);
}

static u32 db_idx_double_store(u64 scope, u64 table, u64 payer, u64 id, u32 secondary_offset) {
   const double* secondary = (double*)offset_to_ptr(secondary_offset, sizeof(double));
   return get_vm_api()->db_idx_double_store(scope, table, payer, id, (const float64_t*)secondary);
}

static void db_idx_double_update(u32 iterator, u64 payer, u32 secondary_offset) {
   const double* secondary = (double*)offset_to_ptr(secondary_offset, sizeof(double));
   get_vm_api()->db_idx_double_update(iterator, payer, (const float64_t*)secondary);
}

static void db_idx_double_remove(u32 iterator) {
   get_vm_api()->db_idx_double_remove(iterator);
}

static u32 db_idx_double_next(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx_double_next(iterator, primary);
}

static u32 db_idx_double_previous(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx_double_previous(iterator, primary);
}

static u32 db_idx_double_find_primary(u64 code, u64 scope, u64 table, u32 secondary_offset, u64 primary) {
   float64_t* secondary = (float64_t*)offset_to_ptr(secondary_offset, sizeof(float64_t));
   return get_vm_api()->db_idx_double_find_primary(code, scope, table, (float64_t*)secondary, primary);
}

static u32 db_idx_double_find_secondary(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   float64_t* secondary = (float64_t*)offset_to_ptr(secondary_offset, sizeof(float64_t));
   return get_vm_api()->db_idx_double_find_secondary(code, scope, table, (const float64_t*)secondary, primary);
}

static u32 db_idx_double_lowerbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   float64_t* secondary = (float64_t*)offset_to_ptr(secondary_offset, sizeof(float64_t));
   return get_vm_api()->db_idx_double_lowerbound(code, scope, table, (float64_t*)secondary, primary);
}

static u32 db_idx_double_upperbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   float64_t* secondary = (float64_t*)offset_to_ptr(secondary_offset, sizeof(float64_t));
   return get_vm_api()->db_idx_double_upperbound(code, scope, table, (float64_t*)secondary, primary);
}

static u32 db_idx_double_end(u64 code, u64 scope, u64 table) {
   return get_vm_api()->db_idx_double_end(code, scope, table);
}

static u32 db_idx_long_double_store(u64 scope, u64 table, u64 payer, u64 id, u32 secondary_offset) {
   float128_t *secondary = (float128_t *)offset_to_ptr(secondary_offset, sizeof(float128_t));
   return get_vm_api()->db_idx_long_double_store(scope, table, payer, id, (const float128_t*)secondary);
}

static void db_idx_long_double_update(u32 iterator, u64 payer, u32 secondary_offset) {
   float128_t *secondary = (float128_t *)offset_to_ptr(secondary_offset, sizeof(float128_t));
   get_vm_api()->db_idx_long_double_update(iterator, payer, (const float128_t*)secondary);
}

static void db_idx_long_double_remove(u32 iterator) {
   get_vm_api()->db_idx_long_double_remove(iterator);
}

static u32 db_idx_long_double_next(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx_long_double_next(iterator, primary);
}

static u32 db_idx_long_double_previous(u32 iterator, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   return get_vm_api()->db_idx_long_double_previous(iterator, primary);
}

static u32 db_idx_long_double_find_primary(u64 code, u64 scope, u64 table, u32 secondary_offset, u64 primary) {
   float128_t* secondary = (float128_t*)offset_to_ptr(secondary_offset, sizeof(float128_t));
   return get_vm_api()->db_idx_long_double_find_primary(code, scope, table, (float128_t*)secondary, primary);
}

static u32 db_idx_long_double_find_secondary(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   const float128_t* secondary = (float128_t*)offset_to_ptr(secondary_offset, sizeof(float128_t));
   return get_vm_api()->db_idx_long_double_find_secondary(code, scope, table, (const float128_t*)secondary, primary);
}

static u32 db_idx_long_double_lowerbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   const float128_t* secondary = (float128_t*)offset_to_ptr(secondary_offset, sizeof(float128_t));
   return get_vm_api()->db_idx_long_double_lowerbound(code, scope, table, (float128_t*)secondary, primary);
}

static u32 db_idx_long_double_upperbound(u64 code, u64 scope, u64 table, u32 secondary_offset, u32 primary_offset) {
   uint64_t* primary = (uint64_t*)offset_to_ptr(primary_offset, sizeof(uint64_t));
   const float128_t* secondary = (float128_t*)offset_to_ptr(secondary_offset, sizeof(float128_t));
   return get_vm_api()->db_idx_long_double_upperbound(code, scope, table, (float128_t*)secondary, primary);
}

static u32 db_idx_long_double_end(u64 code, u64 scope, u64 table) {
   return get_vm_api()->db_idx_long_double_end(code, scope, table);
}

static u32 db_get_table_count(u64 code, u64 scope, u64 table) {
   return get_vm_api()->db_get_table_count(code, scope, table);
}


#if 0
int db_store_i256( u64 scope, u64 table, u64 payer, void* id, int size, const char* buffer, size_t buffer_size ) {
   return get_vm_api()->db_store_i256(scope, table, payer, id, size, buffer, buffer_size);
}

static void db_update_i256( int iterator, u64 payer, const char* buffer, size_t buffer_size ) {
   return get_vm_api()->db_update_i256(iterator, payer, buffer, buffer_size);
}

static void db_remove_i256( int iterator ) {
   return get_vm_api()->db_remove_i256(iterator);
}

int db_get_i256( int iterator, char* buffer, size_t buffer_size ) {
   return get_vm_api()->db_get_i256(iterator, buffer, buffer_size);
}

int db_find_i256( u64 code, u64 scope, u64 table, void* id, int size ) {
   return get_vm_api()->db_find_i256(code, scope, table, id, size);
}

int db_upperbound_i256( u64 code, u64 scope, u64 table, char* id, int size ) {
   return get_vm_api()->db_upperbound_i256(code, scope, table, id, size);
}

int db_lowerbound_i256( u64 code, u64 scope, u64 table, char* id, int size ) {
   return get_vm_api()->db_lowerbound_i256(code, scope, table, id, size);
}

#endif


static void init_db() {
   Z_envZ_db_lowerbound_i64Z_ijjjj = db_lowerbound_i64 ;
   Z_envZ_db_next_i64Z_iii = db_next_i64 ;
   Z_envZ_db_remove_i64Z_vi = db_remove_i64 ;
   Z_envZ_db_store_i64Z_ijjjjii = db_store_i64 ;
   Z_envZ_db_update_i64Z_vijii = db_update_i64 ;
   Z_envZ_db_upperbound_i64Z_ijjjj = db_upperbound_i64 ;
   Z_envZ_db_end_i64Z_ijjj = db_end_i64 ;
   Z_envZ_db_get_countZ_ijjj = db_get_count ;

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

   Z_envZ_db_get_table_countZ_ijjj = db_get_table_count;
}
