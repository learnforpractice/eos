#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

#include "chain_proxy.hpp"
#include "apply_context_proxy.hpp"
#include "../vm_api/vm_api_proxy.hpp"

using namespace std;


class chain_rpc_api_proxy;
class apply_context_proxy;

typedef chain_rpc_api_proxy *(*fn_new_chain_api)(eosio::chain::controller *c);
typedef int (*fn_native_apply)(uint64_t a, uint64_t b, uint64_t c);

struct native_contract {
    string path;
    void *handle;
    fn_native_apply apply;
};

typedef int (*fn_eos_init)(int argc, char** argv);
typedef int (*fn_eos_exec)();

class uuos_proxy {
    public:
        uuos_proxy();
        virtual ~uuos_proxy();

        virtual apply_context_proxy *get_apply_context_proxy();
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

        virtual uint64_t s2n(string& name);
        virtual string n2s(uint64_t n);

        virtual bool set_native_contract(uint64_t contract, const string& native_contract_lib);
        virtual string get_native_contract(uint64_t contract);
        virtual bool call_native_contract(uint64_t receiver, uint64_t first_receiver, uint64_t action);
        virtual void enable_native_contracts(bool debug);
        virtual bool is_native_contracts_enabled();

        fn_new_chain_api new_chain_api = nullptr;

        fn_eos_init eos_init;
        fn_eos_exec eos_exec;

    private:
        string last_error;
        std::shared_ptr<apply_context_proxy> _apply_context_proxy;
        bool native_contracts_enabled = false;
        std::map<uint64_t, native_contract> debug_contracts;
};

typedef void (*fn_init_uuos_proxy)(uuos_proxy *proxy);
extern "C" uuos_proxy *get_uuos_proxy();
