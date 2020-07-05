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
#include <eosio/chain/resource_limits.hpp>
#include <eosio/chain/resource_limits_private.hpp>

#include <boost/algorithm/string.hpp>

#include "db_interface.hpp"
#include <chain_api.hpp>
#include <stacktrace.h>
#include "raw_ex.hpp"

using boost::container::flat_set;
using namespace fc;

extern "C" void execution_start();
extern "C" void execution_end();

#include "../../../unittests/incbin.h"
INCBIN(Accounts, "genesis_accounts.bin");

/*
 * // const unsigned char gAccountsData[];
 * // const unsigned char *const <prefix>FooEnd;
 * // const unsigned int gAccountsSize;
*/

namespace eosio { namespace chain {

#include <vm_api/vm_api.h>

db_interface::db_interface(chainbase::database& _db):
db(_db),
act(action()),
idx64(*this),
idx128(*this),
idx256(*this),
idx_double(*this),
idx_long_double(*this) {
}

void db_interface::init_key_accounts() {
   if (key_accounts_map.size() > 0) {
      return;
   }
   int itr = db_upperbound_i64(N(eosio).to_uint64_t(), N(eosio).to_uint64_t(), N(gaccounts).to_uint64_t(), 0);
   if (itr >= 0) {
      int i = 0;
      while (itr >= 0) {
         uint64_t primary = 0;
         vector<char> key_buffer(34);
         db_get_i64_ex(itr, primary, key_buffer.data(), 34);
         auto pub_key = fc::raw::unpack<public_key_type>(key_buffer);
         auto map_itr = key_accounts_map.find(pub_key);
         if (map_itr == key_accounts_map.end()) {
            key_accounts_map[pub_key] = {account_name(primary)};
         } else {
            key_accounts_map[pub_key].emplace_back(account_name(primary));
         }
         itr = db_next_i64(itr, primary);
         i += 1;
      };
   } else {
      for (int i=0;i<gAccountsSize;i+=42) {
         uint64_t account;
         memcpy(&account, &gAccountsData[i], 8);
         account_name name(account);
         vector<char> key((char *)&gAccountsData[i+8], (char *)&gAccountsData[i+42]);
         auto pub_key = fc::raw::unpack<chain::public_key_type>(key);
         auto itr = key_accounts_map.find(pub_key);
         if (itr == key_accounts_map.end()) {
            key_accounts_map[pub_key] = {name};
         } else {
            key_accounts_map[pub_key].emplace_back(name);
         }
      }
   }
   vmilog("+++++++++++init key accounts done!\n");
}

extern "C" {
   void wasm_interface_init_1(int vm_type);
   bool wasm_interface_apply_1(const eosio::chain::digest_type& code_id, const uint8_t& vm_type, const uint8_t& vm_version);
}

string db_interface::exec_action(uint64_t code, uint64_t action, const vector<char>& args) {
   static bool init = false;
   if (!init) {
      init = true;
//      wasm_interface_init_1(0);
   }
   call_start = fc::time_point::now();
   _pending_console_output.clear();

   receiver = name(code);

   act.account = name(code);
   act.name = name(action);
   act.data = args;
   const auto& account = db.get<account_metadata_object,by_name>(act.account);
   bool existing_code = (account.code_hash != digest_type());
//   wasm_interface_apply_1(account.code_hash, account.vm_type, account.vm_version);
   return _pending_console_output;
}

void db_interface::get_code( uint64_t account, string& code ) {
   const auto& accnt_obj          = db.get<account_object,by_name>( name(account) );
   const auto& accnt_metadata_obj = db.get<account_metadata_object,by_name>( name(account) );

   if( accnt_metadata_obj.code_hash != digest_type() ) {
      const auto& code_obj = db.get<code_object, by_code_hash>(accnt_metadata_obj.code_hash);
      code = string(code_obj.code.begin(), code_obj.code.end());
   }
}

const char* db_interface::get_code_ex(uint64_t receiver, size_t* size ) {

   if (!is_account(account_name(receiver))) {
      *size = 0;
      return nullptr;
   }
   try {
      const auto& account = db.get<account_metadata_object,by_name>(name(receiver));
      bool existing_code = (account.code_hash != digest_type());
      if( existing_code ) {
         const code_object& code_entry = db.get<code_object, by_code_hash>(boost::make_tuple(account.code_hash, account.vm_type, account.vm_version));
         *size = code_entry.code.size();
         return code_entry.code.data();
      }
   } catch (...) {
   }
   return nullptr;
}

#if 0

const shared_string& db_interface::get_code(uint64_t account) {
   const auto &a = db.get<account_object, by_name>(account);
   return a.code;
}

int db_interface::get_code_type(uint64_t account) {
   const auto &a = db.get<account_object, by_name>(account);
   return a.vm_type;
}

digest_type db_interface::get_code_id(uint64_t account) {
   const auto &a = db.get<account_object, by_name>(account);
   return a.code_version;
}
#endif

bool db_interface::is_account(const account_name& account)const {
   return nullptr != db.find<account_object,by_name>( account );
}

void db_interface::require_authorization( const account_name& account ) {
   for( uint32_t i=0; i < act.authorization.size(); i++ ) {
     if( act.authorization[i].actor == account ) {
        used_authorizations[i] = true;
        return;
     }
   }
   EOS_ASSERT( false, missing_auth_exception, "missing authority of ${account}", ("account",account));
}

bool db_interface::has_authorization( const account_name& account )const {
   for( const auto& auth : act.authorization )
     if( auth.actor == account )
        return true;
  return false;
}

void db_interface::require_authorization(const account_name& account,
                                          const permission_name& permission) {
  for( uint32_t i=0; i < act.authorization.size(); i++ )
     if( act.authorization[i].actor == account ) {
        if( act.authorization[i].permission == permission ) {
           used_authorizations[i] = true;
           return;
        }
     }
  EOS_ASSERT( false, missing_auth_exception, "missing authority of ${account}/${permission}",
              ("account",account)("permission",permission) );
}

const table_id_object& db_interface::find_or_create_table( name code, name scope, name table, const account_name &payer ) {
//   require_read_lock(code, scope);
   const auto* existing_tid =  db.find<table_id_object, by_code_scope_table>(boost::make_tuple(code, scope, table));
   if (existing_tid != nullptr) {
      return *existing_tid;
   }

//   require_write_lock(scope);

   update_db_usage(payer, config::billable_size_v<table_id_object>);

   return db.create<table_id_object>([&](table_id_object &t_id){
      t_id.code = code;
      t_id.scope = scope;
      t_id.table = table;
      t_id.payer = payer;
   });
}

int db_interface::db_store_i64( uint64_t code, uint64_t scope, uint64_t table, const account_name& payer, uint64_t id, const char* buffer, size_t buffer_size ) {
//   require_write_lock( scope );
   const auto& tab = find_or_create_table( name(code), name(scope), name(table), name(payer) );
   auto tableid = tab.id;

   FC_ASSERT( payer != account_name(), "must specify a valid account to pay for new record" );

   const auto& obj = db.create<key_value_object>( [&]( auto& o ) {
      o.t_id        = tableid;
      o.primary_key = id;
      o.value.resize( buffer_size );
      o.payer       = payer;
      memcpy( o.value.data(), buffer, buffer_size );
   });

   db.modify( tab, [&]( auto& t ) {
     ++t.count;
   });

   int64_t billable_size = (int64_t)(buffer_size + config::billable_size_v<key_value_object>);
   update_db_usage( payer, billable_size);

   keyval_cache.cache_table( tab );
   return keyval_cache.add( obj );
}

int db_interface::db_store_i64(  uint64_t scope, uint64_t table, const account_name& payer, uint64_t id, const char* buffer, size_t buffer_size ) {
   return db_store_i64(get_receiver().to_uint64_t(),  scope, table, payer, id, buffer, buffer_size );
}

void db_interface::db_update_i64( int iterator, account_name payer, const char* buffer, size_t buffer_size ) {
   const key_value_object& obj = keyval_cache.get( iterator );

   const auto& table_obj = keyval_cache.get_table( obj.t_id );
   // if (table_obj.code != receiver) {
   //    print_stacktrace();
   // }
   FC_ASSERT( table_obj.code == receiver, "db access violation" );

//   require_write_lock( table_obj.scope );

   const int64_t overhead = config::billable_size_v<key_value_object>;
   int64_t old_size = (int64_t)(obj.value.size() + overhead);
   int64_t new_size = (int64_t)(buffer_size + overhead);

   if( payer == account_name() ) payer = obj.payer;

   if( account_name(obj.payer) != payer ) {
      // refund the existing payer
      update_db_usage( obj.payer,  -(old_size) );
      // charge the new payer
      update_db_usage( payer,  (new_size));
   } else if(old_size != new_size) {
      // charge/refund the existing payer the difference
      update_db_usage( obj.payer, new_size - old_size);
   }

   db.modify( obj, [&]( auto& o ) {
     o.value.resize( buffer_size );
     memcpy( o.value.data(), buffer, buffer_size );
     o.payer = payer;
   });
}

void db_interface::db_remove_i64( int iterator ) {
   const key_value_object& obj = keyval_cache.get( iterator );

   const auto& table_obj = keyval_cache.get_table( obj.t_id );
   FC_ASSERT( table_obj.code == receiver, "db access violation" );

//   require_write_lock( table_obj.scope );

   update_db_usage( obj.payer,  -(obj.value.size() + config::billable_size_v<key_value_object>) );

   db.modify( table_obj, [&]( auto& t ) {
      --t.count;
   });
   db.remove( obj );

   if (table_obj.count == 0) {
      remove_table(table_obj);
   }

   keyval_cache.remove( iterator );
}

void db_interface::remove_table( const table_id_object& tid ) {
   update_db_usage(tid.payer, - config::billable_size_v<table_id_object>);
   db.remove(tid);
}

void db_interface::update_db_usage( const account_name& payer, int64_t ram_delta ) {
   if (ram_delta == 0) {
      return;
   }

   const auto& usage  = db.get<resource_limits::resource_usage_object, resource_limits::by_owner>( payer );
//   elog("+++++++${n1}, ${n2} ${n3}", ("n1", ram_delta)("n2", usage.ram_usage)("n3", account));
   EOS_ASSERT( ram_delta <= 0 || UINT64_MAX - usage.ram_usage >= (uint64_t)ram_delta, transaction_exception,
              "Ram usage delta would overflow UINT64_MAX");
   EOS_ASSERT(ram_delta >= 0 || usage.ram_usage >= (uint64_t)(-ram_delta), transaction_exception,
              "Ram usage delta would underflow UINT64_MAX");

   db.modify( usage, [&]( auto& u ) {
     u.ram_usage += ram_delta;
   });
}

void db_interface::db_remove_i64_ex( int iterator ) {
#if 0
   const key_value_object& obj = keyval_cache.get( iterator );

   update_db_usage( obj.payer,  -(obj.value.size() + config::billable_size_v<key_value_object>) );

   const auto& table_obj = keyval_cache.get_table( obj.t_id );
   FC_ASSERT( table_obj.code == receiver, "db access violation" );

//   require_write_lock( table_obj.scope );

   update_db_usage( obj.payer,  -(obj.value.size() + config::billable_size_v<key_value_object>) );

   db.modify( table_obj, [&]( auto& t ) {
      --t.count;
   });
   db.remove( obj );

   if (table_obj.count == 0) {
      remove_table(table_obj);
   }
#endif
   keyval_cache.remove( iterator );
}

void db_interface::db_get_table_i64( int iterator, uint64_t& code, uint64_t& scope, uint64_t& payer, uint64_t& table, uint64_t& id) {
   const key_value_object& obj = keyval_cache.get( iterator );
   const auto& table_obj = keyval_cache.get_table( obj.t_id );

   code = table_obj.code.to_uint64_t();
   scope = table_obj.scope.to_uint64_t();
   table = table_obj.table.to_uint64_t();
   payer = table_obj.payer.to_uint64_t();
   id = obj.primary_key;
}

const table_id_object* db_interface::find_table( name code, name scope, name table ) {
//   require_read_lock(code, scope);
   return db.find<table_id_object, by_code_scope_table>(boost::make_tuple(code, scope, table));
}

int db_interface::db_get_i64( int iterator, char* buffer, size_t buffer_size ) {
   const key_value_object& obj = keyval_cache.get( iterator );
   memcpy( buffer, obj.value.data(), std::min(obj.value.size(), buffer_size) );

   return obj.value.size();
}

int db_interface::db_get_i64( int iterator, string& buffer ) {
   const key_value_object& obj = keyval_cache.get( iterator );
   buffer = string(obj.value.data(), obj.value.size());
   return buffer.size();
}

int db_interface::db_get_i64_ex( int iterator, uint64_t& primary, char* buffer, size_t buffer_size ) {
   const key_value_object& obj = keyval_cache.get( iterator );
   memcpy( buffer, obj.value.data(), std::min(obj.value.size(), buffer_size) );

   primary = obj.primary_key;
   return obj.value.size();
}

const char* db_interface::db_get_i64_exex( int itr, size_t* buffer_size ) {
   const key_value_object& obj = keyval_cache.get( itr );
   *buffer_size = obj.value.size();
   return obj.value.data();
}

int db_interface::db_next_i64( int iterator, uint64_t& primary ) {
   if( iterator < -1 ) return -1; // cannot increment past end iterator of table

   const auto& obj = keyval_cache.get( iterator ); // Check for iterator != -1 happens in this call
   const auto& idx = db.get_index<key_value_index, by_scope_primary>();

   auto itr = idx.iterator_to( obj );
   ++itr;

   if( itr == idx.end() || itr->t_id != obj.t_id ) return keyval_cache.get_end_iterator_by_table_id(obj.t_id);

   primary = itr->primary_key;
   return keyval_cache.add( *itr );
}

int db_interface::db_previous_i64( int iterator, uint64_t& primary ) {
   const auto& idx = db.get_index<key_value_index, by_scope_primary>();

   if( iterator < -1 ) // is end iterator
   {
      auto tab = keyval_cache.find_table_by_end_iterator(iterator);
      FC_ASSERT( tab, "not a valid end iterator" );

      auto itr = idx.upper_bound(tab->id);
      if( idx.begin() == idx.end() || itr == idx.begin() ) return -1; // Empty table

      --itr;

      if( itr->t_id != tab->id ) return -1; // Empty table

      primary = itr->primary_key;
      return keyval_cache.add(*itr);
   }

   const auto& obj = keyval_cache.get(iterator); // Check for iterator != -1 happens in this call

   auto itr = idx.iterator_to(obj);
   if( itr == idx.begin() ) return -1; // cannot decrement past beginning iterator of table

   --itr;

   if( itr->t_id != obj.t_id ) return -1; // cannot decrement past beginning iterator of table

   primary = itr->primary_key;
   return keyval_cache.add(*itr);
}

int db_interface::db_find_i64( uint64_t code, uint64_t scope, uint64_t table, uint64_t id ) {
//   require_read_lock( code, scope ); // redundant?

   const auto* tab = find_table( name(code), name(scope), name(table) );
   if( !tab ) return -1;

   auto table_end_itr = keyval_cache.cache_table( *tab );

   const key_value_object* obj = db.find<key_value_object, by_scope_primary>( boost::make_tuple( tab->id, id ) );
   if( !obj ) return table_end_itr;

   return keyval_cache.add( *obj );
}

int db_interface::db_lowerbound_i64( uint64_t code, uint64_t scope, uint64_t table, uint64_t id ) {
//   require_read_lock( code, scope ); // redundant?

   const auto* tab = find_table( name(code), name(scope), name(table) );
   if( !tab ) return -1;

   auto table_end_itr = keyval_cache.cache_table( *tab );

   const auto& idx = db.get_index<key_value_index, by_scope_primary>();
   auto itr = idx.lower_bound( boost::make_tuple( tab->id, id ) );
   if( itr == idx.end() ) return table_end_itr;
   if( itr->t_id != tab->id ) return table_end_itr;

   return keyval_cache.add( *itr );
}

int db_interface::db_upperbound_i64( uint64_t code, uint64_t scope, uint64_t table, uint64_t id ) {
//   require_read_lock( code, scope ); // redundant?

   const auto* tab = find_table( name(code), name(scope), name(table) );
   if( !tab ) return -1;

   auto table_end_itr = keyval_cache.cache_table( *tab );

   const auto& idx = db.get_index<key_value_index, by_scope_primary>();
   auto itr = idx.upper_bound( boost::make_tuple( tab->id, id ) );
   if( itr == idx.end() ) return table_end_itr;
   if( itr->t_id != tab->id ) return table_end_itr;

   return keyval_cache.add( *itr );
}

int db_interface::db_end_i64( uint64_t code, uint64_t scope, uint64_t table ) {
//   require_read_lock( code, scope ); // redundant?

   const auto* tab = find_table( name(code), name(scope), name(table) );
   if( !tab ) return -1;

   return keyval_cache.cache_table( *tab );
}

uint32_t db_interface::db_get_table_count(uint64_t code, uint64_t scope, uint64_t table) {
   const auto* tab = find_table( name(code), name(scope), name(table) );
   if( !tab ) return 0;
   return tab->count;
}

int db_interface::db_store_i256( uint64_t scope, uint64_t table, const account_name& payer, key256_t& id, const char* buffer, size_t buffer_size ) {
   return db_store_i256( get_receiver().to_uint64_t(), scope, table, payer, id, buffer, buffer_size);
}

int db_interface::db_store_i256( uint64_t code, uint64_t scope, uint64_t table, const account_name& payer, key256_t& id, const char* buffer, size_t buffer_size ) {
//   require_write_lock( scope );
   const auto& tab = find_or_create_table( name(code), name(scope), name(table), name(payer) );
   auto tableid = tab.id;

   EOS_ASSERT( payer != account_name(), invalid_table_payer, "must specify a valid account to pay for new record" );

   const auto& obj = db.create<key256_value_object>( [&]( auto& o ) {
      o.t_id        = tableid;
      o.primary_key = id;
      o.value.resize( buffer_size );
      o.payer       = payer;
      memcpy( o.value.data(), buffer, buffer_size );
   });

   db.modify( tab, [&]( auto& t ) {
     ++t.count;
   });

   int64_t billable_size = (int64_t)(buffer_size + config::billable_size_v<key256_value_object>);
   update_db_usage( payer, billable_size);

   key256val_cache.cache_table( tab );
   return key256val_cache.add( obj );
}

void db_interface::db_update_i256( int iterator, account_name payer, const char* buffer, size_t buffer_size, bool check_code ) {
   const key256_value_object& obj = key256val_cache.get( iterator );

   const auto& table_obj = key256val_cache.get_table( obj.t_id );
   if (check_code) {
      EOS_ASSERT( table_obj.code == get_receiver(), table_access_violation, "db access violation" );
   }

//   require_write_lock( table_obj.scope );

   const int64_t overhead = config::billable_size_v<key256_value_object>;
   int64_t old_size = (int64_t)(obj.value.size() + overhead);
   int64_t new_size = (int64_t)(buffer_size + overhead);

   if( payer == account_name() ) payer = obj.payer;

   if( account_name(obj.payer) != payer ) {
      // refund the existing payer
      update_db_usage( obj.payer,  -(old_size) );
      // charge the new payer
      update_db_usage( payer,  (new_size));
   } else if(old_size != new_size) {
      // charge/refund the existing payer the difference
      update_db_usage( obj.payer, new_size - old_size);
   }

   db.modify( obj, [&]( auto& o ) {
     o.value.resize( buffer_size );
     memcpy( o.value.data(), buffer, buffer_size );
     o.payer = payer;
   });
}

void db_interface::db_remove_i256( int iterator, bool check_code ) {
   const key256_value_object& obj = key256val_cache.get( iterator );

   const auto& table_obj = key256val_cache.get_table( obj.t_id );
   if (check_code) {
      EOS_ASSERT( table_obj.code == get_receiver(), table_access_violation, "db access violation" );
   }

//   require_write_lock( table_obj.scope );

   update_db_usage( obj.payer,  -(obj.value.size() + config::billable_size_v<key256_value_object>) );

   db.modify( table_obj, [&]( auto& t ) {
      --t.count;
   });
   db.remove( obj );

   if (table_obj.count == 0) {
      remove_table(table_obj);
   }

   key256val_cache.remove( iterator );
}

int db_interface::db_get_i256( int iterator, char* buffer, size_t buffer_size ) {
   const key256_value_object& obj = key256val_cache.get( iterator );

   auto s = obj.value.size();
   if( buffer_size == 0 ) return s;

   auto copy_size = std::min( buffer_size, s );
   memcpy( buffer, obj.value.data(), copy_size );

   return copy_size;
}

int db_interface::db_find_i256( uint64_t code, uint64_t scope, uint64_t table, key256_t& id ) {
   //require_read_lock( code, scope ); // redundant?

   const auto* tab = find_table( name(code), name(scope), name(table) );
   if( !tab ) return -1;

   auto table_end_itr = key256val_cache.cache_table( *tab );

   const key256_value_object* obj = db.find<key256_value_object, by_scope_primary>( boost::make_tuple( tab->id, id ) );
   if( !obj ) return table_end_itr;

   return key256val_cache.add( *obj );
}

int db_interface::db_previous_i256( int iterator, key256_t& primary ) {
   const auto& idx = db.get_index<key256_value_index, by_scope_primary>();

   if( iterator < -1 ) // is end iterator
   {
      auto tab = key256val_cache.find_table_by_end_iterator(iterator);
      EOS_ASSERT( tab, invalid_table_iterator, "not a valid end iterator" );

      auto itr = idx.upper_bound(tab->id);
      if( idx.begin() == idx.end() || itr == idx.begin() ) return -1; // Empty table

      --itr;

      if( itr->t_id != tab->id ) return -1; // Empty table

      primary = itr->primary_key;
      return key256val_cache.add(*itr);
   }

   const auto& obj = key256val_cache.get(iterator); // Check for iterator != -1 happens in this call

   auto itr = idx.iterator_to(obj);
   if( itr == idx.begin() ) return -1; // cannot decrement past beginning iterator of table

   --itr;

   if( itr->t_id != obj.t_id ) return -1; // cannot decrement past beginning iterator of table

   primary = itr->primary_key;
   return key256val_cache.add(*itr);
}

int db_interface::db_next_i256( int iterator, key256_t& primary ) {
   if( iterator < -1 ) return -1; // cannot increment past end iterator of table

   const auto& obj = key256val_cache.get( iterator ); // Check for iterator != -1 happens in this call
   const auto& idx = db.get_index<key256_value_index, by_scope_primary>();

   auto itr = idx.iterator_to( obj );
   ++itr;

   if( itr == idx.end() || itr->t_id != obj.t_id ) return key256val_cache.get_end_iterator_by_table_id(obj.t_id);

   primary = itr->primary_key;
   return key256val_cache.add( *itr );
}

int db_interface::db_lowerbound_i256( uint64_t code, uint64_t scope, uint64_t table, key256_t& id ) {
   //require_read_lock( code, scope ); // redundant?

   const auto* tab = find_table( name(code), name(scope), name(table) );
   if( !tab ) return -1;

   auto table_end_itr = key256val_cache.cache_table( *tab );

   const auto& idx = db.get_index<key256_value_index, by_scope_primary>();
   auto itr = idx.lower_bound( boost::make_tuple( tab->id, id ) );
   if( itr == idx.end() ) return table_end_itr;
   if( itr->t_id != tab->id ) return table_end_itr;

   return key256val_cache.add( *itr );
}

int db_interface::db_upperbound_i256( uint64_t code, uint64_t scope, uint64_t table, key256_t& id ) {
   //require_read_lock( code, scope ); // redundant?

   const auto* tab = find_table( name(code), name(scope), name(table) );
   if( !tab ) return -1;

   auto table_end_itr = key256val_cache.cache_table( *tab );

   const auto& idx = db.get_index<key256_value_index, by_scope_primary>();
   auto itr = idx.upper_bound( boost::make_tuple( tab->id, id ) );
   if( itr == idx.end() ) return table_end_itr;
   if( itr->t_id != tab->id ) return table_end_itr;

   return key256val_cache.add( *itr );
}

int db_interface::db_end_i256( uint64_t code, uint64_t scope, uint64_t table ) {
   //require_read_lock( code, scope ); // redundant?

   const auto* tab = find_table( name(code), name(scope), name(table) );
   if( !tab ) return -1;

   return key256val_cache.cache_table( *tab );
}

vector<account_name> db_interface::get_genesis_accounts(chain::public_key_type public_key) {
   auto itr = key_accounts_map.find(public_key);
   if (itr == key_accounts_map.end()) {
      return {};
   }
   if (itr->second.size() <= 100) {
      return itr->second;
   }
   return vector<account_name>(itr->second.begin(), itr->second.begin()+100);
}

void db_interface::init_accounts() {
   init_accounts(gAccountsData, gAccountsSize);
}

void db_interface::init_accounts(const uint8_t* raw_data, size_t size) {
   vector<uint8_t> v(raw_data, raw_data+size);
   init_accounts(v);
}

void db_interface::init_accounts(const std::vector<uint8_t>& raw_data) {
   FC_ASSERT( raw_data.size() % (8+34) == 0, "bad accounts file" );
   for (int i=0;i<raw_data.size();i+=42) {
      uint64_t account;
      memcpy(&account, &raw_data[i], 8);

      account_name name(account);
      vector<char> key((char *)&raw_data[i+8], (char *)&raw_data[i+42]);
      auto pub_key = fc::raw::unpack<chain::public_key_type>(key);
      auto itr = key_accounts_map.find(pub_key);
      if (itr == key_accounts_map.end()) {
         key_accounts_map[pub_key] = {name};
      } else {
         key_accounts_map[pub_key].emplace_back(name);
      }
//      elog("++++${n}", ("n", name(account)));
      db_store_i64(N(eosio).to_uint64_t(), N(eosio).to_uint64_t(), N(gaccounts).to_uint64_t(), N(eosio), account, (char *)&raw_data[i+8], 34);
      if ((i/42+1) % 100000 == 0) {
         vmilog("+++++initialize genesis accounts %d\n", i/42+1);
      }
   }
   vmilog("+++++initialize genesis accounts %d\n", raw_data.size()/42);
}

void db_interface::init_accounts(const string& genesis_accounts_file) {
   dlog("++++genesis_accounts_file: ${s}", ("s", genesis_accounts_file));
   std::ifstream accounts_file(genesis_accounts_file, std::ios::binary);
   FC_ASSERT( accounts_file.is_open(), "accounts file cannot be found" );
   accounts_file.seekg(0, std::ios::end);
   std::vector<uint8_t> accounts;
   int len = accounts_file.tellg();
   FC_ASSERT( len >= 0, "accounts file length is -1" );
//   FC_ASSERT( len % (8+34) == 0, "bad file" );
   accounts.resize(len);
   accounts_file.seekg(0, std::ios::beg);
   accounts_file.read((char*)accounts.data(), accounts.size());
   accounts_file.close();
   init_accounts(accounts);
}


} } /// eosio::chain

using namespace eosio::chain;

int db_interface_get_i64(void *ptr, int itr, string& buffer ) {
   db_interface& db = *(db_interface *)ptr;
   return db.db_get_i64(itr, buffer);
}

int db_interface_next_i64(void *ptr, int itr, uint64_t* primary ) {
   db_interface& db = *(db_interface *)ptr;
   return db.db_next_i64(itr, *primary);
}

int db_interface_previous_i64(void *ptr, int itr, uint64_t* primary ) {
   db_interface& db = *(db_interface *)ptr;
   return db.db_previous_i64(itr, *primary);
}

int db_interface_find_i64(void *ptr, uint64_t code, uint64_t scope, uint64_t table, uint64_t id ) {
   db_interface& db = *(db_interface *)ptr;
   return db.db_find_i64(code, scope, table, id);
}

void db_interface_remove_i64(void *ptr,int itr) {
   db_interface& db = *(db_interface *)ptr;
   db.db_remove_i64( itr );
}

int db_interface_lowerbound_i64(void *ptr, uint64_t code, uint64_t scope, uint64_t table, uint64_t id ) {
   db_interface& db = *(db_interface *)ptr;
   return db.db_lowerbound_i64(code, scope, table, id);
}

int db_interface_upperbound_i64(void *ptr, uint64_t code, uint64_t scope, uint64_t table, uint64_t id ) {
   db_interface& db = *(db_interface *)ptr;
   return db.db_upperbound_i64(code, scope, table, id);
}

int db_interface_end_i64(void *ptr, uint64_t code, uint64_t scope, uint64_t table ) {
   db_interface& db = *(db_interface *)ptr;
   return db.db_end_i64(code, scope, table);
}

bool db_interface_is_account(void *ptr, uint64_t account) {
   db_interface& db = *(db_interface *)ptr;
   return db.is_account(account_name(account));
}

const char* db_interface_get_code_ex(void *ptr, uint64_t receiver, size_t* size ) {
   db_interface& db = *(db_interface *)ptr;
   return db.get_code_ex(receiver, size);
}
