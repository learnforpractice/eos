#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/controller.hpp>
#include <eosio/chain/transaction_context.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <fc/crypto/public_key.hpp>

#include <fc/io/json.hpp>

#include <dlfcn.h>

#include <chain_api.hpp>
#include <eosio/chain/db_api.hpp>

#include <vm_api/vm_api.h>
#include <stacktrace.h>

using namespace fc;
using namespace eosio::chain;

extern "C" const unsigned char pythonvm_wasm[];
extern "C" int pythonvm_wasm_size;
extern "C" char pythonvm_wasm_hash[];
void *chain_get_current_ptr();
static controller::config s_cfg;
static chain_api_cpp s_api = {};

//vm_api.cpp
namespace eosio {
namespace chain {

apply_context *get_apply_context();

static inline apply_context& ctx() {
   return *get_apply_context();
}

}}


static inline controller& ctrl() {
   controller* ptr = (controller*)chain_get_current_ptr();
   if (!ptr) {
      print_stacktrace();
      throw std::runtime_error("controller not specified!");
   }
   return *ptr;
}

static inline controller::config& cfg() {
   return s_cfg;
}

bool is_account(uint64_t account) {
   void* ptr = s_api.chain_get_db_interface(s_api.chain_ptr);
   return s_api.db_interface_is_account(ptr, account);
}

static void n2str(uint64_t n, string& str_name) {
   try {
      str_name = name(n).to_string();
   } catch (...) {

   }
}

static uint64_t str2n(string& str_name) {
   try {
      return name(str_name).to_uint64_t();
   } catch (...) {

   }
   return 0;
}

static bool get_code(uint64_t contract, string& code_id, string& code) {
   try {
      const auto& account = ctrl().db().get<account_metadata_object,by_name>(name(contract));
      bool existing_code = (account.code_hash != digest_type());
      code_id = string(account.code_hash.data(), account.code_hash.data_size());
      if( existing_code ) {
         const code_object& code_entry = ctrl().db().get<code_object, by_code_hash>(boost::make_tuple(account.code_hash, account.vm_type, account.vm_version));
         code = string(code_entry.code.data(), code_entry.code.size());
         return true;
      }
   } catch (...) {
   }
   return false;
}

static const char* get_code_ex( uint64_t receiver, size_t* size ) {
   void *db_ptr = s_api.chain_get_db_interface(s_api.chain_ptr);
   return s_api.db_interface_get_code_ex(db_ptr, receiver, size);
}

static uint32_t get_code_first_block_used(uint64_t contract) {
   try {
      const auto& account = ctrl().db().get<account_metadata_object,by_name>(name(contract));
      bool existing_code = (account.code_hash != digest_type());
      if( existing_code ) {
         const code_object& code_entry = ctrl().db().get<code_object, by_code_hash>(boost::make_tuple(account.code_hash, account.vm_type, account.vm_version));
         return code_entry.first_block_used;
      }
   } catch (...) {
   }
   return 0;
}

static bool get_code_id( uint64_t receiver, uint8_t* code_id, size_t size) {
   if (!is_account(receiver)) {
      return false;
   }
   EOSIO_ASSERT(size == 32, "bad code id size!");
   try {
      const auto& account = ctrl().db().get<account_metadata_object,by_name>(name(receiver));
      memcpy(code_id, account.code_hash.data(), 32);
      return true;
   } catch (...) {
   }
   return false;
}

int get_code_type( uint64_t receiver) {
   if (!is_account(receiver)) {
      return -1;
   }
   try {
      const auto& account = ctrl().db().get<account_metadata_object,by_name>(name(receiver));
      return account.vm_type;
   } catch (...) {
   }
   return -1;
}

static bool get_code_by_code_hash(const digest_type& code_hash, const uint8_t vm_type, const uint8_t vm_version, const char** code, size_t* size, uint32_t* first_block_used) {
//   printf("++++++get_code_by_code_hash\n");
   if (0 == memcmp(pythonvm_wasm_hash, code_hash.data(), 32)) {
//      printf("++++++get_code_by_code_hash 111\n");
      *code = (const char *)pythonvm_wasm;
      *size = pythonvm_wasm_size;
      *first_block_used = 0;
   } else {
      const code_object& code_entry = ctrl().db().get<code_object, by_code_hash>(boost::make_tuple(code_hash, vm_type, vm_version));
      *code = code_entry.code.data();
      *size = code_entry.code.size();
      *first_block_used = code_entry.first_block_used;
   }
   return true;
}

static string get_state_dir() {
   return cfg().state_dir.string();
}

static bool contracts_console() {
   if (!s_api.chain_ptr) {
      return true;
   }
   return ctrl().contracts_console();
}

static void resume_billing_timer() {
   ctx().trx_context.resume_billing_timer();
}

static void pause_billing_timer() {
   ctx().trx_context.pause_billing_timer();
}

static void get_action(uint64_t& receiver, uint64_t& account, uint64_t action) {
   receiver = ctx().get_receiver().to_uint64_t();
   account = ctx().get_action().account.to_uint64_t();
   action = ctx().get_action().name.to_uint64_t();
}

static bool is_producing_block() {
   if (!s_api.chain_ptr) {
      return false;
   }
   return ctrl().is_producing_block();
}

static uint64_t get_microseconds() {
   if (sysconf(_POSIX_THREAD_CPUTIME)){
      struct timespec tv;
      int err = clock_gettime(CLOCK_THREAD_CPUTIME_ID,&tv);
      if (err == 0) {
         return tv.tv_sec * 1000000LL + tv.tv_nsec / 1000LL;
      }
   }
   printf("+++++ERROR: something went wrong!\n");
   return 0;
}

static void get_resource_limits( uint64_t account, int64_t* ram_bytes, int64_t* net_weight, int64_t* cpu_weight ) {
   ctrl().get_resource_limits_manager().get_account_limits( name(account), *ram_bytes, *net_weight, *cpu_weight);
}

typedef void (*fn_vm_register_api)(vm_api* api);

struct debug_contract
{
   void *handle;
   void *apply_entry;
   string path;
};

static bool debug_enabled = false;
static map<string, debug_contract> debug_contract_map;


static bool enable_debug(bool enable) {
   debug_enabled = enable;
   return true;
}

static bool is_debug_enabled() {
   return debug_enabled;
}

static bool add_debug_contract(string& contract_name, string& path) {
   auto itr = debug_contract_map.find(contract_name);
   if (itr != debug_contract_map.end()) {
      dlclose(itr->second.handle);
      debug_contract_map.erase(itr);
   }

   void *handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
   if (handle == NULL) {
      elog("load ${n1} failed: ${n2}", ("n1", dlerror())("n2",path));
      return false;
   }

   void *apply_entry = dlsym(handle, "native_apply");
   if (apply_entry == NULL) {
      elog("load ${n1} failed: apply", ("n1", dlerror()));
      return false;
   }

   fn_vm_register_api vm_register_api = (fn_vm_register_api)dlsym(handle, "vm_register_api");
   if (vm_register_api != nullptr) {
      vm_register_api(get_vm_api());
   } else {
      elog("vm_register_api not found in ${n}", ("n", path));
   }

   debug_contract_map[contract_name] = debug_contract{
      .handle = handle,
      .apply_entry = apply_entry,
      .path = path,
   };
   return true;
}

static bool clear_debug_contract(string& contract_name) {
   auto itr = debug_contract_map.find(contract_name);
   if (itr != debug_contract_map.end()) {
      debug_contract_map.erase(itr);
      return true;
   }
   return false;
}

static void* get_debug_contract_entry(string& contract_name) {
   auto itr = debug_contract_map.find(contract_name);
   if (itr != debug_contract_map.end()) {
      return itr->second.apply_entry;
   }
   return nullptr;   
}

static bool is_builtin_activated(uint32_t feature) {
   return ctrl().is_builtin_activated(static_cast<builtin_protocol_feature_t>(feature));
}

static string call_contract_off_chain(uint64_t contract, uint64_t action, const vector<char>& binargs) {
   // auto trace = ctrl().call_contract(contract, action, binargs);
   // if (trace->action_traces.size() > 0) {
   //    return trace->action_traces[0].console;
   // }
//      return fc::json::to_string(trace);
   return "";
}

//chain_exceptions.cpp
void chain_throw_exception(int type, const char* fmt, ...);

//chain api implemented in chain_manager;
void *chain_new_(string& config, string& _genesis, string& protocol_features_dir, string& snapshot_dir);
bool chain_startup_(void* ptr, bool initdb);
void chain_free_(void *ptr);
void chain_id_(void *ptr, string& chain_id);
void chain_start_block_(void *ptr, string& _time, uint16_t confirm_block_count, string& _new_features);
int chain_abort_block_(void *ptr);
void chain_get_preactivated_protocol_features_(void *ptr, string& result);
void chain_get_unapplied_transactions_(void *ptr, string& result);
bool chain_pack_action_args_(void *ptr, string& name, string& action, string& _args, vector<char>& result);
bool chain_unpack_action_args_(void *ptr, string& name, string& action, string& _binargs, string& result);
void chain_gen_transaction_(string& _actions, string& expiration, string& reference_block_id, string& _chain_id, bool compress, std::string& _private_keys, vector<char>& result);
bool chain_push_transaction_(void *ptr, string& _packed_trx, string& deadline, uint32_t billed_cpu_time_us, string& result);
void chain_push_scheduled_transaction_(void *ptr, string& scheduled_tx_id, string& deadline, uint32_t billed_cpu_time_us, string& result);
void chain_commit_block_(void *ptr);
void chain_finalize_block_(void *ptr, string& _priv_key);
void chain_pop_block_(void *ptr);
void chain_get_account_(void *ptr, string& account, string& result);
void chain_get_global_properties_(void *ptr, string& result);
void chain_get_dynamic_global_properties_(void *ptr, string& result);
void chain_get_actor_whitelist_(void *ptr, string& result);
void chain_get_actor_blacklist_(void *ptr, string& result);
void chain_get_contract_whitelist_(void *ptr, string& result);
void chain_get_contract_blacklist_(void *ptr, string& result);
void chain_get_action_blacklist_(void *ptr, string& result);
void chain_get_key_blacklist_(void *ptr, string& result);
void chain_set_actor_whitelist_(void *ptr, string& params);
void chain_set_actor_blacklist_(void *ptr, string& params);
void chain_set_contract_whitelist_(void *ptr, string& params);
void chain_set_action_blacklist_(void *ptr, string& params);
void chain_set_key_blacklist_(void *ptr, string& params);
uint32_t chain_head_block_num_(void *ptr);
void chain_head_block_time_(void *ptr, string& result);
void chain_head_block_id_(void *ptr, string& result);
void chain_head_block_producer_(void *ptr, string& result);
void chain_head_block_header_(void *ptr, string& result);
void chain_head_block_state_(void *ptr, string& result);
uint32_t chain_fork_db_head_block_num_(void *ptr);
void chain_fork_db_head_block_id_(void *ptr, string& result);
void chain_fork_db_head_block_time_(void *ptr, string& result);
void chain_fork_db_head_block_producer_(void *ptr, string& result);
uint32_t chain_fork_db_pending_head_block_num_(void *ptr);
void chain_fork_db_pending_head_block_id_(void *ptr, string& result);
void chain_fork_db_pending_head_block_time_(void *ptr, string& result);
void chain_fork_db_pending_head_block_producer_(void *ptr, string& result);
void chain_pending_block_time_(void *ptr, string& result);
void chain_pending_block_producer_(void *ptr, string& result);
void chain_pending_block_signing_key_(void *ptr, string& result);
void chain_pending_producer_block_id_(void *ptr, string& result);
void chain_get_pending_trx_receipts_(void *ptr, string& result);
void chain_active_producers_(void *ptr, string& result);
void chain_pending_producers_(void *ptr, string& result);
void chain_proposed_producers_(void *ptr, string& result);
uint32_t chain_last_irreversible_block_num_(void *ptr);
void chain_last_irreversible_block_id_(void *ptr, string& result);
void chain_fetch_block_by_number_(void *ptr, uint32_t block_num, string& raw_block );
void chain_fetch_block_by_id_(void *ptr, string& params, string& raw_block );
void chain_fetch_block_state_by_number_(void *ptr, uint32_t block_num, string& raw_block_state );
void chain_fetch_block_state_by_id_(void *ptr, string& params, string& raw_block_state );
void chain_get_block_id_for_num_(void *ptr, uint32_t block_num, string& result );
void chain_calculate_integrity_hash_(void *ptr, string& result );
bool chain_sender_avoids_whitelist_blacklist_enforcement_(void *ptr, string& sender );
bool chain_check_actor_list_(void *ptr, string& param, string& err);
bool chain_check_contract_list_(void *ptr, string& param, string& err);
bool chain_check_action_list_(void *ptr, string& code, string& action, string& err);
bool chain_check_key_list_(void *ptr, string& param, string& err);
bool chain_is_building_block_(void *ptr);
bool chain_is_producing_block_(void *ptr);
bool chain_is_ram_billing_in_notify_allowed_(void *ptr);
void chain_add_resource_greylist_(void *ptr, string& param);
void chain_remove_resource_greylist_(void *ptr, string& param);
bool chain_is_resource_greylisted_(void *ptr, string& param);
void chain_get_resource_greylist_(void *ptr, string& result);
void chain_get_config_(void *ptr, string& result);
bool chain_validate_expiration_(void *ptr, string& param, string& err);
bool chain_validate_tapos_(void *ptr, string& param, string& err);
bool chain_validate_db_available_size_(void *ptr, string& err);
bool chain_validate_reversible_available_size_(void *ptr, string& err);
bool chain_is_protocol_feature_activated_(void *ptr, string& param);
bool chain_is_builtin_activated_(void *ptr, int feature);
bool chain_is_known_unexpired_transaction_(void *ptr, string& param);
int64_t chain_set_proposed_producers_(void *ptr, string& param);
bool chain_light_validation_allowed_(void *ptr, bool replay_opts_disabled_by_policy);
bool chain_skip_auth_check_(void *ptr);
bool chain_skip_db_sessions_(void *ptr);

bool chain_skip_trx_checks_(void *ptr);
bool chain_contracts_console_(void *ptr);
bool chain_is_uuos_mainnet_(void *ptr);

void chain_get_chain_id_(void *ptr, string& result);
int chain_get_read_mode_(void *ptr);
int chain_get_validation_mode_(void *ptr);
void chain_set_subjective_cpu_leeway_(void *ptr, uint64_t leeway);
void chain_set_greylist_limit_(void *ptr, uint32_t limit);
uint32_t chain_get_greylist_limit_(void *ptr);
void chain_add_to_ram_correction_(void *ptr, string& account, uint64_t ram_bytes);
bool chain_all_subjective_mitigations_disabled_(void *ptr);
void chain_get_scheduled_producer_(void *ptr, string& _block_time, string& result);
void* chain_get_db_interface_(void *ptr);


void pack_native_object_(int type, string& msg, string& packed_message);
void unpack_native_object_(int type, string& packed_message, string& msg);
void uuos_set_log_level_(string& logger_name, int level);
void uuos_shutdown_();
void uuos_sign_digest_(string& _digest, string& _priv_key, string& out);

string& uuos_get_last_error_();
void uuos_set_last_error_(string& error);
void uuos_on_error_(string& _ex);

int db_interface_get_i64(void *ptr, int itr, string& buffer );
int db_interface_next_i64(void *ptr, int itr, uint64_t* primary );
int db_interface_previous_i64(void *ptr, int itr, uint64_t* primary );
int db_interface_find_i64(void *ptr, uint64_t code, uint64_t scope, uint64_t table, uint64_t id );
void db_interface_remove_i64(void *ptr,int itr);
int db_interface_lowerbound_i64(void *ptr, uint64_t code, uint64_t scope, uint64_t table, uint64_t id );
int db_interface_upperbound_i64(void *ptr, uint64_t code, uint64_t scope, uint64_t table, uint64_t id );
int db_interface_end_i64(void *ptr, uint64_t code, uint64_t scope, uint64_t table );
bool db_interface_is_account(void *ptr, uint64_t account);
const char* db_interface_get_code_ex(void *ptr, uint64_t receiver, size_t* size );

void *chain_get_current_ptr() {
   if (!s_api.chain_ptr) {
      print_stacktrace();
      EOSIO_ASSERT(false, "current chain ptr is null!");
   }
   return s_api.chain_ptr;
}

void chain_set_current_ptr(void *ptr) {
   s_api.chain_ptr = ptr;
}

static map<uint32_t, vm_callback> vm_cbs;

static void register_vm_callback_(uint8_t vmtype, uint8_t vmversion, vm_callback *cb) {
   vm_cbs[(uint16_t(vmtype)<<8) + vmversion] = *cb;
}

static vm_callback* get_vm_callback_(uint8_t vmtype, uint8_t vmversion) {
   auto itr = vm_cbs.find((uint16_t(vmtype)<<8) + vmversion);
   if (itr == vm_cbs.end()) {
      return nullptr;
   }
   return &itr->second;
}

static bool get_apply_args_(uint64_t& receiver, uint64_t& code, uint64_t& action) {
   receiver = ctx().get_receiver().to_uint64_t();
   code = ctx().get_action().account.to_uint64_t();
   action = ctx().get_action().name.to_uint64_t();
   return true;
}

void set_public_key_prefix_(const string& prefix) {
   fc::crypto::config::public_key_legacy_prefix = prefix;
}

void get_public_key_prefix_(string& prefix) {
   prefix = fc::crypto::config::public_key_legacy_prefix;
}

extern "C" void chain_api_init() {
    static bool init = false;
    if (init) {
        return;
    }
    init = true;

    s_api = chain_api_cpp {
      .chain_ptr = nullptr,

      .register_vm_callback = register_vm_callback_,
      .get_vm_callback = get_vm_callback_,
      .get_apply_args = get_apply_args_,
      .n2str = n2str,
      .str2n = str2n,
      .get_code = get_code,
      .get_code_ex = get_code_ex,
      .get_code_id = get_code_id,
      .get_code_type = get_code_type,

      .get_state_dir = get_state_dir,
      .contracts_console = contracts_console,
      .resume_billing_timer = resume_billing_timer,
      .pause_billing_timer = pause_billing_timer,
      .is_producing_block = is_producing_block,
      .throw_exception = chain_throw_exception,
      .get_microseconds = get_microseconds,
      .get_code_by_code_hash = get_code_by_code_hash,
      .get_resource_limits = get_resource_limits,

      .enable_debug = enable_debug,
      .is_debug_enabled = is_debug_enabled,
      .add_debug_contract = add_debug_contract,
      .clear_debug_contract = clear_debug_contract,
      .get_debug_contract_entry = get_debug_contract_entry,
      .is_builtin_activated = is_builtin_activated,
      .call_contract_off_chain = call_contract_off_chain,

      .chain_get_current_ptr = chain_get_current_ptr,
      .chain_set_current_ptr = chain_set_current_ptr,

      .set_public_key_prefix = set_public_key_prefix_,
      .get_public_key_prefix = get_public_key_prefix_,

      .chain_new = chain_new_,
      .chain_startup = chain_startup_,
      .chain_free = chain_free_,
      .chain_id = chain_id_,
      .chain_start_block = chain_start_block_,
      .chain_abort_block = chain_abort_block_,
      .chain_get_preactivated_protocol_features = chain_get_preactivated_protocol_features_,
      .chain_get_unapplied_transactions = chain_get_unapplied_transactions_,
      .chain_pack_action_args = chain_pack_action_args_,
      .chain_unpack_action_args = chain_unpack_action_args_,
      .chain_gen_transaction = chain_gen_transaction_,
      .chain_push_transaction = chain_push_transaction_,
      .chain_push_scheduled_transaction = chain_push_scheduled_transaction_,
      .chain_commit_block = chain_commit_block_,
      .chain_finalize_block = chain_finalize_block_,
      .chain_pop_block = chain_pop_block_,
      .chain_get_account = chain_get_account_,
      .chain_get_global_properties = chain_get_global_properties_,
      .chain_get_dynamic_global_properties = chain_get_dynamic_global_properties_,
      .chain_get_actor_whitelist = chain_get_actor_whitelist_,
      .chain_get_actor_blacklist = chain_get_actor_blacklist_,
      .chain_get_contract_whitelist = chain_get_contract_whitelist_,
      .chain_get_contract_blacklist = chain_get_contract_blacklist_,
      .chain_get_action_blacklist = chain_get_action_blacklist_,
      .chain_get_key_blacklist = chain_get_key_blacklist_,
      .chain_set_actor_whitelist = chain_set_actor_whitelist_,
      .chain_set_actor_blacklist = chain_set_actor_blacklist_,
      .chain_set_contract_whitelist = chain_set_contract_whitelist_,
      .chain_set_action_blacklist = chain_set_action_blacklist_,
      .chain_set_key_blacklist = chain_set_key_blacklist_,
      .chain_head_block_num = chain_head_block_num_,
      .chain_head_block_time = chain_head_block_time_,
      .chain_head_block_id = chain_head_block_id_,
      .chain_head_block_producer = chain_head_block_producer_,
      .chain_head_block_header = chain_head_block_header_,
      .chain_head_block_state = chain_head_block_state_,
      .chain_fork_db_head_block_num = chain_fork_db_head_block_num_,
      .chain_fork_db_head_block_id = chain_fork_db_head_block_id_,
      .chain_fork_db_head_block_time = chain_fork_db_head_block_time_,
      .chain_fork_db_head_block_producer = chain_fork_db_head_block_producer_,
      .chain_fork_db_pending_head_block_num = chain_fork_db_pending_head_block_num_,
      .chain_fork_db_pending_head_block_id = chain_fork_db_pending_head_block_id_,
      .chain_fork_db_pending_head_block_time = chain_fork_db_pending_head_block_time_,
      .chain_fork_db_pending_head_block_producer = chain_fork_db_pending_head_block_producer_,
      .chain_pending_block_time = chain_pending_block_time_,
      .chain_pending_block_producer = chain_pending_block_producer_,
      .chain_pending_block_signing_key = chain_pending_block_signing_key_,
      .chain_pending_producer_block_id = chain_pending_producer_block_id_,
      .chain_get_pending_trx_receipts = chain_get_pending_trx_receipts_,
      .chain_active_producers = chain_active_producers_,
      .chain_pending_producers = chain_pending_producers_,
      .chain_proposed_producers = chain_proposed_producers_,
      .chain_last_irreversible_block_num = chain_last_irreversible_block_num_,
      .chain_last_irreversible_block_id = chain_last_irreversible_block_id_,
      .chain_fetch_block_by_number = chain_fetch_block_by_number_,
      .chain_fetch_block_by_id = chain_fetch_block_by_id_,
      .chain_fetch_block_state_by_number = chain_fetch_block_state_by_number_,
      .chain_fetch_block_state_by_id = chain_fetch_block_state_by_id_,
      .chain_get_block_id_for_num = chain_get_block_id_for_num_,
      .chain_calculate_integrity_hash = chain_calculate_integrity_hash_,
      .chain_sender_avoids_whitelist_blacklist_enforcement = chain_sender_avoids_whitelist_blacklist_enforcement_,
      .chain_check_actor_list = chain_check_actor_list_,
      .chain_check_contract_list = chain_check_contract_list_,
      .chain_check_action_list = chain_check_action_list_,
      .chain_check_key_list = chain_check_key_list_,
      .chain_is_building_block = chain_is_building_block_,
      .chain_is_producing_block = chain_is_producing_block_,
      .chain_is_ram_billing_in_notify_allowed = chain_is_ram_billing_in_notify_allowed_,
      .chain_add_resource_greylist = chain_add_resource_greylist_,
      .chain_remove_resource_greylist = chain_remove_resource_greylist_,
      .chain_is_resource_greylisted = chain_is_resource_greylisted_,
      .chain_get_resource_greylist = chain_get_resource_greylist_,
      .chain_get_config = chain_get_config_,
      .chain_validate_expiration = chain_validate_expiration_,
      .chain_validate_tapos = chain_validate_tapos_,
      .chain_validate_db_available_size = chain_validate_db_available_size_,
      .chain_validate_reversible_available_size = chain_validate_reversible_available_size_,
      .chain_is_protocol_feature_activated = chain_is_protocol_feature_activated_,
      .chain_is_builtin_activated = chain_is_builtin_activated_,
      .chain_is_known_unexpired_transaction = chain_is_known_unexpired_transaction_,
      .chain_set_proposed_producers = chain_set_proposed_producers_,
      .chain_light_validation_allowed = chain_light_validation_allowed_,
      .chain_skip_auth_check = chain_skip_auth_check_,
      .chain_skip_db_sessions = chain_skip_db_sessions_,
      .chain_skip_trx_checks = chain_skip_trx_checks_,
      .chain_contracts_console = chain_contracts_console_,
      .chain_is_uuos_mainnet = chain_is_uuos_mainnet_,
      .chain_get_chain_id = chain_get_chain_id_,
      .chain_get_read_mode = chain_get_read_mode_,
      .chain_get_validation_mode = chain_get_validation_mode_,
      .chain_set_subjective_cpu_leeway = chain_set_subjective_cpu_leeway_,
      .chain_set_greylist_limit = chain_set_greylist_limit_,
      .chain_get_greylist_limit = chain_get_greylist_limit_,
      .chain_add_to_ram_correction = chain_add_to_ram_correction_,
      .chain_all_subjective_mitigations_disabled = chain_all_subjective_mitigations_disabled_,
      .chain_get_scheduled_producer = chain_get_scheduled_producer_,
      .chain_get_db_interface = chain_get_db_interface_,

      .pack_native_object = pack_native_object_,
      .unpack_native_object = unpack_native_object_,

      .uuos_set_log_level = uuos_set_log_level_,
      .uuos_shutdown = uuos_shutdown_,
      .uuos_sign_digest = uuos_sign_digest_,

      .uuos_get_last_error = uuos_get_last_error_,
      .uuos_set_last_error = uuos_set_last_error_,
      .uuos_on_error = uuos_on_error_,

      .db_interface_get_i64 = db_interface_get_i64,
      .db_interface_next_i64 = db_interface_next_i64,
      .db_interface_previous_i64 = db_interface_previous_i64,
      .db_interface_find_i64 = db_interface_find_i64,
      .db_interface_remove_i64 = db_interface_remove_i64,
      .db_interface_lowerbound_i64 = db_interface_lowerbound_i64,
      .db_interface_upperbound_i64 = db_interface_upperbound_i64,
      .db_interface_end_i64 = db_interface_end_i64,
      .db_interface_is_account = db_interface_is_account,
      .db_interface_get_code_ex = db_interface_get_code_ex,
    };

    register_chain_api(&s_api);
}

controller& chain_api_get_controller() {
   controller* ptr = (controller*)chain_get_current_ptr();
   if (!ptr) {
      print_stacktrace();
      throw std::runtime_error("controller not specified!");
   }
   return *ptr;
}
