#pragma once
#include <memory>

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

class vm_api_proxy {
    public:
        vm_api_proxy();
        virtual ~vm_api_proxy();
        void set_apply_context(apply_context *ctx);

        virtual size_t read_action_data(char *data, size_t data_size);
        virtual size_t action_data_size();
        virtual void set_action_return_value(const char *data, size_t data_size);

        virtual void require_recipient(uint64_t name);
        virtual void require_auth(uint64_t name);
        virtual void require_auth2(uint64_t name, uint64_t permission);
        virtual bool has_auth(uint64_t name);
        virtual bool is_account(uint64_t name);
        virtual void send_inline(const char *serialized_action, size_t size);
        virtual void send_context_free_inline(const char *serialized_action, size_t size);
        virtual uint64_t  publication_time();
        virtual uint64_t current_receiver();
        virtual size_t get_active_producers(uint64_t *producers, size_t datalen);

        virtual void assert_sha256(const char *data, size_t length, const uint8_t *hash, size_t hash_size);
        virtual void assert_sha1(const char *data, size_t length, const uint8_t *hash, size_t hash_size);
        virtual void assert_sha512(const char *data, size_t length, const uint8_t *hash, size_t hash_size);
        virtual void assert_ripemd160(const char *data, size_t length, const uint8_t *hash, size_t hash_size);
        virtual void assert_recover_key(const char *digest, size_t digest_size, const char *sig, size_t siglen, const char *pub, size_t publen);

        virtual void sha256(const char *data, size_t length, uint8_t *hash, size_t hash_size);
        virtual void sha1(const char *data, size_t length, uint8_t *hash, size_t hash_size);
        virtual void sha512(const char *data, size_t length, uint8_t *hash, size_t hash_size);
        virtual void ripemd160(const char *data, size_t length, uint8_t *hash, size_t hash_size);
        virtual int recover_key(const char *digest, size_t digest_size, const char *sig, size_t siglen, char *pub, size_t publen);
        // virtual int sha3(const char *data, int size, char *result, int size2);

        virtual int32_t db_store_i64(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id,  const char *data, size_t len);
        virtual void db_update_i64(int32_t iterator, uint64_t payer, const char *data, size_t len);
        virtual void db_remove_i64(int32_t iterator);
        virtual int32_t db_get_i64(int32_t iterator, char *data, size_t len);
        virtual int32_t db_next_i64(int32_t iterator, uint64_t *primary);
        virtual int32_t db_previous_i64(int32_t iterator, uint64_t *primary);
        virtual int32_t db_find_i64(uint64_t code, uint64_t scope, uint64_t table, uint64_t id);
        virtual int32_t db_lowerbound_i64(uint64_t code, uint64_t scope, uint64_t table, uint64_t id);
        virtual int32_t db_upperbound_i64(uint64_t code, uint64_t scope, uint64_t table, uint64_t id);
        virtual int32_t db_end_i64(uint64_t code, uint64_t scope, uint64_t table);

#if 0
        virtual int db_store_i256(uint64_t scope, uint64_t table, uint64_t payer, void *id, int size, const char *buffer, size_t buffer_size);
        virtual void db_update_i256(int iterator, uint64_t payer, const char *buffer, size_t buffer_size);
        virtual void db_remove_i256(int iterator);
        virtual int db_get_i256(int iterator, char *buffer, size_t buffer_size);
        virtual int db_find_i256(uint64_t code, uint64_t scope, uint64_t table, void *id, size_t size);
        virtual int db_previous_i256(int iterator, void *primary, size_t size);
        virtual int db_next_i256(int iterator, void *primary, size_t size);
        virtual int db_upperbound_i256(uint64_t code, uint64_t scope, uint64_t table, void *id, int size);
        virtual int db_lowerbound_i256(uint64_t code, uint64_t scope, uint64_t table, void *id, int size);
        virtual int32_t db_end_i256(uint64_t code, uint64_t scope, uint64_t table);
        virtual size_t db_get_table_row_count(uint64_t code, uint64_t scope, uint64_t table);
#endif

        virtual int32_t db_idx64_store(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const uint64_t *secondary);
        virtual void db_idx64_update(int32_t iterator, uint64_t payer, const uint64_t *secondary);
        virtual void db_idx64_remove(int32_t iterator);


        virtual int32_t db_idx64_next(int32_t iterator, uint64_t *primary);
        virtual int32_t db_idx64_previous(int32_t iterator, uint64_t *primary);
        virtual int32_t db_idx64_find_primary(uint64_t code, uint64_t scope, uint64_t table, uint64_t *secondary, uint64_t primary);
        virtual int32_t db_idx64_find_secondary(uint64_t code, uint64_t scope, uint64_t table, const uint64_t *secondary, uint64_t *primary);
        virtual int32_t db_idx64_lowerbound(uint64_t code, uint64_t scope, uint64_t table, uint64_t *secondary, uint64_t *primary);
        virtual int32_t db_idx64_upperbound(uint64_t code, uint64_t scope, uint64_t table, uint64_t *secondary, uint64_t *primary);
        virtual int32_t db_idx64_end(uint64_t code, uint64_t scope, uint64_t table);

        virtual int32_t db_idx128_store(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const __uint128 *secondary);
        virtual void db_idx128_update(int32_t iterator, uint64_t payer, const __uint128 *secondary);
        virtual void db_idx128_remove(int32_t iterator);
        virtual int32_t db_idx128_next(int32_t iterator, uint64_t *primary);
        virtual int32_t db_idx128_previous(int32_t iterator, uint64_t *primary);
        virtual int32_t db_idx128_find_primary(uint64_t code, uint64_t scope, uint64_t table, __uint128 *secondary, uint64_t primary);
        virtual int32_t db_idx128_find_secondary(uint64_t code, uint64_t scope, uint64_t table, const __uint128 *secondary, uint64_t *primary);
        virtual int32_t db_idx128_lowerbound(uint64_t code, uint64_t scope, uint64_t table, __uint128 *secondary, uint64_t *primary);
        virtual int32_t db_idx128_upperbound(uint64_t code, uint64_t scope, uint64_t table, __uint128 *secondary, uint64_t *primary);

        virtual int32_t db_idx128_end(uint64_t code, uint64_t scope, uint64_t table);
        virtual int32_t db_idx256_store(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const __uint128 *data, size_t data_len);
        virtual void db_idx256_update(int32_t iterator, uint64_t payer, const __uint128 *data, size_t data_len);
        virtual void db_idx256_remove(int32_t iterator);
        virtual int32_t db_idx256_next(int32_t iterator, uint64_t *primary);

        virtual int32_t db_idx256_previous(int32_t iterator, uint64_t *primary);
        virtual int32_t db_idx256_find_primary(uint64_t code, uint64_t scope, uint64_t table, __uint128 *data, size_t data_len, uint64_t primary);
        virtual int32_t db_idx256_find_secondary(uint64_t code, uint64_t scope, uint64_t table, const __uint128 *data, size_t data_len, uint64_t *primary);
        virtual int32_t db_idx256_lowerbound(uint64_t code, uint64_t scope, uint64_t table, __uint128 *data, size_t data_len, uint64_t *primary);
        virtual int32_t db_idx256_upperbound(uint64_t code, uint64_t scope, uint64_t table, __uint128 *data, size_t data_len, uint64_t *primary);
        virtual int32_t db_idx256_end(uint64_t code, uint64_t scope, uint64_t table);

        virtual int32_t db_idx_double_store(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const float64_t *secondary);
        virtual void db_idx_double_update(int32_t iterator, uint64_t payer, const float64_t *secondary);
        virtual void db_idx_double_remove(int32_t iterator);
        virtual int32_t db_idx_double_next(int32_t iterator, uint64_t *primary);
        virtual int32_t db_idx_double_previous(int32_t iterator, uint64_t *primary);
        virtual int32_t db_idx_double_find_primary(uint64_t code, uint64_t scope, uint64_t table, float64_t *secondary, uint64_t primary);
        virtual int32_t db_idx_double_find_secondary(uint64_t code, uint64_t scope, uint64_t table, const float64_t *secondary, uint64_t *primary);
        virtual int32_t db_idx_double_lowerbound(uint64_t code, uint64_t scope, uint64_t table, float64_t *secondary, uint64_t *primary);
        virtual int32_t db_idx_double_upperbound(uint64_t code, uint64_t scope, uint64_t table, float64_t *secondary, uint64_t *primary);
        virtual int32_t db_idx_double_end(uint64_t code, uint64_t scope, uint64_t table);

        virtual int32_t db_idx_long_double_store(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const float128_t *secondary);
        virtual void db_idx_long_double_update(int32_t iterator, uint64_t payer, const float128_t *secondary);
        virtual void db_idx_long_double_remove(int32_t iterator);
        virtual int32_t db_idx_long_double_next(int32_t iterator, uint64_t *primary);
        virtual int32_t db_idx_long_double_previous(int32_t iterator, uint64_t *primary);
        virtual int32_t db_idx_long_double_find_primary(uint64_t code, uint64_t scope, uint64_t table, float128_t *secondary, uint64_t primary);
        virtual int32_t db_idx_long_double_find_secondary(uint64_t code, uint64_t scope, uint64_t table, const float128_t *secondary, uint64_t *primary);
        virtual int32_t db_idx_long_double_lowerbound(uint64_t code, uint64_t scope, uint64_t table, float128_t *secondary, uint64_t *primary);
        virtual int32_t db_idx_long_double_upperbound(uint64_t code, uint64_t scope, uint64_t table, float128_t *secondary, uint64_t *primary);
        virtual int32_t db_idx_long_double_end(uint64_t code, uint64_t scope, uint64_t table);


        virtual bool check_transaction_authorization(const char *trx_data,     size_t trx_size,
                                        const char *pubkeys_data, size_t pubkeys_size,
                                        const char *perms_data,   size_t perms_size
                                    );
        virtual bool check_permission_authorization(uint64_t account,
                                        uint64_t permission,
                                        const char *pubkeys_data, size_t pubkeys_size,
                                        const char *perms_data,   size_t perms_size,
                                        uint64_t delay_us
                                    );
        virtual int64_t get_permission_last_used(uint64_t account, uint64_t permission);
        virtual int64_t get_account_creation_time(uint64_t account);

        virtual void prints(const char *cstr);
        virtual void prints_l(const char *cstr, size_t len);
        virtual void printi(int64_t value);
        virtual void printui(uint64_t value);
        virtual void printi128(const __int128 *value);
        virtual void printui128(const __uint128 *value);
        virtual void printsf(float32_t value);
        virtual void printdf(float64_t value);
        virtual void printqf(const float128_t *value);
        virtual void printn(uint64_t name);
        virtual void printhex(const char *data, size_t datalen);

        virtual void set_resource_limits(uint64_t account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight);
        virtual void get_resource_limits(uint64_t account, int64_t *ram_bytes, int64_t *net_weight, int64_t *cpu_weight);
        virtual int64_t set_proposed_producers(const char *producer_data, size_t producer_data_size);
        virtual int64_t set_proposed_producers_ex(uint64_t packed_producer_format, const char *packed_producer_schedule, size_t datalen);
        virtual bool is_privileged(uint64_t account);
        virtual void set_privileged(uint64_t account, bool is_priv);
        virtual void set_blockchain_parameters_packed(const char *data, size_t datalen);
        virtual uint32_t get_blockchain_parameters_packed(char *data, size_t datalen);
        virtual void activate_feature(int64_t f);

        virtual void eosio_abort();
        virtual void eosio_assert(bool test, const char *msg);
        virtual void eosio_assert_message(bool test, const char *msg, size_t msg_len);
        virtual void eosio_assert_code(bool test, uint64_t code);
        virtual void eosio_exit(int32_t code);
        virtual uint64_t  current_time();

        // virtual size_t  now();
        // virtual void checktime();
        // virtual void check_context_free(bool context_free);

        virtual void send_deferred(const __uint128 *sender_id, uint64_t payer, const char *serialized_transaction, size_t size, uint32_t replace_existing);
        virtual bool cancel_deferred(const __uint128 *sender_id);

        virtual size_t read_transaction(char *buffer, size_t size);
        virtual size_t transaction_size();
        virtual int32_t expiration();
        virtual int32_t tapos_block_num();
        virtual int32_t tapos_block_prefix();
        virtual int32_t get_action(uint32_t type, uint32_t index, char *buff, size_t size);

        virtual int32_t get_context_free_data(uint32_t index, char *buff, size_t size);

#if 0
        virtual void token_create(uint64_t issuer, int64_t maximum_supply, uint64_t symbol);
        virtual void token_issue(uint64_t to, int64_t quantity, uint64_t symbol, const char *memo, size_t size2);
        virtual void token_transfer(uint64_t from, uint64_t to, int64_t quantity, uint64_t symbol, const char *memo, size_t size2);
        virtual void token_open(uint64_t owner, uint64_t _symbol, uint64_t ram_payer);
        virtual void token_retire(int64_t amount, uint64_t _symbol, const char *memo, size_t memo_size);
        virtual void token_close(uint64_t owner, uint64_t _symbol);
#endif

        virtual void call_contract(uint64_t contract, const char *args, size_t args_size);
        virtual size_t call_contract_get_args(char *args, size_t args_size);
        virtual size_t call_contract_set_results(const char *result, size_t size);
        virtual size_t call_contract_get_results(char *result, size_t size);

        virtual bool is_feature_activated(const char *digest, size_t size);
        virtual void preactivate_feature(const char *digest, size_t size);

        virtual uint64_t get_sender();

        virtual void __ashlti3(__int128 *ret, uint64_t low, uint64_t high, uint32_t shift);
        virtual void __ashrti3(__int128 *ret, uint64_t low, uint64_t high, uint32_t shift);
        virtual void __lshlti3(__int128 *ret, uint64_t low, uint64_t high, uint32_t shift);
        virtual void __lshrti3(__int128 *ret, uint64_t low, uint64_t high, uint32_t shift);
        virtual void __divti3(__int128 *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb);
        virtual void __udivti3(unsigned __int128 *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb);
        virtual void __multi3(__int128 *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb);
        virtual void __modti3(__int128 *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb);
        virtual void __umodti3(unsigned __int128 *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb);
        // arithmetic long double
        virtual void __addtf3( float128_t *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb );
        virtual void __subtf3( float128_t *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb );
        virtual void __multf3( float128_t *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb );
        virtual void __divtf3( float128_t *ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb );
        virtual void __negtf2( float128_t *ret, uint64_t la, uint64_t ha );
        // conversion long double
        virtual void __extendsftf2( float128_t *ret, float f );
        virtual void __extenddftf2( float128_t *ret, double d );
        virtual double __trunctfdf2( uint64_t l, uint64_t h );
        virtual float __trunctfsf2( uint64_t l, uint64_t h );
        virtual int32_t __fixtfsi( uint64_t l, uint64_t h );
        virtual int64_t __fixtfdi( uint64_t l, uint64_t h );
        virtual void __fixtfti( __int128 *ret, uint64_t l, uint64_t h );
        virtual uint32_t __fixunstfsi( uint64_t l, uint64_t h );
        virtual uint64_t __fixunstfdi( uint64_t l, uint64_t h );
        virtual void __fixunstfti( unsigned __int128 *ret, uint64_t l, uint64_t h );
        virtual void __fixsfti( __int128 *ret, float a );
        virtual void __fixdfti( __int128 *ret, double a );
        virtual void __fixunssfti( unsigned __int128 *ret, float a );
        virtual void __fixunsdfti( unsigned __int128 *ret, double a );
        virtual double __floatsidf( int32_t i );
        virtual void __floatsitf( float128_t *ret, int32_t i );
        virtual void __floatditf( float128_t *ret, uint64_t a );
        virtual void __floatunsitf( float128_t *ret, uint32_t i );
        virtual void __floatunditf( float128_t *ret, uint64_t a );
        virtual double __floattidf( uint64_t l, uint64_t h );
        virtual double __floatuntidf( uint64_t l, uint64_t h );
        virtual int __eqtf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb );
        virtual int __netf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb );
        virtual int __getf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb );
        virtual int __gttf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb );
        virtual int __letf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb );
        virtual int __lttf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb );
        virtual int __cmptf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb );
        virtual int __unordtf2( uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb );

        virtual int is_feature_active( int64_t feature_name );

        virtual void set_resource_limit( uint64_t account, uint64_t resource, int64_t limit );
        virtual int64_t get_resource_limit( uint64_t account, uint64_t resource );

        virtual uint32_t get_wasm_parameters_packed( char *packed_parameters, size_t size, uint32_t max_version );
        virtual void set_wasm_parameters_packed( const char *packed_parameters, size_t size );
        virtual uint32_t get_parameters_packed( const char *packed_parameter_ids, size_t size1, char *packed_parameters, size_t size2);
        virtual void set_parameters_packed( const char *packed_parameters, size_t size );
        virtual uint32_t get_kv_parameters_packed( char *packed_kv_parameters, size_t size, uint32_t max_version );
        virtual void set_kv_parameters_packed( const char *packed_kv_parameters, size_t size );

    private:
        std::unique_ptr<webassembly::interface> _interface;
};
