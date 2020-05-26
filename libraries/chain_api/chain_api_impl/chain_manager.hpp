#pragma once
// #include <string>

// #include <boost/filesystem/path.hpp>
// #include <eosio/chain/platform_timer.hpp>

#include <eosio/chain/controller.hpp>
#include <fc/io/json.hpp>
#include <fc/io/raw.hpp>
#include <fc/variant.hpp>


using namespace std;
namespace bfs = boost::filesystem;
using namespace eosio;
using namespace eosio::chain;

void *chain_new_(string& config, string& protocol_features_dir);
void chain_free_(void *ptr);

namespace eosio {
    namespace chain {
        class apply_context;
        class transaction_context;
        struct signed_transaction;
    }
}

class chain_manager {
public:
    chain_manager();
    bool init(string& config, string& genesis, string& protocol_features_dir, string& snapshot_dir);
    bool startup(bool init_db);

    void log_guard_exception(const chain::guard_exception&e );
    void on_accepted_transaction( const transaction_metadata_ptr& meta );
    void on_accepted_block(const block_state_ptr& bsp);
    controller& chain();
    controller::config& config();
    ~chain_manager();

    controller *cc = nullptr;
    controller::config cfg;
    string snapshot_dir;
    genesis_state genesis;

};

eosio::chain::controller& chain_get_controller(void *ptr);
