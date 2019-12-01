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
    return (void *)ctrl;
}

void chain_free_(void *ptr) {
    delete (eosio::chain::controller*)(ptr);
}

void chain_on_incoming_block_(void *ptr, string& packed_signed_block) {
    auto& chain = *(eosio::chain::controller*)(ptr);
    std::shared_ptr<signed_block> block = std::make_shared<signed_block>();
    fc::datastream<const char*> ds( packed_signed_block.c_str(), packed_signed_block.size() );
    fc::raw::unpack( ds, *block );
    chain_on_incoming_block(chain, block);
}
