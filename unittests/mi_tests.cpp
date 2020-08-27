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

#include "db_interface.hpp"

using namespace eosio::chain;
using namespace eosio::testing;
using namespace eosio_system;

#define ASSET(s) eosio::chain::asset::from_string(#s" SYS")

BOOST_AUTO_TEST_SUITE(mi_tests)

//#include "incbin.h"
//INCBIN(Accounts, "test_genesis_accounts.bin");

BOOST_AUTO_TEST_CASE( mi_test1 ) try {
    eosio_system_tester t(true, TEST_GENESIS_ACCOUNTS_FILE);
    t.produce_blocks(1);
    dlog("++++++++++++");

//    create_accounts( {N(alice), N(bob), N(charlie)} );
    t.create_account_with_resources(N(alice), N(uuos), ASSET(1000.0000), false, ASSET(10.0000), ASSET(10.0000));
    t.stake( N(uuos), N(alice), ASSET(10.0000), ASSET(10.0000) );
    t.transfer( N(uuos), N(alice), ASSET(100000.0000), N(uuos) );

    dlog("++++++++++++");

    t.set_code( N(alice), contracts::mi_test_wasm() );
//    t.set_abi( N(alice),  contracts::mi_test_abi().data() );

} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
