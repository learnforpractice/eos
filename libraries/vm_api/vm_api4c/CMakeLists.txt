add_library(vm_api4c STATIC 
    vm_api4c.cpp
    # chain_api.cpp
    softfloat.cpp
    eosio_injection.cpp
    # compiler_builtins.cpp
    privileged.cpp
    wasm2c/wasm-rt-impl.c
    # jmp_stack.cpp
)

target_link_libraries(vm_api4c PRIVATE vm_api softfloat)

target_include_directories(vm_api4c PRIVATE
    PRIVATE ${CMAKE_SOURCE_DIR}/libraries/chain/chain_api
    PRIVATE ${CMAKE_SOURCE_DIR}/libraries/chain/vm_api
    PRIVATE ${CMAKE_SOURCE_DIR}/libraries/vm_api/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wasm2c
)
