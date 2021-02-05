add_library(vm_api4c STATIC 
    vm_api4c.cpp
    action.cpp
    chain.cpp
    crypto.cpp
    db.cpp
    permission.cpp
    print.cpp
    privileged.cpp
    # softfloat.cpp
    # eosio_injection.cpp
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
