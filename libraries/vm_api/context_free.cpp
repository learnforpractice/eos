#include <vm_api_proxy.hpp>

extern "C" {

int32_t get_context_free_data(uint32_t index, char *data, size_t data_size) {
    return get_vm_api()->get_context_free_data(index, data, data_size);
}

}
