add_library( vm_api SHARED
    vm_api.cpp
    action.cpp
    chain.cpp
    crypto.cpp
    db.cpp
    permission.cpp
    print.cpp
    privileged.cpp
    system.cpp
    transaction.cpp
)

if (APPLE)
else()
    target_compile_options(vm_api PRIVATE -fPIC -D__NATIVE)
endif()

if (APPLE)
      set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
      set(vm_api_LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
      set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
      set(vm_api_LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)

set_target_properties(vm_api  PROPERTIES LINK_FLAGS "${vm_api_LINK_FLAGS}")

target_include_directories(vm_api PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libraries/chain/vm_api
    ${CMAKE_SOURCE_DIR}/libraries/chain/chain_api
)

add_subdirectory(vm_api4c)

# set(EOSIO_WASM_OLD_BEHAVIOR "Off")
# find_package(eosio.cdt REQUIRED)

include(ExternalProject)

message( STATUS "Building contracts in directory `eosio.contracts/contracts`" )

ExternalProject_Add(
   native_test_contract
   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test
   BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/test
   CMAKE_ARGS -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DVM_API_LIB=${CMAKE_CURRENT_BINARY_DIR}/libvm_api${CMAKE_SHARED_LIBRARY_SUFFIX}
   UPDATE_COMMAND ""
   PATCH_COMMAND ""
   TEST_COMMAND ""
   INSTALL_COMMAND ""
   BUILD_ALWAYS 1
)

add_dependencies(native_test_contract vm_api)
