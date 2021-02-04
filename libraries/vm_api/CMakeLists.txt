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

# set(EOSIO_WASM_OLD_BEHAVIOR "Off")
# find_package(eosio.cdt REQUIRED)

# include(ExternalProject)

# message( STATUS "Building contracts in directory `eosio.contracts/contracts`" )

# ExternalProject_Add(
#    native_test_contract
#    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test
#    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/test
#    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Debug -DVM_API_LIB=${CMAKE_CURRENT_BINARY_DIR}/libvm_api${CMAKE_SHARED_LIBRARY_SUFFIX}
#    UPDATE_COMMAND ""
#    PATCH_COMMAND ""
#    TEST_COMMAND ""
#    INSTALL_COMMAND ""
#    BUILD_ALWAYS 1
# )

# add_dependencies(native_test_contract vm_api)




set(EOSIO_SYSTEM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/test_contract/eosio.system)
set(EOSIO_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/test_contract/include)

add_library(native_eosio_system2 SHARED
   ${EOSIO_SYSTEM_DIR}/src/main.cpp
#    ${EOSIO_SYSTEM_DIR}/src/eosio.system.cpp
#    ${EOSIO_SYSTEM_DIR}/src/delegate_bandwidth.cpp
#    ${EOSIO_SYSTEM_DIR}/src/exchange_state.cpp
#    ${EOSIO_SYSTEM_DIR}/src/name_bidding.cpp
#    ${EOSIO_SYSTEM_DIR}/src/native.cpp
#    ${EOSIO_SYSTEM_DIR}/src/producer_pay.cpp
#    ${EOSIO_SYSTEM_DIR}/src/powerup.cpp
#    ${EOSIO_SYSTEM_DIR}/src/rex.cpp
#    ${EOSIO_SYSTEM_DIR}/src/voting.cpp
#    ${EOSIO_SYSTEM_DIR}/src/eosiolib.cpp
   ${EOSIO_SYSTEM_DIR}/src/crypto.cpp
)

if (APPLE)
      set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/test/symbols.list")
      set(eosio_system_LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
      set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/test/version.script")
      set(eosio_system_LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)

target_include_directories( native_eosio_system2
   PRIVATE ${EOSIO_SYSTEM_DIR}/include
   PRIVATE ${EOSIO_SYSTEM_DIR}/../eosio.token/include
   PRIVATE ${EOSIO_HEADER_DIR}
   PRIVATE ${EOSIO_HEADER_DIR}/eosiolib/core
   PRIVATE ${EOSIO_HEADER_DIR}/eosiolib/capi
   PRIVATE ${EOSIO_HEADER_DIR}/eosiolib/contracts
)

target_compile_definitions(native_eosio_system2 PRIVATE EOSIO_NATIVE)

target_compile_options(native_eosio_system2 PRIVATE -Wno-unknown-attributes)

set_target_properties(native_eosio_system2  PROPERTIES LINK_FLAGS "${eosio_system_LINK_FLAGS}")
target_link_libraries(native_eosio_system2 PRIVATE vm_api)
