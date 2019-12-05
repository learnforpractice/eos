#pragma once
#include <string>

#include <boost/filesystem/path.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>

#include <fc/io/json.hpp>
#include <fc/io/raw.hpp>
#include <fc/variant.hpp>

using namespace std;
namespace bfs = boost::filesystem;
using namespace eosio::chain;


void *chain_new_(string& config, string& protocol_features_dir);
void chain_free_(void *ptr);
void chain_on_incoming_block_(void *ptr, string& packed_signed_block, uint32_t& num, string& id);


class chain_manager {
private:
    chain_manager(string& config, string& protocol_features_dir);
public:
    static chain_manager *init(string& config, string& protocol_features_dir);
    static chain_manager& get();
    void on_accepted_block(const block_state_ptr& bsp);
    controller& chain();
    controller::config& config();
    ~chain_manager();

    controller *cc = nullptr;
    controller::config cfg;
    static chain_manager *instance;
};
