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
    ctypedef int          int32_t
    ctypedef unsigned short uint16_t
    ctypedef unsigned char uint8_t
    ctypedef int __uint128_t #hacking

cdef extern from "<Python.h>":
    ctypedef long long PyLongObject

    object PyBytes_FromStringAndSize(const char* str, int size)
    int _PyLong_AsByteArray(PyLongObject* v, unsigned char* bytes, size_t n, int little_endian, int is_signed)

cdef extern from "<uuos.hpp>":
    chain_proxy *chain(uint64_t ptr)
    void uuosext_init()

    ctypedef struct chain_proxy:
        void say_hello()

        void id(string& chain_id);
        void start_block(string& _time, uint16_t confirm_block_count, string& _new_features);
        int abort_block();
        bool startup(bool initdb);
        void finalize_block(string& _priv_keys);
        void commit_block();
        void get_block_id_for_num(uint32_t block_num, string& result);

        string& get_last_error();
        void set_last_error(string& error);

    ctypedef struct uuos_proxy:
        chain_proxy* chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
        void chain_free(chain_proxy* api);

    uuos_proxy *get_uuos_proxy()


def chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir):
    return <uint64_t>get_uuos_proxy().chain_new(config, _genesis, protocol_features_dir, snapshot_dir)

def chain_free(uint64_t ptr):
    get_uuos_proxy().chain_free(<chain_proxy*>ptr)

def chain_say_hello(uint64_t ptr):
    chain(ptr).say_hello()

def id(uint64_t ptr):
    cdef string chain_id
    chain(ptr).id(chain_id)
    return chain_id

def start_block(uint64_t ptr, string& _time, uint16_t confirm_block_count, string& _new_features):
    return chain(ptr).start_block(_time, confirm_block_count, _new_features)

def startup(uint64_t ptr, initdb):
    return chain(ptr).startup(initdb)

def abort_block(uint64_t ptr):
    return chain(ptr).abort_block()

def finalize_block(uint64_t ptr, string& _priv_keys):
    chain(ptr).finalize_block(_priv_keys)

def commit_block(uint64_t ptr):
    chain(ptr).commit_block()

def get_last_error(uint64_t ptr):
    cdef string error
    error = chain(ptr).get_last_error()

def set_last_error(uint64_t ptr, string& error):
    chain(ptr).set_last_error(error)

def get_block_id_for_num(uint64_t ptr, uint32_t block_num):
    cdef string result
    chain(ptr).get_block_id_for_num(block_num, result)
    return result
