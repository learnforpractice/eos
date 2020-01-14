

add_cython_target(_uuos CXX)
add_library(_uuos MODULE ${_uuos} uuos.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../contracts/python37/libpython37/eosiolib_wrapper/db.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../contracts/python37/libpython37/eosiolib_wrapper/utility.c
)

python_extension_module(_uuos)

target_link_libraries(_uuos ${CMAKE_CURRENT_SOURCE_DIR}/../../../build/programs/nodeos/libpyuuos${CMAKE_SHARED_LIBRARY_SUFFIX})

target_include_directories(_uuos PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../contracts
)

install(TARGETS _uuos LIBRARY DESTINATION uuos)
