//static_assert(sizeof(unsigned long) == sizeof(unsigned int), "wrong size!" );

typedef __int128 int128_t ;
typedef unsigned __int128 uint128_t;



#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

//#include <Python.h>

__attribute__((eosio_wasm_import))
void prints( const char* cstr );

__attribute__((eosio_wasm_import))
void eosio_assert(int cond, const char *msg);

__attribute__((eosio_wasm_import))
uint64_t current_time(void);

__attribute__((eosio_wasm_import))
uint64_t s2n(const char *str, int len);

static int initialized = 0;

extern int Py_IgnoreEnvironmentFlag;
extern void Py_InitializeEx(int);
extern void Py_InitializeEx2(int install_sigs, int install_importlib);
extern int PyRun_SimpleString(const char *s);

//extern int Py_IgnoreEnvironmentFlag;

#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

void log_(const char *fmt, ...) {
//void log_(const char *output, int level, int line, const char *file, const char *func) {
   char output[100];
   va_list args;
   va_start(args, fmt);
   int len = vsnprintf(output, sizeof output, fmt, args);
   va_end(args);
   prints(output);
}

char *realpath (const char *__restrict a, char *__restrict b) {
    return 0;
}

void _exit(int a){return;}

typedef void (*fnsayHello)();
void inspect_memory( const char* dest, size_t length );
static char g_n = 0;



extern int Py_FrozenMain(int, char **);

#if 0
extern unsigned char M___main__[];
extern unsigned char M___future__[];
extern unsigned char M__frozen_importlib[];
extern unsigned char M__frozen_importlib_external[];

extern unsigned char M_encodings__latin_1[];
extern unsigned char M_encodings__utf_8[];
#endif

void wasm_print_time(void);
static int g_counter = 0;
static void *current_module;
extern void python_init(void);
extern void *python_load_module(const char *code, int size);
extern void python_call_module(void *m, uint64_t receiver, uint64_t code, uint64_t action);

void *get_current_memory(void);
void *get_code_memory(void);

__attribute__((eosio_wasm_import))
void set_copy_memory_range(int start, int end);

__attribute__((eosio_wasm_import))
int get_code_size(uint64_t account);

__attribute__((eosio_wasm_import))
int get_code(uint64_t account, char *code, size_t size);


extern void python_vm_apply( uint64_t receiver, uint64_t code, uint64_t action );
extern void python_vm_call( uint64_t func_name, uint64_t receiver, uint64_t code, uint64_t action );
extern void python_init(void);
extern void *get_current_memory(void);

int fool_compiler = 0;
void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
    python_vm_apply(receiver, code, action);
    if (fool_compiler) {
        python_vm_call(0, 0, 0, 0);
        python_init();
        get_current_memory();
    }
}



