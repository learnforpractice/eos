#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "chain_rpc_api_proxy.hpp"

using namespace std;

namespace eosio {
    namespace chain {
        class chain_manager;
        class controller;
        struct abi_serializer;
    }
}

class chain_proxy {
    public:
        chain_proxy(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
        virtual ~chain_proxy();

        virtual eosio::chain::controller* chain();
        virtual chain_rpc_api_proxy* api_proxy();

        virtual void say_hello();

        virtual void id(string& chain_id);
        virtual void start_block(string& _time, uint16_t confirm_block_count, string& _new_features);
        virtual int abort_block();
        virtual bool startup(bool initdb);
        virtual void finalize_block(string& _priv_keys);
        virtual void commit_block();
        virtual string get_block_id_for_num(uint32_t block_num);
        virtual string get_global_properties();
        virtual string get_dynamic_global_properties();
        virtual string get_actor_whitelist();
        virtual string get_actor_blacklist();
        virtual string get_contract_whitelist();
        virtual string get_contract_blacklist();
        virtual string get_action_blacklist();
        virtual string get_key_blacklist();
        virtual void set_actor_whitelist(string& params);
        virtual void set_actor_blacklist(string& params);
        virtual void set_contract_whitelist(string& params);
        virtual void set_action_blacklist(string& params);
        virtual void set_key_blacklist(string& params);
        virtual uint32_t head_block_num();
        virtual string head_block_time();
        virtual string head_block_id();
        virtual string head_block_producer();
        virtual string head_block_header();
        virtual string head_block_state();
        virtual uint32_t fork_db_head_block_num();
        virtual string fork_db_head_block_id();
        virtual string fork_db_head_block_time();
        virtual string fork_db_head_block_producer();
        virtual uint32_t fork_db_pending_head_block_num();
        virtual string fork_db_pending_head_block_id();
        virtual string fork_db_pending_head_block_time();
        virtual string fork_db_pending_head_block_producer();
        virtual string pending_block_time();
        virtual string pending_block_producer();
        virtual string pending_producer_block_id();
        virtual string get_pending_trx_receipts();
        virtual string active_producers();
        virtual string pending_producers();
        virtual string proposed_producers();
        virtual uint32_t last_irreversible_block_num();
        virtual string last_irreversible_block_id();
        virtual string fetch_block_by_number(uint32_t block_num);
        virtual string fetch_block_by_id(string& params);
        virtual string fetch_block_state_by_number(uint32_t block_num);
        virtual string fetch_block_state_by_id(string& params);
        virtual string calculate_integrity_hash();
        virtual bool sender_avoids_whitelist_blacklist_enforcement(string& sender);
        virtual bool check_actor_list(string& param);
        virtual bool check_contract_list(string& param);
        virtual bool check_action_list(string& code, string& action);
        virtual bool check_key_list(string& param);
        virtual bool is_building_block();
        virtual bool is_producing_block();
        virtual bool is_ram_billing_in_notify_allowed();
        virtual void add_resource_greylist(string& param);
        virtual void remove_resource_greylist(string& param);
        virtual bool is_resource_greylisted(string& param);
        virtual string get_resource_greylist();
        virtual string get_config();
        virtual bool validate_expiration(string& param);
        virtual bool validate_tapos(string& param);
        virtual bool validate_db_available_size();
        virtual bool validate_reversible_available_size();
        virtual bool is_protocol_feature_activated(string& param);
        virtual bool is_builtin_activated(int feature);
        virtual bool is_known_unexpired_transaction(string& param);
        virtual int64_t set_proposed_producers(string& param);
        virtual bool light_validation_allowed();
        virtual bool skip_auth_check();
        virtual bool skip_db_sessions();
        virtual bool skip_trx_checks();
        virtual bool contracts_console();
        virtual int get_read_mode();
        virtual int get_validation_mode();
        virtual void set_subjective_cpu_leeway(uint64_t leeway);
        virtual void set_greylist_limit(uint32_t limit);
        virtual uint32_t get_greylist_limit();
        virtual void add_to_ram_correction(string& account, uint64_t ram_bytes, uint32_t action_id, const char* event_id);
        virtual bool all_subjective_mitigations_disabled();
        virtual string get_scheduled_producer(string& _block_time);

        virtual void gen_transaction(string& _actions, string& expiration, string& reference_block_id, string& _chain_id, bool compress, std::string& _private_keys, vector<char>& result);
        virtual string push_transaction(string& _packed_trx, string& deadline, uint32_t billed_cpu_time_us);

        virtual string get_scheduled_transactions();
        virtual string get_scheduled_transaction(const unsigned __int128 sender_id, string& sender);
        virtual string push_scheduled_transaction(string& scheduled_tx_id, string& deadline, uint32_t billed_cpu_time_us);

        virtual bool pack_action_args(string& name, string& action, string& _args, vector<char>& result);
        virtual string unpack_action_args(string& name, string& action, string& _binargs);
        virtual string get_producer_public_keys();

        virtual string& get_last_error();
        virtual void set_last_error(string& error);

        virtual eosio::chain::abi_serializer* get_abi_cache(string& account);
        virtual void clear_abi_cache(string& account);

    private:
        void load_abi(string& account);

    private:
        std::unique_ptr<eosio::chain::chain_manager> cm;
        std::shared_ptr<eosio::chain::controller> c;
        std::shared_ptr<chain_rpc_api_proxy> _api_proxy;
        std::map<std::string, std::shared_ptr<eosio::chain::abi_serializer>> abi_cache;
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