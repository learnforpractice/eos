#include <eosio/chain/micropython_interface.hpp>
#include <eosio/chain/apply_context.hpp>
#include <vm_api/vm_api.h>

#include "../vm/vm_micropython/micropython_vm_config.h"

using namespace eosio::chain;

#ifdef PAGE_SIZE
#undef PAGE_SIZE
#endif

#define PAGE_SIZE 65536

extern "C" {
    int micropython_init();
    void *micropython_get_memory();
    size_t micropython_get_memory_size();
    size_t micropython_backup_memory(void *backup, size_t size);
    size_t micropython_restore_memory(void *backup, size_t size);
    void micropython_init_memory(size_t initial_pages);
    int micropython_contract_init(int type, const char *py_src, size_t size);
    int micropython_contract_apply(uint64_t receiver, uint64_t code, uint64_t action);
    size_t micropython_load_frozen_code(const char *str, size_t len, char *content, size_t content_size);
}

micropython_instantiated_module::micropython_instantiated_module()
{

}

long long get_time_us() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long us = te.tv_sec*1000000LL + te.tv_usec; // calculate milliseconds
//    printf("us: %lld\n", us);
    return us;
}

void micropython_instantiated_module::apply(apply_context& context) {
    auto receiver = context.get_receiver().to_uint64_t();
    auto account = context.get_action().account.to_uint64_t();
    auto act = context.get_action().name.to_uint64_t();

    long long start = get_time_us();
    int ret = micropython_contract_apply(receiver, account, act);
    EOS_ASSERT( ret, python_execution_error, "python vm execution error" );
    // printf("+++++++++apply %lld\n", get_time_us() - start);
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

    micropython_init();    
    size_t size = micropython_get_memory_size();
    void *memory = micropython_get_memory();
    initial_vm_memory.resize(size);
    memcpy(initial_vm_memory.data(), memory, size);
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

void micropython_interface::take_snapshoot(micropython_instantiated_module& module) {
//            printf("++++++++++++malloc pos %u\n", *(uint32_t *)ptr2);
    int total_count = 0;
    int block_size = 128;
    int vm_memory_size = micropython_get_memory_size();
    char *vm_start_memory_address = (char *)micropython_get_memory();
    char *ptr1 = (char *)initial_vm_memory.data();
    char *ptr2 = (char *)vm_start_memory_address;

    int initial_memory_size = initial_vm_memory.size();
    module.backup.initial_pages = vm_memory_size / PAGE_SIZE;
    int pos = PYTHON_VM_STACK_SIZE; //do not save stack data as it's temperately
    while(pos<initial_memory_size) {
        if (memcmp(ptr1+pos, ptr2+pos, block_size) == 0) {
            pos += block_size;
            continue;
        }
        int start = pos;
        total_count += 1;
        pos += block_size;
        while (pos<initial_memory_size) {
            if (memcmp(ptr1+pos, ptr2+pos, block_size) == 0) {
                break;
            }
            pos += block_size;
            total_count += 1;
        }

        memory_segment segment;
        int copy_size = pos-start;
        segment.offset = start;
        segment.data.resize(copy_size);
        memcpy(segment.data.data(), vm_start_memory_address + start, copy_size);
        module.backup.segments.emplace_back(segment);
        pos += block_size;
        module.backup.total_segment_size += copy_size;
//        elog("+++++++${n1}, ${n2}", ("n1", start)("n2", copy_size));
    }

    int remain_size = vm_memory_size - initial_memory_size;
    if (remain_size > 0) {
        memory_segment segment;
        segment.offset = initial_memory_size;
        segment.data.resize(remain_size);
        memcpy(segment.data.data(), vm_start_memory_address + initial_memory_size, remain_size);
        module.backup.segments.emplace_back(segment);
    }
}

size_t micropython_interface::get_snapshoot_size(const digest_type& code_hash, const uint8_t vm_type, const uint8_t vm_version, apply_context& context) {
    uint16_t version = ((uint16_t)vm_type<<8) | (uint16_t)vm_version;
    return get_instantiated_module(code_hash, vm_type, vm_version, context)->backup.total_segment_size;
}

static void print_hex(const char *data, size_t size) {
  for (int i=0;i<size;i++) {
    printf("%02x", data[i]);
  }
  printf("\n");
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
        micropython_instantiation_cache.modify(it, [&](auto& c) {
            c.module = runtime_interface->instantiate_module(codeobject->code.data(), codeobject->code.size(), {}, code_hash, vm_type, vm_version);
            auto cleanup = fc::make_scoped_exit([](){
                get_vm_api()->allow_access_apply_context = true;
            });
            get_vm_api()->allow_access_apply_context = false;
            micropython_init_memory(initial_vm_memory.size()/PAGE_SIZE);
            micropython_restore_memory(initial_vm_memory.data(), initial_vm_memory.size());
            //TODO: handle exception in micropython vm            
            size_t code_size = micropython_load_frozen_code("main.mpy", strlen("main.mpy"), NULL, 0);
            EOS_ASSERT( code_size > 0, python_execution_error, "main module not found!" );
            vector<char> content(code_size);
            code_size = micropython_load_frozen_code("main.mpy", strlen("main.mpy"), content.data(), content.size());
            EOS_ASSERT( code_size > 2 && content[0] == 'M', python_execution_error, "BAD mpy code" );
            int mpy_version = content[1];
            //TODO: Initialize contract code from vm with specified mpy version
            EOS_ASSERT( mpy_version == 5, python_execution_error, "BAD mpy version");
            int ret = micropython_contract_init(0, content.data(), content.size());
            EOS_ASSERT( ret, python_execution_error, "python contract init error" );
            // size_t size = micropython_get_memory_size();
            // c.module->backup.data.resize(size);
            // micropython_backup_memory(c.module->backup.data.data(), size);
            take_snapshoot(*c.module);
        });
    } else {
        micropython_init_memory(it->module->backup.initial_pages);
        char *vm_start_memory_address = (char *)micropython_get_memory();
        int pos = 0;
        for (memory_segment& segment: it->module->backup.segments) {
//            ilog("+++++++offset ${n1}, size ${n2}", ("n1", segment.offset)("n2", segment.data.size()));
            memcpy(vm_start_memory_address + pos, initial_vm_memory.data() + pos, segment.offset - pos);
            memcpy(vm_start_memory_address + segment.offset, segment.data.data(), segment.data.size());
            pos = segment.offset + segment.data.size();
        }
        memset(vm_start_memory_address + pos, 0, micropython_get_memory_size() - pos);
//        EOS_ASSERT(memcmp(it->module->backup.data.data(), vm_start_memory_address, it->module->backup.data.size()) == 0, fc::exception, "bad restore!");
//        micropython_restore_memory(it->module->backup.data.data(), it->module->backup.data.size());
    }
    return it->module;
}

