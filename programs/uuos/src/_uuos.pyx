# cython: c_string_type=str, c_string_encoding=ascii

from cython.operator cimport dereference as deref, preincrement as inc
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.map cimport map
from libcpp cimport bool

cdef extern from * :
    ctypedef long long int64_t
    ctypedef unsigned long long uint64_t
    ctypedef unsigned int uint32_t

cdef extern from "Python.h":
    object PyBytes_FromStringAndSize(const char* str, int size)

cdef extern from "uuos.hpp":
    void register_on_accepted_block_cb_()

cdef extern from "native_object.hpp":
    
    void pack_native_object_(int _type, string& msg, string& packed_message)
    void unpack_native_object_(int _type, string& packed_message, string& msg)

    void*   chain_new_(string& config, string& protocol_features_dir, string& snapshot_dir)
    void    chain_free_(void *ptr)
    int    chain_api_get_info_(void *chain_ptr, string& info)
    int    chain_api_get_activated_protocol_features_(void *ptr, string& params, string& result)
    int    chain_api_get_block_(void *ptr, string& params, string& result)
    int    chain_api_get_block_header_state_(void *ptr, string& params, string& result)
    int    chain_api_get_account_(void *chain_ptr, string& params, string& result)
    int    chain_api_get_abi_(void *ptr, string& params, string& results)
    int    chain_api_get_raw_code_and_abi_(void *ptr, string& params, string& results)
    int    chain_api_get_raw_abi_(void *ptr, string& params, string& results)
    int    chain_api_get_table_by_scope_(void *ptr, string& params, string& result)
    int    chain_api_get_currency_balance_(void *ptr, string& params, string& result)
    int    chain_api_get_currency_stats_(void *ptr, string& params, string& result)
    int    chain_api_get_producers_(void *ptr, string& params, string& result)
    int    chain_api_get_producer_schedule_(void *ptr, string& params, string& result)

    int    chain_api_get_scheduled_transactions_(void *ptr, string& params, string& result)
    int    chain_api_abi_json_to_bin_(void *ptr, string& params, string& result)
    int    chain_api_abi_bin_to_json_(void *ptr, string& params, string& result)
    int    chain_api_get_required_keys_(void *ptr, string& params, string& result)
    int    chain_api_get_transaction_id_(void *ptr, string& params, string& result)

    int    chain_api_get_code_(void *ptr, string& params, string& results)
    int    chain_api_get_code_hash_(void *ptr, string& account, string& code_hash)
    int    chain_api_get_table_rows_(void *ptr, string& params, string& results)

    int    chain_api_recover_reversible_blocks_(string& old_reversible_blocks_dir, string& new_reversible_blocks_dir, uint32_t reversible_cache_size, uint32_t truncate_at_block)
    int    chain_api_repair_log_(string& blocks_dir, uint32_t truncate_at_block, string& backup_blocks_dir)

    uint32_t    chain_fork_db_pending_head_block_num_(void *ptr)
    uint32_t    chain_last_irreversible_block_num_(void *ptr)
    void        chain_get_block_id_for_num_(void *ptr, uint32_t num, string& block_id)
    void        chain_id_(void *ptr, string& chain_id)
    void        chain_fetch_block_by_number_(void *ptr, uint32_t block_num, string& raw_block)
    int         chain_is_building_block_(void *ptr);
    int         chain_abort_block_(void *ptr)

    void*       producer_new_(void *chain_ptr, string& config);
    void        producer_free_(void *ptr);
    void        producer_on_incoming_block_(void *ptr, string& packed_signed_block, uint32_t& num, string& id)
    int         producer_start_block_(void *ptr)
    uint64_t    producer_calc_pending_block_time_(void *ptr)
    uint64_t    producer_calc_pending_block_deadline_time_(void *ptr)
    bool        producer_maybe_produce_block_(void *ptr)
    int         producer_get_pending_block_mode_(void *ptr)
    int         producer_process_incomming_transaction_(void *ptr, string& packed_trx, string& raw_packed_trx, string& out)
    int         producer_process_raw_transaction_(void *ptr, string& raw_packed_trx, string& out)
    int         producer_create_snapshot_(void *ptr, string& out)
    int         producer_is_producer_key_(void *ptr, string& _public_key)
    int         producer_schedule_protocol_feature_activations_(void *ptr, string& _features, string& err);

    void        producer_pause_(void *ptr)
    void        producer_resume_(void *ptr)
    bool        producer_paused_(void *ptr)


    void        uuos_recover_key_(string& _digest, string& _sig, string& _pub)
    uint64_t    uuos_current_time_nano_()
    void        uuos_sign_digest_(string& _priv_key, string& _digest, string& out)
    void        uuos_set_version()
    void        uuos_set_log_level_(string& logger_name, int level)

cpdef void hello(str strArg):
    "Prints back 'Hello <param>', for example example: hello.hello('you')"
    print("Hello, {}!)".format(strArg))

cpdef long elevation():
    "Returns elevation of Nevado Sajama."
    return 21463L


def pack_native_object(int _type, string& msg):
    cdef string packed_message
    pack_native_object_(_type, msg, packed_message)
    return <bytes>packed_message

def unpack_native_object(int _type, string& packed_message):
    cdef string msg
    unpack_native_object_(_type, packed_message, msg)
    return <bytes>msg

def chain_new(string& config, string& protocol_features_dir, string& snapshot_dir):
    return <unsigned long long>chain_new_(config, protocol_features_dir, snapshot_dir)

def chain_free(unsigned long long  ptr):
    chain_free_(<void *>ptr);

def chain_api_get_info(uint64_t chain_ptr):
    cdef string info
    err = chain_api_get_info_(<void *>chain_ptr, info)
    return err, info

def chain_api_get_activated_protocol_features(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_activated_protocol_features_(<void *>chain_ptr, params, result)
    return err, result

def chain_api_get_block(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_block_(<void *>chain_ptr, params, result)
    return err, result

def chain_api_get_block_header_state(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_block_header_state_(<void *>chain_ptr, params, result)
    return err, result

def chain_api_get_account(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_account_(<void *>chain_ptr, params, result)
    return err, result

def chain_api_get_code(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_code_(<void *>chain_ptr, params, result)
    r = PyBytes_FromStringAndSize(result.c_str(), result.size())
#    print(r)
    return err, r.decode('utf8')

def chain_api_get_code_hash(uint64_t chain_ptr, string& params):
    cdef string code_hash
    err = chain_api_get_code_hash_(<void *>chain_ptr, params, code_hash)
    return err, code_hash

def chain_api_get_abi(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_abi_(<void *>chain_ptr, params, result)
    r = PyBytes_FromStringAndSize(result.c_str(), result.size())
    return err, r.decode('utf8')

def chain_api_get_raw_code_and_abi(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_raw_code_and_abi_(<void *>chain_ptr, params, result)
    return err, result

def chain_api_get_raw_abi(uint64_t chain_ptr, string& params):
    cdef string result
    err = chain_api_get_raw_abi_(<void *>chain_ptr, params, result)
    r = PyBytes_FromStringAndSize(result.c_str(), result.size())
    return err, r.decode('utf8')

def chain_api_get_table_rows(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_table_rows_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_table_by_scope(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_table_by_scope_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_currency_balance(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_currency_balance_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_currency_stats(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_currency_stats_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_producers(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_producers_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_producer_schedule(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_producer_schedule_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_scheduled_transactions(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_scheduled_transactions_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_abi_json_to_bin(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_abi_json_to_bin_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_abi_bin_to_json(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_abi_bin_to_json_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_required_keys(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_required_keys_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_get_transaction_id(uint64_t chain_ptr, string& params):
    cdef string results
    err = chain_api_get_transaction_id_(<void *>chain_ptr, params, results)
    return err, results

def chain_api_recover_reversible_blocks(string& old_reversible_blocks_dir, string& new_reversible_blocks_dir, uint32_t reversible_cache_size, uint32_t truncate_at_block):
    return chain_api_recover_reversible_blocks_(old_reversible_blocks_dir, new_reversible_blocks_dir, reversible_cache_size, truncate_at_block)

def chain_api_repair_log(string& blocks_dir, uint32_t truncate_at_block):
    cdef string backup_blocks_dir
    chain_api_repair_log_(blocks_dir, truncate_at_block, backup_blocks_dir)
    return backup_blocks_dir


def chain_fork_db_pending_head_block_num(uint64_t ptr):
    return chain_fork_db_pending_head_block_num_(<void *>ptr)

def chain_last_irreversible_block_num(uint64_t ptr):
    return chain_last_irreversible_block_num_(<void *>ptr)

def chain_get_block_id_for_num(uint64_t ptr, uint32_t num):
    cdef string block_id
    chain_get_block_id_for_num_(<void *>ptr, num, block_id)
    return block_id

def chain_id(uint64_t ptr):
    cdef string chain_id
    chain_id_(<void *>ptr, chain_id)
    return chain_id

def chain_fetch_block_by_number(uint64_t ptr, uint32_t block_num ):
    cdef string raw_block
    chain_fetch_block_by_number_(<void *>ptr, block_num, raw_block)
    return <bytes>raw_block

def chain_is_building_block(uint64_t ptr):
    return chain_is_building_block_(<void *>ptr);

def chain_abort_block(uint64_t ptr):
    return chain_abort_block_(<void *>ptr)

def producer_new(uint64_t chain_ptr, string& config):
    return <uint64_t>producer_new_(<void *>chain_ptr, config)

def producer_free(uint64_t ptr):
    producer_free_(<void *>ptr)

def producer_on_incoming_block(uint64_t ptr, string& packed_signed_block):
    cdef uint32_t block_num = 0
    cdef string block_id
    producer_on_incoming_block_(<void *>ptr, packed_signed_block, block_num, block_id)
    return (block_num, block_id)

def producer_start_block(uint64_t ptr):
    return producer_start_block_(<void *>ptr)

def producer_pause(uint64_t ptr):
    producer_pause_(<void *>ptr)

def producer_resume(uint64_t ptr):
    producer_resume_(<void *>ptr)

def producer_paused(uint64_t ptr):
    return producer_paused_(<void *>ptr)

def producer_calc_pending_block_time(uint64_t ptr):
    return producer_calc_pending_block_time_(<void *>ptr)

def producer_calc_pending_block_deadline_time(uint64_t ptr):
    return producer_calc_pending_block_deadline_time_(<void *>ptr)

def producer_maybe_produce_block(uint64_t ptr):
    return producer_maybe_produce_block_(<void *>ptr);

def producer_get_pending_block_mode(uint64_t ptr):
    return producer_get_pending_block_mode_(<void *>ptr)

def producer_process_incomming_transaction(uint64_t ptr, string& packed_trx):
    cdef string out
    cdef string raw_packed_trx
    err = producer_process_incomming_transaction_(<void *>ptr, packed_trx, raw_packed_trx, out)
    return err, out, PyBytes_FromStringAndSize(raw_packed_trx.c_str(), raw_packed_trx.size())

def producer_process_raw_transaction(uint64_t ptr, string& raw_packed_trx):
    cdef string out
    err = producer_process_raw_transaction_(<void *>ptr, raw_packed_trx, out)
    return err, out

def producer_create_snapshot(uint64_t ptr):
    cdef string out
    ret = producer_create_snapshot_(<void *>ptr, out)
    return ret, out

def producer_is_producer_key(uint64_t ptr, string& public_key):
    return producer_is_producer_key_(<void *>ptr, public_key)

def producer_schedule_protocol_feature_activations(uint64_t ptr, string& features):
    cdef string err
    ret = producer_schedule_protocol_feature_activations_(<void *>ptr, features, err)
    return ret, err

g_accepted_block_cb = None
cdef extern int on_accepted_block(string& packed_block, uint32_t block_num, string& block_id):
    global g_accepted_block_cb
    if g_accepted_block_cb:
        block = PyBytes_FromStringAndSize(packed_block.c_str(), packed_block.size())
        id = PyBytes_FromStringAndSize(block_id.c_str(), block_id.size())
        g_accepted_block_cb(block, block_num, id)
    return 1

def set_accepted_block_callback(cb):
    global g_accepted_block_cb
    g_accepted_block_cb = cb

register_on_accepted_block_cb_()


g_config = None
def uuos_set_config(config):
    global g_config
    g_config = config

cdef extern string uuos_config_get_str(option):
    global g_config
    cdef string ret
    try:
        value = getattr(g_config, option)
        value = str(value)
        ret = value
    except Exception as e:
        pass
    return ret

cdef extern int uuos_config_get_int(option):
    global g_config
    try:
        value = getattr(g_config, option)
        value = int(value)
        return value
    except Exception as e:
        print(e)
    return 0

def uuos_recover_key(string& digest, string& sign):
    cdef string pub
    uuos_recover_key_(digest, sign, pub)
    return pub

def uuos_current_time_nano():
    return uuos_current_time_nano_()

def uuos_sign_digest(string& _priv_key, string& _digest):
    cdef string out
    uuos_sign_digest_(_priv_key, _digest, out)
    return out

def uuos_set_log_level(string& logger_name, int level):
    uuos_set_log_level_(logger_name, level)

uuos_set_version()
