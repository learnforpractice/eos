message(STATUS "Using custom FindBoost.cmake")
set(Boost_FOUND TRUE)
set(Boost_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libraries/boost_1_67_0/includes")
set(Boost_LIBRARIES boost_1_67_0)
