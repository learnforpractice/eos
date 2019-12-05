# cython: c_string_type=str, c_string_encoding=ascii

from cython.operator cimport dereference as deref, preincrement as inc
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.map cimport map
from libcpp cimport bool

cdef extern from * :
    ctypedef long long int64_t
    ctypedef unsigned long long uint64_t
    ctypedef unsigned int uint32_t

cdef extern from "native_object.hpp":
    
    void pack_native_object_(int _type, string& msg, string& packed_message)
    void unpack_native_object_(int _type, string& packed_message, string& msg)

    void *chain_new_(string& config, string& protocol_features_dir)
    void chain_free_(void *ptr)
    void chain_api_get_info_(void *chain_ptr, string& info)
    void chain_api_get_table_rows_(void *chain_ptr, string& params, string& result)
    void chain_api_get_account_(void *chain_ptr, string& params, string& result);

    void chain_on_incoming_block_(void *ptr, string& packed_signed_block, uint32_t& num, string& id)

    uint32_t chain_fork_db_pending_head_block_num_(void *ptr)
    uint32_t chain_last_irreversible_block_num_(void *ptr)
    void chain_get_block_id_for_num_(void *ptr, uint32_t num, string& block_id)
    void chain_id_(void *ptr, string& chain_id)
    void chain_fetch_block_by_number_(void *ptr, uint32_t block_num, string& raw_block)
    int chain_is_building_block_(void *ptr);

    void *producer_new_(void *chain_ptr, string& config);
    void producer_free_(void *ptr);
    void producer_on_incoming_block_(void *ptr, string& packed_signed_block, uint32_t& num, string& id)

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
    cdef uint32_t block_num = 0
    cdef string block_id
    chain_on_incoming_block_(<void *>ptr, packed_signed_block, block_num, block_id)
    return (block_num, block_id)

def chain_api_get_info(uint64_t chain_ptr):
    cdef string info
    chain_api_get_info_(<void *>chain_ptr, info)
    return info

def chain_api_get_table_rows_(uint64_t chain_ptr, string& params):
    cdef string result
    chain_api_get_table_rows_(chain_ptr, params, result)
    return result

def chain_api_get_account(uint64_t chain_ptr, string& params):
    cdef string result
    chain_api_get_account_(<void *>chain_ptr, params, result)
    return result

def chain_fork_db_pending_head_block_num(uint64_t ptr):
    return chain_fork_db_pending_head_block_num_(<void *>ptr)

def chain_last_irreversible_block_num(uint64_t ptr):
    return chain_last_irreversible_block_num_(<void *>ptr)

def chain_get_block_id_for_num(uint64_t ptr, uint32_t num):
    cdef string block_id
    chain_get_block_id_for_num_(<void *>ptr, num, block_id)
    return block_id

def chain_id(uint64_t ptr):
    cdef string chain_id
    chain_id_(<void *>ptr, chain_id)
    return chain_id

def chain_fetch_block_by_number(uint64_t ptr, uint32_t block_num ):
    cdef string raw_block
    chain_fetch_block_by_number_(<void *>ptr, block_num, raw_block)
    return <bytes>raw_block

def chain_is_building_block(uint64_t ptr):
    return chain_is_building_block_(<void *>ptr);

def producer_new(uint64_t chain_ptr, string& config):
    return <uint64_t>producer_new_(<void *>chain_ptr, config)

def producer_free(uint64_t ptr):
    producer_free_(<void *>ptr)

def producer_on_incoming_block(uint64_t ptr, string& packed_signed_block):
    cdef uint32_t block_num = 0
    cdef string block_id
    producer_on_incoming_block_(<void *>ptr, packed_signed_block, block_num, block_id)
    return (block_num, block_id)
