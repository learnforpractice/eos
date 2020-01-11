#pragma once

#include <eosio/chain/types.hpp>
#include <eosio/chain/protocol_feature_manager.hpp>

namespace eosio { namespace chain {

   /**
    *  For each action dispatched this receipt is generated
    */
   struct action_receipt {
      account_name                    receiver;
      digest_type                     act_digest;
      uint64_t                        global_sequence = 0; ///< total number of actions dispatched since genesis
      uint64_t                        recv_sequence   = 0; ///< total number of actions with this receiver since genesis
      flat_map<account_name,uint64_t> auth_sequence;
      fc::unsigned_int                code_sequence = 0; ///< total number of setcodes
      fc::unsigned_int                abi_sequence  = 0; ///< total number of setabis
      fc::optional<std::vector<char>> return_value;      ///< return value of the action

      digest_type digest()const { return digest_type::hash(*this); }
   };

} }  /// namespace eosio::chain

FC_REFLECT( eosio::chain::action_receipt,
            (receiver)(act_digest)(global_sequence)(recv_sequence)(auth_sequence)(code_sequence)(abi_sequence)(return_value) )
