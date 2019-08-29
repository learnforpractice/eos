
set( HEADERS
    ${CMAKE_SOURCE_DIR}/libraries/fc/include
    ${CMAKE_SOURCE_DIR}/externals/magic_get/include
    ${CMAKE_SOURCE_DIR}/libraries/wasm-jit/Include
    ${CMAKE_SOURCE_DIR}/contracts
    ${CMAKE_SOURCE_DIR}/libraries/chainbase/include
    ${CMAKE_SOURCE_DIR}/libraries/chain/include
    ${CMAKE_SOURCE_DIR}/libraries/wabt
    ${CMAKE_BINARY_DIR}/libraries/wabt
    ${CMAKE_SOURCE_DIR}/libraries/builtins
    ${CMAKE_SOURCE_DIR}/externals/binaryen/src 
    ${CMAKE_SOURCE_DIR}/libraries/vm/vm_interface
    ${CMAKE_SOURCE_DIR}/libraries/db_api/include
    ${CMAKE_SOURCE_DIR}/libraries/chain_api
    ${CMAKE_SOURCE_DIR}/contracts
    ${CMAKE_BINARY_DIR}/contracts
    ${Boost_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    )

add_library( vm_manager STATIC vm_manager.cpp)
target_link_libraries( vm_manager vm_wasm-1 vm_python2)


target_include_directories( vm_manager
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" 
                            "${CMAKE_SOURCE_DIR}/libraries/chain/db_api/include"
                            PUBLIC ${HEADERS}
                          )

