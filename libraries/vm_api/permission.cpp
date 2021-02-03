#include <capi/types.h>
#include <vm_api_proxy.hpp>

extern "C" {

int32_t check_transaction_authorization( const char* trx_data,     uint32_t trx_size,
                                 const char* pubkeys_data, uint32_t pubkeys_size,
                                 const char* perms_data,   uint32_t perms_size
                               ) {
    return get_vm_api()->check_transaction_authorization(trx_data, trx_size,
                                    pubkeys_data, pubkeys_size,
                                    perms_data, perms_size
    );
}

int32_t check_permission_authorization( capi_name account,
                                capi_name permission,
                                const char* pubkeys_data, uint32_t pubkeys_size,
                                const char* perms_data,   uint32_t perms_size,
                                uint64_t delay_us
                              ) {
    get_vm_api()->check_permission_authorization(account, permission,
                                        pubkeys_data, pubkeys_size,
                                        perms_data, perms_size,
                                        delay_us
    );
}

int64_t get_permission_last_used( capi_name account, capi_name permission ) {
    return get_vm_api()->get_permission_last_used(account, permission);
}

int64_t get_account_creation_time( capi_name account ) {
    return get_vm_api()->get_account_creation_time(account);
}

}