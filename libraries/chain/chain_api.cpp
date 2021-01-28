#include <eosio/chain/controller.hpp>
#include <eosio/chain/chain_manager.hpp>

#include <fc/io/json.hpp>

#include <chain_api.hpp>

#include <dlfcn.h>
using namespace eosio::chain;

#define CATCH_AND_LOG_EXCEPTION()\
   catch ( const fc::exception& e ) {\
      string ex = fc::json::to_string(*e.dynamic_copy_exception(), fc::time_point::maximum()); \
      this->set_last_error(ex);\
   } catch ( const std::exception& e ) {\
      fc::exception fce( \
         FC_LOG_MESSAGE( warn, "rethrow ${what}: ", ("what",e.what())),\
         fc::std_exception_code,\
         BOOST_CORE_TYPEID(e).name(),\
         e.what() ) ;\
        string ex = fc::json::to_string(*fce.dynamic_copy_exception(), fc::time_point::maximum()); \
        this->set_last_error(ex);\
   } catch( ... ) {\
      fc::unhandled_exception e(\
         FC_LOG_MESSAGE(warn, "rethrow"),\
         std::current_exception());\
        string ex = fc::json::to_string(*e.dynamic_copy_exception(), fc::time_point::maximum()); \
        this->set_last_error(ex);\
   }

#define CATCH_AND_LOG_EXCEPTION_AND_RETURN_FALSE() \
   catch ( const fc::exception& e ) {\
      string ex = fc::json::to_string(*e.dynamic_copy_exception(), fc::time_point::maximum()); \
      this->set_last_error(ex);\
      return false; \
   } catch ( const std::exception& e ) {\
      fc::exception fce( \
         FC_LOG_MESSAGE( warn, "rethrow ${what}: ", ("what",e.what())),\
         fc::std_exception_code,\
         BOOST_CORE_TYPEID(e).name(),\
         e.what() ) ;\
        string ex = fc::json::to_string(*fce.dynamic_copy_exception(), fc::time_point::maximum()); \
        this->set_last_error(ex);\
        return false; \
   } catch( ... ) {\
      fc::unhandled_exception e(\
         FC_LOG_MESSAGE(warn, "rethrow"),\
         std::current_exception());\
        string ex = fc::json::to_string(*e.dynamic_copy_exception(), fc::time_point::maximum()); \
        this->set_last_error(ex);\
        return false; \
   }

string& chain_api::get_last_error() {
    return last_error;
}

void chain_api::set_last_error(string& error) {
    last_error = error;
}

chain_api::chain_api(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir) :
    cm(new chain_manager(config, _genesis, protocol_features_dir, snapshot_dir)),
    c(*cm->c)
{
}

chain_api::~chain_api() {

}

void chain_api::say_hello() {
    printf("hello,world from chain_api\n");
}

void chain_api::id(string& chain_id) {
    try {
        chain_id = c.get_chain_id().str();
    } CATCH_AND_LOG_EXCEPTION();
}

void chain_api::start_block(string& _time, uint16_t confirm_block_count, string& _new_features) {
    auto time = fc::time_point::from_iso_string(_time);
    try {
        if (_new_features.size()) {
            auto new_features = fc::json::from_string(_new_features).as<vector<digest_type>>();
            c.start_block(block_timestamp_type(time), confirm_block_count, new_features);
        } else {
            c.start_block(block_timestamp_type(time), confirm_block_count);
        }
    } CATCH_AND_LOG_EXCEPTION();
}

int chain_api::abort_block() {
   try {
        c.abort_block();
        return 1;
   } CATCH_AND_LOG_EXCEPTION();
   return 0;
}


static chain_api *_chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir) {
   return new chain_api(config, _genesis, protocol_features_dir, snapshot_dir);
}

static void _chain_free(chain_api *c) {
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
