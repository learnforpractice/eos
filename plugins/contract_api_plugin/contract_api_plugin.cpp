/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#include <fc/variant.hpp>
#include <fc/io/json.hpp>
#include <eosio/contract_api_plugin/contract_api_plugin.hpp>

#include <vm_manager.hpp>

namespace eosio {

static appbase::abstract_plugin& _contract_api_plugin = app().register_plugin<contract_api_plugin>();

using namespace eosio;
using namespace fc;

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

#define INVOKE_R_R(api_handle, call_name, type) \
   auto params = fc::json::from_string(body).as<type>(); \
   auto result = api_handle->call_name(params);

void contract_api_plugin::plugin_startup() {
   app().get_plugin<http_plugin>().add_api({
       CALL_R_R(contract, this, call_contract, INVOKE_R_R(this, call_contract, eosio::call_contract_params), 200),
   });
}

call_contract_results contract_api_plugin::call_contract( const call_contract_params& params )const {
   string s;
   fc::variant v;
   try {
//      s = db_api::get().exec_action(params.code.value, params.action.value, params.binargs);
      s = vm_manager::get().call_contract_off_chain(params.code.value, params.action.value, params.binargs);
      v = fc::mutable_variant_object("output", s);
      return {v};
   } catch( const boost::interprocess::bad_alloc& ) {
      throw;
   } catch( fc::exception& e ) {
      v = mutable_variant_object("name", e.name())("code", e.code())("what", e.to_string());
   } catch( const std::exception& e ) {
      v = mutable_variant_object("name", BOOST_CORE_TYPEID(e).name())("code", (int64_t)fc::std_exception_code)("what", e.what());
   } catch( ... ) {
      fc::unhandled_exception e(FC_LOG_MESSAGE( warn, "unknow error"), std::current_exception() );
      v = mutable_variant_object("name", BOOST_CORE_TYPEID(e).name())("code", (int64_t)fc::std_exception_code)("what", e.what());
   }
   return {fc::mutable_variant_object("error", v)};
}

#undef INVOKE_R_R
#undef CALL_R_R

}

void uuos_call_contract_off_chain_(string& _params, string& result) {
   string s;
   fc::variant v;
   try {
      auto params = fc::json::from_string(_params).as<eosio::call_contract_params>();
      s = vm_manager::get().call_contract_off_chain(params.code.value, params.action.value, params.binargs);
      v = fc::mutable_variant_object("output", s);
      result = fc::json::to_string(v, fc::time_point::maximum());
      return;
   } catch( const boost::interprocess::bad_alloc& ) {
      throw;
   } catch( fc::exception& e ) {
      v = fc::mutable_variant_object("name", e.name())("code", e.code())("what", e.to_string());
   } catch( const std::exception& e ) {
      v = fc::mutable_variant_object("name", BOOST_CORE_TYPEID(e).name())("code", (int64_t)fc::std_exception_code)("what", e.what());
   } catch( ... ) {
      fc::unhandled_exception e(FC_LOG_MESSAGE( warn, "unknow error"), std::current_exception() );
      v = fc::mutable_variant_object("name", BOOST_CORE_TYPEID(e).name())("code", (int64_t)fc::std_exception_code)("what", e.what());
   }
   result = fc::json::to_string(fc::mutable_variant_object("error", v), fc::time_point::maximum());
}
