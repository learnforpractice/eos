#include <eosio/chain/wasm_interface.hpp>
//#include <eosio/chain/apply_context.hpp>
//#include <eosio/chain/controller.hpp>
//#include <eosio/chain/transaction_context.hpp>
#include <eosio/chain/producer_schedule.hpp>
#include <eosio/chain/exceptions.hpp>
#include <boost/core/ignore_unused.hpp>
#include <eosio/chain/authorization_manager.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <eosio/chain/global_property_object.hpp>
#include <eosio/chain/account_object.hpp>
#include <fc/exception/exception.hpp>
#include <fc/crypto/sha256.hpp>
#include <fc/crypto/sha1.hpp>
#include <fc/io/raw.hpp>

#include <eosio/chain/wasm_eosio_validation.hpp>
#include <eosio/chain/wasm_eosio_injection.hpp>
#include <eosio/chain/wasm_interface_private.hpp>

#include <softfloat.hpp>
#include <compiler_builtins.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <fstream>
#include <chain_api.hpp>
#include "../../../unittests/incbin.h"

extern "C" const unsigned char pythonvm_wasm[];
extern "C" int pythonvm_wasm_size;

INCBIN(PythonVM, "pythonvm.wasm");

namespace eosio { namespace chain {
   using namespace webassembly;
   using namespace webassembly::common;

   wasm_interface::wasm_interface(vm_type vm) : my( new wasm_interface_impl(vm_type::wabt) ) {
      wasm_constraints::set_maximum_linear_memory_init(6*1024*1024);
      wasm_constraints::set_maximum_table_elements(10240);
      wasm_constraints::set_maximum_func_local_bytes(128*1024);
      wasm_constraints::set_maximum_section_elements(10240);

      int size = pythonvm_wasm_size;
      python_code_id = fc::sha256::hash( (char *)gPythonVMData, gPythonVMSize );
      validate((char *)gPythonVMData, gPythonVMSize);
      my->get_instantiated_module(python_code_id, (char *)gPythonVMData, gPythonVMSize);
   }

   wasm_interface::~wasm_interface() {}

   void wasm_interface::validate(const bytes& code) {
      validate(code.data(), code.size());
	}

   void wasm_interface::validate(const char *data, int size) {
      Module module;
      try {
         Serialization::MemoryInputStream stream((U8*)data, size);
         WASM::serialize(stream, module);
      } catch(const Serialization::FatalSerializationException& e) {
         EOS_ASSERT(false, wasm_serialization_error, e.message.c_str());
      } catch(const IR::ValidationException& e) {
         EOS_ASSERT(false, wasm_serialization_error, e.message.c_str());
      }

      wasm_validations::wasm_binary_validation validator(module);
      validator.validate();

      root_resolver resolver(true);
      LinkResult link_result = linkModule(module, resolver);

      //there are a couple opportunties for improvement here--
      //Easy: Cache the Module created here so it can be reused for instantiaion
      //Hard: Kick off instantiation in a separate thread at this location
	}

   bool wasm_interface::apply( const digest_type& code_id, const shared_string& code) {
      my->get_instantiated_module(python_code_id, code)->apply();
      return true;
   }

   bool wasm_interface::call( uint64_t contract, uint64_t func_name, uint64_t arg1, uint64_t arg2, uint64_t arg3 ) {
      digest_type code_id;
      const char* code = nullptr;
      size_t size = 0;

      if (!get_chain_api()->get_code(contract, code_id, &code, &size)) {
         return false;
      }
      
      int type1 = get_chain_api()->get_code_type(contract);
      uint64_t receiver = get_vm_api()->current_receiver();
      int type2 = get_chain_api()->get_code_type(receiver);
      EOSIO_ASSERT(type1 != type2, "vm type of caller and callee should not be the same!");

      my->get_instantiated_module(python_code_id, code, size)->call(func_name, arg1, arg2, arg3);
      return true;
   }

   void wasm_interface::exit() {
      my->runtime_interface->immediately_exit_currently_running_module();
   }

   wasm_instantiated_module_interface::~wasm_instantiated_module_interface() {}
   wasm_runtime_interface::~wasm_runtime_interface() {}


std::istream& operator>>(std::istream& in, wasm_interface::vm_type& runtime) {
   std::string s;
   in >> s;
   if (s == "wavm")
      runtime = eosio::chain::wasm_interface::vm_type::wavm;
   else if (s == "wabt")
      runtime = eosio::chain::wasm_interface::vm_type::wabt;
   else
      in.setstate(std::ios_base::failbit);
   return in;
}

} } /// eosio::chain
