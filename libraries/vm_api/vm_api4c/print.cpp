#include "vm_api4c.hpp"
#include <vm_api_proxy.hpp>

void vm_api4c_proxy::printi(u64 v) {
    get_vm_api()->printi(v);
}

void vm_api4c_proxy::prints(u32 s_offset) {
    char *s = (char *)offset_to_char_ptr(s_offset);
    get_vm_api()->prints(s);
}

void vm_api4c_proxy::printn(u64 n) {
    get_vm_api()->printn(n);
}

void vm_api4c_proxy::printqf(u32 offset) {
    long double *qf = (long double *)offset_to_ptr(offset, sizeof(long double));
    get_vm_api()->printqf(qf);
}

void vm_api4c_proxy::prints_l(u32 s_offset, u32 size) {
    char *s = (char *)offset_to_ptr(s_offset, size);
    get_vm_api()->prints_l(s, size);
}

void vm_api4c_proxy::printui(u64 u) {
    get_vm_api()->printui(u);
}

void vm_api4c_proxy::printhex(u32 data_offset, u32 data_len) {
    uint8_t *data = (uint8_t *)offset_to_ptr(data_offset, data_len);
    get_vm_api()->printhex(data, data_len);
}

