add_library(eosio.system SHARED
    main.cpp
    wasm-rt-impl.c
    eosio.system.c38f27660719f14b488b81f854c89c7f904191ee744601aea9a5180d78702e87.c
)

target_link_libraries(eosio.system PRIVATE eosiolib_native vm_api4c)
target_include_directories(eosio.system PRIVATE ${Boost_INCLUDE_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/libraries/vm/vm_api4c
)

if (APPLE)
    set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
    set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)
set_target_properties(eosio.system  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")


#target_compile_options(eosio.system PRIVATE -fvisibility=hidden)

