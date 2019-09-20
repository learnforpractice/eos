#include <eosio/action_publisher_plugin/action_publisher_plugin.hpp>
#include <eosio/chain/controller.hpp>
#include <eosio/chain/trace.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>

#include <fc/io/json.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/signals2/connection.hpp>

#include "zhelpers.hpp"

namespace eosio {
   using namespace chain;
   using boost::signals2::scoped_connection;

   static appbase::abstract_plugin& _action_publisher_plugin = app().register_plugin<action_publisher_plugin>();

   struct filter_entry {
      name receiver;
      name action;
      name actor;

      std::tuple<name, name, name> key() const {
         return std::make_tuple(receiver, action, actor);
      }

      friend bool operator<( const filter_entry& a, const filter_entry& b ) {
         return a.key() < b.key();
      }
   };

   class action_publisher_plugin_impl {
      public:
         bool bypass_filter = false;
         std::set<filter_entry> filter_on;
         std::set<filter_entry> filter_out;
         chain_plugin*          chain_plug = nullptr;
         fc::optional<scoped_connection> applied_transaction_connection;

         unique_ptr<zmq::context_t> context;
         unique_ptr<zmq::socket_t> publisher;

          bool filter(const action_trace& act) {
            bool pass_on = false;
            if (bypass_filter) {
              pass_on = true;
            }
            if (filter_on.find({ act.receiver, 0, 0 }) != filter_on.end()) {
              pass_on = true;
            }
            if (filter_on.find({ act.receiver, act.act.name, 0 }) != filter_on.end()) {
              pass_on = true;
            }
            for (const auto& a : act.act.authorization) {
              if (filter_on.find({ act.receiver, 0, a.actor }) != filter_on.end()) {
                pass_on = true;
              }
              if (filter_on.find({ act.receiver, act.act.name, a.actor }) != filter_on.end()) {
                pass_on = true;
              }
            }

            if (!pass_on) {  return false;  }

            if (filter_out.find({ act.receiver, 0, 0 }) != filter_out.end()) {
              return false;
            }
            if (filter_out.find({ act.receiver, act.act.name, 0 }) != filter_out.end()) {
              return false;
            }
            for (const auto& a : act.act.authorization) {
              if (filter_out.find({ act.receiver, 0, a.actor }) != filter_out.end()) {
                return false;
              }
              if (filter_out.find({ act.receiver, act.act.name, a.actor }) != filter_out.end()) {
                return false;
              }
            }

            return true;
          }

         set<account_name> account_set( const action_trace& act ) {
            set<account_name> result;

            result.insert( act.receiver );
            for( const auto& a : act.act.authorization ) {
               if( bypass_filter ||
                   filter_on.find({ act.receiver, 0, 0}) != filter_on.end() ||
                   filter_on.find({ act.receiver, 0, a.actor}) != filter_on.end() ||
                   filter_on.find({ act.receiver, act.act.name, 0}) != filter_on.end() ||
                   filter_on.find({ act.receiver, act.act.name, a.actor }) != filter_on.end() ) {
                 if ((filter_out.find({ act.receiver, 0, 0 }) == filter_out.end()) &&
                     (filter_out.find({ act.receiver, 0, a.actor }) == filter_out.end()) &&
                     (filter_out.find({ act.receiver, act.act.name, 0 }) == filter_out.end()) &&
                     (filter_out.find({ act.receiver, act.act.name, a.actor }) == filter_out.end())) {
                   result.insert( a.actor );
                 }
               }
            }
            return result;
         }

         void record_account_action( account_name n, const action_trace& act ) {
            /*
            auto& chain = chain_plug->chain();
            chainbase::database& db = const_cast<chainbase::database&>( chain.db() ); // Override read-only access to state DB (highly unrecommended practice!)

            const auto& idx = db.get_index<account_history_index, by_account_action_seq>();
            auto itr = idx.lower_bound( boost::make_tuple( name(n.value+1), 0 ) );

            uint64_t asn = 0;
            if( itr != idx.begin() ) --itr;
            if( itr->account == n )
               asn = itr->account_sequence_num + 1;

            const auto& a = db.create<account_history_object>( [&]( auto& aho ) {
              aho.account = n;
              aho.action_sequence_num = act.receipt->global_sequence;
              aho.account_sequence_num = asn;
            });
            */
         }

         void on_system_action( const action_trace& at ) {
#if 0
            auto& chain = chain_plug->chain();
            chainbase::database& db = const_cast<chainbase::database&>( chain.db() ); // Override read-only access to state DB (highly unrecommended practice!)
            if( at.act.name == N(newaccount) )
            {
               /*
               const auto create = at.act.data_as<chain::newaccount>();
               add(db, create.owner.keys, create.name, N(owner));
               add(db, create.owner.accounts, create.name, N(owner));
               add(db, create.active.keys, create.name, N(active));
               add(db, create.active.accounts, create.name, N(active));
               */
            }
            else if( at.act.name == N(updateauth) )
            {
               /*
               const auto update = at.act.data_as<chain::updateauth>();
               remove<public_key_history_multi_index, by_account_permission>(db, update.account, update.permission);
               remove<account_control_history_multi_index, by_controlled_authority>(db, update.account, update.permission);
               add(db, update.auth.keys, update.account, update.permission);
               add(db, update.auth.accounts, update.account, update.permission);
               */
            }
            else if( at.act.name == N(deleteauth) )
            {
               /*
               const auto del = at.act.data_as<chain::deleteauth>();
               remove<public_key_history_multi_index, by_account_permission>(db, del.account, del.permission);
               remove<account_control_history_multi_index, by_controlled_authority>(db, del.account, del.permission);
               */
            }
#endif
         }

         void on_action_trace( const action_trace& at ) {
            if( filter( at ) ) {
               dlog("++++++on_action_trace");
               s_sendmore (*publisher, "1111");
               s_send (*publisher, fc::json::to_string(at));
               //idump((fc::json::to_pretty_string(at)));
               #if 0
               auto& chain = chain_plug->chain();
               chainbase::database& db = const_cast<chainbase::database&>( chain.db() ); // Override read-only access to state DB (highly unrecommended practice!)

               db.create<action_history_object>( [&]( auto& aho ) {
                  auto ps = fc::raw::pack_size( at );
                  aho.packed_action_trace.resize(ps);
                  datastream<char*> ds( aho.packed_action_trace.data(), ps );
                  fc::raw::pack( ds, at );
                  aho.action_sequence_num = at.receipt->global_sequence;
                  aho.block_num = chain.head_block_num() + 1;
                  aho.block_time = chain.pending_block_time();
                  aho.trx_id     = at.trx_id;
               });

               auto aset = account_set( at );
               for( auto a : aset ) {
                  record_account_action( a, at );
               }
               #endif
            }
            if( at.receiver == chain::config::system_account_name )
               on_system_action( at );
         }

         void on_applied_transaction( const transaction_trace_ptr& trace ) {
            if( !trace->receipt || (trace->receipt->status != transaction_receipt_header::executed &&
                  trace->receipt->status != transaction_receipt_header::soft_fail) )
               return;
            for( const auto& atrace : trace->action_traces ) {
               if( !atrace.receipt ) continue;
               on_action_trace( atrace );
            }
         }
   };

   action_publisher_plugin::action_publisher_plugin() : my(std::make_shared<action_publisher_plugin_impl>()) {
      my->context = std::make_unique<zmq::context_t>(1);
      my->publisher = std::make_unique<zmq::socket_t>(*my->context, ZMQ_PUB);
      my->publisher->bind("tcp://*:5556");
      dlog("++++++++++hello,world");
   }

   action_publisher_plugin::~action_publisher_plugin() {
   }

   void action_publisher_plugin::set_program_options(options_description& cli, options_description& cfg) {
      dlog("++++++++++hello,world");
      cfg.add_options()
            ("filter-action-on,f", bpo::value<vector<string>>()->composing(),
             "Track actions which match receiver:action:actor. Actor may be blank to include all. Action and Actor both blank allows all from Recieiver. Receiver may not be blank.")
            ;
      cfg.add_options()
            ("filter-action-out,F", bpo::value<vector<string>>()->composing(),
             "Do not track actions which match receiver:action:actor. Action and Actor both blank excludes all from Reciever. Actor blank excludes all from reciever:action. Receiver may not be blank.")
            ;
   }

   void action_publisher_plugin::plugin_initialize(const variables_map& options) {
      try {
         if( options.count( "filter-action-on" )) {
            auto fo = options.at( "filter-action-on" ).as<vector<string>>();
            for( auto& s : fo ) {
               if( s == "*" || s == "\"*\"" ) {
                  my->bypass_filter = true;
                  wlog( "--filter-on * enabled. This can fill shared_mem, causing nodeos to stop." );
                  break;
               }
               std::vector<std::string> v;
               boost::split( v, s, boost::is_any_of( ":" ));
               EOS_ASSERT( v.size() == 3, fc::invalid_arg_exception, "Invalid value ${s} for --filter-on", ("s", s));
               filter_entry fe{v[0], v[1], v[2]};
               EOS_ASSERT( fe.receiver.value, fc::invalid_arg_exception,
                           "Invalid value ${s} for --filter-on", ("s", s));
               my->filter_on.insert( fe );
            }
         }
         if( options.count( "filter-action-out" )) {
            auto fo = options.at( "filter-action-out" ).as<vector<string>>();
            for( auto& s : fo ) {
               std::vector<std::string> v;
               boost::split( v, s, boost::is_any_of( ":" ));
               EOS_ASSERT( v.size() == 3, fc::invalid_arg_exception, "Invalid value ${s} for --filter-out", ("s", s));
               filter_entry fe{v[0], v[1], v[2]};
               EOS_ASSERT( fe.receiver.value, fc::invalid_arg_exception,
                           "Invalid value ${s} for --filter-out", ("s", s));
               my->filter_out.insert( fe );
            }
         }

         my->chain_plug = app().find_plugin<chain_plugin>();
         EOS_ASSERT( my->chain_plug, chain::missing_chain_plugin_exception, ""  );
         auto& chain = my->chain_plug->chain();

         my->applied_transaction_connection.emplace(
               chain.applied_transaction.connect( [&]( std::tuple<const transaction_trace_ptr&, const signed_transaction&> t ) {
                  my->on_applied_transaction( std::get<0>(t) );
               } ));
      } FC_LOG_AND_RETHROW()
   }

   bool action_publisher_plugin::set_filter_in(string& s) {
      if( s == "*" || s == "\"*\"" ) {
         my->bypass_filter = true;
         my->filter_on.clear();
         my->filter_out.clear();
         wlog( "--filter-on * enabled. This can fill shared_mem, causing nodeos to stop." );
         return true;
      }
      std::vector<std::string> v;
      boost::split( v, s, boost::is_any_of( ":" ));
      EOS_ASSERT( v.size() == 3, fc::invalid_arg_exception, "Invalid value ${s} for --filter-on", ("s", s));
      filter_entry fe{v[0], v[1], v[2]};
      EOS_ASSERT( fe.receiver.value, fc::invalid_arg_exception,
                  "Invalid value ${s} for --filter-on", ("s", s));
      my->filter_on.insert( fe );
      return true;
   }

   bool action_publisher_plugin::set_filter_out(string& s) {
      std::vector<std::string> v;
      boost::split( v, s, boost::is_any_of( ":" ));
      EOS_ASSERT( v.size() == 3, fc::invalid_arg_exception, "Invalid value ${s} for --filter-out", ("s", s));
      filter_entry fe{v[0], v[1], v[2]};
      EOS_ASSERT( fe.receiver.value, fc::invalid_arg_exception,
                  "Invalid value ${s} for --filter-out", ("s", s));
      my->filter_out.insert( fe );
      return true;
   }

#define CALL(api_name, api_handle, type, call_name) \
{std::string("/v1/" #api_name "/" #call_name), \
   [api_handle](string, string body, url_response_callback cb) mutable { \
          try { \
             if (body.empty()) body = "{}"; \
             auto params = fc::json::from_string(body).as<type>(); \
             fc::variant result( api_handle.call_name(params) ); \
             cb(200, std::move(result)); \
          } catch (...) { \
             http_plugin::handle_exception(#api_name, #call_name, body, cb); \
          } \
       }}

   void action_publisher_plugin::plugin_startup() {
      ilog( "starting action_publisher_plugin" );
      auto plugin = app().get_plugin<action_publisher_plugin>();
      app().get_plugin<http_plugin>().add_api({
         CALL(action_publisher, plugin, string, set_filter_in),
         CALL(action_publisher, plugin, string, set_filter_out),
      });
   }

   void action_publisher_plugin::plugin_shutdown() {
      my->applied_transaction_connection.reset();
   }

} /// namespace eosio
