include(ExternalProject)
set(EOSIOLIB_NATIVE "${CMAKE_BINARY_DIR}/contracts/vm_api/libvm_api${CMAKE_SHARED_LIBRARY_SUFFIX}")
set(API4C_LIB "${CMAKE_BINARY_DIR}/libraries/vm/vm_api4c/libvm_api4c${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(WASM2C_EXPORTS_LIB "${CMAKE_BINARY_DIR}/libraries/vm/wasm2c_exports/libwasm2c_exports${CMAKE_SHARED_LIBRARY_SUFFIX}")


set(SOFTFLOAT_LIB "${CMAKE_BINARY_DIR}/libraries/softfloat/libsoftfloat${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(CHAIN_API_LIB "${CMAKE_BINARY_DIR}/libraries/chain_api/libchain_api${CMAKE_SHARED_LIBRARY_SUFFIX}")

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
if (BUILD_EVMONE4EOSIO)

ExternalProject_Add(
   evmone4eosio
   SOURCE_DIR ${CMAKE_SOURCE_DIR}/externals/evmone4eosio
   BINARY_DIR ${CMAKE_BINARY_DIR}/externals/evmone4eosio
   CMAKE_ARGS -DWASM2C_EXPORTS_LIB=${WASM2C_EXPORTS_LIB} -DEOSIOLIB_NATIVE=${EOSIOLIB_NATIVE} -DAPI4C_LIB=${API4C_LIB} -DCHAIN_API_LIB=${CHAIN_API_LIB} -DSOFTFLOAT_LIB=${SOFTFLOAT_LIB} -DEOSIOLIB_DIR=${CMAKE_SOURCE_DIR}/contracts #-DBUILD_SHARED_LIBS=true #-DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake
   UPDATE_COMMAND ""
   PATCH_COMMAND ""
   TEST_COMMAND ""
   INSTALL_COMMAND ""
   BUILD_ALWAYS 1
)
add_dependencies(evmone4eosio wasm2c_exports)
add_dependencies(evmone4eosio vm_api4c)

endif()

#add_subdirectory(test-contracts)
add_subdirectory(boost)
add_subdirectory(libzmq)
add_subdirectory(cppzmq)
add_dependencies(cppzmq libzmq)

# add_dependencies(aleth eth_account)
# add_dependencies(aleth vm_api)
