add_library(eosio_token_shared SHARED src/main.cpp ${CMAKE_CURRENT_BINARY_DIR}/eosio.token.obj)

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/eosio.token.obj
      COMMAND eosio-cpp -I${CMAKE_CURRENT_SOURCE_DIR}/include -c -g -O0 -fnative -o ${CMAKE_CURRENT_BINARY_DIR}/eosio.token.obj ${CMAKE_CURRENT_SOURCE_DIR}/src/eosio.token.cpp
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/eosio.token.cpp
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating ${CMAKE_CURRENT_SOURCE_DIR}/eosio.token.obj"
)

target_link_libraries(eosio_token_shared PRIVATE eosiolib_native)
target_compile_options(eosio_token_shared PRIVATE -fvisibility=hidden)

target_include_directories(eosio_token_shared
      PRIVATE ${CMAKE_SOURCE_DIR}/contracts
      PRIVATE ${CMAKE_BINARY_DIR}/contracts
      PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)
