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
    void native_contracts_init();
//    void WASM_RT_ADD_PREFIX(init_08a709bcc192c5dd587f1b8e8a5fd27ebaac4aeffe313e72c792e684f9471cb0)(void);
//    wasm_rt_memory_t* get_memory_08a709bcc192c5dd587f1b8e8a5fd27ebaac4aeffe313e72c792e684f9471cb0(void);
//    void (*WASM_RT_ADD_PREFIX(apply_08a709bcc192c5dd587f1b8e8a5fd27ebaac4aeffe313e72c792e684f9471cb0))(u64, u64, u64);
    void add_native_contract(std::array<uint8_t, 32>& hash, contract _contract);
}

BOOST_AUTO_TEST_SUITE(native_contract_tests)


BOOST_AUTO_TEST_CASE( test1 ) try {
   native_contracts_init();
   std::array<uint8_t, 32> hash = WASM_HASH;
   auto _contract = contract{
        .initialized = false,
        .init = INIT,
        .get_memory = GET_MEMORY,
        .apply = &APPLY,
    };

    add_native_contract(hash, _contract);

   eosio_system_tester t;

//   tester c( setup_policy::preactivate_feature_and_new_bios );
   t.create_account_with_resources(N(bob), N(eosio), ASSET(1000.0000), false, ASSET(10.0000), ASSET(10.0000));

   t.produce_block();

   t.set_code( N(bob), contracts::native_contract_test_wasm() );
   t.set_abi( N(bob), contracts::native_contract_test_abi().data() );

    t.push_action2( N(bob), N(anyaction), N(bob), mvo()
        ( "test", "staticvar")
    );
    t.produce_block();

    t.push_action2( N(bob), N(anyaction), N(bob), mvo()
        ( "test", "staticvar")
    );
    t.produce_block();

    t.push_action2( N(bob), N(anyaction), N(bob), mvo()
        ( "test", "checktime")
    );
    t.produce_block();

    t.push_action2( N(bob), N(anyaction), N(bob), mvo()( "test", "memtest1") );
    t.produce_block();

    BOOST_REQUIRE_EXCEPTION( t.push_action2( N(bob), N(anyaction), N(bob), mvo()( "test", "memtest2") ),
                            eosio_assert_message_exception, eosio_assert_message_is( "fail to grow" )
    );

    t.produce_block();

    t.push_action2( N(bob), N(anyaction), N(bob), mvo()( "test", "recursive") );
    t.produce_block();

} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
