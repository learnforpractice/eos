file(GLOB HEADERS "include/eosio/chain/*.hpp")

add_library( vm_manager
              STATIC
              vm_manager.cpp
             )

target_link_libraries( vm_manager PRIVATE fc vm_micropython vm_lua)

target_include_directories( vm_manager
                           PRIVATE ${CMAKE_SOURCE_DIR}/libraries/chain
                           PRIVATE ${CMAKE_SOURCE_DIR}/libraries/chain/include
                           PRIVATE ${CMAKE_SOURCE_DIR}/libraries/chainbase/include
                           PRIVATE ${CMAKE_SOURCE_DIR}/libraries/wasm-jit/Include
                           PRIVATE ${CMAKE_SOURCE_DIR}/contracts
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
                        )

set_target_properties( vm_manager PROPERTIES PUBLIC_HEADER "${HEADERS}" )
install( TARGETS vm_manager 
   RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
   LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
   PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/eosio/chain
)
