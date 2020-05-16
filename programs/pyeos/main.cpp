#include <stdint.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

typedef int (*fnPy_Main)(int argc, char **argv);
typedef void (*fnPy_Initialize)();
typedef int (*fnPyRun_SimpleString)(const char *command);

int main(int argc, char **argv) {
    const char *python_shared_lib_path = argv[argc-1];
    // python_shared_lib_path = "/usr/local/Cellar/python/3.7.6_1/Frameworks/Python.framework/Versions/3.7/lib/libpython3.7m.dylib";

    void *handle = dlopen(python_shared_lib_path, RTLD_LAZY | RTLD_GLOBAL);
    if (handle == 0) {
        printf("loading %s failed!\n", python_shared_lib_path);
        return -1;
    }

    fnPy_Main pymain = (fnPy_Main)dlsym(handle, "Py_Main");
    if (pymain == 0) {
        printf("++++++++Py_Main not found in shared library\n");
        return -1;
    }

    fnPy_Initialize Py_Initialize = (fnPy_Initialize)dlsym(handle, "Py_Initialize");
    if (Py_Initialize == 0) {
        printf("++++++++Py_Initialize not found in shared library\n");
        return -1;
    }

    fnPyRun_SimpleString PyRun_SimpleString = (fnPyRun_SimpleString)dlsym(handle, "PyRun_SimpleString");
    if (Py_Initialize == 0) {
        printf("++++++++Py_Initialize not found in shared library\n");
        return -1;
    }

    // setenv("PYTHONHOME", "/usr/local/Cellar/python/3.7.6_1/Frameworks/Python.framework/Versions/3.7", true);

    // Py_Initialize();
    // PyRun_SimpleString("import struct;print(struct)");
    // return 0;

    setenv("PYTHONHOME", argv[argc-2], true);
    return pymain(argc-2, argv);

//    setenv("PYTHONHOME", "/usr/local/Cellar/python/3.7.6_1/Frameworks/Python.framework/Versions/3.7", true);
    return 0;
    //return Py_Main(argc, argv);
}
