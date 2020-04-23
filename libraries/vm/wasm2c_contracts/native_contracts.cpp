#include <string.h>
#include "wasm-rt-impl.h"
#include <vm_api/vm_api.h>

#include "native_contracts.hpp"
#include "native_contracts.h"

extern "C" {
    void eosio_native_init_globals();

    /* export: 'eosio_system_get_delegated_balance' */
    extern u64 (*WASM_RT_ADD_PREFIX(Z_eosio_system_get_delegated_balanceZ_jj))(u64);
    /* export: 'eosio_system_get_rex_fund' */
    extern u64 (*WASM_RT_ADD_PREFIX(Z_eosio_system_get_rex_fundZ_jj))(u64);
    /* export: 'system_contract_is_vm_activated' */
    extern u32 (*WASM_RT_ADD_PREFIX(Z_system_contract_is_vm_activatedZ_iii))(u32, u32);

   int64_t eosio_system_get_delegated_balance(uint64_t owner) {
       eosio_native_init_globals();
      return Z_eosio_system_get_delegated_balanceZ_jj(owner);
   }

   int64_t eosio_system_get_rex_fund(uint64_t owner) {
       eosio_native_init_globals();
      return Z_eosio_system_get_rex_fundZ_jj(owner);
   }

   bool system_contract_is_vm_activated( uint8_t vmtype, uint8_t vmversion ) {
       eosio_native_init_globals();
      return Z_system_contract_is_vm_activatedZ_iii(vmtype, vmversion);
   }

}

