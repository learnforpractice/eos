#pragma once

#include <string>
#include <memory>
#include <map>

using namespace std;

namespace eosio {
    namespace chain {
        class controller;
    }
}

class chain_api_proxy {
    public:
        chain_api_proxy(eosio::chain::controller* c);
        virtual ~chain_api_proxy();

        virtual eosio::chain::controller* chain();

        virtual int get_info(string& result);
        virtual int get_activated_protocol_features(string& params, string& result);
        virtual int get_block(string& params, string& result);
        virtual int get_block_header_state(string& params, string& result);
        virtual int get_account(string& params, string& result);
        virtual int get_code(string& params, string& result);
        virtual int get_code_hash(string& params, string& result);
        virtual int get_abi(string& params, string& result);
        virtual int get_raw_code_and_abi(string& params, string& result);
        virtual int get_raw_abi(string& params, string& result);
        virtual int get_table_rows(string& params, string& result);
        virtual int get_table_by_scope(string& params, string& result);
        virtual int get_currency_balance(string& params, string& result);
        virtual int get_currency_stats(string& params, string& result);
        virtual int get_producers(string& params, string& result);
        virtual int get_producer_schedule(string& params, string& result);

        virtual int get_scheduled_transactions(string& params, string& result);
        virtual int abi_json_to_bin(string& params, string& result);
        virtual int abi_bin_to_json(string& params, string& result);
        virtual int get_required_keys(string& params, string& result);
        virtual int get_transaction_id(string& params, string& result);

    private:
        eosio::chain::controller *c;
};
