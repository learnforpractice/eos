#include <eosio/chain/python_interface.hpp>
#include <eosio/chain/apply_context.hpp>

#include <eosio/chain/python_vm/vm_memory.h>
#include <vm_python.h>
#include <eosiolib_native/vm_api.h>
#include <python_vm_config.h>

using namespace pythonvm;
using namespace eosio::chain;

extern "C" {
    int vm_python2_apply(uint64_t receiver, uint64_t account, uint64_t act);
    void vm_python2_init(uint8_t *vm_memory_start, size_t vm_memory_size, fn_vm_load_memory load_memory);

    void wasm2c_python_vm_apply(uint64_t receiver, uint64_t code, uint64_t action);
    void wasm2c_python_vm_call(uint64_t func_name, uint64_t receiver, uint64_t code, uint64_t action);
}

python_instantiated_module::python_instantiated_module()
{

}

void python_instantiated_module::apply(apply_context& context) {
    auto receiver = context.get_receiver().to_uint64_t();
    auto account = context.get_action().account.to_uint64_t();
    auto act = context.get_action().name.to_uint64_t();

    wasm2c_python_vm_apply(receiver, account, act);
}

void python_instantiated_module::call(uint64_t func_name, uint64_t arg1, uint64_t arg2, uint64_t arg3, apply_context& context) {
    EOS_ASSERT( false, wasm_execution_error, "call not implemented in python vm" );
}

void python_instantiated_module::take_snapshoot(vm_memory& _vm_memory) {
    char *mem_start = _vm_memory.data.data();
    uint32_t vm_memory_size = _vm_memory.data.size();

    int total_count = 0;

    int pos = PYTHON_VM_STACK_SIZE/sizeof(uint64_t);
    uint64_t *ptr1 = (uint64_t *)_vm_memory.data_backup.data();
    uint64_t *ptr2 = (uint64_t *)mem_start;

    int contract_mem_start = 0;
    int contract_mem_end = 0;
    get_vm_api()->get_copy_memory_range(&contract_mem_start, &contract_mem_end);

    //   vmdlog("++++contract_mem_start %d, contract_mem_end %d, vm_memory_size %d\n", contract_mem_start, contract_mem_end, vm_memory_size);
    get_vm_api()->eosio_assert(contract_mem_start > 0 && contract_mem_start<vm_memory_size, "bad start contract memory");
    get_vm_api()->eosio_assert(contract_mem_end > 0 && contract_mem_end<vm_memory_size, "bad end contract memory");
    get_vm_api()->eosio_assert(contract_mem_start < contract_mem_end, "bad memory range");

    contract_mem_start = contract_mem_start/8*8;
    contract_mem_end = (contract_mem_end+7)/8*8;

    vm_memory_size = contract_mem_start;
    vm_memory_size/=sizeof(uint64_t);

    //save diff memory
    while(pos<vm_memory_size) {
        if (ptr1[pos] == ptr2[pos]) {
            pos += 1;
            continue;
        }
        int start = pos;
        total_count += 1;
        pos += 1;
        while (pos<vm_memory_size && ptr1[pos] != ptr2[pos]) {
            pos += 1;
            total_count += 1;
        }
        memory_segment segment;
        segment.offset = start*sizeof(uint64_t);
        segment.data.resize((pos-start)*sizeof(uint64_t));
        memcpy(segment.data.data(), &ptr2[start], (pos-start)*sizeof(uint64_t));
        backup.memory_backup.emplace_back(std::move(segment));
        pos += 1;
    }

    memory_segment segment;
    segment.offset = contract_mem_start;
    segment.data.resize(contract_mem_end-contract_mem_start, 0x00);
    memcpy(segment.data.data(), (char *)ptr2 + contract_mem_start, contract_mem_end-contract_mem_start);
    backup.memory_backup.emplace_back(std::move(segment));

    backup.contract_memory_start = contract_mem_start;
    backup.contract_memory_end = contract_mem_end;
}


python_runtime::python_runtime() {}

std::unique_ptr<python_instantiated_module> python_runtime::instantiate_module(const char* code_bytes, size_t code_size, std::vector<uint8_t> initial_memory,
                                                                                     const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version) {
   return std::make_unique<python_instantiated_module>();
}

void python_runtime::immediately_exit_currently_running_module() {
   throw wasm_exit();
}

static struct vm_memory *g_vm_memory = nullptr;

static void vm_load_memory(uint32_t offset_start, uint32_t length) {
    g_vm_memory->load_data_to_writable_memory(offset_start, length);
}

python_interface::python_interface(const chainbase::database& d): db(d) {
    runtime_interface = std::make_unique<python_runtime>();
    _vm_memory = std::make_unique<vm_memory>(PYTHON_VM_PAGES, PYTHON_VM_PAGES);
    g_vm_memory = _vm_memory.get();

    vm_python2_init((uint8_t *)_vm_memory->data.data(), _vm_memory->data.size(), vm_load_memory);
    
    _vm_memory->backup_memory();
    _vm_memory->init_cache();
}

python_interface::~python_interface() {}


//call before dtor to skip what can be minutes of dtor overhead with some runtimes; can cause leaks
void python_interface::indicate_shutting_down() {

}

void python_interface::validate(const controller& control, const bytes& code) {
    
}

//indicate that a particular code probably won't be used after given block_num
void python_interface::code_block_num_last_used(const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version, const uint32_t& block_num) {
    python_cache_index::iterator it = python_instantiation_cache.find(boost::make_tuple(code_hash, vm_type, vm_version));
    if(it != python_instantiation_cache.end()) {
        python_instantiation_cache.modify(it, [block_num](python_cache_entry& e) {
            e.last_block_num_used = block_num;
        });
    }
}

//indicate the current LIB. evicts old cache entries
void python_interface::current_lib(const uint32_t lib) {
    //anything last used before or on the LIB can be evicted
    const auto first_it = python_instantiation_cache.get<by_last_block_num>().begin();
    const auto last_it  = python_instantiation_cache.get<by_last_block_num>().upper_bound(lib);
    python_instantiation_cache.get<by_last_block_num>().erase(first_it, last_it);
}

//Calls apply or error on a given code
void python_interface::apply(const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version, apply_context& context) {
    _vm_memory->counter += 1;
    get_instantiated_module(code_hash, vm_type, vm_version, context)->apply(context);
}

void python_interface::call(uint64_t contract, uint64_t func_name, uint64_t arg1, uint64_t arg2, uint64_t arg3, apply_context& context ) {
    EOS_ASSERT( false, wasm_execution_error, "call not implemented in python vm" );
}

//Immediately exits currently running wasm. UB is called when no wasm running
void python_interface::exit() {

}

const std::unique_ptr<python_instantiated_module>& python_interface::get_instantiated_module( const digest_type& code_hash, const uint8_t& vm_type,
                                                                            const uint8_t& vm_version, apply_context& context )
{
    python_cache_index::iterator it = python_instantiation_cache.find(
                                        boost::make_tuple(code_hash, vm_type, vm_version) );
    const code_object* codeobject = nullptr;
    if(it == python_instantiation_cache.end()) {
        codeobject = &db.get<code_object,by_code_hash>(boost::make_tuple(code_hash, vm_type, vm_version));

        it = python_instantiation_cache.emplace( python_cache_entry{
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
        python_instantiation_cache.modify(it, [&](auto& c) {
            c.module = runtime_interface->instantiate_module((const char*)bytes.data(), bytes.size(), {}, code_hash, vm_type, vm_version);
            _vm_memory->init_smart_contract = true;

            get_vm_api()->allow_access_apply_context = false;

            memcpy(_vm_memory->data.data(), _vm_memory->data_backup.data(), _vm_memory->data_backup.size());

            wasm2c_python_vm_call(1, context.get_receiver().to_uint64_t(), context.get_action().account.to_uint64_t(), 1);

            get_vm_api()->allow_access_apply_context = true;

            c.module->take_snapshoot(*_vm_memory);
        });
    }
    _vm_memory->init_smart_contract = false;
    _vm_memory->segments = &it->module->backup.memory_backup;
    _vm_memory->malloc_memory_start = it->module->backup.contract_memory_end;
    return it->module;
}

