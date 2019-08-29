#include <Python.h>

PyObject* PyInit_spam(void);
PyMODINIT_FUNC PyInit__sha1(void);

PyObject *PyImport_ImportModuleObject(PyObject *name, const char *code, size_t size);

void python_init() {
    Py_IgnoreEnvironmentFlag = 1;
	Py_NoSiteFlag = 1;
    static int initialized = 0;
    if (!initialized) {
        initialized = 1;
        Py_InitializeEx(0);
#if 0
        prints("+++++++++++++++++++++++++intialize __hello__\n");
        /* Import _importlib through its frozen version, _frozen_importlib. */
        if (PyImport_ImportFrozenModule("__hello__") <= 0) {
            Py_FatalError("Py_Initialize: can't import __hello__");
        }
#endif
        //import io, 
        PyRun_SimpleString("import struct, hashlib, db, sys, base58");
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
    }
}

int get_code_size(uint64_t account);
int get_code(uint64_t account, char *code, size_t size);

void *python_load_module(const char *code, int size) {
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
