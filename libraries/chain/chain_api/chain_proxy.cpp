#include <eosio/chain/controller.hpp>
#include <eosio/chain/chain_manager.hpp>
#include <eosio/chain/global_property_object.hpp>

#include <fc/io/json.hpp>

#include "chain_proxy.hpp"

#include <dlfcn.h>
using namespace eosio::chain;

#define CATCH_AND_LOG_EXCEPTION(proxy)\
   catch ( const fc::exception& e ) {\
      string ex = fc::json::to_string(*e.dynamic_copy_exception(), fc::time_point::maximum()); \
      elog("${s}", ("s", ex)); \
      proxy->set_last_error(ex);\
   } catch ( const std::exception& e ) {\
      fc::exception fce( \
         FC_LOG_MESSAGE( warn, "rethrow ${what}: ", ("what",e.what())),\
         fc::std_exception_code,\
         BOOST_CORE_TYPEID(e).name(),\
         e.what() ) ; \
        string ex = fc::json::to_string(*fce.dynamic_copy_exception(), fc::time_point::maximum()); \
        elog("${s}", ("s", ex)); \
        proxy->set_last_error(ex);\
   } catch( ... ) {\
      fc::unhandled_exception e(\
         FC_LOG_MESSAGE(warn, "rethrow"),\
         std::current_exception());\
        string ex = fc::json::to_string(*e.dynamic_copy_exception(), fc::time_point::maximum()); \
        elog("${s}", ("s", ex)); \
        proxy->set_last_error(ex); \
   }

#define CATCH_AND_LOG_EXCEPTION_AND_RETURN_FALSE(proxy) \
   catch ( const fc::exception& e ) {\
      string ex = fc::json::to_string(*e.dynamic_copy_exception(), fc::time_point::maximum()); \
      elog("${s}", ("s", ex)); \
      proxy->set_last_error(ex);\
      return false; \
   } catch ( const std::exception& e ) {\
      fc::exception fce( \
         FC_LOG_MESSAGE( warn, "rethrow ${what}: ", ("what",e.what())),\
         fc::std_exception_code,\
         BOOST_CORE_TYPEID(e).name(),\
         e.what() ) ;\
        string ex = fc::json::to_string(*fce.dynamic_copy_exception(), fc::time_point::maximum()); \
        elog("${s}", ("s", ex)); \
        proxy->set_last_error(ex);\
        return false; \
   } catch( ... ) {\
      fc::unhandled_exception e(\
         FC_LOG_MESSAGE(warn, "rethrow"),\
         std::current_exception());\
        string ex = fc::json::to_string(*e.dynamic_copy_exception(), fc::time_point::maximum()); \
        elog("${s}", ("s", ex)); \
        proxy->set_last_error(ex);\
        return false; \
   }

string& chain_proxy::get_last_error() {
    return last_error;
}

void chain_proxy::set_last_error(string& error) {
    last_error = error;
}

chain_proxy::chain_proxy(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir)
{
    cm = std::make_unique<chain_manager>(config, _genesis, protocol_features_dir, snapshot_dir);
}

chain_proxy::~chain_proxy() {

}

void chain_proxy::init() {
    this->cm->init();
    this->c = this->cm->c;
}

void chain_proxy::say_hello() {
    printf("hello,world from chain_proxy\n");
}

void chain_proxy::id(string& chain_id) {
    try {
        chain_id = c->get_chain_id().str();
    } CATCH_AND_LOG_EXCEPTION(this);
}

bool chain_proxy::startup(bool initdb) {
    try {
        cm->startup(initdb);
        return true;
    } CATCH_AND_LOG_EXCEPTION(this);
    cm.reset();
    return false;
}

void chain_proxy::start_block(string& _time, uint16_t confirm_block_count, string& _new_features) {
    try {
        auto time = fc::time_point::from_iso_string(_time);
        if (_new_features.size()) {
            auto new_features = fc::json::from_string(_new_features).as<vector<digest_type>>();
            c->start_block(block_timestamp_type(time), confirm_block_count, new_features);
        } else {
            c->start_block(block_timestamp_type(time), confirm_block_count);
        }
    } CATCH_AND_LOG_EXCEPTION(this);
}

int chain_proxy::abort_block() {
   try {
        c->abort_block();
        return 1;
   } CATCH_AND_LOG_EXCEPTION(this);
   return 0;
}

void chain_proxy::finalize_block(string& _priv_keys) {
    auto priv_keys = fc::json::from_string(_priv_keys).as<vector<string>>();
    c->finalize_block( [&]( const digest_type d ) {
        vector<signature_type> sigs;
        for (auto& key: priv_keys) {
            auto priv_key = private_key_type(key);
            sigs.emplace_back(priv_key.sign(d));
        }
        return sigs;
    } );
}

void chain_proxy::commit_block() {
    c->commit_block();
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
    } CATCH_AND_LOG_EXCEPTION(this)
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
    } CATCH_AND_LOG_EXCEPTION(this)
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
    } CATCH_AND_LOG_EXCEPTION(this)
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
    } CATCH_AND_LOG_EXCEPTION(this)
    return "";
}

string chain_proxy::fetch_block_by_id(string& params) {
    try {
        auto block_id = fc::json::from_string(params).as<block_id_type>();
        auto block_ptr = c->fetch_block_by_id(block_id);
        if (!block_ptr) {
            return "";
        }
        auto _raw_block = fc::raw::pack<eosio::chain::signed_block>(*block_ptr);
        return string(_raw_block.data(), _raw_block.size());
    } CATCH_AND_LOG_EXCEPTION(this)
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
    } CATCH_AND_LOG_EXCEPTION(this)
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
    } CATCH_AND_LOG_EXCEPTION(this)
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
    } CATCH_AND_LOG_EXCEPTION(this)
    return false;
}

bool chain_proxy::check_contract_list(string& param) {
    try {
        auto _param = fc::json::from_string(param).as<account_name>();
        c->check_contract_list(_param);
        return true;
    } CATCH_AND_LOG_EXCEPTION(this)
    return false;
}

bool chain_proxy::check_action_list(string& code, string& action) {
    try {
        c->check_action_list(name(code), name(action));
        return true;
    } CATCH_AND_LOG_EXCEPTION(this)
    return false;
}

bool chain_proxy::check_key_list(string& param) {
    try {
        auto _param = fc::json::from_string(param).as<public_key_type>();
        c->check_key_list(_param);
        return true;
    } CATCH_AND_LOG_EXCEPTION(this)
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
    } CATCH_AND_LOG_EXCEPTION(this)
    return false;
}

bool chain_proxy::validate_tapos(string& param) {
    try {
        auto trx = fc::json::from_string(param).as<transaction>();
        c->validate_tapos(trx);
        return true;
    } CATCH_AND_LOG_EXCEPTION(this)
    return false;
}

bool chain_proxy::validate_db_available_size() {
    try {
        c->validate_db_available_size();
        return true;
    } CATCH_AND_LOG_EXCEPTION(this)
    return false;
}

bool chain_proxy::validate_reversible_available_size() {
    try {
        c->validate_reversible_available_size();
        return true;
    } CATCH_AND_LOG_EXCEPTION(this)
    return false;
}

bool chain_proxy::is_protocol_feature_activated(string& param) {
    auto digest = fc::json::from_string(param).as<digest_type>();
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
