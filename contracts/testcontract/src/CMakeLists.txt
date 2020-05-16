set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package( eosio.cdt REQUIRED )

add_contract(test test main.cpp)

#set_property(TARGET test APPEND_STRING PROPERTY LINK_FLAGS "-stack-size=18432")

set(WASM_LINK_FLAGS
    --gc-sections
    # --strip-all
    --merge-data-segments
    -e apply
    --only-export *:function
    -lc++
    -lc
    -leosio
    -leosio_dsm
    -mllvm
    -use-cfl-aa-in-codegen=both
    -L${EOSIO_CDT_ROOT}/opt/eosio.cdt/bin/../lib
    -stack-first
   #  --lto-O3
    -zstack-size=18432 #36864 #8192 #9808
    --allow-undefined-file=${EOSIO_CDT_ROOT}/opt/eosio.cdt/bin/../eosio.imports
)

set(WASM_LINKER "${EOSIO_CDT_ROOT}/opt/eosio.cdt/bin/wasm-ld")
set(CMAKE_C_LINK_EXECUTABLE "${WASM_LINKER} <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_CXX_LINK_EXECUTABLE "${WASM_LINKER} <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

target_link_options(test PRIVATE ${WASM_LINK_FLAGS})

macro( pp_wasm file )
   add_custom_command( TARGET ${file} POST_BUILD COMMAND eosio-pp -o ${CMAKE_CURRENT_BINARY_DIR}/${file}.wasm  ${CMAKE_CURRENT_BINARY_DIR}/${file}.wasm )
endmacro( pp_wasm )

macro( backup_wasm file )
   add_custom_command( TARGET ${file} POST_BUILD COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/${file}.wasm  ${CMAKE_CURRENT_BINARY_DIR}/${file}.wasm.origin )
endmacro( backup_wasm )

# backup_wasm(test)
# pp_wasm(test)
