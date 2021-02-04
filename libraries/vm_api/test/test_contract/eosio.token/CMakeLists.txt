set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_native_library(eosio.token_native
   src/eosio.token.cpp
   # ${CMAKE_CURRENT_SOURCE_DIR}/src/eosio.system.cpp
   # ${CMAKE_CURRENT_SOURCE_DIR}/src/delegate_bandwidth.cpp
   # ${CMAKE_CURRENT_SOURCE_DIR}/src/exchange_state.cpp
   # ${CMAKE_CURRENT_SOURCE_DIR}/src/name_bidding.cpp
   # ${CMAKE_CURRENT_SOURCE_DIR}/src/native.cpp
   # ${CMAKE_CURRENT_SOURCE_DIR}/src/producer_pay.cpp
   # ${CMAKE_CURRENT_SOURCE_DIR}/src/powerup.cpp
   # ${CMAKE_CURRENT_SOURCE_DIR}/src/rex.cpp
   # ${CMAKE_CURRENT_SOURCE_DIR}/src/voting.cpp
   # ${CMAKE_CURRENT_SOURCE_DIR}/src/eosiolib.cpp
   # ${CMAKE_CURRENT_SOURCE_DIR}/src/crypto.cpp
)

target_include_directories(eosio.token_native
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../eosio.token/include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")

target_compile_definitions(eosio.token_native PRIVATE __NATIVE)

target_compile_options(eosio.token_native PRIVATE -O=0 -no-abigen)


# add_contract(eosio.token eosio.token ${CMAKE_CURRENT_SOURCE_DIR}/src/eosio.token.cpp)

# target_include_directories(eosio.token
#    PUBLIC
#    ${CMAKE_CURRENT_SOURCE_DIR}/include)

# set_target_properties(eosio.token
#    PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

# configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/eosio.token.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/eosio.token.contracts.md @ONLY )

# target_compile_options( eosio.token PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
