#include <vm_api_proxy.hpp>

extern "C" {

size_t read_action_data(char* msg, size_t len) {
    return get_vm_api()->read_action_data(msg, len);
}

size_t action_data_size() {
    return get_vm_api()->action_data_size();
}

uint64_t current_receiver() {
   return get_vm_api()->current_receiver();
}

void set_action_return_value(const char *data, size_t data_size) {
    get_vm_api()->set_action_return_value(data, data_size);
}

}