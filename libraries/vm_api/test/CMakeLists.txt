# add_library(native_contract SHARED test.cpp)
# target_link_libraries(native_contract PRIVATE vm_api ${CMAKE_CURRENT_BINARY_DIR}/test_contract/test/libhello.a)
# add_dependencies(native_contract native_test_contract)


set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt REQUIRED)

include(ExternalProject)

message( STATUS "Building contracts in directory `eosio.contracts/contracts`" )

ExternalProject_Add(
   native_test_contract
   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test_contract
   BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_contract
   CMAKE_ARGS -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake
   UPDATE_COMMAND ""
   PATCH_COMMAND ""
   TEST_COMMAND ""
   INSTALL_COMMAND ""
   BUILD_ALWAYS 1
)

set( CMAKE_CXX_STANDARD 17 )

if(APPLE)
        set(EOSIO_CDT_PATH ${EOSIO_CDT_ROOT}/opt/eosio.cdt)
else()
        set(EOSIO_CDT_PATH ${EOSIO_CDT_ROOT})
endif()

set(CMAKE_C_COMPILER "${EOSIO_CDT_PATH}/bin/clang-7")
set(CMAKE_CXX_COMPILER "${EOSIO_CDT_PATH}/bin/clang-7")

# ExternalProject_Add(
#    native_test_contract
#    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/test_contract
#    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/test/test_contract
#    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DVM_API_LIB=${CMAKE_CURRENT_BINARY_DIR}/libvm_api.dylib
#    UPDATE_COMMAND ""
#    PATCH_COMMAND ""
#    TEST_COMMAND ""
#    INSTALL_COMMAND ""
#    BUILD_ALWAYS 1
# )

if (APPLE)
      set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
      set(LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
      set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
      set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)

add_library(native_eosio_system SHARED main.cpp)
set_target_properties(native_eosio_system  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")
target_link_libraries(native_eosio_system PRIVATE ${VM_API_LIB} ${CMAKE_CURRENT_BINARY_DIR}/test_contract/eosio.system/libeosio.system_native.a -L${EOSIO_CDT_PATH}/lib native_c++ native_c)
add_dependencies(native_eosio_system native_test_contract)

add_library(native_eosio_token SHARED main_token.cpp)
set_target_properties(native_eosio_token  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")
target_link_libraries(native_eosio_token PRIVATE ${VM_API_LIB} ${CMAKE_CURRENT_BINARY_DIR}/test_contract/eosio.token/libeosio.token_native.a -L${EOSIO_CDT_PATH}/lib native_c++ native_c)
add_dependencies(native_eosio_token native_test_contract)
