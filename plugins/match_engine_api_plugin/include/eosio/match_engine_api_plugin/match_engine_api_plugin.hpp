/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once
#include <appbase/application.hpp>
#include <eosio/match_engine_plugin/match_engine_plugin.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>

namespace eosio {

using namespace appbase;

/**
 *  This is a template plugin, intended to serve as a starting point for making new plugins
 */
class match_engine_api_plugin : public appbase::plugin<match_engine_api_plugin> {
public:
   match_engine_api_plugin();
   virtual ~match_engine_api_plugin();
 
   APPBASE_PLUGIN_REQUIRES((match_engine_plugin)(chain_plugin)(http_plugin))
   virtual void set_program_options(options_description&, options_description& cfg) override;
 
   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

private:
   std::unique_ptr<class match_engine_api_plugin_impl> my;
};

}
