set(PYTHON_INC_FOLDERS 
${CMAKE_SOURCE_DIR}/emscripten/system/include/libcxx
${CMAKE_SOURCE_DIR}/emscripten/system/lib/libcxxabi/include
${CMAKE_SOURCE_DIR}/emscripten/system/include/compat
${CMAKE_SOURCE_DIR}/emscripten/system/include
${CMAKE_SOURCE_DIR}/emscripten/system/include/libc
${CMAKE_SOURCE_DIR}/emscripten/system/lib/libc/musl/arch/emscripten
${CMAKE_SOURCE_DIR}/emscripten/system/local/include
${CMAKE_SOURCE_DIR}/emscripten/system/include/SDL
${CMAKE_SOURCE_DIR}/emscripten/system/lib/libc/musl/src/internal

${CMAKE_SOURCE_DIR}/..
${CMAKE_SOURCE_DIR}/../../externals/magic_get/include
${Boost_INCLUDE_DIR}
)

set(WASM_LINKER "${EOSIO_CDT_ROOT}/opt/eosio.cdt/bin/wasm-ld")

set(CMAKE_C_LINK_EXECUTABLE "${WASM_LINKER} <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_CXX_LINK_EXECUTABLE "${WASM_LINKER} <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# --gc-sections
# --strip-all
# --merge-data-segments
# -e
# apply
# --only-export
# apply:function
# -lc++
# -lc
# -leosio
# -leosio_dsm
# -mllvm
# -use-cfl-aa-in-codegen=both
# -L${EOSIO_CDT_ROOT}/opt/eosio.cdt/bin/../lib
# -stack-first
# --lto-O3
# -zstack-size=16384 #8192 #9808
# --allow-undefined-file=${EOSIO_CDT_ROOT}/opt/eosio.cdt/bin/../eosio.imports

set(WASM_LINK_FLAGS
--gc-sections
#--strip-all
--merge-data-segments
-e apply
--only-export apply:function
-lc++ 
-lc
#-leosio
-mllvm
-use-cfl-aa-in-codegen=both
-L${EOSIO_CDT_ROOT}/opt/eosio.cdt/bin/../lib
-L../libpython37
-stack-first
--lto-O3
-zstack-size=${PYTHON_VM_STACK_SIZE} #20480
--allow-undefined-file=${EOSIO_CDT_ROOT}/opt/eosio.cdt/bin/../eosio.imports)

add_executable(pythonvm pythonvm.c main.cpp)

#./python37/Modules/config-wasm.c ./eosiolib_wrapper/db.c ./eosiolib_wrapper/utility.c

target_compile_options(pythonvm PRIVATE ${PYTHON_FLAGS}) # -Wno-visibility)

target_link_options(pythonvm PRIVATE ${WASM_LINK_FLAGS})

target_link_libraries( pythonvm PRIVATE python37) # libc_emscripten )

target_include_directories(pythonvm
   PRIVATE
   "${PYTHON_INC_FOLDERS}"
   ${CMAKE_SOURCE_DIR}/libpython37/python37
   ${CMAKE_SOURCE_DIR}/libpython37/python37/Include
)

# ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/pythonvm.wasm.c
#     COMMAND python3 ${CMAKE_CURRENT_BINARY_DIR}/gen.py
#     DEPENDS pythonvm ${CMAKE_CURRENT_BINARY_DIR}/pythonvm.wasm
#     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#     COMMENT "Generating ${CMAKE_CURRENT_SOURCE_DIR}/pythonvm.wasm.c"
# )

# add_custom_target (gen_pythonvm_wasm ALL
#    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pythonvm.wasm.c
# )

