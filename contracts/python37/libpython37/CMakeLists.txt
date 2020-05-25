configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../../../libraries/vm/vm_python/python_vm_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/python_vm_config.h)

SET(SRC_FILES
./python37/Modules/arraymodule.c
./python37/Modules/mathmodule.c
./python37/Modules/cmathmodule.c
./python37/Modules/_bisectmodule.c
#./python37/Modules/_datetimemodule.c
./python37/Modules/_json.c
./python37/Modules/_pickle.c
./python37/Modules/binascii.c
./python37/Modules/sha1module.c
./python37/Modules/sha256module.c
./python37/Modules/sha512module.c
./python37/Modules/_sha3/sha3module.c
./python37/Modules/md5module.c

./python37/Modules/_struct.c
./python37/Modules/_threadmodule.c
./python37/Modules/_abc.c
./python37/Modules/_codecsmodule.c
./python37/Modules/_collectionsmodule.c
./python37/Modules/_functoolsmodule.c

./python37/Modules/_io/_iomodule.c
./python37/Modules/_io/bufferedio.c
./python37/Modules/_io/bytesio.c
./python37/Modules/_io/fileio.c
./python37/Modules/_io/iobase.c
./python37/Modules/_io/stringio.c
./python37/Modules/_io/textio.c

./python37/Modules/_localemodule.c
./python37/Modules/_math.c
./python37/Modules/_operator.c
./python37/Modules/_sre.c
    #         Modules/_ssl.c
#    Modules/_stat.c
    #         Modules/_threadmodule.c
#./python37/Modules/_tracemalloc-ss.c
./python37/Modules/_tracemalloc.c
./python37/Modules/_weakref.c
./python37/Modules/atexitmodule.c
./python37/Modules/errnomodule.c
./python37/Modules/faulthandler.c
./python37/Modules/gcmodule.c
./python37/Modules/getbuildinfo.c
./python37/Modules/getpath.c
./python37/Modules/hashtable.c
./python37/Modules/itertoolsmodule.c
./python37/Modules/main.c
#./python37/Modules/posixmodule.c
./python37/Modules/pwdmodule.c
#./python37/Modules/signalmodule.c
./python37/Modules/symtablemodule.c
#./python37/Modules/timemodule.c
./python37/Modules/xxsubtype.c
./python37/Modules/zipimport.c
#./python37/Modules/readline.c

./python37/Objects/call.c
./python37/Objects/abstract.c
./python37/Objects/accu.c
./python37/Objects/boolobject.c
./python37/Objects/bytearrayobject.c
./python37/Objects/bytes_methods.c
./python37/Objects/bytesobject.c
./python37/Objects/capsule.c
./python37/Objects/cellobject.c
./python37/Objects/classobject.c
./python37/Objects/codeobject.c
./python37/Objects/complexobject.c
./python37/Objects/descrobject.c
./python37/Objects/dictobject.c
./python37/Objects/enumobject.c
./python37/Objects/exceptions.c
./python37/Objects/fileobject.c
./python37/Objects/floatobject.c
#./python37/Objects/softfloatobject.c
./python37/Objects/frameobject.c
./python37/Objects/funcobject.c
./python37/Objects/genobject.c
./python37/Objects/iterobject.c
./python37/Objects/listobject.c
./python37/Objects/longobject.c
./python37/Objects/memoryobject.c
./python37/Objects/methodobject.c
./python37/Objects/moduleobject.c
./python37/Objects/namespaceobject.c
./python37/Objects/object.c
./python37/Objects/obmalloc.c
./python37/Objects/odictobject.c
./python37/Objects/rangeobject.c
./python37/Objects/setobject.c
./python37/Objects/sliceobject.c
./python37/Objects/structseq.c
./python37/Objects/tupleobject.c
./python37/Objects/typeobject.c
./python37/Objects/unicodectype.c
./python37/Objects/unicodeobject.c
./python37/Objects/weakrefobject.c

./python37/Parser/acceler.c
./python37/Parser/bitset.c
./python37/Parser/firstsets.c
./python37/Parser/grammar.c
./python37/Parser/grammar1.c
./python37/Parser/listnode.c
./python37/Parser/metagrammar.c
./python37/Parser/myreadline.c
./python37/Parser/node.c
./python37/Parser/parser.c
./python37/Parser/parsetok.c
./python37/Parser/pgen.c
./python37/Parser/tokenizer.c

    #         Programs/_freeze_importlib.c
    #         Programs/_testembed.c
    #         Programs/python.c

./python37/Python/ast_unparse.c
./python37/Python/bootstrap_hash.c
./python37/Python/hamt.c
./python37/Python/ast_opt.c
./python37/Python/pathconfig.c
./python37/Python/context.c
./python37/Python/_warnings.c
./python37/Python/asdl.c
./python37/Python/ast.c
./python37/Python/bltinmodule.c
./python37/Python/ceval.c
./python37/Python/codecs.c
./python37/Python/compile.c
./python37/Python/dtoa.c
#./python37/Python/dynamic_annotations.c
    #         Python/dynload_shlib.c
./python37/Python/errors.c
./python37/Python/fileutils.c
./python37/Python/formatter_unicode.c
./python37/Python/frozen.c
./python37/Python/frozenmain.c
./python37/Python/future.c
./python37/Python/getargs.c
./python37/Python/getcompiler.c
./python37/Python/getcopyright.c
./python37/Python/getopt.c
./python37/Python/getplatform.c
./python37/Python/getversion.c
./python37/Python/graminit.c
./python37/Python/import.c
    #         Python/importdl.c
./python37/Python/marshal.c
./python37/Python/modsupport.c
./python37/Python/mysnprintf.c
./python37/Python/mystrtoul.c
./python37/Python/peephole.c
./python37/Python/pyarena.c
./python37/Python/pyctype.c
./python37/Python/pyfpe.c
./python37/Python/pyhash.c
./python37/Python/pylifecycle.c
./python37/Python/pymath.c
./python37/Python/pystate.c
./python37/Python/pystrcmp.c
./python37/Python/pystrhex.c
./python37/Python/pystrtod.c
./python37/Python/Python-ast.c
./python37/Python/pythonrun.c
#./python37/Python/pytime.c
    #         Python/random.c
./python37/Python/structmember.c
./python37/Python/symtable.c
./python37/Python/sysmodule.c
./python37/Python/thread.c
./python37/Python/traceback.c
./python37/Modules/_blake2/blake2module.c
./python37/Modules/_blake2/blake2b_impl.c
./python37/Modules/_blake2/blake2s_impl.c
./pythoninit.c
#./pythonvm.c
./xxhash.c
./action.cpp
./eosiolib.cpp
./io.c

./python37/Modules/config-wasm.c
./eosiolib_wrapper/db.c
./eosiolib_wrapper/float128.c
./eosiolib_wrapper/utility.c
#./python37/pyconfig.h

# ./malloc/aligned_alloc.c
# ./malloc/memalign.c
# ./malloc/posix_memalign.c
${CMAKE_SOURCE_DIR}/mi/mi.cpp
./eosiolib_wrapper/mi.c
./fenv.c
./dummy.c
./dummy_libc.c
./dummy_pthread.c
)

set(PYTHON_FLAGS -D__WASM -DNDEBUG -DPYTHON_SS -DPy_BUILD_CORE -DPREFIX="" -DEXEC_PREFIX="" -DVERSION="3.7" -DVPATH="" -DPYTHONPATH="")

set(PYTHON_FLAGS ${PYTHON_FLAGS} -DPYTHON_VM_STACK_SIZE=${PYTHON_VM_STACK_SIZE})
set(PYTHON_FLAGS ${PYTHON_FLAGS} -DPYTHON_VM_MAX_MEMORY_SIZE=${PYTHON_VM_MAX_MEMORY_SIZE})
set(PYTHON_FLAGS ${PYTHON_FLAGS} -DPYTHON_VM_MAX_TEMP_MEMORY_SIZE=${PYTHON_VM_MAX_TEMP_MEMORY_SIZE})

set(PYTHON_INC_FOLDERS
    ${CMAKE_SOURCE_DIR}/include/libcxx
    ${CMAKE_SOURCE_DIR}/include/libc
    ${CMAKE_SOURCE_DIR}/libpython37/libc
    ${CMAKE_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/../../externals/magic_get/include
    ${Boost_INCLUDE_DIR}
)

set(CMAKE_C_COMPILER "${EOSIO_CDT_ROOT}/opt/eosio.cdt/bin/clang-7")
set(CMAKE_C_FLAGS "-O3 --target=wasm32 -ffreestanding -nostdlib -fno-builtin -fno-threadsafe-statics -D__WASM -DBOOST_DISABLE_ASSERTS -DBOOST_EXCEPTION_DISABLE -Xclang -load -Xclang ${EOSIO_CDT_ROOT}/opt/eosio.cdt/bin/LLVMEosioApply.dylib -fplugin=${EOSIO_CDT_ROOT}/opt/eosio.cdt/bin/eosio_plugin.dylib -mllvm -use-cfl-aa-in-codegen=both --sysroot=${EOSIO_CDT_ROOT}/opt/eosio.cdt/bin/../")

set(CMAKE_AR "/usr/bin/ar")
set(CMAKE_AR "${EOSIO_CDT_ROOT}/opt/eosio.cdt/bin/eosio-ar")
add_library(python37
  "${SRC_FILES}" 
)

#./python37/Modules/config-wasm.c ./eosiolib_wrapper/db.c ./eosiolib_wrapper/utility.c

target_compile_options(python37 PRIVATE ${PYTHON_FLAGS}) # -Wno-visibility)

target_include_directories(python37
   PRIVATE
   "${PYTHON_INC_FOLDERS}"
   ${CMAKE_CURRENT_SOURCE_DIR}/python37
   ${CMAKE_CURRENT_SOURCE_DIR}/python37/Include
   ${CMAKE_CURRENT_BINARY_DIR}
   ${CMAKE_SOURCE_DIR}/mi
)

# add_library( frozen
#           STATIC
#           ./frozen.c
# )
# set(PYTHON_FLAGS -DNDEBUG -DPYTHON_SS -DPy_BUILD_CORE -DPREFIX="" -DEXEC_PREFIX="" -DVERSION="3.7" -DVPATH="" -DPYTHONPATH="")
# target_compile_options(frozen PRIVATE $<$<COMPILE_LANGUAGE:C>:${PYTHON_FLAGS} -g -msoft-float -m64 -fwrapv -Wall -Wstrict-prototypes -std=c99 -Wextra -Wno-unused-result -Wno-unused-parameter -Wno-missing-field-initializers> )
# target_include_directories( frozen
#                             PRIVATE ${CMAKE_SOURCE_DIR}/contracts/libpython37/python37
#                             PRIVATE ${CMAKE_SOURCE_DIR}/contracts/libpython37/python37/Include
#                         )


# add_library( python3-ss
#           STATIC
#           ${SRC_FILES}
#           ./python37/Modules/config.c
#           ./frozen.c
# )

# set(PYTHON_FLAGS -DNDEBUG -DPYTHON_SS -DPy_BUILD_CORE -DPREFIX="" -DEXEC_PREFIX="" -DVERSION="3.7" -DVPATH="" -DPYTHONPATH="")

# #target_compile_options(python3-ss PRIVATE $<$<COMPILE_LANGUAGE:C>:${PYTHON_FLAGS} -g -msoft-float -m64 -fwrapv -O3 -Wall -Wstrict-prototypes -std=c99 -Wextra -Wno-unused-result -Wno-unused-parameter -Wno-missing-field-initializers> )
# target_compile_options(python3-ss PRIVATE $<$<COMPILE_LANGUAGE:C>:${PYTHON_FLAGS} -g -msoft-float -m64 -fwrapv -Wall -Wstrict-prototypes -std=c99 -Wextra -Wno-unused-result -Wno-unused-parameter -Wno-missing-field-initializers> )

# target_include_directories( python3-ss
#                             PRIVATE ${CMAKE_SOURCE_DIR}/contracts/libpython37/python37
#                             PRIVATE ${CMAKE_SOURCE_DIR}/contracts/libpython37/python37/Include
#                         )
# if (APPLE)
#     target_link_libraries( python3-ss PUBLIC ${OPENSSL_LIBRARIES} intl softfloat)
# else()
#     target_link_libraries( python3-ss PUBLIC ${OPENSSL_LIBRARIES} softfloat)
# endif()
