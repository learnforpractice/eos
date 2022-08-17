#include <eosio/chain/controller.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/chain/chain_manager.hpp>
#include <eosio/chain/global_property_object.hpp>
#include <eosio/chain/generated_transaction_object.hpp>
#include <eosio/chain/webassembly/interface.hpp>

#include <fc/io/json.hpp>

#include <dlfcn.h>

#include "chain_macro.hpp"

#include "ipyeos_proxy.hpp"

extern "C" ipyeos_proxy *get_ipyeos_proxy();

using namespace eosio::chain;

chain_proxy::chain_proxy()
{
}

chain_proxy::~chain_proxy() {

}

int chain_proxy::init(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir) {
    cm = std::make_unique<chain_manager>(config, _genesis, protocol_features_dir, snapshot_dir);
    this->cm->init();
    this->c = this->cm->c;

    chain_rpc_api_proxy *api = get_ipyeos_proxy()->new_chain_api(this->c.get());
    this->_api_proxy = std::shared_ptr<chain_rpc_api_proxy>(api);
    return 1;
}

int chain_proxy::attach(eosio::chain::controller* c) {
    this->c = std::unique_ptr<eosio::chain::controller>(c);
    chain_rpc_api_proxy *api = get_ipyeos_proxy()->new_chain_api(this->c.get());
    this->_api_proxy = std::shared_ptr<chain_rpc_api_proxy>(api);
    return 1;
}

controller* chain_proxy::chain() {
    return this->c.get();
}

chain_rpc_api_proxy* chain_proxy::api_proxy() {
    return this->_api_proxy.get();
}

void chain_proxy::say_hello() {
    printf("hello,world from chain_proxy\n");
}

void chain_proxy::id(string& chain_id) {
    try {
        chain_id = c->get_chain_id().str();
    } CATCH_AND_LOG_EXCEPTION();
}

bool chain_proxy::startup(bool initdb) {
    try {
        cm->startup(initdb);
        return true;
    } CATCH_AND_LOG_EXCEPTION();
    cm.reset();
    return false;
}

int chain_proxy::start_block(string& _time, uint16_t confirm_block_count, string& _new_features) {
    try {
        auto time = fc::time_point::from_iso_string(_time);
        if (_new_features.size()) {
            auto new_features = fc::json::from_string(_new_features).as<vector<digest_type>>();
            c->start_block(block_timestamp_type(time), confirm_block_count, new_features);
        } else {
            c->start_block(block_timestamp_type(time), confirm_block_count);
        }
        return 1;
    } CATCH_AND_LOG_EXCEPTION();
    return 0;
}

int chain_proxy::abort_block() {
   try {
        c->abort_block();
        return 1;
   } CATCH_AND_LOG_EXCEPTION();
   return 0;
}

void chain_proxy::finalize_block(string& _priv_keys) {
    try {
        auto priv_keys = fc::json::from_string(_priv_keys).as<vector<string>>();
        c->finalize_block( [&]( const digest_type d ) {
            vector<signature_type> sigs;
            for (auto& key: priv_keys) {
                auto priv_key = private_key_type(key);
                sigs.emplace_back(priv_key.sign(d));
            }
            return sigs;
        } );
    } CATCH_AND_LOG_EXCEPTION();

}

void chain_proxy::commit_block() {
    try {
        c->commit_block();
    } CATCH_AND_LOG_EXCEPTION();
}

string chain_proxy::get_block_id_for_num(uint32_t block_num ) {
    auto id = c->get_block_id_for_num(block_num);
    return id.str();
}

string chain_proxy::get_global_properties() {
    auto& obj = c->get_global_properties();
    return fc::json::to_string(obj, fc::time_point::maximum());
}

string chain_proxy::get_dynamic_global_properties() {
    try {
        auto& obj = c->get_dynamic_global_properties();
        return fc::json::to_string(obj, fc::time_point::maximum());
    } CATCH_AND_LOG_EXCEPTION()
    return "";
}

string chain_proxy::get_actor_whitelist() {
    return fc::json::to_string(c->get_actor_whitelist(), fc::time_point::maximum());
}

string chain_proxy::get_actor_blacklist() {
    return fc::json::to_string(c->get_actor_blacklist(), fc::time_point::maximum());
}

string chain_proxy::get_contract_whitelist() {
    return fc::json::to_string(c->get_contract_whitelist(), fc::time_point::maximum());
}

string chain_proxy::get_contract_blacklist() {
    return fc::json::to_string(c->get_contract_blacklist(), fc::time_point::maximum());
}

string chain_proxy::get_action_blacklist() {
    return fc::json::to_string(c->get_action_blacklist(), fc::time_point::maximum());
}

string chain_proxy::get_key_blacklist() {
    return fc::json::to_string(c->get_key_blacklist(), fc::time_point::maximum());
}

void chain_proxy::set_actor_whitelist(string& params) {
    auto whitelist = fc::json::from_string(params).as<flat_set<account_name>>();
    c->set_actor_whitelist(whitelist);
}

void chain_proxy::set_actor_blacklist(string& params) {
    auto whitelist = fc::json::from_string(params).as<flat_set<account_name>>();
    c->set_actor_blacklist(whitelist);
}

void chain_proxy::set_contract_whitelist(string& params) {
    auto whitelist = fc::json::from_string(params).as<flat_set<account_name>>();
    c->set_contract_whitelist(whitelist);
}

void chain_proxy::set_action_blacklist(string& params) {
    auto whitelist = fc::json::from_string(params).as<flat_set<pair<account_name, action_name>>>();
    c->set_action_blacklist(whitelist);
}

void chain_proxy::set_key_blacklist(string& params) {
    auto whitelist = fc::json::from_string(params).as<flat_set<public_key_type>>();
    c->set_key_blacklist(whitelist);
}

uint32_t chain_proxy::head_block_num() {
    return c->head_block_num();
}

string chain_proxy::head_block_time() {
    return c->head_block_time();
}

string chain_proxy::head_block_id() {
    return c->head_block_id().str();
}

string chain_proxy::head_block_producer() {
    return c->head_block_producer().to_string();
}

string chain_proxy::head_block_header() {
    return fc::json::to_string(c->head_block_header(), fc::time_point::maximum());
}

string chain_proxy::head_block_state() {
    return fc::json::to_string(c->head_block_state(), fc::time_point::maximum());
}

uint32_t chain_proxy::fork_db_head_block_num() {
    return c->fork_db_head_block_num();
}

string chain_proxy::fork_db_head_block_id() {
    return c->fork_db_head_block_id().str();
}

string chain_proxy::fork_db_head_block_time() {
    return c->fork_db_head_block_time();
}

string chain_proxy::fork_db_head_block_producer() {
    return c->fork_db_head_block_producer().to_string();
}

uint32_t chain_proxy::fork_db_pending_head_block_num() {
    try {
        return c->fork_db_pending_head_block_num();
    } CATCH_AND_LOG_EXCEPTION()
    return 0;
}

string chain_proxy::fork_db_pending_head_block_id() {
    return c->fork_db_pending_head_block_id().str();
}

string chain_proxy::fork_db_pending_head_block_time() {
    return c->fork_db_pending_head_block_time();
}

string chain_proxy::fork_db_pending_head_block_producer() {
    return c->fork_db_pending_head_block_producer().to_string();
}

string chain_proxy::pending_block_time() {
    return c->pending_block_time();
}

string chain_proxy::pending_block_producer() {
    return c->pending_block_producer().to_string();
}

string chain_proxy::pending_producer_block_id() {
    auto id = c->pending_producer_block_id();
    if (id) {
        return id->str();
    }
    return "";
}

string chain_proxy::get_pending_trx_receipts() {
    return fc::json::to_string(c->get_pending_trx_receipts(), fc::time_point::maximum());
}

string chain_proxy::active_producers() {
    return fc::json::to_string(c->active_producers(), fc::time_point::maximum());
}

string chain_proxy::pending_producers() {
    return fc::json::to_string(c->pending_producers(), fc::time_point::maximum());
}

string chain_proxy::proposed_producers() {
    return fc::json::to_string(c->proposed_producers(), fc::time_point::maximum());
}

uint32_t chain_proxy::last_irreversible_block_num() {
    return c->last_irreversible_block_num();
}

string chain_proxy::last_irreversible_block_id() {
    try {
        return c->last_irreversible_block_id().str();
    } CATCH_AND_LOG_EXCEPTION()
    return "";
}

string chain_proxy::fetch_block_by_number(uint32_t block_num) {
    try {
        auto block_ptr = c->fetch_block_by_number(block_num);
        if (!block_ptr) {
            return "";
        }
        auto _raw_block = fc::raw::pack<eosio::chain::signed_block>(*block_ptr);
        return string(_raw_block.data(), _raw_block.size());
    } CATCH_AND_LOG_EXCEPTION()
    return "";
}

string chain_proxy::fetch_block_by_id(string& params) {
    try {
        auto block_id = fc::variant(params).as<block_id_type>();
        auto block_ptr = c->fetch_block_by_id(block_id);
        if (!block_ptr) {
            return "";
        }
        auto _raw_block = fc::raw::pack<eosio::chain::signed_block>(*block_ptr);
        return string(_raw_block.data(), _raw_block.size());
    } CATCH_AND_LOG_EXCEPTION()
    return "";
}

string chain_proxy::fetch_block_state_by_number(uint32_t block_num) {
    try {
        auto block_ptr = c->fetch_block_state_by_number(block_num);
        if (!block_ptr) {
            return "";
        }
        auto _raw_block_state = fc::raw::pack<eosio::chain::block_state>(*block_ptr);
        return string(_raw_block_state.data(), _raw_block_state.size());
    } CATCH_AND_LOG_EXCEPTION()
    return "";
}

string chain_proxy::fetch_block_state_by_id(string& params) {
    try {
        auto block_id = fc::json::from_string(params).as<block_id_type>();
        auto block_ptr = c->fetch_block_state_by_id(block_id);
        if (!block_ptr) {
            return "";
        }
        auto _raw_block_state = fc::raw::pack<eosio::chain::block_state>(*block_ptr);
        return string(_raw_block_state.data(), _raw_block_state.size());
    } CATCH_AND_LOG_EXCEPTION()
    return "";
}

string chain_proxy::calculate_integrity_hash() {
    auto hash = c->calculate_integrity_hash();
    return hash.str();
}

/*
         void write_snapshot( const snapshot_writer_ptr& snapshot )const;
*/

bool chain_proxy::sender_avoids_whitelist_blacklist_enforcement(string& sender) {
    return c->sender_avoids_whitelist_blacklist_enforcement(account_name(sender));
}

bool chain_proxy::check_actor_list(string& param) {
    try {
        auto _param = fc::json::from_string(param).as<flat_set<account_name>>();
        c->check_actor_list(_param);
        return true;
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_proxy::check_contract_list(string& param) {
    try {
        auto _param = fc::variant(param).as<account_name>();
        c->check_contract_list(_param);
        return true;
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_proxy::check_action_list(string& code, string& action) {
    try {
        c->check_action_list(name(code), name(action));
        return true;
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_proxy::check_key_list(string& param) {
    try {
        auto _param = fc::json::from_string(param).as<public_key_type>();
        c->check_key_list(_param);
        return true;
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_proxy::is_building_block() {
    return c->is_building_block();
}

bool chain_proxy::is_producing_block() {
    return c->is_producing_block();
}

bool chain_proxy::is_ram_billing_in_notify_allowed() {
    return c->is_ram_billing_in_notify_allowed();
}

void chain_proxy::add_resource_greylist(string& param) {
    c->add_resource_greylist(name(param));
}

void chain_proxy::remove_resource_greylist(string& param) {
    c->remove_resource_greylist(name(param));
}

bool chain_proxy::is_resource_greylisted(string& param) {
    return c->is_resource_greylisted(name(param));
}

string chain_proxy::get_resource_greylist() {
    return fc::json::to_string(c->get_resource_greylist(), fc::time_point::maximum());
}

string chain_proxy::get_config() {
    return fc::json::to_string(c->get_config(), fc::time_point::maximum());
}

bool chain_proxy::validate_expiration(string& param) {
    try {
        auto trx = fc::json::from_string(param).as<transaction>();
        c->validate_expiration(trx);
        return true;
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_proxy::validate_tapos(string& param) {
    try {
        auto trx = fc::json::from_string(param).as<transaction>();
        c->validate_tapos(trx);
        return true;
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_proxy::validate_db_available_size() {
    try {
        c->validate_db_available_size();
        return true;
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_proxy::validate_reversible_available_size() {
    try {
        c->validate_reversible_available_size();
        return true;
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_proxy::is_protocol_feature_activated(string& param) {
    auto digest = fc::variant(param).as<digest_type>();
    return c->is_protocol_feature_activated(digest);
}

bool chain_proxy::is_builtin_activated(int feature) {
    return c->is_builtin_activated((builtin_protocol_feature_t)feature);
}

bool chain_proxy::is_known_unexpired_transaction(string& param) {
    auto id = fc::json::from_string(param).as<transaction_id_type>();
    return c->is_known_unexpired_transaction(id);
}

int64_t chain_proxy::set_proposed_producers(string& param) {
    auto id = fc::json::from_string(param).as<vector<producer_authority>>();
    return c->set_proposed_producers(id);
}

bool chain_proxy::light_validation_allowed() {
    return c->light_validation_allowed();
}

bool chain_proxy::skip_auth_check() {
    return c->skip_auth_check();
}

bool chain_proxy::skip_db_sessions() {
    return c->skip_db_sessions();
}

bool chain_proxy::skip_trx_checks() {
    return c->skip_trx_checks();
}

bool chain_proxy::contracts_console() {
    return c->contracts_console();
}

int chain_proxy::get_read_mode() {
    return (int)c->get_read_mode();
}

int chain_proxy::get_validation_mode() {
    return (int)c->get_validation_mode();
}

void chain_proxy::set_subjective_cpu_leeway(uint64_t leeway) {
    c->set_subjective_cpu_leeway(fc::microseconds(leeway));
}

void chain_proxy::set_greylist_limit(uint32_t limit) {
    c->set_greylist_limit(limit);
}

uint32_t chain_proxy::get_greylist_limit() {
    return c->get_greylist_limit();
}

void chain_proxy::add_to_ram_correction(string& account, uint64_t ram_bytes, uint32_t action_id, const char* event_id) {
    c->add_to_ram_correction(account_name(account), ram_bytes, action_id, event_id);
}

bool chain_proxy::all_subjective_mitigations_disabled() {
    return c->all_subjective_mitigations_disabled();
}

string chain_proxy::get_scheduled_producer(string& _block_time) {
    auto block_time = fc::time_point::from_iso_string(_block_time);
    auto producer = c->head_block_state()->get_scheduled_producer(block_time);
    return fc::json::to_string(producer, fc::time_point::maximum());
}

void chain_proxy::gen_transaction(bool json, string& _actions, string& expiration, string& reference_block_id, string& _chain_id, bool compress, std::string& _private_keys, vector<char>& result) {
    try {
        signed_transaction trx;
        auto actions = fc::json::from_string(_actions).as<vector<eosio::chain::action>>();
        trx.actions = std::move(actions);
        trx.expiration = fc::time_point::from_iso_string(expiration);
        eosio::chain::block_id_type id(reference_block_id);
        trx.set_reference_block(id);
        trx.max_net_usage_words = 0;

        eosio::chain::chain_id_type chain_id(_chain_id);

        auto priv_keys = fc::json::from_string(_private_keys).as<vector<private_key_type>>();
        for (auto& key: priv_keys) {
            trx.sign(key, chain_id);
        }
        packed_transaction::compression_type type;
        if (compress) {
            type = packed_transaction::compression_type::zlib;
        } else {
            type = packed_transaction::compression_type::none;
        }
        if (json) {
            string s = fc::json::to_string(trx, fc::time_point::maximum());
            result = vector<char>(s.begin(), s.end());
        } else {
            auto packed_trx = packed_transaction(std::move(trx), true, type);
            auto v = fc::raw::pack(packed_trx);
            result = std::move(v);
        }
    } CATCH_AND_LOG_EXCEPTION();
}

string chain_proxy::push_transaction(string& _packed_trx, string& deadline, uint32_t billed_cpu_time_us, bool explicit_cpu_bill, uint32_t subjective_cpu_bill_us) {
    try {
        vector<char> packed_trx(_packed_trx.c_str(), _packed_trx.c_str()+_packed_trx.size());
        auto ptrx = std::make_shared<packed_transaction>();
        fc::datastream<const char*> ds( _packed_trx.c_str(), _packed_trx.size() );
        fc::raw::unpack(ds, *ptrx);
        auto ptrx_meta = transaction_metadata::recover_keys(ptrx, c->get_chain_id());
    //    auto ptrx_meta = transaction_metadata::create_no_recover_keys( trx, transaction_metadata::trx_type::input );
        auto _deadline = fc::time_point::from_iso_string(deadline);
        auto ret = c->push_transaction(ptrx_meta, _deadline, billed_cpu_time_us, explicit_cpu_bill, subjective_cpu_bill_us);
        return fc::json::to_string(ret, fc::time_point::maximum());
        // if (ret->except) {
        //     return false;
        // }
    } CATCH_AND_LOG_EXCEPTION();
    return "";
}

string chain_proxy::get_scheduled_transactions() {
    vector<transaction_id_type> result;
    const auto& idx = c->db().get_index<generated_transaction_multi_index,by_delay>();
    auto itr = idx.begin();
    while( itr != idx.end() && itr->delay_until <= c->pending_block_time() ) {
        result.emplace_back(itr->trx_id);
        ++itr;
    }
    return fc::json::to_string(result, fc::time_point::maximum());
}

string chain_proxy::get_scheduled_transaction(const unsigned __int128 sender_id, string& sender) {
    auto& generated_transaction_idx = c->db().get_index<generated_transaction_multi_index>();
    const auto* gto = c->db().find<generated_transaction_object,by_sender_id>(boost::make_tuple(account_name(sender), sender_id));
    if ( gto ) {
        return fc::json::to_string(*gto, fc::time_point::maximum());
    }
    return "";
}

string chain_proxy::push_scheduled_transaction(string& scheduled_tx_id, string& deadline, uint32_t billed_cpu_time_us) {
    auto id = transaction_id_type(scheduled_tx_id);
    auto _deadline = fc::time_point::from_iso_string(deadline);
    auto ret = c->push_scheduled_transaction(id, _deadline, billed_cpu_time_us, false);
    return fc::json::to_string(ret, fc::time_point::maximum());
}

void chain_proxy::load_abi(string& account) {
    auto itr = abi_cache.find(account);
    if (itr == abi_cache.end()) {
        const auto& accnt = c->db().get<account_object, by_name>(account_name(account));
        abi_def abi;
        if(!abi_serializer::to_abi(accnt.abi, abi)) {
            return;
        }
        abi_cache[account] = std::make_shared<abi_serializer>(abi, abi_serializer::create_yield_function(fc::microseconds(150000)));
    }
}

eosio::chain::abi_serializer* chain_proxy::get_abi_cache(string& account) {
    auto itr = abi_cache.find(account);
   if (itr != abi_cache.end()) {
      return itr->second.get();
   }
   return nullptr;
}

void chain_proxy::clear_abi_cache(string& account) {
   auto itr = abi_cache.find(account);
   if (itr != abi_cache.end()) {
      abi_cache.erase(itr);
   }
}

bool chain_proxy::pack_action_args(string& account, string& action, string& _args, vector<char>& result) {
    try {
        load_abi(account);
        auto& serializer = abi_cache[account];
        if (!serializer) {
            EOS_ASSERT(false, action_validate_exception, "ABI for {account} not found!", ("account", account));
            return false;
        }
        auto action_type = serializer->get_action_type(action_name(action));
        EOS_ASSERT(!action_type.empty(), action_validate_exception, "Unknown action ${action}", ("action", action));

        fc::variant args = fc::json::from_string(_args);
        result = serializer->variant_to_binary(action_type, args, abi_serializer::create_yield_function(fc::microseconds(150000)));
        return true;
    } CATCH_AND_LOG_EXCEPTION();
    return false;
}

string chain_proxy::unpack_action_args(string& name, string& action, string& _binargs) {
    try {
        auto& serializer = abi_cache[name];
        if (!serializer) {
            EOS_ASSERT(false, action_validate_exception, "ABI for {account} not found!", ("account", name));
            return "";
        }
        auto action_type = serializer->get_action_type(action_name(action));
        EOS_ASSERT(!action_type.empty(), action_validate_exception, "Unknown action ${action}", ("action", action));

        bytes binargs = bytes(_binargs.data(), _binargs.data() + _binargs.size());
        auto v = serializer->binary_to_variant(action_type, binargs, abi_serializer::create_yield_function(fc::microseconds(150000)));
        return fc::json::to_string(v, fc::time_point::maximum());
    } CATCH_AND_LOG_EXCEPTION();
    return "";
}

string chain_proxy::get_producer_public_keys() {
    try {
        const auto& auth = c->pending_block_signing_authority();
        std::vector<public_key_type> pub_keys;

        producer_authority::for_each_key(auth, [&](const public_key_type& key){
            pub_keys.emplace_back(key);
        });

        return fc::json::to_string(pub_keys, fc::time_point::maximum());
    } CATCH_AND_LOG_EXCEPTION();
    return "";
}
