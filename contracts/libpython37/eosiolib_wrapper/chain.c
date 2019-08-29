//uint32_t get_active_producers( account_name* producers, uint32_t datalen );
static PyObject *py_get_active_producers(PyObject *self, PyObject *args)
{
    uint64_t *producers;
    uint32_t size;

    size = get_active_producers(NULL, 0);
    producers = (uint64_t *)malloc(size);
    get_active_producers(producers, size);
    PyObject *o = PyTuple_New(size/sizeof(uint64_t));
    for (int i=0;i<size/sizeof(uint64_t);i++) {
        PyObject *item = PyLong_FromUnsignedLongLong(producers[i]);
        PyTuple_SetItem(o, i, item);
    }
    return o;
}

#define CHAIN_METHODS \
    {"get_active_producers",      (PyCFunction)py_get_active_producers, METH_VARARGS, NULL}, \
