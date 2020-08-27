/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <eosio/chain/generated_transaction_object.hpp>
#include <eosio/testing/tester.hpp>

#include <Runtime/Runtime.h>

#include <fc/variant_object.hpp>
#include <fc/io/raw.hpp>
#include <fc/log/logger.hpp>

#include <boost/test/unit_test.hpp>

#include <contracts.hpp>

#include "native_contracts.hpp"

#include "fork_test_utilities.hpp"

#include "native_contract_test/native_contract_test_define.h"

#include "eosio_system_tester.hpp"

using namespace eosio::chain;
using namespace eosio::testing;
using namespace eosio_system;

using mvo = fc::mutable_variant_object;

#define ASSET(s) eosio::chain::asset::from_string(#s" SYS")

extern "C" {
    void sandboxed_contracts_init();
//    void WASM_RT_ADD_PREFIX(init_08a709bcc192c5dd587f1b8e8a5fd27ebaac4aeffe313e72c792e684f9471cb0)(void);
//    wasm_rt_memory_t* get_memory_08a709bcc192c5dd587f1b8e8a5fd27ebaac4aeffe313e72c792e684f9471cb0(void);
//    void (*WASM_RT_ADD_PREFIX(apply_08a709bcc192c5dd587f1b8e8a5fd27ebaac4aeffe313e72c792e684f9471cb0))(u64, u64, u64);
    void add_native_contract(std::array<uint8_t, 32>& hash, contract _contract);
}

static bool is_deadline_exception(const deadline_exception& e) { return true; }

void call_test(TESTER& test, action& act, uint32_t billed_cpu_time_us , uint32_t max_cpu_usage_ms = 200 ) {
   signed_transaction trx;

   trx.actions.push_back(act);
   test.set_transaction_headers(trx);
   auto sigs = trx.sign(test.get_private_key(N(testapi), "active"), test.control->get_chain_id());
   flat_set<public_key_type> keys;
   trx.get_signature_keys(test.control->get_chain_id(), fc::time_point::maximum(), keys);
   auto res = test.push_transaction( trx, fc::time_point::now() + fc::milliseconds(max_cpu_usage_ms), billed_cpu_time_us );
   BOOST_CHECK_EQUAL(res->receipt->status, transaction_receipt::executed);
   test.produce_block();
};

void contract_tests() {
    eosio_system_tester t;
    //   tester t;//( setup_policy::full );
    t.create_account_with_resources(N(testapi), N(uuos), ASSET(1000.0000), false, ASSET(10.0000), ASSET(10.0000));
    //   t.create_account( N(testapi) );

    t.produce_block();

    t.set_code( N(testapi), contracts::native_contract_test_wasm() );
    t.set_abi( N(testapi), contracts::native_contract_test_abi().data() );

    t.push_action2( N(testapi), N(anyaction), N(testapi), mvo()
        ( "test", "staticvar")
    );
    t.produce_block();

    t.push_action2( N(testapi), N(anyaction), N(testapi), mvo()
        ( "test", "staticvar")
    );
    t.produce_block();
/*
    t.push_action2( N(testapi), N(anyaction), N(testapi), mvo()
        ( "test", "checktime")
    );
*/
{
    action a;
    a.authorization = vector<permission_level>{{N(testapi), config::active_name}};
    a.account = N(testapi);
    a.name = N(anyaction);
    a.data = fc::raw::pack(N(checktime));
    BOOST_CHECK_EXCEPTION(call_test( t, a, 5000, 200 ), deadline_exception, is_deadline_exception);
}

    t.produce_block();

    t.push_action2( N(testapi), N(anyaction), N(testapi), mvo()( "test", "memtest1") );
    t.produce_block();

    BOOST_REQUIRE_EXCEPTION( t.push_action2( N(testapi), N(anyaction), N(testapi), mvo()( "test", "memtest2") ),
                            eosio_assert_message_exception, eosio_assert_message_is( "fail to grow" )
    );

    t.produce_block();

//    t.push_action2( N(testapi), N(anyaction), N(testapi), mvo()( "test", "recursive") );
{
    action a;
    a.authorization = vector<permission_level>{{N(testapi), config::active_name}};
    a.account = N(testapi);
    a.name = N(anyaction);
    a.data = fc::raw::pack(N(recursive));
    BOOST_CHECK_EXCEPTION(call_test( t, a, 5000, 200 ), deadline_exception, is_deadline_exception);
}
    t.produce_block();
}

BOOST_AUTO_TEST_SUITE(native_contract_tests)

BOOST_AUTO_TEST_CASE( test1 ) try {
    contract_tests();
} FC_LOG_AND_RETHROW()


BOOST_AUTO_TEST_CASE( test2 ) try {
   sandboxed_contracts_init();
   std::array<uint8_t, 32> hash = WASM_HASH;
   auto _contract = contract{
        .initialized = false,
        .init = INIT,
        .get_memory = GET_MEMORY,
        .apply = &APPLY,
    };

    add_native_contract(hash, _contract);

    contract_tests();

} FC_LOG_AND_RETHROW()



BOOST_AUTO_TEST_SUITE_END()
