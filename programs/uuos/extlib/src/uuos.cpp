#include "uuos.hpp"
#include <dlfcn.h>

using namespace std;

static fn_chain_new s_chain_new = nullptr;
static fn_chain_free s_chain_free = nullptr;

void uuosext_init_chain_api() {
    const char * chain_api_lib = getenv("CHAIN_API_LIB");
    printf("++++chain_api_lib %s\n", chain_api_lib);

    void *handle = dlopen(chain_api_lib, RTLD_LAZY | RTLD_GLOBAL);
    if (handle == 0) {
        printf("loading %s failed! error: %s\n", chain_api_lib, dlerror());
        exit(-1);
        return;
    }

    s_chain_new = (fn_chain_new)dlsym(handle, "chain_new");
    if (s_chain_new == nullptr) {
        printf("++++load chain_new failed! error: %s\n", dlerror());
        exit(-1);
        return;
    }

    s_chain_free = (fn_chain_free)dlsym(handle, "chain_free");
    if (s_chain_free == nullptr) {
        printf("++++load chain_free failed! error: %s\n", dlerror());
        exit(-1);
        return;
    }
}

chain_api* chain_new_(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir) {
    return s_chain_new(config, _genesis, protocol_features_dir, snapshot_dir);
}

void chain_free_(chain_api* api) {
    s_chain_free(api);
}
