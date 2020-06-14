#include <appbase/application.hpp>

#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>
#include <eosio/net_plugin/net_plugin.hpp>
#include <eosio/producer_plugin/producer_plugin.hpp>
#include <eosio/version/version.hpp>

#include <fc/log/logger_config.hpp>
#include <fc/log/appender.hpp>
#include <fc/exception/exception.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <chain_api.hpp>

#include "config.hpp"

using namespace appbase;
using namespace eosio;

namespace detail {

void configure_logging(const bfs::path& config_path)
{
   try {
      try {
         if( fc::exists( config_path ) ) {
            fc::configure_logging( config_path );
         } else {
            fc::configure_logging( fc::logging_config::default_config() );
         }
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
   if( fc::exists( config_path ) ) {
      ilog( "Received HUP.  Reloading logging configuration from ${p}.", ("p", config_path.string()) );
   } else {
      ilog( "Received HUP.  No log config found at ${p}, setting to default.", ("p", config_path.string()) );
   }
   ::detail::configure_logging( config_path );
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
   FIXED_REVERSIBLE  = SUCCESS,
   EXTRACTED_GENESIS = SUCCESS,
   NODE_MANAGEMENT_SUCCESS = 5
};

extern "C"
{
   void evm_init();
   void chain_api_init();
   void init_chain_api_callback();
   void init_history_callback();
   void vm_api_init();
   void sandboxed_contracts_init();
   int create_accounts_snapshot(int argc, char** argv);

   int init_python(bool pyeos, int argc, char **argv);
}

void uuos_set_version() {
   app().set_version(eosio::nodeos::config::version);
}

void uuos_set_default_data_dir_(string& dir) {
   app().set_default_data_dir(fc::path(dir));
}

void uuos_set_default_config_dir_(string& dir) {
   app().set_default_config_dir(fc::path(dir));  
}

int main(int argc, char** argv)
{
   for (int i=0;i<argc;i++) {
      if (strcmp(argv[i], "--create-accounts-snapshot")==0) {
         return create_accounts_snapshot(argc, argv);
      }
   }
   try {
      // evm_init();
      vm_api_init();
      chain_api_init();
      init_chain_api_callback();
      init_history_callback();
      sandboxed_contracts_init();
      {
         auto chain_api = get_chain_api();
         chain_api->uuos_set_version = uuos_set_version;
         chain_api->uuos_set_default_data_dir = uuos_set_default_data_dir_;
         chain_api->uuos_set_default_config_dir = uuos_set_default_config_dir_;
      }
      // return init_python(argc, argv);
      if (strcmp(argv[argc-1], "--pyeos") == 0) {
         argc -= 1;
         return init_python(true, argc, argv);
      } else {
         init_python(false, argc, argv);
      }

//      fc::logger::get(DEFAULT_LOGGER).set_log_level(fc::log_level::debug);

      app().set_version(eosio::nodeos::config::version);
      app().set_version_string(eosio::version::version_client());
      app().set_full_version_string(eosio::version::version_full());

      auto root = fc::app_path();
      app().set_default_data_dir(root / "eosio" / nodeos::config::node_executable_name / "data" );
      app().set_default_config_dir(root / "eosio" / nodeos::config::node_executable_name / "config" );
      http_plugin::set_defaults({
         .default_unix_socket_path = "uuos.sock",
         .default_http_port = 8888
      });
      if(!app().initialize<chain_plugin, net_plugin, producer_plugin>(argc, argv)) {
         const auto& opts = app().get_options();
         if( opts.count("help") || opts.count("version") || opts.count("full-version") || opts.count("print-default-config") ) {
            return SUCCESS;
         }
         return INITIALIZE_FAIL;
      }
      initialize_logging();
      ilog( "${name} version ${ver} ${fv}",
            ("name", nodeos::config::node_executable_name)("ver", app().version_string())
            ("fv", app().version_string() == app().full_version_string() ? "" : app().full_version_string()) );
      ilog("${name} using configuration file ${c}", ("name", nodeos::config::node_executable_name)("c", app().full_config_file_path().string()));
      ilog("${name} data directory is ${d}", ("name", nodeos::config::node_executable_name)("d", app().data_dir().string()));
      app().startup();
      app().set_thread_priority_max();
      app().exec();
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