
# cython: c_string_type=str, c_string_encoding=ascii

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

cdef extern from * :
    ctypedef signed int int32_t
    ctypedef long long int64_t
    ctypedef unsigned long long uint64_t
    ctypedef unsigned int uint32_t
    ctypedef unsigned short uint16_t

cdef extern from "stdint.h":
    ctypedef unsigned long size_t

cdef extern from "Python.h":
    object PyBytes_FromStringAndSize(const char* str, int size)

cdef extern from "uuos.hpp":

    void n2str_(uint64_t n, string& str_name)
    uint64_t str2n_(string& str_name)

    uint32_t read_action_data_(void* msg, uint32_t len)
    uint32_t action_data_size_()

    void require_recipient_(uint64_t name)
    void require_auth_(uint64_t name)

    void require_auth2_(uint64_t name, uint64_t permission)
    bool has_auth_(uint64_t name)
    bool is_account_(uint64_t name)
    void send_inline_(const char *serialized_action, size_t size)
    void send_context_free_inline_(const char *serialized_action, size_t size)
    uint64_t  publication_time_()
    uint64_t current_receiver_()
    uint32_t get_active_producers_(uint64_t* producers, uint32_t datalen)


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

def n2s(uint64_t account):
    cdef string contract_name
    n2str_(account, contract_name)
    return contract_name

def s2n(account):
    return str2n_(account)

def N(account):
    return str2n_(account)

def to_name(account):
    if isinstance(account, int):
        return account
    return s2n(account)

def read_action_data():
    cdef string data
    cdef uint32_t size
    size = action_data_size_()
    data.resize(size)
    read_action_data_(<char *>data.c_str(), size)
    return <bytes>data

def require_recipient(account):
    account = to_name(account)
    require_recipient_(account)

def require_auth(account):
    account = to_name(account)
    require_auth_(account)

def require_auth2_(name, permission):
    name = to_name(name)
    permission = to_name(permission)
    require_auth2_(name, permission)

def has_auth(name):
    name = to_name(name)
    return has_auth_(name)

def is_account(name):
    name = to_name(name)
    return is_account_(name)

def send_inline(serialized_action):
    send_inline_(serialized_action, len(serialized_action))

def send_context_free_inline(serialized_action):
    send_context_free_inline_(serialized_action, len(serialized_action))

def publication_time():
    return publication_time_()

def current_receiver():
    return current_receiver_()

def get_active_producers():
    cdef vector[uint64_t] producers
    cdef uint32_t producer_count
    producers.resize(21)
    producer_count = get_active_producers_(producers.data(), 21)
    prods = []
    for i in range(producer_count):
        prods.append(producers[i])
    return prods

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
