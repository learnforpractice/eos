#include <capi/types.h>
#include <vm_api_proxy.hpp>

uint32_t get_active_producers( capi_name* producers, uint32_t datalen ) {
    return get_vm_api()->get_active_producers(producers, datalen);
}
