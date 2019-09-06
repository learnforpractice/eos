#include <array>
#include <map>

#include "eosio.system.h"
#include <eosiolib_native/vm_api.h>
#include "wasm-rt-impl.h"

using namespace std;

//contracts.cpp
extern "C" {
   void init_contracts();
   wasm_rt_memory_t *get_contract_memory(std::array<uint8_t, 32> hash);
}

struct native_calls
{
//    void (*init)();
    void (*apply)(uint64_t receiver, uint64_t first_receiver ,uint64_t action);
};

static map<std::array<uint8_t, 32>, native_calls> call_map;

extern "C" {
    void WASM_RT_ADD_PREFIX(init)(void);
    void (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(u64, u64, u64);

    void (*WASM_RT_ADD_PREFIX(eosio_token_apply))(u64, u64, u64);
    void WASM_RT_ADD_PREFIX(eosio_token_init)(void);

    wasm_rt_memory_t* get_wasm_rt_memory();
}

void init_eosio_system() {
{
//2aba9fdda5cba904701d5bbf79de32f99f9815ad611e0ee20ca323ea40919d5f
    std::array<uint8_t,32> hash = std::array<uint8_t,32>{0x2a,0xba,0x9f,0xdd,0xa5,0xcb,0xa9,0x04,0x70,0x1d,0x5b,0xbf,0x79,0xde,0x32,0xf9,0x9f,0x98,0x15,0xad,0x61,0x1e,0x0e,0xe2,0x0c,0xa3,0x23,0xea,0x40,0x91,0x9d,0x5f};
    call_map[hash] = native_calls {
        .apply = WASM_RT_ADD_PREFIX(Z_applyZ_vjjj)
    };
}

#define init native_init
#define apply native_apply
#undef init
#undef apply

}

extern "C" void init_vm_api4c();

void vm_on_trap(wasm_rt_trap_t code) {
   get_vm_api()->eosio_assert(0, "vm runtime error");
   switch (code) {
      case WASM_RT_TRAP_NONE:
         get_vm_api()->eosio_assert(0, "vm no error");
         break;
      case WASM_RT_TRAP_OOB:
         get_vm_api()->eosio_assert(0, "vm error out of bounds");
         break;
      case WASM_RT_TRAP_INT_OVERFLOW:
         get_vm_api()->eosio_assert(0, "vm error int overflow");
         break;
      case WASM_RT_TRAP_DIV_BY_ZERO:
         get_vm_api()->eosio_assert(0, "vm error divide by zeror");
         break;
      case WASM_RT_TRAP_INVALID_CONVERSION:
         get_vm_api()->eosio_assert(0, "vm error invalid conversion");
         break;
      case WASM_RT_TRAP_UNREACHABLE:
         get_vm_api()->eosio_assert(0, "vm error unreachable");
         break;
      case WASM_RT_TRAP_CALL_INDIRECT:
         get_vm_api()->eosio_assert(0, "vm error call indirect");
         break;
      case WASM_RT_TRAP_EXHAUSTION:
         get_vm_api()->eosio_assert(0, "vm error exhaustion");
         break;
      default:
         get_vm_api()->eosio_assert(0, "vm unknown error");
         break;
   }
}

extern "C" void native_contracts_init() {
    init_eosio_system();
    init_vm_api4c();
    init_contracts();
    wasm_rt_trap_t code = (wasm_rt_trap_t)wasm_rt_impl_try();
    if (code != 0) {
        printf("A trap occurred with code: %d\n", code);
        vm_on_trap(code);
    }
    (*WASM_RT_ADD_PREFIX(init))();
}

extern "C" void* get_native_eosio_system_apply_entry(uint8_t *hash, size_t size) {
    std::array<uint8_t, 32> arr;
//    get_vm_api()->eosio_assert(size == 32, "bad hash size!");
    memcpy(arr.data(), hash, 32);
    auto itr = call_map.find(arr);
    if (itr == call_map.end()) {
        return nullptr;
    }
    return (void *)itr->second.apply;
}

extern "C" void native_eosio_system_apply(uint64_t a, uint64_t b, uint64_t c) {
    (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(a, b, c);
}

#if 0
void *offset_to_ptr(u32 offset, u32 size) {
    wasm_rt_memory_t *memory = get_wasm_rt_memory();
//    get_vm_api()->eosio_assert(offset + size <= memory->size, "memory access overflow!");
     return memory->data + offset;
}
#endif

extern "C" wasm_rt_memory_t* get_eosio_system_memory();
extern "C" wasm_rt_memory_t* get_eosio_token_memory();

extern "C" wasm_rt_memory_t* get_wasm_rt_memory() {
#if 0
    if (receiver == 6138663577826885632) { //eosio
       return get_eosio_system_memory();
    } else if (receiver == 6138663591592764928) {//eosio.token
      return get_eosio_token_memory();
    }
#endif
    uint64_t receiver = get_vm_api()->current_receiver();
    std::array<uint8_t,32> hash;
    get_vm_api()->get_code_version(receiver, (char *)hash.data(), 32);
   return get_contract_memory(hash);

   get_vm_api()->eosio_assert(0, "memory not found!");
   return nullptr;
}

extern "C" void *offset_to_ptr(u32 offset, u32 size) {
    wasm_rt_memory_t *memory = get_wasm_rt_memory();
//    printf("++++++offset %u, size %u\n", offset, size);
    get_vm_api()->eosio_assert(memory != nullptr, "memory should not be null");
    get_vm_api()->eosio_assert(offset + size <= memory->size && offset + size >= offset, "memory access out of bounds");
    return memory->data + offset;
}

extern "C" void *offset_to_char_ptr(u32 offset) {
    wasm_rt_memory_t *memory = get_wasm_rt_memory();
    get_vm_api()->eosio_assert(memory != nullptr, "memory should not be null");
    for (int i=offset;i<memory->size;i++) {
        if (memory->data[i] == '\0') {
            return memory->data + offset; 
        }
    }
    get_vm_api()->eosio_assert(0, "not a valid c string!");
    return NULL;
}

extern "C" void native_eosio_token_apply(uint64_t receiver, uint64_t first_receiver, uint64_t action) {
    (*WASM_RT_ADD_PREFIX(eosio_token_apply))(receiver, first_receiver, action);
}
