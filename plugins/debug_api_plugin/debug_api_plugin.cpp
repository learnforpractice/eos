/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#include <fc/variant.hpp>
#include <fc/io/json.hpp>
#include <eosio/debug_api_plugin/debug_api_plugin.hpp>

namespace eosio {

static appbase::abstract_plugin& _debug_api_plugin = app().register_plugin<debug_api_plugin>();

using namespace eosio;

#define CALL(api_name, api_handle, call_name, INVOKE, http_response_code) \
{std::string("/v1/" #api_name "/" #call_name), \
   [api_handle](string, string body, url_response_callback cb) mutable { \
          try { \
             if (body.empty()) body = "{}"; \
             INVOKE \
             cb(http_response_code, fc::variant(result)); \
          } catch (...) { \
             http_plugin::handle_exception(#api_name, #call_name, body, cb); \
          } \
       }}


#define CALL_V_R(api_name, api_handle, call_name, INVOKE, http_response_code) \
{std::string("/v1/" #api_name "/" #call_name), \
   [api_handle](string, string body, url_response_callback cb) mutable { \
          try { \
             if (body.empty()) body = "{}"; \
             INVOKE \
             cb(http_response_code, fc::variant()); \
          } catch (...) { \
             http_plugin::handle_exception(#api_name, #call_name, body, cb); \
          } \
       }}

#define CALL_R_R(api_name, api_handle, call_name, INVOKE, http_response_code) \
{std::string("/v1/" #api_name "/" #call_name), \
   [api_handle](string, string body, url_response_callback cb) mutable { \
          try { \
             if (body.empty()) body = "{}"; \
             INVOKE \
             cb(http_response_code, fc::variant(result)); \
          } catch (...) { \
             http_plugin::handle_exception(#api_name, #call_name, body, cb); \
          } \
       }}

#define INVOKE_R_V(api_handle, call_name) \
     auto result = api_handle->call_name();

#define INVOKE_V_R(api_handle, call_name, type) \
     api_handle->call_name(fc::json::from_string(body).as<type>());

#define INVOKE_R_R(api_handle, call_name, type) \
   auto params = fc::json::from_string(body).as<type>(); \
   auto result = api_handle->call_name(params);

void debug_api_plugin::plugin_startup() {
   app().get_plugin<http_plugin>().add_api({
       CALL(debug, this, is_debug_enabled, INVOKE_R_V(this, is_debug_enabled), 200),
       CALL_R_R(debug, this, enable_debug, INVOKE_R_R(this, enable_debug, bool), 200),
       CALL_R_R(debug, this, add_debug_contract, INVOKE_R_R(this, add_debug_contract, eosio::debug_contract_params), 200),
       CALL_R_R(debug, this, clear_debug_contract, INVOKE_R_R(this, clear_debug_contract, string), 200),
   });
}

bool debug_api_plugin::enable_debug(bool enable) {
   return get_chain_api()->enable_debug(enable);
}

bool debug_api_plugin::is_debug_enabled() {
   return get_chain_api()->is_debug_enabled();
}

bool debug_api_plugin::add_debug_contract(debug_contract_params& params) {
   return get_chain_api()->add_debug_contract(params.name, params.path);
}

bool debug_api_plugin::clear_debug_contract(string& contract_name) {
   return get_chain_api()->clear_debug_contract(contract_name);
}

#undef INVOKE_R_V
#undef INVOKE_V_R
#undef INVOKE_R_R
#undef CALL
#undef CALL_R_R

}
