#include <array>
#include <map>

#include "eosio.system.h"

using namespace std;

struct native_calls
{
//    void (*init)();
    void (*apply)(uint64_t receiver, uint64_t first_receiver ,uint64_t action);
};

static map<std::array<uint8_t, 32>, native_calls> call_map;

extern "C" {
    void WASM_RT_ADD_PREFIX(init)(void);
    void (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(u64, u64, u64);
    
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

extern "C" void native_eosio_system_init() {
    (*WASM_RT_ADD_PREFIX(init))();
    init_eosio_system();
    init_vm_api4c();
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
    static bool initialized = false;
    if (!initialized) {
        init_vm_api4c();
        (*WASM_RT_ADD_PREFIX(init))();
        initialized = true;
    }
    (*WASM_RT_ADD_PREFIX(Z_applyZ_vjjj))(a, b, c);
}

void *offset_to_ptr(u32 offset, u32 size) {
    wasm_rt_memory_t *memory = get_wasm_rt_memory();
     return memory->data + offset;
}
