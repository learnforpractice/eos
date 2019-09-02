include(ExternalProject)

if( EOSIO_COMPILE_TEST_CONTRACTS )
   set(EOSIO_WASM_OLD_BEHAVIOR "Off")
   find_package(eosio.cdt REQUIRED)

   message( STATUS "Building contracts in directory `eos/unittests/test-contracts/`" )
   ExternalProject_Add(
      contracts_project
      SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/eosio.contracts/contracts
      BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/eosio.contracts/contracts
      CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake -DEOSIO_COMPILE_TEST_CONTRACTS=${EOSIO_COMPILE_TEST_CONTRACTS} -DCMAKE_BUILD_TYPE=Debug
      UPDATE_COMMAND ""
      PATCH_COMMAND ""
      TEST_COMMAND ""
      INSTALL_COMMAND ""
      BUILD_ALWAYS 1
   )
else()
  message( STATUS "Not building contracts in directory `eos/unittests/test-contracts/`" )
#  add_subdirectory(test-contracts)
endif()
