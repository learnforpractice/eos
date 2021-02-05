#include <algorithm>
#include "uuos.hpp"

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

u64 _s2n(u32 in_offset, u32 in_len) {
    char *in = (char *)offset_to_ptr(in_offset, in_len);
    string s(in, in_len);
    return get_uuos_proxy()->s2n(s);
}

u32 _n2s(u64 n, u32 out_offset, u32 length) {
    char *out = (char *)offset_to_ptr(out_offset, length);
    string s = get_uuos_proxy()->n2s(n);
    int copy_len = std::min(length, (u32)s.size());
    ::memcpy(out, s.c_str(), copy_len);
    return copy_len;
}

/* import: 'env' 'prints' */
void _prints(u32 a) {
    const char * str = (const char *)offset_to_char_ptr(a);
    apply_context_proxy *proxy = get_uuos_proxy()->get_apply_context_proxy();
    if (proxy->get_context()) {
        if ( proxy()->contracts_console() ) {
            proxy->get_vm_api()->prints(str);
        }
    } else {
        printf("%s", (const char*)str);
    }
}

/* import: 'env' 'printi' */
extern void _printi(u64 a) {
    if (get_apply_context_proxy()->get_context()) {
        if ( get_apply_context_proxy()->contracts_console() ) {
            get_vm_api_proxy()->printi((int64_t)a);
        }
    } else {
        printf("%ld", a);
    }
    return;
}

}