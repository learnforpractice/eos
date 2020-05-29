configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gen.py.in ${CMAKE_CURRENT_BINARY_DIR}/gen.py)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/python_vm_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/python_vm_config.h)

add_subdirectory( frozen )

# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gen.py.in ${CMAKE_CURRENT_BINARY_DIR}/gen.py)

# ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/pythonvm.c
#     COMMAND python3 ${CMAKE_CURRENT_BINARY_DIR}/gen.py
#     DEPENDS pythonvm ${CMAKE_BINARY_DIR}/contracts/pythonvm/pythonvm.wasm
#     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#     COMMENT "Generating ${CMAKE_CURRENT_SOURCE_DIR}/pythonvm.c"
# )

if (APPLE)
    set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
    set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)

set(SRCS
    pythonvm.c
    wasm-rt-impl.c
    vm_python.cpp
    ../wasm2c_exports/env.c
    ../wasm2c_exports/env.h
    ${CMAKE_CURRENT_SOURCE_DIR}/pythonvm.wasm.c
)

add_library(vm_python SHARED ${SRCS})

target_link_libraries(vm_python PRIVATE vm_api frozen chain_api vm_api4c)
set_target_properties(vm_python  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")

target_include_directories(vm_python PRIVATE ${Boost_INCLUDE_DIR}
    PRIVATE ${CMAKE_SOURCE_DIR}/externals/magic_get/include
    PRIVATE ${CMAKE_SOURCE_DIR}/contracts
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/libraries/softfloat/source/include
    PUBLIC ${CMAKE_SOURCE_DIR}/libraries/vm/vm_api4c
)

target_compile_options(vm_python PRIVATE -DWASM_RT_MAX_CALL_STACK_DEPTH=300)


# add_dependencies(vm_python pythonvm_project)

# ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/pythonvm.wasm.c
#     COMMAND python3 ${CMAKE_CURRENT_BINARY_DIR}/gen.py
#     DEPENDS contracts_project ${CMAKE_BINARY_DIR}/contracts/python37/pythonvm/pythonvm.wasm
#     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#     COMMENT "Generating ${CMAKE_CURRENT_SOURCE_DIR}/pythonvm.wasm.c"
# )

install(TARGETS vm_python
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

copy_lib(vm_python)


