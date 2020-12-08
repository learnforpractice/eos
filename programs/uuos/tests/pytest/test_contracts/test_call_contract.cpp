#include <eosio/eosio.hpp>
#include <eosio/action.hpp>
#include <eosio/print.hpp>

extern "C" {
    __attribute__((eosio_wasm_import))
    int call_contract_get_args(void* args, size_t size1);

    __attribute__((eosio_wasm_import))
    int call_contract_set_results(void* result, size_t size1);

    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        if (receiver == code) {
            uint64_t args[2];
            int args_size = ::call_contract_get_args(&args, sizeof(args));
            eosio::print("+++++++++++call: arg size:", args_size, "\n");
            eosio::check(args_size == 16, "bad args size");
            if (args[0] == eosio::name("calltest1").value) {
                eosio::print("+++++++++++call: args[1]:", args[1], "\n");
                args[1] += 1;
                ::call_contract_set_results(&args[1], sizeof(uint64_t));
            }
        }
    }
}
