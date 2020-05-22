export PROJECT_DIR=$(pwd)
export CHAIN_API_SHARED_LIB=$PROJECT_DIR/build/libraries/chain_api/libchain_api.so
export UUOS_EXT_LIB=$PROJECT_DIR/programs/pyeos
export PYTHONPATH=$UUOS_EXT_LIB:$PROJECT_DIR/programs/uuos:'/usr/lib/python37.zip':'/usr/lib/python3.7':'/usr/lib/python3.7/lib-dynload':'/home/newworld/.local/lib/python3.7/site-packages':$PATH

export PYTHONHOME="/usr/lib/python3.7"
export PYTHON_SHARED_LIB_PATH="/usr/lib/python3.7/config-3.7m-x86_64-linux-gnu/libpython3.7.so"

#$PROJECT_DIR/build/programs/uuos/uuos -m pytest ../../programs/uuos/tests/test.py::Test::test_create_account


#./build/programs/uuos/uuos -m pytest -s programs/uuos/tests/pytest

./build/programs/uuos/uuos -m pytest programs/uuos/tests/pytest

