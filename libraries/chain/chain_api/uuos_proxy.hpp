#pragma once

#include <string>
#include <vector>
#include <memory>

#include "chain_proxy.hpp"

using namespace std;

class vm_api_proxy;

class uuos_proxy {
    public:
        uuos_proxy();
        virtual ~uuos_proxy();

        virtual vm_api_proxy *get_vm_api_proxy();
        virtual void set_log_level(string& logger_name, int level);
        
        virtual chain_proxy* chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
        virtual void chain_free(chain_proxy* api);

        virtual void set_block_interval_ms(int ms);
        virtual void pack_abi(string& abi, vector<char>& packed_message);

        virtual void pack_native_object(int type, string& msg, vector<char>& packed_message);
        virtual void unpack_native_object(int type, string& packed_message, string& msg);

        virtual string& get_last_error();
        virtual void set_last_error(string& error);

    private:
        string last_error;
        std::shared_ptr<vm_api_proxy> _vm_api_proxy;
};

typedef void (*fn_init_uuos_proxy)(uuos_proxy *proxy);
extern "C" uuos_proxy *get_uuos_proxy();
