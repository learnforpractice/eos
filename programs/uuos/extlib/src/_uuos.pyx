# cython: c_string_type=str, c_string_encoding=ascii

from cython.operator cimport dereference as deref, preincrement as inc
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.map cimport map
from libcpp cimport bool
from libc.stdlib cimport malloc


cdef extern from * :
    ctypedef long long int64_t
    ctypedef unsigned long long uint64_t
    ctypedef unsigned int uint32_t
    ctypedef unsigned short uint16_t
    ctypedef unsigned char uint8_t
    ctypedef int __uint128_t

cdef extern from "Python.h":
    ctypedef long long PyLongObject

    object PyBytes_FromStringAndSize(const char* str, int size)
    int _PyLong_AsByteArray(PyLongObject* v, unsigned char* bytes, size_t n, int little_endian, int is_signed)

cdef extern from "uuos.hpp":
    ctypedef struct chain_api:
        int a
        void say_hello()
    
    chain_api *get_chain_api(uint64_t ptr)
    void uuosext_init_chain_api()

    chain_api* chain_new_(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
    void chain_free_(chain_api* api);

def chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir):
    return <uint64_t>chain_new_(config, _genesis, protocol_features_dir, snapshot_dir)

def chain_free(uint64_t ptr):
    chain_free_(<chain_api*>ptr)

def chain_say_hello(uint64_t ptr):
    get_chain_api(ptr).say_hello()

uuosext_init_chain_api()
