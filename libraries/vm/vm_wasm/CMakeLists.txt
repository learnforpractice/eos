
set( HEADERS
    ${CMAKE_SOURCE_DIR}/libraries/fc/include
    ${CMAKE_SOURCE_DIR}/externals/magic_get/include
    ${CMAKE_SOURCE_DIR}/libraries/wasm-jit/Include
    ${CMAKE_SOURCE_DIR}/libraries/wasm-jit/Source/Runtime
    ${CMAKE_SOURCE_DIR}/libraries/wasm-jit/Include/Runtime
    ${CMAKE_SOURCE_DIR}/contracts
    ${CMAKE_SOURCE_DIR}/contracts/vm_api
    ${CMAKE_SOURCE_DIR}/libraries/chainbase/include
    ${CMAKE_SOURCE_DIR}/libraries/chain/include
    ${CMAKE_SOURCE_DIR}/libraries/wabt
    ${CMAKE_BINARY_DIR}/libraries/wabt
    ${CMAKE_SOURCE_DIR}/libraries/builtins
    ${CMAKE_SOURCE_DIR}/externals/binaryen/src 
    ${CMAKE_SOURCE_DIR}/libraries/vm/vm_interface
    ${Boost_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    )

set(SRCS vm_wasm.cpp
    wasm_interface.cpp
    vm_api.cpp
    ${CMAKE_SOURCE_DIR}/libraries/chain/wasm_eosio_injection.cpp
    ${CMAKE_SOURCE_DIR}/libraries/chain/wasm_eosio_validation.cpp
    ${CMAKE_SOURCE_DIR}/libraries/chain/wasm_eosio_constraints.cpp
    ${CMAKE_SOURCE_DIR}/libraries/chain/webassembly/wabt.cpp
    ${CMAKE_SOURCE_DIR}/libraries/chain/webassembly/wavm.cpp
    ${CMAKE_SOURCE_DIR}/libraries/chain/whitelisted_intrinsics.cpp
    )

if (APPLE)
    set(_symbols_list "${CMAKE_SOURCE_DIR}/libraries/vm/vm_wasm/symbols.list")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
    set(_version_script "${CMAKE_SOURCE_DIR}/libraries/vm/vm_wasm/version.script")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)

set(WASM_DEPEND_LIBS Logging IR WAST WASM Runtime softfloat builtins vm_api chain_api)

# add_library( vm_wasm-1 SHARED ${SRCS} )

# #set(LIBEVM4EOS ${CMAKE_SOURCE_DIR}/externals/aleth/build/evm4eos/libevm4eos${CMAKE_SHARED_LIBRARY_SUFFIX})
# target_link_libraries( vm_wasm-1 wabt ${WASM_DEPEND_LIBS} fc)

# target_include_directories( vm_wasm-1
#                             PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" 
#                             PUBLIC ${HEADERS}
#                             )
# target_compile_options(vm_wasm-1     PRIVATE -D_INDEX=1 -D_EOSIO_SHARED_LIB)
# set_target_properties(vm_wasm-1  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")


set(SRCS_INJECTOR 
#    vm_wasm.cpp
#    wasm_interface.cpp
    vm_api.cpp
    ${CMAKE_SOURCE_DIR}/libraries/wasm-jit/Source/WASM/WASMSerialization.cpp
    ${CMAKE_SOURCE_DIR}/libraries/chain/wasm_eosio_injection.cpp
    ${CMAKE_SOURCE_DIR}/libraries/chain/wasm_eosio_validation.cpp
    ${CMAKE_SOURCE_DIR}/libraries/chain/wasm_eosio_constraints.cpp
#    ${CMAKE_SOURCE_DIR}/libraries/chain/webassembly/wabt.cpp
#    ${CMAKE_SOURCE_DIR}/libraries/chain/webassembly/wavm.cpp
    ${CMAKE_SOURCE_DIR}/libraries/chain/whitelisted_intrinsics.cpp
)

set(WASM_INJECTOR_DEPEND_LIBS Logging IR WASM Runtime softfloat builtins vm_api chain_api)

add_executable( wasm_injector main.cpp ${SRCS_INJECTOR} )
target_link_libraries( wasm_injector wabt ${WASM_INJECTOR_DEPEND_LIBS} fc)

target_include_directories( wasm_injector
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" 
                            PRIVATE ${CMAKE_SOURCE_DIR}/libraries/wasm-jit/Include
                            PRIVATE ${CMAKE_SOURCE_DIR}/libraries/wasm-jit/Include/WASM
                            PUBLIC ${HEADERS}
                            )

target_compile_options(wasm_injector  PRIVATE -D_INDEX=1 -DWASM_INJECTOR_FOR_PYTHON_VM)


add_executable( wasm_preload main_preload.cpp ${SRCS_INJECTOR} )
target_link_libraries( wasm_preload wabt ${WASM_INJECTOR_DEPEND_LIBS} fc vm_micropython)

target_include_directories( wasm_preload
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" 
                            PRIVATE ${CMAKE_SOURCE_DIR}/libraries/wasm-jit/Include
                            PRIVATE ${CMAKE_SOURCE_DIR}/libraries/wasm-jit/Include/WASM
                            PUBLIC ${HEADERS}
                            )

target_compile_options(wasm_preload  PRIVATE -D_INDEX=1 -DWASM_INJECTOR_FOR_PYTHON_VM)

# install(TARGETS vm_wasm-1
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )

# copy_lib(vm_wasm-1)
