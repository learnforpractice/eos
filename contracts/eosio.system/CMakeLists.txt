add_library(eosio.system.native SHARED main.cpp)

set(eosio_system_static_lib ${CMAKE_BINARY_DIR}/externals/eosio.contracts/contracts/eosio.system/libeosio.system.native.a)
set(eosiolib_static_lib ${CMAKE_BINARY_DIR}/externals/eosio.contracts/contracts/eosiolib/libeosiolib.a)

target_link_libraries( eosio.system.native PUBLIC softfloat ${eosio_system_static_lib} ${eosiolib_static_lib} eosiolib_native)

set_target_properties(eosio.system.native  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")

add_dependencies(eosio.system.native contracts_project)
