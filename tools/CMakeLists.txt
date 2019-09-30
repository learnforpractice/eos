configure_file(${CMAKE_CURRENT_SOURCE_DIR}/llvm-gcov.sh ${CMAKE_CURRENT_BINARY_DIR}/llvm-gcov.sh COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ctestwrapper.sh ${CMAKE_CURRENT_BINARY_DIR}/ctestwrapper.sh COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/validate_reflection.py ${CMAKE_CURRENT_BINARY_DIR}/validate_reflection.py COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wasm2c.py.in ${CMAKE_CURRENT_BINARY_DIR}/wasm2c.py)

add_subdirectory(account_snapshot)
