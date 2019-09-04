add_library(eosio.system SHARED eosio.system.c)
target_link_libraries(eosio.system PRIVATE eosiolib_native vm_api4c)
target_include_directories(eosio.system PRIVATE ${Boost_INCLUDE_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/libraries/vm/vm_api4c
)
target_compile_options(eosio.system PRIVATE -fvisibility=hidden)

