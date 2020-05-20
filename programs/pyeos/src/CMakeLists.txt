

add_cython_target(_uuos CXX)
add_library(_uuos MODULE ${_uuos} uuos.cpp
)

python_extension_module(_uuos)

target_include_directories(_uuos PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../libraries/chain_api
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../libraries/chain/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../libraries/fc/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../externals/boost/includes
)

set_property(TARGET _uuos PROPERTY CXX_STANDARD 17)

install(TARGETS _uuos LIBRARY DESTINATION uuos)
