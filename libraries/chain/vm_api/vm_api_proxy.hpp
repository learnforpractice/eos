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
        void set_apply_context(apply_context* ctx);

        virtual size_t read_action_data(char* msg, size_t len);
        virtual size_t action_data_size();
#if 0
        void require_recipient(uint64_t name);
        void require_auth(uint64_t name);
        void require_auth2(uint64_t name, uint64_t permission);
        bool has_auth(uint64_t name);
        bool is_account(uint64_t name);
        void send_inline(const char *serialized_action, size_t size);
        void send_context_free_inline(const char *serialized_action, size_t size);
        uint64_t  publication_time();
        uint64_t current_receiver();
        size_t get_active_producers(uint64_t* producers, size_t datalen);

        void assert_sha256(const char* data, size_t length, const uint8_t *hash, size_t hash_size);
        void assert_sha1(const char* data, size_t length, const uint8_t *hash, size_t hash_size);
        void assert_sha512(const char* data, size_t length, const uint8_t *hash, size_t hash_size);
        void assert_ripemd160(const char* data, size_t length, const uint8_t *hash, size_t hash_size);
        void assert_recover_key(const uint8_t *digest, size_t digest_size, const char* sig, size_t siglen, const char* pub, size_t publen);

        void sha256(const char* data, size_t length, uint8_t *hash, size_t hash_size);
        void sha1(const char* data, size_t length, uint8_t *hash, size_t hash_size);
        void sha512(const char* data, size_t length, uint8_t *hash, size_t hash_size);
        void ripemd160(const char* data, size_t length, uint8_t *hash, size_t hash_size);
        int recover_key(const uint8_t *digest, size_t digest_size, const char* sig, size_t siglen, char* pub, size_t publen);
        int sha3(const char* data, int size, char* result, int size2);

        int32_t db_store_i64(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id,  const char* data, size_t len);
        void db_update_i64(int32_t iterator, uint64_t payer, const char* data, size_t len);
        void db_remove_i64(int32_t iterator);
        int32_t db_get_i64(int32_t iterator, void* data, size_t len);
        int32_t db_next_i64(int32_t iterator, uint64_t* primary);
        int32_t db_previous_i64(int32_t iterator, uint64_t* primary);
        int32_t db_find_i64(uint64_t code, uint64_t scope, uint64_t table, uint64_t id);
        int32_t db_lowerbound_i64(uint64_t code, uint64_t scope, uint64_t table, uint64_t id);
        int32_t db_upperbound_i64(uint64_t code, uint64_t scope, uint64_t table, uint64_t id);
        int32_t db_end_i64(uint64_t code, uint64_t scope, uint64_t table);

        int db_store_i256(uint64_t scope, uint64_t table, uint64_t payer, void* id, int size, const char* buffer, size_t buffer_size);
        void db_update_i256(int iterator, uint64_t payer, const char* buffer, size_t buffer_size);
        void db_remove_i256(int iterator);
        int db_get_i256(int iterator, char* buffer, size_t buffer_size);
        int db_find_i256(uint64_t code, uint64_t scope, uint64_t table, void* id, size_t size);
        int db_previous_i256(int iterator, void* primary, size_t size);
        int db_next_i256(int iterator, void* primary, size_t size);
        int db_upperbound_i256(uint64_t code, uint64_t scope, uint64_t table, void* id, int size);
        int db_lowerbound_i256(uint64_t code, uint64_t scope, uint64_t table, void* id, int size);
        int32_t db_end_i256(uint64_t code, uint64_t scope, uint64_t table);

        size_t db_get_table_row_count(uint64_t code, uint64_t scope, uint64_t table);

        int32_t db_idx64_store(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const uint64_t* secondary);
        void db_idx64_update(int32_t iterator, uint64_t payer, const uint64_t* secondary);
        void db_idx64_remove(int32_t iterator);


        int32_t db_idx64_next(int32_t iterator, uint64_t* primary);
        int32_t db_idx64_previous(int32_t iterator, uint64_t* primary);
        int32_t db_idx64_find_primary(uint64_t code, uint64_t scope, uint64_t table, uint64_t* secondary, uint64_t primary);
        int32_t db_idx64_find_secondary(uint64_t code, uint64_t scope, uint64_t table, const uint64_t* secondary, uint64_t* primary);
        int32_t db_idx64_lowerbound(uint64_t code, uint64_t scope, uint64_t table, uint64_t* secondary, uint64_t* primary);
        int32_t db_idx64_upperbound(uint64_t code, uint64_t scope, uint64_t table, uint64_t* secondary, uint64_t* primary);
        int32_t db_idx64_end(uint64_t code, uint64_t scope, uint64_t table);

        int32_t db_idx128_store(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const __uint128* secondary);
        void db_idx128_update(int32_t iterator, uint64_t payer, const __uint128* secondary);
        void db_idx128_remove(int32_t iterator);
        int32_t db_idx128_next(int32_t iterator, uint64_t* primary);
        int32_t db_idx128_previous(int32_t iterator, uint64_t* primary);
        int32_t db_idx128_find_primary(uint64_t code, uint64_t scope, uint64_t table, __uint128* secondary, uint64_t primary);
        int32_t db_idx128_find_secondary(uint64_t code, uint64_t scope, uint64_t table, const __uint128* secondary, uint64_t* primary);
        int32_t db_idx128_lowerbound(uint64_t code, uint64_t scope, uint64_t table, __uint128* secondary, uint64_t* primary);
        int32_t db_idx128_upperbound(uint64_t code, uint64_t scope, uint64_t table, __uint128* secondary, uint64_t* primary);

        int32_t db_idx128_end(uint64_t code, uint64_t scope, uint64_t table);
        int32_t db_idx256_store(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const __uint128* data, size_t data_len);
        void db_idx256_update(int32_t iterator, uint64_t payer, const void* data, size_t data_len);
        void db_idx256_remove(int32_t iterator);
        int32_t db_idx256_next(int32_t iterator, uint64_t* primary);

        int32_t db_idx256_previous(int32_t iterator, uint64_t* primary);
        int32_t db_idx256_find_primary(uint64_t code, uint64_t scope, uint64_t table, __uint128* data, size_t data_len, uint64_t primary);
        int32_t db_idx256_find_secondary(uint64_t code, uint64_t scope, uint64_t table, const __uint128* data, size_t data_len, uint64_t* primary);
        int32_t db_idx256_lowerbound(uint64_t code, uint64_t scope, uint64_t table, __uint128* data, size_t data_len, uint64_t* primary);
        int32_t db_idx256_upperbound(uint64_t code, uint64_t scope, uint64_t table, __uint128* data, size_t data_len, uint64_t* primary);
        int32_t db_idx256_end(uint64_t code, uint64_t scope, uint64_t table);

        int32_t db_idx_double_store(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const float64_t* secondary);
        void db_idx_double_update(int32_t iterator, uint64_t payer, const float64_t* secondary);
        void db_idx_double_remove(int32_t iterator);
        int32_t db_idx_double_next(int32_t iterator, uint64_t* primary);
        int32_t db_idx_double_previous(int32_t iterator, uint64_t* primary);
        int32_t db_idx_double_find_primary(uint64_t code, uint64_t scope, uint64_t table, float64_t* secondary, uint64_t primary);
        int32_t db_idx_double_find_secondary(uint64_t code, uint64_t scope, uint64_t table, const float64_t* secondary, uint64_t* primary);
        int32_t db_idx_double_lowerbound(uint64_t code, uint64_t scope, uint64_t table, float64_t* secondary, uint64_t* primary);
        int32_t db_idx_double_upperbound(uint64_t code, uint64_t scope, uint64_t table, float64_t* secondary, uint64_t* primary);
        int32_t db_idx_double_end(uint64_t code, uint64_t scope, uint64_t table);

        int32_t db_idx_long_double_store(uint64_t scope, uint64_t table, uint64_t payer, uint64_t id, const float128_t* secondary);
        void db_idx_long_double_update(int32_t iterator, uint64_t payer, const float128_t* secondary);
        void db_idx_long_double_remove(int32_t iterator);
        int32_t db_idx_long_double_next(int32_t iterator, uint64_t* primary);
        int32_t db_idx_long_double_previous(int32_t iterator, uint64_t* primary);
        int32_t db_idx_long_double_find_primary(uint64_t code, uint64_t scope, uint64_t table, float128_t* secondary, uint64_t primary);
        int32_t db_idx_long_double_find_secondary(uint64_t code, uint64_t scope, uint64_t table, const float128_t* secondary, uint64_t* primary);
        int32_t db_idx_long_double_lowerbound(uint64_t code, uint64_t scope, uint64_t table, float128_t* secondary, uint64_t* primary);
        int32_t db_idx_long_double_upperbound(uint64_t code, uint64_t scope, uint64_t table, float128_t* secondary, uint64_t* primary);
        int32_t db_idx_long_double_end(uint64_t code, uint64_t scope, uint64_t table);


        int32_t check_transaction_authorization(const char* trx_data,     size_t trx_size,
                                        const char* pubkeys_data, size_t pubkeys_size,
                                        const char* perms_data,   size_t perms_size
                                    );
        int32_t check_permission_authorization(uint64_t account,
                                        uint64_t permission,
                                        const char* pubkeys_data, size_t pubkeys_size,
                                        const char* perms_data,   size_t perms_size,
                                        uint64_t delay_us
                                    );
        int64_t get_permission_last_used(uint64_t account, uint64_t permission);
        int64_t get_account_creation_time(uint64_t account);



        void prints(const char* cstr);
        void prints_l(const char* cstr, size_t len);
        void printi(int64_t value);
        void printui(uint64_t value);
        void printi128(const __int128* value);
        void printui128(const __uint128* value);
        void printsf(float value);
        void printdf(double value);
        void printqf(const float128_t* value);
        void printn(uint64_t name);
        void printhex(const void* data, size_t datalen);

        void set_resource_limits(uint64_t account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight);
        void get_resource_limits(uint64_t account, int64_t* ram_bytes, int64_t* net_weight, int64_t* cpu_weight);
        int64_t set_proposed_producers(char *producer_data, size_t producer_data_size);
        int64_t set_proposed_producers_ex(uint64_t packed_producer_format, char* packed_producer_schedule, size_t datalen);
        bool is_privileged(uint64_t account);
        void set_privileged(uint64_t account, bool is_priv);
        void set_blockchain_parameters_packed(char* data, size_t datalen);
        size_t get_blockchain_parameters_packed(char* data, size_t datalen);
        void activate_feature(int64_t f);

        void eosio_abort();
        void eosio_assert(size_t test, const char* msg);
        void eosio_assert_message(size_t test, const char* msg, size_t msg_len);
        void eosio_assert_code(size_t test, uint64_t code);
        void eosio_exit(int32_t code);
        uint64_t  current_time();
        size_t  now();

        void checktime();
        void check_context_free(bool context_free);

        void send_deferred(const __uint128* sender_id, uint64_t payer, const char *serialized_transaction, size_t size, size_t replace_existing);
        int cancel_deferred(const __uint128* sender_id);

        size_t read_transaction(char *buffer, size_t size);
        size_t transaction_size();
        int tapos_block_num();
        int tapos_block_prefix();
        size_t expiration();
        int get_action(size_t type, size_t index, char* buff, size_t size);

        void assert_privileged();
        void assert_context_free();
        int get_context_free_data(size_t index, char* buff, size_t size);

        void token_create(uint64_t issuer, int64_t maximum_supply, uint64_t symbol);
        void token_issue(uint64_t to, int64_t quantity, uint64_t symbol, const char* memo, size_t size2);
        void token_transfer(uint64_t from, uint64_t to, int64_t quantity, uint64_t symbol, const char* memo, size_t size2);
        void token_open(uint64_t owner, uint64_t _symbol, uint64_t ram_payer);
        void token_retire(int64_t amount, uint64_t _symbol, const char *memo, size_t memo_size);
        void token_close(uint64_t owner, uint64_t _symbol);

        int call_contract_get_args(void *extra_args, size_t size1);
        int call_contract_set_results(const void *result, size_t size1);
        int call_contract_get_results(void *result, size_t size1);

        void call_contract(uint64_t contract, const char *args, size_t args_size);

        int evm_execute(const char *raw_trx, size_t raw_trx_size);

        void set_last_error(const char* error, size_t size);
        size_t get_last_error(char* error, size_t size);
        void clear_last_error();

        void log(int level, int line, const char *file, const char *func, const char *fmt, ...);
        void set_copy_memory_range(int start, int end);
        void get_copy_memory_range(int *start, int *end);

        int to_base58(const char *in, size_t size1, char *out, size_t size2);
        int from_base58(const char *in, size_t size1, char *out, size_t size2);

        bool is_feature_activated(const char *digest, size_t size);
        void preactivate_feature(const char *digest, size_t size);

        uint64_t get_sender();

        void __ashlti3(__int128* ret, uint64_t low, uint64_t high, size_t shift);
        void __lshrti3(__int128* ret, uint64_t low, uint64_t high, size_t shift);
#endif
    private:
        std::unique_ptr<webassembly::interface> _interface;
};
