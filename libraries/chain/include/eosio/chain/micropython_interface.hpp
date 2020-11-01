#pragma once
#include <eosio/chain/code_object.hpp>
#include <eosio/chain/types.hpp>
#include <eosio/chain/whitelisted_intrinsics.hpp>
#include <eosio/chain/exceptions.hpp>
#include "Runtime/Linker.h"
#include "Runtime/Runtime.h"

#include <eosio/chain/webassembly/runtime_interface.hpp>
#include <eosio/chain/wasm_eosio_injection.hpp>
#include <eosio/chain/transaction_context.hpp>
#include <eosio/chain/code_object.hpp>
#include <eosio/chain/exceptions.hpp>
#include <fc/scoped_exit.hpp>

namespace eosio { namespace chain {

   class apply_context;
   class controller;

   struct vm_micropython_state {
      vector<uint8_t> data;
   };

   class micropython_instantiated_module {
      public:
         micropython_instantiated_module();
         void apply(apply_context& context);
         void call(uint64_t func_name, uint64_t arg1, uint64_t arg2, uint64_t arg3, apply_context& context);
         void take_snapshoot();

         struct vm_micropython_state                            backup;
   };

   class micropython_runtime {
      public:
         micropython_runtime();
         std::unique_ptr<micropython_instantiated_module> instantiate_module(const char* code_bytes, size_t code_size, std::vector<uint8_t> initial_memory,
                                                                              const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version);

         void immediately_exit_currently_running_module();
   };

   struct micropython_cache_entry {
      digest_type                                          code_hash;
      uint32_t                                             first_block_num_used;
      uint32_t                                             last_block_num_used;
      std::unique_ptr<micropython_instantiated_module>          module;
      uint8_t                                              vm_type = 0;
      uint8_t                                              vm_version = 0;
   };
   struct by_hash;
   struct by_first_block_num;
   struct by_last_block_num;

   /**
    * @class micropython_interface
    *
    */
   class micropython_interface {
      public:
         micropython_interface(const chainbase::database& d);
         ~micropython_interface();

         //call before dtor to skip what can be minutes of dtor overhead with some runtimes; can cause leaks
         void indicate_shutting_down();

         //validates code -- does a WASM validation pass and checks the wasm against EOSIO specific constraints
         static void validate(const controller& control, const bytes& code);

         //indicate that a particular code probably won't be used after given block_num
         void code_block_num_last_used(const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version, const uint32_t& block_num);

         //indicate the current LIB. evicts old cache entries
         void current_lib(const uint32_t lib);

         //Calls apply or error on a given code
         void apply(const digest_type& code_hash, const uint8_t& vm_type, const uint8_t& vm_version, apply_context& context);
         void call(uint64_t contract, uint64_t func_name, uint64_t arg1, uint64_t arg2, uint64_t arg3, apply_context& context );
         //Immediately exits currently running wasm. UB is called when no wasm running
         void exit();

         const std::unique_ptr<micropython_instantiated_module>& get_instantiated_module( const digest_type& code_hash, const uint8_t& vm_type,
                                                                                    const uint8_t& vm_version, apply_context& context );
         typedef boost::multi_index_container<
            micropython_cache_entry,
            indexed_by<
               ordered_unique<tag<by_hash>,
                  composite_key< micropython_cache_entry,
                     member<micropython_cache_entry, digest_type, &micropython_cache_entry::code_hash>,
                     member<micropython_cache_entry, uint8_t,     &micropython_cache_entry::vm_type>,
                     member<micropython_cache_entry, uint8_t,     &micropython_cache_entry::vm_version>
                  >
               >,
               ordered_non_unique<tag<by_first_block_num>, member<micropython_cache_entry, uint32_t, &micropython_cache_entry::first_block_num_used>>,
               ordered_non_unique<tag<by_last_block_num>, member<micropython_cache_entry, uint32_t, &micropython_cache_entry::last_block_num_used>>
            >
         > python_cache_index;
      private:
         std::unique_ptr<micropython_runtime> runtime_interface;
         python_cache_index micropython_instantiation_cache;
         const chainbase::database& db;
         static std::map<uint16_t, struct vm_python_info> vm_python_map;
         vector<uint8_t> initial_vm_memory;
//         static std::map<uint16_t, std::shared_ptr<vm_memory>> vm_python_memory_map;
   };

} } // eosio::chain