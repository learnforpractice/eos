#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include "vm_api_proxy.hpp"

/* these are both unfortunate that we didn't make the return type an int64_t */
uint64_t vm_api_proxy::current_time() {
   return _interface->current_time();
}

uint64_t vm_api_proxy::publication_time() {
   return _interface->publication_time();
}

bool vm_api_proxy::is_feature_activated(const capi_checksum256* feature_digest) {
    fc::sha256 _digest((char *)feature_digest->hash, 32);
    legacy_ptr<const fc::sha256> __digest((void *)&_digest);
    return _interface->is_feature_activated(std::move(__digest));
}

uint64_t vm_api_proxy::get_sender() {
   return _interface->get_sender().to_uint64_t();
}
