#include <chain_api.hpp>

typedef chain_api* (*fn_chain_new)(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
typedef void (*fn_chain_free)(chain_api* api);

chain_api* chain_new_(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
void chain_free_(chain_api* api);

void uuosext_init_chain_api();
#define get_chain_api(ptr) ((chain_api*)ptr)
