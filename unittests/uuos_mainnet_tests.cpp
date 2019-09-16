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

BOOST_FIXTURE_TEST_CASE( register_eos_main_net_account_test, eosio_system_tester ) {
    produce_blocks(1);
//    create_accounts( {N(alice), N(bob), N(charlie)} );
    create_account_with_resources(N(alice), N(eosio), ASSET(1000.0000), false, ASSET(10.0000), ASSET(10.0000));
    stake( N(eosio), N(alice), ASSET(10.0000), ASSET(10.0000) );
    transfer( N(eosio), N(alice), ASSET(100000.0000), N(eosio) );

//    buyram( N(eosio), N(alice), ASSET(10) );
//    buyrambytes( N(eosio), N(alice), 1024*1024 );
    produce_block();
    
    create_account_with_resources(N(helloworld11), N(alice), ASSET(10.0000), false, ASSET(10.0000), ASSET(10.0000));
    transfer( N(eosio), N(helloworld11), ASSET(10.0000), N(eosio) );
    produce_block();
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
