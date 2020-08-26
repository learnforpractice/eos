cmake_minimum_required( VERSION 3.5 )

set(EOSIO_VERSION_MIN "2.0")
set(EOSIO_VERSION_SOFT_MAX "2.0")
#set(EOSIO_VERSION_HARD_MAX "")

# find_package(eosio)

### Check the version of eosio
# set(VERSION_MATCH_ERROR_MSG "")
# EOSIO_CHECK_VERSION(VERSION_OUTPUT "${EOSIO_VERSION}"
#                                    "${EOSIO_VERSION_MIN}"
#                                    "${EOSIO_VERSION_SOFT_MAX}"
#                                    "${EOSIO_VERSION_HARD_MAX}"
#                                    VERSION_MATCH_ERROR_MSG)
# if(VERSION_OUTPUT STREQUAL "MATCH")
#    message(STATUS "Using eosio version ${EOSIO_VERSION}")
# elseif(VERSION_OUTPUT STREQUAL "WARN")
#    message(WARNING "Using eosio version ${EOSIO_VERSION} even though it exceeds the maximum supported version of ${EOSIO_VERSION_SOFT_MAX}; continuing with configuration, however build may fail.\nIt is recommended to use eosio version ${EOSIO_VERSION_SOFT_MAX}.x")
# else() # INVALID OR MISMATCH
#    message(FATAL_ERROR "Found eosio version ${EOSIO_VERSION} but it does not satisfy version requirements: ${VERSION_MATCH_ERROR_MSG}\nPlease use eosio version ${EOSIO_VERSION_SOFT_MAX}.x")
# endif(VERSION_OUTPUT STREQUAL "MATCH")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/contracts.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/contracts.hpp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
### UNIT TESTING ###
include(CTest) # eliminates DartConfiguration.tcl errors at test runtime
enable_testing()
# build unit test executable
file(GLOB UNIT_TESTS "*.cpp" "*.hpp") # find all unit test suites
add_executable(contracts_test ${UNIT_TESTS}) # build unit tests as one executable
target_link_libraries( contracts_test eosio_testing chain_api_impl fc )

target_include_directories( contracts_test PUBLIC
                            ${CMAKE_SOURCE_DIR}/libraries/testing/include
                            ${CMAKE_SOURCE_DIR}/libraries/chain/include
                            ${CMAKE_BINARY_DIR}/libraries/chain/include
                            ${CMAKE_SOURCE_DIR}/libraries/chainbase/include
                            ${CMAKE_SOURCE_DIR}/libraries/softfloat/source/include
                            ${CMAKE_SOURCE_DIR}/libraries/wasm-jit/Include
                            ${CMAKE_SOURCE_DIR}/libraries/chain/db_api
                            ${CMAKE_SOURCE_DIR}/test-contracts
                            ${CMAKE_BINARY_DIR}/contracts
                          )

# mark test suites for execution
foreach(TEST_SUITE ${UNIT_TESTS}) # create an independent target for each test suite
  execute_process(COMMAND bash -c "grep -E 'BOOST_AUTO_TEST_SUITE\\s*[(]' ${TEST_SUITE} | grep -vE '//.*BOOST_AUTO_TEST_SUITE\\s*[(]' | cut -d ')' -f 1 | cut -d '(' -f 2" OUTPUT_VARIABLE SUITE_NAME OUTPUT_STRIP_TRAILING_WHITESPACE) # get the test suite name from the *.cpp file
  if (NOT "" STREQUAL "${SUITE_NAME}") # ignore empty lines
    execute_process(COMMAND bash -c "echo ${SUITE_NAME} | sed -e 's/s$//' | sed -e 's/_test$//'" OUTPUT_VARIABLE TRIMMED_SUITE_NAME OUTPUT_STRIP_TRAILING_WHITESPACE) # trim "_test" or "_tests" from the end of ${SUITE_NAME}
    # to run contracts_test with all log from blockchain displayed, put "--verbose" after "--", i.e. "contracts_test -- --verbose"
    add_test(NAME ${TRIMMED_SUITE_NAME}_unit_test COMMAND contracts_test --run_test=${SUITE_NAME} --report_level=detailed --color_output)
  endif()
endforeach(TEST_SUITE)
