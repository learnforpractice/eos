#include <chain_api.hpp>
#include <vm_api/vm_api.h>
#include <algorithm>

extern "C" {

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

void *offset_to_ptr(u32 offset, u32 size);
void *offset_to_char_ptr(u32 offset);

u32 _get_code_size(u64 account) {
    size_t size = 0;
    get_chain_api()->get_code_ex( account, &size );
    return size;
}

u32 _get_code(u64 account, u32 memory_offset, u32 buffer_size) {
    char *memory = (char *)offset_to_ptr(memory_offset, buffer_size);

    size_t size = 0;
    const char *code = get_chain_api()->get_code_ex( account, &size );
    if (size <= buffer_size) {
        memcpy(memory, code, size);
        return size;
    } else {
        memcpy(memory, code, buffer_size);
        return buffer_size;
    }
}

u64 _s2n(u32 in_offset, u32 in_len) {
    char *in = (char *)offset_to_ptr(in_offset, in_len);
    string s(in, in_len);
    return get_chain_api()->str2n(s);
}

u32 _n2s(u64 n, u32 out_offset, u32 length) {
    string s;
    char *out = (char *)offset_to_ptr(out_offset, length);
    get_chain_api()->n2str(n, s);
    int copy_len = std::min(length, (u32)s.size());
    ::memcpy(out, s.c_str(), copy_len);
    return copy_len;
}

/* import: 'env' 'prints' */
void _prints(u32 a) {
    const char * str = (const char *)offset_to_char_ptr(a);
    if (get_vm_api()->is_in_apply_context) {
        if ( get_chain_api()->contracts_console() ) {
            get_vm_api()->prints(str);
        }
    } else {
        vmdlog("%s", (const char*)str);
    }
}

/* import: 'env' 'printi' */
extern void _printi(u64 a) {
    if (get_vm_api()->is_in_apply_context) {
        if ( get_chain_api()->contracts_console() ) {
            get_vm_api()->printi((int64_t)a);
        }
    } else {
        vmdlog("%ld", a);
    }
    return;
}

}