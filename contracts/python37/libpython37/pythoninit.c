#include <Python.h>
#include <eosiolib/system.h>
#include <eosiolib/ext.h>


void *get_current_memory(void);
void *get_temp_memory(void);

__attribute__((eosio_wasm_import))
uint64_t current_receiver();

__attribute__((eosio_wasm_import))
void set_copy_memory_range(int start, int end);

__attribute__((eosio_wasm_import))
int get_code_size(uint64_t account);

__attribute__((eosio_wasm_import))
int get_code(uint64_t account, char *code, size_t size);


static int initialized = 0;
static int g_counter = 0;
static PyObject *g_apply = NULL;

PyObject* PyInit_spam(void);
PyMODINIT_FUNC PyInit__sha1(void);

PyObject *PyImport_ImportModuleObject(PyObject *name, const char *code, size_t size);

void python_init(void) {
    Py_IgnoreEnvironmentFlag = 1;
	Py_NoSiteFlag = 1;
    if (!initialized) {
        initialized = 1;
        Py_InitializeEx(0);
        prints("+++++++++++++++++++++++++python initialized!!!\n");
#if 0
        /* Import _importlib through its frozen version, _frozen_importlib. */
        if (PyImport_ImportFrozenModule("__hello__") <= 0) {
            Py_FatalError("Py_Initialize: can't import __hello__");
        }
#endif
        //import io, 
        PyRun_SimpleString("import struct, hashlib, db, sys, base58");
//        PyRun_SimpleString("print('++hello,world')");
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
    }
}

PyObject *python_load_module(const char *code, int size) {
    PyObject *contract = PyUnicode_FromString("contract");
    PyObject *m = PyImport_ImportModuleObject(contract, code, size);
    if (PyErr_Occurred()) {
        PyErr_Print();
        eosio_assert(0, "python_load_module exit with exceptions!");
    }
    return m;
}

void python_call_module(void *m, uint64_t receiver, uint64_t code, uint64_t action) {
    PyObject *args[3];
    PyObject *d = PyModule_GetDict((PyObject *)m);
    PyObject *apply = PyDict_GetItemString(d, "apply");
    if (apply == NULL) {
        eosio_assert(0, "apply not found!");
        return;
    }
    args[0] = PyLong_FromUnsignedLongLong(receiver);
    args[1] = PyLong_FromUnsignedLongLong(code);
    args[2] = PyLong_FromUnsignedLongLong(action);

    _PyObject_FastCall(apply, args, 3);

    if (PyErr_Occurred()) {
        PyErr_PrintEx(0);
        eosio_assert(0, "python_call_module exit with exceptions!");
    }
    Py_DECREF(args);
}

void python_call_apply(uint64_t receiver, uint64_t code, uint64_t action) {
    PyObject *args[3];
    if (g_apply == NULL) {
        eosio_assert(0, "apply not found!");
        return;
    }
    args[0] = PyLong_FromUnsignedLongLong(receiver);
    args[1] = PyLong_FromUnsignedLongLong(code);
    args[2] = PyLong_FromUnsignedLongLong(action);

    _PyObject_FastCall(g_apply, args, 3);

    if (PyErr_Occurred()) {
        PyErr_PrintEx(0);
        eosio_assert(0, "apply exit with exceptions!");
    }
    Py_DECREF(args);
}

void check_error(void) {
    if (PyErr_Occurred()) {
        PyErr_Print();
        eosio_assert(0, "python call exit with exceptions!");
    }
}

void python_vm_apply( uint64_t receiver, uint64_t code, uint64_t action ) {
    if (g_apply == NULL) {
        eosio_assert(0, "apply not found!");
        return;
    }
    python_call_apply(receiver, code, action);
}

void python_vm_call( uint64_t func_name, uint64_t receiver, uint64_t code, uint64_t action ) {
    if (func_name == 0) {
        g_counter += 1;
        python_init();
        initialized = 1;
    }
    else if (func_name == 1) {
        uint32_t start, end;
        start = (uint32_t)get_current_memory();
//        printf("+++++++++++++++get_current_memory: %d\n", get_current_memory());
        int code_size = get_code_size(receiver);
        char *str_code = (char *)get_temp_memory();//malloc(code_size);
//        prints("++++++++++++code buffer ");printi(str_code);prints("\n");
//        receiver = current_receiver();
        int size = get_code(receiver, str_code, code_size);
        PyObject *m = python_load_module(str_code, code_size);
        PyObject *d = PyModule_GetDict(m);
        g_apply = PyDict_GetItemString(d, "apply");
        if (g_apply == NULL) {
            eosio_assert(0, "apply not found!");
            return;
        }
//        printf("+++++++++++++++get_current_memory: %d\n", get_current_memory());
        memset(str_code, 0, code_size);
        end = (uint32_t)get_current_memory();
        set_copy_memory_range(start, end);
        check_error();
    }
}

