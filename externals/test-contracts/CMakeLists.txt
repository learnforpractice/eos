find_package( Gperftools QUIET )

if( GPERFTOOLS_FOUND )
   message( STATUS "Found gperftools; compiling tests with TCMalloc" )
   list( APPEND PLATFORM_SPECIFIC_LIBS tcmalloc )
endif()

find_package(LLVM 4.0 REQUIRED CONFIG)

link_directories(${LLVM_LIBRARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/contracts.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/contracts.hpp ESCAPE_QUOTES)

### BUILD UNIT TEST EXECUTABLE ###
file(GLOB UNIT_TESTS "*.cpp") # find all unit test suites
add_executable( eosio_contracts_test ${UNIT_TESTS}) # build unit tests as one executable

target_link_libraries( eosio_contracts_test eosio_chain chainbase eosio_testing fc appbase ${PLATFORM_SPECIFIC_LIBS} )

target_compile_options(eosio_contracts_test PUBLIC -DDISABLE_EOSLIB_SERIALIZE)
target_include_directories( eosio_contracts_test PUBLIC
                            ${CMAKE_SOURCE_DIR}/libraries/testing/include
                            ${CMAKE_SOURCE_DIR}/test-contracts
                            ${CMAKE_BINARY_DIR}/contracts
                            ${CMAKE_CURRENT_SOURCE_DIR}/contracts
                            ${CMAKE_CURRENT_BINARY_DIR}/contracts
                            PRIVATE ${CMAKE_SOURCE_DIR}/contracts/libpython37/python37
                            PRIVATE ${CMAKE_SOURCE_DIR}/contracts/libpython37/python37/Include
                            ${CMAKE_CURRENT_BINARY_DIR}/include )
