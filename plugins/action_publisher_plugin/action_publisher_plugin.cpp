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

         unique_ptr<zmq::context_t> context_tcp;
         unique_ptr<zmq::socket_t> publisher_tcp;
         unique_ptr<zmq::context_t> context_ipc;
         unique_ptr<zmq::socket_t> publisher_ipc;

          bool filter(const action_trace& act) {
            bool pass_on = false;
            if (bypass_filter) {
              pass_on = true;
            }
            if (filter_on.find({ act.receiver, {}, {} }) != filter_on.end()) {
              pass_on = true;
            }
            if (filter_on.find({ act.receiver, act.act.name, {} }) != filter_on.end()) {
              pass_on = true;
            }
            for (const auto& a : act.act.authorization) {
              if (filter_on.find({ act.receiver, {}, a.actor }) != filter_on.end()) {
                pass_on = true;
              }
              if (filter_on.find({ act.receiver, act.act.name, a.actor }) != filter_on.end()) {
                pass_on = true;
              }
            }

            if (!pass_on) {  return false;  }

            if (filter_out.find({ act.receiver, {}, {} }) != filter_out.end()) {
              return false;
            }
            if (filter_out.find({ act.receiver, act.act.name, {} }) != filter_out.end()) {
              return false;
            }
            for (const auto& a : act.act.authorization) {
              if (filter_out.find({ act.receiver, {}, a.actor }) != filter_out.end()) {
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
                   filter_on.find({ act.receiver, {},{}}) != filter_on.end() ||
                   filter_on.find({ act.receiver, {}, a.actor}) != filter_on.end() ||
                   filter_on.find({ act.receiver, act.act.name, {}}) != filter_on.end() ||
                   filter_on.find({ act.receiver, act.act.name, a.actor }) != filter_on.end() ) {
                 if ((filter_out.find({ act.receiver, {},{} }) == filter_out.end()) &&
                     (filter_out.find({ act.receiver, {}, a.actor }) == filter_out.end()) &&
                     (filter_out.find({ act.receiver, act.act.name, {} }) == filter_out.end()) &&
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
               string str_action = fc::json::to_string(at, fc::time_point::maximum());
               // string receiver = at.receiver.to_string();
               string receiver = at.receipt->receiver.to_string();

               s_sendmore (*publisher_tcp, receiver);
               s_send (*publisher_tcp, str_action);

               s_sendmore (*publisher_ipc, receiver);
               s_send (*publisher_ipc, str_action);

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
            } else {
            }
            if( at.receiver == chain::config::system_account_name )
               on_system_action( at );
         }

         void on_applied_transaction( const transaction_trace_ptr& trace ) {
            if( !trace->receipt || trace->receipt->status != transaction_receipt_header::executed )
               return;
            for( const auto& atrace : trace->action_traces ) {
               if( !atrace.receipt ) continue;
               on_action_trace( atrace );
            }
         }
   };

   action_publisher_plugin::action_publisher_plugin() : my(std::make_shared<action_publisher_plugin_impl>()) {
   }

   action_publisher_plugin::~action_publisher_plugin() {
   }

   void action_publisher_plugin::set_program_options(options_description& cli, options_description& cfg) {
      cfg.add_options()
            ("filter-action-on", bpo::value<vector<string>>()->composing(),
             "Track actions which match receiver:action:actor. Actor may be blank to include all. Action and Actor both blank allows all from Recieiver. Receiver may not be blank.")
            ;
      cfg.add_options()
            ("filter-action-out", bpo::value<vector<string>>()->composing(),
             "Do not track actions which match receiver:action:actor. Action and Actor both blank excludes all from Reciever. Actor blank excludes all from reciever:action. Receiver may not be blank.")
            ;
      cfg.add_options()
            ("zmq-ipc-address", bpo::value<string>(),
             "zmq ipc address")
            ;
      cfg.add_options()
            ("zmq-tcp-address", bpo::value<string>(),
             "zmq tcp address")
            ;
   }

   void action_publisher_plugin::plugin_initialize(const variables_map& options) {
      try {
         my->context_tcp = std::make_unique<zmq::context_t>(1);
         my->publisher_tcp = std::make_unique<zmq::socket_t>(*my->context_tcp, ZMQ_PUB);

         my->context_ipc = std::make_unique<zmq::context_t>(1);
         my->publisher_ipc = std::make_unique<zmq::socket_t>(*my->context_ipc, ZMQ_PUB);

         if( options.count( "zmq-ipc-address" )) {
            auto ipc = options.at( "zmq-ipc-address" ).as<string>();
            my->publisher_ipc->bind(ipc);
         } else {
            my->publisher_ipc->bind("ipc:///tmp/0");
         }

         if( options.count( "zmq-tcp-address" )) {
            auto tcp = options.at( "zmq-tcp-address" ).as<string>();
            dlog("++++++++++++zmq-tcp-address is ${n}", ("n", tcp));
            my->publisher_tcp->bind(tcp);
         } else {
            dlog("++++++++++++set default zmq-tcp-address tcp://*:5556");
            my->publisher_tcp->bind("tcp://*:5556");
         }

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
               filter_entry fe{eosio::chain::name(v[0]), eosio::chain::name(v[1]), eosio::chain::name(v[2])};
               EOS_ASSERT( fe.receiver.to_uint64_t(), fc::invalid_arg_exception,
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
               filter_entry fe{eosio::chain::name(v[0]), eosio::chain::name(v[1]), eosio::chain::name(v[2])};
               EOS_ASSERT( fe.receiver.to_uint64_t(), fc::invalid_arg_exception,
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

   bool action_publisher_plugin::set_filter_on(string& s) {
      if( s == "*" || s == "\"*\"" ) {
         my->bypass_filter = true;
         my->filter_on.clear();
         wlog( "filter-on * enabled." );
         return true;
      }
      std::vector<std::string> v;
      boost::split( v, s, boost::is_any_of( ":" ));
      EOS_ASSERT( v.size() == 3, fc::invalid_arg_exception, "Invalid value ${s} for --filter-on", ("s", s));
      filter_entry fe{eosio::chain::name(v[0]), eosio::chain::name(v[1]), eosio::chain::name(v[2])};
      EOS_ASSERT( fe.receiver.to_uint64_t(), fc::invalid_arg_exception,
                  "Invalid value ${s} for --filter-on", ("s", s));
      my->filter_on.insert( fe );
      return true;
   }

   bool action_publisher_plugin::set_filter_out(string& s) {
      if (s == "" || s == "\"\"") {
         my->filter_out.clear();
         wlog( "filter-out disabled." );
         return true;
      }
      std::vector<std::string> v;
      boost::split( v, s, boost::is_any_of( ":" ));
      EOS_ASSERT( v.size() == 3, fc::invalid_arg_exception, "Invalid value ${s} for --filter-out", ("s", s));
      filter_entry fe{eosio::chain::name(v[0]), eosio::chain::name(v[1]), eosio::chain::name(v[2])};
      EOS_ASSERT( fe.receiver.to_uint64_t(), fc::invalid_arg_exception,
                  "Invalid value ${s} for --filter-out", ("s", s));
      my->filter_out.insert( fe );
      return true;
   }

   bool action_publisher_plugin::clear_filter_on() {
      my->bypass_filter = false;
      my->filter_on.clear();
      wlog( "filter-on disabled." );
      return true;
   }

   bool action_publisher_plugin::clear_filter_out() {
      my->filter_out.clear();
      wlog( "filter-out disabled." );
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

#define CALL_V_R(api_name, api_handle, type, call_name) \
{std::string("/v1/" #api_name "/" #call_name), \
   [api_handle](string, string body, url_response_callback cb) mutable { \
          try { \
             fc::variant result( api_handle.call_name() ); \
             cb(200, std::move(result)); \
          } catch (...) { \
             http_plugin::handle_exception(#api_name, #call_name, body, cb); \
          } \
       }}

   void action_publisher_plugin::plugin_startup() {
      ilog( "starting action_publisher_plugin" );
      auto plugin = app().get_plugin<action_publisher_plugin>();
      app().get_plugin<http_plugin>().add_api({
         CALL(action_publisher, plugin, string, set_filter_on),
         CALL(action_publisher, plugin, string, set_filter_out),
         CALL_V_R(action_publisher, plugin, string, clear_filter_on),
         CALL_V_R(action_publisher, plugin, string, clear_filter_out),
      });
   }

   void action_publisher_plugin::plugin_shutdown() {
      my->publisher_tcp->close();
      my->publisher_ipc->close();
      my->applied_transaction_connection.reset();
   }

} /// namespace eosio
