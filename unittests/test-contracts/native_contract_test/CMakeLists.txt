if( EOSIO_COMPILE_TEST_CONTRACTS )
   add_contract( native_contract_test native_contract_test native_contract_test.cpp )
else()
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/native_contract_test.wasm ${CMAKE_CURRENT_BINARY_DIR}/native_contract_test.wasm COPYONLY )
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/native_contract_test.abi  ${CMAKE_CURRENT_BINARY_DIR}/native_contract_test.abi  COPYONLY )
endif()


# ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/native_contract_test.c
#     COMMAND python3 ${CMAKE_CURRENT_BINARY_DIR}/wasm2c.py native_contract_test native_contract_test.wasm
#     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/native_contract_test.wasm
#     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#     COMMENT "Generating ${CMAKE_CURRENT_SOURCE_DIR}/native_contract_test.c"
# )

# add_custom_target (native_wasm2c ALL
#    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/native_contract_test.c
# )
