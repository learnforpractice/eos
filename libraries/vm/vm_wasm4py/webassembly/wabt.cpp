//#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/wasm_eosio_constraints.hpp>

//wabt includes
#include <src/interp.h>
#include <src/binary-reader-interp.h>
#include <src/error-formatter.h>

#include <eosio/chain/webassembly/wabt.hpp>

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
         
         if(_env->GetMemoryCount()) {
            _initial_memory_configuration = _env->GetMemory(0)->page_limits;
         }
      }

      void init() override {};

      void apply() override {
         //reset mutable globals
         for(const auto& mg : _initial_globals)
            mg.first->typed_value = mg.second;

         wabt_apply_instance_vars this_run_vars{nullptr};
         static_wabt_vars = &this_run_vars;

         //reset memory to inital size & copy back in initial data
         if(_env->GetMemoryCount()) {
            Memory* memory = this_run_vars.memory = _env->GetMemory(0);
            memory->page_limits = _initial_memory_configuration;
            memory->data.resize(_initial_memory_configuration.initial * WABT_PAGE_SIZE);
            memset(memory->data.data(), 0, memory->data.size());
            memcpy(memory->data.data(), _initial_memory.data(), _initial_memory.size());
         }

         uint64_t account = 0;
         uint64_t act_name = 0;
         uint64_t receiver = get_vm_api()->current_receiver();
         get_vm_api()->get_action_info(&account, &act_name);

         _params[0].set_i64(receiver);
         _params[1].set_i64(account);
         _params[2].set_i64(act_name);

         ExecResult res = _executor.RunStartFunction(_instatiated_module);
         EOS_ASSERT( res.result == interp::Result::Ok, wasm_execution_error, "wabt start function failure (${s})", ("s", ResultToString(res.result)) );

         res = _executor.RunExportByName(_instatiated_module, "apply", _params);
         EOS_ASSERT( res.result == interp::Result::Ok, wasm_execution_error, "wabt execution failure (${s})", ("s", ResultToString(res.result)) );         

      }

      void call(uint64_t func_name, uint64_t arg1, uint64_t arg2, uint64_t arg3) override {
         //reset mutable globals
         for(const auto& mg : _initial_globals)
            mg.first->typed_value = mg.second;

         wabt_apply_instance_vars this_run_vars{nullptr};
         static_wabt_vars = &this_run_vars;

         //reset memory to inital size & copy back in initial data
         if(_env->GetMemoryCount()) {
            Memory* memory = this_run_vars.memory = _env->GetMemory(0);
            memory->page_limits = _initial_memory_configuration;
            memory->data.resize(_initial_memory_configuration.initial * WABT_PAGE_SIZE);
            memset(memory->data.data(), 0, memory->data.size());
            memcpy(memory->data.data(), _initial_memory.data(), _initial_memory.size());
         }

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