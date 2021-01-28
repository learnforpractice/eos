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
    chain_proxy *chain(uint64_t ptr)
    void uuosext_init_chain_api()

    chain_proxy* chain_new_(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
    void chain_free_(chain_proxy* api);

    ctypedef struct chain_proxy:
        int a
        void say_hello()

        void id(string& chain_id);
        void start_block(string& _time, uint16_t confirm_block_count, string& _new_features);
        int abort_block();
        bool startup(bool initdb);
        void commit_block();

        string& get_last_error();
        void set_last_error(string& error);

def chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir):
    return <uint64_t>chain_new_(config, _genesis, protocol_features_dir, snapshot_dir)

def chain_free(uint64_t ptr):
    chain_free_(<chain_proxy*>ptr)

def chain_say_hello(uint64_t ptr):
    chain(ptr).say_hello()

def id(ptr):
    cdef string chain_id
    chain(ptr).id(chain_id)
    return chain_id

def start_block(uint64_t ptr, string& _time, uint16_t confirm_block_count, string& _new_features):
    return chain(ptr).start_block(_time, confirm_block_count, _new_features)

def abort_block(ptr):
    return chain(ptr).abort_block()

def startup(ptr, initdb):
    return chain(ptr).startup(initdb)

def commit_block(ptr):
    return chain(ptr).commit_block()

def get_last_error(ptr):
    cdef string error
    error = chain(ptr).get_last_error()

def set_last_error(ptr, string& error):
    chain(ptr).set_last_error(error)
