#include <eosio/chain/types.hpp>
#include <eosio/chain/account_object.hpp>
#include <eosio/chain/code_object.hpp>
#include <eosio/chain/apply_context.hpp>

#include <chain_api.hpp>

namespace eosio {
    namespace chain {
        apply_context& ctx();
    }
}

using namespace std;
using namespace eosio::chain;

static vector<char> call_args;
static vector<char> call_returns;

void wasm_call(uint64_t contract, uint64_t func_name, const char *args, size_t args_size) {
   call_args.resize(args_size);
   memcpy(call_args.data(), args, args_size);
   call_returns.resize(0);

   auto& contract_account = ctx().control.db().get<account_metadata_object,by_name>( name(contract) );
   ctx().control.get_wasm_interface().apply(contract_account.code_hash, contract_account.vm_type, contract_account.vm_version, ctx());
}

void vm_call(uint64_t contract, uint64_t func_name, const char *args, size_t args_size) {
   int vm_type = get_chain_api()->get_code_type(contract);

   if (vm_type == 0) {
   } else {
      EOS_THROW( eosio_assert_message_exception, "only wasm code supported!" );
   }
   wasm_call(contract, func_name, args, args_size);
}

int call_contract_get_args(void* args, size_t size) {
    if (!args || size == 0) {
        return call_args.size();
    }
    int copy_size = std::min(size, call_args.size());
    memcpy(args, call_args.data(), copy_size);
    return copy_size;
}

int call_contract_set_results(const void* result, size_t size) {
    call_returns.resize(size);
    memcpy(call_returns.data(), result, size);
    return size;
}

int call_contract_get_results(void* result, size_t size) {
    if (!result || size == 0) {
        return call_returns.size();
    }
    int copy_size = std::min(size, call_returns.size());
    memcpy(result, call_returns.data(), copy_size);
    return copy_size;
}

void call_contract_cleanup() {
    call_args.resize(0);
    call_returns.resize(0);
}
