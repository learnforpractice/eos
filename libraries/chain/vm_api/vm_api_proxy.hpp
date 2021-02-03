#pragma once
#include <memory>
#include "../../vm_api/include/capi/types.h"

namespace eosio { namespace chain {
    class apply_context;
}}

namespace eosio { namespace chain { namespace webassembly {
    class interface;
}}}

using namespace eosio::chain;

#ifndef softfloat_types_h
   typedef struct { uint16_t v; } float16_t;
   typedef struct { uint32_t v; } float32_t;
   typedef struct { uint64_t v; } float64_t;
   typedef struct { uint64_t v[2]; } float128_t;
#endif

typedef unsigned __int128 __uint128;
typedef unsigned __int128 uint128_t;

class vm_api_proxy {
    public:
        vm_api_proxy();
        virtual ~vm_api_proxy();
        void set_apply_context(apply_context *ctx);

        //chain.h
        virtual uint32_t get_active_producers( capi_name* producers, uint32_t datalen );
        //db.h
        virtual int32_t db_store_i64(uint64_t scope, capi_name table, capi_name payer, uint64_t id,  const void* data, uint32_t len);
        virtual void db_update_i64(int32_t iterator, capi_name payer, const void* data, uint32_t len);
        virtual void db_remove_i64(int32_t iterator);
        virtual int32_t db_get_i64(int32_t iterator, void* data, uint32_t len);
        virtual int32_t db_next_i64(int32_t iterator, uint64_t* primary);
        virtual int32_t db_previous_i64(int32_t iterator, uint64_t* primary);
        virtual int32_t db_find_i64(capi_name code, uint64_t scope, capi_name table, uint64_t id);
        virtual int32_t db_lowerbound_i64(capi_name code, uint64_t scope, capi_name table, uint64_t id);
        virtual int32_t db_upperbound_i64(capi_name code, uint64_t scope, capi_name table, uint64_t id);
        virtual int32_t db_end_i64(capi_name code, uint64_t scope, capi_name table);
        virtual int32_t db_idx64_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const uint64_t* secondary);
        virtual void db_idx64_update(int32_t iterator, capi_name payer, const uint64_t* secondary);
        virtual void db_idx64_remove(int32_t iterator);
        virtual int32_t db_idx64_next(int32_t iterator, uint64_t* primary);
        virtual int32_t db_idx64_previous(int32_t iterator, uint64_t* primary);
        virtual int32_t db_idx64_find_primary(capi_name code, uint64_t scope, capi_name table, uint64_t* secondary, uint64_t primary);
        virtual int32_t db_idx64_find_secondary(capi_name code, uint64_t scope, capi_name table, const uint64_t* secondary, uint64_t* primary);
        virtual int32_t db_idx64_lowerbound(capi_name code, uint64_t scope, capi_name table, uint64_t* secondary, uint64_t* primary);
        virtual int32_t db_idx64_upperbound(capi_name code, uint64_t scope, capi_name table, uint64_t* secondary, uint64_t* primary);
        virtual int32_t db_idx64_end(capi_name code, uint64_t scope, capi_name table);
        virtual int32_t db_idx128_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const uint128_t* secondary);
        virtual void db_idx128_update(int32_t iterator, capi_name payer, const uint128_t* secondary);
        virtual void db_idx128_remove(int32_t iterator);
        virtual int32_t db_idx128_next(int32_t iterator, uint64_t* primary);
        virtual int32_t db_idx128_previous(int32_t iterator, uint64_t* primary);
        virtual int32_t db_idx128_find_primary(capi_name code, uint64_t scope, capi_name table, uint128_t* secondary, uint64_t primary);
        virtual int32_t db_idx128_find_secondary(capi_name code, uint64_t scope, capi_name table, const uint128_t* secondary, uint64_t* primary);
        virtual int32_t db_idx128_lowerbound(capi_name code, uint64_t scope, capi_name table, uint128_t* secondary, uint64_t* primary);
        virtual int32_t db_idx128_upperbound(capi_name code, uint64_t scope, capi_name table, uint128_t* secondary, uint64_t* primary);
        virtual int32_t db_idx128_end(capi_name code, uint64_t scope, capi_name table);
        virtual int32_t db_idx256_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const uint128_t* data, uint32_t data_len );
        virtual void db_idx256_update(int32_t iterator, capi_name payer, const uint128_t* data, uint32_t data_len);
        virtual void db_idx256_remove(int32_t iterator);
        virtual int32_t db_idx256_next(int32_t iterator, uint64_t* primary);
        virtual int32_t db_idx256_previous(int32_t iterator, uint64_t* primary);
        virtual int32_t db_idx256_find_primary(capi_name code, uint64_t scope, capi_name table, uint128_t* data, uint32_t data_len, uint64_t primary);
        virtual int32_t db_idx256_find_secondary(capi_name code, uint64_t scope, capi_name table, const uint128_t* data, uint32_t data_len, uint64_t* primary);
        virtual int32_t db_idx256_lowerbound(capi_name code, uint64_t scope, capi_name table, uint128_t* data, uint32_t data_len, uint64_t* primary);
        virtual int32_t db_idx256_upperbound(capi_name code, uint64_t scope, capi_name table, uint128_t* data, uint32_t data_len, uint64_t* primary);
        virtual int32_t db_idx256_end(capi_name code, uint64_t scope, capi_name table);
        virtual int32_t db_idx_double_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const double* secondary);
        virtual void db_idx_double_update(int32_t iterator, capi_name payer, const double* secondary);
        virtual void db_idx_double_remove(int32_t iterator);
        virtual int32_t db_idx_double_next(int32_t iterator, uint64_t* primary);
        virtual int32_t db_idx_double_previous(int32_t iterator, uint64_t* primary);
        virtual int32_t db_idx_double_find_primary(capi_name code, uint64_t scope, capi_name table, double* secondary, uint64_t primary);
        virtual int32_t db_idx_double_find_secondary(capi_name code, uint64_t scope, capi_name table, const double* secondary, uint64_t* primary);
        virtual int32_t db_idx_double_lowerbound(capi_name code, uint64_t scope, capi_name table, double* secondary, uint64_t* primary);
        virtual int32_t db_idx_double_upperbound(capi_name code, uint64_t scope, capi_name table, double* secondary, uint64_t* primary);
        virtual int32_t db_idx_double_end(capi_name code, uint64_t scope, capi_name table);
        virtual int32_t db_idx_long_double_store(uint64_t scope, capi_name table, capi_name payer, uint64_t id, const long double* secondary);
        virtual void db_idx_long_double_update(int32_t iterator, capi_name payer, const long double* secondary);
        virtual void db_idx_long_double_remove(int32_t iterator);
        virtual int32_t db_idx_long_double_next(int32_t iterator, uint64_t* primary);
        virtual int32_t db_idx_long_double_previous(int32_t iterator, uint64_t* primary);
        virtual int32_t db_idx_long_double_find_primary(capi_name code, uint64_t scope, capi_name table, long double* secondary, uint64_t primary);
        virtual int32_t db_idx_long_double_find_secondary(capi_name code, uint64_t scope, capi_name table, const long double* secondary, uint64_t* primary);
        virtual int32_t db_idx_long_double_lowerbound(capi_name code, uint64_t scope, capi_name table, long double* secondary, uint64_t* primary);
        virtual int32_t db_idx_long_double_upperbound(capi_name code, uint64_t scope, capi_name table, long double* secondary, uint64_t* primary);
        virtual int32_t db_idx_long_double_end(capi_name code, uint64_t scope, capi_name table);
        //permission.h
        virtual int32_t check_transaction_authorization( const char* trx_data,     uint32_t trx_size,
                                        const char* pubkeys_data, uint32_t pubkeys_size,
                                        const char* perms_data,   uint32_t perms_size
                                    );
        virtual int32_t check_permission_authorization( capi_name account,
                                        capi_name permission,
                                        const char* pubkeys_data, uint32_t pubkeys_size,
                                        const char* perms_data,   uint32_t perms_size,
                                        uint64_t delay_us
                                    );

        virtual int64_t get_permission_last_used( capi_name account, capi_name permission );
        virtual int64_t get_account_creation_time( capi_name account );
        //crypto.h
        virtual void assert_sha256( const char* data, uint32_t length, const capi_checksum256* hash );
        virtual void assert_sha1( const char* data, uint32_t length, const capi_checksum160* hash );
        virtual void assert_sha512( const char* data, uint32_t length, const capi_checksum512* hash );
        virtual void assert_ripemd160( const char* data, uint32_t length, const capi_checksum160* hash );
        virtual void sha256( const char* data, uint32_t length, capi_checksum256* hash );
        virtual void sha1( const char* data, uint32_t length, capi_checksum160* hash );
        virtual void sha512( const char* data, uint32_t length, capi_checksum512* hash );
        virtual void ripemd160( const char* data, uint32_t length, capi_checksum160* hash );
        virtual int32_t recover_key( const capi_checksum256* digest, const char* sig, uint32_t siglen, char* pub, uint32_t publen );
        virtual void assert_recover_key( const capi_checksum256* digest, const char* sig, uint32_t siglen, const char* pub, uint32_t publen );
        //action.h
        virtual uint32_t read_action_data( void* msg, uint32_t len );
        virtual uint32_t action_data_size();
        virtual void require_recipient( capi_name name );
        virtual void require_auth( capi_name name );
        virtual bool has_auth( capi_name name );
        virtual void require_auth2( capi_name name, capi_name permission );
        virtual bool is_account( capi_name name );
        virtual void send_inline(char *serialized_action, uint32_t size);
        virtual void send_context_free_inline(char *serialized_action, uint32_t size);
        virtual uint64_t  publication_time();
        virtual capi_name current_receiver();
        virtual void set_action_return_value(const char *data, uint32_t data_size);

        //print.h
        virtual void prints( const char* cstr );
        virtual void prints_l( const char* cstr, uint32_t len);
        virtual void printi( int64_t value );
        virtual void printui( uint64_t value );
        virtual void printi128( const int128_t* value );
        virtual void printui128( const uint128_t* value );
        virtual void printsf(float value);
        virtual void printdf(double value);
        virtual void printqf(const long double* value);
        virtual void printn( uint64_t name );
        virtual void printhex( const void* data, uint32_t datalen );
        //system.h
        virtual void  eosio_assert( uint32_t test, const char* msg );
        virtual void  eosio_assert_message( uint32_t test, const char* msg, uint32_t msg_len );
        virtual void  eosio_assert_code( uint32_t test, uint64_t code );
        virtual void eosio_exit( int32_t code );
        virtual uint64_t  current_time();
        virtual bool is_feature_activated( const capi_checksum256* feature_digest );
        virtual capi_name get_sender();
        //privileged.h
        virtual void get_resource_limits( capi_name account, int64_t* ram_bytes, int64_t* net_weight, int64_t* cpu_weight );
        virtual void set_resource_limits( capi_name account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight );
        virtual int64_t set_proposed_producers( const char *producer_data, uint32_t producer_data_size );
        virtual int64_t set_proposed_producers_ex( uint64_t producer_data_format, const char *producer_data, uint32_t producer_data_size );
        virtual bool is_privileged( capi_name account );
        virtual void set_privileged( capi_name account, bool is_priv );
        virtual void set_blockchain_parameters_packed( const char* data, uint32_t datalen );
        virtual uint32_t get_blockchain_parameters_packed( char* data, uint32_t datalen );
        virtual void preactivate_feature( const capi_checksum256* feature_digest );

        virtual int is_feature_active( int64_t feature_name );
        virtual void activate_feature( int64_t feature_name );
        virtual void set_resource_limit( uint64_t account, uint64_t resource, int64_t limit );
        virtual int64_t get_resource_limit( uint64_t account, uint64_t resource );
        virtual uint32_t get_wasm_parameters_packed( char *packed_parameters, uint32_t size, uint32_t max_version );
        virtual void set_wasm_parameters_packed( const char *packed_parameters, uint32_t size );
        virtual uint32_t get_parameters_packed( const char *packed_parameter_ids, uint32_t size1, char *packed_parameters, uint32_t size2);
        virtual void set_parameters_packed( const char *packed_parameters, uint32_t size );
        virtual uint32_t get_kv_parameters_packed( char *packed_kv_parameters, uint32_t size, uint32_t max_version );
        virtual void set_kv_parameters_packed( const char *packed_kv_parameters, uint32_t size );

        //transaction.h
        virtual void send_deferred(const uint128_t sender_id, capi_name payer, const char *serialized_transaction, uint32_t size, uint32_t replace_existing = 0);
        virtual int32_t cancel_deferred(const uint128_t sender_id);
        virtual uint32_t read_transaction(char *buffer, uint32_t size);
        virtual uint32_t transaction_size();
        virtual int32_t tapos_block_num();
        virtual int32_t tapos_block_prefix();
        virtual uint32_t expiration();
        virtual int32_t get_action( uint32_t type, uint32_t index, char* buff, uint32_t size );
        virtual int32_t get_context_free_data( uint32_t index, char* buff, uint32_t size );

        virtual void call_contract(capi_name contract, const char* args, uint32_t size1);
        virtual uint32_t call_contract_get_args(char* args, uint32_t size1);
        virtual uint32_t call_contract_set_results(const char* result, uint32_t size1);
        virtual uint32_t call_contract_get_results(char* result, uint32_t size1);

    private:
        std::unique_ptr<webassembly::interface> _interface;
};

typedef void (*fn_init_vm_api)(vm_api_proxy *proxy);
extern "C" void init_vm_api(vm_api_proxy *proxy);
extern "C" vm_api_proxy* get_vm_api();

