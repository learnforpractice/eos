/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <eosio/match_engine_api_plugin/match_engine_api_plugin.hpp>
#include <iostream>

#include <fc/io/json.hpp>

namespace eosio {
   static appbase::abstract_plugin& _match_engine_api_plugin = app().register_plugin<match_engine_api_plugin>();

class match_engine_api_plugin_impl {
   public:
};

match_engine_api_plugin::match_engine_api_plugin():my(new match_engine_api_plugin_impl()){}
match_engine_api_plugin::~match_engine_api_plugin(){}


struct async_result_visitor : public fc::visitor<std::string> {
	template<typename T>
	std::string operator()(const T& v) const {
      const auto deadline = fc::time_point::now() + fc::exception::format_time_limit;
		return fc::json::to_string(v, deadline);
	}
};


#define CALL(api_name, api_handle, api_namespace, call_name, http_response_code) \
{std::string("/v1/" #api_name "/" #call_name), \
   [this, api_handle](string, string body, url_response_callback cb) mutable { \
          try { \
             if (body.empty()) body = "{}"; \
             auto result = api_handle.call_name(fc::json::from_string(body).as<api_namespace::call_name ## _params>()); \
			    const auto deadline = fc::time_point::now() + fc::exception::format_time_limit; \
             cb(http_response_code, fc::json::to_string(result, deadline)); \
          } catch (...) { \
             http_plugin::handle_exception(#api_name, #call_name, body, cb); \
          } \
       }}



#define CALL_ASYNC(api_name, api_handle, api_namespace, call_name, call_result, http_response_code) \
{std::string("/v1/" #api_name "/" #call_name), \
   [this, api_handle](string, string body, url_response_callback cb) mutable { \
      if (body.empty()) body = "{}"; \
      api_handle.call_name(fc::json::from_string(body).as<api_namespace::call_name ## _params>(),\
         [cb, body](const fc::static_variant<fc::exception_ptr, call_result>& result){\
            if (result.contains<fc::exception_ptr>()) {\
               try {\
                  result.get<fc::exception_ptr>()->dynamic_rethrow_exception();\
               } catch (...) {\
                  http_plugin::handle_exception(#api_name, #call_name, body, cb);\
               }\
            } else {\
               cb(http_response_code, result.visit(async_result_visitor()));\
            }\
         });\
   }\
}
#define CHAIN_RO_CALL(call_name, http_response_code) CALL(match_engine, ro_api, match_engine_apis::read_only, call_name, http_response_code)
#define CHAIN_RW_CALL_ASYNC(call_name, call_result, http_response_code) CALL_ASYNC(match_engine, rw_api, match_engine_apis::read_write, call_name, call_result, http_response_code)

#define INVOKE_ASYNC_R_R(api_handle, call_name, in_param0, in_param1) \
   const auto& vs = fc::json::json::from_string(body).as<fc::variants>(); \
   api_handle->call_name(vs.at(0).as<in_param0>(), vs.at(1).as<in_param1>(), result_handler);



void match_engine_api_plugin::set_program_options(options_description&, options_description& cfg) {
   cfg.add_options()
         ("option-name", bpo::value<string>()->default_value("default value"),
          "Option Description")
         ;
}

void match_engine_api_plugin::plugin_initialize(const variables_map& options) {
   if(options.count("option-name")) {
      // Handle the option
   }
}

void match_engine_api_plugin::plugin_startup() {
   // Make the magic happen
    ilog("match_engine_api_plugin start ... ");
    auto ro_api = app().get_plugin<match_engine_plugin>().get_read_only_api();
    auto rw_api = app().get_plugin<match_engine_plugin>().get_read_write_api();

    app().get_plugin<http_plugin>().add_api({
			CHAIN_RO_CALL(get_orders, 200),
       CHAIN_RW_CALL_ASYNC(trade, match_engine_apis::read_write::trade_log, 202)
    });

}

void match_engine_api_plugin::plugin_shutdown() {
   // OK, that's enough magic
}

}
