add_library( vm_api SHARED
    vm_api.cpp
    action.cpp
    call_contract.cpp
    authorization.cpp
    cf_system.cpp
    cf_transaction.cpp
    # compiler_builtins.cpp
    console.cpp
    context_free.cpp
    crypto.cpp
    database.cpp
    privileged.cpp
    permission.cpp
    producer.cpp
    system.cpp
    transaction.cpp
)

if (APPLE)
else()
    target_compile_options(vm_api PRIVATE -fPIC)
endif()

if (APPLE)
      set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
      set(LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
      set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
      set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)

set_target_properties(vm_api  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")

target_include_directories(vm_api PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/libraries/chain/vm_api
)
