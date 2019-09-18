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

#include <boost/algorithm/string.hpp>

#include "db_interface.hpp"
#include <chain_api.hpp>
#include <stacktrace.h>

using boost::container::flat_set;
using namespace fc;

extern "C" void execution_start();
extern "C" void execution_end();

namespace eosio { namespace chain {

#include <eosiolib_native/vm_api.h>

db_interface::db_interface(chainbase::database& _db):
db(_db),
act(action()),
idx64(*this),
idx128(*this),
idx256(*this),
idx_double(*this),
idx_long_double(*this) {

}

extern "C" {
   void wasm_interface_init_1(int vm_type);
   bool wasm_interface_apply_1(const eosio::chain::digest_type& code_id, const uint8_t& vm_type, const uint8_t& vm_version);
}

string db_interface::exec_action(uint64_t code, uint64_t action, const vector<char>& args) {
   static bool init = false;
   if (!init) {
      init = true;
      wasm_interface_init_1(0);
   }
   call_start = fc::time_point::now();
   _pending_console_output.clear();

   receiver = code;

   act.account = name(code);
   act.name = name(action);
   act.data = args;
   const auto& account = db.get<account_metadata_object,by_name>(act.account);
   bool existing_code = (account.code_hash != digest_type());
   wasm_interface_apply_1(account.code_hash, account.vm_type, account.vm_version);
   return _pending_console_output;
}


#if 0
void db_interface::get_code(uint64_t account, string& code) {
   const auto &a = db.get<account_object, by_name>(account);
   code = string(a.code.data(), a.code.size());
}

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
   const auto& tab = find_or_create_table( code, scope, table, payer );
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
   return db_store_i64(get_receiver(),  scope, table,payer, id, buffer, buffer_size );
}

void db_interface::db_update_i64( int iterator, account_name payer, const char* buffer, size_t buffer_size ) {
   const key_value_object& obj = keyval_cache.get( iterator );

   const auto& table_obj = keyval_cache.get_table( obj.t_id );
   if (table_obj.code != receiver) {
      print_stacktrace();
   }
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

void db_interface::update_db_usage( const account_name& payer, int64_t delta ) {
#if 0
   require_write_lock( payer );
   if( (delta > 0) ) {
      if (!(privileged || payer == account_name(receiver))) {
         require_authorization( payer );
      }

      mutable_controller.get_mutable_resource_limits_manager().add_pending_account_ram_usage(payer, delta);
   }
#endif
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

   code = table_obj.code;
   scope = table_obj.scope;
   table = table_obj.table;
   payer = table_obj.payer;
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

   const auto* tab = find_table( code, scope, table );
   if( !tab ) return -1;

   auto table_end_itr = keyval_cache.cache_table( *tab );

   const key_value_object* obj = db.find<key_value_object, by_scope_primary>( boost::make_tuple( tab->id, id ) );
   if( !obj ) return table_end_itr;

   return keyval_cache.add( *obj );
}

int db_interface::db_lowerbound_i64( uint64_t code, uint64_t scope, uint64_t table, uint64_t id ) {
//   require_read_lock( code, scope ); // redundant?

   const auto* tab = find_table( code, scope, table );
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

   const auto* tab = find_table( code, scope, table );
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

   const auto* tab = find_table( code, scope, table );
   if( !tab ) return -1;

   return keyval_cache.cache_table( *tab );
}

uint32_t db_interface::db_get_table_count(uint64_t code, uint64_t scope, uint64_t table) {
   const auto* tab = find_table( code, scope, table );
   if( !tab ) return 0;
   return tab->count;
}

int db_interface::db_store_i256( uint64_t scope, uint64_t table, const account_name& payer, key256_t& id, const char* buffer, size_t buffer_size ) {
   return db_store_i256( get_receiver(), scope, table, payer, id, buffer, buffer_size);
}

int db_interface::db_store_i256( uint64_t code, uint64_t scope, uint64_t table, const account_name& payer, key256_t& id, const char* buffer, size_t buffer_size ) {
//   require_write_lock( scope );
   const auto& tab = find_or_create_table( code, scope, table, payer );
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

   const auto* tab = find_table( code, scope, table );
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

   const auto* tab = find_table( code, scope, table );
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

   const auto* tab = find_table( code, scope, table );
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

   const auto* tab = find_table( code, scope, table );
   if( !tab ) return -1;

   return key256val_cache.cache_table( *tab );
}

#include "../../../unittests/incbin.h"
INCBIN(Accounts, "accounts.bin");
/*
 * // const unsigned char gAccountsData[];
 * // const unsigned char *const <prefix>FooEnd;
 * // const unsigned int gAccountsSize;
*/


void db_interface::init_accounts(std::vector<uint8_t>& raw_data) {
   for (int i=0; i<raw_data.size(); i+=(8+34)) {
      uint64_t account;
      memcpy(&account, &raw_data[i], 8);
      db_store_i64(N(eosio), N(eosio), N(gaccounts), N(eosio),
                  account,
                  (char *)&raw_data[i+8], 34);
      if (i % (42*100000) == 0) {
         vmdlog("+++++initialize accounts %d\n", i);
      }
   }
}

void db_interface::init_accounts(string& genesis_accounts_file) {
   account_record a;
   uint64_t id = 0;
   auto raw_data = fc::raw::pack(a);

   std::ifstream accounts_file(genesis_accounts_file, std::ios::binary);
   FC_ASSERT( accounts_file.is_open(), "accounts file cannot be found" );
   accounts_file.seekg(0, std::ios::end);
   std::vector<uint8_t> accounts;
   int len = accounts_file.tellg();
   FC_ASSERT( len >= 0, "accounts file length is -1" );
   FC_ASSERT( len % (8+34) == 0, "bad file" );
   accounts.resize(len);
   accounts_file.seekg(0, std::ios::beg);
   accounts_file.read((char*)accounts.data(), accounts.size());
   accounts_file.close();
   init_accounts(accounts);
}


} } /// eosio::chain



