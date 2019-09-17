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

#include "fork_test_utilities.hpp"
#include "eosio_system_tester.hpp"

using namespace eosio::chain;
using namespace eosio::testing;
using namespace eosio_system;

#define ASSET(s) eosio::chain::asset::from_string(#s" SYS")

BOOST_AUTO_TEST_SUITE(uuos_mainnet_tests)

BOOST_AUTO_TEST_CASE( register_eos_main_net_account_test ) {
    eosio_system_tester t(true);
    t.produce_blocks(1);

//    create_accounts( {N(alice), N(bob), N(charlie)} );
    dlog("+++++++++++++${n}", ("n", __LINE__));
    t.create_account_with_resources(N(alice), N(eosio), ASSET(1000.0000), false, ASSET(10.0000), ASSET(10.0000));
    t.stake( N(eosio), N(alice), ASSET(10.0000), ASSET(10.0000) );
    t.transfer( N(eosio), N(alice), ASSET(100000.0000), N(eosio) );

    t.set_code( config::system_account_name, contracts::eosio_system_wasm_latest() );
    t.set_abi( config::system_account_name, contracts::eosio_system_abi_latest().data() );


//    buyram( N(eosio), N(alice), ASSET(10) );
//    buyrambytes( N(eosio), N(alice), 1024*1024 );
    t.produce_block();

    t.create_account_with_resources(N(uuoscontract), N(alice), ASSET(10.0000), false, ASSET(10.0000), ASSET(10.0000));
    t.produce_block();

    BOOST_REQUIRE_EXCEPTION( t.create_account_with_resources(N(uuoscontrac), N(alice), ASSET(10.0000), false, ASSET(10.0000), ASSET(10.0000)),
                            eosio_assert_message_exception, eosio_assert_message_is( "no active bid for name" ) );

    //create an account that does not exists on EOS main network
    BOOST_REQUIRE_EXCEPTION( t.create_account_with_resources(N(uuoscontracy), N(alice), ASSET(10.0000), false, ASSET(10.0000), ASSET(10.0000)),
                            eosio_assert_message_exception, eosio_assert_message_is( "account name does not exists on EOS mainnet" ) );

    //create a 13 characters account
    t.create_account_with_resources(N(uuoscontract1), N(alice), ASSET(10.0000), false, ASSET(10.0000), ASSET(10.0000));

    t.produce_block();

    t.transfer( N(eosio), N(uuoscontract), ASSET(10.0000), N(eosio) );
    t.produce_block();
    return;
}

#if 1
BOOST_AUTO_TEST_CASE( test1 ) try {
//   tester c( setup_policy::preactivate_feature_and_new_bios );
   tester c( setup_policy::none );
   return;
   c.create_accounts( {N(alice), N(bob), N(charlie)} );
   c.produce_block();
   vector<uint8_t> v;
   v.resize(1);
   v.data()[0] = 0; 
   c.set_code( N(bob), v, 0, nullptr);
{
   vector<uint8_t> d;
   auto r = c.push_action( N(bob), N(testcreate), d, N(bob));
   ilog("{console}", ("console", r->action_traces[0].console));
   c.produce_block();
}

} FC_LOG_AND_RETHROW()
#endif

BOOST_AUTO_TEST_SUITE_END()
