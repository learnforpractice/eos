#include <eosio/chain_plugin/chain_manager.hpp>
#include <eosio/chain/global_property_object.hpp>

using namespace eosio::chain;

#define CATCH_AND_LOG_EXCEPTION()\
   catch ( const fc::exception& err ) {\
      uuos_on_error(err.dynamic_copy_exception());\
   } catch ( const std::exception& e ) {\
      fc::exception fce( \
         FC_LOG_MESSAGE( warn, "rethrow ${what}: ", ("what",e.what())),\
         fc::std_exception_code,\
         BOOST_CORE_TYPEID(e).name(),\
         e.what() ) ;\
      uuos_on_error(fce.dynamic_copy_exception());\
   } catch( ... ) {\
      fc::unhandled_exception e(\
         FC_LOG_MESSAGE(warn, "rethrow"),\
         std::current_exception());\
      uuos_on_error(e.dynamic_copy_exception());\
   }

static string s_last_error;
static bool s_shutdown = false;

string& uuos_get_last_error_() {
    return s_last_error;
}

void uuos_set_last_error_(string& error) {
    s_last_error = error;
}

void uuos_on_error(const fc::exception_ptr& ex) {
    s_last_error = ex->to_detail_string();
};

namespace eosio {
    protocol_feature_set initialize_protocol_features( const fc::path& p, bool populate_missing_builtins = true );
}

extern "C"
{
   void evm_init();
   void chain_api_init();
   void vm_api_ro_init();
   void vm_api_init();
   void sandboxed_contracts_init();
}

#include <fc/log/logger_config.hpp>

typedef int (*fn_on_accepted_block)(string& packed_block, uint32_t num, string& block_id);
fn_on_accepted_block g_on_accepted_block = nullptr;

void register_on_accepted_block(fn_on_accepted_block cb) {
    g_on_accepted_block = cb;
}

void chain_manager::log_guard_exception(const chain::guard_exception&e ) {
   if (e.code() == chain::database_guard_exception::code_value) {
      elog("Database has reached an unsafe level of usage, shutting down to avoid corrupting the database.  "
           "Please increase the value set for \"chain-state-db-size-mb\" and restart the process!");
   } else if (e.code() == chain::reversible_guard_exception::code_value) {
      elog("Reversible block database has reached an unsafe level of usage, shutting down to avoid corrupting the database.  "
           "Please increase the value set for \"reversible-blocks-db-size-mb\" and restart the process!");
   }

   dlog("Details: ${details}", ("details", e.to_detail_string()));
}

chain_manager::chain_manager() {
}

chain_manager::~chain_manager() {
    if (cc) {
        delete cc;
        cc = nullptr;
    }
}

//string& genesis,
bool chain_manager::init(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir) {
    this->snapshot_dir = snapshot_dir;
    try {
        evm_init();
        vm_api_init();
        vm_api_ro_init();
        chain_api_init();
        sandboxed_contracts_init();
        if (_genesis.size()) {
            genesis = fc::json::from_string(_genesis).as<genesis_state>();
        }
    //    fc::logger::get(DEFAULT_LOGGER).set_log_level(fc::log_level::debug);
        fc::logger::get("producer_plugin").set_log_level(fc::log_level::debug);
        fc::logger::get("transaction_tracing").set_log_level(fc::log_level::debug);

        auto pfs = eosio::initialize_protocol_features( bfs::path(protocol_features_dir) );
        cfg = fc::json::from_string(config).as<eosio::chain::controller::config>();
        ilog("${cfg}", ("cfg", cfg));
        
        chain_id_type chain_id = genesis.compute_chain_id();

        cc = new eosio::chain::controller(cfg, std::move(pfs), chain_id);
        cc->add_indices();
        cc->accepted_block.connect(boost::bind(&chain_manager::on_accepted_block, this, _1));
        cc->accepted_transaction.connect(boost::bind(&chain_manager::on_accepted_transaction, this, _1));

    //     my->chain->accepted_transaction.connect([this]( const transaction_metadata_ptr& meta ) {
    //        my->accepted_transaction_channel.publish( priority::low, meta );
    //     } );
        return true;
    } catch (const database_guard_exception& e) {
        log_guard_exception(e);
        // make sure to properly close the db
    } FC_LOG_AND_DROP();
    delete cc;
    cc = nullptr;
    return false;
}

bool chain_manager::startup(bool init_db) {
    auto shutdown = [](){ return s_shutdown; };
    try {
        if (snapshot_dir.size()) {
            auto infile = std::ifstream(snapshot_dir, (std::ios::in | std::ios::binary));
            auto reader = std::make_shared<istream_snapshot_reader>(infile);
            cc->startup(shutdown, reader);
            infile.close();
        } else if (init_db) {
            cc->startup(shutdown, genesis);
        } else {
            cc->startup(shutdown);
        }
        return true;
    } catch (const database_guard_exception& e) {
        log_guard_exception(e);
        // make sure to properly close the db
    } FC_LOG_AND_DROP();
    delete cc;
    cc = nullptr;
    return false;
}

void chain_manager::on_accepted_transaction( const transaction_metadata_ptr& meta ) {

}

void chain_manager::on_accepted_block(const block_state_ptr& bsp) {
    auto size = fc::raw::pack_size( *bsp->block );
    string packed_block(size, 0);
    fc::datastream<char*> ds( (char *)packed_block.c_str(), packed_block.size() );
    fc::raw::pack(ds, *bsp->block);
    if (g_on_accepted_block) {
//        elog("++++dispatch block to python runtime ${size}", ("size", packed_block.size()));
        string block_id = bsp->block->id().str();
        g_on_accepted_block(packed_block, bsp->block->block_num(), block_id);
    }
}

controller& chain_manager::chain() {
    return *cc;
}

controller::config& chain_manager::config() {
    return cfg;
}

#include <map>
static std::map<void *, chain_manager *> chain_map;

void *chain_new_(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir) {
    chain_manager *manager = new chain_manager();
    if (!manager->init(config, _genesis, protocol_features_dir, snapshot_dir)) {
        delete manager;
        return (void *)0;
    }
    chain_map[manager->cc] = manager;
    return (void *)manager->cc;
}

bool chain_startup_(void* ptr, bool initdb) {
    auto itr = chain_map.find(ptr);
    if (itr == chain_map.end()) {
        return false;
    }
    return ((chain_manager*)itr->second)->startup(initdb);
}

void chain_free_(void *ptr) {
    auto itr = chain_map.find(ptr);
    if (itr == chain_map.end()) {
        return;
    }
    delete (chain_manager*)itr->second;
}

eosio::chain::controller& chain_get_controller(void *ptr) {
    return *(eosio::chain::controller*)ptr;
}

void chain_id_(void *ptr, string& chain_id) {
    try {
        auto& chain = chain_get_controller(ptr);
        chain_id = chain.get_chain_id().str();
    } FC_LOG_AND_DROP();
}

void chain_start_block_(void *ptr, string& _time, uint16_t confirm_block_count, string& _new_features) {
    auto& chain = chain_get_controller(ptr);
    auto time = fc::time_point::from_iso_string(_time);
    if (_new_features.size()) {
        auto new_features = fc::json::from_string(_new_features).as<vector<digest_type>>();
        chain.start_block(block_timestamp_type(time), confirm_block_count, new_features);
    } else {
        chain.start_block(block_timestamp_type(time), confirm_block_count);
    }
}

int chain_abort_block_(void *ptr) {
   try {
        auto& chain = chain_get_controller(ptr);
        chain.abort_block();
        return 1;
   } FC_LOG_AND_DROP();
   return 0;
}

void chain_get_unapplied_transactions_(void *ptr, string& result) {
    // vector<transaction_id_type> values;
    // auto& chain = chain_get_controller(ptr);
    // auto & trxs = chain.get_unapplied_transactions();
    // for (auto& item : trxs) {
    //     values.push_back(item.second->id);
    // }
    // result = fc::json::to_string(fc::variant(values), fc::time_point::maximum());
}

bool chain_pack_action_args_(void *ptr, string& name, string& action, string& _args, vector<char>& result) {
    try {
        auto& chain = chain_get_controller(ptr);
        const auto& accnt = chain.db().get<account_object, by_name>( account_name(name) );
        abi_def abi;
        if( !abi_serializer::to_abi( accnt.abi, abi )) {
            return false;
        }
        auto serializer = abi_serializer( abi, fc::microseconds(150000) );
        auto action_type = serializer.get_action_type(action_name(action));
        EOS_ASSERT(!action_type.empty(), action_validate_exception, "Unknown action ${action}", ("action", action));

        fc::variant args = fc::json::from_string(_args);
        result = serializer.variant_to_binary(action_type, args, fc::microseconds(150000));
        return true;
    } FC_LOG_AND_DROP();
    return false;
}

bool chain_unpack_action_args_(void *ptr, string& name, string& action, string& _binargs, string& result) {
    try {
        auto& chain = chain_get_controller(ptr);
        const auto& accnt = chain.db().get<account_object, by_name>( account_name(name) );
        abi_def abi;
        if( !abi_serializer::to_abi( accnt.abi, abi )) {
            return false;
        }
        auto serializer = abi_serializer( abi, fc::microseconds(150000) );
        auto action_type = serializer.get_action_type(action_name(action));
        EOS_ASSERT(!action_type.empty(), action_validate_exception, "Unknown action ${action}", ("action", action));

        bytes binargs = bytes(_binargs.data(), _binargs.data() + _binargs.size());
        auto v = serializer.binary_to_variant(action_type, binargs, fc::microseconds(150000));
        result = fc::json::to_string(v, fc::time_point::maximum());
        return true;
    } FC_LOG_AND_DROP();
    return false;
}

void chain_gen_transaction_(string& _actions, string& expiration, string& reference_block_id, string& _chain_id, bool compress, std::string& _private_key, vector<char>& result) {
    try {
        signed_transaction trx;
        auto actions = fc::json::from_string(_actions).as<vector<eosio::chain::action>>();
        trx.actions = std::move(actions);
        trx.expiration = fc::time_point::from_iso_string(expiration);
        chain::block_id_type id(reference_block_id);
        trx.set_reference_block(id);
        trx.max_net_usage_words = 0;

        auto priv_key = private_key_type(_private_key);

        chain::chain_id_type chain_id(_chain_id);
        trx.sign(priv_key, chain_id);

        packed_transaction::compression_type type;
        if (compress) {
            type = packed_transaction::compression_type::zlib;
        } else {
            type = packed_transaction::compression_type::none;
        }

        auto packed_trx = packed_transaction(trx, type);
        auto v = fc::raw::pack(packed_trx);
        result = std::move(v);
    } FC_LOG_AND_DROP();
}



bool chain_push_transaction_(void *ptr, string& _packed_trx, string& deadline, uint32_t billed_cpu_time_us, string& result) {
    auto& chain = chain_get_controller(ptr);
    vector<char> packed_trx(_packed_trx.c_str(), _packed_trx.c_str()+_packed_trx.size());
    auto ptrx = std::make_shared<packed_transaction>();
    fc::datastream<const char*> ds( _packed_trx.c_str(), _packed_trx.size() );
    fc::raw::unpack(ds, *ptrx);
    auto ptrx_meta = transaction_metadata::recover_keys(ptrx, chain.get_chain_id());
//    auto ptrx_meta = transaction_metadata::create_no_recover_keys( trx, transaction_metadata::trx_type::input );
    auto _deadline = fc::time_point::from_iso_string(deadline);
    auto ret = chain.push_transaction(ptrx_meta, _deadline, billed_cpu_time_us);
    result = fc::json::to_string(ret, fc::time_point::maximum());
    if (ret->except) {
        return false;
    }
    return true;
}

void chain_push_scheduled_transaction_(void *ptr, string& scheduled_tx_id, string& deadline, uint32_t billed_cpu_time_us, string& result) {
    auto& chain = chain_get_controller(ptr);
    auto id = transaction_id_type(scheduled_tx_id);
    auto _deadline = fc::time_point::from_iso_string(deadline);
    auto ret = chain.push_scheduled_transaction(id, _deadline, billed_cpu_time_us);
    result = fc::json::to_string(ret, fc::time_point::maximum());
}

// void chain_finalize_block_(void *ptr) {
//     auto& chain = chain_get_controller(ptr);
//     chain.finalize_block();
// }

void chain_commit_block_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    chain.commit_block();
}

void chain_finalize_block_(void *ptr, string& _priv_key) {
    auto& chain = chain_get_controller(ptr);
    auto priv_key = private_key_type(_priv_key);
    chain.finalize_block( [&]( const digest_type d ) {
        vector<signature_type> sigs;
        sigs.emplace_back(priv_key.sign(d));
        return sigs;
    } );
}

//void sign_block( const std::function<signature_type( const digest_type& )>& signer_callback );

void chain_pop_block_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
//    chain.pop_block();
}

void chain_get_account_(void *ptr, string& account, string& result) {
    try {
        auto& chain = chain_get_controller(ptr);
        auto ret = chain.get_account(account_name(account));
        result = fc::json::to_string(ret, fc::time_point::maximum());
    } FC_LOG_AND_DROP();
}

void chain_get_global_properties_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    auto& obj = chain.get_global_properties();
    result = fc::json::to_string(obj, fc::time_point::maximum());
}

void chain_get_dynamic_global_properties_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    auto& obj = chain.get_dynamic_global_properties();
    result = fc::json::to_string(obj, fc::time_point::maximum());
}

void chain_get_actor_whitelist_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = fc::json::to_string(chain.get_actor_whitelist(), fc::time_point::maximum());
}

void chain_get_actor_blacklist_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = fc::json::to_string(chain.get_actor_blacklist(), fc::time_point::maximum());
}

void chain_get_contract_whitelist_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = fc::json::to_string(chain.get_contract_whitelist(), fc::time_point::maximum());
}

void chain_get_contract_blacklist_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = fc::json::to_string(chain.get_contract_blacklist(), fc::time_point::maximum());
}

void chain_get_action_blacklist_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = fc::json::to_string(chain.get_action_blacklist(), fc::time_point::maximum());
}

void chain_get_key_blacklist_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = fc::json::to_string(chain.get_key_blacklist(), fc::time_point::maximum());
}

void chain_set_actor_whitelist_(void *ptr, string& params) {
    auto& chain = chain_get_controller(ptr);
    auto whitelist = fc::json::from_string(params).as<flat_set<account_name>>();
    chain.set_actor_whitelist(whitelist);
}

void chain_set_actor_blacklist_(void *ptr, string& params) {
    auto& chain = chain_get_controller(ptr);
    auto whitelist = fc::json::from_string(params).as<flat_set<account_name>>();
    chain.set_actor_blacklist(whitelist);
}

void chain_set_contract_whitelist_(void *ptr, string& params) {
    auto& chain = chain_get_controller(ptr);
    auto whitelist = fc::json::from_string(params).as<flat_set<account_name>>();
    chain.set_contract_whitelist(whitelist);
}

void chain_set_action_blacklist_(void *ptr, string& params) {
    auto& chain = chain_get_controller(ptr);
    auto whitelist = fc::json::from_string(params).as<flat_set< pair<account_name, action_name> >>();
    chain.set_action_blacklist(whitelist);
}

void chain_set_key_blacklist_(void *ptr, string& params) {
    auto& chain = chain_get_controller(ptr);
    auto whitelist = fc::json::from_string(params).as<flat_set<public_key_type>>();
    chain.set_key_blacklist(whitelist);
}

uint32_t chain_head_block_num_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    return chain.head_block_num();
}

void chain_head_block_time_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = chain.head_block_time();
}

void chain_head_block_id_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = chain.head_block_id().str();
}

void chain_head_block_producer_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = chain.head_block_producer().to_string();
}

void chain_head_block_header_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = fc::json::to_string(chain.head_block_header(), fc::time_point::maximum());
}

void chain_head_block_state_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = fc::json::to_string(chain.head_block_state(), fc::time_point::maximum());
}

uint32_t chain_fork_db_head_block_num_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    return chain.fork_db_head_block_num();
}

void chain_fork_db_head_block_id_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = chain.fork_db_head_block_id().str();
}

void chain_fork_db_head_block_time_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = chain.fork_db_head_block_time();
}

void chain_fork_db_head_block_producer_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = chain.fork_db_head_block_producer().to_string();
}

uint32_t chain_fork_db_pending_head_block_num_(void *ptr) {
    try {
        auto& chain = chain_get_controller(ptr);
        return chain.fork_db_pending_head_block_num();
    } FC_LOG_AND_DROP();
    return 0;
}

void chain_fork_db_pending_head_block_id_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = chain.fork_db_pending_head_block_id().str();
}

void chain_fork_db_pending_head_block_time_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = chain.fork_db_pending_head_block_time();
}

void chain_fork_db_pending_head_block_producer_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = chain.fork_db_pending_head_block_producer().to_string();
}

void chain_pending_block_time_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = chain.pending_block_time();
}

void chain_pending_block_producer_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = chain.pending_block_producer().to_string();
}

void chain_pending_block_signing_key_(void *ptr, string& result) {
    try {
        auto& chain = chain_get_controller(ptr);
//        result = std::string(chain.pending_block_signing_key());
    } CATCH_AND_LOG_EXCEPTION()
}

void chain_pending_producer_block_id_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    auto id = chain.pending_producer_block_id();
    if (id) {
        result = id->str();
    }
}

void chain_get_pending_trx_receipts_(void *ptr, string& result) {
    try {
        auto& chain = chain_get_controller(ptr);
        result = fc::json::to_string(chain.get_pending_trx_receipts(), fc::time_point::maximum());
    } CATCH_AND_LOG_EXCEPTION()
}

void chain_active_producers_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = fc::json::to_string(chain.active_producers(), fc::time_point::maximum());
}

void chain_pending_producers_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = fc::json::to_string(chain.pending_producers(), fc::time_point::maximum());
}

void chain_proposed_producers_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = fc::json::to_string(chain.proposed_producers(), fc::time_point::maximum());
}

uint32_t chain_last_irreversible_block_num_(void *ptr) {
    try {
        auto& chain = chain_get_controller(ptr);
        return chain.last_irreversible_block_num();
    } CATCH_AND_LOG_EXCEPTION()
    return 0;
}

void chain_last_irreversible_block_id_(void *ptr, string& result) {
    try {
        auto& chain = chain_get_controller(ptr);
        result = chain.last_irreversible_block_id().str();
    } CATCH_AND_LOG_EXCEPTION()
}

void chain_fetch_block_by_number_(void *ptr, uint32_t block_num, string& raw_block ) {
    try {
        auto& chain = chain_get_controller(ptr);
        auto block_ptr = chain.fetch_block_by_number(block_num);
        if (!block_ptr) {
            return;
        }
        auto _raw_block = fc::raw::pack<eosio::chain::signed_block>(*block_ptr);
        raw_block = string(_raw_block.data(), _raw_block.size());
    } CATCH_AND_LOG_EXCEPTION()
}

void chain_fetch_block_by_id_(void *ptr, string& params, string& raw_block ) {
    try {
        auto& chain = chain_get_controller(ptr);
        auto block_id = fc::json::from_string(params).as<block_id_type>();
        auto block_ptr = chain.fetch_block_by_id(block_id);
        if (!block_ptr) {
            return;
        }
        auto _raw_block = fc::raw::pack<eosio::chain::signed_block>(*block_ptr);
        raw_block = string(_raw_block.data(), _raw_block.size());
    } CATCH_AND_LOG_EXCEPTION()
}

void chain_fetch_block_state_by_number_(void *ptr, uint32_t block_num, string& raw_block_state ) {
    try {
        auto& chain = chain_get_controller(ptr);
        auto block_ptr = chain.fetch_block_state_by_number(block_num);
        if (!block_ptr) {
            return;
        }
        auto _raw_block_state = fc::raw::pack<eosio::chain::block_state>(*block_ptr);
        raw_block_state = string(_raw_block_state.data(), _raw_block_state.size());
    } CATCH_AND_LOG_EXCEPTION()
}

void chain_fetch_block_state_by_id_(void *ptr, string& params, string& raw_block_state ) {
    try {
        auto& chain = chain_get_controller(ptr);
        auto block_id = fc::json::from_string(params).as<block_id_type>();
        auto block_ptr = chain.fetch_block_state_by_id(block_id);
        if (!block_ptr) {
            return;
        }
        auto _raw_block_state = fc::raw::pack<eosio::chain::block_state>(*block_ptr);
        raw_block_state = string(_raw_block_state.data(), _raw_block_state.size());
    } CATCH_AND_LOG_EXCEPTION()
}

void chain_get_block_id_for_num_(void *ptr, uint32_t block_num, string& result ) {
    try {
        auto& chain = chain_get_controller(ptr);
        auto id = chain.get_block_id_for_num(block_num);
        result = id.str();
    } CATCH_AND_LOG_EXCEPTION()
}

void chain_calculate_integrity_hash_(void *ptr, string& result ) {
    auto& chain = chain_get_controller(ptr);
    auto hash = chain.calculate_integrity_hash();
    result = hash.str();
}

/*
         void write_snapshot( const snapshot_writer_ptr& snapshot )const;
*/

bool chain_sender_avoids_whitelist_blacklist_enforcement_(void *ptr, string& sender ) {
    auto& chain = chain_get_controller(ptr);
    return chain.sender_avoids_whitelist_blacklist_enforcement(account_name(sender));
}

bool chain_check_actor_list_(void *ptr, string& param, string& err) {
    try {
        auto& chain = chain_get_controller(ptr);
        auto _param = fc::json::from_string(param).as<flat_set<account_name>>();
        chain.check_actor_list(_param);
        return true;
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_check_contract_list_(void *ptr, string& param, string& err) {
    try {
        auto& chain = chain_get_controller(ptr);
        auto _param = fc::json::from_string(param).as<account_name>();
        chain.check_contract_list(_param);
        return true;
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_check_action_list_(void *ptr, string& code, string& action, string& err) {
    try {
        auto& chain = chain_get_controller(ptr);
        chain.check_action_list(name(code), name(action));
        return true;
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_check_key_list_(void *ptr, string& param, string& err) {
    try {
        auto& chain = chain_get_controller(ptr);
        auto _param = fc::json::from_string(param).as<public_key_type>();
        chain.check_key_list(_param);
        return true;
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_is_building_block_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    return chain.is_building_block();
}

bool chain_is_producing_block_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    return chain.is_producing_block();
}

bool chain_is_ram_billing_in_notify_allowed_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    return chain.is_ram_billing_in_notify_allowed();
}

void chain_add_resource_greylist_(void *ptr, string& param) {
    auto& chain = chain_get_controller(ptr);
    chain.add_resource_greylist(name(param));
}

void chain_remove_resource_greylist_(void *ptr, string& param) {
    auto& chain = chain_get_controller(ptr);
    chain.remove_resource_greylist(name(param));
}

bool chain_is_resource_greylisted_(void *ptr, string& param) {
    auto& chain = chain_get_controller(ptr);
    return chain.is_resource_greylisted(name(param));
}

void chain_get_resource_greylist_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result =fc::json::to_string(chain.get_resource_greylist(), fc::time_point::maximum());
}

void chain_get_config_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result =fc::json::to_string(chain.get_config(), fc::time_point::maximum());
}

bool chain_validate_expiration_(void *ptr, string& param, string& err) {
    try {
        auto& chain = chain_get_controller(ptr);
        auto trx = fc::json::from_string(param).as<transaction>();
        chain.validate_expiration(trx);
        return true;
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_validate_tapos_(void *ptr, string& param, string& err) {
    try {
        auto& chain = chain_get_controller(ptr);
        auto trx = fc::json::from_string(param).as<transaction>();
        chain.validate_tapos(trx);
        return true;
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_validate_db_available_size_(void *ptr, string& err) {
    try {
        auto& chain = chain_get_controller(ptr);
        chain.validate_db_available_size();
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_validate_reversible_available_size_(void *ptr, string& err) {
    try {
        auto& chain = chain_get_controller(ptr);
        chain.validate_reversible_available_size();
    } CATCH_AND_LOG_EXCEPTION()
    return false;
}

bool chain_is_protocol_feature_activated_(void *ptr, string& param) {
    auto& chain = chain_get_controller(ptr);
    auto digest = fc::json::from_string(param).as<digest_type>();
    return chain.is_protocol_feature_activated(digest);
}

bool chain_is_builtin_activated_(void *ptr, int feature) {
    auto& chain = chain_get_controller(ptr);
    return chain.is_builtin_activated((builtin_protocol_feature_t)feature);
}

bool chain_is_known_unexpired_transaction_(void *ptr, string& param) {
    auto& chain = chain_get_controller(ptr);
    auto id = fc::json::from_string(param).as<transaction_id_type>();
    return chain.is_known_unexpired_transaction(id);
}

int64_t chain_set_proposed_producers_(void *ptr, string& param) {
    auto& chain = chain_get_controller(ptr);
    auto id = fc::json::from_string(param).as<vector<producer_authority>>();
    return chain.set_proposed_producers(id);
}

bool chain_light_validation_allowed_(void *ptr, bool replay_opts_disabled_by_policy) {
    auto& chain = chain_get_controller(ptr);
    return chain.light_validation_allowed(replay_opts_disabled_by_policy);
}

bool chain_skip_auth_check_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    return chain.skip_auth_check();
}

bool chain_skip_db_sessions_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    return chain.skip_db_sessions();
}

bool chain_skip_trx_checks_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    return chain.skip_trx_checks();
}

bool chain_contracts_console_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    return chain.contracts_console();
}

bool chain_is_uuos_mainnet_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    return chain.is_uuos_mainnet();
}

void chain_get_chain_id_(void *ptr, string& result) {
    auto& chain = chain_get_controller(ptr);
    result = chain.get_chain_id().str();
}

int chain_get_read_mode_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    return (int)chain.get_read_mode();
}

int chain_get_validation_mode_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    return (int)chain.get_validation_mode();
}

void chain_set_subjective_cpu_leeway_(void *ptr, uint64_t leeway) {
    auto& chain = chain_get_controller(ptr);
    chain.set_subjective_cpu_leeway(fc::microseconds(leeway));
}

void chain_set_greylist_limit_(void *ptr, uint32_t limit) {
    auto& chain = chain_get_controller(ptr);
    chain.set_greylist_limit(limit);
}

uint32_t chain_get_greylist_limit_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    return chain.get_greylist_limit();
}

void chain_add_to_ram_correction_(void *ptr, string& account, uint64_t ram_bytes) {
    auto& chain = chain_get_controller(ptr);
    chain.add_to_ram_correction(account_name(account), ram_bytes);
}

bool chain_all_subjective_mitigations_disabled_(void *ptr) {
    auto& chain = chain_get_controller(ptr);
    return chain.all_subjective_mitigations_disabled();
}

void chain_get_scheduled_producer_(void *ptr, string& _block_time, string& result) {
    auto& chain = chain_get_controller(ptr);
    auto block_time = fc::time_point::from_iso_string(_block_time);
    auto producer = chain.head_block_state()->get_scheduled_producer(block_time);
    result = fc::json::to_string(producer, fc::time_point::maximum());
}

/*
         bool skip_db_sessions( block_status bs )const;
*/
/*
         void set_subjective_cpu_leeway(fc::microseconds leeway);
         void set_greylist_limit( uint32_t limit );
         uint32_t get_greylist_limit()const;

         void add_to_ram_correction( account_name account, uint64_t ram_bytes );
         bool all_subjective_mitigations_disabled()const;
*/

void uuos_recover_key_( string& _digest, string& _sig, string& _pub ) {
   try {
//      ilog("+++++${n}", ("n", _sig));
      auto digest = fc::sha256(_digest);
      auto s = fc::crypto::signature(_sig);
      _pub = string(fc::crypto::public_key( s, digest, false ));
   } FC_LOG_AND_DROP();
}

uint64_t uuos_current_time_nano_() {
   auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
   return uint64_t(ns);
}

void uuos_sign_digest_(string& _priv_key, string& _digest, string& out) {
    try {
        chain::private_key_type priv_key(_priv_key);
        chain::digest_type digest(_digest.c_str(), _digest.size());
        auto sign = priv_key.sign(digest);
        out = string(sign);
    } FC_LOG_AND_DROP();
}


void uuos_set_log_level_(string& logger_name, int level) {
//fc::logger::get(DEFAULT_LOGGER).set_log_level(fc::log_level::debug);
    fc::logger::get(logger_name).set_log_level(fc::log_level(level));
}

void uuos_shutdown_() {
    s_shutdown = true;
}
