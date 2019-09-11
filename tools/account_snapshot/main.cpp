/**
 *  @file
 *  @copyright defined in eosio/LICENSE.txt
 */
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

#include "config.hpp"

#include <iostream>
#include <fstream>

using namespace appbase;
using namespace eosio;
using namespace std;

namespace detail {

void configure_logging(const bfs::path& config_path)
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

void logging_conf_handler()
{
   auto config_path = app().get_logging_conf();
   ilog("Received HUP.  Reloading logging configuration from ${p}.", ("p", config_path.string()));
   if(fc::exists(config_path))
      ::detail::configure_logging(config_path);
   fc::log_config::initialize_appenders( app().get_io_service() );
}

void initialize_logging()
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
   void vm_api_ro_init();
   void vm_api_init();
   void vm_python2_init();
   void native_contracts_init();
   int64_t token_get_balance(uint64_t owner, const char *str_symbol);
}

#include <eosiolib_native/vm_api.h>

int main(int argc, char** argv)
{
   vm_api_init();
   vm_api_ro_init();
   chain_api_init();
   vm_python2_init();
   native_contracts_init();

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

      auto idx = db.get_index<account_metadata_index>().indices();
      auto itr = idx.get<by_name>().end();
      auto itr2 = idx.get<by_name>().upper_bound(itr->name);

      string file_name = "accmounts.bin";
      fstream file(file_name, ios::out | ios::binary);

      int counter = 0;
      while (itr2 != idx.get<by_name>().end()) {
//         ilog("++++${n} ${n2}", ("n", itr2->name)("n2", token_get_balance(itr2->name.value, "EOS")));
         uint64_t value = itr2->name.value;
         file.write((char *)&value, 8);

         value = token_get_balance(itr2->name.value, "EOS");
         file.write((char *)&value, 8);

         itr2 = idx.get<by_name>().upper_bound(itr2->name);
         counter += 1;
         if (counter % 1000 == 0) {
            ilog("++++counter ${n}", ("n", counter));
         }
      }
      file.close();

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
