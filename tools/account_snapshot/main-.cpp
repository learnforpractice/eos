//#include <eosio/chain/db_api.hpp>

#include <eosio/chain/block.hpp>
#include <eosio/chain/account_object.hpp>
#include <eosio/chain/code_object.hpp>
#include <eosio/chain/block_summary_object.hpp>
#include <eosio/chain/eosio_contract.hpp>
#include <eosio/chain/global_property_object.hpp>
#include <eosio/chain/protocol_state_object.hpp>
#include <eosio/chain/contract_table_objects.hpp>
#include <eosio/chain/generated_transaction_object.hpp>
#include <eosio/chain/transaction_object.hpp>
#include <eosio/chain/reversible_block_object.hpp>
#include <eosio/chain/account_object.hpp>

#include <boost/algorithm/string.hpp>

#include <eosio/chain/db_api.hpp>
#include <chain_api.hpp>
#include <stacktrace.h>

#include <chainbase/chainbase.hpp>

using namespace eosio::chain;

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: account_snapshot [state_dir]\n");
        exit(0);
    }
    string path(argv[1], strlen(argv[1]));

    chainbase::database db = chainbase::database(fc::path(path), chainbase::database::read_only, config::default_state_size, true);
    printf("+++%d\n", __LINE__);

    db.add_index<account_index>();
    db.add_index<account_metadata_index>();
    db.add_index<account_ram_correction_index>();
    db.add_index<global_property_multi_index>();
    db.add_index<protocol_state_multi_index>();
    db.add_index<dynamic_global_property_multi_index>();
    db.add_index<block_summary_multi_index>();
    db.add_index<transaction_multi_index>();
    db.add_index<generated_transaction_multi_index>();
    db.add_index<table_id_multi_index>();
    db.add_index<code_index>();

    db.add_index<key_value_index>();
    db.add_index<index64_index>();
    db.add_index<index128_index>();
    db.add_index<index256_index>();
    db.add_index<index_double_index>();
    db.add_index<index_long_double_index>();
    db.add_index<key256_value_index>();

    printf("+++%d\n", __LINE__);

//    db_api api(path);
//    auto idx = api.db.get_index<account_metadata_index>().indices();
    auto idx = db.get_index<account_metadata_index>().indices();
    printf("+++%d\n", __LINE__);
    auto itr = idx.get<by_name>().end();
    printf("+++%d\n", __LINE__);

    auto itr2 = idx.get<by_name>().upper_bound(itr->name);
    printf("+++%d\n", __LINE__);

    while (itr2 != idx.get<by_name>().end()) {
        dlog("++++${n}", ("n", itr2->name));
        itr2 = idx.get<by_name>().upper_bound(itr2->name);
        printf("+++%d\n", __LINE__);
    }
}


