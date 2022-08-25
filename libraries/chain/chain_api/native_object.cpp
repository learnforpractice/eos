#include <string>
#include <vector>
#include <fc/io/json.hpp>
#include <fc/io/raw.hpp>
#include <fc/variant.hpp>
#include <eosio/chain/controller.hpp>
#include <eosio/chain/genesis_state.hpp>
#include <eosio/chain/abi_def.hpp>

#include "../../../plugins/net_plugin/include/eosio/net_plugin/protocol.hpp"

#include "native_object.hpp"
#include "chain_macro.hpp"

using namespace std;
using namespace eosio;
using namespace eosio::chain;

namespace fc {
   void to_variant(const boost::filesystem::path& p, fc::variant& v);
   void from_variant(const fc::variant& v, boost::filesystem::path& p);
}

namespace eosio {
    struct go_away_message_ex {
        go_away_message_ex(go_away_reason r = no_reason) : reason(uint32_t(r)), node_id() {}
        uint32_t reason = uint32_t(no_reason);
        fc::sha256 node_id; ///< for duplicate notification
    };

    template<typename T>
    struct select_ids_ex {
        select_ids_ex() : mode(none),pending(0),ids() {}
        // id_list_modes  mode{none};
        uint32_t       mode{uint32_t(none)};
        uint32_t       pending{0};
        vector<T>      ids;
        bool           empty () const { return (mode == none || ids.empty()); }
    };

    using ordered_txn_ids_ex = select_ids_ex<transaction_id_type>;
    using ordered_blk_ids_ex = select_ids_ex<block_id_type>;

    struct notice_message_ex {
        notice_message_ex() : known_trx(), known_blocks() {}
        ordered_txn_ids_ex known_trx;
        ordered_blk_ids_ex known_blocks;
    };

    struct request_message_ex {
        request_message_ex() : req_trx(), req_blocks() {}
        ordered_txn_ids_ex req_trx;
        ordered_blk_ids_ex req_blocks;
    };
}

FC_REFLECT( eosio::go_away_message_ex, (reason)(node_id) )
FC_REFLECT( eosio::select_ids_ex<fc::sha256>, (mode)(pending)(ids) )
FC_REFLECT( eosio::notice_message_ex, (known_trx)(known_blocks) )
FC_REFLECT( eosio::request_message_ex, (req_trx)(req_blocks) )


template<typename T>
static void pack_cpp_object(string& msg, vector<char>& packed_message)
{
    try {
        auto _msg = fc::json::from_string(msg).as<T>();
        packed_message = fc::raw::pack<T>(_msg);
    } CATCH_AND_LOG_EXCEPTION();
}

template<typename T>
static void unpack_cpp_object(string& packed_message, string& result) {
    try {
        T obj;
        fc::datastream<const char*> ds( packed_message.c_str(), packed_message.size() );
        fc::raw::unpack(ds, obj);
        result = fc::json::to_string(obj, fc::time_point::maximum());
    }CATCH_AND_LOG_EXCEPTION();
}

#define PACK_CPP_OBJECT(obj) \
    case obj ## _type: \
        pack_cpp_object<obj>(msg, packed_message); \
        break;

#define UNPACK_CPP_OBJECT(obj) \
    case obj ## _type: \
        unpack_cpp_object<obj>(packed_message, msg); \
        break;

#define PACK_CPP_OBJECT_EX(obj) \
    case obj ## _type: \
        pack_cpp_object<obj ## _ex>(msg, packed_message); \
        break;

#define UNPACK_CPP_OBJECT_EX(obj) \
    case obj ## _type: \
        unpack_cpp_object<obj ## _ex>(packed_message, msg); \
        break;

void pack_native_object_(int type, string& msg, vector<char>& packed_message) {
    switch(type) {
        PACK_CPP_OBJECT(handshake_message)
        PACK_CPP_OBJECT(chain_size_message)
        PACK_CPP_OBJECT_EX(go_away_message)
        PACK_CPP_OBJECT(time_message)
        PACK_CPP_OBJECT_EX(notice_message)
        PACK_CPP_OBJECT_EX(request_message)
        PACK_CPP_OBJECT(sync_request_message)
        PACK_CPP_OBJECT(signed_block_v0)
        PACK_CPP_OBJECT(packed_transaction_v0)
        PACK_CPP_OBJECT(signed_block)
        PACK_CPP_OBJECT(trx_message_v1)
        PACK_CPP_OBJECT(genesis_state)
        PACK_CPP_OBJECT(abi_def)
        PACK_CPP_OBJECT(transaction)
    }
}

void unpack_native_object_(int type, string& packed_message, string& msg) {
    switch(type) {
        UNPACK_CPP_OBJECT(handshake_message)
        UNPACK_CPP_OBJECT(chain_size_message)
        UNPACK_CPP_OBJECT_EX(go_away_message)
        UNPACK_CPP_OBJECT(time_message)
        UNPACK_CPP_OBJECT_EX(notice_message)
        UNPACK_CPP_OBJECT_EX(request_message)
        UNPACK_CPP_OBJECT(sync_request_message)
        UNPACK_CPP_OBJECT(signed_block_v0)
        UNPACK_CPP_OBJECT(packed_transaction_v0)
        UNPACK_CPP_OBJECT(signed_block)
        UNPACK_CPP_OBJECT(trx_message_v1)
        UNPACK_CPP_OBJECT(genesis_state)
        UNPACK_CPP_OBJECT(abi_def)
        UNPACK_CPP_OBJECT(transaction)
    }
}
