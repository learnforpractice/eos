#pragma once
#include <string>
using namespace std;

void pack_native_object_(int type, string& msg, string& packed_message);
void unpack_native_object_(int type, string& packed_message, string& msg);

void*   chain_new_(string& config, string& protocol_features_dir, string& snapshot_dir);
void    chain_free_(void *ptr);
void    chain_api_get_info_(void *chain_ptr, string& info);

#define DEF_CHAIN_API_RO(api_name) \
    void    chain_api_ ## api_name ## _(void *ptr, string& params, string& result);

DEF_CHAIN_API_RO(get_activated_protocol_features)
DEF_CHAIN_API_RO(get_block)
DEF_CHAIN_API_RO(get_block_header_state)
DEF_CHAIN_API_RO(get_account)
DEF_CHAIN_API_RO(get_code)
DEF_CHAIN_API_RO(get_code_hash)
DEF_CHAIN_API_RO(get_abi)
DEF_CHAIN_API_RO(get_raw_code_and_abi)
DEF_CHAIN_API_RO(get_raw_abi)
DEF_CHAIN_API_RO(get_table_rows)
DEF_CHAIN_API_RO(get_table_by_scope)
DEF_CHAIN_API_RO(get_currency_balance)
DEF_CHAIN_API_RO(get_currency_stats)
DEF_CHAIN_API_RO(get_producers)
DEF_CHAIN_API_RO(get_producer_schedule)

DEF_CHAIN_API_RO(get_scheduled_transactions)
DEF_CHAIN_API_RO(abi_json_to_bin)
DEF_CHAIN_API_RO(abi_bin_to_json)
DEF_CHAIN_API_RO(get_required_keys)
DEF_CHAIN_API_RO(get_transaction_id)


void        chain_id_(void *ptr, string& chain_id);
uint32_t    chain_fork_db_pending_head_block_num_(void *ptr);
uint32_t    chain_last_irreversible_block_num_(void *ptr);
void        chain_get_block_id_for_num_(void *ptr, uint32_t num, string& block_id);
void        chain_fetch_block_by_number_(void *ptr, uint32_t block_num, string& raw_block);
int         chain_is_building_block_(void *ptr);

int         chain_api_recover_reversible_blocks_(string& old_reversible_blocks_dir, string& new_reversible_blocks_dir, uint32_t reversible_cache_size, uint32_t truncate_at_block);
void        chain_api_repair_log_(string& blocks_dir, uint32_t truncate_at_block, string& backup_blocks_dir);
void        chain_api_get_table_rows_(void *ptr, string& params, string& results);

void*       producer_new_(void *chain_ptr, string& config);
void        producer_free_(void *ptr);
void        producer_on_incoming_block_(void *ptr, string& packed_signed_block, uint32_t& num, string& id);

int         producer_start_block_(void *ptr);
uint64_t    producer_calc_pending_block_time_(void *ptr);
uint64_t    producer_calc_pending_block_deadline_time_(void *ptr);
bool        producer_maybe_produce_block_(void *ptr);
uint64_t    producer_now_time_();
int         producer_get_pending_block_mode_(void *ptr);
int         producer_process_incomming_transaction_(void *ptr, string& packed_trx, string& raw_packed_trx, string& out);
int         producer_process_raw_transaction_(void *ptr, string& raw_packed_trx, string& out);
int         producer_create_snapshot_(void *ptr, string& out);

void        uuos_recover_key_(string& _digest, string& _sig, string& _pub);

enum {
    handshake_message_type,
    chain_size_message_type,
    go_away_message_type,
    time_message_type,
    notice_message_type,
    request_message_type,
    sync_request_message_type,
    signed_block_type,         // which = 7
    packed_transaction_type,
    producer_params_type,
    genesis_state_type,
};
