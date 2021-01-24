#include <eosio/chain/controller.hpp>
#include <eosio/chain/chain_api.hpp>

extern "C" void eos_vm_interface_init(int type, eosio::chain::chain_api& api);

namespace eosio { namespace chain {

chain_api::chain_api(const controller::config& conf, controller& ctrl) : conf(conf), c(ctrl) {
    eos_vm_interface_init((int)conf.wasm_runtime, *this);
}

const chainbase::database& chain_api::db() {
    return this->c.db();
}

const boost::filesystem::path chain_api::state_dir() {
    return this->conf.state_dir;
}

bool chain_api::get_code_by_code_hash(const digest_type& code_hash, const uint8_t vm_type, const uint8_t vm_version, const char** code, size_t* size, uint32_t* first_block_used) {
   const code_object& code_entry = this->c.db().get<code_object, by_code_hash>(boost::make_tuple(code_hash, vm_type, vm_version));
   *code = code_entry.code.data();
   *size = code_entry.code.size();
   *first_block_used = code_entry.first_block_used;
   return true;
}

#if defined(EOSIO_EOS_VM_RUNTIME_ENABLED) || defined(EOSIO_EOS_VM_JIT_RUNTIME_ENABLED)
vm::wasm_allocator& chain_api::get_wasm_allocator() {
   return this->c.get_wasm_allocator();
}
#endif

} } //eosio::chain
