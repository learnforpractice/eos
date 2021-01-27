#include "chain_api.hpp"

static fn_chain_new s_chain_new = nullptr;
static fn_chain_free s_chain_free = nullptr;

extern "C" {

chain_api* chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir) {
    return s_chain_new(config, _genesis, protocol_features_dir, snapshot_dir);
}

void chain_free(chain_api* api) {
    s_chain_free(api);
}

void init_chain_api(fn_chain_new _init, fn_chain_free _free) {
    s_chain_new = _init;
    s_chain_free = _free;
}

}
