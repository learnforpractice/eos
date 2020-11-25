//prints defined in chain_api.cpp
extern "C" {
/* import: 'env' 'prints_l' */
void (*Z_envZ_prints_lZ_vii)(u32, u32);
/* import: 'env' 'prints' */
void (*Z_envZ_printsZ_vi)(u32);
/* import: 'env' 'printi' */
void (*Z_envZ_printiZ_vj)(u64);

void (*Z_envZ_printnZ_vj)(u64);
void (*Z_envZ_printuiZ_vj)(u64);

/* import: 'env' 'printhex' */
void (*Z_envZ_printhexZ_vii)(u32, u32);
void (*Z_envZ_printqfZ_vi)(u32);
}



/*
static void _printi(u64 v) {
    get_vm_api()->printi(v);
}

static void _prints(u32 s_offset) {
    char *s = (char *)offset_to_char_ptr(s_offset);
    get_vm_api()->prints(s);
}
*/

static void _printn(u64 n) {
    get_vm_api()->printn(n);
}

static void _printqf(u32 offset) {
    float128_t *qf = (float128_t *)offset_to_ptr(offset, sizeof(float128_t));
    get_vm_api()->printqf(qf);
}

static void _prints_l(u32 s_offset, u32 size) {
    char *s = (char *)offset_to_ptr(s_offset, size);
    if (get_vm_api()->is_in_apply_context) {
        get_vm_api()->prints_l(s, size);
    } else {
        vmelog("%s", s);
    }
}

static void _printui(u64 u) {
    get_vm_api()->printui(u);
}

/* import: 'env' 'prints' */
void _prints(u32 a);
void _printi(u64 a);

void _printhex(u32 data_offset, u32 data_len) {
    uint8_t *data = (uint8_t *)offset_to_ptr(data_offset, data_len);
    get_vm_api()->printhex(data, data_len);
}

void init_print() {
    Z_envZ_printhexZ_vii = _printhex;

    Z_envZ_printiZ_vj = _printi;
    Z_envZ_printsZ_vi = _prints;
    Z_envZ_printqfZ_vi = _printqf;
}
