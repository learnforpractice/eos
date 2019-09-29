/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#pragma once

#include <eosio/eosio.hpp>

class [[eosio::contract]] native_contract_test : public eosio::contract {
public:
   using eosio::contract::contract;

   [[eosio::action]]
   void anyaction( eosio::name test );
};
