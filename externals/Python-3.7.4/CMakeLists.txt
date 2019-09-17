
SET(SRC_FILES
./Python/pyarena.c
./Python/hamt.c
./Python/bltinmodule.c
./Python/marshal.c
./Python/asdl.c
./Python/pystrtod.c
./Python/importdl.c
./Python/future.c
./Python/getcompiler.c
./Python/pyhash.c
./Python/fileutils.c
./Python/_warnings.c
./Python/peephole.c
./Python/getcopyright.c
./Python/mysnprintf.c
./Python/pystate.c
./Python/ceval.c
./Python/getargs.c
./Python/mystrtoul.c
./Python/bootstrap_hash.c
./Python/frozenmain.c
./Python/structmember.c
./Python/pyctype.c
./Python/modsupport.c
./Python/context.c
./Python/pylifecycle.c
./Python/errors.c
./Python/getversion.c
./Python/pystrcmp.c
./Python/pymath.c
./Python/ast_unparse.c
./Python/getopt.c
./Python/pyfpe.c
./Python/symtable.c
./Python/ast.c
./Python/compile.c
./Python/thread.c
./Python/codecs.c
./Python/formatter_unicode.c
./Python/pathconfig.c
./Python/dtoa.c
./Python/pystrhex.c
#./Python/dynload_shlib.c
./Python/import.c
./Python/pytime.c
./Python/frozen.c
./Python/Python-ast.c
./Python/traceback.c
./Python/pythonrun.c
./Python/ast_opt.c
./Python/sysmodule.c
./Python/getplatform.c
./Python/graminit.c
./Python/dynamic_annotations.c
./Objects/classobject.c
./Objects/cellobject.c
./Objects/sliceobject.c
./Objects/tupleobject.c
./Objects/iterobject.c
./Objects/rangeobject.c
./Objects/accu.c
./Objects/dictobject.c
./Objects/namespaceobject.c
./Objects/floatobject.c
./Objects/frameobject.c
./Objects/abstract.c
./Objects/exceptions.c
./Objects/genobject.c
./Objects/methodobject.c
./Objects/boolobject.c
./Objects/fileobject.c
./Objects/call.c
./Objects/weakrefobject.c
./Objects/moduleobject.c
./Objects/enumobject.c
./Objects/odictobject.c
./Objects/setobject.c
./Objects/complexobject.c
./Objects/typeobject.c
./Objects/capsule.c
./Objects/object.c
./Objects/structseq.c
./Objects/bytearrayobject.c
./Objects/codeobject.c
./Objects/unicodectype.c
./Objects/unicodeobject.c
./Objects/funcobject.c
./Objects/obmalloc.c
./Objects/descrobject.c
./Objects/memoryobject.c
./Objects/longobject.c
./Objects/bytesobject.c
./Objects/listobject.c
./Objects/bytes_methods.c
./Parser/myreadline.c
./Parser/bitset.c
./Parser/grammar1.c
./Parser/pgen.c
./Parser/parsetok.c
./Parser/metagrammar.c
./Parser/parser.c
./Parser/node.c
./Parser/tokenizer.c
./Parser/acceler.c
./Parser/listnode.c
./Parser/firstsets.c
./Parser/grammar.c
./Programs/_testembed.c
./Programs/python.c
./Modules/main.c

./Modules/_io/_iomodule.c
./Modules/_io/bufferedio.c
./Modules/_io/bytesio.c
./Modules/_io/fileio.c
./Modules/_io/iobase.c
./Modules/_io/stringio.c
./Modules/_io/textio.c

./Modules/_collectionsmodule.c
./Modules/_weakref.c
./Modules/config.c
./Modules/faulthandler.c
./Modules/_codecsmodule.c
./Modules/xxsubtype.c
./Modules/hashtable.c
./Modules/_operator.c
./Modules/symtablemodule.c
./Modules/posixmodule.c
./Modules/_math.c
./Modules/getpath.c
./Modules/pwdmodule.c
./Modules/_sre.c
./Modules/_stat.c
./Modules/errnomodule.c
./Modules/atexitmodule.c
./Modules/itertoolsmodule.c
./Modules/timemodule.c
./Modules/_abc.c
./Modules/_functoolsmodule.c
./Modules/getbuildinfo.c
./Modules/_threadmodule.c
./Modules/gcmodule.c
./Modules/signalmodule.c
./Modules/_localemodule.c
./Modules/zipimport.c
./Modules/_tracemalloc.c
)

set(PYTHON_FLAGS -D__WASM -DNDEBUG -DPYTHON_SS -DPy_BUILD_CORE -DPREFIX="" -DEXEC_PREFIX="" -DVERSION="3.5" -DVPATH="" -DPYTHONPATH="")

set(PYTHON_INC_FOLDERS 
""
)

add_library( python3-ss
          STATIC
          ${SRC_FILES}
          ./Modules/config.c
)

set(PYTHON_FLAGS -DNDEBUG -DPYTHON_SS -DPy_BUILD_CORE -DPREFIX="" -DEXEC_PREFIX="" -DVERSION="3.7" -DVPATH="" -DPYTHONPATH="")

#target_compile_options(python3-ss PRIVATE $<$<COMPILE_LANGUAGE:C>:${PYTHON_FLAGS} -g -msoft-float -m64 -fwrapv -O3 -Wall -Wstrict-prototypes -std=c99 -Wextra -Wno-unused-result -Wno-unused-parameter -Wno-missing-field-initializers> )
target_compile_options(python3-ss PRIVATE $<$<COMPILE_LANGUAGE:C>:${PYTHON_FLAGS} -g -msoft-float -m64 -fwrapv -Wall -Wstrict-prototypes -std=c99 -Wextra -Wno-unused-result -Wno-unused-parameter -Wno-missing-field-initializers> )

target_include_directories( python3-ss
                            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include
                        )
if (APPLE)
    target_link_libraries( python3-ss PUBLIC ${OPENSSL_LIBRARIES} intl softfloat)
else()
    target_link_libraries( python3-ss PUBLIC ${OPENSSL_LIBRARIES} softfloat)
endif()
