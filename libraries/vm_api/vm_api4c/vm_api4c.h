#include <stdint.h>

#ifndef __VM_API4C_H
#define __VM_API4C_H

#ifdef __cpulsplus
extern "C" {
#endif

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


typedef void* (*fn_offset_to_char_ptr)(u32 offset);
typedef void* (*fn_offset_to_ptr)(u32 offset, u32 size);

void init_vm_api4c();

void *offset_to_ptr(u32 offset, u32 size);
void *offset_to_char_ptr(u32 offset);

void wasm_rt_on_trap(int code);

#ifdef __cpulsplus
}
#endif

#endif
