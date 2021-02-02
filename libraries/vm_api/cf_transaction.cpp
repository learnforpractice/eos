#include <vm_api_proxy.hpp>

extern "C" {

size_t read_transaction(char *data, size_t data_size) {
    return get_vm_api()->read_transaction(data, data_size);
}

size_t transaction_size() {
    return get_vm_api()->transaction_size();
}

int32_t expiration() {
    return get_vm_api()->expiration();
}

int32_t tapos_block_num() {
    return get_vm_api()->tapos_block_num();
}

int32_t tapos_block_prefix() {
    return get_vm_api()->tapos_block_prefix();
}

int32_t get_action( uint32_t type, uint32_t index, char *buffer, size_t buffer_size) {
    return get_vm_api()->get_action(type, index, buffer, buffer_size);
}

}