
set(CMAKE_BUILD_TYPE Debug)
set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_subdirectory(eosio.system)
# add_subdirectory(test)


if( TRUE )

else()

# set( CMAKE_CXX_STANDARD 17 )

# set(EOSIO_SYSTEM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/eosio.system)
# add_library(native_eosio_system2 SHARED
#    main.cpp
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
#    ${EOSIO_SYSTEM_DIR}/src/crypto.cpp
# )

# target_include_directories( native_eosio_system2 
#    PRIVATE ${EOSIO_SYSTEM_DIR}/include
#    PRIVATE ${EOSIO_SYSTEM_DIR}/../eosio.token/include
#    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
#    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/eosiolib/core
#    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/eosiolib/capi
#    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/eosiolib/contracts
# )

# target_compile_definitions(native_eosio_system2 PRIVATE EOSIO_NATIVE)

# target_compile_options(native_eosio_system2 PRIVATE -Wno-error=unknown-warning-option)

# set_target_properties(native_eosio_system2  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")
# target_link_libraries(native_eosio_system2 PRIVATE ${VM_API_LIB})

endif()
