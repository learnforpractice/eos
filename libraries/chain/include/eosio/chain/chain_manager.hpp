#pragma once
#include <chain_api.hpp>

#include <memory>
namespace eosio { namespace chain {

class controller;

class chain_manager {
    public:
        chain_manager(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);

    public:
        std::unique_ptr<controller> c;
};

}}
