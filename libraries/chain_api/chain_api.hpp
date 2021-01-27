#pragma once

#include <string>
#include <memory>

using namespace std;

namespace eosio {
    namespace chain {
        class chain_manager;
    }
}

class chain_api {
    public:
        chain_api(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
        virtual ~chain_api();
    private:
        std::unique_ptr<eosio::chain::chain_manager> c;
};

extern "C" {
    typedef chain_api* (*fn_chain_new)(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
    typedef void (*fn_chain_free)(chain_api* api);
    typedef void (*fn_init_chain_api)(fn_chain_new _init, fn_chain_free _free);

    chain_api* chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
    void chain_free(chain_api* api);
    void int_chain_api(fn_chain_new _init, fn_chain_free _free);
}