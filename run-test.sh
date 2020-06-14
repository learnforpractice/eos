if [[ $( uname ) == 'Darwin' ]]; then
    export PROJECT_DIR="/Users/newworld/dev/uuos3"
    export CHAIN_API_SHARED_LIB=$PROJECT_DIR/build//libraries/chain_api/libchain_api.dylib
    export UUOS_EXT_LIB=$PROJECT_DIR/build/programs/pyeos
    export PYTHONPATH=$PROJECT_DIR/programs/uuos:$UUOS_EXT_LIB

    export PYTHONHOME="/Users/newworld/opt/anaconda3"
    export PYTHON_SHARED_LIB_PATH="/Users/newworld/opt/anaconda3/lib/libpython3.7m.dylib"
else
    export PROJECT_DIR=$(pwd)
    export CHAIN_API_SHARED_LIB=$PROJECT_DIR/build/libraries/chain_api/libchain_api.so
    export UUOS_EXT_LIB=$PROJECT_DIR/build/programs/pyeos
    export PYTHONPATH=$UUOS_EXT_LIB:$PROJECT_DIR/programs/uuos:'/usr/lib/python37.zip':'/usr/lib/python3.7':'/usr/lib/python3.7/lib-dynload':'/home/newworld/.local/lib/python3.7/site-packages':$PATH

    export PYTHONHOME="/usr/lib/python3.7"
    export PYTHON_SHARED_LIB_PATH="/usr/lib/python3.7/config-3.7m-x86_64-linux-gnu/libpython3.7.so"
fi

#./build/programs/uuos/uuos -m pytest programs/uuos/tests/pytest/test_basic.py
if [[ "$1" == "" ]]; then
    ./build/programs/uuos/uuos -m pytest programs/uuos/tests/pytest --pyeos
else
    ./build/programs/uuos/uuos -m pytest -s $1 --pyeos
fi

