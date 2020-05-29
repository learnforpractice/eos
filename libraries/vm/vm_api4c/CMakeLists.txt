add_library(vm_api4c STATIC 
    vm_api4c.cpp
    chain_api.cpp
    softfloat.cpp
    eosio_injection.cpp
    compiler_builtins.cpp
    privileged.cpp
#    vm_memory.cpp
#    wasm-rt-impl.c
)

target_link_libraries(vm_api4c PRIVATE vm_api chain_api)

target_include_directories(vm_api4c PRIVATE ${Boost_INCLUDE_DIR}
    PRIVATE ${CMAKE_SOURCE_DIR}/externals/magic_get/include
    PRIVATE ${CMAKE_SOURCE_DIR}/contracts
    ${CMAKE_SOURCE_DIR}/libraries/wabt
    ${CMAKE_BINARY_DIR}/libraries/wabt
    ${CMAKE_SOURCE_DIR}/libraries/vm/wasm2c_exports
)


