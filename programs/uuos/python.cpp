#include <stdint.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

#define PYTHON_SHARED_LIB_PATH "PYTHON_SHARED_LIB_PATH"

typedef int (*fnPy_Main)(int argc, char **argv);
typedef void (*fnPy_Initialize)();
typedef int (*fnPyRun_SimpleString)(const char *command);

extern "C" int init_python() {
    const char *python_shared_lib_path = getenv(PYTHON_SHARED_LIB_PATH);

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

    Py_Initialize();
    PyRun_SimpleString("import struct;print(struct)\n" \
                        "import os;import sys;sys.path.append('.')\n" \
                        "uuos_lib=os.getenv('UUOS_EXT_LIB')\n"
                        "print(uuos_lib)\n"
                        "sys.path.append(uuos_lib)\n"
                        "import _uuos\n"
                        "_uuos.say_hello()\n"
    );
    
    // PyRun_SimpleString("import _uuos;_uuos.say_hello()");

    return 0;
}
