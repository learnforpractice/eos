#pragma once

#include <eosio/chain/types.hpp>
#include <queue>
#include <tuple>

namespace eosio { namespace chain {

class vm_manager {
public:
    static vm_manager* current_instance;
    static vm_manager& get();
    vm_manager();
    ~vm_manager();
    void setcode(int vm_type, uint64_t account, const bytes& code, bytes& output);
    void apply(uint64_t receiver, uint64_t code, uint64_t action);
    void call(uint64_t contract, uint64_t func_name, uint64_t arg1, uint64_t arg2, uint64_t arg3, const char* extra_args, size_t in_size);
    string call_contract_off_chain(uint64_t contract, uint64_t action, const vector<char>& binargs);
    int get_arg(char* arg, size_t size);
    int set_result(const char* result, size_t size);
    int get_result(char* result, size_t size);

    void init();
    bool is_busy();

    vector<char> call_extra_args;
    vector<char> call_returns;

};

}}


