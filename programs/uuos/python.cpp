#include <stdint.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

#define PYTHON_SHARED_LIB_PATH "PYTHON_SHARED_LIB_PATH"

typedef int (*fnPy_Main)(int argc, wchar_t **argv);
typedef void (*fnPy_Initialize)();
typedef void (*fnPy_InitializeEx)(int initsigs);

typedef int (*fnPyRun_SimpleString)(const char *command);
typedef void* (*fnPyMem_Malloc)(size_t n);
typedef wchar_t * (*fnPy_DecodeLocale)(const char *arg, size_t *size);

extern "C" int start_python(int argc, char **argv) {

    const char *python_shared_lib_path = getenv(PYTHON_SHARED_LIB_PATH);

    void *handle = dlopen(python_shared_lib_path, RTLD_LAZY | RTLD_GLOBAL);
    if (handle == 0) {
        printf("loading %s failed!\n", python_shared_lib_path);
        return -1;
    }

    fnPy_Main Py_Main = (fnPy_Main)dlsym(handle, "Py_Main");
    if (Py_Main == 0) {
        printf("++++++++Py_Main not found in shared library\n");
        return -1;
    }

    fnPy_Initialize Py_Initialize = (fnPy_Initialize)dlsym(handle, "Py_Initialize");
    if (Py_Initialize == 0) {
        printf("++++++++Py_Initialize not found in shared library\n");
        return -1;
    }

    fnPy_InitializeEx Py_InitializeEx = (fnPy_InitializeEx)dlsym(handle, "Py_InitializeEx");
    if (Py_InitializeEx == 0) {
        printf("++++++++Py_InitializeEx not found in shared library\n");
        return -1;
    }


    fnPyRun_SimpleString PyRun_SimpleString = (fnPyRun_SimpleString)dlsym(handle, "PyRun_SimpleString");
    if (Py_Initialize == 0) {
        printf("++++++++Py_Initialize not found in shared library\n");
        return -1;
    }

    fnPyMem_Malloc PyMem_Malloc = (fnPyMem_Malloc)dlsym(handle, "PyMem_Malloc");
    if (PyMem_Malloc == 0) {
        printf("++++++++PyMem_Malloc not found in shared library\n");
        return -1;
    }

    fnPy_DecodeLocale Py_DecodeLocale = (fnPy_DecodeLocale)dlsym(handle, "Py_DecodeLocale");
    if (Py_DecodeLocale == 0) {
        printf("++++++++Py_DecodeLocale not found in shared library\n");
        return -1;
    }

    wchar_t** _argv = (wchar_t**)PyMem_Malloc(sizeof(wchar_t*) * argc);
    for (int i = 0; i < argc; i++) {
        wchar_t* arg = Py_DecodeLocale(argv[i], NULL);
        _argv[i] = arg;
    }
    
    return Py_Main(argc, _argv);

    // Py_InitializeEx(0);
    // PyRun_SimpleString("import struct;print(struct)\n" \
    //                     "import os;import sys;sys.path.append('.')\n" \
    //                     "uuos_lib=os.getenv('UUOS_EXT_LIB')\n"
    //                     "print(uuos_lib)\n"
    //                     "sys.path.append(uuos_lib)\n"
    //                     "import uuos\n"
    // );

    // PyRun_SimpleString("import struct;print(struct)\n" \
    //                     "import os;import sys;sys.path.append('.')\n" \
    //                     "uuos_lib=os.getenv('UUOS_EXT_LIB')\n"
    //                     "print(uuos_lib)\n"
    //                     "sys.path.append(uuos_lib)\n"
    //                     "import main;main.run()\n"
    // );
    
    // PyRun_SimpleString("import _uuos;_uuos.say_hello()");

    return 0;
}
