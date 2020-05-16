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

cdef extern from "Python.h":
    object PyBytes_FromStringAndSize(const char* str, int size)

cdef extern from "uuos.hpp":
    void say_hello_();
#   void register_on_accepted_block_cb_()

def say_hello():
    return say_hello_()
