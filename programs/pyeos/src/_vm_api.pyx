
from libcpp.vector cimport vector

cdef extern from * :
    ctypedef signed int int32_t
    ctypedef long long int64_t
    ctypedef unsigned long long uint64_t
    ctypedef unsigned int uint32_t
    ctypedef unsigned short uint16_t

cdef extern from "Python.h":
    object PyBytes_FromStringAndSize(const char* str, int size)

cdef extern from "uuos.hpp":
   int32_t db_store_i64_(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id,  const char* data, uint32_t _len);
   void db_update_i64_(int32_t iterator, uint64_t payer, const char* data, uint32_t _len);
   void db_remove_i64_(int32_t iterator);
   int32_t db_get_i64_(int32_t iterator, void* data, uint32_t _len);
   int32_t db_next_i64_(int32_t iterator, uint64_t* primary);
   int32_t db_previous_i64_(int32_t iterator, uint64_t* primary);
   int32_t db_find_i64_(uint64_t code, uint64_t scope, uint64_t table, uint64_t id);
   int32_t db_lowerbound_i64_(uint64_t code, uint64_t scope, uint64_t table, uint64_t id);
   int32_t db_upperbound_i64_(uint64_t code, uint64_t scope, uint64_t table, uint64_t id);
   int32_t db_end_i64_(uint64_t code, uint64_t scope, uint64_t table);

cdef object to_bytes(const char *str, int size):
    return PyBytes_FromStringAndSize(str, size)

def db_store_i64(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, data: bytes):
    return db_store_i64_(scope, table, payer, id,  data, len(data))

def db_update_i64(int32_t iterator, uint64_t payer, data):
   db_update_i64_(iterator, payer, data, len(data))

def db_remove_i64(int32_t iterator):
   db_remove_i64_(iterator)

def db_get_i64(int32_t iterator):
    cdef vector[char] v
    cdef int32_t data_len
    data_len = db_get_i64_(iterator, <void *>0, 0)
    v.resize(data_len)
    db_get_i64_(iterator, v.data(), v.size())
    return to_bytes(v.data(), v.size())

def db_next_i64(int32_t iterator):
    cdef uint64_t primary = 0
    cdef next_itr
    next_itr = db_next_i64_(iterator, &primary)
    return next_itr, primary

def db_previous_i64(int32_t iterator):
    cdef uint64_t primary = 0
    cdef next_itr;
    next_itr = db_previous_i64_(iterator, &primary)
    return next_itr, primary

def db_find_i64(uint64_t code, uint64_t scope, uint64_t table, uint64_t id):
    return db_find_i64_(code, scope, table, id)

def db_lowerbound_i64(uint64_t code, uint64_t scope, uint64_t table, uint64_t id):
    return db_lowerbound_i64_(code, scope, table, id);

def db_upperbound_i64(uint64_t code, uint64_t scope, uint64_t table, uint64_t id):
    return db_upperbound_i64_(code, scope, table, id);

def db_end_i64(uint64_t code, uint64_t scope, uint64_t table):
    return db_end_i64_(code, scope, table);
