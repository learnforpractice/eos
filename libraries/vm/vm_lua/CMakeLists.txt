
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vmlua_vm_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/vmlua_vm_config.h)

if (APPLE)
    set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
    set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)

set(SRCS
    vm_lua.cpp
    vmlua.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vmlua.c.bin
)

add_library(vm_lua SHARED ${SRCS})

target_link_libraries(vm_lua PRIVATE vm_api vm_api4c)
set_target_properties(vm_lua  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")

target_include_directories(vm_lua PRIVATE ${Boost_INCLUDE_DIR}
    PRIVATE ${CMAKE_SOURCE_DIR}/externals/magic_get/include
    PRIVATE ${CMAKE_SOURCE_DIR}/contracts
    PRIVATE ${CMAKE_SOURCE_DIR}/contracts/vm_api
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/libraries/softfloat/source/include
    PUBLIC ${CMAKE_SOURCE_DIR}/libraries/vm/vm_api4c
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/wasm2c
)

target_compile_options(vm_lua PRIVATE -DWASM_RT_MAX_CALL_STACK_DEPTH=300)


add_dependencies(vm_lua vmlua)
add_dependencies(vm_lua wasm2c)
add_dependencies(vm_lua wasm_injector)

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/vmlua.c.bin
    COMMAND ${CMAKE_BINARY_DIR}/libraries/vm/vm_wasm/wasm_injector vmlua vmlua_softfloat.wasm
    COMMAND ${CMAKE_BINARY_DIR}/libraries/wabt/wasm2c -o vmlua.c vmlua_softfloat.wasm
    COMMAND mv vmlua.c vmlua.c.bin
    COMMAND mv vmlua.h ${CMAKE_CURRENT_SOURCE_DIR}/vmlua.h
    COMMAND python3  ${CMAKE_CURRENT_SOURCE_DIR}/patch.py
    COMMAND mv vmlua.c.bin ${CMAKE_CURRENT_SOURCE_DIR}/vmlua.c.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/externals/wasm_lua
    DEPENDS vmlua
    COMMENT "Generating vmlua.c.bin"
)

install(TARGETS vm_lua
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

copy_lib(vm_lua)


