add_library(wasm2c_exports SHARED
    env.c
    env.h
)

copy_lib(wasm2c_exports)
