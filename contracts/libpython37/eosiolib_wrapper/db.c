#undef Py_BUILD_CORE
#include <Python.h>
typedef char bool;
typedef __int128 int128_t ;
typedef unsigned __int128 uint128_t;

#undef __WASM
#include <eosiolib/db.h>
#include <eosiolib/action.h>
#include <eosiolib/chain.h>
#include <eosiolib/permission.h>
#include <eosiolib/privileged.h>
#include <eosiolib/system.h>
#include <eosiolib/token.h>
#include <eosiolib/transaction.h>
#include <eosiolib/token.h>
#include <eosiolib/crypto.h>

uint64_t to_name(PyObject *o);

uint64_t parse_db_arg(PyObject *args, int index) {
    PyObject *o;
    o = PyTuple_GetItem(args, index);
    if (o == NULL) {
        return 0;
    }
    return to_name(o);
}

int parse_db_args3(PyObject *args, uint64_t *arg1, uint64_t *arg2, uint64_t *arg3) {
    *arg1 = parse_db_arg(args, 0);
    *arg2 = parse_db_arg(args, 1);
    *arg3 = parse_db_arg(args, 2);
    return *arg1 && *arg2 && *arg3;
}

int parse_db_args4(PyObject *args, uint64_t *arg1, uint64_t *arg2, uint64_t *arg3, uint64_t *arg4) {
    *arg1 = parse_db_arg(args, 0);
    *arg2 = parse_db_arg(args, 1);
    *arg3 = parse_db_arg(args, 2);
    *arg4 = parse_db_arg(args, 3);
    return *arg1 && *arg2 && *arg3 && *arg4;
}

int parse_db_args5(PyObject *args, uint64_t *arg1, uint64_t *arg2, uint64_t *arg3, uint64_t *arg4, uint64_t *arg5) {
    *arg1 = parse_db_arg(args, 0);
    *arg2 = parse_db_arg(args, 1);
    *arg3 = parse_db_arg(args, 2);
    *arg4 = parse_db_arg(args, 3);
    *arg5 = parse_db_arg(args, 4);
    return *arg1 && *arg2 && *arg3 && *arg4 && *arg5;
}

int parse_db_data(PyObject *arg, const char **data, int *len) {
    if (!PyBytes_Check(arg)) {
        PyErr_SetString(PyExc_ValueError, "bad data type.");
        return 0;
    }
    return PyBytes_AsStringAndSize(arg, data, len) != -1;
}

int long_as_byte_array(PyObject *args, int index, char *array, int length) {
    PyObject *o = PyTuple_GetItem(args, index);
    return _PyLong_AsByteArray(o, array, length, 1, 1);
}

PyObject *long_from_byte_array(const char *array, int length) {
    return _PyLong_FromByteArray(array, length, 1, 1);
}

//int32_t db_store_i64(account_name scope, table_name table, account_name payer, uint64_t id,  const void* data, uint32_t len);
static PyObject *py_db_store_i64(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t scope;
    uint64_t table;
    uint64_t payer;
    uint64_t id;
    const char* data;
    Py_ssize_t len;

    if (PyTuple_GET_SIZE(args) != 5) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    if (!parse_db_args4(args, &scope, &table, &payer, &id)) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 4);
    if (!parse_db_data(o, &data, &len)) {
        return NULL;
    }

    int32_t ret = db_store_i64(scope, table, payer, id, data, len);
    return PyLong_FromLong(ret);
}

//void db_update_i64(int32_t iterator, account_name payer, const void* data, uint32_t len);
static PyObject *py_db_update_i64(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t payer;
    const char* data;
    Py_ssize_t len;
    PyObject *o;
    
    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    if (o == NULL) {
        return 0;
    }
    iterator = PyLong_AsLong(o);

    o = PyTuple_GetItem(args, 1);
    if (o == NULL) {
        return 0;
    }
    payer = to_name(o);

    o = PyTuple_GetItem(args, 2);
    parse_db_data(o, &data, &len);

    db_update_i64(iterator, payer, data, len);
    Py_RETURN_NONE;
}

//int32_t db_find_i64(account_name code, account_name scope, table_name table, uint64_t id);
static PyObject *py_db_find_i64(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint64_t id;
    
    if (PyTuple_GET_SIZE(args) != 4) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    if (!parse_db_args4(args, &code, &scope, &table, &id)) {
        return NULL;
    }

    int32_t ret = db_find_i64(code, scope, table, id);
    return PyLong_FromLong(ret);
}

//int32_t db_get_i64(int32_t iterator, void* data, uint32_t len);
static PyObject *py_db_get_i64(PyObject *self, PyObject *args)
{
    int32_t iterator;
    char* data;
    Py_ssize_t len;

    if (!PyArg_ParseTuple(args, "L", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }

    int32_t size = db_get_i64(iterator, NULL, 0);
    if (size <= 0) {
        Py_RETURN_NONE;
    }

    data = (char *)malloc(size);
    db_get_i64(iterator, data, size);
    return PyBytes_FromStringAndSize(data, size);
}

//void db_remove_i64(int32_t iterator);
static PyObject *py_db_remove_i64(PyObject *self, PyObject *args)
{
    int32_t iterator;

    if (!PyArg_ParseTuple(args, "l", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }

    db_remove_i64(iterator);
    Py_RETURN_NONE;
}

//int32_t db_next_i64(int32_t iterator, uint64_t* primary);
static PyObject *py_db_next_i64(PyObject *self, PyObject *args)
{
    uint64_t primary = 0;
    int32_t iterator;

    if (!PyArg_ParseTuple(args, "l", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    iterator = db_next_i64(iterator, &primary);
    return Py_BuildValue("(iK)", iterator, primary);
}

//int32_t db_previous_i64(int32_t iterator, uint64_t* primary);
static PyObject *py_db_previous_i64(PyObject *self, PyObject *args)
{
    uint64_t primary = 0;
    int32_t iterator;

    if (!PyArg_ParseTuple(args, "l", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    iterator = db_previous_i64(iterator, &primary);
    return Py_BuildValue("(iK)", iterator, primary);
}

//int32_t db_lowerbound_i64(account_name code, account_name scope, table_name table, uint64_t id);
static PyObject *py_db_lowerbound_i64(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint64_t id;
    int32_t iterator;

    if (!parse_db_args4(args, &code, &scope, &table, &id)) {
        return NULL;
    }

    iterator = db_lowerbound_i64(code, scope, table, id);
    return PyLong_FromLong(iterator);
}

//int32_t db_upperbound_i64(account_name code, account_name scope, table_name table, uint64_t id);
static PyObject *py_db_upperbound_i64(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint64_t id;
    int32_t iterator;

    if (!parse_db_args4(args, &code, &scope, &table, &id)) {
        return NULL;
    }

    iterator = db_upperbound_i64(code, scope, table, id);
    return PyLong_FromLong(iterator);
}


//int32_t db_end_i64(account_name code, account_name scope, table_name table);
static PyObject *py_db_end_i64(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    int32_t iterator = db_end_i64(code, scope, table);
    return PyLong_FromLong(iterator);
}

//int db_get_count(uint64_t code, uint64_t scope, uint64_t table);
static PyObject *py_db_get_count(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    int32_t count = db_get_count(code, scope, table);
    return PyLong_FromLong(count);
}

/*
int32_t db_idx64_store( account_name scope, 
                        table_name table,
                        account_name payer,
                        uint64_t id,
                        const uint64_t* secondary);
*/

static PyObject *py_db_idx64_store(PyObject *self, PyObject *args)
{
    uint64_t scope;
    uint64_t table;
    uint64_t payer;
    uint64_t id;
    uint64_t secondary;

    if (!parse_db_args5(args, &scope, &table, &payer, &id, &secondary)) {
        return NULL;
    }

    int32_t iterator = db_idx64_store(scope, table, payer, id, &secondary);
    return PyLong_FromLong(iterator);
}

//void db_idx64_update(int32_t iterator, account_name payer, const uint64_t* secondary);
static PyObject *py_db_idx64_update(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t payer;
    uint64_t secondary;
    PyObject *o;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    if (o == NULL) {
        return 0;
    }
    iterator = PyLong_AsLong(o);

    o = PyTuple_GetItem(args, 1);
    if (o == NULL) {
        return 0;
    }
    payer = to_name(o);
    if (payer == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 2);
    if (o == NULL) {
        return 0;
    }
    secondary = to_name(o);
    if (secondary == 0) {
        return NULL;
    }

    db_idx64_update(iterator, payer, &secondary);
    Py_RETURN_NONE;
}

//void db_idx64_remove(int32_t iterator);
static PyObject *py_db_idx64_remove(PyObject *self, PyObject *args)
{
    int32_t iterator;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    db_idx64_remove(iterator);
    Py_RETURN_NONE;
}

//int32_t db_idx64_next(int32_t iterator, uint64_t* primary);
static PyObject *py_db_idx64_next(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t primary;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    iterator = db_idx64_next(iterator, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx64_previous(int32_t iterator, uint64_t* primary);
static PyObject *py_db_idx64_previous(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t primary;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    iterator = db_idx64_previous(iterator, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx64_find_primary(account_name code, account_name scope, table_name table, uint64_t* secondary, uint64_t primary);
static PyObject *py_db_idx64_find_primary(PyObject *self, PyObject *args)
{
    int32_t iterator;

    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint64_t secondary = -1;
    uint64_t primary;

    if (!parse_db_args4(args, &code, &scope, &table, &primary)) {
        return NULL;
    }

    iterator = db_idx64_find_primary(code, scope, table, &secondary, primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(secondary));
    return ret;
}

//int32_t db_idx64_find_secondary(account_name code, account_name scope, table_name table, const uint64_t* secondary, uint64_t* primary);
static PyObject *py_db_idx64_find_secondary(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint64_t secondary;
    uint64_t primary = 0;
    int32_t iterator;

    if (!parse_db_args4(args, &code, &scope, &table, &secondary)) {
        return NULL;
    }

    iterator = db_idx64_find_secondary(code, scope, table, &secondary, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx64_lowerbound(account_name code, account_name scope, table_name table, uint64_t* secondary, uint64_t* primary);
static PyObject *py_db_idx64_lowerbound(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint64_t secondary;
    uint64_t primary = 0;
    int32_t iterator;
    
    if (!parse_db_args4(args, &code, &scope, &table, &secondary)) {
        return NULL;
    }

    iterator = db_idx64_lowerbound(code, scope, table, &secondary, &primary);
    PyObject *ret = PyTuple_New(3);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    PyTuple_SetItem(ret, 2, PyLong_FromUnsignedLongLong(secondary));
    return ret;
}

//int32_t db_idx64_upperbound(account_name code, account_name scope, table_name table, uint64_t* secondary, uint64_t* primary);
static PyObject *py_db_idx64_upperbound(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint64_t secondary;
    uint64_t primary = 0;
    int32_t iterator;
    
    if (!parse_db_args4(args, &code, &scope, &table, &secondary)) {
        return NULL;
    }

    iterator = db_idx64_upperbound(code, scope, table, &secondary, &primary);
    PyObject *ret = PyTuple_New(3);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    PyTuple_SetItem(ret, 2, PyLong_FromUnsignedLongLong(secondary));
    return ret;
}


//int32_t db_idx64_end(account_name code, account_name scope, table_name table);
static PyObject *py_db_idx64_end(PyObject *self, PyObject *args)
{
    PyObject *o;
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    int32_t iterator;

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    iterator = db_idx64_end(code, scope, table);
    return PyLong_FromLong(iterator);
}

//-----------------------------idx128---------------------------------------------------------------

//int32_t db_idx128_store(account_name scope, table_name table, account_name payer, uint64_t id_primary, const uint128_t* secondary);
static PyObject *py_db_idx128_store(PyObject *self, PyObject *args)
{
    uint64_t scope;
    uint64_t table;
    uint64_t payer;
    uint64_t id;
    char secondary[16];
    Py_ssize_t secondary_len;

    if (PyTuple_GET_SIZE(args) != 5) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    if (!parse_db_args4(args, &scope, &table, &payer, &id)) {
        return NULL;
    }

    if (-1 == long_as_byte_array(args, 4, &secondary, 16)) {
        return NULL;
    }

    int32_t iterator = db_idx128_store(scope, table, payer, id, (const uint128_t *)secondary);
    return PyLong_FromLong(iterator);
}

//void db_idx128_update(int32_t iterator, account_name payer, const uint128_t* secondary);
static PyObject *py_db_idx128_update(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t payer;
    char secondary[16];
    Py_ssize_t secondary_len;
    PyObject *o;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    if (!PyLong_Check(o)) {
        PyErr_SetString(PyExc_ValueError, "iterator should be an int type");
        return NULL;
    }
    iterator = PyLong_AsLong(o);

    o = PyTuple_GetItem(args, 1);
    payer = to_name(o);

    if (-1 == long_as_byte_array(args, 2, secondary, 16)) {
        PyErr_SetString(PyExc_ValueError, "secondary should be an int type");
        return NULL;
    }

    db_idx128_update(iterator, payer, (const uint128_t*)secondary);
    Py_RETURN_NONE;
}


//void db_idx64_remove(int32_t iterator);
static PyObject *py_db_idx128_remove(PyObject *self, PyObject *args)
{
    int32_t iterator;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    db_idx128_remove(iterator);
    Py_RETURN_NONE;
}

//int32_t db_idx128_next(int32_t iterator, uint64_t* primary);
static PyObject *py_db_idx128_next(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t primary;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    iterator = db_idx128_next(iterator, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx128_previous(int32_t iterator, uint64_t* primary);
static PyObject *py_db_idx128_previous(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t primary;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    iterator = db_idx128_previous(iterator, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx128_find_primary(account_name code, account_name scope, table_name table, uint128_t* secondary, uint64_t primary);
static PyObject *py_db_idx128_find_primary(PyObject *self, PyObject *args)
{
    int32_t iterator;

    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint128_t secondary;
    uint64_t primary;

    if (PyTuple_GET_SIZE(args) != 4) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    if (!parse_db_args4(args, &code, &scope, &table, &primary)) {
        return NULL;
    }
    iterator = db_idx128_find_primary(code, scope, table, &secondary, primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));

    PyObject *_secondary = long_from_byte_array((char *)&secondary, 16);
    PyTuple_SetItem(ret, 1, _secondary);
    return ret;
}

//int32_t db_idx128_find_secondary(account_name code, account_name scope, table_name table, const uint128_t* secondary, uint64_t* primary);
static PyObject *py_db_idx128_find_secondary(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint128_t secondary;
    Py_ssize_t secondary_len;
    uint64_t primary = 0;
    int32_t iterator;

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    if (-1 == long_as_byte_array(args, 3, (char *)secondary, 16)) {
        PyErr_SetString(PyExc_ValueError, "secondary should be an int type.");
        return NULL;
    }

    iterator = db_idx128_find_secondary(code, scope, table, secondary, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx128_lowerbound(account_name code, account_name scope, table_name table, uint128_t* secondary, uint64_t* primary);
static PyObject *py_db_idx128_lowerbound(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint128_t secondary;
    uint64_t primary;
    int32_t iterator;

    if (PyTuple_GET_SIZE(args) != 4) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    if (-1 == long_as_byte_array(args, 3, (char *)&secondary, 16)) {
        PyErr_SetString(PyExc_ValueError, "secondary should be a int type");
        return NULL;
    }

    iterator = db_idx128_lowerbound(code, scope, table, &secondary, &primary);
    PyObject *ret = PyTuple_New(3);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));

    PyObject *_secondary = long_from_byte_array((char *)&secondary, 16);
    PyTuple_SetItem(ret, 2, _secondary);
    return ret;
}

//int32_t db_idx128_upperbound(account_name code, account_name scope, table_name table, uint128_t* secondary, uint64_t* primary);
static PyObject *py_db_idx128_upperbound(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint128_t secondary;
    uint64_t primary;
    int32_t iterator;

    if (PyTuple_GET_SIZE(args) != 4) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    if (-1 == long_as_byte_array(args, 3, (char *)&secondary, 16)) {
        PyErr_SetString(PyExc_ValueError, "secondary should be a int type");
        return NULL;
    }

    iterator = db_idx128_upperbound(code, scope, table, &secondary, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyBytes_FromStringAndSize((char *)&secondary, sizeof(secondary)));

    PyObject *_secondary = long_from_byte_array((char *)&secondary, 16);
    PyTuple_SetItem(ret, 2, _secondary);
    return ret;
}

//int32_t db_idx128_end(account_name code, account_name scope, table_name table);
static PyObject *py_db_idx128_end(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    int32_t iterator;

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    iterator = db_idx128_end(code, scope, table);
    return PyLong_FromLong(iterator);
}


//-----------------------------idx256---------------------------------------------------------------

//int32_t db_idx256_store(account_name scope, table_name table, account_name payer, uint64_t id, const uint128_t* data, uint32_t data_len );
static PyObject *py_db_idx256_store(PyObject *self, PyObject *args)
{
    uint64_t scope;
    uint64_t table;
    uint64_t payer;
    uint64_t id;
    uint128_t secondary[2];
    Py_ssize_t secondary_len;

    if (PyTuple_GET_SIZE(args) != 5) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    if (!parse_db_args4(args, &scope, &table, &payer, &id)) {
        return NULL;
    }

    PyObject *o = PyTuple_GetItem(args, 4);
    int ret = _PyLong_AsByteArray(o, (char *)&secondary, 32, 1, 1);
    if (ret == -1) {
        return NULL;
    }

    int32_t iterator = db_idx256_store(scope, table, payer, id, secondary, 2);
    return PyLong_FromLong(iterator);
}

//void db_idx256_update(int32_t iterator, account_name payer, const uint128_t* data, uint32_t data_len);
static PyObject *py_db_idx256_update(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t payer;
    uint128_t secondary[2];
    PyObject *o;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);    
    iterator = PyLong_AsLong(o);
    if (PyErr_Occurred()) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);
    payer = to_name(o);
    if (payer == 0) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 2);
    int ret = _PyLong_AsByteArray(o, (char *)&secondary, 32, 1, 1);
    if (ret == -1) {
        PyErr_SetString(PyExc_ValueError, "wrong 3th argument");
        return NULL;
    }

    db_idx256_update(iterator, payer, secondary, 2);
    Py_RETURN_NONE;
}

//void db_idx256_remove(int32_t iterator);
static PyObject *py_db_idx256_remove(PyObject *self, PyObject *args)
{
    int32_t iterator;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    db_idx256_remove(iterator);
    Py_RETURN_NONE;
}

//int32_t db_idx256_next(int32_t iterator, uint64_t* primary);
static PyObject *py_db_idx256_next(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t primary;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    iterator = db_idx256_next(iterator, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx256_previous(int32_t iterator, uint64_t* primary);
static PyObject *py_db_idx256_previous(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t primary;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    iterator = db_idx256_previous(iterator, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx256_find_primary(account_name code, account_name scope, table_name table, uint128_t* data, uint32_t data_len, uint64_t primary);
static PyObject *py_db_idx256_find_primary(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint128_t secondary[2];
    uint64_t primary;

    if (PyTuple_GET_SIZE(args) != 4) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    if (!parse_db_args4(args, &code, &scope, &table, &primary)) {
        return NULL;
    }

    iterator = db_idx256_find_primary(code, scope, table, &secondary, 2, primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));

    PyObject* _secondary = long_from_byte_array((char *)secondary, 32);
    PyTuple_SetItem(ret, 1, _secondary);
    return ret;
}

//int32_t db_idx256_find_secondary(account_name code, account_name scope, table_name table, const uint128_t* data, uint32_t data_len, uint64_t* primary);
static PyObject *py_db_idx256_find_secondary(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint128_t secondary[2];
    Py_ssize_t secondary_len;
    uint64_t primary = 0;
    int32_t iterator;

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    PyObject *o = PyTuple_GetItem(args, 3);
    if (_PyLong_AsByteArray(o, (char *)secondary, 32, 1, 1) == -1) {
        PyErr_SetString(PyExc_ValueError, "wrong 4th argument");
        return NULL;
    }

    iterator = db_idx256_find_secondary(code, scope, table, secondary, 2, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx256_lowerbound(account_name code, account_name scope, table_name table, uint128_t* data, uint32_t data_len, uint64_t* primary);
static PyObject *py_db_idx256_lowerbound(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint128_t secondary[2];
    uint64_t primary;
    int32_t iterator;

    if (PyTuple_GET_SIZE(args) != 4) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    if (-1 == long_as_byte_array(args, 3, (char *)&secondary, 16)) {
        PyErr_SetString(PyExc_ValueError, "secondary should be a int type");
        return NULL;
    }

    iterator = db_idx256_lowerbound(code, scope, table, secondary, 2, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyBytes_FromStringAndSize((char *)&secondary, sizeof(secondary)));

    PyObject *_secondary = long_from_byte_array((char *)&secondary, 16);
    PyTuple_SetItem(ret, 2, _secondary);
    return ret;
}

//int32_t db_idx256_upperbound(account_name code, account_name scope, table_name table, uint128_t* data, uint32_t data_len, uint64_t* primary);
static PyObject *py_db_idx256_upperbound(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    uint128_t secondary[2];
    uint64_t primary;
    int32_t iterator;

    if (PyTuple_GET_SIZE(args) != 4) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    if (-1 == long_as_byte_array(args, 3, (char *)&secondary, 16)) {
        PyErr_SetString(PyExc_ValueError, "secondary should be a int type");
        return NULL;
    }

    iterator = db_idx256_upperbound(code, scope, table, secondary, 2, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyBytes_FromStringAndSize((char *)&secondary, sizeof(secondary)));

    PyObject *_secondary = long_from_byte_array((char *)&secondary, 16);
    PyTuple_SetItem(ret, 2, _secondary);
    return ret;
}

//int32_t db_idx256_end(account_name code, account_name scope, table_name table);
static PyObject *py_db_idx256_end(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    int32_t iterator;

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    iterator = db_idx256_end(code, scope, table);
    return PyLong_FromLong(iterator);
}


//----------------------------------------idx_double---------------------------------------------------------------
//int32_t db_idx_double_store(account_name scope, table_name table, account_name payer, uint64_t id, const double* secondary);
static PyObject *py_db_idx_double_store(PyObject *self, PyObject *args)
{
    uint64_t scope;
    uint64_t table;
    uint64_t payer;
    uint64_t id;
    double secondary;

    if (!PyArg_ParseTuple(args, "KKKKd", &scope, &table, &payer, &id, &secondary)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }

    if (!parse_db_args4(args, &scope, &table, &payer, &id)) {
        return NULL;
    }

    PyObject *o = PyTuple_GetItem(args, 4);
    if (!PyFloat_Check(o)) {
        PyErr_SetString(PyExc_ValueError, "wrong argument type");
        return NULL;
    }

    secondary = PyFloat_AsDouble(o);

    int32_t iterator = db_idx_double_store(scope, table, payer, id, &secondary);
    return PyLong_FromLong(iterator);
}

//void db_idx_double_update(int32_t iterator, account_name payer, const double* secondary);
static PyObject *py_db_idx_double_update(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t payer;
    double secondary;

    if (!PyArg_ParseTuple(args, "iKd", &iterator, &payer, &secondary)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }

    db_idx_double_update(iterator, payer, &secondary);
    Py_RETURN_NONE;
}

//void db_idx_double_remove(int32_t iterator);
static PyObject *py_db_idx_double_remove(PyObject *self, PyObject *args)
{
    int32_t iterator;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    db_idx_double_remove(iterator);
    Py_RETURN_NONE;
}

//int32_t db_idx_double_next(int32_t iterator, uint64_t* primary);
static PyObject *py_db_idx_double_next(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t primary;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    iterator = db_idx_double_next(iterator, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx_double_previous(int32_t iterator, uint64_t* primary);
static PyObject *py_db_idx_double_previous(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t primary;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    iterator = db_idx_double_previous(iterator, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx_double_find_primary(account_name code, account_name scope, table_name table, double* secondary, uint64_t primary);
static PyObject *py_db_idx_double_find_primary(PyObject *self, PyObject *args)
{
    int32_t iterator;

    uint64_t code;
    uint64_t scope;
    uint64_t table;
    double secondary;
    uint64_t primary;

    if (!parse_db_args4(args, &code, &scope, &table, &primary)) {
        return NULL;
    }

    iterator = db_idx_double_find_primary(code, scope, table, &secondary, primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyFloat_FromDouble(secondary));
    return ret;
}

//int32_t db_idx_double_find_secondary(account_name code, account_name scope, table_name table, const double* secondary, uint64_t* primary);
static PyObject *py_db_idx_double_find_secondary(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    double secondary;
    uint64_t primary = 0;
    int32_t iterator;

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    PyObject *o = PyTuple_GetItem(args, 3);
    if (!PyFloat_Check(o)) {
        PyErr_SetString(PyExc_ValueError, "wrong argument type");
        return NULL;
    }

    secondary = PyFloat_AsDouble(o);

    iterator = db_idx_double_find_secondary(code, scope, table, &secondary, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx_double_lowerbound(account_name code, account_name scope, table_name table, double* secondary, uint64_t* primary);
static PyObject *py_db_idx_double_lowerbound(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    double secondary;
    uint64_t primary;
    int32_t iterator;

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    iterator = db_idx_double_lowerbound(code, scope, table, &secondary, &primary);
    PyObject *ret = PyTuple_New(3);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    PyTuple_SetItem(ret, 2, PyFloat_FromDouble(secondary));
    return ret;
}

//int32_t db_idx_double_upperbound(account_name code, account_name scope, table_name table, double* secondary, uint64_t* primary);
static PyObject *py_db_idx_double_upperbound(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    double secondary;
    uint64_t primary;
    int32_t iterator;

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    iterator = db_idx_double_upperbound(code, scope, table, &secondary, &primary);
    PyObject *ret = PyTuple_New(3);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    PyTuple_SetItem(ret, 2, PyFloat_FromDouble(secondary));
    return ret;
}
//int32_t db_idx_double_end(account_name code, account_name scope, table_name table);
static PyObject *py_db_idx_double_end(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    int32_t iterator;

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    iterator = db_idx_double_end(code, scope, table);
    return PyLong_FromLong(iterator);
}

//-----------------------------idx_long_double---------------------------------------------------------------

//int32_t db_idx_long_double_store(account_name scope, table_name table, account_name payer, uint64_t id, const long double* secondary);
static PyObject *py_db_idx_long_double_store(PyObject *self, PyObject *args)
{
    uint64_t scope;
    uint64_t table;
    uint64_t payer;
    uint64_t id;
    const char *secondary;
    Py_ssize_t secondary_len;

    if (!parse_db_args4(args, &scope, &table, &payer, &id)) {
        return NULL;
    }

    PyObject *o = PyTuple_GetItem(args, 4);
    if (!parse_db_data(o, &secondary, &secondary_len)) {
        return NULL;
    }

    if (secondary_len !=sizeof(long double)) {
        PyErr_SetString(PyExc_ValueError, "secondary_len should be 16 bytes long");
        return NULL;
    }

    int32_t iterator = db_idx_long_double_store(scope, table, payer, id, &secondary);
    return PyLong_FromLong(iterator);
}

//void db_idx_long_double_update(int32_t iterator, account_name payer, const long double* secondary);
static PyObject *py_db_idx_long_double_update(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t payer;
    long double secondary;
    Py_ssize_t secondary_len;

    if (!PyArg_ParseTuple(args, "iKs#", &iterator, &payer, &secondary, &secondary_len)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    if (secondary_len !=sizeof(uint128_t)) {
        PyErr_SetString(PyExc_ValueError, "secondary_len should be 16 bytes long");
        return NULL;
    }
    db_idx_long_double_update(iterator, payer, &secondary);
    Py_RETURN_NONE;
}

//void db_idx_long_double_remove(int32_t iterator);
static PyObject *py_db_idx_long_double_remove(PyObject *self, PyObject *args)
{
    int32_t iterator;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    db_idx_long_double_remove(iterator);
    Py_RETURN_NONE;
}

//int32_t db_idx_long_double_next(int32_t iterator, uint64_t* primary);
static PyObject *py_db_idx_long_double_next(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t primary;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    iterator = db_idx_long_double_next(iterator, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx_long_double_previous(int32_t iterator, uint64_t* primary);
static PyObject *py_db_idx_long_double_previous(PyObject *self, PyObject *args)
{
    int32_t iterator;
    uint64_t primary;

    if (!PyArg_ParseTuple(args, "i", &iterator)) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments");
        return NULL;
    }
    iterator = db_idx_long_double_previous(iterator, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx_long_double_find_primary(account_name code, account_name scope, table_name table, long double* secondary, uint64_t primary);
static PyObject *py_db_idx_long_double_find_primary(PyObject *self, PyObject *args)
{
    int32_t iterator;

    uint64_t code;
    uint64_t scope;
    uint64_t table;
    long double secondary;
    uint64_t primary;

    if (!parse_db_args4(args, &code, &scope, &table, &primary)) {
        return NULL;
    }

    iterator = db_idx_long_double_find_primary(code, scope, table, &secondary, primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyBytes_FromStringAndSize(&secondary, sizeof(secondary)));
    return ret;
}

//int32_t db_idx_long_double_find_secondary(account_name code, account_name scope, table_name table, const long double* secondary, uint64_t* primary);
static PyObject *py_db_idx_long_double_find_secondary(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
//    long double *secondary;
    const char *secondary;
    Py_ssize_t secondary_len;
    uint64_t primary = 0;
    int32_t iterator;

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    PyObject *o = PyTuple_GetItem(args, 3);
    if (!parse_db_data(o, &secondary, &secondary_len)) {
        return NULL;
    }

    if (secondary_len != sizeof(long double)) {
        PyErr_SetString(PyExc_ValueError, "secondary should be 16 bytes long.");
        return NULL;
    }
    iterator = db_idx_long_double_find_secondary(code, scope, table, secondary, &primary);
    PyObject *ret = PyTuple_New(2);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    return ret;
}

//int32_t db_idx_long_double_lowerbound(account_name code, account_name scope, table_name table, long double* secondary, uint64_t* primary);
static PyObject *py_db_idx_long_double_lowerbound(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    long double secondary;
    uint64_t primary;
    int32_t iterator;

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    iterator = db_idx_long_double_lowerbound(code, scope, table, &secondary, &primary);
    PyObject *ret = PyTuple_New(3);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    PyTuple_SetItem(ret, 2, PyBytes_FromStringAndSize(&secondary, sizeof(secondary)));
    return ret;
}

//int32_t db_idx_long_double_upperbound(account_name code, account_name scope, table_name table, long double* secondary, uint64_t* primary);
static PyObject *py_db_idx_long_double_upperbound(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    long double secondary;
    uint64_t primary;
    int32_t iterator;

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    iterator = db_idx_long_double_upperbound(code, scope, table, &secondary, &primary);
    PyObject *ret = PyTuple_New(3);
    PyTuple_SetItem(ret, 0, PyLong_FromLong(iterator));
    PyTuple_SetItem(ret, 1, PyLong_FromUnsignedLongLong(primary));
    PyTuple_SetItem(ret, 2, PyBytes_FromStringAndSize((char *)&secondary, sizeof(secondary)));
    return ret;
}

//int32_t db_idx_long_double_end(account_name code, account_name scope, table_name table);
static PyObject *py_db_idx_long_double_end(PyObject *self, PyObject *args)
{
    uint64_t code;
    uint64_t scope;
    uint64_t table;
    int32_t iterator;

    if (!parse_db_args3(args, &code, &scope, &table)) {
        return NULL;
    }

    iterator = db_idx_long_double_end(code, scope, table);
    return PyLong_FromLong(iterator);
}

static PyMethodDef db_methods[] = {
    {"store_i64",                    (PyCFunction)py_db_store_i64,           METH_VARARGS, NULL},
    {"update_i64",                   (PyCFunction)py_db_update_i64,          METH_VARARGS, NULL},
    {"find_i64",                     (PyCFunction)py_db_find_i64,            METH_VARARGS, NULL},
    {"get_i64",                      (PyCFunction)py_db_get_i64,             METH_VARARGS, NULL},
    {"remove_i64",                   (PyCFunction)py_db_remove_i64,          METH_VARARGS, NULL},
    {"next_i64",                     (PyCFunction)py_db_next_i64,            METH_VARARGS, NULL},
    {"previous_i64",                 (PyCFunction)py_db_previous_i64,        METH_VARARGS, NULL},
    {"upperbound_i64",               (PyCFunction)py_db_upperbound_i64,      METH_VARARGS, NULL},
    {"lowerbound_i64",               (PyCFunction)py_db_lowerbound_i64,      METH_VARARGS, NULL},
    {"end_i64",                      (PyCFunction)py_db_end_i64,             METH_VARARGS, NULL},
    {"get_count",                    (PyCFunction)py_db_get_count,             METH_VARARGS, NULL},

    {"idx64_store",                  (PyCFunction)py_db_idx64_store,         METH_VARARGS, NULL},
    {"idx64_update",                 (PyCFunction)py_db_idx64_update,        METH_VARARGS, NULL},
    {"idx64_remove",                 (PyCFunction)py_db_idx64_remove,        METH_VARARGS, NULL},
    {"idx64_next",                   (PyCFunction)py_db_idx64_next,          METH_VARARGS, NULL},
    {"idx64_previous",               (PyCFunction)py_db_idx64_previous,                  METH_VARARGS, NULL},
    {"idx64_find_primary",           (PyCFunction)py_db_idx64_find_primary,              METH_VARARGS, NULL},
    {"idx64_find_secondary",         (PyCFunction)py_db_idx64_find_secondary,            METH_VARARGS, NULL},
    {"idx64_lowerbound",             (PyCFunction)py_db_idx64_lowerbound,                METH_VARARGS, NULL},
    {"idx64_upperbound",             (PyCFunction)py_db_idx64_upperbound,                METH_VARARGS, NULL},
    {"idx64_end",                    (PyCFunction)py_db_idx64_end,                       METH_VARARGS, NULL},

    {"idx128_store",                 (PyCFunction)py_db_idx128_store,                    METH_VARARGS, NULL},
    {"idx128_update",                (PyCFunction)py_db_idx128_update,                   METH_VARARGS, NULL},
    {"idx128_remove",                 (PyCFunction)py_db_idx128_remove,                    METH_VARARGS, NULL},
    {"idx128_next",                  (PyCFunction)py_db_idx128_next,                     METH_VARARGS, NULL},
    {"idx128_previous",              (PyCFunction)py_db_idx128_previous,                 METH_VARARGS, NULL},
    {"idx128_find_primary",          (PyCFunction)py_db_idx128_find_primary,             METH_VARARGS, NULL},
    {"idx128_find_secondary",        (PyCFunction)py_db_idx128_find_secondary,           METH_VARARGS, NULL},
    {"idx128_lowerbound",            (PyCFunction)py_db_idx128_lowerbound,               METH_VARARGS, NULL},
    {"idx128_upperbound",            (PyCFunction)py_db_idx128_upperbound,               METH_VARARGS, NULL},
    {"idx128_end",                   (PyCFunction)py_db_idx128_end,                      METH_VARARGS, NULL},
 
    {"idx256_update",                (PyCFunction)py_db_idx256_update,                   METH_VARARGS, NULL},
    {"idx256_remove",                (PyCFunction)py_db_idx256_remove,                   METH_VARARGS, NULL},
    {"idx256_next",                  (PyCFunction)py_db_idx256_next,                     METH_VARARGS, NULL},
    {"idx256_previous",              (PyCFunction)py_db_idx256_previous,                 METH_VARARGS, NULL},
    {"idx256_find_primary",          (PyCFunction)py_db_idx256_find_primary,             METH_VARARGS, NULL},
    {"idx256_find_secondary",        (PyCFunction)py_db_idx256_find_secondary,           METH_VARARGS, NULL},
    {"idx256_lowerbound",            (PyCFunction)py_db_idx256_lowerbound,               METH_VARARGS, NULL},
    {"idx256_upperbound",            (PyCFunction)py_db_idx256_upperbound,               METH_VARARGS, NULL},
    {"idx256_end",                   (PyCFunction)py_db_idx256_end,                      METH_VARARGS, NULL},
    {"idx_double_store",             (PyCFunction)py_db_idx_double_store,                METH_VARARGS, NULL},
    {"idx_double_update",            (PyCFunction)py_db_idx_double_update,               METH_VARARGS, NULL},
    {"idx_double_remove",            (PyCFunction)py_db_idx_double_remove,               METH_VARARGS, NULL},
    {"idx_double_next",              (PyCFunction)py_db_idx_double_next,                 METH_VARARGS, NULL},
    {"idx_double_previous",          (PyCFunction)py_db_idx_double_previous,             METH_VARARGS, NULL},
    {"idx_double_find_primary",      (PyCFunction)py_db_idx_double_find_primary,         METH_VARARGS, NULL},
    {"idx_double_find_secondary",    (PyCFunction)py_db_idx_double_find_secondary,       METH_VARARGS, NULL},
    {"idx_double_lowerbound",        (PyCFunction)py_db_idx_double_lowerbound,           METH_VARARGS, NULL},
    {"idx_double_upperbound",        (PyCFunction)py_db_idx_double_upperbound,           METH_VARARGS, NULL},
    {"idx_double_end",               (PyCFunction)py_db_idx_double_end,                  METH_VARARGS, NULL},
    {"idx_long_double_store",        (PyCFunction)py_db_idx_long_double_store,           METH_VARARGS, NULL},
    {"idx_long_double_update",       (PyCFunction)py_db_idx_long_double_update,          METH_VARARGS, NULL},
    {"idx_long_double_remove",       (PyCFunction)py_db_idx_long_double_remove,          METH_VARARGS, NULL},
    {"idx_long_double_next",         (PyCFunction)py_db_idx_long_double_next,            METH_VARARGS, NULL},
    {"idx_long_double_previous",     (PyCFunction)py_db_idx_long_double_previous,        METH_VARARGS, NULL},
    {"idx_long_double_find_primary", (PyCFunction)py_db_idx_long_double_find_primary,    METH_VARARGS, NULL},
    {"idx_long_double_find_secondary", (PyCFunction)py_db_idx_long_double_find_secondary, METH_VARARGS, NULL},
    {"idx_long_double_lowerbound",   (PyCFunction)py_db_idx_long_double_lowerbound,      METH_VARARGS, NULL},
    {"idx_long_double_upperbound",   (PyCFunction)py_db_idx_long_double_upperbound,      METH_VARARGS, NULL},
    {"idx_long_double_end",          (PyCFunction)py_db_idx_long_double_end,             METH_VARARGS, NULL},
    {NULL,                      NULL}           /* sentinel */
};

static struct PyModuleDef dbmodule = {
    PyModuleDef_HEAD_INIT,
    "_db",
    NULL, //thread_doc,
    -1,
    db_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit__db(void)
{
    PyObject *m;
    /* Create the module and add the functions */
    return PyModule_Create(&dbmodule);
}

