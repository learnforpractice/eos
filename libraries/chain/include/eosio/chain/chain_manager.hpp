#pragma once
#include <chain_proxy.hpp>
#include <eosio/chain/controller.hpp>

#include <memory>

namespace eosio { namespace chain {

class controller;

class chain_manager {
    public:
        chain_manager(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
        ~chain_manager();
        void init();
        bool startup(bool init_db);
        void shutdown();
    public:
        std::shared_ptr<controller> c;
        controller::config cfg;
        genesis_state genesis;
        string snapshot_dir;
        string protocol_features_dir;
        bool _shutdown = false;
};

}}

namespace fc {
   void to_variant(const boost::filesystem::path& p, fc::variant& v);
   void from_variant(const fc::variant& v, boost::filesystem::path& p);
} // fc