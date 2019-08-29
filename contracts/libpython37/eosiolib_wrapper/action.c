
//uint32_t read_action_data( void* msg, uint32_t len );
static PyObject *py_read_action_data(PyObject *self, PyObject *args)
{
    uint32_t size = read_action_data(NULL, 0);
    if (size <= 0) {
        Py_RETURN_NONE;
    }
    char *buffer = (char *)malloc(size);
    read_action_data(buffer, size);
//    prints("++++read_action_data size:");printi(size);prints("\n");
    PyObject *ret = PyBytes_FromStringAndSize(buffer, size);
    free(buffer);
    return ret;
}

//uint32_t action_data_size(void);
static PyObject *py_action_data_size(PyObject *self, PyObject *args)
{
    return PyLong_FromUnsignedLongLong(action_data_size());
}

//void require_recipient( account_name name );
static PyObject *py_require_recipient(PyObject *self, PyObject *args)
{
    PyObject *o = NULL;
    uint64_t name = 0;
    const char *cname = NULL;
    Py_ssize_t size = 0;
    if (PyTuple_GET_SIZE(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    name = to_name(o);
    if (name == 0) {
        return NULL;
    }

    require_recipient(name);
    Py_RETURN_NONE;
}

//void require_auth( account_name name );
static PyObject *py_require_auth(PyObject *self, PyObject *args)
{
    PyObject *o = NULL;
    uint64_t name = 0;
    const char *cname = NULL;
    Py_ssize_t size = 0;
    if (PyTuple_GET_SIZE(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    name = to_name(o);
    if (name == 0) {
        return NULL;
    }

    require_auth(name);
    Py_RETURN_NONE;
}

//void require_auth2( account_name name, permission_name permission );
static PyObject *py_require_auth2(PyObject *self, PyObject *args)
{
    PyObject *o = NULL;

    uint64_t permission = 0;
    uint64_t name = 0;

    const char *cname = NULL;
    Py_ssize_t size = 0;

    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    name = to_name(o);
    if (name == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);
    permission = to_name(o);
    if (permission == 0) {
        return NULL;
    }

    require_auth2(name, permission);
    Py_RETURN_NONE;
}

//bool has_auth( account_name name );
static PyObject *py_has_auth(PyObject *self, PyObject *args)
{
    PyObject *o = NULL;
    uint64_t name = 0;
    const char *cname = NULL;
    Py_ssize_t size = 0;
    if (PyTuple_GET_SIZE(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    name = to_name(o);
    if (name == 0) {
        return NULL;
    }

    if (has_auth(name)) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

//bool is_account( account_name name );
static PyObject *py_is_account(PyObject *self, PyObject *args)
{
    PyObject *o = NULL;
    uint64_t name = 0;
    const char *cname = NULL;
    Py_ssize_t size = 0;
    if (PyTuple_GET_SIZE(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    name = to_name(o);
    if (name == 0) {
        return NULL;
    }

    if (is_account(name)) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

void pythonvm_send_inline(uint64_t account, uint64_t action_name, uint64_t actor, uint64_t permission, char *data, size_t len);

static PyObject *py_send_inline(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t account;
    uint64_t action_name;
    uint64_t actor;
    uint64_t permission;
    
    char *data;
    Py_ssize_t len;

    if (!PyTuple_Check(args)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }

    if (PyTuple_GET_SIZE(args) != 5) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    account = to_name(o);
    if (account == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);
    action_name = to_name(o);
    if (action_name == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 2);
    actor = to_name(o);
    if (actor == 0) {
        return NULL;
    }


    o = PyTuple_GetItem(args, 3);
    permission = to_name(o);
    if (permission == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 4);
    if (!PyBytes_Check(o)) {
        PyErr_SetString(PyExc_ValueError, "5th parameters should be a bytes type.");
        return NULL;
    }

    PyBytes_AsStringAndSize(o, &data, &len);
//    prints("+++++data is: ");prints(data);prints("\n");

    pythonvm_send_inline(account, action_name, actor, permission, data, len);
    
    Py_RETURN_NONE;
}

//   void send_inline(char *serialized_action, size_t size);
static PyObject *py_send_inline_raw(PyObject *self, PyObject *args)
{
    char *data;
    Py_ssize_t len;
    
    if (!PyTuple_Check(args)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }

    if (PyTuple_GET_SIZE(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    PyObject *action = PyTuple_GetItem(args, 0);
    if (!PyBytes_Check(action)) {
        PyErr_SetString(PyExc_ValueError, "serialized_action parameters should be a bytes type.");
        return NULL;
    }

    PyBytes_AsStringAndSize(action, &data, &len);

    send_inline(data, len);
    Py_RETURN_NONE;
}

//void send_context_free_inline(char *serialized_action, size_t size);
static PyObject *py_send_context_free_inline(PyObject *self, PyObject *args)
{
    char *data;
    Py_ssize_t len;
    if (!PyArg_ParseTuple(args, "s#", &data, &len)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    send_context_free_inline(data, len);
    Py_RETURN_NONE;
}

//uint64_t  publication_time( void );
static PyObject *py_publication_time(PyObject *self, PyObject *args)
{
    uint64_t n = publication_time();
    return PyLong_FromUnsignedLongLong(n);
}

//account_name current_receiver( void );
static PyObject *py_current_receiver(PyObject *self, PyObject *args)
{
    uint64_t n = current_receiver();
    return PyLong_FromUnsignedLongLong(n);
}

static PyObject *py_string_to_name(PyObject *self, PyObject *args)
{
    const char *str;
    Py_ssize_t str_len;
    if (!PyArg_ParseTuple(args, "s#", &str, &str_len)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    uint64_t n = s2n(str, str_len);
    return PyLong_FromUnsignedLongLong(n);
}

static PyObject *py_name_to_string(PyObject *self, PyObject *args)
{
    uint64_t n;
    char name[13];
    if (!PyArg_ParseTuple(args, "K", &n)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    int size = n2s(n, name, sizeof(name));
    return PyUnicode_FromStringAndSize(name, size);
}


void* pythonvm_action_new(uint64_t account, 
                                    uint64_t act_name, 
                                    uint64_t actor,
                                    uint64_t permission,
                                    const char *data,
                                    size_t len);
static PyObject *py_action_new(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t account;
    uint64_t action_name;
    uint64_t actor;
    uint64_t permission;
    
    char *data;
    Py_ssize_t len;

    if (!PyTuple_Check(args)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }

    if (PyTuple_GET_SIZE(args) != 5) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    account = PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 1);
    action_name = PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 2);
    actor = PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 3);
    permission = PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 4);
    if (!PyBytes_Check(o)) {
        PyErr_SetString(PyExc_ValueError, "5th parameters should be a bytes type.");
        return NULL;
    }

    PyBytes_AsStringAndSize(o, &data, &len);
//    prints("+++++data is: ");prints(data);prints("\n");

    void * ptr = pythonvm_action_new(account, action_name, actor, permission, data, len);
    return PyLong_FromUnsignedLongLong((uint64_t)ptr);
}

void pythonvm_action_add_permission(void *_a, uint64_t actor, uint64_t permission);

static PyObject *py_action_add_permission(PyObject *self, PyObject *args)
{
    PyObject *o;
    void *a;
    uint64_t actor;
    uint64_t permission;
    
    if (!PyTuple_Check(args)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    a  = (void *)PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 1);
    actor = PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 2);
    permission = PyLong_AsUnsignedLongLong(o);
    pythonvm_action_add_permission(a, actor, permission);
    Py_RETURN_NONE;
}

void pythonvm_send_inline2(void *_a);
static PyObject *py_send_inline2(PyObject *self, PyObject *args)
{
    PyObject *o;
    void *a;
    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    a  = (void *)PyLong_AsUnsignedLongLong(o);
    pythonvm_send_inline2(a);
    Py_RETURN_NONE;
}

void pythonvm_action_free(void *a);
static PyObject *py_action_free(PyObject *self, PyObject *args)
{
    PyObject *o;
    void *a;
    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    a  = (void *)PyLong_AsUnsignedLongLong(o);
    pythonvm_action_free(a);
    Py_RETURN_NONE;
}

void* pythonvm_transaction_new(uint32_t expiration, 
                                    uint32_t ref_block_num, 
                                    uint32_t ref_block_prefix, 
                                    uint32_t max_net_usage_words, 
                                    uint32_t max_cpu_usage_ms, 
                                    uint32_t delay_sec);
static PyObject *py_transaction_new(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint32_t expiration;
    uint32_t ref_block_num;
    uint32_t ref_block_prefix;
    uint32_t max_net_usage_words;
    uint32_t max_cpu_usage_ms;
    uint32_t delay_sec;

    if (PyTuple_GET_SIZE(args) != 6) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    expiration  = (uint32_t)PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 1);
    ref_block_num  = (uint32_t)PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 2);
    ref_block_prefix  = (uint32_t)PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 3);
    max_net_usage_words  = (uint32_t)PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 4);
    max_cpu_usage_ms  = (uint32_t)PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 5);
    delay_sec  = (uint32_t)PyLong_AsUnsignedLongLong(o);

    void *ptr = pythonvm_transaction_new(expiration, ref_block_num, ref_block_prefix, max_net_usage_words, max_cpu_usage_ms, delay_sec);
    return PyLong_FromUnsignedLongLong((uint64_t)ptr);
}

void pythonvm_transaction_add_action(void *_t, void *_a);
static PyObject *py_transaction_add_action(PyObject *self, PyObject *args)
{
    PyObject *o;
    void *a;
    void *t;
    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    t  = (void *)PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 1);
    a  = (void *)PyLong_AsUnsignedLongLong(o);

    pythonvm_transaction_add_action(t, a);
    Py_RETURN_NONE;
}

void pythonvm_transaction_add_context_free_action(void *_t, void *_a);
static PyObject *py_transaction_add_context_free_action(PyObject *self, PyObject *args)
{
    PyObject *o;
    void *a;
    void *t;
    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    t  = (void *)PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 1);
    a  = (void *)PyLong_AsUnsignedLongLong(o);

    pythonvm_transaction_add_context_free_action(t, a);
    Py_RETURN_NONE;
}

void pythonvm_transaction_send(void *t, uint128_t sender_id, uint64_t payer, bool replace_existing);
static PyObject *py_transaction_send(PyObject *self, PyObject *args)
{
    PyObject *o;
    void *t;
    uint128_t _sender_id;
    uint64_t payer;
    int _replace_existing;

    if (PyTuple_GET_SIZE(args) != 4) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    t  = (void *)PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 1);
    _PyLong_AsByteArray((PyLongObject *)o, &_sender_id, sizeof(uint128_t), 1, 0);

    o = PyTuple_GetItem(args, 2);
    payer  = PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 3);
    _replace_existing = PyLong_AsLong(o);

    pythonvm_transaction_send(t, _sender_id, payer, _replace_existing);
    Py_RETURN_NONE;
}

void pythonvm_transaction_free(void *t);
static PyObject *py_transaction_free(PyObject *self, PyObject *args)
{
    PyObject *o;
    void *a;
    if (PyTuple_GET_SIZE(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    a  = (void *)PyLong_AsUnsignedLongLong(o);
    pythonvm_transaction_free(a);
    Py_RETURN_NONE;
}

void call_contract(uint64_t contract, uint64_t func_name, uint64_t arg1, uint64_t arg2, uint64_t arg3, void* extra_args, size_t size1);
int call_contract_get_results(void* result, size_t size1);

static PyObject *py_call_contract(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t contract;
    uint64_t func_name;
    uint64_t arg1;
    uint64_t arg2; 
    uint64_t arg3;
    char* extra_args;
    size_t size1;
    char* results;
    size_t size2;

    if (PyTuple_GET_SIZE(args) != 6) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }
    o = PyTuple_GetItem(args, 0);
    contract = to_name(o);

    o = PyTuple_GetItem(args, 1);
    func_name = to_name(o);

    o = PyTuple_GetItem(args, 2);
    if (!PyLong_Check(o)) {
        PyErr_SetString(PyExc_ValueError, "3th parameter should be a int type.");
        return NULL;
    }
    arg1 = PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 3);
    if (!PyLong_Check(o)) {
        PyErr_SetString(PyExc_ValueError, "4th parameter should be a int type.");
        return NULL;
    }
    arg2 = PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 4);
    if (!PyLong_Check(o)) {
        PyErr_SetString(PyExc_ValueError, "5th parameter should be a int type.");
        return NULL;
    }
    arg3 = PyLong_AsUnsignedLongLong(o);

    o = PyTuple_GetItem(args, 5);
    PyBytes_AsStringAndSize(o, &extra_args, &size1);

    call_contract(contract, func_name, arg1, arg2, arg3, extra_args, size1);

    int results_size = call_contract_get_results(NULL, 0);
    if (results_size) {
        results = (char *)malloc(results_size);
        call_contract_get_results(results, results_size);
        return PyBytes_FromStringAndSize(results, size2);
    } else {
        Py_RETURN_NONE;
    }
}


#define ACTION_METHODS \
    {"read_action_data",        (PyCFunction)py_read_action_data, METH_VARARGS, NULL}, \
    {"action_data_size",        (PyCFunction)py_action_data_size, METH_VARARGS, NULL}, \
    {"require_recipient",       (PyCFunction)py_require_recipient, METH_VARARGS, NULL}, \
    {"require_auth",            (PyCFunction)py_require_auth, METH_VARARGS, NULL}, \
    {"require_auth2",           (PyCFunction)py_require_auth2, METH_VARARGS, NULL}, \
    {"has_auth",                (PyCFunction)py_has_auth, METH_VARARGS, NULL}, \
    {"is_account",              (PyCFunction)py_is_account, METH_VARARGS, NULL}, \
    {"send_inline",             (PyCFunction)py_send_inline, METH_VARARGS, NULL}, \
    {"send_inline2",         (PyCFunction)py_send_inline2, METH_VARARGS, NULL}, \
    {"send_context_free_inline",(PyCFunction)py_send_context_free_inline, METH_VARARGS, NULL}, \
    {"publication_time",        (PyCFunction)py_publication_time, METH_VARARGS, NULL}, \
    {"current_receiver",        (PyCFunction)py_current_receiver, METH_VARARGS, NULL}, \
    {"N",                       (PyCFunction)py_string_to_name, METH_VARARGS, NULL}, \
    {"s2n",                     (PyCFunction)py_string_to_name, METH_VARARGS, NULL}, \
    {"n2s",                     (PyCFunction)py_name_to_string, METH_VARARGS, NULL}, \
    {"call_contract",               (PyCFunction)py_call_contract, METH_VARARGS, NULL}, \
    {"action_new",              (PyCFunction)py_action_new, METH_VARARGS, NULL}, \
    {"action_add_permission",   (PyCFunction)py_action_add_permission, METH_VARARGS, NULL}, \
    {"action_free",             (PyCFunction)py_action_free, METH_VARARGS, NULL}, \
    {"transaction_new",         (PyCFunction)py_transaction_new, METH_VARARGS, NULL}, \
    {"transaction_send",        (PyCFunction)py_transaction_send, METH_VARARGS, NULL}, \
    {"transaction_free",        (PyCFunction)py_transaction_free, METH_VARARGS, NULL}, \
    {"transaction_add_action",  (PyCFunction)py_transaction_add_action, METH_VARARGS, NULL}, \
    {"transaction_add_context_free_action",  (PyCFunction)py_transaction_add_context_free_action, METH_VARARGS, NULL},

//    {"require_write_lock",      (PyCFunction)py_require_write_lock, METH_VARARGS, NULL}, \
//    {"require_read_lock",      (PyCFunction)py_require_read_lock, METH_VARARGS, NULL}, \

