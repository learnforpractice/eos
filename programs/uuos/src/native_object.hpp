#pragma once
#include <string>
using namespace std;

void pack_native_object_(int type, string& msg, string& packed_message);
void unpack_native_object_(int type, string& packed_message, string& msg);

void*   chain_new_(string& config, string& protocol_features_dir);
void    chain_free_(void *ptr);
void    chain_api_get_info_(void *chain_ptr, string& info);
void    chain_api_get_activated_protocol_features_(void *ptr, string& params, string& result);
void    chain_api_get_block_(void *ptr, string& params, string& result);
void    chain_api_get_block_header_state_(void *ptr, string& params, string& result);
void    chain_api_get_account_(void *chain_ptr, string& params, string& result);
void    chain_api_get_code_(void *ptr, string& params, string& results );
void    chain_api_get_code_hash_(void *ptr, string& account, string& code_hash );
void    chain_api_get_abi_(void *ptr, string& params, string& results );
void    chain_api_get_raw_code_and_abi_(void *ptr, string& params, string& results );
void    chain_api_get_raw_abi_(void *ptr, string& params, string& results );
void    chain_api_get_table_rows_(void *chain_ptr, string& params, string& result);
void    chain_api_get_table_by_scope_(void *ptr, string& params, string& result);
void    chain_api_get_currency_balance_(void *ptr, string& params, string& result);
void    chain_api_get_currency_stats_(void *ptr, string& params, string& result);
void    chain_api_get_producers_(void *ptr, string& params, string& result);

void    chain_on_incoming_block_(void *ptr, string& packed_signed_block, uint32_t& num, string& id);


void        chain_id_(void *ptr, string& chain_id);
uint32_t    chain_fork_db_pending_head_block_num_(void *ptr);
uint32_t    chain_last_irreversible_block_num_(void *ptr);
void        chain_get_block_id_for_num_(void *ptr, uint32_t num, string& block_id);
void        chain_fetch_block_by_number_(void *ptr, uint32_t block_num, string& raw_block );
int         chain_is_building_block_(void *ptr);

int     chain_api_recover_reversible_blocks_(string& old_reversible_blocks_dir, string& new_reversible_blocks_dir, uint32_t reversible_cache_size, uint32_t truncate_at_block);
void    chain_api_repair_log_(string& blocks_dir, uint32_t truncate_at_block, string& backup_blocks_dir);
void    chain_api_get_table_rows_(void *ptr, string& params, string& results );

void*   producer_new_(void *chain_ptr, string& config);
void    producer_free_(void *ptr);
void    producer_on_incoming_block_(void *ptr, string& packed_signed_block, uint32_t& num, string& id);

int         producer_start_block_(void *ptr);
uint64_t    producer_calc_pending_block_time_(void *ptr);
uint64_t    producer_calc_pending_block_deadline_time_(void *ptr);
bool        producer_maybe_produce_block_(void *ptr);
uint64_t    producer_now_time_();
int         producer_get_pending_block_mode_(void *ptr);

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
