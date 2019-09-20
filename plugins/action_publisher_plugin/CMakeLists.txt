add_executable(zmqtest main.cpp )
target_link_libraries( zmqtest libzmq-static cppzmq-static)
target_include_directories( zmqtest PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/../chain_interface/include" "${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/appbase/include")


file(GLOB HEADERS "include/eosio/action_publisher_plugin/*.hpp")
add_library(action_publisher_plugin action_publisher_plugin.cpp )

target_link_libraries( action_publisher_plugin chain_plugin http_plugin eosio_chain appbase libzmq-static cppzmq-static)
target_include_directories( action_publisher_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/../chain_interface/include" "${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/appbase/include")

