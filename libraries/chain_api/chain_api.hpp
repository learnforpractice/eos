/*
 * chain_api.hpp
 *
 *  Created on: Oct 13, 2018
 *      Author: newworld
 */

#ifndef CHAIN_API_HPP_
#define CHAIN_API_HPP_

#include <stdint.h>
#include <string>
#include <eosio/chain/types.hpp>

using namespace std;
using namespace eosio::chain;

enum class enum_builtin_protocol_feature : uint32_t {
   preactivate_feature,
   only_link_to_existing_permission,
   replace_deferred,
   no_duplicate_deferred_id,
   fix_linkauth_restriction,
   disallow_empty_producer_schedule,
   restrict_action_to_self,
   only_bill_first_authorizer,
   forward_setcode,
   get_sender,
   ram_restrictions,
   webauthn_key,
   wtmsig_block_signatures,
   code_version,
   pythonvm
};

struct chain_api_cpp
{
   int64_t (*get_current_exception)(std::string& what);
   void (*n2str)(uint64_t n, string& str_name);
   uint64_t (*str2n)(string& str_name);

   bool (*get_code)(uint64_t contract, digest_type& code_id, const char** code, size_t* size);
   const char* (*get_code_ex)( uint64_t contract, size_t* size );
   bool (*get_code_id)( uint64_t receiver,  digest_type& code_id);
   int (*get_code_type)( uint64_t receiver);

   bool (*get_code_by_code_hash)(const digest_type& code_hash, const uint8_t vm_type, const uint8_t vm_version, const char** code, size_t* size, uint32_t* first_block_used);

   string (*get_state_dir)();
   bool (*contracts_console)();
   void (*resume_billing_timer)(void);
   void (*pause_billing_timer)(void);
   bool (*is_producing_block)();
   bool (*is_pythonvm_loading_finished)();
//vm_exceptions.cpp
   void (*throw_exception)(int type, const char* fmt, ...);
   uint64_t (*get_microseconds)();

   void (*set_mem_segment_range)(const char *start, int offset, int size);
   void (*get_mem_segment_range)(const char **start, int *offset, int *size);

   void (*get_resource_limits)( uint64_t account, int64_t* ram_bytes, int64_t* net_weight, int64_t* cpu_weight );

   bool (*enable_debug)(bool enable);
   bool (*is_debug_enabled)();
   bool (*add_debug_contract)(string& contract_name, string& path);
   bool (*clear_debug_contract)(string& contract_name);
   void* (*get_debug_contract_entry)(string& contract_name);
   bool (*is_builtin_activated)(uint32_t feature);
};

extern "C" void register_chain_api(struct chain_api_cpp* api);
extern "C" struct chain_api_cpp* get_chain_api();
extern "C" [[ noreturn ]] void chain_throw_exception(int type, const char* fmt, ...);

#endif /* CHAIN_API_HPP_ */

