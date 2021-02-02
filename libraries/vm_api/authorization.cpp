#include <vm_api_proxy.hpp>

extern "C" {

void require_auth(uint64_t account) {
   get_vm_api()->require_auth(account);
}

bool has_auth(uint64_t account) {
   return get_vm_api()->has_auth(account);
}

void require_auth2(uint64_t account, uint64_t permission ) {
   get_vm_api()->require_auth2(account, permission);
}

void require_recipient(uint64_t recipient) {
   get_vm_api()->require_recipient(recipient);
}

bool is_account(uint64_t account ) {
   return get_vm_api()->is_account(account);
}

}


