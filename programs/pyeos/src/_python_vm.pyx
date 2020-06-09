# cython: c_string_type=str, c_string_encoding=ascii

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

cdef extern from * :
    ctypedef long long int64_t
    ctypedef unsigned long long uint64_t
    ctypedef unsigned int uint32_t
    ctypedef unsigned short uint16_t
    ctypedef unsigned char uint8_t

cdef extern from "uuos.hpp":
    void say_hello_();
    void n2str_(uint64_t n, string& str_name);
    bool get_apply_args_(uint64_t& receiver, uint64_t& code, uint64_t& action);


import os
import marshal
import traceback

module = type(os)
py_contracts = {}

def n2s(uint64_t account):
    cdef string contract_name
    n2str_(account, contract_name)
    return contract_name

cdef extern int cpython_setcode(uint64_t account, string& raw_code):
    print('+++++++++hello,world+++++++++++')
    try:
        if raw_code.size() == 0:
            if account in py_contracts:
                del py_contracts[account]
            return 1

        _raw_code = <bytes>(&raw_code)[0]
        code = marshal.loads(_raw_code)
        contract_name = n2s(account)
        print(account, contract_name)
        m = module(contract_name)
        exec(code, m.__dict__)
        if account in py_contracts:
            del py_contracts[account]
        py_contracts[account] = m
        return 1;
    except Exception as e:
        traceback.print_exc(e)
    return 0

cdef extern int cpython_apply(string& _hash, uint8_t vmtype, uint8_t vmversion):
    cdef uint64_t receiver
    cdef uint64_t code
    cdef uint64_t action
    try:
        get_apply_args_(receiver, code, action)
        py_contracts[code].apply(receiver, code, action)
        return 1
    except Exception as e:
        traceback.print_exc(e)
    return 0
