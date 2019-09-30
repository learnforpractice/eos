

//void get_resource_limits( account_name account, int64_t* ram_bytes, int64_t* net_weight, int64_t* cpu_weight );
static PyObject *py_get_resource_limits(PyObject *self, PyObject *args)
{
    uint64_t account;
    int64_t ram_bytes;
    int64_t net_weight;
    int64_t cpu_weight;

    if (!PyArg_ParseTuple(args, "K", &account)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    get_resource_limits(account, &ram_bytes, &net_weight, &cpu_weight);
    PyObject *ret = PyTuple_New(3);
    PyTuple_SetItem(ret, 0, PyLong_FromLongLong(ram_bytes));
    PyTuple_SetItem(ret, 1, PyLong_FromLongLong(net_weight));
    PyTuple_SetItem(ret, 2, PyLong_FromLongLong(cpu_weight));
    return ret;
}

//void set_resource_limits( account_name account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight );
static PyObject *py_set_resource_limits(PyObject *self, PyObject *args)
{
    uint64_t account;
    int64_t ram_bytes;
    int64_t net_weight;
    int64_t cpu_weight;

    if (!PyArg_ParseTuple(args, "KLLL", &account, &ram_bytes, &net_weight, &cpu_weight)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    set_resource_limits(account, ram_bytes, net_weight, cpu_weight);
    Py_RETURN_TRUE;
}

//int64_t set_proposed_producers( char *producer_data, uint32_t producer_data_size );
static PyObject *py_set_proposed_producers(PyObject *self, PyObject *args)
{
    char *producer_data;
    uint32_t producer_data_size;
    if (!PyArg_ParseTuple(args, "s#", &producer_data, &producer_data_size)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    set_proposed_producers(producer_data, producer_data_size);
    Py_RETURN_NONE;
}

#if 0
//void set_active_producers( char *producer_data, uint32_t producer_data_size );
static PyObject *py_set_active_producers(PyObject *self, PyObject *args)
{
    char *producer_data;
    uint32_t producer_data_size;
    if (!PyArg_ParseTuple(args, "s#", &producer_data, &producer_data_size)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    set_active_producers(producer_data, producer_data_size);
    Py_RETURN_NONE;
}
#endif

//bool is_privileged( account_name account );
static PyObject *py_is_privileged(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t name;
    
    if (PyTuple_GET_SIZE(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    name = to_name(o);
    if (name == 0) {
        return NULL;
    }
    if (is_privileged(name)) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

//void set_privileged( account_name account, bool is_priv );
static PyObject *py_set_privileged(PyObject *self, PyObject *args)
{
    uint64_t account;
    int is_priv;
    if (!PyArg_ParseTuple(args, "Ki", &account, &is_priv)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    set_privileged(account, is_priv);
    Py_RETURN_NONE;
}

//void set_blockchain_parameters_packed(char* data, uint32_t datalen);
static PyObject *py_set_blockchain_parameters_packed(PyObject *self, PyObject *args)
{
    char *data;
    Py_ssize_t data_len;
    if (!PyArg_ParseTuple(args, "s#", &data, &data_len)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    set_blockchain_parameters_packed(data, data_len);
    Py_RETURN_NONE;
}

//uint32_t get_blockchain_parameters_packed(char* data, uint32_t datalen);
static PyObject *py_get_blockchain_parameters_packed(PyObject *self, PyObject *args)
{
    char buf[84];//char buf[sizeof(eosio::blockchain_parameters)]
    int ret = get_blockchain_parameters_packed(buf, sizeof(buf));
    return PyBytes_FromStringAndSize(buf, sizeof(buf));
}

//void activate_feature( int64_t f );
static PyObject *py_activate_feature(PyObject *self, PyObject *args)
{
    int64_t f;
    if (!PyArg_ParseTuple(args, "L", &f)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    activate_feature(f);
    Py_RETURN_NONE;
}

//    {"set_active_producers",                (PyCFunction)py_set_active_producers, METH_VARARGS, NULL},

#define PRIVILEGED_METHODS \
    {"get_resource_limits",                 (PyCFunction)py_get_resource_limits, METH_VARARGS, NULL}, \
    {"set_resource_limits",                 (PyCFunction)py_set_resource_limits, METH_VARARGS, NULL}, \
    {"set_proposed_producers",              (PyCFunction)py_set_proposed_producers, METH_VARARGS, NULL}, \
    {"is_privileged",                       (PyCFunction)py_is_privileged, METH_VARARGS, NULL}, \
    {"set_privileged",                      (PyCFunction)py_set_privileged, METH_VARARGS, NULL}, \
    {"set_blockchain_parameters_packed",    (PyCFunction)py_set_blockchain_parameters_packed, METH_VARARGS, NULL}, \
    {"get_blockchain_parameters_packed",    (PyCFunction)py_get_blockchain_parameters_packed, METH_VARARGS, NULL}, \
    {"activate_feature",                    (PyCFunction)py_activate_feature, METH_VARARGS, NULL},
