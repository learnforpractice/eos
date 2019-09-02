add_library(eosio.token.native SHARED main.cpp)

set(eosio_token_static_lib ${CMAKE_BINARY_DIR}/externals/eosio.contracts/contracts/eosio.token/libeosio.token.native.a)
set(eosiolib_static_lib ${CMAKE_BINARY_DIR}/externals/eosio.contracts/contracts/eosiolib/libeosiolib.a)

target_link_libraries( eosio.token.native PUBLIC ${eosio_token_static_lib} ${eosiolib_static_lib} eosiolib_native)

add_dependencies(eosio.token.native contracts_project)
