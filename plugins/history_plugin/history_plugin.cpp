#include <eosio/history_plugin/history_plugin.hpp>
#include <eosio/history_plugin/account_control_history_object.hpp>
#include <eosio/history_plugin/public_key_history_object.hpp>
#include <eosio/chain/permission_object.hpp>

#include <eosio/chain/controller.hpp>
#include <eosio/chain/trace.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>

#include <fc/io/json.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/signals2/connection.hpp>
#include <db_interface.hpp>
#include <chain_api.hpp>

namespace eosio {
   using namespace chain;
   using boost::signals2::scoped_connection;

   static appbase::abstract_plugin& _history_plugin = app().register_plugin<history_plugin>();


   struct account_history_object : public chainbase::object<account_history_object_type, account_history_object>  {
      OBJECT_CTOR( account_history_object );

      id_type      id;
      account_name account; ///< the name of the account which has this action in its history
      uint64_t     action_sequence_num = 0; ///< the sequence number of the relevant action (global)
      int32_t      account_sequence_num = 0; ///< the sequence number for this account (per-account)
   };

   struct action_history_object : public chainbase::object<action_history_object_type, action_history_object> {

      OBJECT_CTOR( action_history_object, (packed_action_trace) );

      id_type      id;
      uint64_t     action_sequence_num; ///< the sequence number of the relevant action

      shared_string        packed_action_trace;
      uint32_t             block_num;
      block_timestamp_type block_time;
      transaction_id_type  trx_id;
   };
   using account_history_id_type = account_history_object::id_type;
   using action_history_id_type  = action_history_object::id_type;


   struct by_action_sequence_num;
   struct by_account_action_seq;
   struct by_trx_id;

   using action_history_index = chainbase::shared_multi_index_container<
      action_history_object,
      indexed_by<
         ordered_unique<tag<by_id>, member<action_history_object, action_history_object::id_type, &action_history_object::id>>,
         ordered_unique<tag<by_action_sequence_num>, member<action_history_object, uint64_t, &action_history_object::action_sequence_num>>,
         ordered_unique<tag<by_trx_id>,
            composite_key< action_history_object,
               member<action_history_object, transaction_id_type, &action_history_object::trx_id>,
               member<action_history_object, uint64_t, &action_history_object::action_sequence_num >
            >
         >
      >
   >;

   using account_history_index = chainbase::shared_multi_index_container<
      account_history_object,
      indexed_by<
         ordered_unique<tag<by_id>, member<account_history_object, account_history_object::id_type, &account_history_object::id>>,
         ordered_unique<tag<by_account_action_seq>,
            composite_key< account_history_object,
               member<account_history_object, account_name, &account_history_object::account >,
               member<account_history_object, int32_t, &account_history_object::account_sequence_num >
            >
         >
      >
   >;

} /// namespace eosio

CHAINBASE_SET_INDEX_TYPE(eosio::account_history_object, eosio::account_history_index)
CHAINBASE_SET_INDEX_TYPE(eosio::action_history_object, eosio::action_history_index)

namespace eosio {
   struct fake_chain_plugin {
      fake_chain_plugin(eosio::chain::controller& ctrl): ctrl(ctrl) {

      }
      
      eosio::chain::controller& chain()const {
         return ctrl;
      }

      fc::microseconds get_abi_serializer_max_time() const {
         return fc::microseconds(150000);
      }

      chainbase::database& db() const {
         return const_cast<chainbase::database&>(ctrl.db());
      }

      eosio::chain::controller& ctrl;
   };

   template<typename MultiIndex, typename LookupType>
   static void remove(chainbase::database& db, const account_name& account_name, const permission_name& permission)
   {
      const auto& idx = db.get_index<MultiIndex, LookupType>();
      auto& mutable_idx = db.get_mutable_index<MultiIndex>();
      while(!idx.empty()) {
         auto key = boost::make_tuple(account_name, permission);
         const auto& itr = idx.lower_bound(key);
         if (itr == idx.end())
            break;

         const auto& range_end = idx.upper_bound(key);
         if (itr == range_end)
            break;

         mutable_idx.remove(*itr);
      }
   }

   static void add(chainbase::database& db, const vector<key_weight>& keys, const account_name& name, const permission_name& permission)
   {
      for (auto pub_key_weight : keys ) {
         try {
            db.create<public_key_history_object>([&](public_key_history_object& obj) {
               obj.public_key = pub_key_weight.key;
               obj.name = name;
               obj.permission = permission;
            });
         } FC_LOG_AND_DROP((name))
      }
   }

   static void add(chainbase::database& db, const shared_vector<key_weight>& keys, const account_name& name, const permission_name& permission)
   {
      for (auto pub_key_weight : keys ) {
         try {
            db.create<public_key_history_object>([&](public_key_history_object& obj) {
               obj.public_key = pub_key_weight.key;
               obj.name = name;
               obj.permission = permission;
            });
         } FC_LOG_AND_DROP((name))
      }
   }

   static void add(chainbase::database& db, const shared_vector<shared_key_weight>& keys, const account_name& name, const permission_name& permission)
   {
      for (auto pub_key_weight : keys ) {
         try {
            db.create<public_key_history_object>([&](public_key_history_object& obj) {
               obj.public_key = pub_key_weight.key;
               obj.name = name;
               obj.permission = permission;
            });
         } FC_LOG_AND_DROP((name))
      }
   }

   static void add(chainbase::database& db, const vector<permission_level_weight>& controlling_accounts, const account_name& account_name, const permission_name& permission)
   {
      for (auto controlling_account : controlling_accounts ) {
         db.create<account_control_history_object>([&](account_control_history_object& obj) {
            obj.controlled_account = account_name;
            obj.controlled_permission = permission;
            obj.controlling_account = controlling_account.permission.actor;
         });
      }
   }

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

   class history_plugin_impl {
      public:
         bool bypass_filter = false;
         std::set<filter_entry> filter_on;
         std::set<filter_entry> filter_out;
         bool filter_transfer = false;

         // chain_plugin*          chain_plug = nullptr;
         fc::optional<fake_chain_plugin>          chain_plug;

         fc::optional<scoped_connection> applied_transaction_connection;
         map<chain::public_key_type, vector<chain::account_name>> key_accounts_map;

          bool filter(const action_trace& act) {
            bool pass_on = false;
            if (bypass_filter) {
              pass_on = true;
            }

            do {
               if (filter_transfer) {
                  if (act.act.account == N(eosio.token) && act.act.name == N(transfer)) {
                     pass_on = true;
                     break;
                  }
               }

               if (filter_on.find({ act.receiver, {}, {} }) != filter_on.end()) {
                  pass_on = true;
                  break;
               } else if (filter_on.find({ act.receiver, act.act.name, {} }) != filter_on.end()) {
                  pass_on = true;
                  break;
               }
               
               for (const auto& a : act.act.authorization) {
                  if (filter_on.find({ act.receiver, {}, a.actor }) != filter_on.end()) {
                     pass_on = true;
                     break;
                  }
                  if (filter_on.find({ act.receiver, act.act.name, a.actor }) != filter_on.end()) {
                     pass_on = true;
                     break;
                  }
               }
            } while (false);

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
                   filter_on.find({ act.receiver, {}, {}}) != filter_on.end() ||
                   filter_on.find({ act.receiver, {}, a.actor}) != filter_on.end() ||
                   filter_on.find({ act.receiver, act.act.name, {}}) != filter_on.end() ||
                   filter_on.find({ act.receiver, act.act.name, a.actor }) != filter_on.end() ) {
                 if ((filter_out.find({ act.receiver, {}, {} }) == filter_out.end()) &&
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
            auto& chain = chain_plug->chain();
            chainbase::database& db = const_cast<chainbase::database&>( chain.db() ); // Override read-only access to state DB (highly unrecommended practice!)

            const auto& idx = db.get_index<account_history_index, by_account_action_seq>();
            auto itr = idx.lower_bound( boost::make_tuple( name(n.to_uint64_t()+1), 0 ) );

            uint64_t asn = 0;
            if( itr != idx.begin() ) --itr;
            if( itr->account == n )
               asn = itr->account_sequence_num + 1;

            const auto& a = db.create<account_history_object>( [&]( auto& aho ) {
              aho.account = n;
              aho.action_sequence_num = act.receipt->global_sequence;
              aho.account_sequence_num = asn;
            });
         }

         void on_system_action( const action_trace& at ) {
            auto& chain = chain_plug->chain();
            chainbase::database& db = const_cast<chainbase::database&>( chain.db() ); // Override read-only access to state DB (highly unrecommended practice!)
            if( at.act.name == N(newaccount) )
            {
               const auto create = at.act.data_as<chain::newaccount>();
               add(db, create.owner.keys, create.name, N(owner));
               add(db, create.owner.accounts, create.name, N(owner));
               add(db, create.active.keys, create.name, N(active));
               add(db, create.active.accounts, create.name, N(active));
            }
            else if( at.act.name == N(updateauth) )
            {
               const auto update = at.act.data_as<chain::updateauth>();
               remove<public_key_history_multi_index, eosio::by_account_permission>(db, update.account, update.permission);
               remove<account_control_history_multi_index, by_controlled_authority>(db, update.account, update.permission);
               add(db, update.auth.keys, update.account, update.permission);
               add(db, update.auth.accounts, update.account, update.permission);
            }
            else if( at.act.name == N(deleteauth) )
            {
               const auto del = at.act.data_as<chain::deleteauth>();
               remove<public_key_history_multi_index, eosio::by_account_permission>(db, del.account, del.permission);
               remove<account_control_history_multi_index, by_controlled_authority>(db, del.account, del.permission);
            }
         }

         void on_action_trace( const action_trace& at ) {
            if( filter( at ) ) {
               //idump((fc::json::to_pretty_string(at)));
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

   history_plugin::history_plugin()
   :my(std::make_shared<history_plugin_impl>()) {
   }

   history_plugin::~history_plugin() {
   }



   void history_plugin::set_program_options(options_description& cli, options_description& cfg) {
      cfg.add_options()
            ("filter-on,f", bpo::value<vector<string>>()->composing(),
             "Track actions which match receiver:action:actor. Actor may be blank to include all. Action and Actor both blank allows all from Recieiver. Receiver may not be blank.")
            ;
      cfg.add_options()
            ("filter-out,F", bpo::value<vector<string>>()->composing(),
             "Do not track actions which match receiver:action:actor. Action and Actor both blank excludes all from Reciever. Actor blank excludes all from reciever:action. Receiver may not be blank.")
            ;
      cfg.add_options()
         ("filter-transfer", bpo::bool_switch()->default_value(false),
          "filter eosio.token transfer action \n")
          ;
   }

   void history_plugin::plugin_initialize(const variables_map& options) {
      try {
         if( options.count( "filter-on" )) {
            auto fo = options.at( "filter-on" ).as<vector<string>>();
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
         if( options.count( "filter-out" )) {
            auto fo = options.at( "filter-out" ).as<vector<string>>();
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

         my->filter_transfer = options.at( "filter-transfer" ).as<bool>();


//         my->chain_plug = app().find_plugin<chain_plugin>();
         my->chain_plug.emplace(app().find_plugin<chain_plugin>()->chain());

         EOS_ASSERT( my->chain_plug, chain::missing_chain_plugin_exception, ""  );
         auto& chain = my->chain_plug->chain();

         chainbase::database& db = const_cast<chainbase::database&>( chain.db() ); // Override read-only access to state DB (highly unrecommended practice!)
         // TODO: Use separate chainbase database for managing the state of the history_plugin (or remove deprecated history_plugin entirely)
         db.add_index<account_history_index>();
         db.add_index<action_history_index>();
         db.add_index<account_control_history_multi_index>();
         db.add_index<public_key_history_multi_index>();

         my->applied_transaction_connection.emplace(
               chain.applied_transaction.connect( [&]( std::tuple<const transaction_trace_ptr&, const signed_transaction&> t ) {
                  my->on_applied_transaction( std::get<0>(t) );
               } ));
      } FC_LOG_AND_RETHROW()
   }

   void history_plugin::plugin_initialize(chain::controller& chain, history_plugin_options& options)
    {
      try {

         if( options.filter_on.size()) {
            auto fo = options.filter_on;//options.at( "filter-on" ).as<vector<string>>();
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
               EOS_ASSERT( fe.receiver.value, fc::invalid_arg_exception,
                           "Invalid value ${s} for --filter-on", ("s", s));
               my->filter_on.insert( fe );
            }
         }
         if( options.filter_out.size()) {
            auto fo = options.filter_out;//options.at( "filter-out" ).as<vector<string>>();
            for( auto& s : fo ) {
               std::vector<std::string> v;
               boost::split( v, s, boost::is_any_of( ":" ));
               EOS_ASSERT( v.size() == 3, fc::invalid_arg_exception, "Invalid value ${s} for --filter-out", ("s", s));
               filter_entry fe{eosio::chain::name(v[0]), eosio::chain::name(v[1]), eosio::chain::name(v[2])};
               EOS_ASSERT( fe.receiver.value, fc::invalid_arg_exception,
                           "Invalid value ${s} for --filter-out", ("s", s));
               my->filter_out.insert( fe );
            }
         }

         my->filter_transfer = options.filter_transfer;


         my->chain_plug.emplace(chain);
         EOS_ASSERT( my->chain_plug, chain::missing_chain_plugin_exception, ""  );
         auto& chain = my->chain_plug->chain();

//         chainbase::database& db = const_cast<chainbase::database&>( chain.db() ); // Override read-only access to state DB (highly unrecommended practice!)
         chainbase::database& db = my->chain_plug->db();
         // TODO: Use separate chainbase database for managing the state of the history_plugin (or remove deprecated history_plugin entirely)
         db.add_index<account_history_index>();
         db.add_index<action_history_index>();
         db.add_index<account_control_history_multi_index>();
         db.add_index<public_key_history_multi_index>();

         my->applied_transaction_connection.emplace(
               chain.applied_transaction.connect( [&]( std::tuple<const transaction_trace_ptr&, const signed_transaction&> t ) {
                  my->on_applied_transaction( std::get<0>(t) );
               } ));
      } FC_LOG_AND_RETHROW()
   }

   void history_plugin::plugin_startup() {
      if (my->chain_plug->chain().is_uuos_mainnet()) {
         my->chain_plug->chain().get_db_interface().init_key_accounts();
      }
//      auto& db = app().get_plugin<chain_plugin>().chain().db();
//      auto& db = my->chain_plug->chain().db();
      chainbase::database& db = const_cast<chainbase::database&>( my->chain_plug->chain().db() ); // Override read-only access to state DB (highly unrecommended practice!)
      {
         const auto& pub_key_idx = db.get_index<public_key_history_multi_index, by_pub_key>();
         auto itr = pub_key_idx.upper_bound(public_key_type());
         if (itr != pub_key_idx.end()) {
            //already initialized
            return;
         }
      }
      const auto& accounts = db.get_index<account_metadata_index, by_name>();
      auto itr = accounts.upper_bound(account_name(0));
      int counter = 0;
      while (itr != accounts.end()) {
         const auto& permissions = db.get_index<permission_index,by_owner>();
         auto perm = permissions.lower_bound( boost::make_tuple( itr->name ) );
         while( perm != permissions.end() && perm->owner == itr->name ) {
            if (perm->auth.keys.size() != 0) {
               add(db, perm->auth.keys, itr->name, perm->name);
            }
            ++perm;
         }
         counter += 1;
         if (counter % 100000 == 0) {
            ilog("++++++++init history_plugin: ${n}", ("n",counter));
         }
         itr++;
      }
      ilog("++++++++init history_plugin: ${n}", ("n",counter));
      ilog("++++++++++++++++history_plugin initialized");
   }

   void history_plugin::plugin_shutdown() {
      my->applied_transaction_connection.reset();
   }




   namespace history_apis {
      read_only::get_actions_result read_only::get_actions( const read_only::get_actions_params& params )const {
         edump((params));
        auto& chain = history->chain_plug->chain();
        const auto& db = chain.db();
        const auto abi_serializer_max_time = history->chain_plug->get_abi_serializer_max_time();

        const auto& idx = db.get_index<account_history_index, by_account_action_seq>();

        int32_t start = 0;
        int32_t pos = params.pos ? *params.pos : -1;
        int32_t end = 0;
        int32_t offset = params.offset ? *params.offset : -20;
        auto n = params.account_name;
        idump((pos));
        if( pos == -1 ) {
            auto itr = idx.lower_bound( boost::make_tuple( name(n.to_uint64_t()+1), 0 ) );
            if( itr == idx.begin() ) {
               if( itr->account == n )
                  pos = itr->account_sequence_num+1;
            } else if( itr != idx.begin() ) --itr;

            if( itr->account == n )
               pos = itr->account_sequence_num + 1;
        }

        if( pos== -1 ) pos = 0xfffffff;

        if( offset > 0 ) {
           start = pos;
           end   = start + offset;
        } else {
           start = pos + offset;
           if( start > pos ) start = 0;
           end   = pos;
        }
        EOS_ASSERT( end >= start, chain::plugin_exception, "end position is earlier than start position" );

        idump((start)(end));

        auto start_itr = idx.lower_bound( boost::make_tuple( n, start ) );
        auto end_itr = idx.upper_bound( boost::make_tuple( n, end) );

        auto start_time = fc::time_point::now();
        auto end_time = start_time;

        get_actions_result result;
        result.last_irreversible_block = chain.last_irreversible_block_num();
        while( start_itr != end_itr ) {
           const auto& a = db.get<action_history_object, by_action_sequence_num>( start_itr->action_sequence_num );
           fc::datastream<const char*> ds( a.packed_action_trace.data(), a.packed_action_trace.size() );
           action_trace t;
           fc::raw::unpack( ds, t );
           result.actions.emplace_back( ordered_action_result{
                                 start_itr->action_sequence_num,
                                 start_itr->account_sequence_num,
                                 a.block_num, a.block_time,
                                 chain.to_variant_with_abi(t, abi_serializer::create_yield_function( abi_serializer_max_time ))
                                 });

           end_time = fc::time_point::now();
           if( end_time - start_time > fc::microseconds(100000) ) {
              result.time_limit_exceeded_error = true;
              break;
           }
           ++start_itr;
        }
        return result;
      }


      read_only::get_transaction_result read_only::get_transaction( const read_only::get_transaction_params& p )const {
         auto& chain = history->chain_plug->chain();
         const auto abi_serializer_max_time = history->chain_plug->get_abi_serializer_max_time();

         transaction_id_type input_id;
         auto input_id_length = p.id.size();
         try {
            FC_ASSERT( input_id_length <= 64, "hex string is too long to represent an actual transaction id" );
            FC_ASSERT( input_id_length >= 8,  "hex string representing transaction id should be at least 8 characters long to avoid excessive collisions" );
            input_id = transaction_id_type(p.id);
         } EOS_RETHROW_EXCEPTIONS(transaction_id_type_exception, "Invalid transaction ID: ${transaction_id}", ("transaction_id", p.id))

         auto txn_id_matched = [&input_id, input_id_size = input_id_length/2, no_half_byte_at_end = (input_id_length % 2 == 0)]
                               ( const transaction_id_type &id ) -> bool // hex prefix comparison
         {
            bool whole_byte_prefix_matches = memcmp( input_id.data(), id.data(), input_id_size ) == 0;
            if( !whole_byte_prefix_matches || no_half_byte_at_end )
               return whole_byte_prefix_matches;

            // check if half byte at end of specified part of input_id matches
            return (*(input_id.data() + input_id_size) & 0xF0) == (*(id.data() + input_id_size) & 0xF0);
         };

         const auto& db = chain.db();
         const auto& idx = db.get_index<action_history_index, by_trx_id>();
         auto itr = idx.lower_bound( boost::make_tuple( input_id ) );

         bool in_history = (itr != idx.end() && txn_id_matched(itr->trx_id) );

         if( !in_history && !p.block_num_hint ) {
            EOS_THROW(tx_not_found, "Transaction ${id} not found in history and no block hint was given", ("id",p.id));
         }

         get_transaction_result result;

         if( in_history ) {
            result.id         = itr->trx_id;
            result.last_irreversible_block = chain.last_irreversible_block_num();
            result.block_num  = itr->block_num;
            result.block_time = itr->block_time;

            while( itr != idx.end() && itr->trx_id == result.id ) {

              fc::datastream<const char*> ds( itr->packed_action_trace.data(), itr->packed_action_trace.size() );
              action_trace t;
              fc::raw::unpack( ds, t );
              result.traces.emplace_back( chain.to_variant_with_abi(t, abi_serializer::create_yield_function( abi_serializer_max_time )) );

              ++itr;
            }

            auto blk = chain.fetch_block_by_number( result.block_num );
            if( blk || chain.is_building_block() ) {
               const vector<transaction_receipt>& receipts = blk ? blk->transactions : chain.get_pending_trx_receipts();
               for (const auto &receipt: receipts) {
                    if (receipt.trx.contains<packed_transaction>()) {
                        auto &pt = receipt.trx.get<packed_transaction>();
                        if (pt.id() == result.id) {
                            fc::mutable_variant_object r("receipt", receipt);
                            r("trx", chain.to_variant_with_abi(pt.get_signed_transaction(), abi_serializer::create_yield_function( abi_serializer_max_time )));
                            result.trx = move(r);
                            break;
                        }
                    } else {
                        auto &id = receipt.trx.get<transaction_id_type>();
                        if (id == result.id) {
                            fc::mutable_variant_object r("receipt", receipt);
                            result.trx = move(r);
                            break;
                        }
                    }
               }
            }
         } else {
            auto blk = chain.fetch_block_by_number(*p.block_num_hint);
            bool found = false;
            if (blk) {
               for (const auto& receipt: blk->transactions) {
                  if (receipt.trx.contains<packed_transaction>()) {
                     auto& pt = receipt.trx.get<packed_transaction>();
                     const auto& id = pt.id();
                     if( txn_id_matched(id) ) {
                        result.id = id;
                        result.last_irreversible_block = chain.last_irreversible_block_num();
                        result.block_num = *p.block_num_hint;
                        result.block_time = blk->timestamp;
                        fc::mutable_variant_object r("receipt", receipt);
                        r("trx", chain.to_variant_with_abi(pt.get_signed_transaction(), abi_serializer::create_yield_function( abi_serializer_max_time )));
                        result.trx = move(r);
                        found = true;
                        break;
                     }
                  } else {
                     auto& id = receipt.trx.get<transaction_id_type>();
                     if( txn_id_matched(id) ) {
                        result.id = id;
                        result.last_irreversible_block = chain.last_irreversible_block_num();
                        result.block_num = *p.block_num_hint;
                        result.block_time = blk->timestamp;
                        fc::mutable_variant_object r("receipt", receipt);
                        result.trx = move(r);
                        found = true;
                        break;
                     }
                  }
               }
            }

            if (!found) {
               EOS_THROW(tx_not_found, "Transaction ${id} not found in history or in block number ${n}", ("id",p.id)("n", *p.block_num_hint));
            }
         }

         return result;
      }

      read_only::get_key_accounts_results read_only::get_key_accounts(const get_key_accounts_params& params) const {
         std::set<account_name> accounts;
         const auto& db = history->chain_plug->chain().db();
         const auto& pub_key_idx = db.get_index<public_key_history_multi_index, by_pub_key>();
         auto range = pub_key_idx.equal_range( params.public_key );
         for (auto obj = range.first; obj != range.second; ++obj)
            accounts.insert(obj->name);
         return {vector<account_name>(accounts.begin(), accounts.end())};
      }

      read_only::get_key_accounts_ex_results read_only::get_key_accounts_ex(const get_key_accounts_ex_params& params) const {
         std::vector<account_name> accounts;
         std::vector<int> active_flags;
         const auto& db = history->chain_plug->chain().db();
         const auto& pub_key_idx = db.get_index<public_key_history_multi_index, by_pub_key>();
         auto range = pub_key_idx.equal_range( params.public_key );
         int count = 0;
         for (auto obj = range.first; obj != range.second; ++obj) {
            if (std::find(accounts.begin(), accounts.end(), obj->name) == accounts.end()) {
               accounts.push_back(obj->name);
               active_flags.push_back(1);
               count += 1;
               if (count > 100) {
                  break;
               }
            }
         }
         auto& dbif = history->chain_plug->chain().get_db_interface();
         vector<account_name> v = dbif.get_genesis_accounts(params.public_key);
         for (auto& a : v) {
            if (std::find(accounts.begin(), accounts.end(), a) == accounts.end()) {
               accounts.push_back(a);
               if (dbif.is_account(a)) {
                  active_flags.push_back(1);
               } else {
                  active_flags.push_back(0);
               }
            }
            count += 1;
            if (count > 100) {
               break;
            }
         }
         
         return {vector<account_name>(accounts.begin(), accounts.end()), vector<int>(active_flags.begin(), active_flags.end())};
      }

      read_only::get_controlled_accounts_results read_only::get_controlled_accounts(const get_controlled_accounts_params& params) const {
         std::set<account_name> accounts;
         const auto& db = history->chain_plug->chain().db();
         const auto& account_control_idx = db.get_index<account_control_history_multi_index, by_controlling>();
         auto range = account_control_idx.equal_range( params.controlling_account );
         for (auto obj = range.first; obj != range.second; ++obj)
            accounts.insert(obj->controlled_account);
         return {vector<account_name>(accounts.begin(), accounts.end())};
      }

   } /// history_apis



} /// namespace eosio

using namespace eosio;

#define CATCH_AND_LOG_EXCEPTION()\
   catch ( const fc::exception& e ) {\
      string ex = fc::json::to_string(*e.dynamic_copy_exception(), fc::time_point::maximum()); \
      get_chain_api()->uuos_on_error(ex);\
   } catch ( const std::exception& e ) {\
      fc::exception fce( \
         FC_LOG_MESSAGE( warn, "rethrow ${what}: ", ("what",e.what())),\
         fc::std_exception_code,\
         BOOST_CORE_TYPEID(e).name(),\
         e.what() ) ;\
        string ex = fc::json::to_string(*fce.dynamic_copy_exception(), fc::time_point::maximum()); \
        get_chain_api()->uuos_on_error(ex);\
   } catch( ... ) {\
      fc::unhandled_exception e(\
         FC_LOG_MESSAGE(warn, "rethrow"),\
         std::current_exception());\
        string ex = fc::json::to_string(*e.dynamic_copy_exception(), fc::time_point::maximum()); \
        get_chain_api()->uuos_on_error(ex);\
   }

void* history_new_(void *ptr, string& cfg) {
   history_plugin* history = nullptr;
   try {
      auto _cfg = fc::json::from_string(cfg).as<eosio::history_plugin_options>();
      history = new history_plugin();
      auto& chain = *((eosio::chain::controller*)ptr);
      history->plugin_initialize(chain, _cfg);
      history->plugin_startup();
      return (void *)history;
   } catch ( boost::interprocess::bad_alloc& ) {
      elog("bad_alloc");
   } CATCH_AND_LOG_EXCEPTION();
   if (history) {
      delete history;
   }
   return (void *)0;
}

void history_free_(void *ptr) {
   if (ptr) {
      delete (history_plugin*)ptr;
   }
}

void history_get_actions_(void *ptr, const string& param, string& result) {
   try {
      auto history = (eosio::history_plugin*)ptr;
      auto _param = fc::json::from_string(param).as<eosio::history_apis::read_only::get_actions_params>();
      result = fc::json::to_string(history->get_read_only_api().get_actions(_param), fc::time_point::maximum());
   } CATCH_AND_LOG_EXCEPTION()
}

void history_get_transaction_(void *ptr, const string& param, string& result) {
   try {
      auto history = (eosio::history_plugin*)ptr;
      auto _param = fc::json::from_string(param).as<eosio::history_apis::read_only::get_transaction_params>();
      result = fc::json::to_string(history->get_read_only_api().get_transaction(_param), fc::time_point::maximum());
   } CATCH_AND_LOG_EXCEPTION()
}

void history_get_key_accounts_(void *ptr, const string& param, string& result) {
   try {
      auto history = (eosio::history_plugin*)ptr;
      auto _param = fc::json::from_string(param).as<eosio::history_apis::read_only::get_key_accounts_params>();
      result = fc::json::to_string(history->get_read_only_api().get_key_accounts(_param), fc::time_point::maximum());
   } CATCH_AND_LOG_EXCEPTION()
}

void history_get_key_accounts_ex_(void *ptr, const string& param, string& result) {
   try {
      auto history = (eosio::history_plugin*)ptr;
      auto _param = fc::json::from_string(param).as<eosio::history_apis::read_only::get_key_accounts_ex_params>();
      result = fc::json::to_string(history->get_read_only_api().get_key_accounts_ex(_param), fc::time_point::maximum());
   } CATCH_AND_LOG_EXCEPTION()
}

void history_get_controlled_accounts_(void *ptr, const string& param, string& result) {
   try {
      auto history = (eosio::history_plugin*)ptr;
      auto _param = fc::json::from_string(param).as<eosio::history_apis::read_only::get_controlled_accounts_params>();
      result = fc::json::to_string(history->get_read_only_api().get_controlled_accounts(_param), fc::time_point::maximum());
   } CATCH_AND_LOG_EXCEPTION()
}

extern "C" void init_history_callback() {
   auto* api = get_chain_api();
   api->history_new = history_new_;
   api->history_free = history_free_;
   api->history_get_actions = history_get_actions_;
   api->history_get_transaction = history_get_transaction_;
   api->history_get_key_accounts = history_get_key_accounts_;
   api->history_get_key_accounts_ex = history_get_key_accounts_ex_;
   api->history_get_controlled_accounts = history_get_controlled_accounts_;
}
