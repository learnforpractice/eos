
void _send_deferred(const uint128_t *sender_id, uint64_t payer, 
                    const char *serialized_transaction, 
                    size_t size, 
                    uint32_t replace_existing);
static PyObject *py_send_deferred(PyObject *self, PyObject *args)
{
    PyObject *sender_id;
    PyObject *payer;
    PyObject *serialized_transaction;
    PyObject *replace_existing;

    uint128_t _sender_id;
    uint64_t _payer;
    char *_serialized_transaction;
    Py_ssize_t _serialized_transaction_len;
    uint32_t _replace_existing;


    if (!PyTuple_Check(args)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }

    if (PyTuple_GET_SIZE(args) != 4) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    sender_id = PyTuple_GetItem(args, 0);
    payer = PyTuple_GetItem(args, 1);
    serialized_transaction = PyTuple_GetItem(args, 2);
    replace_existing = PyTuple_GetItem(args, 3);

    if (!PyLong_Check(sender_id)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count.");
        return NULL;
    }

    if (!PyLong_Check(payer)) {
        PyErr_SetString(PyExc_ValueError, "payer should be a integer type.");
        return NULL;
    }

    if (!PyBytes_Check(serialized_transaction)) {
        PyErr_SetString(PyExc_ValueError, "serialized_transaction should be a bytes type.");
        return NULL;
    }

    if (!PyLong_Check(replace_existing)) {
        PyErr_SetString(PyExc_ValueError, "replace_existing should be a integer type.");
        return NULL;
    }

    _PyLong_AsByteArray((PyLongObject *)sender_id, &_sender_id, sizeof(uint128_t), 1, 0);
    _payer = PyLong_AsUnsignedLongLong(payer);
    PyBytes_AsStringAndSize(serialized_transaction, &_serialized_transaction, &_serialized_transaction_len);
    _replace_existing = PyLong_AsLong(replace_existing);

    _send_deferred(&_sender_id, &_payer, _serialized_transaction, _serialized_transaction_len, _replace_existing);

    Py_RETURN_NONE;
}

int _cancel_deferred(const uint128_t *sender_id);
static PyObject *py_cancel_deferred(PyObject *self, PyObject *args)
{
    uint128_t sender_id;
    if (PyTuple_GET_SIZE(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "argument number must be 1.");
        return NULL;
    }
    
    PyObject *o = PyTuple_GetItem(args, 0);

    _PyLong_AsByteArray((PyLongObject *)o, &sender_id, sizeof(uint128_t), 1, 0);
    int ret = _cancel_deferred(&sender_id);
    return PyLong_FromLong(ret);
}

//size_t read_transaction(char *buffer, size_t size);
static PyObject *py_read_transaction(PyObject *self, PyObject *args)
{
    char *buffer;
    size_t size = read_transaction(NULL, 0);
    buffer = (char *)malloc(size);
    read_transaction(buffer, size);
    return PyBytes_FromStringAndSize(buffer, size);
}

//size_t transaction_size();
static PyObject *py_transaction_size(PyObject *self, PyObject *args)
{
    return PyLong_FromLong(transaction_size());
}

//int tapos_block_num();
static PyObject *py_tapos_block_num(PyObject *self, PyObject *args)
{
    return PyLong_FromUnsignedLong((uint32_t)tapos_block_num());
}

//int tapos_block_prefix();
static PyObject *py_tapos_block_prefix(PyObject *self, PyObject *args)
{
    return PyLong_FromUnsignedLong((uint32_t)tapos_block_prefix());
}

uint32_t expiration(void);
static PyObject *py_expiration(PyObject *self, PyObject *args)
{
    return PyLong_FromUnsignedLong(expiration());
}

int get_action( uint32_t type, uint32_t index, char* buff, size_t size );
static PyObject *py_get_action(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t type;
    uint64_t index;
    char *data;
    Py_ssize_t len;

    if (!PyTuple_Check(args)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }

    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    type = PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 1);
    index = PyLong_AsUnsignedLongLong(o);
    len = get_action(type, index, NULL, 0);
    if (len <= 0) {
        Py_RETURN_NONE;
    }
    data = (char *)malloc(len);
    get_action((uint32_t)type, (uint32_t)index, data, len);
    o = PyBytes_FromStringAndSize(data, len);
    free(data);
    return o;
}

//int get_context_free_data( uint32_t index, char* buff, size_t size );
static PyObject *py_get_context_free_data(PyObject *self, PyObject *args)
{
    uint32_t index;
    char *buff;
    if (!PyArg_ParseTuple(args, "I", &index)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    int size = get_context_free_data(index, NULL, size);
    if (size <= 0) {
        Py_RETURN_NONE;
    }
    buff = (char *)malloc(size);
    get_context_free_data(index, buff, size);
    free(buff);
    return PyBytes_FromStringAndSize(buff, size);
}

#if 0
void assert_privileged();
static PyObject *py_assert_privileged(PyObject *self, PyObject *args) {
    assert_privileged();
}

void assert_context_free();
static PyObject *py_assert_context_free(PyObject *self, PyObject *args) {
    assert_context_free();
}
#endif

#define TRANSACTION_METHODS \
    {"send_deferred",           (PyCFunction)py_send_deferred, METH_VARARGS, NULL}, \
    {"cancel_deferred",         (PyCFunction)py_cancel_deferred, METH_VARARGS, NULL}, \
    {"transaction_cancel",         (PyCFunction)py_cancel_deferred, METH_VARARGS, NULL}, \
    {"read_transaction",        (PyCFunction)py_read_transaction, METH_VARARGS, NULL}, \
    {"transaction_size",        (PyCFunction)py_transaction_size, METH_VARARGS, NULL}, \
    {"tapos_block_num",         (PyCFunction)py_tapos_block_num, METH_VARARGS, NULL}, \
    {"tapos_block_prefix",      (PyCFunction)py_tapos_block_prefix, METH_VARARGS, NULL}, \
    {"expiration",              (PyCFunction)py_expiration, METH_VARARGS, NULL}, \
    {"get_action",              (PyCFunction)py_get_action, METH_VARARGS, NULL}, \
    {"get_context_free_data",   (PyCFunction)py_get_context_free_data, METH_VARARGS, NULL},

//    {"assert_privileged",   (PyCFunction)py_assert_privileged, METH_VARARGS, NULL},
//    {"assert_context_free",   (PyCFunction)py_assert_context_free, METH_VARARGS, NULL},    
