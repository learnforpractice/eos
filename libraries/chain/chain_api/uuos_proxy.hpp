#pragma once

#include <string>
#include "chain_proxy.hpp"

using namespace std;

class uuos_proxy {
    public:
        uuos_proxy();
        virtual ~uuos_proxy();
        virtual void set_log_level(string& logger_name, int level);
        
        virtual chain_proxy* chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
        virtual void chain_free(chain_proxy* api);

        virtual void set_block_interval_ms(int ms);
        virtual void pack_abi(string& abi, vector<char>& packed_message);

        virtual string& get_last_error();
        virtual void set_last_error(string& error);

    private:
        string last_error;

};

typedef void (*fn_init_uuos_proxy)(uuos_proxy *proxy);
extern "C" uuos_proxy *get_uuos_proxy();
