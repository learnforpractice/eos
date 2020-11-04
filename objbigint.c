# 1 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 323 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c" 2
# 29 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c"
# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/string.h" 1







# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/features.h" 1
# 9 "/Users/newworld/dev/uuos3/contracts/include/libc/string.h" 2
# 23 "/Users/newworld/dev/uuos3/contracts/include/libc/string.h"
# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/bits/alltypes.h" 1


# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/bits/wchar.h" 1


# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/bits/stdint.h" 1


typedef unsigned __int128 uint128_t;

typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef __int128 int128_t;

typedef long long int64_t;
typedef int int32_t;
typedef short int16_t;
typedef char int8_t;

typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;
typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;

typedef int8_t int_fast8_t;
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef int64_t int_fast64_t;
typedef uint8_t uint_fast8_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;





typedef int32_t intptr_t;


typedef uint32_t uintptr_t;

typedef long long int intmax_t;
typedef long long unsigned int uintmax_t;
# 4 "/Users/newworld/dev/uuos3/contracts/include/libc/bits/wchar.h" 2
# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/bits/stddef.h" 1






   typedef unsigned int size_t;
   typedef int ptrdiff_t;
# 5 "/Users/newworld/dev/uuos3/contracts/include/libc/bits/wchar.h" 2


typedef uint32_t wchar_t;



typedef wchar_t wint_t;
# 4 "/Users/newworld/dev/uuos3/contracts/include/libc/bits/alltypes.h" 2
# 14 "/Users/newworld/dev/uuos3/contracts/include/libc/bits/alltypes.h"
typedef size_t ssize_t;

typedef __builtin_va_list va_list;
typedef __builtin_va_list __isoc_va_list;

typedef unsigned long wctype_t;

typedef struct __mbstate_t { unsigned __opaque1, __opaque2; } mbstate_t;

typedef int regoff_t;

struct __locale_struct;

typedef struct __locale_struct * locale_t;

typedef struct _IO_FILE FILE;

typedef int64_t off_t;

typedef unsigned long wctype_t;

typedef long time_t;
typedef long suseconds_t;

struct timeval { time_t tv_sec; suseconds_t tv_usec; };
struct timespec { time_t tv_sec; long tv_nsec; };

typedef float float_t;
typedef double double_t;
# 24 "/Users/newworld/dev/uuos3/contracts/include/libc/string.h" 2

void *memcpy (void *restrict, const void *restrict, size_t);
void *memmove (void *, const void *, size_t);
void *memset (void *, int, size_t);
int memcmp (const void *, const void *, size_t);
void *memchr (const void *, int, size_t);

char *strcpy (char *restrict, const char *restrict);
char *strncpy (char *restrict, const char *restrict, size_t);

char *strcat (char *restrict, const char *restrict);
char *strncat (char *restrict, const char *restrict, size_t);

int strcmp (const char *, const char *);
int strncmp (const char *, const char *, size_t);

int strcoll (const char *, const char *);
size_t strxfrm (char *restrict, const char *restrict, size_t);

char *strchr (const char *, int);
char *strrchr (const char *, int);

size_t strcspn (const char *, const char *);
size_t strspn (const char *, const char *);
char *strpbrk (const char *, const char *);
char *strstr (const char *, const char *);
char *strtok (char *restrict, const char *restrict);

size_t strlen (const char *);

char *strerror (int);



# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/strings.h" 1
# 16 "/Users/newworld/dev/uuos3/contracts/include/libc/strings.h"
int bcmp (const void *, const void *, size_t);
void bcopy (const void *, void *, size_t);
void bzero (void *, size_t);
char *index (const char *, int);
char *rindex (const char *, int);



int ffs (int);
int ffsl (long);
int ffsll (long long);


int strcasecmp (const char *, const char *);
int strncasecmp (const char *, const char *, size_t);

int strcasecmp_l (const char *, const char *, locale_t);
int strncasecmp_l (const char *, const char *, size_t, locale_t);
# 58 "/Users/newworld/dev/uuos3/contracts/include/libc/string.h" 2





char *strtok_r (char *restrict, const char *restrict, char **restrict);
int strerror_r (int, char *, size_t);
char *stpcpy(char *restrict, const char *restrict);
char *stpncpy(char *restrict, const char *restrict, size_t);
size_t strnlen (const char *, size_t);
char *strdup (const char *);
char *strndup (const char *, size_t);
char *strsignal(int);
char *strerror_l (int, locale_t);
int strcoll_l (const char *, const char *, locale_t);
size_t strxfrm_l (char *restrict, const char *restrict, size_t, locale_t);




void *memccpy (void *restrict, const void *restrict, int, size_t);



char *strsep(char **, const char *);
size_t strlcat (char *, const char *, size_t);
size_t strlcpy (char *, const char *, size_t);
# 30 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c" 2
# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/stdio.h" 1
# 50 "/Users/newworld/dev/uuos3/contracts/include/libc/stdio.h"
typedef union _G_fpos64_t {
 char __opaque[16];
 double __align;
} fpos_t;

extern FILE *const stdin;
extern FILE *const stdout;
extern FILE *const stderr;





int feof(FILE *);
int ferror(FILE *);
int fflush(FILE *);
void clearerr(FILE *);

int fseek(FILE *, long, int);
long ftell(FILE *);
void rewind(FILE *);

int fgetpos(FILE *restrict, fpos_t *restrict);
int fsetpos(FILE *, const fpos_t *);

size_t fread(void *restrict, size_t, size_t, FILE *restrict);
size_t fwrite(const void *restrict, size_t, size_t, FILE *restrict);

int fgetc(FILE *);
int getc(FILE *);
int getchar(void);
int ungetc(int, FILE *);

int fputc(int, FILE *);
int putc(int, FILE *);
int putchar(int);

char *fgets(char *restrict, int, FILE *restrict);




int fputs(const char *restrict, FILE *restrict);
int puts(const char *);

int printf(const char *restrict, ...);
int fprintf(FILE *restrict, const char *restrict, ...);
int sprintf(char *restrict, const char *restrict, ...);
int snprintf(char *restrict, size_t, const char *restrict, ...);

int vprintf(const char *restrict, __isoc_va_list);
int vfprintf(FILE *restrict, const char *restrict, __isoc_va_list);
int vsprintf(char *restrict, const char *restrict, __isoc_va_list);
int vsnprintf(char *restrict, size_t, const char *restrict, __isoc_va_list);

int scanf(const char *restrict, ...);
int fscanf(FILE *restrict, const char *restrict, ...);
int sscanf(const char *restrict, const char *restrict, ...);
int vscanf(const char *restrict, __isoc_va_list);
int vfscanf(FILE *restrict, const char *restrict, __isoc_va_list);
int vsscanf(const char *restrict, const char *restrict, __isoc_va_list);

void perror(const char *);

int setvbuf(FILE *restrict, char *restrict, int, size_t);
void setbuf(FILE *restrict, char *restrict);




FILE *fmemopen(void *restrict, size_t, const char *restrict);
FILE *open_memstream(char **, size_t *);
int fileno(FILE *);
int fseeko(FILE *, off_t, int);
off_t ftello(FILE *);
int dprintf(int, const char *restrict, ...);
int vdprintf(int, const char *restrict, __isoc_va_list);
int getc_unlocked(FILE *);
int getchar_unlocked(void);
int putc_unlocked(int, FILE *);
int putchar_unlocked(int);
ssize_t getdelim(char **restrict, size_t *restrict, int, FILE *restrict);
ssize_t getline(char **restrict, size_t *restrict, FILE *restrict);



void setlinebuf(FILE *);
void setbuffer(FILE *, char *, size_t);
int fgetc_unlocked(FILE *);
int fputc_unlocked(int, FILE *);
int fflush_unlocked(FILE *);
size_t fread_unlocked(void *, size_t, size_t, FILE *);
size_t fwrite_unlocked(const void *, size_t, size_t, FILE *);
void clearerr_unlocked(FILE *);
int feof_unlocked(FILE *);
int ferror_unlocked(FILE *);
int fileno_unlocked(FILE *);
int getw(FILE *);
int putw(int, FILE *);
char *fgetln(FILE *, size_t *);
int asprintf(char **, const char *, ...);
int vasprintf(char **, const char *, __isoc_va_list);
# 31 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c" 2
# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/assert.h" 1
# 19 "/Users/newworld/dev/uuos3/contracts/include/libc/assert.h"
_Noreturn void __assert_fail (const char *, const char *, int, const char *);
# 32 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c" 2

# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/parsenumbase.h" 1
# 29 "/Users/newworld/dev/uuos3/externals/micropython/py/parsenumbase.h"
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/mpconfig.h" 1
# 62 "/Users/newworld/dev/uuos3/externals/micropython/py/mpconfig.h"
# 1 "/Users/newworld/dev/uuos3/externals/micropython/ports/uuosio/mpconfigport.h" 1
# 27 "/Users/newworld/dev/uuos3/externals/micropython/ports/uuosio/mpconfigport.h"
# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/stdint.h" 1
# 22 "/Users/newworld/dev/uuos3/contracts/include/libc/stdint.h"
typedef int8_t int_fast8_t;
typedef int64_t int_fast64_t;

typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;

typedef uint8_t uint_fast8_t;
typedef uint64_t uint_fast64_t;

typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;
# 28 "/Users/newworld/dev/uuos3/externals/micropython/ports/uuosio/mpconfigport.h" 2
# 136 "/Users/newworld/dev/uuos3/externals/micropython/ports/uuosio/mpconfigport.h"
extern const struct _mp_obj_module_t mp_module_utime;
# 186 "/Users/newworld/dev/uuos3/externals/micropython/ports/uuosio/mpconfigport.h"
typedef int mp_int_t;
typedef unsigned mp_uint_t;
typedef long mp_off_t;
# 197 "/Users/newworld/dev/uuos3/externals/micropython/ports/uuosio/mpconfigport.h"
# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/alloca.h" 1
# 11 "/Users/newworld/dev/uuos3/contracts/include/libc/alloca.h"
void *alloca(size_t);
# 198 "/Users/newworld/dev/uuos3/externals/micropython/ports/uuosio/mpconfigport.h" 2
# 63 "/Users/newworld/dev/uuos3/externals/micropython/py/mpconfig.h" 2
# 699 "/Users/newworld/dev/uuos3/externals/micropython/py/mpconfig.h"
typedef double mp_float_t;
# 30 "/Users/newworld/dev/uuos3/externals/micropython/py/parsenumbase.h" 2

size_t mp_parse_num_base(const char *str, size_t len, int *base);
# 34 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c" 2
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/smallint.h" 1
# 30 "/Users/newworld/dev/uuos3/externals/micropython/py/smallint.h"
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/misc.h" 1
# 33 "/Users/newworld/dev/uuos3/externals/micropython/py/misc.h"
# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/stdbool.h" 1
# 34 "/Users/newworld/dev/uuos3/externals/micropython/py/misc.h" 2

# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/stddef.h" 1
# 36 "/Users/newworld/dev/uuos3/externals/micropython/py/misc.h" 2

typedef unsigned char byte;
typedef unsigned int uint;
# 87 "/Users/newworld/dev/uuos3/externals/micropython/py/misc.h"
void *m_malloc(size_t num_bytes);
void *m_malloc_maybe(size_t num_bytes);
void *m_malloc_with_finaliser(size_t num_bytes);
void *m_malloc0(size_t num_bytes);





void *m_realloc(void *ptr, size_t new_num_bytes);
void *m_realloc_maybe(void *ptr, size_t new_num_bytes, _Bool allow_move);
void m_free(void *ptr);

__attribute__((noreturn)) void m_malloc_fail(size_t num_bytes);
# 120 "/Users/newworld/dev/uuos3/externals/micropython/py/misc.h"
typedef uint32_t unichar;







unichar utf8_get_char(const byte *s);
const byte *utf8_next_char(const byte *s);
size_t utf8_charlen(const byte *str, size_t len);
# 144 "/Users/newworld/dev/uuos3/externals/micropython/py/misc.h"
_Bool unichar_isspace(unichar c);
_Bool unichar_isalpha(unichar c);
_Bool unichar_isprint(unichar c);
_Bool unichar_isdigit(unichar c);
_Bool unichar_isxdigit(unichar c);
_Bool unichar_isident(unichar c);
_Bool unichar_isalnum(unichar c);
_Bool unichar_isupper(unichar c);
_Bool unichar_islower(unichar c);
unichar unichar_tolower(unichar c);
unichar unichar_toupper(unichar c);
mp_uint_t unichar_xdigit_value(unichar c);





typedef struct _vstr_t {
    size_t alloc;
    size_t len;
    char *buf;
    _Bool fixed_buf : 1;
} vstr_t;




void vstr_init(vstr_t *vstr, size_t alloc);
void vstr_init_len(vstr_t *vstr, size_t len);
void vstr_init_fixed_buf(vstr_t *vstr, size_t alloc, char *buf);
struct _mp_print_t;
void vstr_init_print(vstr_t *vstr, size_t alloc, struct _mp_print_t *print);
void vstr_clear(vstr_t *vstr);
vstr_t *vstr_new(size_t alloc);
void vstr_free(vstr_t *vstr);
static inline void vstr_reset(vstr_t *vstr) {
    vstr->len = 0;
}
static inline char *vstr_str(vstr_t *vstr) {
    return vstr->buf;
}
static inline size_t vstr_len(vstr_t *vstr) {
    return vstr->len;
}
void vstr_hint_size(vstr_t *vstr, size_t size);
char *vstr_extend(vstr_t *vstr, size_t size);
char *vstr_add_len(vstr_t *vstr, size_t len);
char *vstr_null_terminated_str(vstr_t *vstr);
void vstr_add_byte(vstr_t *vstr, byte v);
void vstr_add_char(vstr_t *vstr, unichar chr);
void vstr_add_str(vstr_t *vstr, const char *str);
void vstr_add_strn(vstr_t *vstr, const char *str, size_t len);
void vstr_ins_byte(vstr_t *vstr, size_t byte_pos, byte b);
void vstr_ins_char(vstr_t *vstr, size_t char_pos, unichar chr);
void vstr_cut_head_bytes(vstr_t *vstr, size_t bytes_to_cut);
void vstr_cut_tail_bytes(vstr_t *vstr, size_t bytes_to_cut);
void vstr_cut_out_bytes(vstr_t *vstr, size_t byte_pos, size_t bytes_to_cut);
void vstr_printf(vstr_t *vstr, const char *fmt, ...);
# 220 "/Users/newworld/dev/uuos3/externals/micropython/py/misc.h"
int DEBUG_printf(const char *fmt, ...);

extern mp_uint_t mp_verbose_flag;
# 231 "/Users/newworld/dev/uuos3/externals/micropython/py/misc.h"
typedef uint64_t mp_float_uint_t;
# 240 "/Users/newworld/dev/uuos3/externals/micropython/py/misc.h"
typedef union _mp_float_union_t {
    mp_float_t f;

    struct {
        mp_float_uint_t frc : (52);
        mp_float_uint_t exp : (11);
        mp_float_uint_t sgn : 1;
    } p;







    mp_float_uint_t i;
} mp_float_union_t;
# 301 "/Users/newworld/dev/uuos3/externals/micropython/py/misc.h"
typedef const char *mp_rom_error_text_t;
# 31 "/Users/newworld/dev/uuos3/externals/micropython/py/smallint.h" 2
# 64 "/Users/newworld/dev/uuos3/externals/micropython/py/smallint.h"
_Bool mp_small_int_mul_overflow(mp_int_t x, mp_int_t y);
mp_int_t mp_small_int_modulo(mp_int_t dividend, mp_int_t divisor);
mp_int_t mp_small_int_floor_divide(mp_int_t num, mp_int_t denom);
# 35 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c" 2
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/runtime.h" 1
# 29 "/Users/newworld/dev/uuos3/externals/micropython/py/runtime.h"
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h" 1
# 32 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h"
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/mpthread.h" 1
# 33 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h" 2

# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/nlr.h" 1
# 32 "/Users/newworld/dev/uuos3/externals/micropython/py/nlr.h"
# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/limits.h" 1







# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/bits/limits.h" 1
# 9 "/Users/newworld/dev/uuos3/contracts/include/libc/limits.h" 2
# 33 "/Users/newworld/dev/uuos3/externals/micropython/py/nlr.h" 2
# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/assert.h" 1
# 19 "/Users/newworld/dev/uuos3/contracts/include/libc/assert.h"
_Noreturn void __assert_fail (const char *, const char *, int, const char *);
# 34 "/Users/newworld/dev/uuos3/externals/micropython/py/nlr.h" 2
# 91 "/Users/newworld/dev/uuos3/externals/micropython/py/nlr.h"
# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/setjmp.h" 1
# 10 "/Users/newworld/dev/uuos3/contracts/include/libc/setjmp.h"
typedef unsigned long __jmp_buf[6];
typedef struct __jmp_buf_tag {
 __jmp_buf __jb;
 unsigned long __fl;
 unsigned long __ss[128/sizeof(long)];
} jmp_buf[1];




typedef jmp_buf sigjmp_buf;
int sigsetjmp (sigjmp_buf, int);
_Noreturn void siglongjmp (sigjmp_buf, int);




int _setjmp (jmp_buf);
_Noreturn void _longjmp (jmp_buf, int);


int setjmp (jmp_buf);
int setjmp_ex (jmp_buf);

_Noreturn void longjmp (jmp_buf, int);
_Noreturn void longjmp_ex (jmp_buf, int);
# 92 "/Users/newworld/dev/uuos3/externals/micropython/py/nlr.h" 2


typedef struct _nlr_buf_t nlr_buf_t;
struct _nlr_buf_t {

    nlr_buf_t *prev;
    void *ret_val;


    jmp_buf jmpbuf;





    void *pystack;

};
# 140 "/Users/newworld/dev/uuos3/externals/micropython/py/nlr.h"
unsigned int nlr_push_tail(nlr_buf_t *top);
void nlr_pop(void);
__attribute__((noreturn)) void nlr_jump(void *val);




__attribute__((noreturn)) void nlr_jump_fail(void *val);
# 35 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h" 2
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/obj.h" 1
# 29 "/Users/newworld/dev/uuos3/externals/micropython/py/obj.h"
# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/assert.h" 1
# 19 "/Users/newworld/dev/uuos3/contracts/include/libc/assert.h"
_Noreturn void __assert_fail (const char *, const char *, int, const char *);
# 30 "/Users/newworld/dev/uuos3/externals/micropython/py/obj.h" 2



# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/qstr.h" 1
# 39 "/Users/newworld/dev/uuos3/externals/micropython/py/qstr.h"
enum {



# 1 "/Users/newworld/dev/uuos3/externals/micropython/ports/uuosio/build/genhdr/qstrdefs.generated.h" 1


MP_QSTRnull,
MP_QSTR_,
MP_QSTR___dir__,
MP_QSTR__0x0a_,
MP_QSTR__space_,
MP_QSTR__star_,
MP_QSTR__slash_,
MP_QSTR__lt_module_gt_,
MP_QSTR__,
MP_QSTR___call__,
MP_QSTR___class__,
MP_QSTR___delitem__,
MP_QSTR___enter__,
MP_QSTR___exit__,
MP_QSTR___getattr__,
MP_QSTR___getitem__,
MP_QSTR___hash__,
MP_QSTR___init__,
MP_QSTR___int__,
MP_QSTR___iter__,
MP_QSTR___len__,
MP_QSTR___main__,
MP_QSTR___module__,
MP_QSTR___name__,
MP_QSTR___new__,
MP_QSTR___next__,
MP_QSTR___qualname__,
MP_QSTR___repr__,
MP_QSTR___setitem__,
MP_QSTR___str__,
MP_QSTR_ArithmeticError,
MP_QSTR_AssertionError,
MP_QSTR_AttributeError,
MP_QSTR_BaseException,
MP_QSTR_EOFError,
MP_QSTR_Ellipsis,
MP_QSTR_Exception,
MP_QSTR_GeneratorExit,
MP_QSTR_ImportError,
MP_QSTR_IndentationError,
MP_QSTR_IndexError,
MP_QSTR_KeyError,
MP_QSTR_KeyboardInterrupt,
MP_QSTR_LookupError,
MP_QSTR_MemoryError,
MP_QSTR_NameError,
MP_QSTR_NoneType,
MP_QSTR_NotImplementedError,
MP_QSTR_OSError,
MP_QSTR_OverflowError,
MP_QSTR_RuntimeError,
MP_QSTR_StopIteration,
MP_QSTR_SyntaxError,
MP_QSTR_SystemExit,
MP_QSTR_TypeError,
MP_QSTR_ValueError,
MP_QSTR_ZeroDivisionError,
MP_QSTR_abs,
MP_QSTR_all,
MP_QSTR_any,
MP_QSTR_append,
MP_QSTR_args,
MP_QSTR_bool,
MP_QSTR_builtins,
MP_QSTR_bytearray,
MP_QSTR_bytecode,
MP_QSTR_bytes,
MP_QSTR_callable,
MP_QSTR_chr,
MP_QSTR_classmethod,
MP_QSTR_clear,
MP_QSTR_close,
MP_QSTR_const,
MP_QSTR_copy,
MP_QSTR_count,
MP_QSTR_dict,
MP_QSTR_dir,
MP_QSTR_divmod,
MP_QSTR_end,
MP_QSTR_endswith,
MP_QSTR_eval,
MP_QSTR_exec,
MP_QSTR_extend,
MP_QSTR_find,
MP_QSTR_format,
MP_QSTR_from_bytes,
MP_QSTR_get,
MP_QSTR_getattr,
MP_QSTR_globals,
MP_QSTR_hasattr,
MP_QSTR_hash,
MP_QSTR_id,
MP_QSTR_index,
MP_QSTR_insert,
MP_QSTR_int,
MP_QSTR_isalpha,
MP_QSTR_isdigit,
MP_QSTR_isinstance,
MP_QSTR_islower,
MP_QSTR_isspace,
MP_QSTR_issubclass,
MP_QSTR_isupper,
MP_QSTR_items,
MP_QSTR_iter,
MP_QSTR_join,
MP_QSTR_key,
MP_QSTR_keys,
MP_QSTR_len,
MP_QSTR_list,
MP_QSTR_little,
MP_QSTR_locals,
MP_QSTR_lower,
MP_QSTR_lstrip,
MP_QSTR_main,
MP_QSTR_map,
MP_QSTR_micropython,
MP_QSTR_next,
MP_QSTR_object,
MP_QSTR_open,
MP_QSTR_ord,
MP_QSTR_pop,
MP_QSTR_popitem,
MP_QSTR_pow,
MP_QSTR_print,
MP_QSTR_range,
MP_QSTR_read,
MP_QSTR_readinto,
MP_QSTR_readline,
MP_QSTR_remove,
MP_QSTR_replace,
MP_QSTR_repr,
MP_QSTR_reverse,
MP_QSTR_rfind,
MP_QSTR_rindex,
MP_QSTR_round,
MP_QSTR_rsplit,
MP_QSTR_rstrip,
MP_QSTR_self,
MP_QSTR_send,
MP_QSTR_sep,
MP_QSTR_set,
MP_QSTR_setattr,
MP_QSTR_setdefault,
MP_QSTR_sort,
MP_QSTR_sorted,
MP_QSTR_split,
MP_QSTR_start,
MP_QSTR_startswith,
MP_QSTR_staticmethod,
MP_QSTR_step,
MP_QSTR_stop,
MP_QSTR_str,
MP_QSTR_strip,
MP_QSTR_sum,
MP_QSTR_super,
MP_QSTR_throw,
MP_QSTR_to_bytes,
MP_QSTR_tuple,
MP_QSTR_type,
MP_QSTR_update,
MP_QSTR_upper,
MP_QSTR_utf_hyphen_8,
MP_QSTR_value,
MP_QSTR_values,
MP_QSTR_write,
MP_QSTR_zip,
MP_QSTR___abs__,
MP_QSTR___add__,
MP_QSTR___aenter__,
MP_QSTR___aexit__,
MP_QSTR___aiter__,
MP_QSTR___and__,
MP_QSTR___anext__,
MP_QSTR___bases__,
MP_QSTR___bool__,
MP_QSTR___build_class__,
MP_QSTR___contains__,
MP_QSTR___dict__,
MP_QSTR___divmod__,
MP_QSTR___doc__,
MP_QSTR___eq__,
MP_QSTR___file__,
MP_QSTR___floordiv__,
MP_QSTR___ge__,
MP_QSTR___gt__,
MP_QSTR___iadd__,
MP_QSTR___import__,
MP_QSTR___invert__,
MP_QSTR___isub__,
MP_QSTR___le__,
MP_QSTR___lshift__,
MP_QSTR___lt__,
MP_QSTR___matmul__,
MP_QSTR___mod__,
MP_QSTR___mul__,
MP_QSTR___ne__,
MP_QSTR___neg__,
MP_QSTR___or__,
MP_QSTR___path__,
MP_QSTR___pos__,
MP_QSTR___pow__,
MP_QSTR___repl_print__,
MP_QSTR___reversed__,
MP_QSTR___rshift__,
MP_QSTR___sub__,
MP_QSTR___traceback__,
MP_QSTR___truediv__,
MP_QSTR___xor__,
MP_QSTR__percent__hash_o,
MP_QSTR__percent__hash_x,
MP_QSTR__brace_open__colon__hash_b_brace_close_,
MP_QSTR_maximum_space_recursion_space_depth_space_exceeded,
MP_QSTR__lt_lambda_gt_,
MP_QSTR__lt_listcomp_gt_,
MP_QSTR__lt_dictcomp_gt_,
MP_QSTR__lt_setcomp_gt_,
MP_QSTR__lt_genexpr_gt_,
MP_QSTR__lt_string_gt_,
MP_QSTR__lt_stdin_gt_,
MP_QSTR_pystack_space_exhausted,
MP_QSTR_ARRAY,
MP_QSTR_BFINT16,
MP_QSTR_BFINT32,
MP_QSTR_BFINT8,
MP_QSTR_BFUINT16,
MP_QSTR_BFUINT32,
MP_QSTR_BFUINT8,
MP_QSTR_BF_LEN,
MP_QSTR_BF_POS,
MP_QSTR_BIG_ENDIAN,
MP_QSTR_BytesIO,
MP_QSTR_DecompIO,
MP_QSTR_EACCES,
MP_QSTR_EADDRINUSE,
MP_QSTR_EAGAIN,
MP_QSTR_EALREADY,
MP_QSTR_EBADF,
MP_QSTR_ECONNABORTED,
MP_QSTR_ECONNREFUSED,
MP_QSTR_ECONNRESET,
MP_QSTR_EEXIST,
MP_QSTR_EHOSTUNREACH,
MP_QSTR_EINPROGRESS,
MP_QSTR_EINVAL,
MP_QSTR_EIO,
MP_QSTR_EISDIR,
MP_QSTR_ENOBUFS,
MP_QSTR_ENODEV,
MP_QSTR_ENOENT,
MP_QSTR_ENOMEM,
MP_QSTR_ENOTCONN,
MP_QSTR_EOPNOTSUPP,
MP_QSTR_EPERM,
MP_QSTR_ETIMEDOUT,
MP_QSTR_FLOAT32,
MP_QSTR_FLOAT64,
MP_QSTR_INT,
MP_QSTR_INT16,
MP_QSTR_INT32,
MP_QSTR_INT64,
MP_QSTR_INT8,
MP_QSTR_LITTLE_ENDIAN,
MP_QSTR_LONG,
MP_QSTR_LONGLONG,
MP_QSTR_NATIVE,
MP_QSTR_OrderedDict,
MP_QSTR_POLLERR,
MP_QSTR_POLLHUP,
MP_QSTR_POLLIN,
MP_QSTR_POLLOUT,
MP_QSTR_PTR,
MP_QSTR_SHORT,
MP_QSTR_StopAsyncIteration,
MP_QSTR_StringIO,
MP_QSTR_UINT,
MP_QSTR_UINT16,
MP_QSTR_UINT32,
MP_QSTR_UINT64,
MP_QSTR_UINT8,
MP_QSTR_ULONG,
MP_QSTR_ULONGLONG,
MP_QSTR_USHORT,
MP_QSTR_UnicodeError,
MP_QSTR_VOID,
MP_QSTR_a2b_base64,
MP_QSTR_abs_tol,
MP_QSTR_acos,
MP_QSTR_acosh,
MP_QSTR_add,
MP_QSTR_addressof,
MP_QSTR_argv,
MP_QSTR_array,
MP_QSTR_asin,
MP_QSTR_asinh,
MP_QSTR_assert_recover_key,
MP_QSTR_assert_ripemd160,
MP_QSTR_assert_sha1,
MP_QSTR_assert_sha256,
MP_QSTR_assert_sha512,
MP_QSTR_atan,
MP_QSTR_atan2,
MP_QSTR_atanh,
MP_QSTR_b2a_base64,
MP_QSTR_bigint,
MP_QSTR_bin,
MP_QSTR_bound_method,
MP_QSTR_bytearray_at,
MP_QSTR_byteorder,
MP_QSTR_bytes_at,
MP_QSTR_calcsize,
MP_QSTR_call_contract,
MP_QSTR_ceil,
MP_QSTR_center,
MP_QSTR_chaindb,
MP_QSTR_chainlib,
MP_QSTR_choice,
MP_QSTR_closure,
MP_QSTR_cmath,
MP_QSTR_code,
MP_QSTR_compile,
MP_QSTR_complex,
MP_QSTR_copysign,
MP_QSTR_cos,
MP_QSTR_cosh,
MP_QSTR_current_receiver,
MP_QSTR_db_end_i256,
MP_QSTR_db_end_i64,
MP_QSTR_db_find_i256,
MP_QSTR_db_find_i64,
MP_QSTR_db_get_i256,
MP_QSTR_db_get_i64,
MP_QSTR_db_get_table_count,
MP_QSTR_db_lowerbound_i256,
MP_QSTR_db_lowerbound_i64,
MP_QSTR_db_next_i256,
MP_QSTR_db_next_i64,
MP_QSTR_db_previous_i256,
MP_QSTR_db_previous_i64,
MP_QSTR_db_remove_i256,
MP_QSTR_db_remove_i64,
MP_QSTR_db_store_i256,
MP_QSTR_db_store_i64,
MP_QSTR_db_update_i256,
MP_QSTR_db_update_i64,
MP_QSTR_db_upperbound_i256,
MP_QSTR_db_upperbound_i64,
MP_QSTR_decode,
MP_QSTR_decompress,
MP_QSTR_default,
MP_QSTR_degrees,
MP_QSTR_delattr,
MP_QSTR_deleter,
MP_QSTR_dict_view,
MP_QSTR_difference,
MP_QSTR_difference_update,
MP_QSTR_digest,
MP_QSTR_discard,
MP_QSTR_doc,
MP_QSTR_dump,
MP_QSTR_dumps,
MP_QSTR_e,
MP_QSTR_encode,
MP_QSTR_enumerate,
MP_QSTR_erf,
MP_QSTR_erfc,
MP_QSTR_errorcode,
MP_QSTR_execfile,
MP_QSTR_exit,
MP_QSTR_exp,
MP_QSTR_expm1,
MP_QSTR_fabs,
MP_QSTR_filter,
MP_QSTR_float,
MP_QSTR_floor,
MP_QSTR_flush,
MP_QSTR_fmod,
MP_QSTR_framebuf,
MP_QSTR_frexp,
MP_QSTR_fromkeys,
MP_QSTR_frozenset,
MP_QSTR_function,
MP_QSTR_gamma,
MP_QSTR_generator,
MP_QSTR_get_active_producers,
MP_QSTR_getrandbits,
MP_QSTR_getter,
MP_QSTR_getvalue,
MP_QSTR_group,
MP_QSTR_has_auth,
MP_QSTR_heapify,
MP_QSTR_heappop,
MP_QSTR_heappush,
MP_QSTR_hello,
MP_QSTR_help,
MP_QSTR_hex,
MP_QSTR_hexlify,
MP_QSTR_imag,
MP_QSTR_implementation,
MP_QSTR_input,
MP_QSTR_intersection,
MP_QSTR_intersection_update,
MP_QSTR_ipoll,
MP_QSTR_is_account,
MP_QSTR_isclose,
MP_QSTR_isdisjoint,
MP_QSTR_isfinite,
MP_QSTR_isinf,
MP_QSTR_isnan,
MP_QSTR_issubset,
MP_QSTR_issuperset,
MP_QSTR_iterable,
MP_QSTR_iterator,
MP_QSTR_kbd_intr,
MP_QSTR_keepends,
MP_QSTR_ldexp,
MP_QSTR_lgamma,
MP_QSTR_load,
MP_QSTR_loads,
MP_QSTR_log,
MP_QSTR_log10,
MP_QSTR_log2,
MP_QSTR_match,
MP_QSTR_math,
MP_QSTR_max,
MP_QSTR_maxsize,
MP_QSTR_mem_info,
MP_QSTR_memoryview,
MP_QSTR_mi,
MP_QSTR_mi_new,
MP_QSTR_min,
MP_QSTR_modf,
MP_QSTR_modify,
MP_QSTR_module,
MP_QSTR_modules,
MP_QSTR_mpy,
MP_QSTR_n2s,
MP_QSTR_name,
MP_QSTR_namedtuple,
MP_QSTR_oct,
MP_QSTR_opt_level,
MP_QSTR_pack,
MP_QSTR_pack_into,
MP_QSTR_partition,
MP_QSTR_path,
MP_QSTR_pend_throw,
MP_QSTR_phase,
MP_QSTR_pi,
MP_QSTR_platform,
MP_QSTR_polar,
MP_QSTR_poll,
MP_QSTR_print_exception,
MP_QSTR_property,
MP_QSTR_publication_time,
MP_QSTR_pystack_use,
MP_QSTR_qstr_info,
MP_QSTR_radians,
MP_QSTR_randint,
MP_QSTR_random,
MP_QSTR_randrange,
MP_QSTR_read_action_data,
MP_QSTR_real,
MP_QSTR_recover_key,
MP_QSTR_rect,
MP_QSTR_register,
MP_QSTR_rel_tol,
MP_QSTR_require_auth,
MP_QSTR_require_auth2,
MP_QSTR_require_recipient,
MP_QSTR_reversed,
MP_QSTR_ripemd160,
MP_QSTR_rpartition,
MP_QSTR_s2n,
MP_QSTR_say_hello,
MP_QSTR_say_hello2,
MP_QSTR_schedule,
MP_QSTR_search,
MP_QSTR_seed,
MP_QSTR_seek,
MP_QSTR_select,
MP_QSTR_send_context_free_inline,
MP_QSTR_send_inline,
MP_QSTR_setter,
MP_QSTR_sha1,
MP_QSTR_sha256,
MP_QSTR_sha512,
MP_QSTR_sin,
MP_QSTR_single,
MP_QSTR_sinh,
MP_QSTR_sizeof,
MP_QSTR_sleep,
MP_QSTR_sleep_ms,
MP_QSTR_sleep_us,
MP_QSTR_slice,
MP_QSTR_splitlines,
MP_QSTR_sqrt,
MP_QSTR_stack_use,
MP_QSTR_struct,
MP_QSTR_symmetric_difference,
MP_QSTR_symmetric_difference_update,
MP_QSTR_sys,
MP_QSTR_tan,
MP_QSTR_tanh,
MP_QSTR_tell,
MP_QSTR_ticks_add,
MP_QSTR_ticks_cpu,
MP_QSTR_ticks_diff,
MP_QSTR_ticks_ms,
MP_QSTR_ticks_us,
MP_QSTR_trunc,
MP_QSTR_uarray,
MP_QSTR_ubinascii,
MP_QSTR_ucollections,
MP_QSTR_uctypes,
MP_QSTR_uerrno,
MP_QSTR_uhashlib,
MP_QSTR_uheapq,
MP_QSTR_uio,
MP_QSTR_ujson,
MP_QSTR_unhexlify,
MP_QSTR_uniform,
MP_QSTR_union,
MP_QSTR_unpack,
MP_QSTR_unpack_from,
MP_QSTR_unregister,
MP_QSTR_urandom,
MP_QSTR_ure,
MP_QSTR_uselect,
MP_QSTR_ustruct,
MP_QSTR_usys,
MP_QSTR_utime,
MP_QSTR_uzlib,
MP_QSTR_version,
MP_QSTR_version_info,
# 43 "/Users/newworld/dev/uuos3/externals/micropython/py/qstr.h" 2


    MP_QSTRnumber_of,
};

typedef size_t qstr;

typedef struct _qstr_pool_t {
    struct _qstr_pool_t *prev;
    size_t total_prev_len;
    size_t alloc;
    size_t len;
    const byte *qstrs[];
} qstr_pool_t;




void qstr_init(void);

mp_uint_t qstr_compute_hash(const byte *data, size_t len);
qstr qstr_find_strn(const char *str, size_t str_len);

qstr qstr_from_str(const char *str);
qstr qstr_from_strn(const char *str, size_t len);

mp_uint_t qstr_hash(qstr q);
const char *qstr_str(qstr q);
size_t qstr_len(qstr q);
const byte *qstr_data(qstr q, size_t *len);

void qstr_pool_info(size_t *n_pool, size_t *n_qstr, size_t *n_str_data_bytes, size_t *n_total_bytes);
void qstr_dump_data(void);
# 34 "/Users/newworld/dev/uuos3/externals/micropython/py/obj.h" 2
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/mpprint.h" 1
# 48 "/Users/newworld/dev/uuos3/externals/micropython/py/mpprint.h"
typedef void (*mp_print_strn_t)(void *data, const char *str, size_t len);

typedef struct _mp_print_t {
    void *data;
    mp_print_strn_t print_strn;
} mp_print_t;



extern const mp_print_t mp_plat_print;





int mp_print_str(const mp_print_t *print, const char *str);
int mp_print_strn(const mp_print_t *print, const char *str, size_t len, int flags, char fill, int width);

int mp_print_float(const mp_print_t *print, mp_float_t f, char fmt, int flags, char fill, int width, int prec);


int mp_printf(const mp_print_t *print, const char *fmt, ...);
# 35 "/Users/newworld/dev/uuos3/externals/micropython/py/obj.h" 2
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/runtime0.h" 1
# 65 "/Users/newworld/dev/uuos3/externals/micropython/py/runtime0.h"
typedef enum {


    MP_UNARY_OP_POSITIVE,
    MP_UNARY_OP_NEGATIVE,
    MP_UNARY_OP_INVERT,
    MP_UNARY_OP_NOT,


    MP_UNARY_OP_BOOL,
    MP_UNARY_OP_LEN,
    MP_UNARY_OP_HASH,
    MP_UNARY_OP_ABS,
    MP_UNARY_OP_INT,
    MP_UNARY_OP_SIZEOF,
} mp_unary_op_t;

typedef enum {




    MP_BINARY_OP_LESS,
    MP_BINARY_OP_MORE,
    MP_BINARY_OP_EQUAL,
    MP_BINARY_OP_LESS_EQUAL,
    MP_BINARY_OP_MORE_EQUAL,
    MP_BINARY_OP_NOT_EQUAL,
    MP_BINARY_OP_IN,
    MP_BINARY_OP_IS,
    MP_BINARY_OP_EXCEPTION_MATCH,


    MP_BINARY_OP_INPLACE_OR,
    MP_BINARY_OP_INPLACE_XOR,
    MP_BINARY_OP_INPLACE_AND,
    MP_BINARY_OP_INPLACE_LSHIFT,
    MP_BINARY_OP_INPLACE_RSHIFT,
    MP_BINARY_OP_INPLACE_ADD,
    MP_BINARY_OP_INPLACE_SUBTRACT,
    MP_BINARY_OP_INPLACE_MULTIPLY,
    MP_BINARY_OP_INPLACE_MAT_MULTIPLY,
    MP_BINARY_OP_INPLACE_FLOOR_DIVIDE,
    MP_BINARY_OP_INPLACE_TRUE_DIVIDE,
    MP_BINARY_OP_INPLACE_MODULO,
    MP_BINARY_OP_INPLACE_POWER,


    MP_BINARY_OP_OR,
    MP_BINARY_OP_XOR,
    MP_BINARY_OP_AND,
    MP_BINARY_OP_LSHIFT,
    MP_BINARY_OP_RSHIFT,
    MP_BINARY_OP_ADD,
    MP_BINARY_OP_SUBTRACT,
    MP_BINARY_OP_MULTIPLY,
    MP_BINARY_OP_MAT_MULTIPLY,
    MP_BINARY_OP_FLOOR_DIVIDE,
    MP_BINARY_OP_TRUE_DIVIDE,
    MP_BINARY_OP_MODULO,
    MP_BINARY_OP_POWER,






    MP_BINARY_OP_DIVMOD,



    MP_BINARY_OP_CONTAINS,



    MP_BINARY_OP_REVERSE_OR,
    MP_BINARY_OP_REVERSE_XOR,
    MP_BINARY_OP_REVERSE_AND,
    MP_BINARY_OP_REVERSE_LSHIFT,
    MP_BINARY_OP_REVERSE_RSHIFT,
    MP_BINARY_OP_REVERSE_ADD,
    MP_BINARY_OP_REVERSE_SUBTRACT,
    MP_BINARY_OP_REVERSE_MULTIPLY,
    MP_BINARY_OP_REVERSE_MAT_MULTIPLY,
    MP_BINARY_OP_REVERSE_FLOOR_DIVIDE,
    MP_BINARY_OP_REVERSE_TRUE_DIVIDE,
    MP_BINARY_OP_REVERSE_MODULO,
    MP_BINARY_OP_REVERSE_POWER,


    MP_BINARY_OP_NOT_IN,
    MP_BINARY_OP_IS_NOT,
} mp_binary_op_t;
# 36 "/Users/newworld/dev/uuos3/externals/micropython/py/obj.h" 2








typedef void *mp_obj_t;
typedef const void *mp_const_obj_t;




typedef struct _mp_obj_type_t mp_obj_type_t;



struct _mp_obj_base_t {
    const mp_obj_type_t *type ;
};
typedef struct _mp_obj_base_t mp_obj_base_t;
# 86 "/Users/newworld/dev/uuos3/externals/micropython/py/obj.h"
static inline _Bool mp_obj_is_small_int(mp_const_obj_t o) {
    return (((mp_int_t)(o)) & 1) != 0;
}



static inline _Bool mp_obj_is_qstr(mp_const_obj_t o) {
    return (((mp_int_t)(o)) & 7) == 2;
}



static inline _Bool mp_obj_is_immediate_obj(mp_const_obj_t o) {
    return (((mp_int_t)(o)) & 7) == 6;
}






extern const struct _mp_obj_float_t mp_const_float_e_obj;
extern const struct _mp_obj_float_t mp_const_float_pi_obj;


mp_float_t mp_obj_float_get(mp_obj_t self_in);
mp_obj_t mp_obj_new_float(mp_float_t value);


static inline _Bool mp_obj_is_obj(mp_const_obj_t o) {
    return (((mp_int_t)(o)) & 3) == 0;
}
# 306 "/Users/newworld/dev/uuos3/externals/micropython/py/obj.h"
typedef mp_const_obj_t mp_rom_obj_t;
# 398 "/Users/newworld/dev/uuos3/externals/micropython/py/obj.h"
typedef struct _mp_map_elem_t {
    mp_obj_t key;
    mp_obj_t value;
} mp_map_elem_t;

typedef struct _mp_rom_map_elem_t {
    mp_rom_obj_t key;
    mp_rom_obj_t value;
} mp_rom_map_elem_t;

typedef struct _mp_map_t {
    size_t all_keys_are_qstrs : 1;
    size_t is_fixed : 1;
    size_t is_ordered : 1;
    size_t used : (8 * sizeof(size_t) - 3);
    size_t alloc;
    mp_map_elem_t *table;
} mp_map_t;


typedef enum _mp_map_lookup_kind_t {
    MP_MAP_LOOKUP = 0,
    MP_MAP_LOOKUP_ADD_IF_NOT_FOUND = 1,
    MP_MAP_LOOKUP_REMOVE_IF_FOUND = 2,
    MP_MAP_LOOKUP_ADD_IF_NOT_FOUND_OR_REMOVE_IF_FOUND = 3,
} mp_map_lookup_kind_t;

static inline _Bool mp_map_slot_is_filled(const mp_map_t *map, size_t pos) {
    (void)0;
    return (map)->table[pos].key != (((mp_obj_t)(void *)0)) && (map)->table[pos].key != (((mp_obj_t)(void *)4));
}

void mp_map_init(mp_map_t *map, size_t n);
void mp_map_init_fixed_table(mp_map_t *map, size_t n, const mp_obj_t *table);
mp_map_t *mp_map_new(size_t n);
void mp_map_deinit(mp_map_t *map);
void mp_map_free(mp_map_t *map);
mp_map_elem_t *mp_map_lookup(mp_map_t *map, mp_obj_t index, mp_map_lookup_kind_t lookup_kind);
void mp_map_clear(mp_map_t *map);
void mp_map_dump(mp_map_t *map);



typedef struct _mp_set_t {
    size_t alloc;
    size_t used;
    mp_obj_t *table;
} mp_set_t;

static inline _Bool mp_set_slot_is_filled(const mp_set_t *set, size_t pos) {
    return (set)->table[pos] != (((mp_obj_t)(void *)0)) && (set)->table[pos] != (((mp_obj_t)(void *)4));
}

void mp_set_init(mp_set_t *set, size_t n);
mp_obj_t mp_set_lookup(mp_set_t *set, mp_obj_t index, mp_map_lookup_kind_t lookup_kind);
mp_obj_t mp_set_remove_first(mp_set_t *set);
void mp_set_clear(mp_set_t *set);



typedef mp_obj_t (*mp_fun_0_t)(void);
typedef mp_obj_t (*mp_fun_1_t)(mp_obj_t);
typedef mp_obj_t (*mp_fun_2_t)(mp_obj_t, mp_obj_t);
typedef mp_obj_t (*mp_fun_3_t)(mp_obj_t, mp_obj_t, mp_obj_t);
typedef mp_obj_t (*mp_fun_var_t)(size_t n, const mp_obj_t *);


typedef mp_obj_t (*mp_fun_kw_t)(size_t n, const mp_obj_t *, mp_map_t *);
# 484 "/Users/newworld/dev/uuos3/externals/micropython/py/obj.h"
typedef enum {
    PRINT_STR = 0,
    PRINT_REPR = 1,
    PRINT_EXC = 2,
    PRINT_JSON = 3,
    PRINT_RAW = 4,
    PRINT_EXC_SUBCLASS = 0x80,
} mp_print_kind_t;

typedef struct _mp_obj_iter_buf_t {
    mp_obj_base_t base;
    mp_obj_t buf[3];
} mp_obj_iter_buf_t;





typedef void (*mp_print_fun_t)(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind);
typedef mp_obj_t (*mp_make_new_fun_t)(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
typedef mp_obj_t (*mp_call_fun_t)(mp_obj_t fun, size_t n_args, size_t n_kw, const mp_obj_t *args);
typedef mp_obj_t (*mp_unary_op_fun_t)(mp_unary_op_t op, mp_obj_t);
typedef mp_obj_t (*mp_binary_op_fun_t)(mp_binary_op_t op, mp_obj_t, mp_obj_t);
typedef void (*mp_attr_fun_t)(mp_obj_t self_in, qstr attr, mp_obj_t *dest);
typedef mp_obj_t (*mp_subscr_fun_t)(mp_obj_t self_in, mp_obj_t index, mp_obj_t value);
typedef mp_obj_t (*mp_getiter_fun_t)(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf);


typedef struct _mp_buffer_info_t {
    void *buf;
    size_t len;
    int typecode;
} mp_buffer_info_t;



typedef struct _mp_buffer_p_t {
    mp_int_t (*get_buffer)(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags);
} mp_buffer_p_t;
_Bool mp_get_buffer(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags);
void mp_get_buffer_raise(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags);

struct _mp_obj_type_t {

    mp_obj_base_t base;


    uint16_t flags;


    uint16_t name;


    mp_print_fun_t print;


    mp_make_new_fun_t make_new;


    mp_call_fun_t call;



    mp_unary_op_fun_t unary_op;
    mp_binary_op_fun_t binary_op;
# 561 "/Users/newworld/dev/uuos3/externals/micropython/py/obj.h"
    mp_attr_fun_t attr;






    mp_subscr_fun_t subscr;




    mp_getiter_fun_t getiter;



    mp_fun_1_t iternext;


    mp_buffer_p_t buffer_p;


    const void *protocol;





    const void *parent;


    struct _mp_obj_dict_t *locals_dict;
};


extern const mp_obj_type_t mp_type_type;
extern const mp_obj_type_t mp_type_object;
extern const mp_obj_type_t mp_type_NoneType;
extern const mp_obj_type_t mp_type_bool;
extern const mp_obj_type_t mp_type_int;
extern const mp_obj_type_t mp_type_bigint;
extern const mp_obj_type_t mp_type_str;
extern const mp_obj_type_t mp_type_bytes;
extern const mp_obj_type_t mp_type_bytearray;
extern const mp_obj_type_t mp_type_memoryview;
extern const mp_obj_type_t mp_type_float;
extern const mp_obj_type_t mp_type_complex;
extern const mp_obj_type_t mp_type_tuple;
extern const mp_obj_type_t mp_type_list;
extern const mp_obj_type_t mp_type_map;
extern const mp_obj_type_t mp_type_enumerate;
extern const mp_obj_type_t mp_type_filter;
extern const mp_obj_type_t mp_type_deque;
extern const mp_obj_type_t mp_type_dict;
extern const mp_obj_type_t mp_type_ordereddict;
extern const mp_obj_type_t mp_type_range;
extern const mp_obj_type_t mp_type_set;
extern const mp_obj_type_t mp_type_frozenset;
extern const mp_obj_type_t mp_type_slice;
extern const mp_obj_type_t mp_type_zip;
extern const mp_obj_type_t mp_type_array;
extern const mp_obj_type_t mp_type_super;
extern const mp_obj_type_t mp_type_gen_wrap;
extern const mp_obj_type_t mp_type_native_gen_wrap;
extern const mp_obj_type_t mp_type_gen_instance;
extern const mp_obj_type_t mp_type_fun_builtin_0;
extern const mp_obj_type_t mp_type_fun_builtin_1;
extern const mp_obj_type_t mp_type_fun_builtin_2;
extern const mp_obj_type_t mp_type_fun_builtin_3;
extern const mp_obj_type_t mp_type_fun_builtin_var;
extern const mp_obj_type_t mp_type_fun_bc;
extern const mp_obj_type_t mp_type_module;
extern const mp_obj_type_t mp_type_staticmethod;
extern const mp_obj_type_t mp_type_classmethod;
extern const mp_obj_type_t mp_type_property;
extern const mp_obj_type_t mp_type_stringio;
extern const mp_obj_type_t mp_type_bytesio;
extern const mp_obj_type_t mp_type_reversed;
extern const mp_obj_type_t mp_type_polymorph_iter;


extern const mp_obj_type_t mp_type_BaseException;
extern const mp_obj_type_t mp_type_ArithmeticError;
extern const mp_obj_type_t mp_type_AssertionError;
extern const mp_obj_type_t mp_type_AttributeError;
extern const mp_obj_type_t mp_type_EOFError;
extern const mp_obj_type_t mp_type_Exception;
extern const mp_obj_type_t mp_type_GeneratorExit;
extern const mp_obj_type_t mp_type_ImportError;
extern const mp_obj_type_t mp_type_IndentationError;
extern const mp_obj_type_t mp_type_IndexError;
extern const mp_obj_type_t mp_type_KeyboardInterrupt;
extern const mp_obj_type_t mp_type_KeyError;
extern const mp_obj_type_t mp_type_LookupError;
extern const mp_obj_type_t mp_type_MemoryError;
extern const mp_obj_type_t mp_type_NameError;
extern const mp_obj_type_t mp_type_NotImplementedError;
extern const mp_obj_type_t mp_type_OSError;
extern const mp_obj_type_t mp_type_OverflowError;
extern const mp_obj_type_t mp_type_RuntimeError;
extern const mp_obj_type_t mp_type_StopAsyncIteration;
extern const mp_obj_type_t mp_type_StopIteration;
extern const mp_obj_type_t mp_type_SyntaxError;
extern const mp_obj_type_t mp_type_SystemExit;
extern const mp_obj_type_t mp_type_TypeError;
extern const mp_obj_type_t mp_type_UnicodeError;
extern const mp_obj_type_t mp_type_ValueError;
extern const mp_obj_type_t mp_type_ViperTypeError;
extern const mp_obj_type_t mp_type_ZeroDivisionError;
# 692 "/Users/newworld/dev/uuos3/externals/micropython/py/obj.h"
extern const struct _mp_obj_str_t mp_const_empty_bytes_obj;
extern const struct _mp_obj_tuple_t mp_const_empty_tuple_obj;
extern const struct _mp_obj_dict_t mp_const_empty_dict_obj;
extern const struct _mp_obj_singleton_t mp_const_ellipsis_obj;
extern const struct _mp_obj_singleton_t mp_const_notimplemented_obj;
extern const struct _mp_obj_exception_t mp_const_GeneratorExit_obj;
# 726 "/Users/newworld/dev/uuos3/externals/micropython/py/obj.h"
mp_obj_t mp_obj_new_type(qstr name, mp_obj_t bases_tuple, mp_obj_t locals_dict);
static inline mp_obj_t mp_obj_new_bool(mp_int_t x) {
    return x ? ((mp_obj_t)(((3) << 3) | 6)) : ((mp_obj_t)(((1) << 3) | 6));
}
mp_obj_t mp_obj_new_cell(mp_obj_t obj);
mp_obj_t mp_obj_new_int(mp_int_t value);
mp_obj_t mp_obj_new_int_from_uint(mp_uint_t value);
mp_obj_t mp_obj_new_int_from_str_len(const char **str, size_t len, _Bool neg, unsigned int base);
mp_obj_t mp_obj_new_int_from_ll(long long val);
mp_obj_t mp_obj_new_int_from_ull(unsigned long long val);
mp_obj_t mp_obj_new_str(const char *data, size_t len);
mp_obj_t mp_obj_new_str_via_qstr(const char *data, size_t len);
mp_obj_t mp_obj_new_str_from_vstr(const mp_obj_type_t *type, vstr_t *vstr);
mp_obj_t mp_obj_new_bytes(const byte *data, size_t len);
mp_obj_t mp_obj_new_bytearray(size_t n, void *items);
mp_obj_t mp_obj_new_bytearray_by_ref(size_t n, void *items);

mp_obj_t mp_obj_new_int_from_float(mp_float_t val);
mp_obj_t mp_obj_new_complex(mp_float_t real, mp_float_t imag);

mp_obj_t mp_obj_new_exception(const mp_obj_type_t *exc_type);
mp_obj_t mp_obj_new_exception_arg1(const mp_obj_type_t *exc_type, mp_obj_t arg);
mp_obj_t mp_obj_new_exception_args(const mp_obj_type_t *exc_type, size_t n_args, const mp_obj_t *args);
mp_obj_t mp_obj_new_exception_msg(const mp_obj_type_t *exc_type, mp_rom_error_text_t msg);
mp_obj_t mp_obj_new_exception_msg_varg(const mp_obj_type_t *exc_type, mp_rom_error_text_t fmt, ...);



mp_obj_t mp_obj_new_fun_bc(mp_obj_t def_args, mp_obj_t def_kw_args, const byte *code, const mp_uint_t *const_table);
mp_obj_t mp_obj_new_fun_native(mp_obj_t def_args_in, mp_obj_t def_kw_args, const void *fun_data, const mp_uint_t *const_table);
mp_obj_t mp_obj_new_fun_asm(size_t n_args, const void *fun_data, mp_uint_t type_sig);
mp_obj_t mp_obj_new_gen_wrap(mp_obj_t fun);
mp_obj_t mp_obj_new_closure(mp_obj_t fun, size_t n_closed, const mp_obj_t *closed);
mp_obj_t mp_obj_new_tuple(size_t n, const mp_obj_t *items);
mp_obj_t mp_obj_new_list(size_t n, mp_obj_t *items);
mp_obj_t mp_obj_new_dict(size_t n_args);
mp_obj_t mp_obj_new_set(size_t n_args, mp_obj_t *items);
mp_obj_t mp_obj_new_slice(mp_obj_t start, mp_obj_t stop, mp_obj_t step);
mp_obj_t mp_obj_new_bound_meth(mp_obj_t meth, mp_obj_t self);
mp_obj_t mp_obj_new_getitem_iter(mp_obj_t *args, mp_obj_iter_buf_t *iter_buf);
mp_obj_t mp_obj_new_module(qstr module_name);
mp_obj_t mp_obj_new_memoryview(byte typecode, size_t nitems, void *items);

const mp_obj_type_t *mp_obj_get_type(mp_const_obj_t o_in);
const char *mp_obj_get_type_str(mp_const_obj_t o_in);
_Bool mp_obj_is_subclass_fast(mp_const_obj_t object, mp_const_obj_t classinfo);
mp_obj_t mp_obj_cast_to_native_base(mp_obj_t self_in, mp_const_obj_t native_type);

void mp_obj_print_helper(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind);
void mp_obj_print(mp_obj_t o, mp_print_kind_t kind);
void mp_obj_print_exception(const mp_print_t *print, mp_obj_t exc);

_Bool mp_obj_is_true(mp_obj_t arg);
_Bool mp_obj_is_callable(mp_obj_t o_in);
mp_obj_t mp_obj_equal_not_equal(mp_binary_op_t op, mp_obj_t o1, mp_obj_t o2);
_Bool mp_obj_equal(mp_obj_t o1, mp_obj_t o2);

static inline _Bool mp_obj_is_integer(mp_const_obj_t o) {
    return (mp_obj_is_small_int(o) || (mp_obj_is_obj(o) && (((mp_obj_base_t *)((void *)o))->type == (&mp_type_int)))) || ((o) == ((mp_obj_t)(((1) << 3) | 6)) || (o) == ((mp_obj_t)(((3) << 3) | 6)));
}
mp_int_t mp_obj_get_int(mp_const_obj_t arg);
mp_int_t mp_obj_get_int_truncated(mp_const_obj_t arg);
_Bool mp_obj_get_int_maybe(mp_const_obj_t arg, mp_int_t *value);

mp_float_t mp_obj_get_float(mp_obj_t self_in);
_Bool mp_obj_get_float_maybe(mp_obj_t arg, mp_float_t *value);
void mp_obj_get_complex(mp_obj_t self_in, mp_float_t *real, mp_float_t *imag);
_Bool mp_obj_get_complex_maybe(mp_obj_t self_in, mp_float_t *real, mp_float_t *imag);

void mp_obj_get_array(mp_obj_t o, size_t *len, mp_obj_t **items);
void mp_obj_get_array_fixed_n(mp_obj_t o, size_t len, mp_obj_t **items);
size_t mp_get_index(const mp_obj_type_t *type, size_t len, mp_obj_t index, _Bool is_slice);
mp_obj_t mp_obj_id(mp_obj_t o_in);
mp_obj_t mp_obj_len(mp_obj_t o_in);
mp_obj_t mp_obj_len_maybe(mp_obj_t o_in);
mp_obj_t mp_obj_subscr(mp_obj_t base, mp_obj_t index, mp_obj_t val);
mp_obj_t mp_generic_unary_op(mp_unary_op_t op, mp_obj_t o_in);


mp_obj_t mp_obj_cell_get(mp_obj_t self_in);
void mp_obj_cell_set(mp_obj_t self_in, mp_obj_t obj);



mp_int_t mp_obj_int_get_truncated(mp_const_obj_t self_in);

mp_int_t mp_obj_int_get_checked(mp_const_obj_t self_in);

mp_uint_t mp_obj_int_get_uint_checked(mp_const_obj_t self_in);



_Bool mp_obj_is_exception_type(mp_obj_t self_in);
_Bool mp_obj_is_exception_instance(mp_obj_t self_in);
_Bool mp_obj_exception_match(mp_obj_t exc, mp_const_obj_t exc_type);
void mp_obj_exception_clear_traceback(mp_obj_t self_in);
void mp_obj_exception_add_traceback(mp_obj_t self_in, qstr file, size_t line, qstr block);
void mp_obj_exception_get_traceback(mp_obj_t self_in, size_t *n, size_t **values);
mp_obj_t mp_obj_exception_get_value(mp_obj_t self_in);
mp_obj_t mp_obj_exception_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args);
mp_obj_t mp_alloc_emergency_exception_buf(mp_obj_t size_in);
void mp_init_emergency_exception_buf(void);


_Bool mp_obj_str_equal(mp_obj_t s1, mp_obj_t s2);
qstr mp_obj_str_get_qstr(mp_obj_t self_in);
const char *mp_obj_str_get_str(mp_obj_t self_in);
const char *mp_obj_str_get_data(mp_obj_t self_in, size_t *len);
mp_obj_t mp_obj_str_intern(mp_obj_t str);
mp_obj_t mp_obj_str_intern_checked(mp_obj_t obj);
void mp_str_print_quoted(const mp_print_t *print, const byte *str_data, size_t str_len, _Bool is_bytes);
# 857 "/Users/newworld/dev/uuos3/externals/micropython/py/obj.h"
static inline float mp_obj_get_float_to_f(mp_obj_t o) {
    return (float)mp_obj_get_float(o);
}

static inline double mp_obj_get_float_to_d(mp_obj_t o) {
    return mp_obj_get_float(o);
}

static inline mp_obj_t mp_obj_new_float_from_f(float o) {
    return mp_obj_new_float((mp_float_t)o);
}

static inline mp_obj_t mp_obj_new_float_from_d(double o) {
    return mp_obj_new_float(o);
}




static inline mp_int_t mp_float_hash(mp_float_t val) {
    return (mp_int_t)val;
}

mp_obj_t mp_obj_float_binary_op(mp_binary_op_t op, mp_float_t lhs_val, mp_obj_t rhs);


void mp_obj_complex_get(mp_obj_t self_in, mp_float_t *real, mp_float_t *imag);
mp_obj_t mp_obj_complex_binary_op(mp_binary_op_t op, mp_float_t lhs_real, mp_float_t lhs_imag, mp_obj_t rhs_in);





void mp_obj_tuple_get(mp_obj_t self_in, size_t *len, mp_obj_t **items);
void mp_obj_tuple_del(mp_obj_t self_in);
mp_int_t mp_obj_tuple_hash(mp_obj_t self_in);


mp_obj_t mp_obj_list_append(mp_obj_t self_in, mp_obj_t arg);
mp_obj_t mp_obj_list_remove(mp_obj_t self_in, mp_obj_t value);
void mp_obj_list_get(mp_obj_t self_in, size_t *len, mp_obj_t **items);
void mp_obj_list_set_len(mp_obj_t self_in, size_t len);
void mp_obj_list_store(mp_obj_t self_in, mp_obj_t index, mp_obj_t value);
mp_obj_t mp_obj_list_sort(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs);


typedef struct _mp_obj_dict_t {
    mp_obj_base_t base;
    mp_map_t map;
} mp_obj_dict_t;
mp_obj_t mp_obj_dict_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
void mp_obj_dict_init(mp_obj_dict_t *dict, size_t n_args);
size_t mp_obj_dict_len(mp_obj_t self_in);
mp_obj_t mp_obj_dict_get(mp_obj_t self_in, mp_obj_t index);
mp_obj_t mp_obj_dict_store(mp_obj_t self_in, mp_obj_t key, mp_obj_t value);
mp_obj_t mp_obj_dict_delete(mp_obj_t self_in, mp_obj_t key);
mp_obj_t mp_obj_dict_copy(mp_obj_t self_in);
static inline mp_map_t *mp_obj_dict_get_map(mp_obj_t dict) {
    return &((mp_obj_dict_t *)((void *)dict))->map;
}


void mp_obj_set_store(mp_obj_t self_in, mp_obj_t item);


typedef struct {
    mp_int_t start;
    mp_int_t stop;
    mp_int_t step;
} mp_bound_slice_t;


typedef struct _mp_obj_slice_t {
    mp_obj_base_t base;
    mp_obj_t start;
    mp_obj_t stop;
    mp_obj_t step;
} mp_obj_slice_t;
void mp_obj_slice_indices(mp_obj_t self_in, mp_int_t length, mp_bound_slice_t *result);



typedef struct _mp_obj_fun_builtin_fixed_t {
    mp_obj_base_t base;
    union {
        mp_fun_0_t _0;
        mp_fun_1_t _1;
        mp_fun_2_t _2;
        mp_fun_3_t _3;
    } fun;
} mp_obj_fun_builtin_fixed_t;

typedef struct _mp_obj_fun_builtin_var_t {
    mp_obj_base_t base;
    uint32_t sig;
    union {
        mp_fun_var_t var;
        mp_fun_kw_t kw;
    } fun;
} mp_obj_fun_builtin_var_t;

qstr mp_obj_fun_get_name(mp_const_obj_t fun);
qstr mp_obj_code_get_name(const byte *code_info);

mp_obj_t mp_identity(mp_obj_t self);
extern const mp_obj_fun_builtin_fixed_t mp_identity_obj;
mp_obj_t mp_identity_getiter(mp_obj_t self, mp_obj_iter_buf_t *iter_buf);


typedef struct _mp_obj_module_t {
    mp_obj_base_t base;
    mp_obj_dict_t *globals;
} mp_obj_module_t;
static inline mp_obj_dict_t *mp_obj_module_get_globals(mp_obj_t module) {
    return ((mp_obj_module_t *)((void *)module))->globals;
}

_Bool mp_obj_is_package(mp_obj_t module);



typedef struct _mp_obj_static_class_method_t {
    mp_obj_base_t base;
    mp_obj_t fun;
} mp_obj_static_class_method_t;
typedef struct _mp_rom_obj_static_class_method_t {
    mp_obj_base_t base;
    mp_rom_obj_t fun;
} mp_rom_obj_static_class_method_t;


const mp_obj_t *mp_obj_property_get(mp_obj_t self_in);



void mp_seq_multiply(const void *items, size_t item_sz, size_t len, size_t times, void *dest);

_Bool mp_seq_get_fast_slice_indexes(mp_uint_t len, mp_obj_t slice, mp_bound_slice_t *indexes);



_Bool mp_seq_cmp_bytes(mp_uint_t op, const byte *data1, size_t len1, const byte *data2, size_t len2);
_Bool mp_seq_cmp_objs(mp_uint_t op, const mp_obj_t *items1, size_t len1, const mp_obj_t *items2, size_t len2);
mp_obj_t mp_seq_index_obj(const mp_obj_t *items, size_t len, size_t n_args, const mp_obj_t *args);
mp_obj_t mp_seq_count_obj(const mp_obj_t *items, size_t len, mp_obj_t value);
mp_obj_t mp_seq_extract_slice(size_t len, const mp_obj_t *seq, mp_bound_slice_t *indexes);
# 36 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h" 2
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/objlist.h" 1
# 31 "/Users/newworld/dev/uuos3/externals/micropython/py/objlist.h"
typedef struct _mp_obj_list_t {
    mp_obj_base_t base;
    size_t alloc;
    size_t len;
    mp_obj_t *items;
} mp_obj_list_t;

void mp_obj_list_init(mp_obj_list_t *o, size_t n);
# 37 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h" 2
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/objexcept.h" 1
# 30 "/Users/newworld/dev/uuos3/externals/micropython/py/objexcept.h"
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/objtuple.h" 1
# 31 "/Users/newworld/dev/uuos3/externals/micropython/py/objtuple.h"
typedef struct _mp_obj_tuple_t {
    mp_obj_base_t base;
    size_t len;
    mp_obj_t items[];
} mp_obj_tuple_t;

typedef struct _mp_rom_obj_tuple_t {
    mp_obj_base_t base;
    size_t len;
    mp_rom_obj_t items[];
} mp_rom_obj_tuple_t;

void mp_obj_tuple_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind);
mp_obj_t mp_obj_tuple_unary_op(mp_unary_op_t op, mp_obj_t self_in);
mp_obj_t mp_obj_tuple_binary_op(mp_binary_op_t op, mp_obj_t lhs, mp_obj_t rhs);
mp_obj_t mp_obj_tuple_subscr(mp_obj_t base, mp_obj_t index, mp_obj_t value);
mp_obj_t mp_obj_tuple_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf);

extern const mp_obj_type_t mp_type_attrtuple;
# 59 "/Users/newworld/dev/uuos3/externals/micropython/py/objtuple.h"
void mp_obj_attrtuple_print_helper(const mp_print_t *print, const qstr *fields, mp_obj_tuple_t *o);


mp_obj_t mp_obj_new_attrtuple(const qstr *fields, size_t n, const mp_obj_t *items);
# 31 "/Users/newworld/dev/uuos3/externals/micropython/py/objexcept.h" 2

typedef struct _mp_obj_exception_t {
    mp_obj_base_t base;
    size_t traceback_alloc : (8 * sizeof(size_t) / 2);
    size_t traceback_len : (8 * sizeof(size_t) / 2);
    size_t *traceback_data;
    mp_obj_tuple_t *args;
} mp_obj_exception_t;

void mp_obj_exception_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind);
void mp_obj_exception_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest);
# 38 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h" 2
# 60 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h"
typedef struct _mp_sched_item_t {
    mp_obj_t func;
    mp_obj_t arg;
} mp_sched_item_t;


typedef struct _mp_state_mem_t {






    byte *gc_alloc_table_start;
    size_t gc_alloc_table_byte_len;



    byte *gc_pool_start;
    byte *gc_pool_end;

    int gc_stack_overflow;
    size_t gc_stack[(64)];
    uint16_t gc_lock_depth;




    uint16_t gc_auto_collect_enabled;


    size_t gc_alloc_amount;
    size_t gc_alloc_threshold;


    size_t gc_last_free_atb_index;
# 105 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h"
} mp_state_mem_t;



typedef struct _mp_state_vm_t {







    qstr_pool_t *last_pool;


    mp_obj_exception_t mp_emergency_exception_obj;
# 135 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h"
    mp_obj_exception_t mp_kbd_exception;



    mp_obj_dict_t mp_loaded_modules_dict;


    volatile mp_obj_t mp_pending_exception;


    mp_sched_item_t sched_queue[(1)];
# 159 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h"
    mp_obj_dict_t dict_main;


    mp_obj_list_t mp_sys_path_obj;
    mp_obj_list_t mp_sys_argv_obj;



    mp_obj_dict_t *mp_module_builtins_override_dict;
# 176 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h"
    const char *readline_hist[8];




    vstr_t *repl_line;
# 207 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h"
    byte *qstr_last_chunk;
    size_t qstr_last_alloc;
    size_t qstr_last_used;







    mp_uint_t mp_optimise_value;
# 229 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h"
    volatile int16_t sched_state;
    uint8_t sched_len;
    uint8_t sched_idx;






} mp_state_vm_t;



typedef struct _mp_state_thread_t {

    char *stack_top;






    uint8_t *pystack_start;
    uint8_t *pystack_end;
    uint8_t *pystack_cur;
# 262 "/Users/newworld/dev/uuos3/externals/micropython/py/mpstate.h"
    mp_obj_dict_t *dict_locals;
    mp_obj_dict_t *dict_globals;

    nlr_buf_t *nlr_top;






} mp_state_thread_t;



typedef struct _mp_state_ctx_t {
    mp_state_thread_t thread;
    mp_state_vm_t vm;
    mp_state_mem_t mem;
} mp_state_ctx_t;

extern mp_state_ctx_t mp_state_ctx;
# 30 "/Users/newworld/dev/uuos3/externals/micropython/py/runtime.h" 2
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/pystack.h" 1
# 37 "/Users/newworld/dev/uuos3/externals/micropython/py/pystack.h"
void mp_pystack_init(void *start, void *end);
void *mp_pystack_alloc(size_t n_bytes);




static inline void mp_pystack_free(void *ptr) {
    (void)0;
    (void)0;
# 58 "/Users/newworld/dev/uuos3/externals/micropython/py/pystack.h"
    (mp_state_ctx.thread.pystack_cur) = (uint8_t *)ptr;
}

static inline void mp_pystack_realloc(void *ptr, size_t n_bytes) {
    mp_pystack_free(ptr);
    mp_pystack_alloc(n_bytes);
}

static inline size_t mp_pystack_usage(void) {
    return (mp_state_ctx.thread.pystack_cur) - (mp_state_ctx.thread.pystack_start);
}

static inline size_t mp_pystack_limit(void) {
    return (mp_state_ctx.thread.pystack_end) - (mp_state_ctx.thread.pystack_start);
}
# 98 "/Users/newworld/dev/uuos3/externals/micropython/py/pystack.h"
static inline void *mp_local_alloc(size_t n_bytes) {
    return mp_pystack_alloc(n_bytes);
}

static inline void mp_local_free(void *ptr) {
    mp_pystack_free(ptr);
}

static inline void *mp_nonlocal_alloc(size_t n_bytes) {
    return mp_pystack_alloc(n_bytes);
}

static inline void *mp_nonlocal_realloc(void *ptr, size_t old_n_bytes, size_t new_n_bytes) {
    (void)old_n_bytes;
    mp_pystack_realloc(ptr, new_n_bytes);
    return ptr;
}

static inline void mp_nonlocal_free(void *ptr, size_t n_bytes) {
    (void)n_bytes;
    mp_pystack_free(ptr);
}
# 31 "/Users/newworld/dev/uuos3/externals/micropython/py/runtime.h" 2

typedef enum {
    MP_VM_RETURN_NORMAL,
    MP_VM_RETURN_YIELD,
    MP_VM_RETURN_EXCEPTION,
} mp_vm_return_kind_t;

typedef enum {
    MP_ARG_BOOL = 0x001,
    MP_ARG_INT = 0x002,
    MP_ARG_OBJ = 0x003,
    MP_ARG_KIND_MASK = 0x0ff,
    MP_ARG_REQUIRED = 0x100,
    MP_ARG_KW_ONLY = 0x200,
} mp_arg_flag_t;

typedef union _mp_arg_val_t {
    _Bool u_bool;
    mp_int_t u_int;
    mp_obj_t u_obj;
    mp_rom_obj_t u_rom_obj;
} mp_arg_val_t;

typedef struct _mp_arg_t {
    uint16_t qst;
    uint16_t flags;
    mp_arg_val_t defval;
} mp_arg_t;


extern const byte mp_unary_op_method_name[];
extern const byte mp_binary_op_method_name[];

void mp_init(void);
void mp_deinit(void);

void mp_keyboard_interrupt(void);
void mp_handle_pending(_Bool raise_exc);
void mp_handle_pending_tail(mp_uint_t atomic_state);


void mp_sched_lock(void);
void mp_sched_unlock(void);

_Bool mp_sched_schedule(mp_obj_t function, mp_obj_t arg);



int mp_print_mp_int(const mp_print_t *print, mp_obj_t x, int base, int base_char, int flags, char fill, int width, int prec);

void mp_arg_check_num_sig(size_t n_args, size_t n_kw, uint32_t sig);
static inline void mp_arg_check_num(size_t n_args, size_t n_kw, size_t n_args_min, size_t n_args_max, _Bool takes_kw) {
    mp_arg_check_num_sig(n_args, n_kw, ((uint32_t)((((uint32_t)(n_args_min)) << 17) | (((uint32_t)(n_args_max)) << 1) | ((takes_kw) ? 1 : 0))));
}
void mp_arg_parse_all(size_t n_pos, const mp_obj_t *pos, mp_map_t *kws, size_t n_allowed, const mp_arg_t *allowed, mp_arg_val_t *out_vals);
void mp_arg_parse_all_kw_array(size_t n_pos, size_t n_kw, const mp_obj_t *args, size_t n_allowed, const mp_arg_t *allowed, mp_arg_val_t *out_vals);
__attribute__((noreturn)) void mp_arg_error_terse_mismatch(void);
__attribute__((noreturn)) void mp_arg_error_unimpl_kw(void);

static inline mp_obj_dict_t *mp_locals_get(void) {
    return (mp_state_ctx.thread.dict_locals);
}
static inline void mp_locals_set(mp_obj_dict_t *d) {
    (mp_state_ctx.thread.dict_locals) = d;
}
static inline mp_obj_dict_t *mp_globals_get(void) {
    return (mp_state_ctx.thread.dict_globals);
}
static inline void mp_globals_set(mp_obj_dict_t *d) {
    (mp_state_ctx.thread.dict_globals) = d;
}

mp_obj_t mp_load_name(qstr qst);
mp_obj_t mp_load_global(qstr qst);
mp_obj_t mp_load_build_class(void);
void mp_store_name(qstr qst, mp_obj_t obj);
void mp_store_global(qstr qst, mp_obj_t obj);
void mp_delete_name(qstr qst);
void mp_delete_global(qstr qst);

mp_obj_t mp_unary_op(mp_unary_op_t op, mp_obj_t arg);
mp_obj_t mp_binary_op(mp_binary_op_t op, mp_obj_t lhs, mp_obj_t rhs);

mp_obj_t mp_call_function_0(mp_obj_t fun);
mp_obj_t mp_call_function_1(mp_obj_t fun, mp_obj_t arg);
mp_obj_t mp_call_function_2(mp_obj_t fun, mp_obj_t arg1, mp_obj_t arg2);
mp_obj_t mp_call_function_n_kw(mp_obj_t fun, size_t n_args, size_t n_kw, const mp_obj_t *args);
mp_obj_t mp_call_method_n_kw(size_t n_args, size_t n_kw, const mp_obj_t *args);
mp_obj_t mp_call_method_n_kw_var(_Bool have_self, size_t n_args_n_kw, const mp_obj_t *args);
mp_obj_t mp_call_method_self_n_kw(mp_obj_t meth, mp_obj_t self, size_t n_args, size_t n_kw, const mp_obj_t *args);


mp_obj_t mp_call_function_1_protected(mp_obj_t fun, mp_obj_t arg);
mp_obj_t mp_call_function_2_protected(mp_obj_t fun, mp_obj_t arg1, mp_obj_t arg2);

typedef struct _mp_call_args_t {
    mp_obj_t fun;
    size_t n_args, n_kw, n_alloc;
    mp_obj_t *args;
} mp_call_args_t;
# 140 "/Users/newworld/dev/uuos3/externals/micropython/py/runtime.h"
void mp_unpack_sequence(mp_obj_t seq, size_t num, mp_obj_t *items);
void mp_unpack_ex(mp_obj_t seq, size_t num, mp_obj_t *items);
mp_obj_t mp_store_map(mp_obj_t map, mp_obj_t key, mp_obj_t value);
mp_obj_t mp_load_attr(mp_obj_t base, qstr attr);
void mp_convert_member_lookup(mp_obj_t obj, const mp_obj_type_t *type, mp_obj_t member, mp_obj_t *dest);
void mp_load_method(mp_obj_t base, qstr attr, mp_obj_t *dest);
void mp_load_method_maybe(mp_obj_t base, qstr attr, mp_obj_t *dest);
void mp_load_method_protected(mp_obj_t obj, qstr attr, mp_obj_t *dest, _Bool catch_all_exc);
void mp_load_super_method(qstr attr, mp_obj_t *dest);
void mp_store_attr(mp_obj_t base, qstr attr, mp_obj_t val);

mp_obj_t mp_getiter(mp_obj_t o, mp_obj_iter_buf_t *iter_buf);
mp_obj_t mp_iternext_allow_raise(mp_obj_t o);
mp_obj_t mp_iternext(mp_obj_t o);
mp_vm_return_kind_t mp_resume(mp_obj_t self_in, mp_obj_t send_value, mp_obj_t throw_value, mp_obj_t *ret_val);

mp_obj_t mp_make_raise_obj(mp_obj_t o);

mp_obj_t mp_import_name(qstr name, mp_obj_t fromlist, mp_obj_t level);
mp_obj_t mp_import_from(mp_obj_t module, qstr name);
void mp_import_all(mp_obj_t module);


__attribute__((noreturn)) void mp_raise_msg(const mp_obj_type_t *exc_type, mp_rom_error_text_t msg);
__attribute__((noreturn)) void mp_raise_msg_varg(const mp_obj_type_t *exc_type, mp_rom_error_text_t fmt, ...);
__attribute__((noreturn)) void mp_raise_ValueError(mp_rom_error_text_t msg);
__attribute__((noreturn)) void mp_raise_TypeError(mp_rom_error_text_t msg);
__attribute__((noreturn)) void mp_raise_NotImplementedError(mp_rom_error_text_t msg);
__attribute__((noreturn)) void mp_raise_OSError(int errno_);
__attribute__((noreturn)) void mp_raise_recursion_depth(void);
# 182 "/Users/newworld/dev/uuos3/externals/micropython/py/runtime.h"
int mp_native_type_from_qstr(qstr qst);
mp_uint_t mp_native_from_obj(mp_obj_t obj, mp_uint_t type);
mp_obj_t mp_native_to_obj(mp_uint_t val, mp_uint_t type);






void mp_warning(const char *category, const char *msg, ...);





int setjmp_ex (jmp_buf);
_Noreturn void longjmp_ex (jmp_buf, int);
# 36 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c" 2

# 1 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.h" 1
# 29 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.h"
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/mpz.h" 1
# 64 "/Users/newworld/dev/uuos3/externals/micropython/py/mpz.h"
typedef uint16_t mpz_dig_t;
typedef uint32_t mpz_dbl_dig_t;
typedef int32_t mpz_dbl_dig_signed_t;
# 93 "/Users/newworld/dev/uuos3/externals/micropython/py/mpz.h"
typedef struct _mpz_t {
    size_t neg : 1;
    size_t fixed_dig : 1;
    size_t alloc : (8 * sizeof(size_t) - 2);
    size_t len;
    mpz_dig_t *dig;
} mpz_t;




void mpz_init_zero(mpz_t *z);
void mpz_init_from_int(mpz_t *z, mp_int_t val);
void mpz_init_fixed_from_int(mpz_t *z, mpz_dig_t *dig, size_t dig_alloc, mp_int_t val);
void mpz_deinit(mpz_t *z);

void mpz_set(mpz_t *dest, const mpz_t *src);
void mpz_set_from_int(mpz_t *z, mp_int_t src);
void mpz_set_from_ll(mpz_t *z, long long i, _Bool is_signed);

void mpz_set_from_float(mpz_t *z, mp_float_t src);

size_t mpz_set_from_str(mpz_t *z, const char *str, size_t len, _Bool neg, unsigned int base);
void mpz_set_from_bytes(mpz_t *z, _Bool big_endian, size_t len, const byte *buf);

static inline _Bool mpz_is_zero(const mpz_t *z) {
    return z->len == 0;
}
static inline _Bool mpz_is_neg(const mpz_t *z) {
    return z->len != 0 && z->neg != 0;
}
int mpz_cmp(const mpz_t *lhs, const mpz_t *rhs);

void mpz_abs_inpl(mpz_t *dest, const mpz_t *z);
void mpz_neg_inpl(mpz_t *dest, const mpz_t *z);
void mpz_not_inpl(mpz_t *dest, const mpz_t *z);
void mpz_shl_inpl(mpz_t *dest, const mpz_t *lhs, mp_uint_t rhs);
void mpz_shr_inpl(mpz_t *dest, const mpz_t *lhs, mp_uint_t rhs);
void mpz_add_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_sub_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_mul_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_pow_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_pow3_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs, const mpz_t *mod);
void mpz_and_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_or_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_xor_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_divmod_inpl(mpz_t *dest_quo, mpz_t *dest_rem, const mpz_t *lhs, const mpz_t *rhs);

static inline size_t mpz_max_num_bits(const mpz_t *z) {
    return z->len * (16);
}
mp_int_t mpz_hash(const mpz_t *z);
_Bool mpz_as_int_checked(const mpz_t *z, mp_int_t *value);
_Bool mpz_as_uint_checked(const mpz_t *z, mp_uint_t *value);
void mpz_as_bytes(const mpz_t *z, _Bool big_endian, size_t len, byte *buf);

mp_float_t mpz_as_float(const mpz_t *z);

size_t mpz_as_str_inpl(const mpz_t *z, unsigned int base, const char *prefix, char base_char, char comma, char *str);
# 30 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.h" 2


typedef struct _mp_obj_bigint_t {
    mp_obj_base_t base;
    mpz_t mpz;
} mp_obj_bigint_t;



size_t mp_int_format_size(size_t num_bits, int base, const char *prefix, char comma);

mp_obj_bigint_t *mp_obj_bigint_new_mpz(void);

void mp_obj_bigint_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
char *mp_obj_bigint_formatted(char **buf, size_t *buf_size, size_t *fmt_size, mp_const_obj_t self_in,
    int base, const char *prefix, char base_char, char comma);
char *mp_obj_bigint_formatted_impl(char **buf, size_t *buf_size, size_t *fmt_size, mp_const_obj_t self_in,
    int base, const char *prefix, char base_char, char comma);
mp_int_t mp_obj_bigint_hash(mp_obj_t self_in);
mp_obj_t mp_obj_bigint_from_bytes_impl(_Bool big_endian, size_t len, const byte *buf);
void mp_obj_bigint_to_bytes_impl(mp_obj_t self_in, _Bool big_endian, size_t len, byte *buf);
int mp_obj_bigint_sign(mp_obj_t self_in);
mp_obj_t mp_obj_bigint_unary_op(mp_unary_op_t op, mp_obj_t o_in);
mp_obj_t mp_obj_bigint_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in);
mp_obj_t mp_obj_bigint_pow3(mp_obj_t base, mp_obj_t exponent, mp_obj_t modulus);


mp_int_t mp_obj_bigint_get_checked(mp_const_obj_t self_in);
mp_obj_t mp_obj_new_bigint_from_ll(long long val);
mp_obj_t mp_obj_new_bigint_from_ull(unsigned long long val);


mp_obj_t mp_obj_bigint_binary_op_extra_cases(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in);
# 38 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c" 2


# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/math.h" 1
# 39 "/Users/newworld/dev/uuos3/contracts/include/libc/math.h"
int __fpclassify(double);
int __fpclassifyf(float);
int __fpclassifyl(long double);

static inline unsigned __FLOAT_BITS(float __f)
{
 union {float __f; unsigned __i;} __u;
 __u.__f = __f;
 return __u.__i;
}
static inline unsigned long long __DOUBLE_BITS(double __f)
{
 union {double __f; unsigned long long __i;} __u;
 __u.__f = __f;
 return __u.__i;
}
# 81 "/Users/newworld/dev/uuos3/contracts/include/libc/math.h"
int __signbit(double);
int __signbitf(float);
int __signbitl(long double);
# 96 "/Users/newworld/dev/uuos3/contracts/include/libc/math.h"
static inline int __islessf(float_t __x, float_t __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x < __y; }
static inline int __isless(double_t __x, double_t __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x < __y; }
static inline int __islessl(long double __x, long double __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x < __y; }
static inline int __islessequalf(float_t __x, float_t __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x <= __y; }
static inline int __islessequal(double_t __x, double_t __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x <= __y; }
static inline int __islessequall(long double __x, long double __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x <= __y; }
static inline int __islessgreaterf(float_t __x, float_t __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x != __y; }
static inline int __islessgreater(double_t __x, double_t __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x != __y; }
static inline int __islessgreaterl(long double __x, long double __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x != __y; }
static inline int __isgreaterf(float_t __x, float_t __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x > __y; }
static inline int __isgreater(double_t __x, double_t __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x > __y; }
static inline int __isgreaterl(long double __x, long double __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x > __y; }
static inline int __isgreaterequalf(float_t __x, float_t __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x >= __y; }
static inline int __isgreaterequal(double_t __x, double_t __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x >= __y; }
static inline int __isgreaterequall(long double __x, long double __y) { return !(( sizeof((__x)) == sizeof(float) ? (__FLOAT_BITS((__x)) & 0x7fffffff) > 0x7f800000 : sizeof((__x)) == sizeof(double) ? (__DOUBLE_BITS((__x)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__x)) == 0) ? ((void)(__y),1) : ( sizeof((__y)) == sizeof(float) ? (__FLOAT_BITS((__y)) & 0x7fffffff) > 0x7f800000 : sizeof((__y)) == sizeof(double) ? (__DOUBLE_BITS((__y)) & -1ULL>>1) > 0x7ffULL<<52 : __fpclassifyl((__y)) == 0)) && __x >= __y; }
# 123 "/Users/newworld/dev/uuos3/contracts/include/libc/math.h"
double acos(double);
float acosf(float);
long double acosl(long double);

double acosh(double);
float acoshf(float);
long double acoshl(long double);

double asin(double);
float asinf(float);
long double asinl(long double);

double asinh(double);
float asinhf(float);
long double asinhl(long double);

double atan(double);
float atanf(float);
long double atanl(long double);

double atan2(double, double);
float atan2f(float, float);
long double atan2l(long double, long double);

double atanh(double);
float atanhf(float);
long double atanhl(long double);

double cbrt(double);
float cbrtf(float);
long double cbrtl(long double);

double ceil(double);
float ceilf(float);
long double ceill(long double);

double copysign(double, double);
float copysignf(float, float);
long double copysignl(long double, long double);

double cos(double);
float cosf(float);
long double cosl(long double);

double cosh(double);
float coshf(float);
long double coshl(long double);

double erf(double);
float erff(float);
long double erfl(long double);

double erfc(double);
float erfcf(float);
long double erfcl(long double);

double exp(double);
float expf(float);
long double expl(long double);

double exp2(double);
float exp2f(float);
long double exp2l(long double);

double expm1(double);
float expm1f(float);
long double expm1l(long double);

double fabs(double);
float fabsf(float);
long double fabsl(long double);

double fdim(double, double);
float fdimf(float, float);
long double fdiml(long double, long double);

double floor(double);
float floorf(float);
long double floorl(long double);

double fma(double, double, double);



double fmax(double, double);
float fmaxf(float, float);
long double fmaxl(long double, long double);

double fmin(double, double);
float fminf(float, float);
long double fminl(long double, long double);

double fmod(double, double);
float fmodf(float, float);
long double fmodl(long double, long double);

double frexp(double, int *);
float frexpf(float, int *);
long double frexpl(long double, int *);

double hypot(double, double);
float hypotf(float, float);
long double hypotl(long double, long double);

int ilogb(double);
int ilogbf(float);
int ilogbl(long double);

double ldexp(double, int);
float ldexpf(float, int);
long double ldexpl(long double, int);

double lgamma(double);
float lgammaf(float);
long double lgammal(long double);

long long llrint(double);
long long llrintf(float);
long long llrintl(long double);

long long llround(double);
long long llroundf(float);
long long llroundl(long double);

double log(double);
float logf(float);
long double logl(long double);

double log10(double);
float log10f(float);
long double log10l(long double);

double log1p(double);
float log1pf(float);
long double log1pl(long double);

double log2(double);
float log2f(float);
long double log2l(long double);

double logb(double);
float logbf(float);
long double logbl(long double);

long lrint(double);
long lrintf(float);
long lrintl(long double);

long lround(double);
long lroundf(float);
long lroundl(long double);

double modf(double, double *);
float modff(float, float *);
long double modfl(long double, long double *);

double nan(const char *);
float nanf(const char *);
long double nanl(const char *);

double nearbyint(double);
float nearbyintf(float);
long double nearbyintl(long double);

double nextafter(double, double);
float nextafterf(float, float);
long double nextafterl(long double, long double);

double nexttoward(double, long double);
float nexttowardf(float, long double);
long double nexttowardl(long double, long double);

double pow(double, double);
float powf(float, float);
long double powl(long double, long double);

double remainder(double, double);
float remainderf(float, float);
long double remainderl(long double, long double);

double remquo(double, double, int *);
float remquof(float, float, int *);
long double remquol(long double, long double, int *);

double rint(double);
float rintf(float);
long double rintl(long double);

double round(double);
float roundf(float);
long double roundl(long double);

double scalbln(double, long);
float scalblnf(float, long);
long double scalblnl(long double, long);

double scalbn(double, int);
float scalbnf(float, int);
long double scalbnl(long double, int);

double sin(double);
float sinf(float);
long double sinl(long double);

double sinh(double);
float sinhf(float);
long double sinhl(long double);

double sqrt(double);
float sqrtf(float);
long double sqrtl(long double);

double tan(double);
float tanf(float);
long double tanl(long double);

double tanh(double);
float tanhf(float);
long double tanhl(long double);

double tgamma(double);
float tgammaf(float);
long double tgammal(long double);

double trunc(double);
float truncf(float);
long double truncl(long double);
# 372 "/Users/newworld/dev/uuos3/contracts/include/libc/math.h"
extern int signgam;

double j0(double);
double j1(double);
double jn(int, double);

double y0(double);
double y1(double);
double yn(int, double);





double drem(double, double);
float dremf(float, float);

int finite(double);
int finitef(float);

double scalb(double, double);
float scalbf(float, float);

double significand(double);
float significandf(float);

double lgamma_r(double, int*);
float lgammaf_r(float, int*);

float j0f(float);
float j1f(float);
float jnf(int, float);

float y0f(float);
float y1f(float);
float ynf(int, float);
# 41 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c" 2






static const mpz_dig_t maxsize_dig[] = {

    ((0x7fffffff) >> (16) * 0) & ((1L << (16)) - 1),



     ((0x7fffffff) >> (16) * 1) & ((1L << (16)) - 1),
# 68 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c"
};
# 80 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c"
mp_obj_bigint_t *mp_obj_bigint_new_mpz(void) {
    mp_obj_bigint_t *o = (((mp_obj_bigint_t *)(m_malloc(sizeof(mp_obj_bigint_t) * (1)))));
    o->base.type = &mp_type_int;
    mpz_init_zero(&o->mpz);
    return o;
}
# 96 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c"
char *mp_obj_bigint_formatted_impl(char **buf, size_t *buf_size, size_t *fmt_size, mp_const_obj_t self_in,
    int base, const char *prefix, char base_char, char comma) {
    (void)0;
    const mp_obj_bigint_t *self = ((void *)self_in);

    size_t needed_size = mp_bigint_format_size(mpz_max_num_bits(&self->mpz), base, prefix, comma);
    if (needed_size > *buf_size) {
        *buf = ((char *)(m_malloc(sizeof(char) * (needed_size))));
        *buf_size = needed_size;
    }
    char *str = *buf;

    *fmt_size = mpz_as_str_inpl(&self->mpz, base, prefix, base_char, comma, str);

    return str;
}

mp_obj_t mp_obj_bigint_from_bytes_impl(_Bool big_endian, size_t len, const byte *buf) {
    mp_obj_bigint_t *o = mp_obj_bigint_new_mpz();
    mpz_set_from_bytes(&o->mpz, big_endian, len, buf);
    return ((mp_obj_t)o);
}

void mp_obj_bigint_to_bytes_impl(mp_obj_t self_in, _Bool big_endian, size_t len, byte *buf) {
    (void)0;
    mp_obj_bigint_t *self = ((void *)self_in);
    memset(buf, 0, len);
    mpz_as_bytes(&self->mpz, big_endian, len, buf);
}

int mp_obj_bigint_sign(mp_obj_t self_in) {
    if (mp_obj_is_small_int(self_in)) {
        mp_int_t val = (((mp_int_t)(self_in)) >> 1);
        if (val < 0) {
            return -1;
        } else if (val > 0) {
            return 1;
        } else {
            return 0;
        }
    }
    mp_obj_bigint_t *self = ((void *)self_in);
    if (self->mpz.len == 0) {
        return 0;
    } else if (self->mpz.neg == 0) {
        return 1;
    } else {
        return -1;
    }
}

mp_obj_t mp_obj_bigint_unary_op(mp_unary_op_t op, mp_obj_t o_in) {
    mp_obj_bigint_t *o = ((void *)o_in);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(!mpz_is_zero(&o->mpz));
        case MP_UNARY_OP_HASH:
            return ((mp_obj_t)((((mp_uint_t)(mpz_hash(&o->mpz))) << 1) | 1));
        case MP_UNARY_OP_POSITIVE:
            return o_in;
        case MP_UNARY_OP_NEGATIVE: { mp_obj_bigint_t *o2 = mp_obj_bigint_new_mpz();
                                     mpz_neg_inpl(&o2->mpz, &o->mpz);
                                     return ((mp_obj_t)o2);
        }
        case MP_UNARY_OP_INVERT: { mp_obj_bigint_t *o2 = mp_obj_bigint_new_mpz();
                                   mpz_not_inpl(&o2->mpz, &o->mpz);
                                   return ((mp_obj_t)o2);
        }
        case MP_UNARY_OP_ABS: {
            mp_obj_bigint_t *self = ((void *)o_in);
            if (self->mpz.neg == 0) {
                return o_in;
            }
            mp_obj_bigint_t *self2 = mp_obj_bigint_new_mpz();
            mpz_abs_inpl(&self2->mpz, &self->mpz);
            return ((mp_obj_t)self2);
        }
        default:
            return (((mp_obj_t)(void *)0));
    }
}

mp_obj_t mp_obj_bigint_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    const mpz_t *zlhs;
    const mpz_t *zrhs;
    mpz_t z_int;
    mpz_dig_t z_int_dig[((sizeof(mp_int_t) * 8 + (16) - 1) / (16))];


    if (mp_obj_is_small_int(lhs_in)) {
        mpz_init_fixed_from_int(&z_int, z_int_dig, ((sizeof(mp_int_t) * 8 + (16) - 1) / (16)), (((mp_int_t)(lhs_in)) >> 1));
        zlhs = &z_int;
    } else {
        (void)0;
        zlhs = &((mp_obj_bigint_t *)((void *)lhs_in))->mpz;
    }


    if (mp_obj_is_small_int(rhs_in)) {
        mpz_init_fixed_from_int(&z_int, z_int_dig, ((sizeof(mp_int_t) * 8 + (16) - 1) / (16)), (((mp_int_t)(rhs_in)) >> 1));
        zrhs = &z_int;
    } else if ((mp_obj_is_obj(rhs_in) && (((mp_obj_base_t *)((void *)rhs_in))->type == (&mp_type_int)))) {
        zrhs = &((mp_obj_bigint_t *)((void *)rhs_in))->mpz;

    } else if ((mp_obj_is_obj((rhs_in)) && (((mp_obj_base_t *)((void *)(rhs_in)))->type == (&mp_type_float)))) {
        return mp_obj_float_binary_op(op, mpz_as_float(zlhs), rhs_in);


    } else if ((mp_obj_is_obj(rhs_in) && (((mp_obj_base_t *)((void *)rhs_in))->type == (&mp_type_complex)))) {
        return mp_obj_complex_binary_op(op, mpz_as_float(zlhs), 0, rhs_in);

    } else {

        return mp_obj_bigint_binary_op_extra_cases(op, lhs_in, rhs_in);
    }


    if (op == MP_BINARY_OP_TRUE_DIVIDE || op == MP_BINARY_OP_INPLACE_TRUE_DIVIDE) {
        if (mpz_is_zero(zrhs)) {
            goto zero_division_error;
        }
        mp_float_t flhs = mpz_as_float(zlhs);
        mp_float_t frhs = mpz_as_float(zrhs);
        return mp_obj_new_float(flhs / frhs);
    }


    if (op >= MP_BINARY_OP_INPLACE_OR && op < MP_BINARY_OP_CONTAINS) {
        mp_obj_bigint_t *res = mp_obj_bigint_new_mpz();

        switch (op) {
            case MP_BINARY_OP_ADD:
            case MP_BINARY_OP_INPLACE_ADD:
                mpz_add_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_SUBTRACT:
            case MP_BINARY_OP_INPLACE_SUBTRACT:
                mpz_sub_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_MULTIPLY:
            case MP_BINARY_OP_INPLACE_MULTIPLY:
                mpz_mul_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_FLOOR_DIVIDE:
            case MP_BINARY_OP_INPLACE_FLOOR_DIVIDE: {
                if (mpz_is_zero(zrhs)) {
                zero_division_error:
                    mp_raise_msg(&mp_type_ZeroDivisionError, (mp_rom_error_text_t)"divide by zero");
                }
                mpz_t rem;
                mpz_init_zero(&rem);
                mpz_divmod_inpl(&res->mpz, &rem, zlhs, zrhs);
                mpz_deinit(&rem);
                break;
            }
            case MP_BINARY_OP_MODULO:
            case MP_BINARY_OP_INPLACE_MODULO: {
                if (mpz_is_zero(zrhs)) {
                    goto zero_division_error;
                }
                mpz_t quo;
                mpz_init_zero(&quo);
                mpz_divmod_inpl(&quo, &res->mpz, zlhs, zrhs);
                mpz_deinit(&quo);
                break;
            }

            case MP_BINARY_OP_AND:
            case MP_BINARY_OP_INPLACE_AND:
                mpz_and_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_OR:
            case MP_BINARY_OP_INPLACE_OR:
                mpz_or_inpl(&res->mpz, zlhs, zrhs);
                break;
            case MP_BINARY_OP_XOR:
            case MP_BINARY_OP_INPLACE_XOR:
                mpz_xor_inpl(&res->mpz, zlhs, zrhs);
                break;

            case MP_BINARY_OP_LSHIFT:
            case MP_BINARY_OP_INPLACE_LSHIFT:
            case MP_BINARY_OP_RSHIFT:
            case MP_BINARY_OP_INPLACE_RSHIFT: {
                mp_int_t irhs = mp_obj_bigint_get_checked(rhs_in);
                if (irhs < 0) {
                    mp_raise_ValueError((mp_rom_error_text_t)"negative shift count");
                }
                if (op == MP_BINARY_OP_LSHIFT || op == MP_BINARY_OP_INPLACE_LSHIFT) {
                    mpz_shl_inpl(&res->mpz, zlhs, irhs);
                } else {
                    mpz_shr_inpl(&res->mpz, zlhs, irhs);
                }
                break;
            }

            case MP_BINARY_OP_POWER:
            case MP_BINARY_OP_INPLACE_POWER:
                if (mpz_is_neg(zrhs)) {

                    return mp_obj_float_binary_op(op, mpz_as_float(zlhs), rhs_in);



                }
                mpz_pow_inpl(&res->mpz, zlhs, zrhs);
                break;

            default: {
                (void)0;
                if (mpz_is_zero(zrhs)) {
                    goto zero_division_error;
                }
                mp_obj_bigint_t *quo = mp_obj_bigint_new_mpz();
                mpz_divmod_inpl(&quo->mpz, &res->mpz, zlhs, zrhs);
                mp_obj_t tuple[2] = {((mp_obj_t)quo), ((mp_obj_t)res)};
                return mp_obj_new_tuple(2, tuple);
            }
        }

        return ((mp_obj_t)res);

    } else {
        int cmp = mpz_cmp(zlhs, zrhs);
        switch (op) {
            case MP_BINARY_OP_LESS:
                return mp_obj_new_bool(cmp < 0);
            case MP_BINARY_OP_MORE:
                return mp_obj_new_bool(cmp > 0);
            case MP_BINARY_OP_LESS_EQUAL:
                return mp_obj_new_bool(cmp <= 0);
            case MP_BINARY_OP_MORE_EQUAL:
                return mp_obj_new_bool(cmp >= 0);
            case MP_BINARY_OP_EQUAL:
                return mp_obj_new_bool(cmp == 0);

            default:
                return (((mp_obj_t)(void *)0));
        }
    }
}


static mpz_t *mp_mpz_for_int(mp_obj_t arg, mpz_t *temp) {
    if (mp_obj_is_small_int(arg)) {
        mpz_init_from_int(temp, (((mp_int_t)(arg)) >> 1));
        return temp;
    } else {
        mp_obj_bigint_t *arp_p = ((void *)arg);
        return &(arp_p->mpz);
    }
}

mp_obj_t mp_obj_bigint_pow3(mp_obj_t base, mp_obj_t exponent, mp_obj_t modulus) {
    if (!(mp_obj_is_small_int(base) || (mp_obj_is_obj(base) && (((mp_obj_base_t *)((void *)base))->type == (&mp_type_int)))) || !(mp_obj_is_small_int(exponent) || (mp_obj_is_obj(exponent) && (((mp_obj_base_t *)((void *)exponent))->type == (&mp_type_int)))) || !(mp_obj_is_small_int(modulus) || (mp_obj_is_obj(modulus) && (((mp_obj_base_t *)((void *)modulus))->type == (&mp_type_int))))) {
        mp_raise_TypeError((mp_rom_error_text_t)"pow() with 3 arguments requires integers");
    } else {
        mp_obj_t result = mp_obj_new_bigint_from_ull(0);
        mp_obj_bigint_t *res_p = (mp_obj_bigint_t *)((void *)result);

        mpz_t l_temp, r_temp, m_temp;
        mpz_t *lhs = mp_mpz_for_int(base, &l_temp);
        mpz_t *rhs = mp_mpz_for_int(exponent, &r_temp);
        mpz_t *mod = mp_mpz_for_int(modulus, &m_temp);

        mpz_pow3_inpl(&(res_p->mpz), lhs, rhs, mod);

        if (lhs == &l_temp) {
            mpz_deinit(lhs);
        }
        if (rhs == &r_temp) {
            mpz_deinit(rhs);
        }
        if (mod == &m_temp) {
            mpz_deinit(mod);
        }
        return result;
    }
}


mp_obj_t mp_obj_new_bigint(mp_int_t value) {
    if (((((value) ^ ((value) << 1)) & (((mp_uint_t)1) << ((sizeof(mp_uint_t)) * 8 - 1))) == 0)) {
        return ((mp_obj_t)((((mp_uint_t)(value)) << 1) | 1));
    }
    return mp_obj_new_bigint_from_ll(value);
}

mp_obj_t mp_obj_new_bigint_from_ll(long long val) {
    mp_obj_bigint_t *o = mp_obj_bigint_new_mpz();
    mpz_set_from_ll(&o->mpz, val, 1);
    return ((mp_obj_t)o);
}

mp_obj_t mp_obj_new_bigint_from_ull(unsigned long long val) {
    mp_obj_bigint_t *o = mp_obj_bigint_new_mpz();
    mpz_set_from_ll(&o->mpz, val, 0);
    return ((mp_obj_t)o);
}

mp_obj_t mp_obj_new_bigint_from_uint(mp_uint_t value) {


    if ((value & ~~((((mp_uint_t)1) << ((sizeof(mp_uint_t)) * 8 - 1)) | ((((mp_uint_t)1) << ((sizeof(mp_uint_t)) * 8 - 1)) >> 1))) == 0) {
        return ((mp_obj_t)((((mp_uint_t)(value)) << 1) | 1));
    }
    return mp_obj_new_bigint_from_ull(value);
}

mp_obj_t mp_obj_new_bigint_from_str_len(const char **str, size_t len, _Bool neg, unsigned int base) {
    mp_obj_bigint_t *o = mp_obj_bigint_new_mpz();
    size_t n = mpz_set_from_str(&o->mpz, *str, len, neg, base);
    *str += n;
    return ((mp_obj_t)o);
}

mp_int_t mp_obj_bigint_get_truncated(mp_const_obj_t self_in) {
    if (mp_obj_is_small_int(self_in)) {
        return (((mp_int_t)(self_in)) >> 1);
    } else {
        const mp_obj_bigint_t *self = ((void *)self_in);

        return mpz_hash(&self->mpz);
    }
}

mp_int_t mp_obj_bigint_get_checked(mp_const_obj_t self_in) {
    if (mp_obj_is_small_int(self_in)) {
        return (((mp_int_t)(self_in)) >> 1);
    } else {
        const mp_obj_bigint_t *self = ((void *)self_in);
        mp_int_t value;
        if (mpz_as_int_checked(&self->mpz, &value)) {
            return value;
        } else {

            mp_raise_msg(&mp_type_OverflowError, (mp_rom_error_text_t)"overflow converting long int to machine word");
        }
    }
}

mp_uint_t mp_obj_bigint_get_uint_checked(mp_const_obj_t self_in) {
    if (mp_obj_is_small_int(self_in)) {
        if ((((mp_int_t)(self_in)) >> 1) >= 0) {
            return (((mp_int_t)(self_in)) >> 1);
        }
    } else {
        const mp_obj_bigint_t *self = ((void *)self_in);
        mp_uint_t value;
        if (mpz_as_uint_checked(&self->mpz, &value)) {
            return value;
        }
    }

    mp_raise_msg(&mp_type_OverflowError, (mp_rom_error_text_t)"overflow converting long int to machine word");
}
# 460 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c"
# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/stdlib.h" 1
# 21 "/Users/newworld/dev/uuos3/contracts/include/libc/stdlib.h"
int atoi (const char *);
long atol (const char *);
long long atoll (const char *);
double atof (const char *);

float strtof (const char *restrict, char **restrict);
double strtod (const char *restrict, char **restrict);
long double strtold (const char *restrict, char **restrict);

long strtol (const char *restrict, char **restrict, int);
unsigned long strtoul (const char *restrict, char **restrict, int);
long long strtoll (const char *restrict, char **restrict, int);
unsigned long long strtoull (const char *restrict, char **restrict, int);

void *malloc (size_t);
void *calloc (size_t, size_t);
void *realloc (void *, size_t);
void free (void *);

void *aligned_alloc(size_t, size_t);
_Noreturn void abort (void);
int atexit (void (*) (void));
_Noreturn void exit (int);
_Noreturn void _Exit (int);
int at_quick_exit (void (*) (void));
_Noreturn void quick_exit (int);

char *getenv (const char *);

int system (const char *);

void *bsearch (const void *, const void *, size_t, size_t, int (*)(const void *, const void *));
void qsort (void *, size_t, size_t, int (*)(const void *, const void *));

int abs (int);
long labs (long);
long long llabs (long long);

typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;
typedef struct { long long quot, rem; } lldiv_t;

div_t div (int, int);
ldiv_t ldiv (long, long);
lldiv_t lldiv (long long, long long);

int mblen (const char *, size_t);
int mbtowc (wchar_t *restrict, const char *restrict, size_t);
int wctomb (char *, wchar_t);
size_t mbstowcs (wchar_t *restrict, const char *restrict, size_t);
size_t wcstombs (char *restrict, const wchar_t *restrict, size_t);




size_t __ctype_get_mb_cur_max(void);
# 86 "/Users/newworld/dev/uuos3/contracts/include/libc/stdlib.h"
int posix_memalign (void **, size_t, size_t);
int setenv (const char *, const char *, int);
int unsetenv (const char *);
int getsubopt (char **, char *const *, char **);





int putenv (char *);




int clearenv(void);
# 461 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c" 2
# 1 "/Users/newworld/dev/uuos3/contracts/include/libc/assert.h" 1
# 19 "/Users/newworld/dev/uuos3/contracts/include/libc/assert.h"
_Noreturn void __assert_fail (const char *, const char *, int, const char *);
# 462 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c" 2


# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/parsenum.h" 1
# 30 "/Users/newworld/dev/uuos3/externals/micropython/py/parsenum.h"
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/lexer.h" 1
# 33 "/Users/newworld/dev/uuos3/externals/micropython/py/lexer.h"
# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/reader.h" 1
# 36 "/Users/newworld/dev/uuos3/externals/micropython/py/reader.h"
typedef struct _mp_reader_t {
    void *data;
    mp_uint_t (*readbyte)(void *data);
    void (*close)(void *data);
} mp_reader_t;

void mp_reader_new_mem(mp_reader_t *reader, const byte *buf, size_t len, size_t free_len);
void mp_reader_new_file(mp_reader_t *reader, const char *filename);
void mp_reader_new_file_from_fd(mp_reader_t *reader, int fd, _Bool close_fd);
# 34 "/Users/newworld/dev/uuos3/externals/micropython/py/lexer.h" 2







typedef enum _mp_token_kind_t {
    MP_TOKEN_END,

    MP_TOKEN_INVALID,
    MP_TOKEN_DEDENT_MISMATCH,
    MP_TOKEN_LONELY_STRING_OPEN,

    MP_TOKEN_NEWLINE,
    MP_TOKEN_INDENT,
    MP_TOKEN_DEDENT,

    MP_TOKEN_NAME,
    MP_TOKEN_INTEGER,
    MP_TOKEN_FLOAT_OR_IMAG,
    MP_TOKEN_STRING,
    MP_TOKEN_BYTES,

    MP_TOKEN_ELLIPSIS,

    MP_TOKEN_KW_FALSE,
    MP_TOKEN_KW_NONE,
    MP_TOKEN_KW_TRUE,
    MP_TOKEN_KW___DEBUG__,
    MP_TOKEN_KW_AND,
    MP_TOKEN_KW_AS,
    MP_TOKEN_KW_ASSERT,

    MP_TOKEN_KW_ASYNC,
    MP_TOKEN_KW_AWAIT,

    MP_TOKEN_KW_BREAK,
    MP_TOKEN_KW_CLASS,
    MP_TOKEN_KW_CONTINUE,
    MP_TOKEN_KW_DEF,
    MP_TOKEN_KW_DEL,
    MP_TOKEN_KW_ELIF,
    MP_TOKEN_KW_ELSE,
    MP_TOKEN_KW_EXCEPT,
    MP_TOKEN_KW_FINALLY,
    MP_TOKEN_KW_FOR,
    MP_TOKEN_KW_FROM,
    MP_TOKEN_KW_GLOBAL,
    MP_TOKEN_KW_IF,
    MP_TOKEN_KW_IMPORT,
    MP_TOKEN_KW_IN,
    MP_TOKEN_KW_IS,
    MP_TOKEN_KW_LAMBDA,
    MP_TOKEN_KW_NONLOCAL,
    MP_TOKEN_KW_NOT,
    MP_TOKEN_KW_OR,
    MP_TOKEN_KW_PASS,
    MP_TOKEN_KW_RAISE,
    MP_TOKEN_KW_RETURN,
    MP_TOKEN_KW_TRY,
    MP_TOKEN_KW_WHILE,
    MP_TOKEN_KW_WITH,
    MP_TOKEN_KW_YIELD,

    MP_TOKEN_OP_ASSIGN,
    MP_TOKEN_OP_TILDE,


    MP_TOKEN_OP_LESS,
    MP_TOKEN_OP_MORE,
    MP_TOKEN_OP_DBL_EQUAL,
    MP_TOKEN_OP_LESS_EQUAL,
    MP_TOKEN_OP_MORE_EQUAL,
    MP_TOKEN_OP_NOT_EQUAL,


    MP_TOKEN_OP_PIPE,
    MP_TOKEN_OP_CARET,
    MP_TOKEN_OP_AMPERSAND,
    MP_TOKEN_OP_DBL_LESS,
    MP_TOKEN_OP_DBL_MORE,
    MP_TOKEN_OP_PLUS,
    MP_TOKEN_OP_MINUS,
    MP_TOKEN_OP_STAR,
    MP_TOKEN_OP_AT,
    MP_TOKEN_OP_DBL_SLASH,
    MP_TOKEN_OP_SLASH,
    MP_TOKEN_OP_PERCENT,
    MP_TOKEN_OP_DBL_STAR,


    MP_TOKEN_DEL_PIPE_EQUAL,
    MP_TOKEN_DEL_CARET_EQUAL,
    MP_TOKEN_DEL_AMPERSAND_EQUAL,
    MP_TOKEN_DEL_DBL_LESS_EQUAL,
    MP_TOKEN_DEL_DBL_MORE_EQUAL,
    MP_TOKEN_DEL_PLUS_EQUAL,
    MP_TOKEN_DEL_MINUS_EQUAL,
    MP_TOKEN_DEL_STAR_EQUAL,
    MP_TOKEN_DEL_AT_EQUAL,
    MP_TOKEN_DEL_DBL_SLASH_EQUAL,
    MP_TOKEN_DEL_SLASH_EQUAL,
    MP_TOKEN_DEL_PERCENT_EQUAL,
    MP_TOKEN_DEL_DBL_STAR_EQUAL,

    MP_TOKEN_DEL_PAREN_OPEN,
    MP_TOKEN_DEL_PAREN_CLOSE,
    MP_TOKEN_DEL_BRACKET_OPEN,
    MP_TOKEN_DEL_BRACKET_CLOSE,
    MP_TOKEN_DEL_BRACE_OPEN,
    MP_TOKEN_DEL_BRACE_CLOSE,
    MP_TOKEN_DEL_COMMA,
    MP_TOKEN_DEL_COLON,
    MP_TOKEN_DEL_PERIOD,
    MP_TOKEN_DEL_SEMICOLON,
    MP_TOKEN_DEL_EQUAL,
    MP_TOKEN_DEL_MINUS_MORE,
} mp_token_kind_t;



typedef struct _mp_lexer_t {
    qstr source_name;
    mp_reader_t reader;

    unichar chr0, chr1, chr2;

    size_t line;
    size_t column;

    mp_int_t emit_dent;
    mp_int_t nested_bracket_level;

    size_t alloc_indent_level;
    size_t num_indent_level;
    uint16_t *indent_level;

    size_t tok_line;
    size_t tok_column;
    mp_token_kind_t tok_kind;
    vstr_t vstr;
} mp_lexer_t;

mp_lexer_t *mp_lexer_new(qstr src_name, mp_reader_t reader);
mp_lexer_t *mp_lexer_new_from_str_len(qstr src_name, const char *str, size_t len, size_t free_len);

void mp_lexer_free(mp_lexer_t *lex);
void mp_lexer_to_next(mp_lexer_t *lex);





typedef enum {
    MP_IMPORT_STAT_NO_EXIST,
    MP_IMPORT_STAT_DIR,
    MP_IMPORT_STAT_FILE,
} mp_import_stat_t;

mp_import_stat_t mp_import_stat(const char *path);
mp_lexer_t *mp_lexer_new_from_file(const char *filename);
# 31 "/Users/newworld/dev/uuos3/externals/micropython/py/parsenum.h" 2



mp_obj_t mp_parse_num_integer(const char *restrict str, size_t len, int base, mp_lexer_t *lex);
mp_obj_t mp_parse_num_decimal(const char *str, size_t len, _Bool allow_imag, _Bool force_complex, mp_lexer_t *lex);
# 465 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c" 2


# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/objstr.h" 1
# 31 "/Users/newworld/dev/uuos3/externals/micropython/py/objstr.h"
typedef struct _mp_obj_str_t {
    mp_obj_base_t base;
    mp_uint_t hash;

    size_t len;
    const byte *data;
} mp_obj_str_t;
# 64 "/Users/newworld/dev/uuos3/externals/micropython/py/objstr.h"
mp_obj_t mp_obj_str_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args);
void mp_str_print_json(const mp_print_t *print, const byte *str_data, size_t str_len);
mp_obj_t mp_obj_str_format(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs);
mp_obj_t mp_obj_str_split(size_t n_args, const mp_obj_t *args);
mp_obj_t mp_obj_new_str_copy(const mp_obj_type_t *type, const byte *data, size_t len);
mp_obj_t mp_obj_new_str_of_type(const mp_obj_type_t *type, const byte *data, size_t len);

mp_obj_t mp_obj_str_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in);
mp_int_t mp_obj_str_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags);

const byte *str_index_to_ptr(const mp_obj_type_t *type, const byte *self_data, size_t self_len,
    mp_obj_t index, _Bool is_slice);
const byte *find_subbytes(const byte *haystack, size_t hlen, const byte *needle, size_t nlen, int direction);

extern const mp_obj_fun_builtin_var_t str_encode_obj;
extern const mp_obj_fun_builtin_var_t str_find_obj;
extern const mp_obj_fun_builtin_var_t str_rfind_obj;
extern const mp_obj_fun_builtin_var_t str_index_obj;
extern const mp_obj_fun_builtin_var_t str_rindex_obj;
extern const mp_obj_fun_builtin_fixed_t str_join_obj;
extern const mp_obj_fun_builtin_var_t str_split_obj;
extern const mp_obj_fun_builtin_var_t str_splitlines_obj;
extern const mp_obj_fun_builtin_var_t str_rsplit_obj;
extern const mp_obj_fun_builtin_var_t str_startswith_obj;
extern const mp_obj_fun_builtin_var_t str_endswith_obj;
extern const mp_obj_fun_builtin_var_t str_strip_obj;
extern const mp_obj_fun_builtin_var_t str_lstrip_obj;
extern const mp_obj_fun_builtin_var_t str_rstrip_obj;
extern const mp_obj_fun_builtin_var_t str_format_obj;
extern const mp_obj_fun_builtin_var_t str_replace_obj;
extern const mp_obj_fun_builtin_var_t str_count_obj;
extern const mp_obj_fun_builtin_fixed_t str_partition_obj;
extern const mp_obj_fun_builtin_fixed_t str_rpartition_obj;
extern const mp_obj_fun_builtin_fixed_t str_center_obj;
extern const mp_obj_fun_builtin_fixed_t str_lower_obj;
extern const mp_obj_fun_builtin_fixed_t str_upper_obj;
extern const mp_obj_fun_builtin_fixed_t str_isspace_obj;
extern const mp_obj_fun_builtin_fixed_t str_isalpha_obj;
extern const mp_obj_fun_builtin_fixed_t str_isdigit_obj;
extern const mp_obj_fun_builtin_fixed_t str_isupper_obj;
extern const mp_obj_fun_builtin_fixed_t str_islower_obj;
extern const mp_obj_fun_builtin_var_t bytes_decode_obj;
# 468 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c" 2

# 1 "/Users/newworld/dev/uuos3/externals/micropython/py/binary.h" 1
# 37 "/Users/newworld/dev/uuos3/externals/micropython/py/binary.h"
size_t mp_binary_get_size(char struct_type, char val_type, size_t *palign);
mp_obj_t mp_binary_get_val_array(char typecode, void *p, size_t index);
void mp_binary_set_val_array(char typecode, void *p, size_t index, mp_obj_t val_in);
void mp_binary_set_val_array_from_int(char typecode, void *p, size_t index, mp_int_t val);
mp_obj_t mp_binary_get_val(char struct_type, char val_type, byte *p_base, byte **ptr);
void mp_binary_set_val(char struct_type, char val_type, mp_obj_t val_in, byte *p_base, byte **ptr);
long long mp_binary_get_int(size_t size, _Bool is_signed, _Bool big_endian, const byte *src);
void mp_binary_set_int(size_t val_sz, _Bool big_endian, byte *dest, mp_uint_t val);
# 470 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c" 2







static mp_obj_t mp_obj_bigint_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    mp_arg_check_num(n_args, n_kw, 0, 2, 0);

    switch (n_args) {
        case 0:
            return ((mp_obj_t)((((mp_uint_t)(0)) << 1) | 1));

        case 1:
            if ((mp_obj_is_small_int(args[0]) || (mp_obj_is_obj(args[0]) && (((mp_obj_base_t *)((void *)args[0]))->type == (&mp_type_int))))) {

                return args[0];
            }







            else if ((mp_obj_is_qstr(args[0]) || (mp_obj_is_obj(args[0]) && ((mp_obj_base_t *)((void *)args[0]))->type->binary_op == mp_obj_str_binary_op))) {

                size_t l;
                const char *s = mp_obj_str_get_data(args[0], &l);
                return mp_parse_num_integer(s, l, 0, ((void*)0));

            } else if ((mp_obj_is_obj((args[0])) && (((mp_obj_base_t *)((void *)(args[0])))->type == (&mp_type_float)))) {
                return mp_obj_new_int_from_float(mp_obj_float_get(args[0]));

            } else {
                return mp_unary_op(MP_UNARY_OP_INT, args[0]);
            }

        case 2:
        default: {

            size_t l;
            const char *s = mp_obj_str_get_data(args[0], &l);
            return mp_parse_num_integer(s, l, mp_obj_get_int(args[1]), ((void*)0));
        }
    }
}



typedef enum {
    MP_FP_CLASS_FIT_SMALLINT,
    MP_FP_CLASS_FIT_LONGINT,
    MP_FP_CLASS_OVERFLOW
} mp_fp_as_int_class_t;

static mp_fp_as_int_class_t mp_classify_fp_as_int(mp_float_t val) {
    union {
        mp_float_t f;



        uint32_t i[2];

    } u = {val};

    uint32_t e;



    e = u.i[(1)];




    if (e & (1U << (((52) + (11)) % 32))) {

        e |= u.i[(!(1))] != 0;

        if ((e & ~(1 << (((52) + (11)) % 32))) == 0) {

            e = 0;
        } else {
            e += ((1 << (11)) - 1) << ((52) % 32);
        }
    } else {
        e &= ~((1 << ((52) % 32)) - 1);
    }


    if (e <= ((8 * sizeof(uintptr_t) + ((1 << ((11) - 1)) - 1) - 3) << ((52) % 32))) {
        return MP_FP_CLASS_FIT_SMALLINT;
    }






    return MP_FP_CLASS_FIT_LONGINT;



}
# 617 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c"
typedef mp_int_t fmt_int_t;
typedef mp_uint_t fmt_uint_t;


void mp_obj_bigint_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;


    char stack_buf[sizeof(fmt_int_t) * 4];
    char *buf = stack_buf;
    size_t buf_size = sizeof(stack_buf);
    size_t fmt_size;

    char *str = mp_obj_bigint_formatted(&buf, &buf_size, &fmt_size, self_in, 10, ((void*)0), '\0', '\0');
    mp_print_str(print, str);

    if (buf != stack_buf) {
        ((void)(buf_size), m_free(buf));
    }
}

static const uint8_t log_base2_floor[] = {
    0, 1, 1, 2,
    2, 2, 2, 3,
    3, 3, 3, 3,
    3, 3, 3, 4,






};

size_t mp_bigint_format_size(size_t num_bits, int base, const char *prefix, char comma) {
    (void)0;
    size_t num_digits = num_bits / log_base2_floor[base - 1] + 1;
    size_t num_commas = comma ? num_digits / 3 : 0;
    size_t prefix_len = prefix ? strlen(prefix) : 0;
    return num_digits + num_commas + prefix_len + 2;
}
# 666 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c"
char *mp_obj_bigint_formatted(char **buf, size_t *buf_size, size_t *fmt_size, mp_const_obj_t self_in,
    int base, const char *prefix, char base_char, char comma) {
    fmt_int_t num;




    if (mp_obj_is_small_int(self_in)) {

        num = (((mp_int_t)(self_in)) >> 1);
    } else {
        (void)0;







        return mp_obj_bigint_formatted_impl(buf, buf_size, fmt_size, self_in, base, prefix, base_char, comma);

    }


    char sign = '\0';
    if (num < 0) {
        num = -num;
        sign = '-';
    }

    size_t needed_size = mp_bigint_format_size(sizeof(fmt_int_t) * 8, base, prefix, comma);
    if (needed_size > *buf_size) {
        *buf = ((char *)(m_malloc(sizeof(char) * (needed_size))));
        *buf_size = needed_size;
    }
    char *str = *buf;

    char *b = str + needed_size;
    *(--b) = '\0';
    char *last_comma = b;

    if (num == 0) {
        *(--b) = '0';
    } else {
        do {

            int c = (fmt_uint_t)num % base;
            num = (fmt_uint_t)num / base;
            if (c >= 10) {
                c += base_char - 10;
            } else {
                c += '0';
            }
            *(--b) = c;
            if (comma && num != 0 && b > str && (last_comma - b) == 3) {
                *(--b) = comma;
                last_comma = b;
            }
        }
        while (b > str && num != 0);
    }
    if (prefix) {
        size_t prefix_len = strlen(prefix);
        char *p = b - prefix_len;
        if (p > str) {
            b = p;
            while (*prefix) {
                *p++ = *prefix++;
            }
        }
    }
    if (sign && b > str) {
        *(--b) = sign;
    }
    *fmt_size = *buf + needed_size - b - 1;

    return b;
}
# 816 "/Users/newworld/dev/uuos3/externals/micropython/extmod/uuosio/objbigint.c"
mp_obj_t mp_obj_bigint_binary_op_extra_cases(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    if (rhs_in == ((mp_obj_t)(((1) << 3) | 6))) {

        return mp_binary_op(op, lhs_in, ((mp_obj_t)((((mp_uint_t)(0)) << 1) | 1)));
    } else if (rhs_in == ((mp_obj_t)(((3) << 3) | 6))) {

        return mp_binary_op(op, lhs_in, ((mp_obj_t)((((mp_uint_t)(1)) << 1) | 1)));
    } else if (op == MP_BINARY_OP_MULTIPLY) {
        if ((mp_obj_is_qstr(rhs_in) || (mp_obj_is_obj(rhs_in) && ((mp_obj_base_t *)((void *)rhs_in))->type->binary_op == mp_obj_str_binary_op)) || (mp_obj_is_obj(rhs_in) && (((mp_obj_base_t *)((void *)rhs_in))->type == (&mp_type_tuple))) || (mp_obj_is_obj(rhs_in) && (((mp_obj_base_t *)((void *)rhs_in))->type == (&mp_type_list)))) {

            return mp_binary_op(op, rhs_in, lhs_in);
        }
    }
    return (((mp_obj_t)(void *)0));
}

__attribute__((eosio_wasm_import))
void print_hex(const char *data, size_t size);


static mp_obj_t int_from_bytes(size_t n_args, const mp_obj_t *args) {

    (void)n_args;


    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, (1));

    const byte *buf = (const byte *)bufinfo.buf;
    int delta = 1;
    if (args[2] == ((mp_obj_t)((((mp_uint_t)(MP_QSTR_little)) << 3) | 2))) {
        buf += bufinfo.len - 1;
        delta = -1;
    }

    mp_uint_t value = 0;
    size_t len = bufinfo.len;
    for (; len--; buf += delta) {

        if (value > (((mp_int_t)(~(((mp_int_t)(((mp_int_t)(((mp_uint_t)1) << ((sizeof(mp_uint_t)) * 8 - 1))) >> 1))))) >> 8)) {

            byte char_buf[64];
            print_hex(bufinfo.buf, bufinfo.len);
            print_hex("\xa0\x0a", 2);
            mp_obj_t o = mp_obj_bigint_from_bytes_impl(args[2] != ((mp_obj_t)((((mp_uint_t)(MP_QSTR_little)) << 3) | 2)), bufinfo.len, bufinfo.buf);
            mp_obj_bigint_to_bytes_impl(o, 0, 64, char_buf);
            print_hex(char_buf, 64);
            return o;
        }

        value = (value << 8) | *buf;
    }
    return mp_obj_new_int_from_uint(value);
}

static const mp_obj_fun_builtin_var_t int_from_bytes_fun_obj = {{&mp_type_fun_builtin_var}, ((uint32_t)((((uint32_t)(3)) << 17) | (((uint32_t)(4)) << 1) | ((0) ? 1 : 0))), .fun.var = int_from_bytes};
static const mp_rom_obj_static_class_method_t int_from_bytes_obj = {{&mp_type_classmethod}, (&int_from_bytes_fun_obj)};

static mp_obj_t int_to_bytes(size_t n_args, const mp_obj_t *args) {

    (void)n_args;

    mp_int_t len = mp_obj_get_int(args[1]);
    if (len < 0) {
        mp_raise_ValueError(((void*)0));
    }
    _Bool big_endian = args[2] != ((mp_obj_t)((((mp_uint_t)(MP_QSTR_little)) << 3) | 2));

    vstr_t vstr;
    vstr_init_len(&vstr, len);
    byte *data = (byte *)vstr.buf;
    memset(data, 0, len);


    if (!mp_obj_is_small_int(args[0])) {
        mp_obj_bigint_to_bytes_impl(args[0], big_endian, len, data);
    } else

    {
        mp_int_t val = (((mp_int_t)(args[0])) >> 1);
        size_t l = (((size_t)len) < (sizeof(val)) ? ((size_t)len) : (sizeof(val)));
        mp_binary_set_int(l, big_endian, data + (big_endian ? (len - l) : 0), val);
    }

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
static const mp_obj_fun_builtin_var_t int_to_bytes_obj = {{&mp_type_fun_builtin_var}, ((uint32_t)((((uint32_t)(3)) << 17) | (((uint32_t)(4)) << 1) | ((0) ? 1 : 0))), .fun.var = int_to_bytes};

static const mp_rom_map_elem_t int_locals_dict_table[] = {
    { ((mp_obj_t)((((mp_uint_t)(MP_QSTR_from_bytes)) << 3) | 2)), (&int_from_bytes_obj) },
    { ((mp_obj_t)((((mp_uint_t)(MP_QSTR_to_bytes)) << 3) | 2)), (&int_to_bytes_obj) },
};

static const mp_obj_dict_t int_locals_dict = { .base = {&mp_type_dict}, .map = { .all_keys_are_qstrs = 1, .is_fixed = 1, .is_ordered = 1, .used = (sizeof(int_locals_dict_table) / sizeof((int_locals_dict_table)[0])), .alloc = (sizeof(int_locals_dict_table) / sizeof((int_locals_dict_table)[0])), .table = (mp_map_elem_t *)(mp_rom_map_elem_t *)int_locals_dict_table, }, };

const mp_obj_type_t mp_type_bigint = {
    { &mp_type_type },
    .name = MP_QSTR_bigint,
    .print = mp_obj_bigint_print,
    .make_new = mp_obj_bigint_make_new,
    .unary_op = mp_obj_bigint_unary_op,
    .binary_op = mp_obj_bigint_binary_op,
    .locals_dict = (mp_obj_dict_t *)&int_locals_dict,
};
