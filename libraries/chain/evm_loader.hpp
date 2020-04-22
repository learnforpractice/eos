#pragma once
#include <stdint.h>
#include <stdlib.h>

typedef int (* fn_evm_init)();
typedef int (* fn_evm_execute)(const char *raw_trx, size_t raw_trx_size, const char *sender_address, size_t sender_address_size);
typedef int (* fn_evm_recover_key)(const uint8_t* _sig, size_t _sig_size, const uint8_t* _message, size_t _message_len, uint8_t* _serialized_public_key, size_t _serialized_public_key_size);
typedef int (* fn_evm_call_native)(int type, const uint8_t *packed_args, size_t packed_args_size, uint8_t *output, size_t output_size);
typedef int (* fn_ethereum_vm_execute_trx)(const uint8_t *trx, size_t trx_size, const uint8_t *sender, size_t sender_size);
typedef void (* fn_ethereum_vm_apply)(uint64_t receiver, uint64_t code, uint64_t action);

struct evm_interface {
    fn_evm_init evm_init;
    fn_evm_execute execute;
    fn_evm_call_native call_native;
    fn_evm_recover_key recover_key;

    fn_ethereum_vm_execute_trx ethereum_vm_execute_trx;
    fn_ethereum_vm_apply ethereum_vm_apply;
};

evm_interface& evm_get_interface();
