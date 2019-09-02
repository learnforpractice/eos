/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#pragma once

#include <eosio/http_plugin/http_plugin.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>

#include <chain_api.hpp>

#include <appbase/application.hpp>

namespace eosio {

using namespace appbase;

struct db_size_index_count {
   string   index;
   uint64_t row_count;
};

struct db_size_stats {
   uint64_t                    free_bytes;
   uint64_t                    used_bytes;
   uint64_t                    size;
   vector<db_size_index_count> indices;
};

struct enable_debug {
   bool enabled;
};

struct is_debug_enabled {
   bool enabled;
};

struct debug_contract_params {
   string name;
   string path;
};

class debug_api_plugin : public plugin<debug_api_plugin> {
public:
   APPBASE_PLUGIN_REQUIRES((http_plugin) (chain_plugin))

   debug_api_plugin() = default;
   debug_api_plugin(const debug_api_plugin&) = delete;
   debug_api_plugin(debug_api_plugin&&) = delete;
   debug_api_plugin& operator=(const debug_api_plugin&) = delete;
   debug_api_plugin& operator=(debug_api_plugin&&) = delete;
   virtual ~debug_api_plugin() override = default;

   virtual void set_program_options(options_description& cli, options_description& cfg) override {}
   void plugin_initialize(const variables_map& vm) {}
   void plugin_startup();
   void plugin_shutdown() {}

   db_size_stats get();
   bool enable_debug(bool enable);
   bool is_debug_enabled();

   bool add_debug_contract(debug_contract_params& params);
   bool clear_debug_contract(string& contract_name);

private:
};

}

FC_REFLECT( eosio::db_size_index_count, (index)(row_count) )
FC_REFLECT( eosio::db_size_stats, (free_bytes)(used_bytes)(size)(indices) )
FC_REFLECT( eosio::enable_debug, (enabled) )
FC_REFLECT( eosio::is_debug_enabled, (enabled) )
FC_REFLECT( eosio::debug_contract_params, (name)(path) )

