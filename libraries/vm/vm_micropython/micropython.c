#include "micropython.c.bin"
#include <wasm-rt-impl.h>

//libwasm2c_contracts.a
uint32_t wasm_rt_call_stack_depth = 0;
uint32_t g_saved_call_stack_depth = 0;
jmp_buf g_jmp_buf;

u32 (*Z_envZ_memsetZ_iiii)(u32, u32, u32);
u32 (*Z_envZ_memcpyZ_iiii)(u32, u32, u32);
u32 (*Z_envZ_memmoveZ_iiii)(u32, u32, u32);
void (*Z_envZ_prints_lZ_vii)(u32, u32);
void (*Z_envZ_eosio_assertZ_vii)(u32, u32);
u32 (*Z_envZ_call_vm_apiZ_iiiiii)(u32, u32, u32, u32, u32);
u64 (*Z_envZ_s2nZ_jii)(u32, u32);
u32 (*Z_envZ_n2sZ_ijii)(u64, u32, u32);
void (*Z_envZ_print_hexZ_vii)(u32, u32);

uint64_t s2n( const char *str, size_t str_size );
int n2s(uint64_t value, char *str, size_t str_size);
int call_vm_api(int function_type,  void *input, size_t input_size, void *output, size_t output_size);

#include <stdio.h>

static void print_hex(char *data, size_t size) {
  for (int i=0;i<size;i++) {
    printf("%02x", data[i]);
  }
  printf("\n");
}

void checktime(void);
void vm_checktime(void) {
  checktime();
}

void _print_hex(u32 data_offset, u32 size) {
  char *data = (char *)get_memory_ptr(data_offset);
  print_hex(data, size);
}

static u64 _s2n(u32 str_offset, u32 str_size) {
  char *str = get_memory_ptr(str_offset);
  return s2n(str, str_size);
}

static u32 _n2s(u64 n, u32 str_offset, u32 str_size) {
  char *str = get_memory_ptr(str_offset);
  u32 ret = n2s(n, str, str_size);
//  printf("+++++++++++++_n2s: %s %d\n", str, ret);
  return ret;
}

static u32 _call_vm_api(u32 function_type, u32 input_offset, u32 input_size, u32 output_offset, u32 output_size) {
  char *input = get_memory_ptr(input_offset);
  char *output = get_memory_ptr(output_offset);
  return call_vm_api(function_type, input, input_size, output, output_size);
}


//void * memset ( void * ptr, int value, size_t num );

static u32 _memset(u32 ptr_offset, u32 value, u32 num) {
  char *ptr = get_memory_ptr(ptr_offset);
  memset(ptr, value, num);
  return ptr_offset;
}

//void * memcpy ( void * destination, const void * source, size_t num );
static u32 _memcpy(u32 dest_offset, u32 src_offset, u32 num) {
  char *src_ptr = get_memory_ptr(src_offset);
  char *dest_ptr = get_memory_ptr(dest_offset);
  memcpy(dest_ptr, src_ptr, num);
  return dest_offset;
}

//void * memmove ( void * destination, const void * source, size_t num )
static u32 _memmove(u32 dest_offset, u32 src_offset, u32 num) {
  char *src_ptr = get_memory_ptr(src_offset);
  char *dest_ptr = get_memory_ptr(dest_offset);
  memmove(dest_ptr, src_ptr, num);
  return dest_offset;
}

static void _prints(u32 a, u32 b) {

}

static void _eosio_assert(u32 a, u32 b) {

}

void WASM_RT_ADD_PREFIX(init)(void) {
  Z_envZ_memsetZ_iiii = _memset;
  Z_envZ_memcpyZ_iiii = _memcpy;
  Z_envZ_memmoveZ_iiii = _memmove;
  Z_envZ_prints_lZ_vii = _prints;
  Z_envZ_eosio_assertZ_vii = _eosio_assert;
  Z_envZ_call_vm_apiZ_iiiiii = _call_vm_api;

  Z_envZ_s2nZ_jii = _s2n;
  Z_envZ_n2sZ_ijii = _n2s;

  Z_envZ_print_hexZ_vii = _print_hex;

  init_func_types();
  init_globals();
  init_memory();
  init_table();
  init_exports();
  memcpy(&(M0.data[0]), &g1, 4);
}

static void *get_memory_ptr(int offset) {
  return M0.data + offset;
}

int micropython_init() {
  init();
  mp_js_init(64*1024);
  return 1;
}

int micropython_contract_init(int type, const char *py_src, size_t size) {
  u32 offset = malloc(size);
  char *ptr = (char *)get_memory_ptr(offset);
  memcpy(ptr, py_src, size);
  return micropython_init_module(type, offset, size);
}

int micropython_contract_apply(uint64_t receiver, uint64_t code, uint64_t action) {
  u32 ptr_offset = malloc(1);
//  printf("+++++++++free_memory start pos: %d\n", ptr_offset);
  wasm_rt_call_stack_depth = 0;
  int trap_code = wasm_rt_impl_try();
  if (trap_code == 0) {
    return micropython_apply(receiver, code, action);
  } else {
//    printf("++++trap code: %d\n", trap_code);
  }
  return 0;
}

void *micropython_get_memory() {
  return M0.data;
}

size_t micropython_get_memory_size() {
  return M0.size;
}

size_t micropython_backup_memory(void *backup, size_t size) {
  size_t copy_size = 0;
  if (size > M0.size) {
    copy_size = M0.size;
  } else {
    copy_size = size;
  }
  memcpy(backup, M0.data, copy_size);
  return copy_size;
}

size_t micropython_restore_memory(void *backup, size_t size) {
  size_t copy_size = 0;
  if (size > M0.size) {
    copy_size = M0.size;
  } else {
    copy_size = size;
  }

  memcpy(M0.data, backup, copy_size);
  return copy_size;
}
