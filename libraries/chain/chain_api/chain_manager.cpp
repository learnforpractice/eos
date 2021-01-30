#include <eosio/chain/controller.hpp>
#include <eosio/chain/chain_manager.hpp>

#include <fc/io/raw.hpp>
#include <fc/io/json.hpp>
#include <string>

using namespace std;

namespace fc {
   void to_variant(const boost::filesystem::path& p, fc::variant& v) {
      v = p.generic_string(); 
   }

   void from_variant(const fc::variant& v, boost::filesystem::path& p) {
      p = v.get_string();
   }
} // fc

namespace eosio { namespace chain {

std::optional<builtin_protocol_feature> read_builtin_protocol_feature( const fc::path& p  ) {
   try {
      return fc::json::from_file<builtin_protocol_feature>( p );
   } catch( const fc::exception& e ) {
      wlog( "problem encountered while reading '${path}':\n${details}",
            ("path", p.generic_string())("details",e.to_detail_string()) );
   } catch( ... ) {
      dlog( "unknown problem encountered while reading '${path}'",
            ("path", p.generic_string()) );
   }
   return {};
}

protocol_feature_set initialize_protocol_features( const fc::path& p, bool populate_missing_builtins = true ) {
   using boost::filesystem::directory_iterator;

   protocol_feature_set pfs;

   bool directory_exists = true;

   if( fc::exists( p ) ) {
      EOS_ASSERT( fc::is_directory( p ), plugin_exception,
                  "Path to protocol-features is not a directory: ${path}",
                  ("path", p.generic_string())
      );
   } else {
      if( populate_missing_builtins )
         bfs::create_directories( p );
      else
         directory_exists = false;
   }

   auto log_recognized_protocol_feature = []( const builtin_protocol_feature& f, const digest_type& feature_digest ) {
      if( f.subjective_restrictions.enabled ) {
         if( f.subjective_restrictions.preactivation_required ) {
            if( f.subjective_restrictions.earliest_allowed_activation_time == time_point{} ) {
               ilog( "Support for builtin protocol feature '${codename}' (with digest of '${digest}') is enabled with preactivation required",
                     ("codename", builtin_protocol_feature_codename(f.get_codename()))
                     ("digest", feature_digest)
               );
            } else {
               ilog( "Support for builtin protocol feature '${codename}' (with digest of '${digest}') is enabled with preactivation required and with an earliest allowed activation time of ${earliest_time}",
                     ("codename", builtin_protocol_feature_codename(f.get_codename()))
                     ("digest", feature_digest)
                     ("earliest_time", f.subjective_restrictions.earliest_allowed_activation_time)
               );
            }
         } else {
            if( f.subjective_restrictions.earliest_allowed_activation_time == time_point{} ) {
               ilog( "Support for builtin protocol feature '${codename}' (with digest of '${digest}') is enabled without activation restrictions",
                     ("codename", builtin_protocol_feature_codename(f.get_codename()))
                     ("digest", feature_digest)
               );
            } else {
               ilog( "Support for builtin protocol feature '${codename}' (with digest of '${digest}') is enabled without preactivation required but with an earliest allowed activation time of ${earliest_time}",
                     ("codename", builtin_protocol_feature_codename(f.get_codename()))
                     ("digest", feature_digest)
                     ("earliest_time", f.subjective_restrictions.earliest_allowed_activation_time)
               );
            }
         }
      } else {
         ilog( "Recognized builtin protocol feature '${codename}' (with digest of '${digest}') but support for it is not enabled",
               ("codename", builtin_protocol_feature_codename(f.get_codename()))
               ("digest", feature_digest)
         );
      }
   };

   map<builtin_protocol_feature_t, fc::path>  found_builtin_protocol_features;
   map<digest_type, std::pair<builtin_protocol_feature, bool> > builtin_protocol_features_to_add;
   // The bool in the pair is set to true if the builtin protocol feature has already been visited to add
   map< builtin_protocol_feature_t, std::optional<digest_type> > visited_builtins;

   // Read all builtin protocol features
   if( directory_exists ) {
      for( directory_iterator enditr, itr{p}; itr != enditr; ++itr ) {
         auto file_path = itr->path();
         if( !fc::is_regular_file( file_path ) || file_path.extension().generic_string().compare( ".json" ) != 0 )
            continue;

         auto f = read_builtin_protocol_feature( file_path );

         if( !f ) continue;

         auto res = found_builtin_protocol_features.emplace( f->get_codename(), file_path );

         EOS_ASSERT( res.second, plugin_exception,
                     "Builtin protocol feature '${codename}' was already included from a previous_file",
                     ("codename", builtin_protocol_feature_codename(f->get_codename()))
                     ("current_file", file_path.generic_string())
                     ("previous_file", res.first->second.generic_string())
         );

         const auto feature_digest = f->digest();

         builtin_protocol_features_to_add.emplace( std::piecewise_construct,
                                                   std::forward_as_tuple( feature_digest ),
                                                   std::forward_as_tuple( *f, false ) );
      }
   }

   // Add builtin protocol features to the protocol feature manager in the right order (to satisfy dependencies)
   using itr_type = map<digest_type, std::pair<builtin_protocol_feature, bool>>::iterator;
   std::function<void(const itr_type&)> add_protocol_feature =
   [&pfs, &builtin_protocol_features_to_add, &visited_builtins, &log_recognized_protocol_feature, &add_protocol_feature]( const itr_type& itr ) -> void {
      if( itr->second.second ) {
         return;
      } else {
         itr->second.second = true;
         visited_builtins.emplace( itr->second.first.get_codename(), itr->first );
      }

      for( const auto& d : itr->second.first.dependencies ) {
         auto itr2 = builtin_protocol_features_to_add.find( d );
         if( itr2 != builtin_protocol_features_to_add.end() ) {
            add_protocol_feature( itr2 );
         }
      }

      pfs.add_feature( itr->second.first );

      log_recognized_protocol_feature( itr->second.first, itr->first );
   };

   for( auto itr = builtin_protocol_features_to_add.begin(); itr != builtin_protocol_features_to_add.end(); ++itr ) {
      add_protocol_feature( itr );
   }

   auto output_protocol_feature = [&p]( const builtin_protocol_feature& f, const digest_type& feature_digest ) {
      static constexpr int max_tries = 10;

      string filename( "BUILTIN-" );
      filename += builtin_protocol_feature_codename( f.get_codename() );
      filename += ".json";

      auto file_path = p / filename;

      EOS_ASSERT( !fc::exists( file_path ), plugin_exception,
                  "Could not save builtin protocol feature with codename '${codename}' because a file at the following path already exists: ${path}",
                  ("codename", builtin_protocol_feature_codename( f.get_codename() ))
                  ("path", file_path.generic_string())
      );

      if( fc::json::save_to_file( f, file_path ) ) {
         ilog( "Saved default specification for builtin protocol feature '${codename}' (with digest of '${digest}') to: ${path}",
               ("codename", builtin_protocol_feature_codename(f.get_codename()))
               ("digest", feature_digest)
               ("path", file_path.generic_string())
         );
      } else {
         elog( "Error occurred while writing default specification for builtin protocol feature '${codename}' (with digest of '${digest}') to: ${path}",
               ("codename", builtin_protocol_feature_codename(f.get_codename()))
               ("digest", feature_digest)
               ("path", file_path.generic_string())
         );
      }
   };

   std::function<digest_type(builtin_protocol_feature_t)> add_missing_builtins =
   [&pfs, &visited_builtins, &output_protocol_feature, &log_recognized_protocol_feature, &add_missing_builtins, populate_missing_builtins]
   ( builtin_protocol_feature_t codename ) -> digest_type {
      auto res = visited_builtins.emplace( codename, std::optional<digest_type>() );
      if( !res.second ) {
         EOS_ASSERT( res.first->second, protocol_feature_exception,
                     "invariant failure: cycle found in builtin protocol feature dependencies"
         );
         return *res.first->second;
      }

      auto f = protocol_feature_set::make_default_builtin_protocol_feature( codename,
      [&add_missing_builtins]( builtin_protocol_feature_t d ) {
         return add_missing_builtins( d );
      } );

      if( !populate_missing_builtins )
         f.subjective_restrictions.enabled = false;

      const auto& pf = pfs.add_feature( f );
      res.first->second = pf.feature_digest;

      log_recognized_protocol_feature( f, pf.feature_digest );

      if( populate_missing_builtins )
         output_protocol_feature( f, pf.feature_digest );

      return pf.feature_digest;
   };

   for( const auto& p : builtin_protocol_feature_codenames ) {
      auto itr = found_builtin_protocol_features.find( p.first );
      if( itr != found_builtin_protocol_features.end() ) continue;

      add_missing_builtins( p.first );
   }

   return pfs;
}

chain_manager::chain_manager(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir) {

   this->cfg = fc::json::from_string(config).as<eosio::chain::controller::config>();
   this->genesis = fc::json::from_string(_genesis).as<genesis_state>();
   this->snapshot_dir = snapshot_dir;
   this->protocol_features_dir = protocol_features_dir;
}

void chain_manager::init() {
    auto pfs = initialize_protocol_features( boost::filesystem::path(protocol_features_dir) );
    auto chain_id = this->genesis.compute_chain_id();

    this->c = std::make_shared<controller>(this->cfg, std::move(pfs), chain_id);
    this->c->add_indices();
}

chain_manager::~chain_manager() {
}

void chain_manager::shutdown() {
   _shutdown = true;
}

bool chain_manager::startup(bool init_db) {
   auto _shutdown = [](){ return true; };
   auto _check_shutdown = [&](){ return _shutdown; };

   if (snapshot_dir.size()) {
      auto infile = std::ifstream(snapshot_dir, (std::ios::in | std::ios::binary));
      auto reader = std::make_shared<istream_snapshot_reader>(infile);
      c->startup(_shutdown, _check_shutdown, reader);
      infile.close();
   } else if (init_db) {
      c->startup(_shutdown, _check_shutdown, genesis);
   } else {
      c->startup(_shutdown, _check_shutdown);
   }
   return true;
}

}}


