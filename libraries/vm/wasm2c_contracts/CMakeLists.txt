add_library(wasm2c_contracts SHARED
    main.cpp
    wasm-rt-impl.c
    native_contracts.cpp
)

target_link_libraries(wasm2c_contracts PRIVATE vm_api vm_api4c)
target_include_directories(wasm2c_contracts PRIVATE ${Boost_INCLUDE_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/libraries/vm/vm_api4c
    PRIVATE ${CMAKE_SOURCE_DIR}/contracts
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

if (APPLE)
    set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
    set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)
set_target_properties(wasm2c_contracts  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")


#target_compile_options(eosio.system PRIVATE -fvisibility=hidden)


install(TARGETS wasm2c_contracts
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

#copy_lib(wasm2c_contracts)
