#include <eosio/chain/webassembly/wavm.hpp>
#include <eosio/chain/wasm_eosio_constraints.hpp>
#include <eosio/chain/wasm_eosio_injection.hpp>
#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/exceptions.hpp>

#include "IR/Module.h"
#include "Platform/Platform.h"
#include "WAST/WAST.h"
#include "IR/Operators.h"
#include "IR/Validate.h"
#include "Runtime/Linker.h"
#include "Runtime/Intrinsics.h"

#include <vector>
#include <iterator>

#include <chain_api.hpp>
#include <eosiolib_native/vm_api.h>

using namespace IR;
using namespace Runtime;

namespace eosio { namespace chain { namespace webassembly { namespace wavm {

running_instance_context the_running_instance_context;

namespace detail {
struct wavm_runtime_initializer {
   wavm_runtime_initializer() {
      Runtime::init();
   }
};

using live_module_ref = std::list<ObjectInstance*>::iterator;

struct wavm_live_modules {
   live_module_ref add_live_module(ModuleInstance* module_instance) {
      return live_modules.insert(live_modules.begin(), asObject(module_instance));
   }

   void remove_live_module(live_module_ref it) {
      live_modules.erase(it);
      run_wavm_garbage_collection();
   }

   void run_wavm_garbage_collection() {
      //need to pass in a mutable list of root objects we want the garbage collector to retain
      std::vector<ObjectInstance*> root;
      std::copy(live_modules.begin(), live_modules.end(), std::back_inserter(root));
      Runtime::freeUnreferencedObjects(std::move(root));
   }

   std::list<ObjectInstance*> live_modules;
};

static wavm_live_modules the_wavm_live_modules;

}

class wavm_instantiated_module : public wasm_instantiated_module_interface {
   public:
      wavm_instantiated_module(ModuleInstance* instance, std::unique_ptr<Module> module, std::vector<uint8_t> initial_mem) :
         _initial_memory(initial_mem),
         _instance(instance),
         _module_ref(detail::the_wavm_live_modules.add_live_module(instance))
      {
         //The memory instance is reused across all wavm_instantiated_modules, but for wasm instances
         // that didn't declare "memory", getDefaultMemory() won't see it. It would also be possible
         // to say something like if(module->memories.size()) here I believe
         if(getDefaultMemory(_instance))
            _initial_memory_config = module->memories.defs.at(0).type;
      }

      ~wavm_instantiated_module() {
         detail::the_wavm_live_modules.remove_live_module(_module_ref);
      }

      void apply() override {
         uint64_t account = 0;
         uint64_t act_name = 0;
         uint64_t receiver = get_vm_api()->current_receiver();
         get_vm_api()->get_action_info(&account, &act_name);

         vector<Value> args = {Value(uint64_t(receiver)),
	                            Value(uint64_t(account)),
                               Value(uint64_t(act_name))};

         call("apply", args);
      }

      void call(uint64_t func_name, uint64_t arg1, uint64_t arg2, uint64_t arg3) override {
         vector<Value> args = {
            Value(uint64_t(func_name)),
            Value(uint64_t(arg1)),
            Value(uint64_t(arg2)),
            Value(uint64_t(arg3))
         };
         string str_func_name;
         get_chain_api()->n2str(func_name, str_func_name);
         call(str_func_name, args);
      }

   private:
      void call(const string &entry_point, const vector <Value> &args) {
         try {
            FunctionInstance* call = asFunctionNullable(getInstanceExport(_instance,entry_point));
            if( !call )
               return;

            EOS_ASSERT( getFunctionType(call)->parameters.size() == args.size(), wasm_exception, "" );

            //The memory instance is reused across all wavm_instantiated_modules, but for wasm instances
            // that didn't declare "memory", getDefaultMemory() won't see it
            MemoryInstance* default_mem = getDefaultMemory(_instance);
            if(default_mem) {
               //reset memory resizes the sandbox'ed memory to the module's init memory size and then
               // (effectively) memzeros it all
               resetMemory(default_mem, _initial_memory_config);

               char* memstart = &memoryRef<char>(getDefaultMemory(_instance), 0);
               memcpy(memstart, _initial_memory.data(), _initial_memory.size());
            }

            the_running_instance_context.memory = default_mem;
//            the_running_instance_context.apply_ctx = &context;

            resetGlobalInstances(_instance);
            runInstanceStartFunc(_instance);
            Runtime::invokeFunction(call,args);
         } catch( const wasm_exit& e ) {
         } catch( const Runtime::Exception& e ) {
             EOS_THROW(wasm_execution_error,
                         "cause: ${cause}\n${callstack}",
                         ("cause", string(describeExceptionCause(e.cause)))
                         ("callstack", e.callStack));
         } FC_CAPTURE_AND_RETHROW()
      }


      std::vector<uint8_t>     _initial_memory;
      //naked pointer because ModuleInstance is opaque
      //_instance is deleted via WAVM's object garbage collection when wavm_rutime is deleted
      ModuleInstance*          _instance;
      detail::live_module_ref  _module_ref;
      MemoryType               _initial_memory_config;
};

wavm_runtime::wavm_runtime() {
   static detail::wavm_runtime_initializer the_wavm_runtime_initializer;
}

wavm_runtime::~wavm_runtime() {
}

bool wavm_runtime::inject_module(IR::Module& module) {
   wasm_injections::wasm_binary_injection<true> injector(module);
   injector.inject();
   return true;
}

std::unique_ptr<wasm_instantiated_module_interface> wavm_runtime::instantiate_module(const char* code_bytes, size_t code_size, std::vector<uint8_t> initial_memory,
                                                                                     const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version) {
   std::unique_ptr<Module> module = std::make_unique<Module>();
   try {
      Serialization::MemoryInputStream stream((const U8*)code_bytes, code_size);
      WASM::serialize(stream, *module);
   } catch(const Serialization::FatalSerializationException& e) {
      EOS_ASSERT(false, wasm_serialization_error, e.message.c_str());
   } catch(const IR::ValidationException& e) {
      EOS_ASSERT(false, wasm_serialization_error, e.message.c_str());
   }

   try {
      eosio::chain::webassembly::common::root_resolver resolver;
      LinkResult link_result = linkModule(*module, resolver);
      ModuleInstance *instance = instantiateModule(*module, std::move(link_result.resolvedImports));
      EOS_ASSERT(instance != nullptr, wasm_exception, "Fail to Instantiate WAVM Module");

      return std::make_unique<wavm_instantiated_module>(instance, std::move(module), initial_memory);
   }
   catch(const Runtime::Exception& e) {
      EOS_THROW(wasm_execution_error, "Failed to stand up WAVM instance: ${m}", ("m", describeExceptionCause(e.cause)));
   }
}

void wavm_runtime::immediately_exit_currently_running_module() {
#ifdef _WIN32
   throw wasm_exit();
#else
   Platform::immediately_exit(nullptr);
#endif
}

}}}}
