#include <eosio/chain/wasm_interface.hpp>
#include <eosio/chain/chain_proxy.hpp>

using namespace eosio::chain;
using namespace eosio::chain::webassembly::common;

//libraries/chain/webassembly/eos-vm-oc/compile_monitor.cpp
extern "C" void start_compile_monitor();

extern "C" {
   eosio::chain::wasm_interface* eos_vm_interface_init2(int vmtype, bool tierup, eosio::chain::chain_proxy& api) {
#ifdef EOSIO_EOS_VM_OC_RUNTIME_ENABLED
      if (tierup) {
         static bool init = false;
         if (!init) {
            init = true;
            start_compile_monitor();
         }
      }
#endif
//      bool tierup = api.conf.eosvmoc_tierup;
      wasm_interface* interface = new wasm_interface((wasm_interface::vm_type)vmtype, tierup, api.db(), api.state_dir(), api.conf.eosvmoc_config, api);
//      wasm_interface* interface = new wasm_interface(wasm_interface::vm_type::wabt, tierup, api.db(), api.state_dir(), api.conf.eosvmoc_config, api);
      return interface;
   }
}
