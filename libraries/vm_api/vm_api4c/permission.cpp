extern "C" {
/* import: 'env' 'check_transaction_authorization' */
u32 (*Z_envZ_check_transaction_authorizationZ_iiiiiii)(u32, u32, u32, u32, u32, u32);
/* import: 'env' 'check_permission_authorization' */
u32 (*Z_envZ_check_permission_authorizationZ_ijjiiiij)(u64, u64, u32, u32, u32, u32, u64);
/* import: 'env' 'get_permission_last_used' */
u64 (*Z_envZ_get_permission_last_usedZ_jjj)(u64, u64);
/* import: 'env' 'get_account_creation_time' */
u64 (*Z_envZ_get_account_creation_timeZ_jj)(u64);

}

static u32 check_transaction_authorization( u32 trx_data_offset, u32 trx_size,
                                 u32 pubkeys_data_offset, u32 pubkeys_size,
                                 u32 perms_data_offset, u32 perms_size
                               ) {
  const char* trx_data = (char *)offset_to_ptr(trx_data_offset, trx_size);
  const char* pubkeys_data = (char *)offset_to_ptr(pubkeys_data_offset, pubkeys_size);
  const char* perms_data = (char *)offset_to_ptr(perms_data_offset, perms_size);

  return get_vm_api()->check_transaction_authorization(trx_data, trx_size,
         pubkeys_data, pubkeys_size,
         perms_data,   perms_size
       );
}

static u32 check_permission_authorization( u64 account,
                                u64 permission,
                                u32 pubkeys_data_offset, u32 pubkeys_size,
                                u32 perms_data_offset,   u32 perms_size,
                                u64 delay_us
                              ) {
  const char* pubkeys_data = (char *)offset_to_ptr(pubkeys_data_offset, pubkeys_size);
  const char* perms_data = (char *)offset_to_ptr(perms_data_offset, perms_size);

  return get_vm_api()->check_permission_authorization( account,
         permission,
         pubkeys_data, pubkeys_size,
         perms_data,   perms_size,
         delay_us
       );
}

static u64 get_permission_last_used( u64 account, u64 permission ) {
   return get_vm_api()->get_permission_last_used( account, permission );
}

static u64 get_account_creation_time( u64 account ) {
   return get_vm_api()->get_account_creation_time( account );
}

