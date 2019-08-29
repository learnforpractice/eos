//void assert_sha1( const char* data, uint32_t length, const struct checksum160* hash );
static PyObject *py_assert_sha1(PyObject *self, PyObject *args)
{
    PyObject *o;
    const char *data;
    Py_ssize_t data_len;
    const char *hash;
    Py_ssize_t hash_len;


    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);    
    if (-1 == PyBytes_AsStringAndSize(o, &data, &data_len)) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);    
    if (-1 == PyBytes_AsStringAndSize(o, &hash, &hash_len)) {
        return NULL;
    }

    if (hash_len != 20) {
        PyErr_SetString(PyExc_ValueError, "wrong checksum160 hash length");
        return NULL;
    }

    assert_sha1(data, data_len, (const struct checksum160 *)hash);
    Py_RETURN_NONE;
}

//void assert_sha256( const char* data, uint32_t length, const struct checksum256* hash );
static PyObject *py_assert_sha256(PyObject *self, PyObject *args)
{
    PyObject *o;
    const char *data;
    Py_ssize_t data_len;
    const char *hash;
    Py_ssize_t hash_len;

    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);    
    if (-1 == PyBytes_AsStringAndSize(o, &data, &data_len)) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);    
    if (-1 == PyBytes_AsStringAndSize(o, &hash, &hash_len)) {
        return NULL;
    }

    if (hash_len != sizeof(struct checksum256)) {
        PyErr_SetString(PyExc_ValueError, "wrong checksum256 hash length");
        return NULL;
    }

    assert_sha256(data, data_len, (const struct checksum256 *)hash);
    Py_RETURN_NONE;
}

//void assert_sha512( const char* data, uint32_t length, const struct checksum512* hash );
static PyObject *py_assert_sha512(PyObject *self, PyObject *args)
{
    PyObject *o;
    const char *data;
    Py_ssize_t data_len;
    const char *hash;
    Py_ssize_t hash_len;

    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);    
    if (-1 == PyBytes_AsStringAndSize(o, &data, &data_len)) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);    
    if (-1 == PyBytes_AsStringAndSize(o, &hash, &hash_len)) {
        return NULL;
    }

    if (hash_len != sizeof(struct checksum512)) {
        PyErr_SetString(PyExc_ValueError, "wrong checksum512 hash length");
        return NULL;
    }

    assert_sha512(data, data_len, (const struct checksum512 *)hash);
    Py_RETURN_NONE;
}


//void assert_ripemd160( const char* data, uint32_t length, const struct checksum160* hash );
static PyObject *py_assert_ripemd160(PyObject *self, PyObject *args)
{
    PyObject *o;
    const char *data;
    Py_ssize_t data_len;
    const char *hash;
    Py_ssize_t hash_len;

    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);    
    if (-1 == PyBytes_AsStringAndSize(o, &data, &data_len)) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);    
    if (-1 == PyBytes_AsStringAndSize(o, &hash, &hash_len)) {
        return NULL;
    }

    if (hash_len != 20) {
        PyErr_SetString(PyExc_ValueError, "wrong checksum160 hash length");
        return NULL;
    }

    assert_ripemd160(data, data_len, (const struct checksum160 *)hash);
    Py_RETURN_NONE;
}

//void sha256( const char* data, uint32_t length, struct checksum256* hash );
static PyObject *py_sha256(PyObject *self, PyObject *args)
{
    PyObject *o;
    const char *data;
    Py_ssize_t data_len;
    struct checksum256 hash;

    if (PyTuple_GET_SIZE(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);    
    if (-1 == PyBytes_AsStringAndSize(o, &data, &data_len)) {
        return NULL;
    }

    sha256(data, data_len, &hash);
    return PyBytes_FromStringAndSize((char *)&hash, sizeof(hash));
}

//void sha1( const char* data, uint32_t length, struct checksum160* hash );
static PyObject *py_sha1(PyObject *self, PyObject *args)
{
    PyObject *o;
    const char *data;
    Py_ssize_t data_len;
    struct checksum160 hash;

    if (PyTuple_GET_SIZE(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);    
    if (-1 == PyBytes_AsStringAndSize(o, &data, &data_len)) {
        return NULL;
    }

    sha1(data, data_len, &hash);
    return PyBytes_FromStringAndSize((char *)&hash, 20);
}

//void sha512( const char* data, uint32_t length, struct checksum512* hash );
static PyObject *py_sha512(PyObject *self, PyObject *args)
{
    PyObject *o;
    const char *data;
    Py_ssize_t data_len;
    struct checksum512 hash;

    if (PyTuple_GET_SIZE(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);    
    if (-1 == PyBytes_AsStringAndSize(o, &data, &data_len)) {
        return NULL;
    }

    sha512(data, data_len, &hash);
    return PyBytes_FromStringAndSize((char *)&hash, sizeof(hash));
}

//void ripemd160( const char* data, uint32_t length, struct checksum160* hash );
static PyObject *py_ripemd160(PyObject *self, PyObject *args)
{
    PyObject *o;
    const char *data;
    Py_ssize_t data_len;
    struct checksum160 hash;

    if (PyTuple_GET_SIZE(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);    
    if (-1 == PyBytes_AsStringAndSize(o, &data, &data_len)) {
        return NULL;
    }

    ripemd160(data, data_len, &hash);
    return PyBytes_FromStringAndSize((char *)&hash, 20);
}

//int recover_key( const struct checksum256* digest, const char* sig, size_t siglen, char* pub, size_t publen );
static PyObject *py_recover_key(PyObject *self, PyObject *args)
{
    PyObject *o;
    const char *digest;
    Py_ssize_t digest_len;
    const char *sig;
    Py_ssize_t sig_len;
    char pub[128];

    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);    
    if (-1 == PyBytes_AsStringAndSize(o, &digest, &digest_len)) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);    
    if (-1 == PyBytes_AsStringAndSize(o, &sig, &sig_len)) {
        return NULL;
    }

    int size = recover_key(digest, sig, sig_len, pub, sizeof(pub));
    return PyBytes_FromStringAndSize((char *)&pub, size);
}

//void assert_recover_key( const struct checksum256* digest, const char* sig, size_t siglen, const char* pub, size_t publen );
static PyObject *py_assert_recover_key(PyObject *self, PyObject *args)
{
    PyObject *o;
    const char *digest;
    Py_ssize_t digest_len;
    const char *sig;
    Py_ssize_t sig_len;
    const char *pub;
    Py_ssize_t pub_len;

    if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);    
    if (-1 == PyBytes_AsStringAndSize(o, &digest, &digest_len)) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);    
    if (-1 == PyBytes_AsStringAndSize(o, &sig, &sig_len)) {
        return NULL;
    }

    o = PyTuple_GetItem(args, 2);    
    if (-1 == PyBytes_AsStringAndSize(o, &pub, &pub_len)) {
        return NULL;
    }

    assert_recover_key((struct checksum256*)digest, sig, sig_len, pub, pub_len);
    Py_RETURN_NONE;
}

static PyObject *py_to_base58(PyObject *self, PyObject *args)
{
    PyObject *o;
    char *in;
    Py_ssize_t in_size;

    if (PyTuple_GET_SIZE(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);    
    if (-1 == PyBytes_AsStringAndSize(o, &in, &in_size)) {
        return NULL;
    }
    char *out = (char *)malloc(in_size*2);
    int out_size = to_base58(in, in_size, out, in_size*2);
    o = PyBytes_FromStringAndSize(out, out_size);
    free(out);
    return o;
}

static PyObject *py_from_base58(PyObject *self, PyObject *args)
{
    PyObject *o;
    char *in;
    Py_ssize_t in_size;

    if (PyTuple_GET_SIZE(args) != 1) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);    
    if (-1 == PyBytes_AsStringAndSize(o, &in, &in_size)) {
        return NULL;
    }
    char *out = (char *)malloc(in_size);
    int out_size = from_base58(in, in_size, out, in_size);

    o = PyBytes_FromStringAndSize(out, out_size);
    free(out);
    return o;
}

#include "../xxhash.h"
static PyObject *py_xxhash(PyObject *self, PyObject *args)
{
    PyObject *o;
    char *buffer;
    Py_ssize_t buffer_size = 0;
    uint64_t seed;
    uint64_t hash;

    if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_ValueError, "wrong arguments count");
        return NULL;
    }

    o = PyTuple_GetItem(args, 0);
    if (PyBytes_Check(o)) {
        if (-1 == PyBytes_AsStringAndSize(o, &buffer, &buffer_size)) {
            return NULL;
        }
    } else if (PyUnicode_Check(o)) {
        buffer = PyUnicode_AsUTF8AndSize(o, &buffer_size);
        if (buffer == NULL) {
            return NULL;
        }
    } else {
        PyErr_SetString(PyExc_ValueError, "argument should be a string or bytes");
        return NULL;
    }

    o = PyTuple_GetItem(args, 1);
    seed = PyLong_AsUnsignedLongLong(o);
    hash = XXH64(buffer, buffer_size, seed);
    return PyLong_FromUnsignedLongLong(hash);
}

#define CRYPTO_METHODS \
    {"assert_sha1",          (PyCFunction)py_assert_sha1, METH_VARARGS, NULL}, \
    {"assert_sha256",        (PyCFunction)py_assert_sha256, METH_VARARGS, NULL}, \
    {"assert_sha512",        (PyCFunction)py_assert_sha512, METH_VARARGS, NULL}, \
    {"assert_ripemd160",     (PyCFunction)py_assert_ripemd160, METH_VARARGS, NULL}, \
    {"sha1",                 (PyCFunction)py_sha1, METH_VARARGS, NULL}, \
    {"sha256",               (PyCFunction)py_sha256, METH_VARARGS, NULL}, \
    {"sha512",               (PyCFunction)py_sha512, METH_VARARGS, NULL}, \
    {"ripemd160",            (PyCFunction)py_ripemd160, METH_VARARGS, NULL}, \
    {"recover_key",          (PyCFunction)py_recover_key, METH_VARARGS, NULL}, \
    {"assert_recover_key",   (PyCFunction)py_assert_recover_key, METH_VARARGS, NULL}, \
    {"to_base58",            (PyCFunction)py_to_base58, METH_VARARGS, NULL}, \
    {"from_base58",          (PyCFunction)py_from_base58, METH_VARARGS, NULL}, \
    {"xxhash",               (PyCFunction)py_xxhash, METH_VARARGS, NULL},
