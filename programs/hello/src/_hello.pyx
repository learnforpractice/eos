# cython: c_string_type=str, c_string_encoding=ascii

from cython.operator cimport dereference as deref, preincrement as inc
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.map cimport map
from libcpp cimport bool


cdef extern from "cpp_object.hpp":
    void pack_cpp_object_(int _type, string& msg, string& packed_message)
    void unpack_cpp_object_(int _type, string& packed_message, string& msg)

cpdef void hello(str strArg):
    "Prints back 'Hello <param>', for example example: hello.hello('you')"
    print("Hello, {}!)".format(strArg))

cpdef long elevation():
    "Returns elevation of Nevado Sajama."
    return 21463L


def pack_native_object(int _type, string& msg):
    cdef string packed_message
    pack_cpp_object_(_type, msg, packed_message)
    return <bytes>packed_message

def unpack_native_object(int _type, string& packed_message):
    cdef string msg
    unpack_cpp_object_(_type, packed_message, msg)
    return <bytes>msg
