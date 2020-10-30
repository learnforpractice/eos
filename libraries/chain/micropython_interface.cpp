#include <eosio/chain/micropython_interface.hpp>
#include <eosio/chain/apply_context.hpp>
#include <vm_api/vm_api.h>


using namespace eosio::chain;

extern "C" {

}


micropython_instantiated_module::micropython_instantiated_module()
{

}

void micropython_instantiated_module::apply(apply_context& context) {
    
}

void micropython_instantiated_module::call(uint64_t func_name, uint64_t arg1, uint64_t arg2, uint64_t arg3, apply_context& context) {
    EOS_ASSERT( false, wasm_execution_error, "call not implemented in python vm" );
}

void micropython_instantiated_module::take_snapshoot() {

}


micropython_runtime::micropython_runtime() {}

std::unique_ptr<micropython_instantiated_module> micropython_runtime::instantiate_module(const char* code_bytes, size_t code_size, std::vector<uint8_t> initial_memory,
                                                                                     const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version) {
   return std::make_unique<micropython_instantiated_module>();
}

void micropython_runtime::immediately_exit_currently_running_module() {
   throw wasm_exit();
}

micropython_interface::micropython_interface(const chainbase::database& d): db(d) {
    runtime_interface = std::make_unique<micropython_runtime>();
}

micropython_interface::~micropython_interface() {}


//call before dtor to skip what can be minutes of dtor overhead with some runtimes; can cause leaks
void micropython_interface::indicate_shutting_down() {

}

void micropython_interface::validate(const controller& control, const bytes& code) {
    
}

//indicate that a particular code probably won't be used after given block_num
void micropython_interface::code_block_num_last_used(const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version, const uint32_t& block_num) {
    python_cache_index::iterator it = micropython_instantiation_cache.find(boost::make_tuple(code_hash, vm_type, vm_version));
    if(it != micropython_instantiation_cache.end()) {
        micropython_instantiation_cache.modify(it, [block_num](micropython_cache_entry& e) {
            e.last_block_num_used = block_num;
        });
    }
}

//indicate the current LIB. evicts old cache entries
void micropython_interface::current_lib(const uint32_t lib) {
    //anything last used before or on the LIB can be evicted
    const auto first_it = micropython_instantiation_cache.get<by_last_block_num>().begin();
    const auto last_it  = micropython_instantiation_cache.get<by_last_block_num>().upper_bound(lib);
    micropython_instantiation_cache.get<by_last_block_num>().erase(first_it, last_it);
}

static uint64_t get_microseconds() {
   struct timeval  tv;
   gettimeofday(&tv, NULL);
   return tv.tv_sec * 1000000LL + tv.tv_usec * 1LL ;
}

//Calls apply or error on a given code
void micropython_interface::apply(const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version, apply_context& context) {
    uint16_t version = ((uint16_t)vm_type<<8) | (uint16_t)vm_version;
    get_instantiated_module(code_hash, vm_type, vm_version, context)->apply(context);
}

void micropython_interface::call(uint64_t contract, uint64_t func_name, uint64_t arg1, uint64_t arg2, uint64_t arg3, apply_context& context ) {
    EOS_ASSERT( false, wasm_execution_error, "call not implemented in python vm" );
}

//Immediately exits currently running wasm. UB is called when no wasm running
void micropython_interface::exit() {

}

const std::unique_ptr<micropython_instantiated_module>& micropython_interface::get_instantiated_module( const digest_type& code_hash, const uint8_t& vm_type,
                                                                            const uint8_t& vm_version, apply_context& context )
{
    uint16_t version = ((uint16_t)vm_type<<8) | (uint16_t)vm_version;

    python_cache_index::iterator it = micropython_instantiation_cache.find(
                                        boost::make_tuple(code_hash, vm_type, vm_version) );
    const code_object* codeobject = nullptr;
    if(it == micropython_instantiation_cache.end()) {
        codeobject = &db.get<code_object,by_code_hash>(boost::make_tuple(code_hash, vm_type, vm_version));

        it = micropython_instantiation_cache.emplace( micropython_cache_entry{
                                                    .code_hash = code_hash,
                                                    .first_block_num_used = codeobject->first_block_used,
                                                    .last_block_num_used = UINT32_MAX,
                                                    .module = nullptr,
                                                    .vm_type = vm_type,
                                                    .vm_version = vm_version
                                                } ).first;
    }

    if(!it->module) {
        if(!codeobject)
            codeobject = &db.get<code_object,by_code_hash>(boost::make_tuple(code_hash, vm_type, vm_version));
#if 0
        auto timer_pause = fc::make_scoped_exit([&](){
            context.trx_context.resume_billing_timer();
        });
        context.trx_context.pause_billing_timer();
#endif
        vector<char> bytes;
        micropython_instantiation_cache.modify(it, [&](auto& c) {
            c.module = runtime_interface->instantiate_module((const char*)bytes.data(), bytes.size(), {}, code_hash, vm_type, vm_version);

            get_vm_api()->allow_access_apply_context = false;
//
            get_vm_api()->allow_access_apply_context = true;

            c.module->take_snapshoot();
        });
    }
    return it->module;
}

