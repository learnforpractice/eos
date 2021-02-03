#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include "vm_api_proxy.hpp"

int32_t vm_api_proxy::check_transaction_authorization(
                                        const char *trx_data,     uint32_t trx_size,
                                        const char *pubkeys_data, uint32_t pubkeys_size,
                                        const char *perms_data,   uint32_t perms_size) {
    legacy_span<const char> _trx_data((void *)trx_data, trx_size);
    legacy_span<const char> _pubkeys_data((void *)pubkeys_data, pubkeys_size);
    legacy_span<const char> _perms_data((void *)perms_data, perms_size);
    return _interface->check_transaction_authorization(std::move(_trx_data), std::move(_pubkeys_data), std::move(_perms_data));
}

int32_t vm_api_proxy::check_permission_authorization(
                                       uint64_t account,
                                       uint64_t permission,
                                       const char *pubkeys_data, uint32_t pubkeys_size,
                                       const char *perms_data,   uint32_t perms_size,
                                       uint64_t delay_us
) {
    legacy_span<const char> _pubkeys_data((void *)pubkeys_data, pubkeys_size);
    legacy_span<const char> _perms_data((void *)perms_data, perms_size);
    return _interface->check_permission_authorization(name(account),
                                    name(permission),
                                    std::move(_pubkeys_data),
                                    std::move(_perms_data),
                                    delay_us
    );
}

int64_t vm_api_proxy::get_permission_last_used(uint64_t account, uint64_t permission) {
    return _interface->get_permission_last_used(name(account), name(permission));
}

int64_t vm_api_proxy::get_account_creation_time( uint64_t account ) {
    return _interface->get_account_creation_time(name(account));
}
