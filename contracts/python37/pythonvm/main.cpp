#include <eosio/print.hpp>
#include <eosio/name.hpp>
#include <eosio/action.hpp>

using namespace eosio;

extern "C" void python_vm_apply(uint64_t receive, uint64_t code, uint64_t action);

extern "C" {
    void sayhello();
    void apply_( uint64_t receiver, uint64_t code, uint64_t action ) {
        python_vm_apply(0, 0, 0);
    }
}