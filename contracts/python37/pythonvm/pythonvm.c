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
#if 1

#if 0
char* setlocale (int category, const char* locale) {
    prints(__FUNCTION__);prints("\n");
    return NULL;
}

//char *nl_langinfo_l(nl_item item, locale_t locale) {
char *__nl_langinfo_l(int item, int locale) {
    return NULL;
}

//char *nl_langinfo(nl_item item) {
char *nl_langinfo(int item) {
    prints(__FUNCTION__);prints("\n");
    return NULL;
}

//const char *__lctrans(const char *, const struct __locale_map *);
const char *__lctrans(const char * a, int b) {
    prints(__FUNCTION__);prints("\n");
    return NULL;
}
#endif

//int clock_gettime(clockid_t clk_id, struct timespec *tp) {
#if 0
int clock_gettime (clockid_t a, struct timespec * b) {
    return 0;
    if (!initialized) {
        return 0;
    }
    uint64_t time = current_time();
    b->tv_sec = time/1000000;
    b->tv_nsec = time%1000000*1000;
//        prints(__FUNCTION__);prints("\n");
    return 0;
}
#endif

void exit(int a) {
    prints(__FUNCTION__);prints("\n");
}

void flockfile(FILE *filehandle) {
    prints(__FUNCTION__);prints("\n");
    return;
}

void funlockfile(FILE *filehandle) {
    prints(__FUNCTION__);prints("\n");
}


FILE * fopen ( const char * filename, const char * mode ) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}

FILE *fdopen(int fd, const char *mode) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}

int ioctl(int a, int b, int c) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}

#if 0
int localeconv() {
    prints(__FUNCTION__);prints("\n");
    return 0;
}
#endif

int sigaction(int a, int b, int c) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}
int sigemptyset(int a) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}

int sigfillset(void *set) {
    return 0;
}

/*
int getrlimit(int resource, struct rlimit *rlim);
int setrlimit(int resource, const struct rlimit *rlim);
*/
int getrlimit(int resource, void *rlim) {
    eosio_assert(0, "getrlimit not implemented");
    return 0;
}

int setrlimit(int resource, void *rlim) {
    eosio_assert(0, "setrlimit not implemented");
    return 0;
}

#if 0
double strtod (const char* str, char** endptr) {
    prints(__FUNCTION__);prints("\n");
    return 0.0;
}
#endif

void printi( int64_t value );

#if 0
int __syscall_ret (int i32) {
    return 0;
}

void __lock(volatile int *l) {
    return;
}

void __unlock(volatile int *l) {

}
#endif



int __syscall(int a) {
prints(__FUNCTION__);prints("\n");
    return 0;
}

int __syscall1(int a, int b) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}

void wasm_syscall(void);
int __syscall3 (int which, ...) {
    prints(__FUNCTION__);prints(" ");printi(which);prints("\n");
//    wasm_syscall();
    return 0;
}

int __syscall5(int a, int b) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}

int __syscall_ret(int a) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}

#if 0
int __unlock() {
    prints(__FUNCTION__);prints("\n");
    return 0;
}

void __lock(int a) {
    prints(__FUNCTION__);prints("\n");    
}
#endif

void __block_all_sigs(void* n) {
    prints(__FUNCTION__);prints("\n");
}

#if 1

int emscripten_asm_const_int (int a, int b, int c) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}
int pthread_self(void) {
//    prints(__FUNCTION__);prints("\n");
    return 0;
}

int raise(int a) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}
#endif

#endif

#if 0
int fprintf(FILE *restrict f, const char *restrict fmt, ...) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}
#endif

int _fstat(int fd, struct stat *statbuf);
int fstat(int fd, struct stat *statbuf) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}
#define off_t int
int ftruncate(int fd, off_t length) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}

int _open(const char *pathname, int flags, int mode);
//int open(const char *pathname, int flags, mode_t mode) {
int open(const char *pathname, int flags, int mode) {
    return _open(pathname, flags, mode);
}

off_t _lseek(int fd, off_t offset, int whence);
off_t lseek(int fd, off_t offset, int whence) {
    return _lseek(fd, offset, whence);
}

ssize_t _write(int fd, const void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count) {
    return _write(fd, buf, count);
}

ssize_t _read(int fildes, void *buf, size_t nbyte);
ssize_t read(int fildes, void *buf, size_t nbyte) {
    return _read(fildes, buf, nbyte);
}

int _close(int fd);
int close(int fd) {
    return _close(fd);
}

int fcntl(int fd, int cmd, ... /* arg */ ) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}

int isatty(int fd) {
    prints(__FUNCTION__);printi(fd);prints("\n");
    if (fd == 11 || fd == 22 || fd == 33) {
        return 1;//file descriptor referring to a terminal
    }
    return 0;
}

int pthread_cond_destroy(int a) {
    prints(__FUNCTION__);prints("\n");
    return 0;
}

int pthread_cond_init(int a, int b) {
//    prints(__FUNCTION__);prints("\n");
    return 0;
}

int pthread_cond_signal(int a) {
//    prints(__FUNCTION__);prints("\n");
    return 0;
}

int pthread_cond_timedwait(int a, int b, int c) {
    return 0;
}

int pthread_cond_wait(int a, int b) {
    return 0;
}
void pthread_exit(int a) {

}
int pthread_getspecific(int a) {
    return 0;
}
int pthread_key_create(int a, int b) {
    return 0;
}
int pthread_key_delete(int a) {
    return 0;
}
int pthread_mutex_destroy(int a) {
    return 0;
}
int pthread_mutex_init(int a, int b) {
    return 0;
}
int pthread_mutex_lock(int a) {
    return 0;
}
int pthread_mutex_trylock(int a) {
    return 0;
}
int pthread_mutex_unlock(int a) {
    return 0;
}
int pthread_setspecific(int a, int b) {
    return 0;
}

/*
int pthread_attr_init(pthread_attr_t *);
int pthread_attr_setstacksize(pthread_attr_t *, size_t);
int pthread_mutexattr_destroy(pthread_mutexattr_t *);
int pthread_create(pthread_t *__restrict, const pthread_attr_t *__restrict, void *(*)(void *), void *__restrict);
int pthread_detach(pthread_t);
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset);
int pthread_attr_destroy(pthread_attr_t *);
*/

int pthread_attr_destroy(void *a) {
    return 0;
}

int pthread_attr_init(void *a) {
    return 0;
}
int pthread_attr_setstacksize(void * a, size_t b) {
    return 0;
}
int pthread_mutexattr_destroy(void * a) {
    return 0;
}
int pthread_create(void *a, const void *b, void *(*c)(void *), void *d) {
    return 0;
}
int pthread_detach(void *a) {
    return 0;
}

int pthread_sigmask(int how, const void *set, void *oldset) {
    return 0;
}

int gettimeofday(struct timeval *restrict tv, void *restrict tz) {
    eosio_assert(0, "not implemented");
    return 0;
}

int confstr(int a, int b, int c) {
    return 0;
}
int dup(int a) {
    return 0;
}
int getcwd(int a, int b) {
    return 0;
}
int getrusage(int a, int b) {
    return 0;
}
int readlink(int a, int b, int c) {
    return 0;
}
int select(int a, int b, int c, int d, int e) {
    return 0;
}
int siginterrupt(int a, int b) {
    return 0;
}
int signal(int a, int b) {
    return 0;
}
int stat(int a, int b) {
    return 0;
}
int syscall(int a, int b, int c) {
    return 0;
}
int sysconf(int a) {
    return 0;
}

int __mmap(int a, int b, int c, int d, int e, int f) {
    prints(__FUNCTION__);prints("\n");
    eosio_assert(0, "not implemented");
    return 0;
}

int __munmap(int a, int b) {
    prints(__FUNCTION__);prints("\n");
    eosio_assert(0, "not implemented");
    return 0;
}

int __sys_open(int a, int b) {
    return 0;
}

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

int eosio_token_apply( uint64_t receiver, uint64_t code, uint64_t action );
void python_init(void);
void call( uint64_t func_name, uint64_t receiver, uint64_t code, uint64_t action );

void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
    prints("++++++g_counter:");printi(g_counter++);prints("\n");
    python_init();
    return;
//    prints("+++current_module:");printi(current_module);prints("\n");
#if 0
    if (eosio_token_apply(receiver, code, action)) {
        return;
    }
#endif
    python_call_module(current_module, receiver, code, action);

    if (!current_module) {
        return;
    }
    if (receiver == 0) {
        call(0, receiver, code, action);
    } else {
        python_call_module(current_module, receiver, code, action);
    }
//    prints("++++++++++++get_current_memory ");printi(get_current_memory());prints("\n");
}

void check_error(void);

void call( uint64_t func_name, uint64_t receiver, uint64_t code, uint64_t action ) {
    if (action == 0) {
        g_counter += 1;
        python_init();
        initialized = 1;
    }
    else if (1 == action) {
        int start, end;
        start = get_current_memory();
//        printf("+++++++++++++++get_current_memory: %d\n", get_current_memory());
        int code_size = get_code_size(receiver);
        char *str_code = (char *)get_code_memory();//malloc(code_size);
//        prints("++++++++++++code buffer ");printi(str_code);prints("\n");
        int size = get_code(receiver, str_code, code_size);
        current_module = python_load_module(str_code, code_size);
//        printf("+++++++++++++++get_current_memory: %d\n", get_current_memory());
        memset(str_code, 0, code_size);
        end = get_current_memory();
        set_copy_memory_range(start, end);
        check_error();
    }
}

//EOSIO_ABI( eosio::token, (create)(issue)(transfer) )


#include <stdlib.h>

void python_call_module(void *m, uint64_t receiver, uint64_t code, uint64_t action);
void sayhello(void) {
    python_call_module(0, 0, 0, 0);
    printf("hello,world");
}

