set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package( eosio.cdt REQUIRED )

include(ExternalProject)
ExternalProject_Add(
    test_project
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/src
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    TEST_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_ALWAYS 1
  )
