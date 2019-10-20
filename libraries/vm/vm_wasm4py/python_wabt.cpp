//#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/wasm_eosio_constraints.hpp>

//wabt includes
#include <src/interp.h>
#include <src/binary-reader-interp.h>
#include <src/error-formatter.h>

#include <eosio/chain/webassembly/wabt.hpp>
#include <eosiolib_native/vm_api.h>
#include <chain_api.hpp>

namespace eosio { namespace chain { namespace webassembly { namespace wabt_runtime {

//yep 🤮
static wabt_apply_instance_vars* static_wabt_vars;

using namespace wabt;
using namespace wabt::interp;
namespace wasm_constraints = eosio::chain::wasm_constraints;

uint64_t get_microseconds() {
   if (sysconf(_POSIX_THREAD_CPUTIME)){
      struct timespec tv;
      int err = clock_gettime(CLOCK_THREAD_CPUTIME_ID,&tv);
      if (err == 0) {
         return tv.tv_sec * 1000000LL + tv.tv_nsec / 1000LL;
      }
   }
   printf("+++++ERROR: something went wrong!\n");
   return 0;
}

#if 1
#if 0
#define MEMCPY memcpy_fast
#define MEMSET memset_fast
#else
#define MEMCPY memcpy
#define MEMSET memset
#endif

void memcpy_fast(void *dest, void *src, size_t size) {
   for (int i=0;i<size/sizeof(uint64_t);i++) {
      ((uint64_t*)dest)[i] = ((uint64_t*)src)[i];
   }
   for (int i = size - size % sizeof(uint64_t);i<size;i++) {
      ((char *)dest)[i] = ((char *)src)[i];
   }
}

void memset_fast(void *dest, char c, size_t size) {
   for (int i=0;i<size/sizeof(uint64_t);i++) {
      ((uint64_t*)dest)[i] = c;
   }
   for (int i = size - size % sizeof(uint64_t);i<size;i++) {
      ((char *)dest)[i] = c;
   }
}
#endif

struct vm_state_backup {
   std::vector<MemorySegment>                                 memory_backup;
   int                                                        max_memory_segment_offset;
   int                                                        max_memory_segment_size;
   char                                                       *max_memory_segment_start;
   std::vector<std::pair<Global*, TypedValue>>                globals_backup;
   Limits                                                     memory_configuration_backup;
};

class wabt_instantiated_module : public wasm_instantiated_module_interface {
   public:
      wabt_instantiated_module(std::unique_ptr<interp::Environment> e, std::vector<uint8_t> initial_mem, interp::DefinedModule* mod) :
         _env(move(e)), _instatiated_module(mod), _initial_memory(initial_mem),
         _executor(_env.get(), nullptr, Thread::Options(64*1024,
                                                        wasm_constraints::maximum_call_depth+2))
      {
         for(Index i = 0; i < _env->GetGlobalCount(); ++i) {
            if(_env->GetGlobal(i)->mutable_ == false)
               continue;
            _initial_globals.emplace_back(_env->GetGlobal(i), _env->GetGlobal(i)->typed_value);
         }
         Memory* memory = _env->GetMemory(0);
         if(_env->GetMemoryCount()) {
            _initial_memory_configuration = memory->page_limits;
            memory->memory_segments = nullptr;
         }
         #ifdef WASM_FOR_PYTHON
            init();
            
            TypedValues _call_params{4, TypedValue(Type::I64)};
            _call_params[0].set_i64(N(init));
            _call_params[1].set_i64(0);
            _call_params[2].set_i64(0);
            _call_params[3].set_i64(0);

            ExecResult res;
            res = _executor.RunExportByName(_instatiated_module, "call", _call_params);
            EOS_ASSERT( res.result == interp::Result::Ok, wasm_execution_error, "wabt execution failure (${s})", ("s", ResultToString(res.result)) );

            memory->backup_memory();
            memory->init_cache();

            _globals_backup.clear();
            for(const auto& mg : _initial_globals) {
               _globals_backup.emplace_back(mg.first, mg.second);
            }
         #endif
      }

      void init() override {
         //reset mutable globals
         for(const auto& mg : _initial_globals)
            mg.first->typed_value = mg.second;
         wabt_apply_instance_vars this_run_vars{nullptr};
         static_wabt_vars = &this_run_vars;

         //reset memory to inital size & copy back in initial data
         if(_env->GetMemoryCount()) {
            Memory* memory = this_run_vars.memory = _env->GetMemory(0);
            memory->page_limits = _initial_memory_configuration;
            vmdlog("++++initial memory pool size: %d, _initial_memory.size() %d\n", _initial_memory_configuration.initial * WABT_PAGE_SIZE, _initial_memory.size());
            memory->data.resize(_initial_memory_configuration.initial * WABT_PAGE_SIZE);
            MEMSET(memory->data.data(), 0, memory->data.size());
            MEMCPY(memory->data.data(), _initial_memory.data(), _initial_memory.size());
         }

         ExecResult res;
         res = _executor.RunStartFunction(_instatiated_module);
         EOS_ASSERT( res.result == interp::Result::Ok, wasm_execution_error, "wabt start function failure (${s})", ("s", ResultToString(res.result)) );
      }

      void init_contract(uint64_t receiver, digest_type& code_id) {
         auto* memory = _env->GetMemory(0);
         for(const auto& mg : _globals_backup) {
            mg.first->typed_value = mg.second;
         }
         memory->restore_page_limits();
         //initialize
         try {
            get_vm_api()->allow_access_apply_context = false;
            call(1, receiver, 0, 0);
            get_vm_api()->allow_access_apply_context = true;
         } catch (...) {
            //restore memory back
            memory->restore_memory();
//            get_vm_api()->eosio_assert(0,"pythonvm execution error!");
            throw;
         }
         auto backup = std::make_shared<vm_state_backup>();
         for(const auto& mg : _initial_globals) {
            backup->globals_backup.emplace_back(mg.first, mg.second);
         }
         backup->memory_configuration_backup = memory->page_limits;

         uint64_t time_start = get_microseconds();
         int total_count = 0;
         int i = 0;
         int copy_start;
         int copy_end;
         uint64_t *ptr1 = (uint64_t *)memory->data_backup.data();
         uint64_t *ptr2 = (uint64_t *)memory->data.data();

         get_vm_api()->get_copy_memory_range(&copy_start, &copy_end);
         vmdlog("++++++++copy_start %d, copy_end %d, size is %d\n", copy_start, copy_end, copy_end - copy_start);
         int memory_backup_size = copy_start;//_memory_backup.size();
//         int memory_backup_size = _memory_backup.size();
         memory_backup_size = memory_backup_size/sizeof(uint64_t);

         //save diff memory
         while(i<memory_backup_size) {
            if (ptr1[i] == ptr2[i]) {
               i += 1;
               continue;
            }
            int start = i;
            total_count += 1;
            i += 1;
            while (i<memory_backup_size && ptr1[i] != ptr2[i]) {
               i += 1;
               total_count += 1;
            }
            MemorySegment segment;
            segment.offset = start*sizeof(uint64_t);
            segment.data.resize((i-start)*sizeof(uint64_t));
            MEMCPY(segment.data.data(), &ptr2[start], (i-start)*sizeof(uint64_t));
            backup->memory_backup.emplace_back(std::move(segment));

//            vmdlog("++++++++++++++++offset %d, size %d\n", start*sizeof(uint64_t), (i-start)*sizeof(uint64_t));
            i += 1;
         //               vmdlog("++++%d %d %d\n", i, (uint8_t)_env->GetMemory(0)->data[i], _memory_backup[i]);
         }
#if 1
         int copy_size = copy_end-copy_start;
         MemorySegment segment;
         segment.offset = copy_start;
         segment.data.resize(copy_size);
         MEMCPY(segment.data.data(), &memory->data[copy_start], copy_size);
         backup->memory_backup.emplace_back(std::move(segment));

         backup->max_memory_segment_offset = copy_start;
         backup->max_memory_segment_size = copy_size;
         int pos = backup->memory_backup.size()-1;
         backup->max_memory_segment_start = backup->memory_backup[pos].data.data();

         vmdlog("+++++++++++++++++grow memory size: %d \n", copy_size);
         vmdlog("++++++++++++++++memory diff duration %llu\n", get_microseconds()-time_start);
#endif
         _contract_state_backup[receiver] = backup;
         _contract_state_backup2[code_id] = backup;
         memory->memory_segments = &backup->memory_backup;
         memory->memory_end = copy_end;
         //restore memory back
         MEMCPY(memory->data.data(), memory->data_backup.data(), memory->data_backup.size());
         vmdlog("+++++++++++++++total diff count is %d\n", total_count*sizeof(uint64_t)+copy_size);
         vmdlog("default memory size: %d, _initial_memory.size: %d\n", memory->data.size(), _initial_memory.size());
    }

      void init_contract_v2(uint64_t receiver, digest_type& code_id) {
         for(const auto& mg : _globals_backup) {
            mg.first->typed_value = mg.second;
         }
         _env->GetMemory(0)->restore_page_limits();
         //initialize
         std::shared_ptr<vm_state_backup> backup = std::make_shared<vm_state_backup>();
         size_t code_size;
         const char *compiled_code = get_chain_api()->get_code_ex(receiver, &code_size);
         int index = 0;
//         char *memory_ptr = _env->GetMemory(0)->data.data();
         backup->max_memory_segment_offset = 0;
         backup->max_memory_segment_size = 0;
         backup->max_memory_segment_start = nullptr;

         int total_size = 0;
         while(index < code_size) {
            int segment_size = ((int*)&compiled_code[index])[0];
            int segment_offset = ((int*)&compiled_code[index])[1];
//            vmdlog("+++++%d %d\n", segment_offset, segment_size);
            EOSIO_ASSERT(index + segment_size + sizeof(int)*2 <= code_size, "segment out of range");

            MemorySegment segment;
            segment.offset = segment_offset;
            segment.data.resize(segment_size);

            MEMCPY(segment.data.data(), compiled_code+index+sizeof(int)*2, segment_size);

            backup->memory_backup.push_back(std::move(segment));
//            MEMCPY(&memory_ptr[segment_offset], segment.data.data(), segment_size);

            index += sizeof(int)*2;
            index += segment_size;
            if (index == code_size) {
               backup->max_memory_segment_offset = segment_offset;
               backup->max_memory_segment_size = segment_size;
               int pos = backup->memory_backup.size()-1;
               backup->max_memory_segment_start = backup->memory_backup[pos].data.data();
               _env->GetMemory(0)->memory_end = segment_offset+segment_size;
               vmdlog("++++save the last segment segment offset: %d, segment size: %d\n", segment_offset, segment_size);
            }
            total_size += segment_size;
         }
         vmdlog("+++++++++++total size: %d\n", total_size);
         _contract_state_backup[receiver] = backup;
         _contract_state_backup2[code_id] = backup;
         _env->GetMemory(0)->memory_segments = &backup->memory_backup;
         vmdlog("default memory size: %d, _initial_memory.size: %d\n", _env->GetMemory(0)->data.size(), _initial_memory.size());
    }

    void restore_contract(struct vm_state_backup& backup) {
#if 0
      auto& write_memory = _env->GetMemory(0)->data;
         int copy_size = backup.max_memory_segment_offset;
//         MEMCPY(_env->GetMemory(0)->data.data(), _memory_backup.data(), _memory_backup.size());
         MEMCPY(write_memory.data(), _memory_backup.data(), copy_size);
         //restore to initial state
         vmdlog("++++++_memory_backup.size(): %d \n", _memory_backup.size());
         char *memory_ptr = write_memory.data();
         for (auto const& item: backup.memory_backup) {
            MEMCPY(&memory_ptr[item.offset], item.data.data(), item.data.size());
         }
#endif
         for(const auto& mg : _globals_backup) {
            mg.first->typed_value = mg.second;
         }
         _env->GetMemory(0)->restore_page_limits();
    }

      void apply() override {
        Memory *memory = _env->GetMemory(0);
        memory->base_address = memory->data.data();
        memory->memory_segments = nullptr;
//        memory->init_smart_contract = false;

        if (memory->counter == 0xffffffff) {
           //reset counter make IsWriteMemoryInUse function properly 
            memset(memory->in_use.data(), 0, memory->in_use.size()*sizeof(memory->in_use[0]));
            memset(memory->memory_segments_cache.data(), 0, memory->memory_segments_cache.size()*sizeof(memory->memory_segments_cache[0]));
//            memory->memory_segments_cache.assign(memory->memory_segments_cache.size(), {0, 0});
           memory->counter = 1;
        } else {
           memory->counter += 1;
        }

//         MEMCPY(memory->data.data(), _memory_backup.data(), 1881432);//_memory_backup.size());
         uint64_t receiver = get_vm_api()->current_receiver();
         digest_type code_id;
         get_chain_api()->get_code_id(receiver, (uint8_t*)code_id.data(), 32);

        auto it = _contract_state_backup2.find(code_id);
        if (it == _contract_state_backup2.end()) {
            eosio::chain::digest_type empty;
            vmdlog("++++++++++++++++init_contract\n");
            MEMCPY(memory->data.data(), memory->data_backup.data(), memory->data_backup.size());
            memory->init_smart_contract = true;
            int vm_type = get_chain_api()->get_code_type(receiver);
            if (vm_type == 1) {
               init_contract(receiver, code_id);
            } else if (vm_type == 2) {
               init_contract_v2(receiver, code_id);
            }
        } else {
           uint64_t start = get_microseconds();
           restore_contract(*it->second);
           memory->memory_segments = &it->second->memory_backup;
           memory->memory_end = it->second->max_memory_segment_offset+it->second->max_memory_segment_size;
           vmdlog("++++++++++++++++restore memory cost: %d\n", get_microseconds()-start);
        }

        memory->init_smart_contract = false;
        wabt_apply_instance_vars this_run_vars{nullptr};
        static_wabt_vars = &this_run_vars;
        this_run_vars.memory = memory;

        uint64_t account = 0;
        uint64_t act_name = 0;

        get_vm_api()->get_action_info(&account, &act_name);
        _params[0].set_i64(receiver);
        _params[1].set_i64(account);
        _params[2].set_i64(act_name);

        ExecResult res;
        uint64_t start = get_microseconds();
        try {
            res = _executor.RunExportByName(_instatiated_module, "apply", _params);
            vmdlog("++++++++++++++++wabt::apply cost %llu\n", get_microseconds()-start);
         } catch (...) {
            //get_vm_api()->eosio_assert(0,"pythonvm execution error!");
            throw;
         }
        EOS_ASSERT( res.result == interp::Result::Ok, wasm_execution_error, "wabt execution failure (${s})", ("s", ResultToString(res.result)) );
      }

      void call(uint64_t func_name, uint64_t arg1, uint64_t arg2, uint64_t arg3) override {
         wabt_apply_instance_vars this_run_vars{nullptr};
         static_wabt_vars = &this_run_vars;
         this_run_vars.memory = _env->GetMemory(0);

         TypedValues _call_params{4, TypedValue(Type::I64)};
         _call_params[0].set_i64(func_name);
         _call_params[1].set_i64(arg1);
         _call_params[2].set_i64(arg2);
         _call_params[3].set_i64(arg3);

         ExecResult res;
         res = _executor.RunExportByName(_instatiated_module, "call", _call_params);
         EOS_ASSERT( res.result == interp::Result::Ok, wasm_execution_error, "wabt execution failure (${s})", ("s", ResultToString(res.result)) );
      }

private:
    std::unique_ptr<interp::Environment>              _env;
    DefinedModule*                                    _instatiated_module;  //this is owned by the Environment
    std::vector<uint8_t>                              _initial_memory;
    TypedValues                                       _params{3, TypedValue(Type::I64)};
    std::vector<std::pair<Global*, TypedValue>>       _initial_globals;
    Limits                                            _initial_memory_configuration;
    Executor                                          _executor;
    std::vector<std::pair<Global*, TypedValue>>       _globals_backup;
    std::map<uint64_t, std::shared_ptr<struct vm_state_backup>>      _contract_state_backup;
    std::map<fc::sha256, std::shared_ptr<struct vm_state_backup>>    _contract_state_backup2;

};

wabt_runtime::wabt_runtime() {}

std::unique_ptr<wasm_instantiated_module_interface> wabt_runtime::instantiate_module(const char* code_bytes, size_t code_size, std::vector<uint8_t> initial_memory) {
   std::unique_ptr<interp::Environment> env = std::make_unique<interp::Environment>();
   for(auto it = intrinsic_registrator::get_map().begin() ; it != intrinsic_registrator::get_map().end(); ++it) {
      interp::HostModule* host_module = env->AppendHostModule(it->first);
      for(auto itf = it->second.begin(); itf != it->second.end(); ++itf) {
         host_module->AppendFuncExport(itf->first, itf->second.sig, [fn=itf->second.func](const auto* f, const auto* fs, const auto& args, auto& res) {
            TypedValue ret = fn(*static_wabt_vars, args);
            if(ret.type != Type::Void)
               res[0] = ret;
            return interp::Result::Ok;
         });
      }
   }

   interp::DefinedModule* instantiated_module = nullptr;
   wabt::Errors errors;

   wabt::Result res = ReadBinaryInterp(env.get(), code_bytes, code_size, read_binary_options, &errors, &instantiated_module);
   EOS_ASSERT( Succeeded(res), wasm_execution_error, "Error building wabt interp: ${e}", ("e", wabt::FormatErrorsToString(errors, Location::Type::Binary)) );
   
   return std::make_unique<wabt_instantiated_module>(std::move(env), initial_memory, instantiated_module);
}

void wabt_runtime::immediately_exit_currently_running_module() {
   throw wasm_exit();
}

}}}}
