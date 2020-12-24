#include <string.h>
#include "native_contracts.hpp"
#include "wasm-rt-impl.h"
#include <vm_api/vm_api.h>

extern "C" {
std::map<std::array<uint8_t, 32>, contract> contracts_map;

void add_native_contract(std::array<uint8_t, 32>& hash, contract _contract) {
    contracts_map[hash] = _contract;
}

extern void WASM_RT_ADD_PREFIX(init_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_127)(void);
extern wasm_rt_memory_t* get_memory_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_127(void);
extern void (*WASM_RT_ADD_PREFIX(apply_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_127))(u64, u64, u64);
extern wasm_rt_memory_t* get_memory_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_127();

void init_contracts() {

// contracts_map[std::array<uint8_t, 32>{0x5e,0x8e,0x65,0x5a,0x05,0xb3,0x4e,0x78,0x24,0x67,0x68,0x4d,0x71,0x48,0x24,0x44,0x04,0xb5,0x91,0xa6,0xc1,0xec,0x26,0x87,0xea,0xcb,0x26,0x89,0x26,0xa3,0x7e,0x59,}] = contract{
//     .initialized = false,
//     .init = WASM_RT_ADD_PREFIX(init_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_127),
//     .get_memory = get_memory_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_127,
//     .apply = &apply_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_127,
//  };
    
}



void *get_apply_entry(std::array<uint8_t, 32> hash) {
    auto itr = contracts_map.find(hash);
    if (itr == contracts_map.end()) {
        return nullptr;
    }
    if (!itr->second.initialized) {
        wasm_rt_memory_t *mem;
        itr->second.init();
        itr->second.initialized = true;
        mem = itr->second.get_memory();
        mem->data_backup = (uint8_t*)calloc(mem->size, 1);
        mem->data_backup_size = mem->size;
        memcpy(mem->data_backup, mem->data, mem->size);
    } else {
        wasm_rt_memory_t *mem;
        mem = itr->second.get_memory();
        memcpy(mem->data, mem->data_backup, mem->data_backup_size);
        memset(mem->data + mem->data_backup_size, 0, mem->initial_pages * PAGE_SIZE-mem->data_backup_size);
    }
    return (void *)(**itr->second.apply);
}

wasm_rt_memory_t *get_contract_memory(std::array<uint8_t, 32> hash) {
    wasm_rt_memory_t *mem;
    auto itr = contracts_map.find(hash);
    if (itr == contracts_map.end()) {
        return nullptr;
    }
    if (!itr->second.initialized) {
        itr->second.init();
        itr->second.initialized = true;
        mem = itr->second.get_memory();
        mem->data_backup = (uint8_t*)calloc(mem->size, 1);
        mem->data_backup_size = mem->size;
        memcpy(mem->data_backup, mem->data, mem->size);
    } else {
        mem = itr->second.get_memory();
    }
    return mem;
}

void vm_on_trap(wasm_rt_trap_t code) {
   switch (code) {
      case WASM_RT_TRAP_NONE:
         EOSIO_THROW("vm no error");
         break;
      case WASM_RT_TRAP_OOB:
         EOSIO_THROW("vm error out of bounds");
         break;
      case WASM_RT_TRAP_INT_OVERFLOW:
         EOSIO_THROW("vm error int overflow");
         break;
      case WASM_RT_TRAP_DIV_BY_ZERO:
         EOSIO_THROW("vm error divide by zeror");
         break;
      case WASM_RT_TRAP_INVALID_CONVERSION:
         EOSIO_THROW("vm error invalid conversion");
         break;
      case WASM_RT_TRAP_UNREACHABLE:
         EOSIO_THROW("vm error unreachable");
         break;
      case WASM_RT_TRAP_CALL_INDIRECT:
         EOSIO_THROW("vm error call indirect");
         break;
      case WASM_RT_TRAP_EXHAUSTION:
         EOSIO_THROW("vm error exhaustion");
         break;
      default:
         EOSIO_THROW("vm unknown error");
         break;
   }
}

typedef void (*fn_contract_apply)(uint64_t receiver, uint64_t first_receiver, uint64_t action);

bool native_contract_apply(std::array<uint8_t, 32>& hash, uint64_t receiver, uint64_t first_receiver, uint64_t action) {
    fn_contract_apply apply = (fn_contract_apply)get_apply_entry(hash);
    if (!apply) {
        return false;
    }

    wasm_rt_trap_t code = (wasm_rt_trap_t)wasm_rt_impl_try2();
    if (code != 0) {
//        printf("A trap occurred with code: %d\n", code);
        vm_on_trap(code);
    }
    (*apply)(receiver, first_receiver, action);
    return true;
}


}
