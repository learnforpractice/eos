#pragma once

#define CATCH_AND_LOG_EXCEPTION(proxy)\
   catch ( const fc::exception& e ) {\
      string ex = fc::json::to_string(*e.dynamic_copy_exception(), fc::time_point::maximum()); \
      elog("${s}", ("s", ex)); \
      proxy->set_last_error(ex);\
   } catch ( const std::exception& e ) {\
      fc::exception fce( \
         FC_LOG_MESSAGE( warn, "rethrow ${what}: ", ("what",e.what())),\
         fc::std_exception_code,\
         BOOST_CORE_TYPEID(e).name(),\
         e.what() ) ; \
        string ex = fc::json::to_string(*fce.dynamic_copy_exception(), fc::time_point::maximum()); \
        elog("${s}", ("s", ex)); \
        proxy->set_last_error(ex);\
   } catch( ... ) {\
      fc::unhandled_exception e(\
         FC_LOG_MESSAGE(warn, "rethrow"),\
         std::current_exception());\
        string ex = fc::json::to_string(*e.dynamic_copy_exception(), fc::time_point::maximum()); \
        elog("${s}", ("s", ex)); \
        proxy->set_last_error(ex); \
   }

#define CATCH_AND_LOG_EXCEPTION_AND_RETURN_FALSE(proxy) \
   catch ( const fc::exception& e ) {\
      string ex = fc::json::to_string(*e.dynamic_copy_exception(), fc::time_point::maximum()); \
      elog("${s}", ("s", ex)); \
      proxy->set_last_error(ex);\
      return false; \
   } catch ( const std::exception& e ) {\
      fc::exception fce( \
         FC_LOG_MESSAGE( warn, "rethrow ${what}: ", ("what",e.what())),\
         fc::std_exception_code,\
         BOOST_CORE_TYPEID(e).name(),\
         e.what() ) ;\
        string ex = fc::json::to_string(*fce.dynamic_copy_exception(), fc::time_point::maximum()); \
        elog("${s}", ("s", ex)); \
        proxy->set_last_error(ex);\
        return false; \
   } catch( ... ) {\
      fc::unhandled_exception e(\
         FC_LOG_MESSAGE(warn, "rethrow"),\
         std::current_exception());\
        string ex = fc::json::to_string(*e.dynamic_copy_exception(), fc::time_point::maximum()); \
        elog("${s}", ("s", ex)); \
        proxy->set_last_error(ex);\
        return false; \
   }
