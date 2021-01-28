#include <eosio/chain/controller.hpp>
#include <eosio/chain/chain_manager.hpp>

#include <fc/io/json.hpp>

#include <chain_proxy.hpp>

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







static chain_proxy *_chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir) {
   chain_proxy *proxy = new chain_proxy(config, _genesis, protocol_features_dir, snapshot_dir);
   try {
       proxy->init();
   } CATCH_AND_LOG_EXCEPTION(proxy);
   return proxy;
}

static void _chain_free(chain_proxy *c) {
   if (c) {
      delete c;
   }
}

extern "C" void uuos_init_chain_api() {
    const char *chain_api_lib = getenv("CHAIN_API_LIB");
    void *handle = dlopen(chain_api_lib, RTLD_LAZY | RTLD_GLOBAL);
    if (handle == 0) {
        printf("loading %s failed! error: %s\n", chain_api_lib, dlerror());
        exit(-1);
        return;
    }

    fn_init_chain_api init_chain_api = (fn_init_chain_api)dlsym(handle, "init_chain_api");
    if (!init_chain_api) {
        printf("loading init_chain_api failed! error: %s\n", dlerror());
        exit(-1);
        return;
    }
    init_chain_api(_chain_new, _chain_free);
}
