CC=clang CXX=clang++ python3.7 setup.py sdist bdist_wheel
#CC=clang CXX=clang++ python3.7 setup.py sdist bdist_wheel  -- -DCMAKE_TOOLCHAIN_FILE=$(pwd)/cmake/polly/clang-fpic.cmake -- -j7
#install_name_tool -add_rpath  /Users/newworld/dev/uuos2/build/programs/nodeos ./_skbuild/macosx-10.9-x86_64-3.7/cmake-install/uuos/_uuos.cpython-37m-darwin.so
#cp ./_skbuild/macosx-10.9-x86_64-3.7/cmake-install/uuos/_uuos.cpython-37m-darwin.so .
cp ./_skbuild/linux-x86_64-3.7/cmake-build/src/_uuos.cpython-37m-x86_64-linux-gnu.so .
