#undef Py_BUILD_CORE
#include <Python.h>
typedef char bool;
typedef __int128 int128_t ;
typedef unsigned __int128 uint128_t;

#include <eosiolib/db.h>

int parse_db_data(PyObject *arg, const char **data, Py_ssize_t *len);

int get_f128_values(PyObject *args, long double *a, long double *b) {
    PyObject *o;
    long double *temp;
    Py_ssize_t len = 0;

    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return 0;
    }

    o = PyTuple_GetItem(args, 0);
    if (PyFloat_Check(o)) {
        *a = (long double)PyFloat_AsDouble(o);
    } else {
        if (!parse_db_data(o, (const char **)&temp, &len)) {
            return 0;
        }
        if (len !=sizeof(long double)) {
            PyErr_SetString(PyExc_ValueError, "wrong size");
            return 0;
        }
        *a = *temp;
    }

    o = PyTuple_GetItem(args, 1);
    if (PyFloat_Check(o)) {
        *b = (long double)PyFloat_AsDouble(o);
    } else {
        if (!parse_db_data(o, (const char **)&temp, &len)) {
            return 0;
        }
        if (len !=sizeof(long double)) {
            PyErr_SetString(PyExc_ValueError, "wrong size");
            return 0;
        }
        *b = *temp;
    }
    return 1;
}

//int32_t db_store_i64(account_name scope, table_name table, account_name payer, uint64_t id,  const void* data, uint32_t len);
static PyObject *py_f128_add(PyObject *self, PyObject *args)
{
    long double a;
    long double b;
    if (!get_f128_values(args, &a, &b)) {
        return NULL;
    }
    long double ret = a + b;
    return PyBytes_FromStringAndSize((char *)&ret, sizeof(long double));
}

static PyObject *py_f128_sub(PyObject *self, PyObject *args)
{
    long double a;
    long double b;
    if (!get_f128_values(args, &a, &b)) {
        return NULL;
    }
    long double ret = a - b;
    return PyBytes_FromStringAndSize((char *)&ret, sizeof(long double));
}

static PyObject *py_f128_mul(PyObject *self, PyObject *args)
{
    long double a;
    long double b;
    if (!get_f128_values(args, &a, &b)) {
        return NULL;
    }
    long double ret = a * b;
    return PyBytes_FromStringAndSize((char *)&ret, sizeof(long double));
}

static PyObject *py_f128_div(PyObject *self, PyObject *args)
{
    long double a;
    long double b;
    if (!get_f128_values(args, &a, &b)) {
        return NULL;
    }
    long double ret = a / b;
    return PyBytes_FromStringAndSize((char *)&ret, sizeof(long double));
}

static PyMethodDef db_methods[] = {
    {"add",                    (PyCFunction)py_f128_add,           METH_VARARGS, NULL},
    {"sub",                    (PyCFunction)py_f128_sub,           METH_VARARGS, NULL},
    {"mul",                    (PyCFunction)py_f128_mul,           METH_VARARGS, NULL},
    {"div",                    (PyCFunction)py_f128_div,           METH_VARARGS, NULL},
    {NULL,                      NULL}
};

static struct PyModuleDef f128module = {
    PyModuleDef_HEAD_INIT,
    "_float128",
    NULL, //thread_doc,
    -1,
    db_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit__float128(void)
{
    PyObject *m;
    /* Create the module and add the functions */
    return PyModule_Create(&f128module);
}

