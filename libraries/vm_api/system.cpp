#include <vm_api_proxy.hpp>

extern "C" {

/* these are both unfortunate that we didn't make the return type an int64_t */
uint64_t vm_api_proxy::current_time() {
   return get_vm_api()->current_time();
}

uint64_t vm_api_proxy::publication_time() {
   return get_vm_api()->publication_time();
}

bool vm_api_proxy::is_feature_activated(const char *digest, size_t size) {
    return get_vm_api()->is_feature_activated(digest, size);
}

uint64_t vm_api_proxy::get_sender() {
   return get_vm_api()->get_sender();
}

}
