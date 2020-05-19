#include <stdint.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

typedef void (*fn_say_hello)();

extern "C" void say_hello_() {
    const char * python_shared_lib_path = "/Users/newworld/dev/uuos2/programs/pyeos/_skbuild/macosx-10.9-x86_64-3.7/cmake-build/hello/libhello.dylib";
    void *handle = dlopen(python_shared_lib_path, RTLD_LAZY | RTLD_GLOBAL);
    if (handle == 0) {
        printf("loading %s failed!\n", python_shared_lib_path);
        return;
    }
    
    fn_say_hello say_hello = (fn_say_hello)dlsym(handle, "say_hello");
    say_hello();
}
