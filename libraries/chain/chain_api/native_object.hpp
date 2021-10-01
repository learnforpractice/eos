#pragma once
#include <string>
#include <vector>

using namespace std;

void pack_native_object_(int type, string& msg, vector<char>& packed_obj);
void unpack_native_object_(int type, string& packed_obj, string& result);

enum {
    handshake_message_type,
    chain_size_message_type,
    go_away_message_type,
    time_message_type,
    notice_message_type,
    request_message_type,
    sync_request_message_type,
    signed_block_v0_type,         // which = 7
    packed_transaction_v0_type,   // which = 8
    signed_block_type,            // which = 9
    trx_message_v1_type,          // which = 10

    genesis_state_type,           // 11
    abi_def_type,                 // 12
    transaction_type              // 13
};
