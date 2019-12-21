

add_cython_target(_uuos CXX)
add_library(_uuos MODULE ${_uuos} uuos.cpp)

python_extension_module(_uuos)

target_link_libraries(_uuos ${CMAKE_CURRENT_SOURCE_DIR}/../../../build/programs/nodeos/libpyuuos${CMAKE_SHARED_LIBRARY_SUFFIX})

target_include_directories(_uuos PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS _uuos LIBRARY DESTINATION uuos)
