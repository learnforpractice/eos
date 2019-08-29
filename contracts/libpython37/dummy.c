#if 0

void *stdout = 11;
void *stderr = 22;
void *stdin = 33;

int kill(int a, int b) {
    return 0;
}

#ifndef WASM_RT_MODULE_PREFIX
#define WASM_RT_MODULE_PREFIX
#endif

#define WASM_RT_PASTE_(x, y) x ## y
#define WASM_RT_PASTE(x, y) WASM_RT_PASTE_(x, y)
#define WASM_RT_ADD_PREFIX(x) WASM_RT_PASTE(WASM_RT_MODULE_PREFIX, x)

#include <stdint.h>

/* TODO(binji): only use stdint.h types in header */
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

/* import: 'env' 'Py_GetExecPrefix' */
u32 Py_GetExecPrefix(void){return 0;}
/* import: 'env' 'Py_GetPath' */
u32 Py_GetPath(void){return 0;}
/* import: 'env' 'Py_GetPrefix' */
u32 Py_GetPrefix(void){return 0;}
/* import: 'env' 'Py_GetProgramFullPath' */
u32 Py_GetProgramFullPath(void){return 0;}

#if 0
/* import: 'env' '__addtf3' */
void __addtf3jjjj(u32, u64, u64, u64, u64){return 0;}
/* import: 'env' '__eqtf2' */
u32 __eqtf2Z_ijjjj(u64, u64, u64, u64){return 0;}

/* import: 'env' '__multf3' */
void __multf3jjjj(u32, u64, u64, u64, u64){return 0;}
/* import: 'env' '__netf2' */
u32 __netf2Z_ijjjj(u64, u64, u64, u64){return 0;}
/* import: 'env' '__nl_langinfo_l' */
u32 __nl_langinfo_l(u32 a, u32 b){return 0;}
/* import: 'env' '__subtf3' */
void __subtf3jjjj(u32, u64, u64, u64, u64){return 0;}
/* import: 'env' '__unlock' */
void __unlock(u32 a){return 0;}
/* import: 'env' '__unordtf2' */
u32 __unordtf2Z_ijjjj(u64, u64, u64, u64){return 0;}


#endif

/* import: 'env' '__errno_location' */
u32 __errno_location(void){return 0;}
/* import: 'env' '__extenddftf2' */
void __extenddftf2d(u32 a, f64 b){return;}
/* import: 'env' '__fixtfsi' */
u32 __fixtfsi(u64 a, u64 b){return 0;}
/* import: 'env' '__fixunstfsi' */
u32 __fixunstfsi(u64 a, u64 b){return 0;}
/* import: 'env' '__floatsitf' */
void __floatsitf(u32 a, u32 b){return;}
/* import: 'env' '__floatunsitf' */
void __floatunsitf(u32 a, u32 b){return;}
/* import: 'env' '__libc_current_sigrtmax' */
u32 __libc_current_sigrtmax(void){return 0;}
/* import: 'env' '__libc_current_sigrtmin' */
u32 __libc_current_sigrtmin(void){return 0;}
/* import: 'env' '__lock' */
void __lock(u32 a){return;}
/* import: 'env' '_exit' */
void _exit(u32 a){return;}
/* import: 'env' 'abort' */
void abort(void){return;}
/* import: 'env' 'access' */
u32 access(u32 a, u32 b){return 0;}
/* import: 'env' 'alarm' */
u32 alarm(u32 a){return 0;}
/* import: 'env' 'bind_textdomain_codeset' */
u32 bind_textdomain_codeset(u32 a, u32 b){return 0;}
/* import: 'env' 'bindtextdomain' */
u32 bindtextdomain(u32 a, u32 b){return 0;}
/* import: 'env' 'chdir' */
u32 chdir(u32 a){return 0;}
/* import: 'env' 'chmod' */
u32 chmod(u32 a, u32 b){return 0;}
/* import: 'env' 'chown' */
u32 chown(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'chroot' */
u32 chroot(u32 a){return 0;}
/* import: 'env' 'clock' */
u32 clock(void){return 0;}
/* import: 'env' 'clock_getres' */
u32 clock_getres(u32 a, u32 b){return 0;}
/* import: 'env' 'clock_gettime' */
u32 clock_gettime(u32 a, u32 b){return 0;}
/* import: 'env' 'clock_settime' */
u32 clock_settime(u32 a, u32 b){return 0;}
/* import: 'env' 'close' */
u32 close(u32 a){return 0;}
/* import: 'env' 'closedir' */
u32 closedir(u32 a){return 0;}
/* import: 'env' 'confstr' */
u32 confstr(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'ctermid' */
u32 ctermid(u32 a){return 0;}
/* import: 'env' 'dcgettext' */
u32 dcgettext(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'dgettext' */
u32 dgettext(u32 a, u32 b){return 0;}
/* import: 'env' 'dup' */
u32 dup(u32 a){return 0;}
/* import: 'env' 'dup2' */
u32 dup2(u32 a, u32 b){return 0;}
/* import: 'env' 'dup3' */
u32 dup3(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'endpwent' */
void endpwent(void){return;}
/* import: 'env' 'execv' */
u32 execv(u32 a, u32 b){return 0;}
/* import: 'env' 'execve' */
u32 execve(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'exit' */
void exit(u32 a){return;}
/* import: 'env' 'fchdir' */
u32 fchdir(u32 a){return 0;}
/* import: 'env' 'fchmod' */
u32 fchmod(u32 a, u32 b){return 0;}
/* import: 'env' 'fchmodat' */
u32 fchmodat(u32 a, u32 b, u32 c, u32 e){return 0;}
/* import: 'env' 'fchown' */
u32 fchown(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'fchownat' */
u32 fchownat(u32 a, u32 b, u32 c, u32 d, u32 e){return 0;}
/* import: 'env' 'fcntl' */
u32 fcntl(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'fdatasync' */
u32 fdatasync(u32 a){return 0;}
/* import: 'env' 'fdopen' */
u32 fdopen(u32 a, u32 b){return 0;}
/* import: 'env' 'fdopendir' */
u32 fdopendir(u32 a){return 0;}
/* import: 'env' 'fexecve' */
u32 fexecve(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'flockfile' */
void flockfile(u32 a){return;}
/* import: 'env' 'fopen' */
u32 fopen(u32 a, u32 b){return 0;}
/* import: 'env' 'fork' */
u32 fork(void){return 0;}
/* import: 'env' 'forkpty' */
u32 forkpty(u32 a, u32 b, u32 c, u32 e){return 0;}
/* import: 'env' 'fpathconf' */
u32 fpathconf(u32 a, u32 b){return 0;}
/* import: 'env' 'fstat' */
u32 fstat(u32 a, u32 b){return 0;}
/* import: 'env' 'fstatat' */
u32 fstatat(u32 a, u32 b, u32 c, u32 e){return 0;}
/* import: 'env' 'fstatvfs' */
u32 fstatvfs(u32 a, u32 b){return 0;}
/* import: 'env' 'fsync' */
u32 fsync(u32 a){return 0;}
/* import: 'env' 'ftruncate' */
u32 ftruncate(u32 a, u32 b){return 0;}
/* import: 'env' 'funlockfile' */
void funlockfile(u32 a){return;}
/* import: 'env' 'futimens' */
u32 futimens(u32 a, u32 b){return 0;}
/* import: 'env' 'getcwd' */
u32 getcwd(u32 a, u32 b){return 0;}
/* import: 'env' 'getegid' */
u32 getegid(void){return 0;}
/* import: 'env' 'geteuid' */
u32 geteuid(void){return 0;}
/* import: 'env' 'getgid' */
u32 getgid(void){return 0;}
/* import: 'env' 'getgroups' */
u32 getgroups(u32 a, u32 b){return 0;}
/* import: 'env' 'getitimer' */
u32 getitimer(u32 a, u32 b){return 0;}
/* import: 'env' 'getloadavg' */
u32 getloadavg(u32 a, u32 b){return 0;}
/* import: 'env' 'getlogin' */
u32 getlogin(void){return 0;}
/* import: 'env' 'getpgid' */
u32 getpgid(u32 a){return 0;}
/* import: 'env' 'getpgrp' */
u32 getpgrp(void){return 0;}
/* import: 'env' 'getpid' */
u32 getpid(void){return 0;}
/* import: 'env' 'getppid' */
u32 getppid(void){return 0;}
/* import: 'env' 'getpriority' */
u32 getpriority(u32 a, u32 b){return 0;}
/* import: 'env' 'getpwent' */
u32 getpwent(void){return 0;}
/* import: 'env' 'getpwnam' */
u32 getpwnam(u32 a){return 0;}
/* import: 'env' 'getpwuid' */
u32 getpwuid(u32 a){return 0;}
/* import: 'env' 'getresgid' */
u32 getresgid(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'getresuid' */
u32 getresuid(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'getrlimit' */
u32 getrlimit(u32 a, u32 b){return 0;}
/* import: 'env' 'getrusage' */
u32 getrusage(u32 a, u32 b){return 0;}
/* import: 'env' 'getsid' */
u32 getsid(u32 a){return 0;}
/* import: 'env' 'gettext' */
u32 gettext(u32 a){return 0;}
/* import: 'env' 'getuid' */
u32 getuid(void){return 0;}
/* import: 'env' 'ioctl' */
u32 ioctl(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'isatty' */
u32 isatty(u32 a){return 0;}
/* import: 'env' 'killpg' */
u32 killpg(u32 a, u32 b){return 0;}
/* import: 'env' 'lchmod' */
u32 lchmod(u32 a, u32 b){return 0;}
/* import: 'env' 'lchown' */
u32 lchown(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'link' */
u32 link(u32 a, u32 b){return 0;}
/* import: 'env' 'linkat' */
u32 linkat(u32 a, u32 b, u32 c, u32 d, u32 e){return 0;}
/* import: 'env' 'localeconv' */
u32 localeconv(void){return 0;}
/* import: 'env' 'localtime' */
u32 localtime(u32 a){return 0;}
/* import: 'env' 'lockf' */
u32 lockf(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'lseek' */
u32 lseek(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'lstat' */
u32 lstat(u32 a, u32 b){return 0;}
/* import: 'env' 'memcpy' */
u32 memcpy(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'memmove' */
u32 memmove(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'memset' */
u32 memset(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'mkdir' */
u32 mkdir(u32 a, u32 b){return 0;}
/* import: 'env' 'mkdirat' */
u32 mkdirat(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'mkfifo' */
u32 mkfifo(u32 a, u32 b){return 0;}
/* import: 'env' 'mkfifoat' */
u32 mkfifoat(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'mknod' */
u32 mknod(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'mknodat' */
u32 mknodat(u32 a, u32 b, u32 c, u32 e){return 0;}
/* import: 'env' 'mktime' */
u32 mktime(u32 a){return 0;}
/* import: 'env' 'nice' */
u32 nice(u32 a){return 0;}
/* import: 'env' 'nl_langinfo' */
u32 nl_langinfo(u32 a){return 0;}
/* import: 'env' 'open' */
u32 open(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'openat' */
u32 openat(u32 a, u32 b, u32 c, u32 e){return 0;}
/* import: 'env' 'opendir' */
u32 opendir(u32 a){return 0;}
/* import: 'env' 'openpty' */
u32 openpty(u32 a, u32 b, u32 c, u32 d, u32 e){return 0;}
/* import: 'env' 'pathconf' */
u32 pathconf(u32 a, u32 b){return 0;}
/* import: 'env' 'pause' */
u32 pause(void){return 0;}
/* import: 'env' 'pipe' */
u32 pipe(u32 a){return 0;}
/* import: 'env' 'pipe2' */
u32 pipe2(u32 a, u32 b){return 0;}
/* import: 'env' 'posix_fadvise' */
u32 posix_fadvise(u32 a, u32 b, u32 c, u32 e){return 0;}
/* import: 'env' 'posix_fallocate' */
u32 posix_fallocate(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'pread' */
u32 pread(u32 a, u32 b, u32 c, u32 e){return 0;}
/* import: 'env' 'prints_l' */
void prints_l(u32 a, u32 b){return;}
/* import: 'env' 'pwrite' */
u32 pwrite(u32 a, u32 b, u32 c, u32 e){return 0;}
/* import: 'env' 'raise' */
u32 raise(u32 a){return 0;}
/* import: 'env' 'read' */
u32 read(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'readdir' */
u32 readdir(u32 a){return 0;}
/* import: 'env' 'readlink' */
u32 readlink(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'readlinkat' */
u32 readlinkat(u32 a, u32 b, u32 c, u32 e){return 0;}
/* import: 'env' 'readv' */
u32 readv(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'rename' */
u32 rename(u32 a, u32 b){return 0;}
/* import: 'env' 'renameat' */
u32 renameat(u32 a, u32 b, u32 c, u32 e){return 0;}
/* import: 'env' 'rewinddir' */
void rewinddir(u32 a){return;}
/* import: 'env' 'rmdir' */
u32 rmdir(u32 a){return 0;}
/* import: 'env' 'sched_yield' */
u32 sched_yield(void){return 0;}
/* import: 'env' 'select' */
u32 select(u32 a, u32 b, u32 c, u32 d, u32 e){return 0;}
/* import: 'env' 'setegid' */
u32 setegid(u32 a){return 0;}
/* import: 'env' 'seteuid' */
u32 seteuid(u32 a){return 0;}
/* import: 'env' 'setgid' */
u32 setgid(u32 a){return 0;}
/* import: 'env' 'setgroups' */
u32 setgroups(u32 a, u32 b){return 0;}
/* import: 'env' 'setitimer' */
u32 setitimer(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'setlocale' */
u32 setlocale(u32 a, u32 b){return 0;}
/* import: 'env' 'setpgid' */
u32 setpgid(u32 a, u32 b){return 0;}
/* import: 'env' 'setpgrp' */
u32 setpgrp(void){return 0;}
/* import: 'env' 'setpriority' */
u32 setpriority(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'setpwent' */
void setpwent(void){return;}
/* import: 'env' 'setregid' */
u32 setregid(u32 a, u32 b){return 0;}
/* import: 'env' 'setresgid' */
u32 setresgid(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'setresuid' */
u32 setresuid(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'setreuid' */
u32 setreuid(u32 a, u32 b){return 0;}
/* import: 'env' 'setrlimit' */
u32 setrlimit(u32 a, u32 b){return 0;}
/* import: 'env' 'setsid' */
u32 setsid(void){return 0;}
/* import: 'env' 'setuid' */
u32 setuid(u32 a){return 0;}
/* import: 'env' 'sigaction' */
u32 sigaction(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'sigemptyset' */
u32 sigemptyset(u32 a){return 0;}
/* import: 'env' 'siginterrupt' */
u32 siginterrupt(u32 a, u32 b){return 0;}
/* import: 'env' 'sigismember' */
u32 sigismember(u32 a, u32 b){return 0;}
/* import: 'env' 'sigpending' */
u32 sigpending(u32 a){return 0;}
/* import: 'env' 'stat' */
u32 stat(u32 a, u32 b){return 0;}
/* import: 'env' 'statvfs' */
u32 statvfs(u32 a, u32 b){return 0;}
/* import: 'env' 'strerror' */
u32 strerror(u32 a){return 0;}
/* import: 'env' 'strtod' */
f64 strtod(u32 a, u32 b){return 0;}
/* import: 'env' 'symlink' */
u32 symlink(u32 a, u32 b){return 0;}
/* import: 'env' 'symlinkat' */
u32 symlinkat(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'sync' */
void sync(void){return;}
/* import: 'env' 'sysconf' */
u32 sysconf(u32 a){return 0;}
/* import: 'env' 'system' */
u32 system(u32 a){return 0;}
/* import: 'env' 'tcgetpgrp' */
u32 tcgetpgrp(u32 a){return 0;}
/* import: 'env' 'tcsetpgrp' */
u32 tcsetpgrp(u32 a, u32 b){return 0;}
/* import: 'env' 'textdomain' */
u32 textdomain(u32 a){return 0;}
/* import: 'env' 'time' */
u32 time(u32 a){return 0;}
/* import: 'env' 'times' */
u32 times(u32 a){return 0;}
/* import: 'env' 'truncate' */
u32 truncate(u32 a, u32 b){return 0;}
/* import: 'env' 'ttyname' */
u32 ttyname(u32 a){return 0;}
/* import: 'env' 'umask' */
u32 umask(u32 a){return 0;}
/* import: 'env' 'uname' */
u32 uname(u32 a){return 0;}
/* import: 'env' 'unlink' */
u32 unlink(u32 a){return 0;}
/* import: 'env' 'unlinkat' */
u32 unlinkat(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'utimensat' */
u32 utimensat(u32 a, u32 b, u32 c, u32 e){return 0;}
/* import: 'env' 'wait' */
u32 wait(u32 a){return 0;}
/* import: 'env' 'wait3' */
u32 wait3(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'wait4' */
u32 wait4(u32 a, u32 b, u32 c, u32 e){return 0;}
/* import: 'env' 'waitid' */
u32 waitid(u32 a, u32 b, u32 c, u32 e){return 0;}
/* import: 'env' 'waitpid' */
u32 waitpid(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'wcscoll' */
u32 wcscoll(u32 a, u32 b){return 0;}
/* import: 'env' 'wcsxfrm' */
u32 wcsxfrm(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'write' */
u32 write(u32 a, u32 b, u32 c){return 0;}
/* import: 'env' 'writev' */
u32 writev(u32 a, u32 b, u32 c){return 0;}

#endif

