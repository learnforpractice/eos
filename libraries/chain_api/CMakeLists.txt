add_subdirectory(chain_api_impl)

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
    ${CMAKE_SOURCE_DIR}/libraries/chain_api
    ${Boost_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    )

if (BUILD_SHARED)
    add_library( chain_api SHARED chain_api.cpp chain_exceptions.cpp)
else()
    add_library( chain_api STATIC chain_api.cpp chain_exceptions.cpp)
endif()

if (APPLE)
    set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
    set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)

set_target_properties(chain_api  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")

target_link_libraries( chain_api fc)

target_include_directories( chain_api
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" 
                            PUBLIC ${HEADERS}
                          )

install(TARGETS chain_api
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

copy_lib(chain_api)

