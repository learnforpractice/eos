
add_library( frozen
          STATIC
          ./frozen.c
)
set(PYTHON_FLAGS -DNDEBUG -DPYTHON_SS -DPy_BUILD_CORE -DPREFIX="" -DEXEC_PREFIX="" -DVERSION="3.7" -DVPATH="" -DPYTHONPATH="")
target_compile_options(frozen PRIVATE $<$<COMPILE_LANGUAGE:C>:${PYTHON_FLAGS} -g -msoft-float -m64 -fwrapv -Wall -Wstrict-prototypes -std=c99 -Wextra -Wno-unused-result -Wno-unused-parameter -Wno-missing-field-initializers> )
target_include_directories( frozen
                            PRIVATE ${CMAKE_SOURCE_DIR}/contracts/libpython37/python37
                            PRIVATE ${CMAKE_SOURCE_DIR}/contracts/libpython37/python37/Include
                        )