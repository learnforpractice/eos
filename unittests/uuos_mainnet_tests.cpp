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

BOOST_AUTO_TEST_SUITE(uuos_mainnet_tests)

//#include "incbin.h"
//INCBIN(Accounts, "test_genesis_accounts.bin");

BOOST_AUTO_TEST_CASE( genesis_accounts_test ) {
    eosio_system_tester t(true, TEST_GENESIS_ACCOUNTS_FILE);
    t.produce_blocks(1);
//    create_accounts( {N(alice), N(bob), N(charlie)} );
    t.create_account_with_resources(N(alice), N(uuos), ASSET(1000.0000), false, ASSET(10.0000), ASSET(10.0000));
    t.stake( N(uuos), N(alice), ASSET(10.0000), ASSET(10.0000) );
    t.transfer( N(uuos), N(alice), ASSET(100000.0000), N(uuos) );

    t.set_code( config::system_account_name, contracts::eosio_system_wasm_latest() );
    t.set_abi( config::system_account_name, contracts::eosio_system_abi_latest().data() );


//    buyram( N(uuos), N(alice), ASSET(10) );
//    buyrambytes( N(uuos), N(alice), 1024*1024 );
    t.produce_block();

    BOOST_REQUIRE_EXCEPTION( t.create_account_with_resources(N(helloworld11), N(uuos), ASSET(10.0000), false, ASSET(10.0000), ASSET(10.0000)),
                            eosio_assert_message_exception, eosio_assert_message_is( "public key not match!" ) );
    t.produce_block();
    dlog("++++++++++++");
    //create an account that does not exists on main network
    BOOST_REQUIRE_EXCEPTION( t.create_account_with_resources(N(helloworld21), N(uuos), ASSET(10.0000), false, ASSET(10.0000), ASSET(10.0000)),
                            eosio_assert_message_exception, eosio_assert_message_is( "account name does not exists on EOS mainnet" ) );
    dlog("++++++++++++");
    //create an account that less than 12 characters with privileged account
    BOOST_REQUIRE_EXCEPTION( t.create_account_with_resources(N(helloworld), N(uuos), ASSET(10.0000), false, ASSET(10.0000), ASSET(10.0000)),
                            eosio_assert_message_exception, eosio_assert_message_is( "account name does not exists on EOS mainnet" ) );
    dlog("++++++++++++");
    //create an account that less than 12 characters
    BOOST_REQUIRE_EXCEPTION( t.create_account_with_resources(N(helloworld), N(alice), ASSET(10.0000), false, ASSET(10.0000), ASSET(10.0000)),
                            eosio_assert_message_exception, eosio_assert_message_is( "no active bid for name" ) );
    dlog("++++++++++++");
    //create a 13 characters account
    t.create_account_with_resources(N(uuoscontract1), N(alice), ASSET(10.0000), false, ASSET(10.0000), ASSET(10.0000));
    dlog("++++++++++++");
    string str_key("EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV");
    public_key_type key(str_key);
    authority auth( key );
    t.create_account_with_resources( N(helloworld11), N(uuos), auth, auth );
    dlog("++++++++++++");

    dlog("+++++${n}", ("n", t.get_balance(N(helloworld11))));
    t.transfer( N(uuos), N(helloworld11), ASSET(10.0000), N(uuos) );
    t.produce_block();
    dlog("+++++${n}", ("n", t.get_balance(N(helloworld11))));
    return;
}


BOOST_AUTO_TEST_CASE( active_account_test ) try {
    eosio_system_tester t(true, TEST_GENESIS_ACCOUNTS_FILE);
    t.produce_blocks(1);
    dlog("++++++++++++");

//    create_accounts( {N(alice), N(bob), N(charlie)} );
    t.create_account_with_resources(N(alice), N(uuos), ASSET(1000.0000), false, ASSET(10.0000), ASSET(10.0000));
    t.stake( N(uuos), N(alice), ASSET(10.0000), ASSET(10.0000) );
    t.transfer( N(uuos), N(alice), ASSET(100000.0000), N(uuos) );

    dlog("++++++++++++");

    t.set_code( config::system_account_name, contracts::eosio_system_wasm_latest() );
    t.set_abi( config::system_account_name, contracts::eosio_system_abi_latest().data() );


    string str_key("EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV");
/*
    public_key_type key(str_key);
    authority auth( key );
    t.create_account_with_resources( N(helloworld11), N(alice), auth, auth );
*/
    dlog("++++++++++++");
{
    signed_transaction trx;
    string s = "activate helloworld11";
    sha256 hash = sha256::hash(s.c_str(), s.size());
    string str_priv_key("5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc");
    private_key_type priv_key(str_priv_key); 
    signature_type sign = priv_key.sign( hash );

    trx.actions.emplace_back( t.get_action( N(uuos), N(activateacc),
                                           vector<permission_level>{{N(alice), config::active_name}},
                                           mutable_variant_object()
                                             ("activator", "alice")
                                             ("account", "helloworld11")
                                             ("sign", sign)
    ) );

    t.set_transaction_headers(trx);
    trx.sign( get_private_key( N(alice), "active" ), t.control->get_chain_id()  );
    t.push_transaction( trx );
    t.produce_block();
}

{
    signed_transaction trx;
    string s = "activate helloworld11";
    sha256 hash = sha256::hash(s.c_str(), s.size());
    string str_priv_key("5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc");
    private_key_type priv_key(str_priv_key); 
    signature_type sign = priv_key.sign( hash );

    trx.actions.emplace_back( t.get_action( N(uuos), N(activateacc),
                                           vector<permission_level>{{N(alice), config::active_name}},
                                           mutable_variant_object()
                                             ("activator", "alice")
                                             ("account", "helloworld11")
                                             ("sign", sign)
    ) );

    t.set_transaction_headers(trx);
    trx.sign( get_private_key( N(alice), "active" ), t.control->get_chain_id()  );
    BOOST_REQUIRE_EXCEPTION( t.push_transaction( trx ),
                            eosio_assert_message_exception, eosio_assert_message_is( "account has already been activated" ) );
    t.produce_block();
}

{
    signed_transaction trx;
    string s = "activate helloworld11";
    sha256 hash = sha256::hash(s.c_str(), s.size());
    string str_priv_key("5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3");
    private_key_type priv_key(str_priv_key); 
    signature_type sign = priv_key.sign( hash );

    trx.actions.emplace_back( t.get_action( N(uuos), N(activateacc),
                                           vector<permission_level>{{N(alice), config::active_name}},
                                           mutable_variant_object()
                                             ("activator", "alice")
                                             ("account", "helloworld12")
                                             ("sign", sign)
    ) );

    t.set_transaction_headers(trx);
    trx.sign( get_private_key( N(alice), "active" ), t.control->get_chain_id()  );
    BOOST_REQUIRE_EXCEPTION( t.push_transaction( trx ),
                            eosio_assert_message_exception,
                            eosio_assert_message_is( "public key mismatch" ) 
    );
    t.produce_block();
}

    dlog("+++++${n}", ("n", t.get_balance(N(helloworld11))));
    t.transfer( N(uuos), N(helloworld11), ASSET(10.0000), N(uuos) );
    t.produce_block();
    dlog("+++++${n}", ("n", t.get_balance(N(helloworld11))));
} FC_LOG_AND_RETHROW()


BOOST_AUTO_TEST_CASE( producer_test ) try {
    eosio_system_tester t(true, TEST_GENESIS_ACCOUNTS_FILE);
    t.produce_blocks(1);

//    create_accounts( {N(alice), N(bob), N(charlie)} );
    t.create_account_with_resources(N(alice), N(uuos), ASSET(1000.0000), false, ASSET(1000000.0000), ASSET(10.0000), true);
    t.create_account_with_resources(N(bob), N(uuos), ASSET(1000.0000), false, ASSET(999999.0000), ASSET(0.9000), true);

    t.set_code( config::system_account_name, contracts::eosio_system_wasm_latest() );
    t.set_abi( config::system_account_name, contracts::eosio_system_abi_latest().data() );
    t.push_action( N(alice), N(regproducer), mutable_variant_object()
                                             ("producer", "alice")
                                             ("producer_key", "EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV")
                                             ("url", "https://uuos.io")
                                             ("location", 1)
    );

    t.push_action2( N(uuos), N(undelegatebw), N(alice), mvo()
                          ("from",     "alice")
                          ("receiver", "alice")
                          ("unstake_net_quantity", ASSET(900000.0000))
                          ("unstake_cpu_quantity", ASSET(1.0000))
    );

    signed_transaction trx;
    trx.actions.emplace_back( t.get_action( N(uuos), N(regproducer),
                                           vector<permission_level>{{N(bob), config::active_name}},
                                           mutable_variant_object()
                                             ("producer", "bob")
                                             ("producer_key", "EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV")
                                             ("url", "https://uuos.io")
                                             ("location", 1)
    ) );
    t.set_transaction_headers(trx);
    trx.sign( get_private_key( N(bob), "active" ), t.control->get_chain_id() );
    BOOST_REQUIRE_EXCEPTION(t.push_transaction( trx ),
                            eosio_assert_message_exception,
                            eosio_assert_message_is("no enough staking")
    );


#if 0
    BOOST_REQUIRE_EXCEPTION(t.regproducer(N(bob)),
                            eosio_assert_message_exception,
                            eosio_assert_message_is("no enough staking")
    );

    BOOST_REQUIRE_EXCEPTION(t.push_action( N(bob), N(regproducer), mutable_variant_object()
                                             ("producer", "bob")
                                             ("producer_key", "EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV")
                                             ("url", "https://uuos.io")
                                             ("location", 1)
                            ),
                            eosio_assert_message_exception,
                            eosio_assert_message_is("no enough staking")
    );
#endif
} FC_LOG_AND_RETHROW()

#if 0
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
