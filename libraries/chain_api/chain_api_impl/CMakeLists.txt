file(GLOB HEADERS "include/eosio/chain/*.hpp"
                  "include/eosio/chain/webassembly/*.hpp"
                  "${CMAKE_CURRENT_BINARY_DIR}/include/eosio/chain/core_symbol.hpp" )

add_library( chain_api_impl
             chain_api.cpp
             chain_exceptions.cpp
             chain_manager.cpp
             native_object.cpp
)

target_link_libraries(chain_api_impl eosio_chain mpy-cross)

target_include_directories(chain_api_impl
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}/include"
                                   "${CMAKE_CURRENT_SOURCE_DIR}/../wasm-jit/Include"
                                   "${CMAKE_CURRENT_SOURCE_DIR}/libraries/eos-vm/include"
                                   "${CMAKE_SOURCE_DIR}/libraries/wabt"
                                   "${CMAKE_BINARY_DIR}/libraries/wabt"
                                    "${CMAKE_SOURCE_DIR}/contracts/vm_api/include"
                                    "${CMAKE_SOURCE_DIR}/contracts/vm_api"
                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts"
                              PRIVATE "${CMAKE_BINARY_DIR}/contracts"
                              PRIVATE ${CMAKE_SOURCE_DIR}/externals/magic_get/include
                              PRIVATE "${CMAKE_SOURCE_DIR}/libraries/chain_api"
                              PRIVATE "${CMAKE_SOURCE_DIR}/libraries/chain/db_api/include"
                              PUBLIC "${CMAKE_SOURCE_DIR}/libraries/chain/db_api"
                              PRIVATE  ${CMAKE_SOURCE_DIR}/plugins/net_plugin/include
                        )
