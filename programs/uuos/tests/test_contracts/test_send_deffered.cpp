#include <vector>
#include <eosio/print.hpp>
#include <eosio/name.hpp>
#include <eosio/action.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>

using namespace std;
using namespace eosio;

#define N(name) name##_n.value

struct transfer {
    name from;
    name to;
    asset amount;
    string memo;
    EOSLIB_SERIALIZE( transfer, (from)(to)(amount)(memo) )
};


extern "C" {
    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        if (N("sayhello") == action) {
            name alice("alice");
            struct action a;
            a.account = "uuos.token"_n;
            a.name = "transfer"_n;
            a.authorization.push_back({alice, "active"_n});
            
            transfer t;
            t.from = alice;
            t.to = "bob"_n;
            t.amount.amount = 10000;
            t.amount.symbol = symbol("UUOS", 4);
            a.data = eosio::pack<transfer>(t);
            
            transaction trx;
            trx.delay_sec = 3;
            trx.actions.push_back(a);
            trx.send(1, "alice"_n);
        }
    }
}
