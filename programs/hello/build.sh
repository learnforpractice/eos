cp ./_skbuild/macosx-10.9-x86_64-3.7/cmake-build/hello/_hello.cpython-37m-darwin.so .
install_name_tool -add_rpath  /Users/newworld/dev/uuos2/build/programs/nodeos _hello.cpython-37m-darwin.so
CC=clang CXX=clang++ python3 setup.py sdist bdist_wheel --plat-name macosx-10.9-x86_64

