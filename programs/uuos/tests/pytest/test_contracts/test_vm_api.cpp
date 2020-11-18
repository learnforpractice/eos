#include <eosio/eosio.hpp>
#include <eosio/action.hpp>
#include <eosio/print.hpp>

using namespace eosio;

extern "C" {

    __attribute__((eosio_wasm_import))
    int get_code_hash(uint64_t account, char *hash, size_t size);

    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        if (receiver == code) {
            char hash[32];
            int ret = get_code_hash("alice"_n.value, hash, 32);
            eosio::printhex(hash, 32);
            check(ret, "bad call");

//            memset(hash, 0, 32);
            ret = get_code_hash("uuos.stake"_n.value, hash, 32);
            eosio::printhex(hash, 32);
            check(!ret, "bad call");
        }
    }
}
