#pragma once

#include <stdint.h>
#include <map>
#include <array>

#include <wasm-rt.h>

#define PAGE_SIZE 65536U
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
