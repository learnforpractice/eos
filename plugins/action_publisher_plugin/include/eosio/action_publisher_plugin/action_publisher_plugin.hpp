/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#pragma once
#include <appbase/application.hpp>

#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>

namespace fc { class variant; }

namespace eosio {
   using chain::transaction_id_type;
   using std::shared_ptr;
   using namespace appbase;
   using chain::name;
   using fc::optional;
   using chain::uint128_t;

   typedef shared_ptr<class action_publisher_plugin_impl> history_ptr;
   typedef shared_ptr<const class action_publisher_plugin_impl> history_const_ptr;

/**
 *  This plugin tracks all actions and keys associated with a set of configured accounts. It enables
 *  wallets to paginate queries for history.
 *
 *  An action will be included in the account's history if any of the following:
 *     - receiver
 *     - any account named in auth list
 *
 *  A key will be linked to an account if the key is referneced in authorities of updateauth or newaccount
 */
class action_publisher_plugin : public plugin<action_publisher_plugin> {
   public:
      APPBASE_PLUGIN_REQUIRES((chain_plugin)(http_plugin))

      action_publisher_plugin();
      virtual ~action_publisher_plugin();

      virtual void set_program_options(options_description& cli, options_description& cfg) override;

      void plugin_initialize(const variables_map& options);
      void plugin_startup();
      void plugin_shutdown();

      bool set_filter_on(string& filter_in);
      bool set_filter_out(string& filter_out);
      bool clear_filter_on();
      bool clear_filter_out();

   private:
      history_ptr my;
};

} /// namespace eosio

