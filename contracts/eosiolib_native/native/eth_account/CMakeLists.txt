if( EOSIO_COMPILE_TEST_CONTRACTS )

if( EOSIO_COMPILE_TEST_CONTRACTS )
   set(EOSIO_WASM_OLD_BEHAVIOR "Off")
   find_package( eosio.cdt REQUIRED )
endif()

if ("${CMAKE_GENERATOR}" STREQUAL "Ninja")
   add_compile_options(-fcolor-diagnostics -D_NATIVE)
endif()

set(CMAKE_AR "/usr/bin/ar")
#add_contract( utility utility utility.cpp )
add_native_library(eth_account eth_account.cpp ./src/eosio.token.cpp)
target_compile_options(eth_account PUBLIC -D_NATIVE)

target_include_directories(eth_account
   PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

endif()
