#include <vm_api_proxy.hpp>

extern "C" {

size_t get_active_producers( uint64_t *producers, size_t size ) {
    return get_vm_api()->get_active_producers(producers, size);
}

}