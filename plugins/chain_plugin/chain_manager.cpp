#include <eosio/chain_plugin/chain_manager.hpp>

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

chain_manager *chain_manager::instance = nullptr;
static chain_manager *g_manager = nullptr;

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

chain_manager::chain_manager(string& config, string& protocol_features_dir, string& snapshot_dir) {
    evm_init();
    vm_api_init();
    vm_api_ro_init();
    chain_api_init();
    sandboxed_contracts_init();

//    fc::logger::get(DEFAULT_LOGGER).set_log_level(fc::log_level::debug);
    fc::logger::get("producer_plugin").set_log_level(fc::log_level::debug);
    fc::logger::get("transaction_tracing").set_log_level(fc::log_level::debug);

    auto pfs = eosio::initialize_protocol_features( bfs::path(protocol_features_dir) );
    cfg = fc::json::from_string(config).as<eosio::chain::controller::config>();
    ilog("${cfg}", ("cfg", cfg));

    cc = new eosio::chain::controller(cfg, std::move(pfs));
    cc->add_indices();
    cc->accepted_block.connect(boost::bind(&chain_manager::on_accepted_block, this, _1));
    cc->accepted_transaction.connect(boost::bind(&chain_manager::on_accepted_transaction, this, _1));



//     my->chain->accepted_transaction.connect([this]( const transaction_metadata_ptr& meta ) {
//        my->accepted_transaction_channel.publish( priority::low, meta );
//     } );

    auto shutdown = [](){ return false; };

    try {
        if (snapshot_dir.size()) {
            auto infile = std::ifstream(snapshot_dir, (std::ios::in | std::ios::binary));
            auto reader = std::make_shared<istream_snapshot_reader>(infile);
            cc->startup(shutdown, reader);
            infile.close();
        } else {
            cc->startup(shutdown);
        }
    } catch (const database_guard_exception& e) {
        log_guard_exception(e);
        // make sure to properly close the db
        delete cc;
    }
}

chain_manager *chain_manager::init(string& config, string& protocol_features_dir, string& snapshot_dir) {
    if (!instance) {
        instance = new chain_manager(config, protocol_features_dir, snapshot_dir);
    }
    return instance;
}

chain_manager& chain_manager::get() {
    return *instance;
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

chain_manager::~chain_manager() {
    delete cc;
}

void *chain_new_(string& config, string& protocol_features_dir, string& snapshot_dir) {
//    auto plugin = eosio::chain_plugin();
/*
    auto shutdown = [](){ return app().is_quiting(); };
    if (my->snapshot_path) {
        auto infile = std::ifstream(my->snapshot_path->generic_string(), (std::ios::in | std::ios::binary));
        auto reader = std::make_shared<istream_snapshot_reader>(infile);
        my->chain->startup(shutdown, reader);
        infile.close();
    } else {
        my->chain->startup(shutdown);
    }
*/
    g_manager = chain_manager::init(config, protocol_features_dir, snapshot_dir);
    return (void *)g_manager->cc;
}

void chain_free_(void *ptr) {
//    g_manager
    if (!ptr) {
        return;
    }
    delete g_manager;
}

uint32_t chain_fork_db_pending_head_block_num_(void *ptr) {
    try {
        auto& chain = *(eosio::chain::controller*)(ptr);
        return chain.fork_db_pending_head_block_num();
    } FC_LOG_AND_DROP();
    return 0;
}

uint32_t chain_last_irreversible_block_num_(void *ptr) {
    try {
        auto& chain = *(eosio::chain::controller*)(ptr);
        return chain.last_irreversible_block_num();
    } FC_LOG_AND_DROP();
    return 0;
}

void chain_get_block_id_for_num_(void *ptr, uint32_t num, string& block_id) {
    try {
        auto& chain = *(eosio::chain::controller*)(ptr);
        block_id = chain.get_block_id_for_num(num).str();
    } FC_LOG_AND_DROP();
}

void chain_id_(void *ptr, string& chain_id) {
    try {
        auto& chain = *(eosio::chain::controller*)(ptr);
        chain_id = chain.get_chain_id().str();
    } FC_LOG_AND_DROP();
}

void chain_fetch_block_by_number_(void *ptr, uint32_t block_num, string& raw_block ) {
    try {
        auto& chain = *(eosio::chain::controller*)(ptr);
        auto block_ptr = chain.fetch_block_by_number(block_num);
        if (!block_ptr) {
            return;
        }
        auto _raw_block = fc::raw::pack<eosio::chain::signed_block>(*block_ptr);
        raw_block = string(_raw_block.data(), _raw_block.size());
    } FC_LOG_AND_DROP();
}

int chain_is_building_block_(void *ptr) {
    auto& chain = *(eosio::chain::controller*)(ptr);
    return chain.is_building_block();
}

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

