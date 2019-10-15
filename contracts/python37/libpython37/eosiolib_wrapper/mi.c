#include <mi.h>
#include <Python.h>

uint64_t to_name(PyObject *o);

int get_list_size(PyObject *o) {
    if (PyTuple_Check(o)) {
        return PyTuple_GET_SIZE(o);
    }
    if (PyList_Check(o)) {
        return PyList_GET_SIZE(o);
    }
    return 0;
}

PyObject* get_list_obj(PyObject *o, int index) {
    if (PyTuple_Check(o)) {
        return PyTuple_GetItem(o, index);
    }
    if (PyList_Check(o)) {
        return PyList_GetItem(o, index);
    }
    return NULL;
}

bool parse_arg(PyObject *args, int index, uint64_t *arg) {
    PyObject *o;
    o = PyTuple_GetItem(args, index);
    if (o == NULL) {
        return 0;
    }
    *arg = to_name(o);
    return 1;
}

int parse_args3(PyObject *args, uint64_t *arg1, uint64_t *arg2, uint64_t *arg3) {
    if (!parse_arg(args, 0, arg1)) {
        return 0;
    }
    if (!parse_arg(args, 1, arg2)) {
        return 0;
    }
    if (!parse_arg(args, 2, arg3)) {
        return 0;
    }
    return 1;
}

int parse_args4(PyObject *args, uint64_t *arg1, uint64_t *arg2, uint64_t *arg3, uint64_t *arg4) {
    if (!parse_arg(args, 0, arg1)) {
        return 0;
    }
    if (!parse_arg(args, 1, arg2)) {
        return 0;
    }
    if (!parse_arg(args, 2, arg3)) {
        return 0;
    }
    if (!parse_arg(args, 3, arg4)) {
        return 0;
    }
    return 1;
}

int parse_args5(PyObject *args, uint64_t *arg1, uint64_t *arg2, uint64_t *arg3, uint64_t *arg4, uint64_t *arg5) {
    if (!parse_arg(args, 0, arg1)) {
        return 0;
    }
    if (!parse_arg(args, 1, arg2)) {
        return 0;
    }
    if (!parse_arg(args, 2, arg3)) {
        return 0;
    }
    if (!parse_arg(args, 3, arg4)) {
        return 0;
    }
    if (!parse_arg(args, 4, arg5)) {
        return 0;
    }
    return 1;
}

int parse_data(PyObject *arg, char **data, int *len) {
    if (!PyBytes_Check(arg)) {
        PyErr_SetString(PyExc_ValueError, "bad data type.");
        return 0;
    }
    return PyBytes_AsStringAndSize(arg, data, len) != -1;
}

//void *mi_new(uint64_t code, uint64_t scope, uint64_t table, int * _indexes, uint32_t size);    

static PyObject *py_mi_new(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint64_t id;
    const char* data;
    Py_ssize_t len;

    if (PyTuple_GET_SIZE(args) != 4) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    if (!parse_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 3);

    if (PyList_Check(o)) {
        Py_ssize_t size = PyList_GET_SIZE(o);
        int indexes[size];
        for (int i=0;i<size;i++) {
            indexes[i] = _PyLong_AsInt(PyList_GetItem(o, i));
        }
        void * ptr = mi_new(code, scope, table, indexes, size);
        return PyLong_FromVoidPtr(ptr);
    }

    if (PyTuple_Check(o)) {
        Py_ssize_t size = PyTuple_GET_SIZE(o);
        int indexes[size];
        for (int i=0;i<size;i++) {
            indexes[i] = _PyLong_AsInt(PyTuple_GetItem(o, i));
        }
        void * ptr = mi_new(code, scope, table, indexes, size);
        return PyLong_FromVoidPtr(ptr);
    }
    
    return NULL;
}

static PyObject *py_mi_store(PyObject *self, PyObject *args)
{
    PyObject *o;
    char *data;
    int data_len;

    if (PyTuple_GET_SIZE(args) != 5) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    uint64_t primary_key = to_name(o);

    o = PyTuple_GetItem(args, 2);
    if (!parse_data(o, &data, &data_len)) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 4);
    uint64_t payer = to_name(o);

    o = PyTuple_GetItem(args, 3);

    if (!PyList_Check(o)) {
        PyErr_SetString(PyExc_ValueError, "wrong 4th argument type");
        return NULL;
    }

    int secondry_values_size = get_list_size(o);
    struct vm_buffer secondary_values[secondry_values_size];
    for (int i=0;i<secondry_values_size;i++) {
        PyObject *bytes = get_list_obj(o, i);
        if (!parse_data(bytes, &secondary_values[i].data, &secondary_values[i].size)) {
            return NULL;
        }
    }
    mi_store(ptr, primary_key, data, data_len, secondary_values, secondry_values_size, payer);
    Py_RETURN_NONE; 
}

static PyObject *py_mi_modify(PyObject *self, PyObject *args)
{
    PyObject *o;
    char *data;
    int data_len;

    if (PyTuple_GET_SIZE(args) != 6) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    int itr = _PyLong_AsInt(o);

    o = PyTuple_GetItem(args, 2);
    uint64_t primary_key = _PyLong_AsInt(o);

    o = PyTuple_GetItem(args, 3);
    if (!parse_data(o, &data, &data_len)) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 5);
    uint64_t payer = to_name(o);

    o = PyTuple_GetItem(args, 4);
    if (!PyList_Check(o)) {
        PyErr_SetString(PyExc_ValueError, "wrong 5th argument type");
        return NULL;
    }

    int secondry_values_size = get_list_size(o);
    struct vm_buffer secondary_values[secondry_values_size];
    for (int i=0;i<secondry_values_size;i++) {
        PyObject *bytes = get_list_obj(o, i);
        if (!parse_data(bytes, &secondary_values[i].data, &secondary_values[i].size)) {
            return NULL;
        }
    }
    mi_modify(ptr, itr, primary_key, data, data_len, secondary_values, secondry_values_size, payer);
    Py_RETURN_NONE;
}

static PyObject *py_mi_erase(PyObject *self, PyObject *args)
{
    PyObject *o;
    char *data;
    int data_len;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    int itr = _PyLong_AsInt(o);

    o = PyTuple_GetItem(args, 2);
    uint64_t primary_key = _PyLong_AsInt(o);

    mi_erase(ptr, itr, primary_key);
    Py_RETURN_NONE;
}

static PyObject *py_mi_find(PyObject *self, PyObject *args)
{
    PyObject *o;
    char *data;
    int data_len;

    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    uint64_t primary_key = to_name(o);

    int itr = mi_find(ptr, primary_key);
    return PyLong_FromLong(itr);
}

static PyObject *py_mi_get(PyObject *self, PyObject *args)
{
    PyObject *o;
    char *data;
    int data_len;

    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    uint64_t itr = _PyLong_AsInt(o);

    int size = mi_get(ptr, itr, NULL, 0);
    char *buffer[size];
    mi_get(ptr, itr, buffer, size);

    return PyBytes_FromStringAndSize(buffer, size);
}

static PyObject *py_mi_next(PyObject *self, PyObject *args)
{
    PyObject *o;

    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    int itr = _PyLong_AsInt(o);

    uint64_t primary_key = 0;
    itr = mi_next(ptr, itr, &primary_key);

    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(itr));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary_key));
    return ret;
}

static PyObject *py_mi_previous(PyObject *self, PyObject *args)
{
    PyObject *o;

    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    int itr = _PyLong_AsInt(o);

    uint64_t primary_key = 0;
    itr = mi_previous(ptr, itr, &primary_key);

    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(itr));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary_key));
    return ret;
}

#if 0
int32_t mi_lowerbound(void *ptr, uint64_t code, uint64_t scope, uint64_t table, uint64_t primary_key);

int32_t mi_upperbound(void *ptr, uint64_t code, uint64_t scope, uint64_t table, uint64_t primary_key);

int32_t mi_idx_find(void *ptr, int32_t secondary_index, uint64_t *primary_key, const void *key, uint32_t key_size);

void mi_idx_update(void *ptr, int32_t secondary_index, int32_t iterator, const void* secondary_key, uint32_t secondary_key_size, uint64_t payer );

bool mi_get_by_secondary_key(void *ptr, int32_t secondary_index, 
                            const void *secondary_key, uint32_t secondary_key_size, 
                            const void *data, uint32_t data_size);

int32_t mi_idx_next(void *ptr, int32_t secondary_index, int32_t itr_secondary, uint64_t *primary_key);

int32_t mi_idx_previous(void *ptr, int32_t secondary_index, int32_t itr_secondary, uint64_t *primary_key);

int32_t mi_idx_lowerbound(void *ptr, int32_t secondary_index, uint64_t code, uint64_t scope, uint64_t table, 
                        void *secondary, uint32_t secondary_size, uint64_t *primary_key);

int32_t mi_idx_upperbound(void *ptr, int32_t secondary_index, uint64_t code, uint64_t scope, uint64_t table, 
                        void *secondary, uint32_t secondary_size, uint64_t *primary_key);
#endif


static PyMethodDef mi_methods[] = {
    {"new",                    (PyCFunction)py_mi_new,           METH_VARARGS, NULL},
    {"store",                  (PyCFunction)py_mi_store,         METH_VARARGS, NULL},
    {"modify",                 (PyCFunction)py_mi_modify,        METH_VARARGS, NULL},
    {"erase",                  (PyCFunction)py_mi_erase,         METH_VARARGS, NULL},
    {"find",                   (PyCFunction)py_mi_find,          METH_VARARGS, NULL},
    {"get",                    (PyCFunction)py_mi_get,           METH_VARARGS, NULL},
    {"next",                   (PyCFunction)py_mi_next,          METH_VARARGS, NULL},
    {"previous",               (PyCFunction)py_mi_previous,      METH_VARARGS, NULL},
    {NULL,                      NULL}           /* sentinel */
};

static struct PyModuleDef mimodule = {
    PyModuleDef_HEAD_INIT,
    "_mi",
    NULL, //thread_doc,
    -1,
    mi_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit__mi(void)
{
    PyObject *m;
    /* Create the module and add the functions */
    return PyModule_Create(&mimodule);
}