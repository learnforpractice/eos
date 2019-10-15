if( EOSIO_COMPILE_TEST_CONTRACTS )

   add_contract( mi_test mi_test main.cpp 
      ${CMAKE_SOURCE_DIR}/../../contracts/mi/mi.cpp
   )
   
   target_include_directories(mi_test PRIVATE 
      PRIVATE ${CMAKE_SOURCE_DIR}/../../contracts/mi
   )

else()
   # configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/noop.wasm ${CMAKE_CURRENT_BINARY_DIR}/noop.wasm COPYONLY )
   # configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/noop.abi  ${CMAKE_CURRENT_BINARY_DIR}/noop.abi  COPYONLY )
endif()
