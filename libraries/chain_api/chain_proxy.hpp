#pragma once

#include <string>
#include <memory>

using namespace std;

namespace eosio {
    namespace chain {
        class chain_manager;
        class controller;
    }
}

class chain_proxy {
    public:
        chain_proxy(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
        virtual ~chain_proxy();
        void init();

        virtual void say_hello();

        virtual void id(string& chain_id);
        virtual void start_block(string& _time, uint16_t confirm_block_count, string& _new_features);
        virtual int abort_block();
        virtual bool startup(bool initdb);
        virtual void finalize_block(string& _priv_keys);
        virtual void commit_block();

        virtual string& get_last_error();
        virtual void set_last_error(string& error);

    private:
        std::unique_ptr<eosio::chain::chain_manager> cm;
        std::shared_ptr<eosio::chain::controller> c;
        string last_error;
};

extern "C" {
    typedef chain_proxy* (*fn_chain_new)(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
    typedef void (*fn_chain_free)(chain_proxy* api);
    typedef void (*fn_init_chain_api)(fn_chain_new _init, fn_chain_free _free);

    chain_proxy* chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
    void chain_free(chain_proxy* api);
    void int_chain_api(fn_chain_new _init, fn_chain_free _free);
}