file(GLOB HEADERS "include/eosio/chain_plugin/*.hpp")
add_executable(action_publisher main.cpp )

target_link_libraries( action_publisher chain_plugin eosio_chain appbase libzmq-static cppzmq-static)
target_include_directories( action_publisher PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/../chain_interface/include" "${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/appbase/include")


file(GLOB HEADERS "include/eosio/history_plugin/*.hpp")
add_library( history_plugin
             history_plugin.cpp
             ${HEADERS} )

target_link_libraries( history_plugin chain_plugin eosio_chain appbase )
target_include_directories( history_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

