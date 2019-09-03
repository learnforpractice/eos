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

using namespace eosio::chain;
using namespace eosio::testing;

BOOST_AUTO_TEST_SUITE(pythonvm_tests)

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
