//void token_create( uint64_t issuer, void* maximum_supply, size_t size);
static PyObject *py_token_create(PyObject *self, PyObject *args)
{
    uint64_t issuer;
    uint64_t maximum_supply;
    uint64_t symbol;
    PyObject *o;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    issuer = to_name(o);
    if (issuer == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);
    maximum_supply = to_name(o);
    if (maximum_supply == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 2);
    symbol = to_name(o);
    if (symbol == 0) {
        return NULL;
    }

    token_create(issuer, maximum_supply, symbol);
    Py_RETURN_NONE;
}

//void token_issue( uint64_t to, void* quantity, size_t size1, const char* memo, size_t size2 );
static PyObject *py_token_issue(PyObject *self, PyObject *args)
{
    uint64_t to;
    uint64_t quantity;
    uint64_t symbol;
    char *memo;
    Py_ssize_t memo_len;

    PyObject *o;

    if (PyTuple_GET_SIZE(args) != 4) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    to = to_name(o);
    if (to == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);
    quantity = to_name(o);
    if (quantity == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 2);
    symbol = to_name(o);
    if (symbol == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 3);

    if (PyUnicode_Check(o)) {
        memo = (char *)PyUnicode_AsUTF8AndSize(o, &memo_len);
    } else if (PyBytes_Check(o)) {
        PyBytes_AsStringAndSize(o, &memo, &memo_len);
    } else {
        PyErr_SetString(PyExc_ValueError, "wrong 4th argument");
        return NULL;
    }

    token_issue(to, quantity, symbol, memo, memo_len);
    Py_RETURN_NONE;
}

//void token_transfer( uint64_t from, uint64_t to, uint64_t quantity, uint64_t symbol, const char* memo, size_t size2);
static PyObject *py_token_transfer(PyObject *self, PyObject *args)
{
    uint64_t from;
    uint64_t to;
    uint64_t quantity;
    uint64_t symbol;

    char *memo;
    Py_ssize_t memo_len;

    PyObject *o;

    if (PyTuple_GET_SIZE(args) != 5) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    from = to_name(o);
    if (from == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);
    to = to_name(o);
    if (to == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 2);
    quantity = to_name(o);
    if (quantity == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 3);
    symbol = to_name(o);
    if (symbol == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 4);

    if (PyUnicode_Check(o)) {
        memo = (char *)PyUnicode_AsUTF8AndSize(o, &memo_len);
    } else if (PyBytes_Check(o)) {
        PyBytes_AsStringAndSize(o, &memo, &memo_len);
    } else {
        PyErr_SetString(PyExc_ValueError, "wrong 4th argument");
        return NULL;
    }

    token_transfer(from, to, quantity, symbol, memo, memo_len);
    Py_RETURN_NONE;
}

//void token_open( uint64_t owner, uint64_t _symbol, uint64_t ram_payer );
static PyObject *py_token_open(PyObject *self, PyObject *args)
{
    uint64_t owner;
    uint64_t symbol;
    uint64_t ram_payer;
    PyObject *o;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    owner = to_name(o);
    if (owner == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);
    symbol = to_name(o);
    if (symbol == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 2);
    ram_payer = to_name(o);
    if (ram_payer == 0) {
        return NULL;
    }

    token_open(owner, symbol, ram_payer);
    Py_RETURN_NONE;
}

//void token_retire( int64_t amount, uint64_t _symbol, const char *memo, size_t memo_size );
static PyObject *py_token_retire(PyObject *self, PyObject *args)
{
    uint64_t amount;
    uint64_t symbol;
    char *memo;
    Py_ssize_t memo_len;

    PyObject *o;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    amount = to_name(o);
    if (amount == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);
    symbol = to_name(o);
    if (symbol == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 2);

    if (PyUnicode_Check(o)) {
        memo = (char *)PyUnicode_AsUTF8AndSize(o, &memo_len);
    } else if (PyBytes_Check(o)) {
        PyBytes_AsStringAndSize(o, &memo, &memo_len);
    } else {
        PyErr_SetString(PyExc_ValueError, "wrong 4th argument");
        return NULL;
    }

    token_retire(amount, symbol, memo, memo_len);
    Py_RETURN_NONE;
}

//void (*token_close)( uint64_t owner, uint64_t _symbol );
static PyObject *py_token_close(PyObject *self, PyObject *args)
{
    uint64_t owner;
    uint64_t symbol;
    PyObject *o;

    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    owner = to_name(o);
    if (owner == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);
    symbol = to_name(o);
    if (symbol == 0) {
        return NULL;
    }

    token_close(owner, symbol);
    Py_RETURN_NONE;
}

#define TOKEN_METHODS \
    {"token_create",   (PyCFunction)py_token_create, METH_VARARGS, NULL}, \
    {"token_issue",    (PyCFunction)py_token_issue, METH_VARARGS, NULL}, \
    {"token_transfer", (PyCFunction)py_token_transfer, METH_VARARGS, NULL}, \
    {"token_open", (PyCFunction)py_token_transfer, METH_VARARGS, NULL}, \
    {"token_retire", (PyCFunction)py_token_transfer, METH_VARARGS, NULL}, \
    {"token_close", (PyCFunction)py_token_transfer, METH_VARARGS, NULL},
