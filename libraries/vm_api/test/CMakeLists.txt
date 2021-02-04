set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt REQUIRED)

include(ExternalProject)

message( STATUS "Building contracts in directory `eosio.contracts/contracts`" )
ExternalProject_Add(
   native_test_contract
   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test_contract
   BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_contract
   CMAKE_ARGS -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake
   UPDATE_COMMAND ""
   PATCH_COMMAND ""
   TEST_COMMAND ""
   INSTALL_COMMAND ""
   BUILD_ALWAYS 1
)

add_library(native_contract SHARED test.cpp)
target_link_libraries(native_contract PRIVATE vm_api ${CMAKE_CURRENT_BINARY_DIR}/test_contract/test/libhello.a)
add_dependencies(native_contract native_test_contract)

add_library(native_eosio_system SHARED main.cpp)
target_link_libraries(native_eosio_system PRIVATE vm_api ${CMAKE_CURRENT_BINARY_DIR}/test_contract/eosio.system/libeosio.system_native.a)
add_dependencies(native_eosio_system native_test_contract)

