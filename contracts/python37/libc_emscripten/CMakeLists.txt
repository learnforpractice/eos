file(GLOB CRYPT_SOURCES  "musl/src/crypt/*.c")
file(GLOB CTYPE_SOURCES  "musl/src/ctype/*.c")
file(GLOB ENV_SOURCES  "musl/src/env/*.c")
file(GLOB ERRNO_SOURCES  "musl/src/errno/*.c")
#file(GLOB EXIT_SOURCES  "musl/src/exit/*.c")
#file(GLOB LOCALE_SOURCES "musl/src/locale/*.c")
file(GLOB MATH_SOURCES  "musl/src/math/*.c")
file(GLOB MBYTE_SOURCES  "musl/src/multibyte/*.c")
#file(GLOB MISC_SOURCES "musl/src/misc/*.c")
file(GLOB SEARCH_SOURCES "musl/src/search/*.c")
file(GLOB STDIO_SOURCES "musl/src/stdio/*.c")
file(GLOB STDLIB_SOURCES "musl/src/stdlib/*.c")
file(GLOB STRING_SOURCES "musl/src/string/*.c")
#file(GLOB TIME_SOURCES "musl/src/time/*.c")
#file(GLOB THREAD_SOURCES "musl/src/thread/*.c") #only for __lock __unlock
set(INTERNAL_SOURCES musl/src/internal/floatscan.c musl/src/internal/intscan.c musl/src/internal/shgetc.c musl/src/internal/libc.c)



set(PYTHON_INC_FOLDERS 
${CMAKE_SOURCE_DIR}/emscripten/system/include/libcxx
#${CMAKE_SOURCE_DIR}/contracts/emscripten/system/lib/libcxxabi/include
${CMAKE_SOURCE_DIR}/emscripten/system/include/compat
${CMAKE_SOURCE_DIR}/emscripten/system/include
${CMAKE_SOURCE_DIR}/emscripten/system/include/libc
${CMAKE_SOURCE_DIR}/emscripten/system/lib/libc/musl/arch/emscripten
#${CMAKE_SOURCE_DIR}/contracts/emscripten/system/local/include
${CMAKE_SOURCE_DIR}/emscripten/system/include/SDL
${CMAKE_SOURCE_DIR}/emscripten/system/lib/libc/musl/src/internal
)

set(CMAKE_C_COMPILER "/usr/local/Cellar/eosio.cdt/1.6.1/opt/eosio.cdt/bin/clang-7")
set(CMAKE_C_FLAGS " -O3 --target=wasm32 -ffreestanding -nostdlib -fno-builtin -fno-threadsafe-statics -DBOOST_DISABLE_ASSERTS -DBOOST_EXCEPTION_DISABLE -Xclang -load -Xclang /usr/local/Cellar/eosio.cdt/1.6.1/opt/eosio.cdt/bin/LLVMEosioApply.dylib -fplugin=/usr/local/Cellar/eosio.cdt/1.6.1/opt/eosio.cdt/bin/eosio_plugin.dylib -mllvm -use-cfl-aa-in-codegen=both --sysroot=/usr/local/Cellar/eosio.cdt/1.6.1/opt/eosio.cdt/bin/../")

#set(CMAKE_AR "/usr/bin/ar")
set(CMAKE_AR "/usr/local/Cellar/eosio.cdt//1.6.1/opt/eosio.cdt/bin/eosio-ar")
add_library(libc_emscripten ${CRYPT_SOURCES} ${CTYPE_SOURCES} ${ENV_SOURCES} ${ERRNO_SOURCES} ${EXIT_SOURCES} ${INTERNAL_SOURCES} ${MATH_SOURCES}
  ${MBYTE_SOURCES} ${MISC_SOURCES} ${SEARCH_SOURCES} ${STDIO_SOURCES} ${STDLIB_SOURCES} ${STRING_SOURCES} ${TIME_SOURCES} ${THREAD_SOURCES}
)
#${LOCALE_SOURCES}

target_compile_options(libc_emscripten PRIVATE ${PYTHON_FLAGS}) # -Wno-visibility)

target_include_directories(libc_emscripten
   PRIVATE
   "${PYTHON_INC_FOLDERS}"
)
