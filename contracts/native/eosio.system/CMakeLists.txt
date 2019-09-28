add_library(eosio.system.native SHARED 
    ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.system/src/eosio.system.cpp
    ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.system/include/eosio.system/eosio.system.hpp
    ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.system/src/eosio.system.cpp
    ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.system/src/delegate_bandwidth.cpp
    ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.system/src/exchange_state.cpp
    ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.system/src/native.cpp
    ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.system/src/producer_pay.cpp
    ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.system/src/rex.cpp
    ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.system/src/voting.cpp
    ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.system/src/activate_accounts.cpp
)

target_link_libraries( eosio.system.native PRIVATE eosiolib.native eosiolib_native )

target_include_directories( eosio.system.native
                            PRIVATE ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.system/include
                            PRIVATE ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.token/include
                            PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include"
#                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/libcxx"
#                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/libc"
                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/eosiolib/core"
                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/eosiolib/capi"
                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/eosiolib/contracts"
                        )
target_compile_options( eosio.system.native PRIVATE -DEOSIO_NATIVE -Wno-unknown-attributes )

install(TARGETS eosio.system.native
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

copy_lib(eosio.system.native)
