include(ExternalProject)

if( EOSIO_COMPILE_TEST_CONTRACTS )
   set(EOSIO_WASM_OLD_BEHAVIOR "Off")
   find_package(eosio.cdt REQUIRED)

   message( STATUS "Building contracts in directory `eos/unittests/test-contracts/`" )
   ExternalProject_Add(
      contracts_project
      SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/eosio.contracts/contracts
      BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/eosio.contracts/contracts
      CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake -DEOSIO_COMPILE_TEST_CONTRACTS=${EOSIO_COMPILE_TEST_CONTRACTS}
      UPDATE_COMMAND ""
      PATCH_COMMAND ""
      TEST_COMMAND ""
      INSTALL_COMMAND ""
      BUILD_ALWAYS 1
   )

   # message( STATUS "Building contracts in directory `eos/unittests/test-contracts/`" )
   # ExternalProject_Add(
   #    contracts_tests_project
   #    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/eosio.contracts/tests
   #    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/eosio.contracts/tests
   #    CMAKE_ARGS -Deosio_DIR=${CMAKE_BINARY_DIR}/modules -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake -DEOSIO_COMPILE_TEST_CONTRACTS=${EOSIO_COMPILE_TEST_CONTRACTS}
   #    UPDATE_COMMAND ""
   #    PATCH_COMMAND ""
   #    TEST_COMMAND ""
   #    INSTALL_COMMAND ""
   #    BUILD_ALWAYS 1
   # )

   # ExternalProject_Add(
   #    contracts_debug_project
   #    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/eosio.contracts/contracts
   #    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/eosio.contracts/contracts-debug
   #    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake -DEOSIO_COMPILE_TEST_CONTRACTS=${EOSIO_COMPILE_TEST_CONTRACTS} -DCMAKE_BUILD_TYPE=Debug
   #    UPDATE_COMMAND ""
   #    PATCH_COMMAND ""
   #    TEST_COMMAND ""
   #    INSTALL_COMMAND ""
   #    BUILD_ALWAYS 1
   # )
else()
  message( STATUS "Not building contracts in directory `eos/unittests/test-contracts/`" )
#  add_subdirectory(test-contracts)
endif()

ExternalProject_Add(
   aleth
   SOURCE_DIR ${CMAKE_SOURCE_DIR}/externals/aleth
   BINARY_DIR ${CMAKE_BINARY_DIR}/aleth
#   CMAKE_ARGS -DBUILD_SHARED_LIBS=true #-DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake
   UPDATE_COMMAND ""
   PATCH_COMMAND ""
   TEST_COMMAND ""
   INSTALL_COMMAND ""
   BUILD_ALWAYS 1
)

#add_subdirectory(test-contracts)
add_subdirectory(boost)
add_subdirectory(Python-3.7.4)
add_subdirectory(libzmq)
add_subdirectory(cppzmq)
add_dependencies(cppzmq libzmq)
add_dependencies(aleth eth_account)
add_dependencies(aleth vm_api)
