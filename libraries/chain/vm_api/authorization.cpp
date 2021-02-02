#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include "vm_api_proxy.hpp"

void vm_api_proxy::require_auth(uint64_t account) {
   _interface->require_auth(name(account));
}

bool vm_api_proxy::has_auth(uint64_t account) {
   return _interface->has_auth(name(account));
}

void vm_api_proxy::require_auth2(uint64_t account, uint64_t permission ) {
   _interface->require_auth2(name(account), name(permission));
}

void vm_api_proxy::require_recipient(uint64_t recipient) {
   _interface->require_recipient(name(recipient));
}

bool vm_api_proxy::is_account(uint64_t account ) {
   return _interface->is_account(name(account));
}
