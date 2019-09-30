/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#include <cstdlib>

#include <iostream>

#include <eosio/chain/exceptions.hpp>

#include <fc/log/logger.hpp>

#include <boost/test/included/unit_test.hpp>

#include <Python.h>

//extern uint32_t EOS_TESTING_GENESIS_TIMESTAMP;

void translate_fc_exception(const fc::exception &e) {
   std::cerr << "\033[33m" <<  e.to_detail_string() << "\033[0m" << std::endl;
   BOOST_TEST_FAIL("Caught Unexpected Exception");
}

extern "C"
{
   void chain_api_init();
   void vm_api_ro_init();
   void vm_api_init();
   int Py_FrozenMain(int argc, char **argv);
   int run_python(int argc, char **argv);

   int Py_InitFrozenMain(int argc, char **argv);
}


boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[]) {
   // Turn off blockchain logging if no --verbose parameter is not added
   // To have verbose enabled, call "tests/chain_test -- --verbose"
//   run_python(argc, argv);
//   return nullptr;

   vm_api_init();
   vm_api_ro_init();
   chain_api_init();

   Py_InitFrozenMain(argc, argv);
   PyRun_SimpleString("print('hello from unit test')");
//   Py_FrozenMain(argc, argv);

   bool is_verbose = false;
   std::string verbose_arg = "--verbose";
   for (int i = 0; i < argc; i++) {
      if (verbose_arg == argv[i]) {
         is_verbose = true;
         break;
      }
   }
   if(is_verbose) {
      fc::logger::get(DEFAULT_LOGGER).set_log_level(fc::log_level::debug);
   } else {
      fc::logger::get(DEFAULT_LOGGER).set_log_level(fc::log_level::off);
   }

   // Register fc::exception translator
   boost::unit_test::unit_test_monitor.register_exception_translator<fc::exception>(&translate_fc_exception);

   std::srand(time(NULL));
   std::cout << "Random number generator seeded to " << time(NULL) << std::endl;
   /*
   const char* genesis_timestamp_str = getenv("EOS_TESTING_GENESIS_TIMESTAMP");
   if( genesis_timestamp_str != nullptr )
   {
      EOS_TESTING_GENESIS_TIMESTAMP = std::stoul( genesis_timestamp_str );
   }
   std::cout << "EOS_TESTING_GENESIS_TIMESTAMP is " << EOS_TESTING_GENESIS_TIMESTAMP << std::endl;
   */
   return nullptr;
}
