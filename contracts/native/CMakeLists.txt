include(ExternalProject)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt REQUIRED)

ExternalProject_Add(
    native_contracts_project
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/native_contracts
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/native_contracts
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake -DEOSIO_COMPILE_CONTRACTS=TRUE
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    TEST_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_ALWAYS 1
  )
# add_subdirectory( native_contracts )
# add_subdirectory( eosiolib )
# add_subdirectory( eth_account )
