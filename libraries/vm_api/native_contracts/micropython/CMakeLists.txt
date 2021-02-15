add_library(micropython_wasm2c SHARED
    micropython_eosio.c
)

if (APPLE)
      set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
      set(micropython_LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
      set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
      set(micropython_LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)

target_include_directories( micropython_wasm2c
    ${CMAKE_SOURCE_DIR}/libraries/vm_api/vm_api4c
)

target_compile_definitions(micropython_wasm2c PRIVATE EOSIO_NATIVE)

target_compile_options(micropython_wasm2c PRIVATE -Wno-attributes)

set_target_properties(micropython_wasm2c  PROPERTIES LINK_FLAGS "${micropython_LINK_FLAGS}")
target_link_libraries(micropython_wasm2c PRIVATE vm_api4c)
