/*
int32_t
check_transaction_authorization( const char* trx_data,     uint32_t trx_size,
                                const char* pubkeys_data, uint32_t pubkeys_size,
                                const char* perms_data,   uint32_t perms_size
                              );
*/
static PyObject *py_check_transaction_authorization(PyObject *self, PyObject *args)
{
    const char *trx_data;
    Py_ssize_t trx_data_len;
    const char *pubkeys_data;
    Py_ssize_t pubkeys_data_len;
    const char *perms_data;
    Py_ssize_t perms_data_len;

    if (!PyArg_ParseTuple(args, "s#s#s#", &trx_data, &trx_data_len, 
                                          &pubkeys_data, &pubkeys_data_len,
                                          &perms_data, &perms_data_len    
    )) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    uint32_t ret = check_transaction_authorization(trx_data, trx_data_len,
                                                   pubkeys_data, pubkeys_data_len,
                                                   perms_data, perms_data_len);
    return PyLong_FromLong(ret);
}

/*
int32_t
check_permission_authorization( account_name account,
                                permission_name permission,
                                const char* pubkeys_data, uint32_t pubkeys_size,
                                const char* perms_data,   uint32_t perms_size,
                                uint64_t delay_us
                              );
*/
static PyObject *py_check_permission_authorization(PyObject *self, PyObject *args)
{
    uint64_t account;
    uint64_t permission;
    const char* pubkeys_data;
    Py_ssize_t pubkeys_size;
    const char* perms_data;
    Py_ssize_t perms_size;
    uint64_t delay_us;

    if (!PyArg_ParseTuple(args, "KKs#s#K",  &account, 
                                            &permission, 
                                            &pubkeys_data, &pubkeys_size,
                                            &perms_data, &perms_size, 
                                            &delay_us )) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    uint32_t ret = check_permission_authorization(account, 
                                                  permission, 
                                                  pubkeys_data, pubkeys_size,
                                                  perms_data, perms_size,
                                                  delay_us);
    return PyLong_FromLong(ret);
}


//int64_t get_permission_last_used( account_name account, permission_name permission );
static PyObject *py_get_permission_last_used(PyObject *self, PyObject *args)
{
    uint64_t account;
    uint64_t permission;
    if (!PyArg_ParseTuple(args, "KK", &account, &permission)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    int64_t ret = get_permission_last_used(account, permission);
    return PyLong_FromLongLong(ret);
}

//int64_t get_account_creation_time( account_name account );
static PyObject *py_get_account_creation_time(PyObject *self, PyObject *args)
{
    uint64_t account;
    if (!PyArg_ParseTuple(args, "KK", &account)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    int64_t ret = get_account_creation_time(account);
    return PyLong_FromLongLong(ret);
}


#define PERMISSION_METHODS \
    {"check_transaction_authorization",     (PyCFunction)py_check_transaction_authorization, METH_VARARGS, NULL}, \
    {"check_permission_authorization",      (PyCFunction)py_check_permission_authorization, METH_VARARGS, NULL}, \
    {"get_permission_last_used",            (PyCFunction)py_get_permission_last_used, METH_VARARGS, NULL}, \
    {"get_account_creation_time",        (PyCFunction)py_get_account_creation_time, METH_VARARGS, NULL}, 
