

add_cython_target(_hello CXX)
add_library(_hello MODULE ${_hello} hello.cpp)

python_extension_module(_hello)

target_link_libraries(_hello /Users/newworld/dev/uuos2/build/programs/nodeos/libpyuuos.dylib)

target_include_directories(_hello PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS _hello LIBRARY DESTINATION hello)
