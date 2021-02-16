#include "vm_api4c.hpp"
#include <vm_api_proxy.hpp>

u32 check_transaction_authorization( u32 trx_data_offset, u32 trx_size,
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

u32 check_permission_authorization( u64 account,
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

u64 get_permission_last_used( u64 account, u64 permission ) {
   return get_vm_api()->get_permission_last_used( account, permission );
}

u64 get_account_creation_time( u64 account ) {
   return get_vm_api()->get_account_creation_time( account );
}

