add_library(eosio.token.native SHARED 
${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.token/src/eosio.token.cpp
${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.token/include/eosio.token/eosio.token.hpp
main.cpp)

target_link_libraries( eosio.token.native PRIVATE eosiolib_native )

target_include_directories( eosio.token.native
                            PRIVATE ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.token/include
                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include"
#                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/libcxx"
#                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/libc"
                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/eosiolib/core"
                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/eosiolib/capi"
                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/eosiolib/contracts"
                        )
target_compile_options( eosio.token.native PRIVATE -DEOSIO_NATIVE -Wno-unknown-attributes )
