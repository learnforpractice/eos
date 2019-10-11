#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/controller.hpp>
#include <eosio/chain/transaction_context.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <fc/io/json.hpp>

#include <dlfcn.h>

#include <chain_api.hpp>
#include <eosio/chain/db_api.hpp>

#include <eosiolib_native/vm_api.h>

using namespace fc;
using namespace eosio::chain;

extern "C" const unsigned char pythonvm_wasm[];
extern "C" int pythonvm_wasm_size;
extern "C" char pythonvm_wasm_hash[];

static controller *s_ctrl = nullptr;
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
   if (!s_ctrl) {
      throw std::runtime_error("controller not specified!");
   }
   return *s_ctrl;
}

static inline controller::config& cfg() {
   return s_cfg;
}

bool is_account(uint64_t account) {
   return nullptr != ctrl().db().find<account_object,by_name>( name(account) );
}

static void n2str(uint64_t n, string& str_name) {
   try {
      str_name = name(n).to_string();
   } catch (...) {

   }
}

static uint64_t str2n(string& str_name) {
   try {
      return name(str_name).value;
   } catch (...) {

   }
   return 0;
}

static bool get_code(uint64_t contract, digest_type& code_id, const char** code, size_t *size) {
   try {
      const auto& account = ctrl().db().get<account_metadata_object,by_name>(name(contract));
      bool existing_code = (account.code_hash != digest_type());
      code_id = account.code_hash;
      if( existing_code ) {
         const code_object& code_entry = ctrl().db().get<code_object, by_code_hash>(boost::make_tuple(account.code_hash, account.vm_type, account.vm_version));
         *code = code_entry.code.data();
         *size = code_entry.code.size();
         return true;
      }
   } catch (...) {
   }
   return false;
}

static const char* get_code_ex( uint64_t receiver, size_t* size ) {
   if (!is_account(receiver)) {
      *size = 0;
      return nullptr;
   }
   try {
      const auto& account = ctrl().db().get<account_metadata_object,by_name>(name(receiver));
      bool existing_code = (account.code_hash != digest_type());
      if( existing_code ) {
         const code_object& code_entry = ctrl().db().get<code_object, by_code_hash>(boost::make_tuple(account.code_hash, account.vm_type, account.vm_version));
         *size = code_entry.code.size();
         return code_entry.code.data();
      }
   } catch (...) {
   }
   return nullptr;
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
   get_vm_api()->eosio_assert(size == 32, "bad code id size!");
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
   if (!s_ctrl) {
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
   receiver = ctx().get_receiver();
   account = ctx().get_action().account;
   action = ctx().get_action().name;
}

static bool is_producing_block() {
   if (!s_ctrl) {
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
   try {
      auto trace = ctrl().call_contract(contract, action, binargs);
      return fc::json::to_string(trace);
   }FC_CAPTURE_AND_LOG((contract));
   return "";
}

//chain_exceptions.cpp
void chain_throw_exception(int type, const char* fmt, ...);

extern "C" void chain_api_init() {
    static bool init = false;
    if (init) {
        return;
    }
    init = true;

    s_api = chain_api_cpp {
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
    };
    register_chain_api(&s_api);
}

void chain_api_set_controller(controller *_ctrl, controller::config _cfg) {
   s_ctrl = _ctrl;
   s_cfg = _cfg;
}

controller& chain_api_get_controller() {
   return *s_ctrl;
}
