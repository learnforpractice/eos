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

cdef extern from "<Python.h>":
    ctypedef long long PyLongObject

    object PyBytes_FromStringAndSize(const char* str, int size)
    int _PyLong_AsByteArray(PyLongObject* v, unsigned char* bytes, size_t n, int little_endian, int is_signed)

cdef extern from "<uuos.hpp>":
    void uuosext_init()

    ctypedef struct uuos_proxy:
        void set_log_level(string& logger_name, int level)
        void set_block_interval_ms(int ms)

    uuos_proxy *get_uuos_proxy()

uuosext_init()

def set_log_level(string& logger_name, int level):
    get_uuos_proxy().set_log_level(logger_name, level)

def set_block_interval_ms(int ms):
    get_uuos_proxy().set_block_interval_ms(ms)
