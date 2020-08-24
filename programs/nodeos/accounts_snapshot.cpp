/**
 *  @file
 *  @copyright defined in eosio/LICENSE.txt
 */
#include <stdint.h>
#include <stdlib.h>
#include <softfloat_types.h>
#include <vm_api/vm_api.h>

#include <appbase/application.hpp>

#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>
#include <eosio/net_plugin/net_plugin.hpp>
#include <eosio/producer_plugin/producer_plugin.hpp>

#include <fc/log/logger_config.hpp>
#include <fc/log/appender.hpp>
#include <fc/exception/exception.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <eosio/chain/permission_object.hpp>

#include <iostream>
#include <fstream>

#include "config.hpp"

using namespace appbase;
using namespace eosio;
using namespace std;

namespace detail {

static void configure_logging(const bfs::path& config_path)
{
   try {
      try {
         fc::configure_logging(config_path);
      } catch (...) {
         elog("Error reloading logging.json");
         throw;
      }
   } catch (const fc::exception& e) {
      elog("${e}", ("e",e.to_detail_string()));
   } catch (const boost::exception& e) {
      elog("${e}", ("e",boost::diagnostic_information(e)));
   } catch (const std::exception& e) {
      elog("${e}", ("e",e.what()));
   } catch (...) {
      // empty
   }
}

} // namespace detail

static void logging_conf_handler()
{
   auto config_path = app().get_logging_conf();
   ilog("Received HUP.  Reloading logging configuration from ${p}.", ("p", config_path.string()));
   if(fc::exists(config_path))
      ::detail::configure_logging(config_path);
   fc::log_config::initialize_appenders( app().get_io_service() );
}

static void initialize_logging()
{
   auto config_path = app().get_logging_conf();
   if(fc::exists(config_path))
     fc::configure_logging(config_path); // intentionally allowing exceptions to escape
   fc::log_config::initialize_appenders( app().get_io_service() );

   app().set_sighup_callback(logging_conf_handler);
}

enum return_codes {
   OTHER_FAIL        = -2,
   INITIALIZE_FAIL   = -1,
   SUCCESS           = 0,
   BAD_ALLOC         = 1,
   DATABASE_DIRTY    = 2,
   FIXED_REVERSIBLE  = 3,
   EXTRACTED_GENESIS = 4,
   NODE_MANAGEMENT_SUCCESS = 5
};

extern "C"
{
   void chain_api_init();
   void vm_api_init();
//   void vm_python2_init();
   void sandboxed_contracts_init();
   int64_t token_get_balance(uint64_t owner, const char *str_symbol);
}

public_key_type find_public_key_by_name2(const chainbase::database& db, account_name name, permission_name perm_name, int depth) {
//   ilog("+++++++${name} ${perm_name}", ("name", name)("perm_name", perm_name));
   const auto& permissions = db.get_index<permission_index,by_owner>();
   auto perm = permissions.lower_bound( boost::make_tuple( name ) );
   while( perm != permissions.end() && perm->owner == name ) {// && perm->name == perm_name) {
      if (perm->auth.keys.size() != 0) {
         return perm->auth.keys[0].key;
      }
      ++perm;
   }

   depth -= 1;
   if (depth <= 0) {
      return public_key_type();
   }
   
   perm = permissions.lower_bound( boost::make_tuple( name ) );
   while( perm != permissions.end() && perm->owner == name ) {// && perm->name == perm_name) {
      for (auto& account: perm->auth.accounts) {
         if (N(uuos.code) == account.permission.permission) {
            continue;
         }
         auto public_key = find_public_key_by_name2(db, account.permission.actor, account.permission.permission, depth);
//         ilog("+++++++${name} ${perm_name} ${public_key}", ("name", account.permission.actor)("perm_name", account.permission.permission)("public_key", public_key));
         if (public_key != public_key_type()) {
//            ilog("+++++++got it ${key}", ("key", public_key));
            return public_key;
         }
      }
      ++perm;
   }
//   ilog("+++++++${name}", ("name", name));
   return public_key_type();
}

public_key_type find_public_key_by_name(const chainbase::database& db, account_name name) {
   const auto& permissions = db.get_index<permission_index,by_owner>();
   auto perm = permissions.lower_bound( boost::make_tuple( name ) );
   while( perm != permissions.end() && perm->owner == name ) {
      if (perm->auth.keys.size() != 0) {
         return perm->auth.keys[0].key;
      }
      ++perm;
   }

   perm = permissions.lower_bound( boost::make_tuple( name ) );
   while( perm != permissions.end() && perm->owner == name ) {
      for (auto& account: perm->auth.accounts) {
         if (N(uuos.code) == account.permission.permission) {
            continue;
         }
//         ilog("----${name} ${perm_name}", ("name", account.permission.actor)("perm_name", account.permission.permission));
         auto public_key = find_public_key_by_name2(db, account.permission.actor, account.permission.permission, 5);
         if (public_key != public_key_type()) {
            return public_key;
         }
      }
      ++perm;
   }
   return public_key_type();
}

map<account_name, bool> ignore_accounts;

void init_ignore_accounts() {
    for (auto& a : vector<string>{
                     "eosio.bpay",
                     "eosio.msig",
                     "eosio.names",
                     "eosio.ram",
                     "eosio.ramfee",
                     "eosio.saving",
                     "eosio.stake",
                     "eosio.token",
                     "eosio.vpay",
                     "eosio.rex",

                     "blacklistmee",
                     "ge2dmmrqgene",
                     "gu2timbsguge",
                     "ge4tsmzvgege",
                     "gezdonzygage",
                     "ha4tkobrgqge",
                     "ha4tamjtguge",
                     "gq4dkmzzhege",
                     "gu2teobyg4ge",
                     "gq4demryhage",
                     "q4dfv32fxfkx",
                     "ktl2qk5h4bor",
                     "haydqnbtgene",
                     "g44dsojygyge",
                     "guzdonzugmge",
                     "ha4doojzgyge",
                     "gu4damztgyge",
                     "haytanjtgige",
                     "exchangegdax",
                     "cmod44jlp14k",
                     "2fxfvlvkil4e",
                     "yxbdknr3hcxt",
                     "yqjltendhyjp",
                     "pm241porzybu",
                     "xkc2gnxfiswe",
                     "ic433gs42nky",
                     "fueaji11lhzg",
                     "w1ewnn4xufob",
                     "ugunxsrux2a3",
                     "gz3q24tq3r21",
                     "u5rlltjtjoeo",
                     "k5thoceysinj",
                     "ebhck31fnxbi",
                     "pvxbvdkces1x",
                     "oucjrjjvkrom",
                     "neverlandwal",
                     "tseol5n52kmo",
                     "potus1111111",
                     "craigspys211",
                     "eosfomoplay1",
                     "wangfuhuahua",
                     "ha4timrzguge",
                     "guytqmbuhege",
                     "huobldeposit",
                     "gm3dcnqgenes",
                     "gm34qnqrepqt",
                     "gt3ftnqrrpqp",
                     "gtwvtqptrpqp",
                     "gm31qndrspqr",
                     "lxl2atucpyos",
                     "g4ytenbxgqge",
                     "jinwen121212",
                     "ha4tomztgage",
                     "my1steosobag",
                     "iloveyouplay",
                     "eoschinaeos2",
                     "eosholderkev",
                     "dreams12true",
                     "imarichman55",
                     "gizdcnjyg4ge",
                     "gyzdmmjsgige",
                     "guzdanrugene",
                     "earthsop1sys",
                     "refundwallet",
                     "jhonnywalker",
                     "alibabaioeos",
                     "whitegroupes",
                     "24cryptoshop",
                     "minedtradeos",
                     "newdexmobapp",
                     "ftsqfgjoscma",
                     "hpbcc4k42nxy",
                     "3qyty1khhkhv",
                     "xzr2fbvxwtgt",
                     "myqdqdj4qbge",
                     "shprzailrazt",
                     "qkwrmqowelyu",
                     "lhjuy3gdkpq4",
                     "lmfsopxpr324",
                     "lcxunh51a1gt",
                     "geydddsfkk5e",
                     "pnsdiia1pcuy",
                     "kwmvzswquqpb",
                     "guagddoefdqu",
    }) {
      ignore_accounts[account_name(a)] = true;
    }
}

extern "C" int create_accounts_snapshot(int argc, char** argv)
{
   vm_api_init();
   chain_api_init();
//   vm_python2_init();
//   wasmc2_contracts_init();

   init_ignore_accounts();

   fc::logger::get(DEFAULT_LOGGER).set_log_level(fc::log_level::debug);

   try {
      app().set_version(eosio::nodeos::config::version);

      auto root = fc::app_path();
      app().set_default_data_dir(root / "eosio" / nodeos::config::node_executable_name / "data" );
      app().set_default_config_dir(root / "eosio" / nodeos::config::node_executable_name / "config" );
      http_plugin::set_defaults({
         .default_unix_socket_path = "uuos.sock",
         .default_http_port = 8888
      });
      if(!app().initialize<chain_plugin, net_plugin, producer_plugin>(argc, argv))
         return INITIALIZE_FAIL;
      initialize_logging();
      ilog("${name} version ${ver}", ("name", nodeos::config::node_executable_name)("ver", app().version_string()));
      ilog("${name} using configuration file ${c}", ("name", nodeos::config::node_executable_name)("c", app().full_config_file_path().string()));
      ilog("${name} data directory is ${d}", ("name", nodeos::config::node_executable_name)("d", app().data_dir().string()));
//      app().startup();

      vm_api *api_ro = get_vm_api_ro();
      vm_api *api = get_vm_api();
      printf("+++++++++++++%p %p\n", api_ro, api);

      app().get_plugin<chain_plugin>().plugin_startup();
      auto& db = app().get_plugin<chain_plugin>().chain().db();
      vm_register_api(api_ro);

      const auto& accounts = db.get_index<account_metadata_index, by_name>();
      auto itr = accounts.upper_bound(account_name(0));

      string file_name = "genesis_accounts.bin";
      fstream file(file_name, ios::out | ios::binary);

      int counter = 0;
      while (itr != accounts.end()) {
         if (ignore_accounts.find(itr->name) != ignore_accounts.end()) {
            itr++;
            continue;
         }
         auto public_key = find_public_key_by_name(db, itr->name);
         if (public_key == public_key_type()) {
            ilog("++++${n} ${key}", ("n", itr->name)("key", public_key));
            itr++;
            continue;
         }
         file.write((char*)&itr->name, 8);
         auto raw = fc::raw::pack(public_key);
         file.write(raw.data(), raw.size());
         counter += 1;
         if (counter % 10000 == 0) {
            ilog("++++counter ${n}", ("n", counter));
         }
         itr++;
      }
      ilog("++++counter ${n}", ("n", counter));
      file.close();
      app().shutdown();
//      app().exec();
   } catch( const extract_genesis_state_exception& e ) {
      return EXTRACTED_GENESIS;
   } catch( const fixed_reversible_db_exception& e ) {
      return FIXED_REVERSIBLE;
   } catch( const node_management_success& e ) {
      return NODE_MANAGEMENT_SUCCESS;
   } catch( const fc::exception& e ) {
      if( e.code() == fc::std_exception_code ) {
         if( e.top_message().find( "database dirty flag set" ) != std::string::npos ) {
            elog( "database dirty flag set (likely due to unclean shutdown): replay required" );
            return DATABASE_DIRTY;
         } else if( e.top_message().find( "database metadata dirty flag set" ) != std::string::npos ) {
            elog( "database metadata dirty flag set (likely due to unclean shutdown): replay required" );
            return DATABASE_DIRTY;
         }
      }
      elog( "${e}", ("e", e.to_detail_string()));
      return OTHER_FAIL;
   } catch( const boost::interprocess::bad_alloc& e ) {
      elog("bad alloc");
      return BAD_ALLOC;
   } catch( const boost::exception& e ) {
      elog("${e}", ("e",boost::diagnostic_information(e)));
      return OTHER_FAIL;
   } catch( const std::runtime_error& e ) {
      if( std::string(e.what()).find("database dirty flag set") != std::string::npos ) {
         elog( "database dirty flag set (likely due to unclean shutdown): replay required" );
         return DATABASE_DIRTY;
      } else if( std::string(e.what()) == "database metadata dirty flag set" ) {
         elog( "database metadata dirty flag set (likely due to unclean shutdown): replay required" );
         return DATABASE_DIRTY;
      } else {
         elog( "${e}", ("e",e.what()));
      }
      return OTHER_FAIL;
   } catch( const std::exception& e ) {
      elog("${e}", ("e",e.what()));
      return OTHER_FAIL;
   } catch( ... ) {
      elog("unknown exception");
      return OTHER_FAIL;
   }

   ilog("${name} successfully exiting", ("name", nodeos::config::node_executable_name));
   return SUCCESS;
}
