CC=clang CXX=clang++ python3.7 setup.py sdist bdist_wheel --plat-name macosx-10.9-x86_64
#install_name_tool -add_rpath  /Users/newworld/dev/uuos2/build/programs/nodeos ./_skbuild/macosx-10.9-x86_64-3.7/cmake-install/uuos/_uuos.cpython-37m-darwin.so
#cp ./_skbuild/macosx-10.9-x86_64-3.7/cmake-install/uuos/_uuos.cpython-37m-darwin.so .

cp ./_skbuild/macosx-10.9-x86_64-3.7/cmake-install/uuos/_uuos.cpython-37m-darwin.so .
