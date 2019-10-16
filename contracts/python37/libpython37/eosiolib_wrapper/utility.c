#undef Py_BUILD_CORE
#include <Python.h>

__attribute__((eosio_wasm_import))
uint64_t s2n(const char *in, size_t in_len);

__attribute__((eosio_wasm_import))
int n2s(uint64_t n, char *out, size_t length);

//#include <eosiolib/utility.h>
#if 0
//int pack_bytes(const char *in, int in_size, char *out, int out_size);
static PyObject *py_pack_bytes(PyObject *self, PyObject *args)
{
    const char *buffer;
    Py_ssize_t len;

    if (!PyArg_ParseTuple(args, "s#", &buffer, &len)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    int out_size = len+sizeof(int);
    char *out = (char *)malloc(out_size);
    int size = pack_bytes(buffer, len, out, out_size);
    return PyBytes_FromStringAndSize(out, size);
}


//int unpack_bytes(const char *in, int in_size, char *out, int out_size);
static PyObject *py_unpack_bytes(PyObject *self, PyObject *args)
{
    const char *buffer;
    Py_ssize_t len;

    if (!PyArg_ParseTuple(args, "s#", &buffer, &len)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    int out_size = len;
    char *out = (char *)malloc(out_size);
    int size = unpack_bytes(buffer, len, out, out_size);
    return PyBytes_FromStringAndSize(out, size);
}
#define UTILITY_METHODS \
    {"pack_bytes",      (PyCFunction)py_pack_bytes, METH_VARARGS, NULL}, \
    {"unpack_bytes",    (PyCFunction)py_unpack_bytes, METH_VARARGS, NULL},
#else
#define UTILITY_METHODS
#endif

uint64_t to_name(PyObject *o) {
    uint64_t name = 0;
    char *cname = NULL;
    Py_ssize_t size = 0;

    if (PyLong_Check(o)) {
        name = PyLong_AsUnsignedLongLong(o);
    } else if (PyUnicode_Check(o)) {
        cname = (char *)PyUnicode_AsUTF8AndSize(o, &size);
        name = s2n(cname, size);
    } else if (PyBytes_Check(o)) {
        PyBytes_AsStringAndSize(o, &cname, &size);
        name = s2n(cname, size);
    } else {
        PyErr_SetString(PyExc_ValueError, "wrong argument");
        return 0;
    }
    if (name == 0) {
        PyErr_SetString(PyExc_ValueError, "Name not properly normalized");
        return 0;
    }
    return name;
}


int o2n(PyObject *o, uint64_t* n) {
    uint64_t name = 0;
    char *cname = NULL;
    Py_ssize_t size = 0;

    if (PyLong_Check(o)) {
        name = PyLong_AsUnsignedLongLong(o);
    } else if (PyUnicode_Check(o)) {
        cname = (char *)PyUnicode_AsUTF8AndSize(o, &size);
        name = s2n(cname, size);
        if (name == 0) {
            PyErr_SetString(PyExc_ValueError, "Name not properly normalized");
            return 0;
        }
    } else if (PyBytes_Check(o)) {
        PyBytes_AsStringAndSize(o, &cname, &size);
        name = s2n(cname, size);
        if (name == 0) {
            PyErr_SetString(PyExc_ValueError, "Name not properly normalized");
            return 0;
        }
    } else {
        PyErr_SetString(PyExc_ValueError, "wrong name type");
        return 0;
    }
    *n = name;
    return 1;
}

