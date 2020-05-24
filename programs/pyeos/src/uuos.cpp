#include "uuos.hpp"

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

typedef struct chain_api_cpp* (*fn_get_chain_api)();

chain_api_cpp* get_chain_api() {
    static struct chain_api_cpp* chain_api;

    if (!chain_api) {
        const char * chain_api_lib = getenv("CHAIN_API_SHARED_LIB");
        void *handle = dlopen(chain_api_lib, RTLD_LAZY | RTLD_LOCAL);
        if (handle == 0) {
            printf("loading %s failed!\n", chain_api_lib);
            return nullptr;
        }
        fn_get_chain_api api = (fn_get_chain_api)dlsym(handle, "get_chain_api");
        chain_api = api();
    }
    return chain_api;
}

