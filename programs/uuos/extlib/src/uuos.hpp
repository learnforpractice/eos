#include <chain_proxy.hpp>

typedef chain_proxy* (*fn_chain_new)(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
typedef void (*fn_chain_free)(chain_proxy* api);

chain_proxy* chain_new_(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
void chain_free_(chain_proxy* api);

void uuosext_init_chain_api();
#define chain(ptr) ((chain_proxy*)ptr)
