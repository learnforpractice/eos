add_library( chain_api SHARED
    chain_proxy.cpp
    uuos.cpp
)

if (APPLE)
else()
    target_compile_options(chain_api PRIVATE -fPIC)
endif()

if (APPLE)
      set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
      set(LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
      set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
      set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)

set_target_properties(chain_api  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")
