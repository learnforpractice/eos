#pragma once
#include <chain_api.hpp>
#include <eosio/chain/controller.hpp>

#include <memory>

namespace eosio { namespace chain {

class controller;

class chain_manager {
    public:
        chain_manager(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
        ~chain_manager();
        bool startup(bool init_db);
        void shutdown();
    public:
        std::unique_ptr<controller> c;
        controller::config cfg;
        string snapshot_dir;
        genesis_state genesis;
        bool _shutdown = false;
};

}}
