#include "vm_manager.hpp"
//#include <vm_interface.hpp>

#include <fc/scoped_exit.hpp>
#include <eosio/chain/db_api.hpp>
#include <chain_api.hpp>

#include <vm_api/vm_api.h>

extern "C" const unsigned char pythonvm_wasm[];
extern "C" int pythonvm_wasm_size;
extern "C" char pythonvm_wasm_hash[];

static vm_api _vm_api;
extern "C" struct vm_api* get_vm_api_ro() {
   return &_vm_api;
}

extern "C" {
    size_t get_last_error(char* error, size_t size);
    int evm_apply(uint64_t receiver, uint64_t code, uint64_t action);

//    void vm_python2_init();
    int vm_python2_apply(uint64_t receiver, uint64_t account, uint64_t act);

    void wasm_interface_init_1(int vm_type);
    bool wasm_interface_apply_1(const eosio::chain::digest_type& code_id, const uint8_t& vm_type, const uint8_t& vm_version);
    bool wasm_interface_call_1(uint64_t contract, uint64_t func_name, uint64_t arg1, uint64_t arg2, uint64_t arg3);
}

using namespace eosio::chain;

namespace eosio { namespace chain {

vm_manager* vm_manager::current_instance = nullptr;

vm_manager::vm_manager() {
    init();
}

void vm_manager::init() {
//    vm_python2_init();
//    wasm_interface_init_1(1);
}

vm_manager::~vm_manager() {
}

vm_manager& vm_manager::get() {
    if (current_instance == nullptr) {
        current_instance = new vm_manager();
    }
    return *current_instance;
}

void vm_manager::setcode(int vm_type, uint64_t account, const bytes& code, bytes& output) {
}

extern "C" void eosio_apply(uint64_t receiver, uint64_t code, uint64_t action);

void vm_manager::apply(uint64_t receiver, uint64_t code, uint64_t action) {
    call_returns.resize(0);
    int vm_type = get_chain_api()->get_code_type(receiver);
    if (vm_type == VM_TYPE_PY) {
        bool activated = get_chain_api()->is_builtin_activated((uint32_t)enum_builtin_protocol_feature::python_vm);
        EOSIO_ASSERT(activated, "pythonvm not activated!");
//        vm_python2_apply(receiver, code, action);
    }
}

string vm_manager::call_contract_off_chain(uint64_t contract, uint64_t action, const vector<char>& binargs) {
    return get_chain_api()->call_contract_off_chain(contract, action, binargs);
}

int vm_manager::get_extra_args(char* args, size_t size) {
    if (!args || size == 0) {
        return call_extra_args.size();
    }
    int copy_size = std::min(size, call_extra_args.size());
    memcpy(args, call_extra_args.data(), copy_size);
    return copy_size;
}

int vm_manager::set_result(const char* result, size_t size) {
    call_returns.resize(size);
    memcpy(call_returns.data(), result, size);
    return size;
}

int vm_manager::get_result(char* result, size_t size) {
    if (!result || size == 0) {
        return call_returns.size();
    }
    int copy_size = std::min(size, call_returns.size());
    memcpy(result, call_returns.data(), copy_size);
    return copy_size;
}

bool vm_manager::is_busy() {
    return false;
//    return vm_wavm.is_busy();
}

}}

