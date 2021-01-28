#include <fc/log/logger.hpp>
#include <uuos_proxy.hpp>

uuos_proxy::uuos_proxy() {
}

uuos_proxy::~uuos_proxy() {
}

chain_proxy* uuos_proxy::chain_new(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir) {
    chain_proxy *proxy = new chain_proxy(config, _genesis, protocol_features_dir, snapshot_dir);
    proxy->init();
    return proxy;
}

void uuos_proxy::chain_free(chain_proxy* c) {
   if (c) {
      delete c;
   }
}

void uuos_proxy::set_log_level(string& logger_name, int level) {
    fc::logger::get(logger_name).set_log_level(fc::log_level(level));
}
