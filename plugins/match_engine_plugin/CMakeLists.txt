file(GLOB HEADERS "include/eosio/match_engine_plugin/*.hpp")
add_library( match_engine_plugin
             match_engine_plugin.cpp
             ${HEADERS} )

add_dependencies(match_engine_plugin eosio_chain)

target_link_libraries( match_engine_plugin chain_plugin history_plugin eosio_chain appbase fc )
target_include_directories( match_engine_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
target_include_directories( match_engine_plugin PUBLIC ${CMAKE_BINARY_DIR}/contracts /usr/local/include/librdkafka)
