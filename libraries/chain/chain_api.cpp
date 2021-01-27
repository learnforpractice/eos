#include <eosio/chain/controller.hpp>
#include <eosio/chain/chain_manager.hpp>
#include <chain_api.hpp>

#include <dlfcn.h>

chain_api::chain_api(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir) {
    c = std::make_unique<eosio::chain::chain_manager>(config, _genesis, protocol_features_dir, snapshot_dir);
}

chain_api::~chain_api() {

}

extern "C" chain_api *chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir) {
   return new chain_api(config, _genesis, protocol_features_dir, snapshot_dir);
}

extern "C" void chain_free(chain_api *c) {
   if (c) {
      delete c;
   }
}

void uuos_init_chain_api(fn_chain_new _init, fn_chain_free _free) {
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
    init_chain_api(_init, _free);
}
