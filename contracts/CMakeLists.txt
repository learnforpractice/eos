add_subdirectory( eosiolib )
add_subdirectory( eosiolib_native )
add_subdirectory( native )
add_subdirectory( test )

include(ExternalProject)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt REQUIRED)
#message( STATUS "Building contracts in directory utility" )

set(PYTHON_VM_MEMORY_DEFINES -DPYTHON_VM_STACK_SIZE=${PYTHON_VM_STACK_SIZE})
set(PYTHON_VM_MEMORY_DEFINES ${PYTHON_VM_MEMORY_DEFINES} -DPYTHON_VM_MAX_MEMORY_SIZE=${PYTHON_VM_MAX_MEMORY_SIZE})
set(PYTHON_VM_MEMORY_DEFINES ${PYTHON_VM_MEMORY_DEFINES} -DPYTHON_VM_MAX_CODE_SIZE=${PYTHON_VM_MAX_CODE_SIZE})
set(PYTHON_VM_MEMORY_DEFINES ${PYTHON_VM_MEMORY_DEFINES} -DPYTHON_VM_MAX_FROZEN_CODE_SIZE=${PYTHON_VM_MAX_FROZEN_CODE_SIZE})


#set(BUILD_PYTHON_VM TRUE)
if (BUILD_PYTHON_VM)
  ExternalProject_Add(
    pythonvm_project
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/python37
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/python37
    CMAKE_ARGS ${PYTHON_VM_MEMORY_DEFINES} -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake -DEOSIO_COMPILE_CONTRACTS=TRUE
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    TEST_COMMAND ""
    INSTALL_COMMAND ""
    BUILD_ALWAYS 1
  )
endif()

# ExternalProject_Add(
#    native_project
#    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/eosiolib_native/native
#    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/eosiolib_native/native
#    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake -DEOSIO_COMPILE_CONTRACTS=TRUE
#    UPDATE_COMMAND ""
#    PATCH_COMMAND ""
#    TEST_COMMAND ""
#    INSTALL_COMMAND ""
#    BUILD_ALWAYS 1
# )
