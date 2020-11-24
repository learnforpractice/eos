
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/micropython_vm_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/micropython_vm_config.h)

if (APPLE)
    set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
    set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)

set(SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/micropython.c
    ${CMAKE_CURRENT_SOURCE_DIR}/micropython.c.bin
    vm_api.c
    vm_micropython.cpp
    code_loader.c
)

add_library(vm_micropython SHARED ${SRCS})

target_link_libraries(vm_micropython PRIVATE vm_api vm_api4c mpy-cross)
set_target_properties(vm_micropython  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")

target_include_directories(vm_micropython PRIVATE ${Boost_INCLUDE_DIR}
    PRIVATE ${CMAKE_SOURCE_DIR}/externals/magic_get/include
    PRIVATE ${CMAKE_SOURCE_DIR}/contracts
    PRIVATE ${CMAKE_SOURCE_DIR}/contracts/vm_api
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/libraries/softfloat/source/include
    PUBLIC ${CMAKE_SOURCE_DIR}/libraries/vm/vm_api4c
    PUBLIC ${CMAKE_SOURCE_DIR}/libraries/vm/vm_api4c/wasm2c
)

target_compile_options(vm_micropython PRIVATE -DWASM_RT_MAX_CALL_STACK_DEPTH=300)

# add_dependencies(vm_micropython pythonvm_project)

# ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/pythonvm.wasm.c
#     COMMAND python3 ${CMAKE_CURRENT_BINARY_DIR}/gen.py
#     DEPENDS contracts_project ${CMAKE_BINARY_DIR}/contracts/python37/pythonvm/pythonvm.wasm
#     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#     COMMENT "Generating ${CMAKE_CURRENT_SOURCE_DIR}/pythonvm.wasm.c"
# )

add_dependencies(vm_micropython micropython)
add_dependencies(vm_micropython wasm2c)
add_dependencies(vm_micropython wasm_injector)

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/micropython.c.bin
    COMMAND ${CMAKE_BINARY_DIR}/libraries/vm/vm_wasm/wasm_injector micropython micropython_softfloat.wasm
    COMMAND ${CMAKE_BINARY_DIR}/libraries/wabt/wasm2c -o micropython.c micropython_softfloat.wasm
    COMMAND mv micropython.c micropython.c.bin
    COMMAND mv micropython.h ${CMAKE_CURRENT_SOURCE_DIR}/micropython.h
    COMMAND python3  ${CMAKE_CURRENT_SOURCE_DIR}/patch.py
    COMMAND mv micropython.c.bin ${CMAKE_CURRENT_SOURCE_DIR}/micropython.c.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/externals/micropython/ports/uuosio
    DEPENDS micropython
    COMMENT "Generating micropython.c.bin"
)

add_executable(run_test main.c)
target_link_libraries(run_test PRIVATE vm_micropython)

install(TARGETS vm_micropython
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

copy_lib(vm_micropython)


