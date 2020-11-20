add_subdirectory( eosiolib )
add_subdirectory( native_contracts )
add_subdirectory( vm_api )
add_subdirectory( test )

include(ExternalProject)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt REQUIRED)
#message( STATUS "Building contracts in directory utility" )

set(PYTHON_VM_MEMORY_DEFINES -DPYTHON_VM_STACK_SIZE=${PYTHON_VM_STACK_SIZE})
set(PYTHON_VM_MEMORY_DEFINES ${PYTHON_VM_MEMORY_DEFINES} -DPYTHON_VM_MAX_MEMORY_SIZE=${PYTHON_VM_MAX_MEMORY_SIZE})
set(PYTHON_VM_MEMORY_DEFINES ${PYTHON_VM_MEMORY_DEFINES} -DPYTHON_VM_MAX_TEMP_MEMORY_SIZE=${PYTHON_VM_MAX_TEMP_MEMORY_SIZE})


if (APPLE)
  set(BUILD_PYTHON_VM TRUE)
endif()

