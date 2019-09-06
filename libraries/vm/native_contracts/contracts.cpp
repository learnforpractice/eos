
#include <stdint.h>
#include <map>
#include <array>

#include <wasm-rt.h>

#define WASM_RT_ADD_PREFIX(x) x


typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef float f32;
typedef double f64;

struct contract
{
    bool initialized = false;
    void (*init)();
    void (**apply)(uint64_t, uint64_t, uint64_t);
    wasm_rt_memory_t* (*get_memory)();
};
extern "C" {
std::map<std::array<uint8_t, 32>, contract> contracts_map;

extern void WASM_RT_ADD_PREFIX(init_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_127)(void);
extern void WASM_RT_ADD_PREFIX(init_eosio_token_e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb_0)(void);
extern void WASM_RT_ADD_PREFIX(init_eosio_token_3e0cf4172ab025f9fff5f1db11ee8a34d44779492e1d668ae1dc2d129e865348_180)(void);
extern void WASM_RT_ADD_PREFIX(init_eosio_5710f43eca84ffc70bd6f152aad9e10068687c9a18307c586eba68c116766623_1717927)(void);
extern void WASM_RT_ADD_PREFIX(init_eosio_254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed_186)(void);
extern void WASM_RT_ADD_PREFIX(init_eosio_047c7e09c81da6dec89ab970603696268049d1cc3ded37987ff9b8d89dedd371_1722535)(void);
extern void WASM_RT_ADD_PREFIX(init_eosio_254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed_12246)(void);
extern void WASM_RT_ADD_PREFIX(init_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_11999)(void);

extern wasm_rt_memory_t* get_memory_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_127(void);
extern wasm_rt_memory_t* get_memory_eosio_token_e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb_0(void);
extern wasm_rt_memory_t* get_memory_eosio_token_3e0cf4172ab025f9fff5f1db11ee8a34d44779492e1d668ae1dc2d129e865348_180(void);
extern wasm_rt_memory_t* get_memory_eosio_5710f43eca84ffc70bd6f152aad9e10068687c9a18307c586eba68c116766623_1717927(void);
extern wasm_rt_memory_t* get_memory_eosio_254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed_186(void);
extern wasm_rt_memory_t* get_memory_eosio_047c7e09c81da6dec89ab970603696268049d1cc3ded37987ff9b8d89dedd371_1722535(void);
extern wasm_rt_memory_t* get_memory_eosio_254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed_12246(void);
extern wasm_rt_memory_t* get_memory_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_11999(void);

extern void (*WASM_RT_ADD_PREFIX(apply_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_127))(u64, u64, u64);
extern void (*WASM_RT_ADD_PREFIX(apply_eosio_token_e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb_0))(u64, u64, u64);
extern void (*WASM_RT_ADD_PREFIX(apply_eosio_token_3e0cf4172ab025f9fff5f1db11ee8a34d44779492e1d668ae1dc2d129e865348_180))(u64, u64, u64);
extern void (*WASM_RT_ADD_PREFIX(apply_eosio_5710f43eca84ffc70bd6f152aad9e10068687c9a18307c586eba68c116766623_1717927))(u64, u64, u64);
extern void (*WASM_RT_ADD_PREFIX(apply_eosio_254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed_186))(u64, u64, u64);
extern void (*WASM_RT_ADD_PREFIX(apply_eosio_047c7e09c81da6dec89ab970603696268049d1cc3ded37987ff9b8d89dedd371_1722535))(u64, u64, u64);
extern void (*WASM_RT_ADD_PREFIX(apply_eosio_254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed_12246))(u64, u64, u64);
extern void (*WASM_RT_ADD_PREFIX(apply_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_11999))(u64, u64, u64);

extern wasm_rt_memory_t* get_memory_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_127();
extern wasm_rt_memory_t* get_memory_eosio_token_e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb_0();
extern wasm_rt_memory_t* get_memory_eosio_token_3e0cf4172ab025f9fff5f1db11ee8a34d44779492e1d668ae1dc2d129e865348_180();
extern wasm_rt_memory_t* get_memory_eosio_5710f43eca84ffc70bd6f152aad9e10068687c9a18307c586eba68c116766623_1717927();
extern wasm_rt_memory_t* get_memory_eosio_254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed_186();
extern wasm_rt_memory_t* get_memory_eosio_047c7e09c81da6dec89ab970603696268049d1cc3ded37987ff9b8d89dedd371_1722535();
extern wasm_rt_memory_t* get_memory_eosio_254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed_12246();
extern wasm_rt_memory_t* get_memory_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_11999();

void init_contracts() {

contracts_map[std::array<uint8_t, 32>{0x5e,0x8e,0x65,0x5a,0x05,0xb3,0x4e,0x78,0x24,0x67,0x68,0x4d,0x71,0x48,0x24,0x44,0x04,0xb5,0x91,0xa6,0xc1,0xec,0x26,0x87,0xea,0xcb,0x26,0x89,0x26,0xa3,0x7e,0x59,}] = contract{
    .initialized = false,
    .init = WASM_RT_ADD_PREFIX(init_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_127),
    .get_memory = get_memory_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_127,
    .apply = &apply_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_127,
 };
    

contracts_map[std::array<uint8_t, 32>{0xe5,0x36,0x99,0x6b,0xc4,0x80,0xcc,0xc3,0x48,0x6b,0x63,0x0f,0x0c,0x42,0xca,0x1e,0xf3,0xb9,0xf4,0x77,0x59,0x5e,0x88,0x5d,0xcf,0x88,0x62,0x84,0x8a,0x21,0x85,0xbb,}] = contract{
    .initialized = false,
    .init = WASM_RT_ADD_PREFIX(init_eosio_token_e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb_0),
    .get_memory = get_memory_eosio_token_e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb_0,
    .apply = &apply_eosio_token_e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb_0,
 };
    

contracts_map[std::array<uint8_t, 32>{0x3e,0x0c,0xf4,0x17,0x2a,0xb0,0x25,0xf9,0xff,0xf5,0xf1,0xdb,0x11,0xee,0x8a,0x34,0xd4,0x47,0x79,0x49,0x2e,0x1d,0x66,0x8a,0xe1,0xdc,0x2d,0x12,0x9e,0x86,0x53,0x48,}] = contract{
    .initialized = false,
    .init = WASM_RT_ADD_PREFIX(init_eosio_token_3e0cf4172ab025f9fff5f1db11ee8a34d44779492e1d668ae1dc2d129e865348_180),
    .get_memory = get_memory_eosio_token_3e0cf4172ab025f9fff5f1db11ee8a34d44779492e1d668ae1dc2d129e865348_180,
    .apply = &apply_eosio_token_3e0cf4172ab025f9fff5f1db11ee8a34d44779492e1d668ae1dc2d129e865348_180,
 };
    

contracts_map[std::array<uint8_t, 32>{0x57,0x10,0xf4,0x3e,0xca,0x84,0xff,0xc7,0x0b,0xd6,0xf1,0x52,0xaa,0xd9,0xe1,0x00,0x68,0x68,0x7c,0x9a,0x18,0x30,0x7c,0x58,0x6e,0xba,0x68,0xc1,0x16,0x76,0x66,0x23,}] = contract{
    .initialized = false,
    .init = WASM_RT_ADD_PREFIX(init_eosio_5710f43eca84ffc70bd6f152aad9e10068687c9a18307c586eba68c116766623_1717927),
    .get_memory = get_memory_eosio_5710f43eca84ffc70bd6f152aad9e10068687c9a18307c586eba68c116766623_1717927,
    .apply = &apply_eosio_5710f43eca84ffc70bd6f152aad9e10068687c9a18307c586eba68c116766623_1717927,
 };
    

contracts_map[std::array<uint8_t, 32>{0x25,0x42,0x47,0xa1,0x8a,0xf3,0x99,0xe2,0x37,0xf4,0x7e,0xa6,0x3e,0x7b,0x59,0x70,0xa8,0xf9,0xd6,0x81,0x91,0x50,0xef,0xe4,0x43,0xd8,0xe7,0x11,0x89,0x59,0x15,0xed,}] = contract{
    .initialized = false,
    .init = WASM_RT_ADD_PREFIX(init_eosio_254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed_186),
    .get_memory = get_memory_eosio_254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed_186,
    .apply = &apply_eosio_254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed_186,
 };
    

contracts_map[std::array<uint8_t, 32>{0x04,0x7c,0x7e,0x09,0xc8,0x1d,0xa6,0xde,0xc8,0x9a,0xb9,0x70,0x60,0x36,0x96,0x26,0x80,0x49,0xd1,0xcc,0x3d,0xed,0x37,0x98,0x7f,0xf9,0xb8,0xd8,0x9d,0xed,0xd3,0x71,}] = contract{
    .initialized = false,
    .init = WASM_RT_ADD_PREFIX(init_eosio_047c7e09c81da6dec89ab970603696268049d1cc3ded37987ff9b8d89dedd371_1722535),
    .get_memory = get_memory_eosio_047c7e09c81da6dec89ab970603696268049d1cc3ded37987ff9b8d89dedd371_1722535,
    .apply = &apply_eosio_047c7e09c81da6dec89ab970603696268049d1cc3ded37987ff9b8d89dedd371_1722535,
 };
    

contracts_map[std::array<uint8_t, 32>{0x25,0x42,0x47,0xa1,0x8a,0xf3,0x99,0xe2,0x37,0xf4,0x7e,0xa6,0x3e,0x7b,0x59,0x70,0xa8,0xf9,0xd6,0x81,0x91,0x50,0xef,0xe4,0x43,0xd8,0xe7,0x11,0x89,0x59,0x15,0xed,}] = contract{
    .initialized = false,
    .init = WASM_RT_ADD_PREFIX(init_eosio_254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed_12246),
    .get_memory = get_memory_eosio_254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed_12246,
    .apply = &apply_eosio_254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed_12246,
 };
    

contracts_map[std::array<uint8_t, 32>{0x5e,0x8e,0x65,0x5a,0x05,0xb3,0x4e,0x78,0x24,0x67,0x68,0x4d,0x71,0x48,0x24,0x44,0x04,0xb5,0x91,0xa6,0xc1,0xec,0x26,0x87,0xea,0xcb,0x26,0x89,0x26,0xa3,0x7e,0x59,}] = contract{
    .initialized = false,
    .init = WASM_RT_ADD_PREFIX(init_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_11999),
    .get_memory = get_memory_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_11999,
    .apply = &apply_eosio_5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59_11999,
 };
    
}


void *get_apply_entry(std::array<uint8_t, 32> hash) {
    auto itr = contracts_map.find(hash);
    if (itr == contracts_map.end()) {
        return nullptr;
    }
    if (!itr->second.initialized) {
        itr->second.init();
        itr->second.initialized = true;
    }
    return (void *)(**itr->second.apply);
}

wasm_rt_memory_t *get_contract_memory(std::array<uint8_t, 32> hash) {
    auto itr = contracts_map.find(hash);
    if (itr == contracts_map.end()) {
        return nullptr;
    }
    if (!itr->second.initialized) {
        itr->second.init();
        itr->second.initialized = true;
    }
    return itr->second.get_memory();
}

}
