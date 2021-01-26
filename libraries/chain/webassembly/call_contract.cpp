#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/chain/transaction_context.hpp>
#include <eosio/chain/apply_context.hpp>

namespace eosio { namespace chain { namespace webassembly {

   void interface::call_contract(uint64_t contract, legacy_span<const char> buffer) {
      context.call_contract(contract, buffer.data(), buffer.size());
   }

   int interface::call_contract_get_args(legacy_span<char> buffer) {
      return context.call_contract_get_args(buffer.data(), buffer.size());
   }

   int interface::call_contract_set_results(legacy_span<const char> buffer) {
      return context.call_contract_set_results(buffer.data(), buffer.size());
   }

   int interface::call_contract_get_results(legacy_span<char> buffer) {
      return context.call_contract_get_results(buffer.data(), buffer.size());
   }

}}} // ns eosio::chain::webassembly
