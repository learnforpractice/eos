file(GLOB HEADERS "include/eosio/match_engine_api_plugin/*.hpp")
add_library( match_engine_api_plugin
             match_engine_api_plugin.cpp
             ${HEADERS} )

target_link_libraries( match_engine_api_plugin match_engine_plugin chain_plugin http_plugin appbase fc rdkafka)
target_include_directories( match_engine_api_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
