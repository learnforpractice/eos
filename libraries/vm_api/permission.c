#include <capi/types.h>
#include <vm_api_proxy.hpp>

int64_t get_permission_last_used( capi_name account, capi_name permission ) {
    return get_vm_api()->get_permission_last_used(account, permission);
}

int64_t get_account_creation_time( capi_name account ) {
    return get_vm_api()->get_account_creation_time(account);
}
