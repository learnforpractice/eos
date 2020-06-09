#include "uuos.hpp"

using namespace std;

typedef void (*fn_say_hello)();

typedef struct chain_api_cpp* (*fn_get_chain_api)();
typedef struct vm_api* (*fn_get_vm_api)();

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

        vm_callback cb;
        cb.setcode = cpython_setcode;
        cb.apply = cpython_apply;
        chain_api->register_vm_callback(2, 0, &cb);
    }
    return chain_api;
}

vm_api* uuos_get_vm_api() {
    static struct vm_api* s_api;
    if (!s_api) {
        const char * vm_api_lib = getenv("VM_API_SHARED_LIB");
        void *handle = dlopen(vm_api_lib, RTLD_LAZY | RTLD_LOCAL);
        if (handle == 0) {
            printf("loading %s failed!\n", vm_api_lib);
            return nullptr;
        }
        fn_get_vm_api _get_vm_api = (fn_get_vm_api)dlsym(handle, "get_vm_api");
        s_api = _get_vm_api();
    }
    return s_api;
}
