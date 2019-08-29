
//void  eosio_assert( uint32_t test, const char* msg );
static PyObject *py_eosio_assert(PyObject *self, PyObject *args)
{
    int test;
    const char *msg;
    Py_ssize_t msg_len;
    if (!PyArg_ParseTuple(args, "ps#", &test, &msg, &msg_len)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    eosio_assert(test, msg);
    Py_RETURN_NONE;
}

//void  eosio_assert_message( uint32_t test, const char* msg, uint32_t msg_len );
static PyObject *py_eosio_assert_message(PyObject *self, PyObject *args)
{
    int test;
    const char *msg;
    Py_ssize_t msg_len;
    if (!PyArg_ParseTuple(args, "ps#", &test, &msg, &msg_len)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    eosio_assert_message(test, msg, msg_len);
    Py_RETURN_NONE;
}

//void  eosio_assert_code( uint32_t test, uint64_t code );
static PyObject *py_eosio_assert_code(PyObject *self, PyObject *args)
{
    int test;
    uint64_t code;
    if (!PyArg_ParseTuple(args, "IK", &test, &code)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    eosio_assert_code(test, code);
    Py_RETURN_NONE;
}

static PyObject *py_current_time(PyObject *self, PyObject *args)
{
    return PyLong_FromUnsignedLongLong(current_time());
}

static PyObject *py_now(PyObject *self, PyObject *args)
{
    return PyLong_FromUnsignedLongLong(current_time()/1000000);
}

//    {"current_time",             py_current_time,        METH_VARARGS, NULL},
#define SYSTEM_METHODS \
    {"current_time",             (PyCFunction)py_current_time, METH_NOARGS, NULL}, \
    {"now",                      (PyCFunction)py_now, METH_NOARGS, NULL}, \
    {"eosio_assert",             (PyCFunction)py_eosio_assert, METH_VARARGS, NULL}, \
    {"eosio_assert_message",     (PyCFunction)py_eosio_assert_message, METH_VARARGS, NULL}, \
    {"eosio_assert_code",        (PyCFunction)py_eosio_assert_code, METH_VARARGS, NULL},
