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

void *chain_new_(string& config, string& protocol_features_dir) {
//    auto plugin = eosio::chain_plugin();
    evm_init();
    vm_api_init();
    vm_api_ro_init();
    chain_api_init();
    sandboxed_contracts_init();

    auto pfs = eosio::initialize_protocol_features( bfs::path(protocol_features_dir) );
    auto cfg = fc::json::from_string(config).as<eosio::chain::controller::config>();
    eosio::chain::controller *ctrl = new eosio::chain::controller(cfg, std::move(pfs));
    ctrl->add_indices();

    auto shutdown = [](){ return false; };
    ctrl->startup(shutdown);

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

    return (void *)ctrl;
}

void chain_free_(void *ptr) {
    delete (eosio::chain::controller*)(ptr);
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
