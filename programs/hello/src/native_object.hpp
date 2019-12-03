#pragma once
#include <string>
using namespace std;

void pack_native_object_(int type, string& msg, string& packed_message);
void unpack_native_object_(int type, string& packed_message, string& msg);

void *chain_new_(string& config, string& protocol_features_dir);
void chain_free_(void *ptr);
void chain_api_get_info_(void *chain_ptr, string& info);
void chain_api_get_table_rows_(void *chain_ptr, string& params, string& result);
void chain_api_get_account_(void *chain_ptr, string& params, string& result);

void chain_on_incoming_block_(void *ptr, string& packed_signed_block, uint32_t& num, string& id);


void chain_id_(void *ptr, string& chain_id);
uint32_t chain_fork_db_pending_head_block_num_(void *ptr);
uint32_t chain_last_irreversible_block_num_(void *ptr);
void chain_get_block_id_for_num_(void *ptr, uint32_t num, string& block_id);

enum {
    handshake_message_type,
    chain_size_message_type,
    go_away_message_type,
    time_message_type,
    notice_message_type,
    request_message_type,
    sync_request_message_type,
    signed_block_type,         // which = 7
    packed_transaction_type
};
