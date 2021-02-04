# set(EOSIO_WASM_OLD_BEHAVIOR "Off")
# find_package(eosio.cdt REQUIRED)

# include(ExternalProject)

# message( STATUS "Building contracts in directory `eosio.contracts/contracts`" )

# ExternalProject_Add(
#    native_test_contract
#    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test_contract
#    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_contract
#    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake
#    UPDATE_COMMAND ""
#    PATCH_COMMAND ""
#    TEST_COMMAND ""
#    INSTALL_COMMAND ""
#    BUILD_ALWAYS 1
# )

# set( CMAKE_CXX_STANDARD 17 )

# if(APPLE)
#         set(EOSIO_CDT_PATH ${EOSIO_CDT_ROOT}/opt/eosio.cdt)
# else()
#         set(EOSIO_CDT_PATH ${EOSIO_CDT_ROOT})
# endif()

# set(CMAKE_C_COMPILER "${EOSIO_CDT_PATH}/bin/clang-7")
# set(CMAKE_CXX_COMPILER "${EOSIO_CDT_PATH}/bin/clang-7")


# if (APPLE)
#       set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
#       set(LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
# else()
#       set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
#       set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
# endif (APPLE)

# add_library(native_eosio_system SHARED main.cpp)
# set_target_properties(native_eosio_system  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")
# target_link_libraries(native_eosio_system PRIVATE ${VM_API_LIB} ${CMAKE_CURRENT_BINARY_DIR}/test_contract/eosio.system/libeosio.system_native.a -L${EOSIO_CDT_PATH}/lib native_c++ native_c)
# add_dependencies(native_eosio_system native_test_contract)

# add_library(native_eosio_token SHARED main_token.cpp)
# set_target_properties(native_eosio_token  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")
# target_link_libraries(native_eosio_token PRIVATE ${VM_API_LIB} ${CMAKE_CURRENT_BINARY_DIR}/test_contract/eosio.token/libeosio.token_native.a -L${EOSIO_CDT_PATH}/lib native_c++ native_c)
# add_dependencies(native_eosio_token native_test_contract)


set(EOSIO_SYSTEM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test_contract/eosio.system)
set(EOSIO_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test_contract/include)

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
      set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
      set(eosio_system_LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
      set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
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
target_link_libraries(native_eosio_system2 PRIVATE ${VM_API_LIB})

