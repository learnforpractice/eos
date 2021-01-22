#include <eosio/chain/webassembly/eos-vm.hpp>
#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/transaction_context.hpp>
#include <eosio/chain/wasm_eosio_constraints.hpp>
//eos-vm includes
#include <eosio/vm/backend.hpp>
#include <vm_api.h>
#include <chain_api.hpp>


namespace eosio { namespace chain { namespace webassembly { namespace eos_vm_runtime {

using namespace eosio::vm;

namespace wasm_constraints = eosio::chain::wasm_constraints;

namespace {

  struct checktime_watchdog {
     checktime_watchdog() {}
     template<typename F>
     struct guard {
        guard(F&& func) : _func(static_cast<F&&>(func)) {
           get_chain_api()->timer_set_expiration_callback(&callback, this);
           if(get_chain_api()->timer_expired()) {
              _func(); // it's harmless if _func is invoked twice
           }
        }
        ~guard() {
           get_chain_api()->timer_set_expiration_callback(nullptr, nullptr);
        }
        static void callback(void* data) {
           guard* self = static_cast<guard*>(data);
           self->_func();
        }
//        transaction_checktime_timer& _timer;
        F _func;
     };
     template<typename F>
     guard<F> scoped_run(F&& func) {
        return guard{static_cast<F&&>(func)};
     }

  };

}

template<typename Impl>
class eos_vm_instantiated_module : public wasm_instantiated_module_interface {
      using backend_t = backend<apply_context, Impl>;
   public:
      
      eos_vm_instantiated_module(eos_vm_runtime<Impl>* runtime, std::unique_ptr<backend_t> mod) :
         _runtime(runtime),
         _instantiated_module(std::move(mod)) {}

      void apply(apply_context& context) override {
         static vm::wasm_allocator                 wasm_alloc;
//         _instantiated_module->set_wasm_allocator((vm::wasm_allocator *)get_chain_api()->get_wasm_allocator());
         _instantiated_module->set_wasm_allocator(&wasm_alloc);
         _runtime->_bkend = _instantiated_module.get();
         auto fn = [&]() {
            uint64_t receiver;
            uint64_t first_receiver;
            uint64_t action;
            _runtime->_bkend->initialize(&context);
            get_chain_api()->get_apply_args(receiver, first_receiver, action);
            const auto& res = _runtime->_bkend->call(
                &context, "env", "apply", receiver, first_receiver, action);
         };
         try {
            checktime_watchdog wd;
            _runtime->_bkend->timed_run(wd, fn);
         } catch(eosio::vm::timeout_exception&) {
            get_vm_api()->checktime();
         } catch(eosio::vm::wasm_memory_exception& e) {
            elog("+++wasm_memory_exception ${e}", ("e", e.detail()));
            FC_THROW_EXCEPTION(wasm_execution_error, "access violation");
         } catch(eosio::vm::exception& e) {
            // FIXME: Do better translation
            FC_THROW_EXCEPTION(wasm_execution_error, "something went wrong...");
         }
         _runtime->_bkend = nullptr;
      }

   private:
      eos_vm_runtime<Impl>*            _runtime;
      std::unique_ptr<backend_t> _instantiated_module;
};

template<typename Impl>
eos_vm_runtime<Impl>::eos_vm_runtime() {}

template<typename Impl>
void eos_vm_runtime<Impl>::immediately_exit_currently_running_module() {
   throw wasm_exit{};
}

template<typename Impl>
bool eos_vm_runtime<Impl>::inject_module(IR::Module& module) {
   return false;
}

template<typename Impl>
std::unique_ptr<wasm_instantiated_module_interface> eos_vm_runtime<Impl>::instantiate_module(const char* code_bytes, size_t code_size, std::vector<uint8_t>,
                                                                                             const digest_type&, const uint8_t&, const uint8_t&) {
   using backend_t = backend<apply_context, Impl>;
   try {
      wasm_code_ptr code((uint8_t*)code_bytes, code_size);
      std::unique_ptr<backend_t> bkend = std::make_unique<backend_t>(code, code_size);
      registered_host_functions<apply_context>::resolve(bkend->get_module());
      return std::make_unique<eos_vm_instantiated_module<Impl>>(this, std::move(bkend));
   } catch(eosio::vm::exception& e) {
      FC_THROW_EXCEPTION(wasm_execution_error, "Error building eos-vm interp: ${e}", ("e", e.what()));
   }
}

template class eos_vm_runtime<eosio::vm::interpreter>;
template class eos_vm_runtime<eosio::vm::jit>;

}}}}
