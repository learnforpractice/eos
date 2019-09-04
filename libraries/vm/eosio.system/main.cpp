#include <array>
#include <map>

#include "eosio.system.c38f27660719f14b488b81f854c89c7f904191ee744601aea9a5180d78702e87.h"

using namespace std;

struct native_calls
{
    void (*init)();
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
    std::array<uint8_t,32> hash = std::array<uint8_t,32>{0xc3,0x8f,0x27,0x66,0x07,0x19,0xf1,0x4b,0x48,0x8b,0x81,0xf8,0x54,0xc8,0x9c,0x7f,0x90,0x41,0x91,0xee,0x74,0x46,0x01,0xae,0xa9,0xa5,0x18,0x0d,0x78,0x70,0x2e,0x87};
    call_map[hash] = native_calls {
        .init = WASM_RT_ADD_PREFIX(init),
        .apply = WASM_RT_ADD_PREFIX(Z_applyZ_vjjj)
    };
}

#define init native_init
#define apply native_apply
#undef init
#undef apply

}

extern "C" void native_init() {
    (*WASM_RT_ADD_PREFIX(init))();
}

extern "C" void init_vm_api4c();

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
