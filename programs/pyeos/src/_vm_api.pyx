cdef extern from * :
    ctypedef signed int int32_t
    ctypedef long long int64_t
    ctypedef unsigned long long uint64_t
    ctypedef unsigned int uint32_t
    ctypedef unsigned short uint16_t

cdef extern from "uuos.hpp":
   int32_t db_store_i64_(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id,  const char* data, uint32_t _len);

def db_store_i64(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, data: bytes):
    return db_store_i64_(scope, table, payer, id,  data, len(data))


