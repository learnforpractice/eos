#include <boost/filesystem/path.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>

#include "native_object.hpp"
#include "chain.hpp"

#include <fc/io/json.hpp>
#include <fc/io/raw.hpp>
#include <fc/variant.hpp>
        
namespace bfs = boost::filesystem;

using namespace eosio::chain;

namespace eosio {
    protocol_feature_set initialize_protocol_features( const fc::path& p, bool populate_missing_builtins = true );
}

void chain_on_incoming_block(controller& chain, const signed_block_ptr& block);

extern "C"
{
   void evm_init();
   void chain_api_init();
   void vm_api_ro_init();
   void vm_api_init();
   void sandboxed_contracts_init();
}


class chain_manager {
public:
    chain_manager(string& config, string& protocol_features_dir) {
        evm_init();
        vm_api_init();
        vm_api_ro_init();
        chain_api_init();
        sandboxed_contracts_init();

        auto pfs = eosio::initialize_protocol_features( bfs::path(protocol_features_dir) );
        auto cfg = fc::json::from_string(config).as<eosio::chain::controller::config>();
        cc = new eosio::chain::controller(cfg, std::move(pfs));
        cc->add_indices();

        auto shutdown = [](){ return false; };
        cc->startup(shutdown);

        cc->accepted_block.connect(  boost::bind(&chain_manager::on_accepted_block, this, _1));
    }

    void on_accepted_block(const block_state_ptr& bsp) {
    //    g_chain
    }

    eosio::chain::controller& chain() {
        return *cc;
    }

    ~chain_manager() {
        delete cc;
    }

    eosio::chain::controller *cc = nullptr;
};

static chain_manager *g_manager = nullptr;

void *chain_new_(string& config, string& protocol_features_dir) {
//    auto plugin = eosio::chain_plugin();
/*
    auto shutdown = [](){ return app().is_quiting(); };
    if (my->snapshot_path) {
        auto infile = std::ifstream(my->snapshot_path->generic_string(), (std::ios::in | std::ios::binary));
        auto reader = std::make_shared<istream_snapshot_reader>(infile);
        my->chain->startup(shutdown, reader);
        infile.close();
    } else {
        my->chain->startup(shutdown);
    }
*/
    g_manager = new chain_manager(config, protocol_features_dir);
    return (void *)g_manager->cc;
}

void chain_free_(void *ptr) {
//    g_manager
    delete g_manager;
}

void chain_on_incoming_block_(void *ptr, string& packed_signed_block, uint32_t& num, string& id) {
    try {
        auto& chain = *(eosio::chain::controller*)(ptr);
        std::shared_ptr<signed_block> block = std::make_shared<signed_block>();
        fc::datastream<const char*> ds( packed_signed_block.c_str(), packed_signed_block.size() );
        fc::raw::unpack( ds, *block );
        num = block->block_num();
        id = fc::json::to_string<block_id_type>(block->id());
    //    elog("+++++block: ${block}", ("block", *block));
        chain_on_incoming_block(chain, block);
    } FC_LOG_AND_DROP();
}

uint32_t chain_fork_db_pending_head_block_num_(void *ptr) {
    try {
        auto& chain = *(eosio::chain::controller*)(ptr);
        return chain.fork_db_pending_head_block_num();
    } FC_LOG_AND_DROP();
    return 0;
}

uint32_t chain_last_irreversible_block_num_(void *ptr) {
    try {
        auto& chain = *(eosio::chain::controller*)(ptr);
        return chain.last_irreversible_block_num();
    } FC_LOG_AND_DROP();
    return 0;
}

void chain_get_block_id_for_num_(void *ptr, uint32_t num, string& block_id) {
    try {
        auto& chain = *(eosio::chain::controller*)(ptr);
        block_id = chain.get_block_id_for_num(num).str();
    } FC_LOG_AND_DROP();
}

void chain_id_(void *ptr, string& chain_id) {
    try {
        auto& chain = *(eosio::chain::controller*)(ptr);
        chain_id = chain.get_chain_id().str();
    } FC_LOG_AND_DROP();
}

void chain_fetch_block_by_number_(void *ptr, uint32_t block_num, string& raw_block ) {
    try {
        auto& chain = *(eosio::chain::controller*)(ptr);
        auto block_ptr = chain.fetch_block_by_number(block_num);
        if (!block_ptr) {
            return;
        }
        auto _raw_block = fc::raw::pack<eosio::chain::signed_block>(*block_ptr);
        raw_block = string(_raw_block.data(), _raw_block.size());
    } FC_LOG_AND_DROP();
}

int chain_is_building_block_(void *ptr) {
    auto& chain = *(eosio::chain::controller*)(ptr);
    return chain.is_building_block();
}

