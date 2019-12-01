# cython: c_string_type=str, c_string_encoding=ascii

from cython.operator cimport dereference as deref, preincrement as inc
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.map cimport map
from libcpp cimport bool

#cdef extern from <stdint.h>:
    

cdef extern from "native_object.hpp":
    void pack_native_object_(int _type, string& msg, string& packed_message)
    void unpack_native_object_(int _type, string& packed_message, string& msg)

    void *chain_new_(string& config, string& protocol_features_dir)
    void chain_free_(void *ptr)

    void chain_on_incoming_block_(void *ptr, string& packed_signed_block)

cpdef void hello(str strArg):
    "Prints back 'Hello <param>', for example example: hello.hello('you')"
    print("Hello, {}!)".format(strArg))

cpdef long elevation():
    "Returns elevation of Nevado Sajama."
    return 21463L


def pack_native_object(int _type, string& msg):
    cdef string packed_message
    pack_native_object_(_type, msg, packed_message)
    return <bytes>packed_message

def unpack_native_object(int _type, string& packed_message):
    cdef string msg
    unpack_native_object_(_type, packed_message, msg)
    return <bytes>msg

def chain_new(string& config, string& protocol_features_dir):
    return <unsigned long long>chain_new_(config, protocol_features_dir)

def chain_free(unsigned long long  ptr):
    chain_free_(<void *>ptr);

def chain_on_incoming_block(unsigned long long ptr, string& packed_signed_block):
    chain_on_incoming_block_(<void *>ptr, packed_signed_block)
