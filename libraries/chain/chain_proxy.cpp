#include <eosio/chain/controller.hpp>
#include <eosio/chain/wasm_interface.hpp>
#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/transaction_context.hpp>
#include <eosio/chain/chain_proxy.hpp>

#include <dlfcn.h>
#include <vm_api.h>

extern "C" void* eos_vm_interface_init(int type, bool tierup, eosio::chain::chain_proxy& api);
extern "C" void eos_vm_interface_apply(void* interface, const eosio::chain::digest_type& code_hash, const uint8_t vm_type, const uint8_t vm_version, eosio::chain::apply_context& context);

namespace eosio { namespace chain {

typedef wasm_interface* (*fn_eos_vm_interface_init)(int type, bool tierup, eosio::chain::chain_proxy& api);


chain_proxy::chain_proxy(const controller::config& cfg, chainbase::database& db, controller& ctrl) : conf(cfg), _db(db), c(ctrl) {
//"/Users/newworld/dev/uuos3/build/libraries/chain/libeosio_chain_shared.dylib";
#if 0
    const char *vm_eos_path = getenv("CHAIN_LIB");
    const char *vm_eos_path2 = getenv("CHAIN_LIB2");;
     printf("%s\n", vm_eos_path);
     printf("%s\n", vm_eos_path2);

    void *handle = dlopen(vm_eos_path, RTLD_LAZY | RTLD_LOCAL);
    if (!handle) {
        printf("+++error:%s\n", dlerror());
    }

    EOS_ASSERT(handle, assert_exception, "load vm_eos lib failed!");
    fn_eos_vm_interface_init init = (fn_eos_vm_interface_init)dlsym(handle, "eos_vm_interface_init3");
    EOS_ASSERT(init, assert_exception, "load eos_vm_interface_init failed!");
    this->eos_vm_interface = init((int)conf.wasm_runtime, false, *this);
    printf("++++++++=init %p\n", init);

    handle = dlopen(vm_eos_path2, RTLD_LAZY | RTLD_LOCAL);
    if (!handle) {
        printf("+++error:%s\n", dlerror());
    }
    vmilog("+++++++handle %p\n", handle);

    EOS_ASSERT(handle, assert_exception, "load vm_eos lib failed2!");
    init = (fn_eos_vm_interface_init)dlsym(handle, "eos_vm_interface_init3");
    vmilog("++++++++=init %p\n", init);
    EOS_ASSERT(init, assert_exception, "load eos_vm_interface_init failed!");

    #ifdef EOSIO_EOS_VM_OC_RUNTIME_ENABLED
        this->eos_vm_micropython = init((int)conf.wasm_runtime, conf.eosvmoc_tierup, *this);
    #else
        this->eos_vm_micropython = init((int)conf.wasm_runtime, false, *this);
    #endif
#else
    this->eos_vm_interface = new wasm_interface( cfg.wasm_runtime, false, db, cfg.state_dir, cfg.eosvmoc_config, *this );
    #ifdef EOSIO_EOS_VM_OC_RUNTIME_ENABLED
        this->eos_vm_micropython = new wasm_interface( cfg.wasm_runtime, cfg.eosvmoc_tierup, db, cfg.state_dir, cfg.eosvmoc_config, *this );
    #else
        this->eos_vm_micropython = new wasm_interface( cfg.wasm_runtime, false, db, cfg.state_dir, cfg.eosvmoc_config, *this );
    #endif
#endif
}

void chain_proxy::set_context(apply_context* ctx) {
    this->ctx = ctx;
}

apply_context& chain_proxy::get_context() {
    get_vm_api()->eosio_assert(this->ctx != nullptr, "apply_context can not be null");
    return *this->ctx;
}

const chainbase::database& chain_proxy::db() {
    return _db;
    return this->c.db();
}

const boost::filesystem::path chain_proxy::state_dir() {
    return this->conf.state_dir;
}

bool chain_proxy::get_code_by_code_hash(const digest_type& code_hash, const uint8_t vm_type, const uint8_t vm_version, const char** code, size_t* size, uint32_t* first_block_used) {
   const code_object& code_entry = this->c.db().get<code_object, by_code_hash>(boost::make_tuple(code_hash, vm_type, vm_version));
   *code = code_entry.code.data();
   *size = code_entry.code.size();
   *first_block_used = code_entry.first_block_used;
   return true;
}

void chain_proxy::eos_vm_interface_apply(const digest_type& code_hash, const uint8_t vm_type, const uint8_t vm_version, eosio::chain::apply_context& context) {
    this->eos_vm_interface->apply(code_hash, vm_type, vm_version, context);
}

void chain_proxy::eos_vm_micropython_apply(const digest_type& code_hash, const uint8_t vm_type, const uint8_t vm_version, eosio::chain::apply_context& context) {
    this->eos_vm_micropython->apply(code_hash, vm_type, vm_version, context);
}

bool chain_proxy::contracts_console() {
    return this->c.contracts_console();
}

} } //eosio::chain