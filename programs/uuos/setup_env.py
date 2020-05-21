import os
import sys
import shutil
import logging
import subprocess

logging.basicConfig(filename='logfile.log', level=logging.INFO, format='%(asctime)s %(levelname)s %(lineno)d %(module)s %(message)s')
logger=logging.getLogger(__name__)

formatter = logging.Formatter('%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s')
handler = logging.StreamHandler()
handler.setFormatter(formatter)

logger.addHandler(handler)
python3 = shutil.which('python3.7')
if not python3:
    python3 = shutil.which('python3.8')
    if not python3:
        raise Exception('python version >= 3.7 not found')
print(python3)

def find_python_shared_lib():
    for p in sys.path:
        for root, dirs, files in os.walk(os.path.dirname(p)):
            for file in files:
                if file.startswith('libpython3'):
                    _, ext = os.path.splitext(file)
                    if ext in ['.so', '.dylib']:
                        return os.path.join(root, file)
    return None

python3_shared_lib_path = find_python_shared_lib()
logger.info(python3_shared_lib_path)

python3home = os.path.dirname(python3_shared_lib_path)
python3home = os.path.dirname(python3home)

logger.info((python3home, python3_shared_lib_path))

# export PROJECT_DIR="/Users/newworld/dev/uuos3"
# export CHAIN_API_SHARED_LIB=$PROJECT_DIR/build//libraries/chain_api/libchain_api.dylib
# export UUOS_EXT_LIB=$PROJECT_DIR/programs/pyeos
# export PYTHONPATH=$PROJECT_DIR/programs/uuos

# export PYTHONHOME="/Users/newworld/opt/anaconda3"
# export PYTHON_SHARED_LIB_PATH="/Users/newworld/opt/anaconda3/lib/libpython3.7m.dylib"

