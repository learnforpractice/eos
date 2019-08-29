add_subdirectory( eosiolib )
add_subdirectory( eosiolib_native )

include(ExternalProject)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt REQUIRED)

message( STATUS "Building contracts in directory utility" )
ExternalProject_Add(
   contracts_project
   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/python37
   BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/python37
   CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake -DEOSIO_COMPILE_CONTRACTS=TRUE
   UPDATE_COMMAND ""
   PATCH_COMMAND ""
   TEST_COMMAND ""
   INSTALL_COMMAND ""
   BUILD_ALWAYS 1
)


ExternalProject_Add(
   native_project
   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/eosiolib_native/native
   BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/eosiolib_native/native
   CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake -DEOSIO_COMPILE_CONTRACTS=TRUE
   UPDATE_COMMAND ""
   PATCH_COMMAND ""
   TEST_COMMAND ""
   INSTALL_COMMAND ""
   BUILD_ALWAYS 1
)
