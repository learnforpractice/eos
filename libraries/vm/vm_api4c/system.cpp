#include "stacktrace.h"
static void eosio_assert( u32 test, u32 msg_offset ) {
   if (test) {
   } else {
      // print_stacktrace();
      const char* msg = (char *)offset_to_char_ptr(msg_offset);
      EOSIO_ASSERT( test, msg );
   }
}

static void  eosio_assert_message( u32 test, u32 msg_offset, u32 msg_len ) {
   const char* msg = (char *)offset_to_ptr(msg_offset, msg_len);
   get_vm_api()->eosio_assert_message( test, msg, msg_len );
}

static void  eosio_assert_code( u32 test, u64 code ) {
   get_vm_api()->eosio_assert_code( test, (uint64_t)code );
}

static void  eosio_exit( u32 code ) {
   get_vm_api()->eosio_exit( (int32_t)code );
}

static u64  current_time() {
   return get_vm_api()->current_time();
}

static void call_contract(u64 contract, u32 args_offset, u32 size1) {
   const char *args = (char *)offset_to_ptr(args_offset, size1);
   get_vm_api()->call_contract(contract, args, size1);
}

static u32 call_contract_get_args(u32 extra_args_offset, u32 size1) {
   char *extra_args = (char *)offset_to_ptr(extra_args_offset, size1);
   return get_vm_api()->call_contract_get_args(extra_args, size1);
}

static u32 call_contract_set_results(u32 results_offset, u32 size1) {
   char *results = (char *)offset_to_ptr(results_offset, size1);
   return get_vm_api()->call_contract_set_results(results, size1);
}

static u32 call_contract_get_results(u32 results_offset, u32 size1) {
   char *results = (char *)offset_to_ptr(results_offset, size1);
   return get_vm_api()->call_contract_get_results(results, size1);
}

static void wasm_syscall() {
   EOSIO_ASSERT(0, "bad syscall");
}

static u32 get_code_hash(u64 contract, u32 hash_offset, u32 size) {
   char *hash = (char *)offset_to_ptr(hash_offset, size);
   return get_vm_api()->get_code_hash(contract, hash, size);
}
