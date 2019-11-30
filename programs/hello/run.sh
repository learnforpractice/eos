cp ./_skbuild/macosx-10.9-x86_64-3.7/cmake-build/hello/_hello.cpython-37m-darwin.so .
install_name_tool -add_rpath  /Users/newworld/dev/uuos2/build/programs/nodeos _hello.cpython-37m-darwin.so
python3 main.py

