declare ARCH=$( uname )

if [[ $ARCH == "Linux" ]]; then
    python3.7 -m pip uninstall uuosio -y;python3.7 -m pip install build/programs/uuos/extlib/dist/uuosio-0.1.0-cp37-cp37m-linux_x86_64.whl
else
    python3.7 -m pip uninstall uuosio -y;python3.7 -m pip install build/programs/uuos/extlib/dist/uuosio-0.1.0-cp37-cp37m-macosx_10_9_x86_64.whl
fi