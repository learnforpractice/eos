set(CHAIN_DIR ${CMAKE_SOURCE_DIR}/libraries/chain)
set(CHAIN_BINARY_DIR ${CMAKE_BINARY_DIR}/libraries/chain)

file(GLOB HEADERS "${CHAIN_DIR}/include/eosio/chain/*.hpp"
                  "${CHAIN_DIR}/include/eosio/chain/webassembly/*.hpp"
                  "${CHAIN_BINARY_DIR}/include/eosio/chain/core_symbol.hpp" )

if(APPLE AND UNIX)
   set(PLATFORM_TIMER_IMPL ${CHAIN_DIR}/platform_timer_macos.cpp)
else()
   try_run(POSIX_TIMER_TEST_RUN_RESULT POSIX_TIMER_TEST_COMPILE_RESULT ${CMAKE_CURRENT_BINARY_DIR} ${CHAIN_DIR}/platform_timer_posix_test.c)
   if(POSIX_TIMER_TEST_RUN_RESULT EQUAL 0)
      set(PLATFORM_TIMER_IMPL ${CHAIN_DIR}/platform_timer_posix.cpp)
      set(CHAIN_RT_LINKAGE rt)
   else()
      set(PLATFORM_TIMER_IMPL ${CHAIN_DIR}/platform_timer_asio_fallback.cpp)
   endif()
endif()

if("eos-vm-oc" IN_LIST EOSIO_WASM_RUNTIMES)
   set(CHAIN_EOSVMOC_SOURCES ${CHAIN_DIR}/webassembly/eos-vm-oc/code_cache.cpp
                           ${CHAIN_DIR}/webassembly/eos-vm-oc/executor.cpp
                           ${CHAIN_DIR}/webassembly/eos-vm-oc/memory.cpp
                           ${CHAIN_DIR}/webassembly/eos-vm-oc/intrinsic.cpp
                           ${CHAIN_DIR}/webassembly/eos-vm-oc/LLVMJIT.cpp
                           ${CHAIN_DIR}/webassembly/eos-vm-oc/LLVMEmitIR.cpp
                           ${CHAIN_DIR}/webassembly/eos-vm-oc/compile_monitor.cpp
                           ${CHAIN_DIR}/webassembly/eos-vm-oc/compile_trampoline.cpp
                           ${CHAIN_DIR}/webassembly/eos-vm-oc/ipc_helpers.cpp
                           ${CHAIN_DIR}/webassembly/eos-vm-oc/gs_seg_helpers.c
                           ${CHAIN_DIR}/webassembly/eos-vm-oc.cpp)

   if(LLVM_VERSION VERSION_LESS 7.1 AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      enable_language(ASM-LLVMWAR)
      list(APPEND CHAIN_EOSVMOC_SOURCES ${CHAIN_DIR}/webassembly/eos-vm-oc/llvmWARshim.llvmwar)
   else()
      list(APPEND CHAIN_EOSVMOC_SOURCES ${CHAIN_DIR}/webassembly/eos-vm-oc/llvmWARshim.cpp)
   endif()

   llvm_map_components_to_libnames(LLVM_LIBS support core passes mcjit native orcjit)
   include_directories(${LLVM_INCLUDE_DIRS})
   add_definitions(${LLVM_DEFINITIONS})

   option(EOSVMOC_ENABLE_DEVELOPER_OPTIONS "enable developer options for EOS VM OC" OFF)
endif()

if("eos-vm" IN_LIST EOSIO_WASM_RUNTIMES OR "eos-vm-jit" IN_LIST EOSIO_WASM_RUNTIMES)
   set(CHAIN_EOSVM_SOURCES "${CHAIN_DIR}/webassembly/eos-vm.cpp")
   set(CHAIN_EOSVM_LIBRARIES eos-vm)
endif()

## SORT .cpp by most likely to change / break compile
add_library( vm_eos SHARED
            ${CHAIN_DIR}/chain_config.cpp
            ${CHAIN_DIR}/chain_id_type.cpp
            ${CHAIN_DIR}/genesis_state.cpp
            ${CHAIN_BINARY_DIR}/genesis_state_root_key.cpp

            ${CHAIN_DIR}/wast_to_wasm.cpp
            wasm_interface.cpp

            ${CHAIN_DIR}/wasm_eosio_validation.cpp
            ${CHAIN_DIR}/wasm_eosio_injection.cpp

            ${CHAIN_DIR}/webassembly/wabt.cpp

             ${CHAIN_EOSVMOC_SOURCES}
             ${CHAIN_EOSVM_SOURCES}

            ${CHAIN_DIR}/genesis_intrinsics.cpp
            ${CHAIN_DIR}/whitelisted_intrinsics.cpp
            ${CHAIN_DIR}/thread_utils.cpp
            ${CHAIN_DIR}/platform_timer_accuracy.cpp

            ${PLATFORM_TIMER_IMPL}
            ${HEADERS}
             )


if (APPLE)
   set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
   set(LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
   set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
   set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)

set_target_properties(vm_eos  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")

# set(LIBEVM4EOS ${CMAKE_BINARY_DIR}/externals/evmone4eosio/lib/libevmone.dylib)

target_link_libraries( vm_eos fc Logging IR WAST WASM Runtime
                       softfloat builtins wabt ${CHAIN_EOSVM_LIBRARIES} ${LLVM_LIBS} ${CHAIN_RT_LINKAGE}
                       vm_api
                     )

target_include_directories( vm_eos
                            PUBLIC "${CHAIN_DIR}/include"
                                    "${CHAIN_BINARY_DIR}/include"
                                    "${CHAIN_DIR}/../wasm-jit/Include"
                                    "${CHAIN_DIR}/libraries/eos-vm/include"
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
                              PUBLIC "${CMAKE_SOURCE_DIR}/libraries/vm/vm_manager/include"
                              PRIVATE "${CMAKE_SOURCE_DIR}/libraries/chainbase/include"
                        )


# add_dependencies(vm_eos aleth)

if("eos-vm-oc" IN_LIST EOSIO_WASM_RUNTIMES)
   target_link_libraries(vm_eos "-Wl,-wrap=main")
endif()

foreach(RUNTIME ${EOSIO_WASM_RUNTIMES})
   string(TOUPPER "${RUNTIME}" RUNTIMEUC)
   string(REPLACE "-" "_" RUNTIMEUC ${RUNTIMEUC})
   target_compile_definitions(vm_eos PUBLIC "EOSIO_${RUNTIMEUC}_RUNTIME_ENABLED")
endforeach()

if(EOSVMOC_ENABLE_DEVELOPER_OPTIONS)
   message(WARNING "EOS VM OC Developer Options are enabled; these are NOT supported")
   target_compile_definitions(vm_eos PUBLIC EOSIO_EOS_VM_OC_DEVELOPER)
endif()

copy_lib(vm_eos)

install( TARGETS vm_eos
   RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
   LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
)

install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/eosio/chain/
      DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/eosio/chain
      FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" EXCLUDE
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/eosio/chain/core_symbol.hpp DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/eosio/chain)
#if(MSVC)
#  set_source_files_properties( db_init.cpp db_block.cpp database.cpp block_log.cpp PROPERTIES COMPILE_FLAGS "/bigobj" )
#endif(MSVC)
