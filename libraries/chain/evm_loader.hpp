#pragma once
#include <stdint.h>
#include <stdlib.h>

typedef int (* fn_evm_get_account_id)(const char* account, size_t account_size, const char* arbitrary_string, size_t arbitrary_string_size, char* hash, size_t hash_size);
typedef int (* fn_evm_execute)(const char *raw_trx, size_t raw_trx_size, const char *sender_address, size_t sender_address_size);
typedef int (* fn_evm_recover_key)(const uint8_t* _sig, uint32_t _sig_size, const uint8_t* _message, uint32_t _message_len, uint8_t* _serialized_public_key, uint32_t _serialized_public_key_size);
typedef int (* fn_evm_init)();
struct evm_interface {
    fn_evm_init evm_init;
    fn_evm_get_account_id get_account_id;
    fn_evm_execute execute;
    fn_evm_recover_key recover_key;
};

evm_interface& evm_get_interface();
