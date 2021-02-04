set(CMAKE_BUILD_TYPE Debug)
set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_subdirectory(eosio.system)
add_subdirectory(test)
