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

class chain_api {
    public:
        chain_api(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
        virtual ~chain_api();
        virtual void say_hello();

        virtual void id(string& chain_id);
        virtual void start_block(string& _time, uint16_t confirm_block_count, string& _new_features);
        virtual int abort_block();

        virtual string& get_last_error();
        virtual void set_last_error(string& error);

    private:
        std::unique_ptr<eosio::chain::chain_manager> cm;
        eosio::chain::controller& c;
        string last_error;
};

extern "C" {
    typedef chain_api* (*fn_chain_new)(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
    typedef void (*fn_chain_free)(chain_api* api);
    typedef void (*fn_init_chain_api)(fn_chain_new _init, fn_chain_free _free);

    chain_api* chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
    void chain_free(chain_api* api);
    void int_chain_api(fn_chain_new _init, fn_chain_free _free);
}