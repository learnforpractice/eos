set(Sources WASMSerialization.cpp ${CMAKE_SOURCE_DIR}/libraries/chain/wasm_eosio_constraints.cpp)
set(PublicHeaders ${WAVM_INCLUDE_DIR}/WASM/WASM.h)
include_directories(${WAVM_INCLUDE_DIR}/WASM)

add_library(WASM STATIC ${Sources} ${PublicHeaders})
add_definitions(-DWEBASSEMBLY_API=DLL_EXPORT)
target_link_libraries(WASM Logging IR)

target_include_directories( WASM
                            PRIVATE "${CMAKE_SOURCE_DIR}/libraries/chain/include"
)

install(TARGETS WASM
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
