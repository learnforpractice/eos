#include <mi.h>
#include <Python.h>

uint64_t to_name(PyObject *o);
int o2n(PyObject *o, uint64_t* n);

int get_secondary_value(PyObject *o, void *mi, int index, char *secondary_key, int secondary_key_len) {
    int indexes[MAX_INDEXES];
    int count = mi_get_indexes(mi, indexes, MAX_INDEXES);
    if (index >= count) {
        PyErr_SetString(PyExc_ValueError, "index overflow");
        return 0;
    }

    int index_type = indexes[index];

    if (index_type == idx64) {
        if (!PyLong_Check(o)) {
            PyErr_SetString(PyExc_ValueError, "bad index type");
            return 0;
        }
        if (secondary_key_len < sizeof(uint64_t)) {
            PyErr_SetString(PyExc_ValueError, "secondary key buffer size not large enough");
            return 0;
        }
        uint64_t value = PyLong_AsUnsignedLongLong(o);
        memcpy(secondary_key, &value, sizeof(uint64_t));
        return sizeof(uint64_t);
    } else if (index_type == idx128) {
        if (!PyLong_Check(o)) {
            PyErr_SetString(PyExc_ValueError, "bad index type");
            return 0;
        }
        if (secondary_key_len < sizeof(uint64_t)*2) {
            PyErr_SetString(PyExc_ValueError, "secondary key buffer size not large enough");
            return 0;
        }
        if (-1 == _PyLong_AsByteArray((PyLongObject *)o, (unsigned char *)&secondary_key, 16, 1, 0)) {
            return 0;
        }
        return 16;
    } else if (index_type == idx256) {
        if (!PyLong_Check(o)) {
            PyErr_SetString(PyExc_ValueError, "bad index type");
            return 0;
        }
        if (secondary_key_len < sizeof(uint64_t)*4) {
            PyErr_SetString(PyExc_ValueError, "secondary key buffer size not large enough");
            return 0;
        }
        if (-1 == _PyLong_AsByteArray((PyLongObject *)o, (unsigned char *)&secondary_key, 32, 1, 0)) {
            return 0;
        }
        return 32;
    } else if (index_type == idx_double) {
        if (!PyFloat_Check(o)) {
            PyErr_SetString(PyExc_ValueError, "bad index type");
            return 0;
        }
        if (secondary_key_len < sizeof(double)) {
            PyErr_SetString(PyExc_ValueError, "secondary key buffer size not large enough");
            return 0;
        }
        double d = PyFloat_AsDouble(o);
        memcpy(secondary_key, &d, sizeof(double));
        return sizeof(double);
    } else if (index_type == idx_long_double) {
        if (!PyBytes_Check(o)) {
            PyErr_SetString(PyExc_ValueError, "bad index type");
            return 0;
        }
        if (secondary_key_len < 16) {
            PyErr_SetString(PyExc_ValueError, "secondary key buffer size not large enough");
            return 0;
        }
        char *buffer;
        int size = 0;
        PyBytes_AsStringAndSize(o, &buffer, &size);
        if (size != 16) {
            PyErr_SetString(PyExc_ValueError, "bytes sould be 16 bytes long");
            return 0;
        }
        memcpy(secondary_key, buffer, size);
        return 16;
    } else {
        return 0;
    }
}

int get_list_size(PyObject *o) {
    if (PyTuple_Check(o)) {
        return PyTuple_GET_SIZE(o);
    }
    if (PyList_Check(o)) {
        return PyList_GET_SIZE(o);
    }
    PyErr_SetString(PyExc_ValueError, "object should be a list or tuple type");
    return -1;
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

int parse_bytes(PyObject *arg, char **data, int *len) {
    if (!PyBytes_Check(arg)) {
        PyErr_SetString(PyExc_ValueError, "bad data type.");
        return 0;
    }
    return PyBytes_AsStringAndSize(arg, data, len) != -1;
}

int parse_arg(PyObject *args, int index, uint64_t *arg) {
    PyObject *o;
    o = PyTuple_GetItem(args, index);
    if (o == NULL) {
        PyErr_SetString(PyExc_ValueError, "arg not exist at index");
        return 0;
    }
    return o2n(o, arg);
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
    int size = get_list_size(o);
    if (size == -1) {
        return NULL;
    }
    int indexes[size];
    for (int i=0;i<size;i++) {
        PyObject *oo = get_list_obj(o, i);
        if (!PyLong_Check(oo)) {
            PyErr_SetString(PyExc_ValueError, "secondary index type shoud be an int object");
            return NULL;
        }
        int index_type = _PyLong_AsInt(oo);
        if (index_type >=idx64 && index_type <= idx_long_double) {
            indexes[i] = index_type;
        } else {
            PyErr_SetString(PyExc_ValueError, "wrong index type");
            return NULL;
        }
    }
    void * ptr = mi_new(code, scope, table, indexes, size);
    return PyLong_FromVoidPtr(ptr);
}

static PyObject *py_mi_store(PyObject *self, PyObject *args)
{
    PyObject *o;
    char *data;
    int data_len;
    struct vm_buffer secondary_values[MAX_INDEXES];
    char secondary_keys[MAX_INDEXES][32];

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

    int secondary_values_size = get_list_size(o);
    if (-1 == secondary_values_size) {
        return NULL;
    }
    if (secondary_values_size > MAX_INDEXES) {
        PyErr_SetString(PyExc_ValueError, "secondary indexes count should be <= 12");
        return NULL;
    }
    for (int i=0;i<secondary_values_size;i++) {
        PyObject *oo = get_list_obj(o, i);
        int size = get_secondary_value(oo, ptr, i, secondary_keys[i], 32);
        if (size == 0) {
            return NULL;
        }
        secondary_values[i].data = secondary_keys[i];
        secondary_values[i].size = size;
    }
    mi_store(ptr, primary_key, data, data_len, secondary_values, secondary_values_size, payer);
    Py_RETURN_NONE; 
}

static PyObject *py_mi_modify(PyObject *self, PyObject *args)
{
    PyObject *o;
    char *data;
    int data_len;
    struct vm_buffer secondary_values[MAX_INDEXES];
    char secondary_keys[MAX_INDEXES][32];

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

    int secondary_values_size = get_list_size(o);
    if (-1 == secondary_values_size) {
        return NULL;
    }
    if (secondary_values_size > MAX_INDEXES) {
        PyErr_SetString(PyExc_ValueError, "secondary indexes count should be <= 12");
        return NULL;
    }
    for (int i=0;i<secondary_values_size;i++) {
        PyObject *oo = get_list_obj(o, i);
        int size = get_secondary_value(oo, ptr, i, secondary_keys[i], 32);
        if (size == 0) {
            return NULL;
        }
        secondary_values[i].data = secondary_keys[i];
        secondary_values[i].size = size;
    }

    mi_modify(ptr, itr, primary_key, data, data_len, secondary_values, secondary_values_size, payer);
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
    uint64_t primary_key;

    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    
    if (!o2n(o, &primary_key)) {
        return NULL;
    }

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

    struct vm_buffer vb;
    mi_get(ptr, itr, &vb);
    o = PyBytes_FromStringAndSize((char *)vb.data, vb.size);
    free(vb.data);
    return o;
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

static PyObject *py_mi_lowerbound(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint64_t primary_key;

    if (PyTuple_GET_SIZE(args) != 5) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    if (!parse_arg(args, 1, &code)) {
        return NULL;
    }
    if (!parse_arg(args, 2, &scope)) {
        return NULL;
    }
    if (!parse_arg(args, 3, &table)) {
        return NULL;
    }
    if (!parse_arg(args, 4, &primary_key)) {
        return NULL;
    }
    int itr = mi_lowerbound(ptr, code, scope, table, primary_key);
    return PyLong_FromLong(itr);
}

static PyObject *py_mi_upperbound(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint64_t primary_key;

    if (PyTuple_GET_SIZE(args) != 5) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    if (!parse_arg(args, 1, &code)) {
        return NULL;
    }
    if (!parse_arg(args, 2, &scope)) {
        return NULL;
    }
    if (!parse_arg(args, 3, &table)) {
        return NULL;
    }
    if (!parse_arg(args, 4, &primary_key)) {
        return NULL;
    }
    int itr = mi_upperbound(ptr, code, scope, table, primary_key);
    return PyLong_FromLong(itr);
}

static PyObject *py_mi_idx_find(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t primary_key = 0;
    char *secondary_key;
    Py_ssize_t secondary_key_len;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }    
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    int32_t secondary_index = _PyLong_AsInt(o);

    o = PyTuple_GetItem(args, 2);
    if (!parse_bytes(o, &secondary_key, &secondary_key_len)) {
        return NULL;
    }

    int32_t itr = mi_idx_find(ptr, secondary_index, &primary_key, secondary_key, secondary_key_len);

    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(itr));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary_key));
    return ret;
}

static PyObject *py_mi_idx_update(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t payer = 0;
    char *secondary_key;
    Py_ssize_t secondary_key_len;

    if (PyTuple_GET_SIZE(args) != 5) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }    
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    int32_t secondary_index = _PyLong_AsInt(o);

    o = PyTuple_GetItem(args, 2);
    int32_t iterator = _PyLong_AsInt(o);

    o = PyTuple_GetItem(args, 3);
    if (!parse_bytes(o, &secondary_key, &secondary_key_len)) {
        return NULL;
    }

    if (!parse_arg(args, 4, &payer)) {
        return NULL;
    }
    mi_idx_update(ptr, secondary_index, iterator, secondary_key, secondary_key_len, payer);

    Py_RETURN_NONE;
}

static PyObject *py_mi_get_by_secondary_key(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t payer = 0;
    char *secondary_key;
    Py_ssize_t secondary_key_len;
    char *data;
    int data_len;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }    
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    int32_t secondary_index = _PyLong_AsInt(o);

    o = PyTuple_GetItem(args, 2);
    if (!parse_bytes(o, &secondary_key, &secondary_key_len)) {
        return NULL;
    }

    struct vm_buffer vb;
    if (!mi_get_by_secondary_key(ptr, secondary_index, secondary_key, secondary_key_len, &vb)) {
        Py_RETURN_NONE;
    }
    
    o = PyBytes_FromStringAndSize(vb.data, vb.size);
    free(vb.data);
    return o;
}

static PyObject *py_mi_idx_next(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t primary_key = 0;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }    
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    int32_t secondary_index = _PyLong_AsInt(o);

    o = PyTuple_GetItem(args, 2);
    int32_t itr_secondary = _PyLong_AsInt(o);

    int itr = mi_idx_next(ptr, secondary_index, itr_secondary, &primary_key);


    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(itr));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary_key));
    return ret;
}


static PyObject *py_mi_idx_previous(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t primary_key = 0;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }    
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    int32_t secondary_index = _PyLong_AsInt(o);

    o = PyTuple_GetItem(args, 2);
    int32_t itr_secondary = _PyLong_AsInt(o);

    int itr = mi_idx_previous(ptr, secondary_index, itr_secondary, &primary_key);

    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(itr));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary_key));
    return ret;
}

static PyObject *py_mi_idx_lowerbound(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint64_t primary_key = 0;

    char *secondary_key;
    int secondary_key_len;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }    
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    int32_t secondary_index = _PyLong_AsInt(o);

    if (!parse_arg(args, 2, &code)) {
        return NULL;
    }
    if (!parse_arg(args, 3, &scope)) {
        return NULL;
    }
    if (!parse_arg(args, 4, &table)) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 5);
    if (!parse_data(o, &secondary_key, &secondary_key_len)) {
        return NULL;
    }

    int itr = mi_idx_lowerbound(ptr, secondary_index, code, scope, table, secondary_key, secondary_key_len, &primary_key);

    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(itr));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary_key));
    return ret;
}

static PyObject *py_mi_idx_upperbound(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint64_t primary_key = 0;

    char *secondary_key;
    int secondary_key_len;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }    
    o = PyTuple_GetItem(args, 0);
    void *ptr = PyLong_AsVoidPtr(o);

    o = PyTuple_GetItem(args, 1);
    int32_t secondary_index = _PyLong_AsInt(o);

    if (!parse_arg(args, 2, &code)) {
        return NULL;
    }
    if (!parse_arg(args, 3, &scope)) {
        return NULL;
    }
    if (!parse_arg(args, 4, &table)) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 5);
    if (!parse_data(o, &secondary_key, &secondary_key_len)) {
        return NULL;
    }

    int itr = mi_idx_upperbound(ptr, secondary_index, code, scope, table, secondary_key, secondary_key_len, &primary_key);

    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(itr));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary_key));
    return ret;
}

static PyMethodDef mi_methods[] = {
    {"new",                    (PyCFunction)py_mi_new,           METH_VARARGS, NULL},
    {"store",                  (PyCFunction)py_mi_store,         METH_VARARGS, NULL},
    {"modify",                 (PyCFunction)py_mi_modify,        METH_VARARGS, NULL},
    {"erase",                  (PyCFunction)py_mi_erase,         METH_VARARGS, NULL},
    {"find",                   (PyCFunction)py_mi_find,          METH_VARARGS, NULL},
    {"get",                    (PyCFunction)py_mi_get,           METH_VARARGS, NULL},
    {"next",                   (PyCFunction)py_mi_next,          METH_VARARGS, NULL},
    {"previous",               (PyCFunction)py_mi_previous,      METH_VARARGS, NULL},
    {"upperbound",             (PyCFunction)py_mi_upperbound,    METH_VARARGS, NULL},
    {"lowerbound",             (PyCFunction)py_mi_lowerbound,    METH_VARARGS, NULL},
    {"idx_find",               (PyCFunction)py_mi_idx_find,      METH_VARARGS, NULL},
    {"idx_update",             (PyCFunction)py_mi_idx_update,    METH_VARARGS, NULL},
    {"get_by_secondary_key",   (PyCFunction)py_mi_get_by_secondary_key,      METH_VARARGS, NULL},
    {"idx_next",               (PyCFunction)py_mi_idx_next,      METH_VARARGS, NULL},
    {"idx_previous",           (PyCFunction)py_mi_idx_previous,      METH_VARARGS, NULL},
    {"idx_upperbound",         (PyCFunction)py_mi_idx_upperbound,      METH_VARARGS, NULL},
    {"idx_lowerbound",         (PyCFunction)py_mi_idx_lowerbound,      METH_VARARGS, NULL},    
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