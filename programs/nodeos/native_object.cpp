#include <eosio/net_plugin/protocol.hpp>
#include <Python.h>
#include <string>
#include <vector>
#include <fc/io/json.hpp>
#include <fc/io/raw.hpp>
#include <fc/variant.hpp>
#include <eosio/chain/controller.hpp>
#include <eosio/producer_plugin/producer_plugin.hpp>
#include <eosio/chain/genesis_state.hpp>

#include "native_object.hpp"

using namespace std;
using namespace eosio;
using namespace eosio::chain;

template<typename T>
static void pack_cpp_object(string& msg, string& packed_message)
{
    try {
        auto _msg = fc::json::from_string(msg).as<T>();
        auto _packed_message = fc::raw::pack<T>(_msg);
        packed_message = string(_packed_message.data(), _packed_message.size());
    } FC_LOG_AND_DROP();
}

template<typename T>
static void unpack_cpp_object(string& packed_message, string& msg) {
    try {
        T _msg;
        fc::datastream<const char*> ds( packed_message.c_str(), packed_message.size() );
        fc::raw::unpack(ds, _msg);

//        vector<char> _packed_message(packed_message.c_str(), packed_message.c_str()+packed_message.size());
//        auto _msg = fc::raw::unpack<T>(_packed_message);
        auto v = fc::variant(_msg);
//        return;
        msg = fc::json::to_string(v);
    }FC_LOG_AND_DROP();
}

#define PACK_CPP_OBJECT(obj) \
    case obj ## _type: \
        pack_cpp_object<obj>(msg, packed_message); \
        break;

#define UNPACK_CPP_OBJECT(obj) \
    case obj ## _type: \
        unpack_cpp_object<obj>(packed_message, msg); \
        break;

using controller_config = eosio::chain::controller::config;
using producer_params = producer_plugin::producer_params;

void pack_native_object_(int type, string& msg, string& packed_message) {
    switch(type) {
        PACK_CPP_OBJECT(handshake_message)
        PACK_CPP_OBJECT(chain_size_message)
        PACK_CPP_OBJECT(go_away_message)
        PACK_CPP_OBJECT(time_message)
        PACK_CPP_OBJECT(notice_message)
        PACK_CPP_OBJECT(request_message)
        PACK_CPP_OBJECT(sync_request_message)
        PACK_CPP_OBJECT(signed_block)
        PACK_CPP_OBJECT(packed_transaction)
        PACK_CPP_OBJECT(controller_config)
        PACK_CPP_OBJECT(producer_params)
        PACK_CPP_OBJECT(genesis_state)
    }
}

void unpack_native_object_(int type, string& packed_message, string& msg) {
    switch(type) {
        UNPACK_CPP_OBJECT(handshake_message)
        UNPACK_CPP_OBJECT(chain_size_message)
        UNPACK_CPP_OBJECT(go_away_message)
        UNPACK_CPP_OBJECT(time_message)
        UNPACK_CPP_OBJECT(notice_message)
        UNPACK_CPP_OBJECT(request_message)
        UNPACK_CPP_OBJECT(sync_request_message)
        UNPACK_CPP_OBJECT(signed_block)
        UNPACK_CPP_OBJECT(packed_transaction)
        UNPACK_CPP_OBJECT(controller_config)
        UNPACK_CPP_OBJECT(producer_params)
        UNPACK_CPP_OBJECT(genesis_state)
    }
}

