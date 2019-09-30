set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt REQUIRED)

add_subdirectory( libc_emscripten )
add_subdirectory( libpython37 )
add_subdirectory( pythonvm )
