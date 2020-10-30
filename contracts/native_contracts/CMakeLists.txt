# add_library(native_contracts SHARED 
#     main.cpp
# )

# target_link_libraries( native_contracts PRIVATE eosiolib.native vm_api )

# target_include_directories( native_contracts
#                             PRIVATE ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.system/include
#                             PRIVATE ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.token/include
#                             PRIVATE ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts

#                             PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include"
# #                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/libcxx"
# #                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/libc"
#                               PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/eosiolib/core"
#                               PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/eosiolib/capi"
#                               PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/eosiolib/contracts"
#                         )
# target_compile_options( native_contracts PRIVATE -DEOSIO_NATIVE -Wno-unknown-attributes )

# install(TARGETS native_contracts
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )

# copy_lib(native_contracts)

# set(EOSIO_WASM_OLD_BEHAVIOR "Off")
# find_package(eosio.cdt REQUIRED)

add_library(native_contracts
                  main.cpp
                  crypto.cpp
                  eosiolib.cpp
                    # /usr/local/Cellar//eosio.cdt/1.7.0/opt/eosio.cdt/lib/libnative.a
                    # /usr/local/Cellar//eosio.cdt/1.7.0/opt/eosio.cdt/lib/libnative_eosio.a
)

target_include_directories( native_contracts
                            PRIVATE ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.system/include
                            PRIVATE ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/eosio.token/include
                            PRIVATE ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts
                            PRIVATE ${CMAKE_SOURCE_DIR}/externals/eosio.contracts/contracts/include/eosio

                            PRIVATE "${CMAKE_SOURCE_DIR}/contracts"
                            PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include"
#                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/libcxx"
#                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/libc"
                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/eosiolib/core"
                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/eosiolib/capi"
                              PRIVATE "${CMAKE_SOURCE_DIR}/contracts/include/eosiolib/contracts"
                        )
target_compile_definitions(native_contracts PRIVATE EOSIO_NATIVE)
target_compile_options(native_contracts PRIVATE -Wno-unknown-attributes)
