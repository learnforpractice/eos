set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_native_library(hello main.cpp)
