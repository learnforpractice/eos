import sys
from uuosio import uuos
if __name__ == "__main__":
    ret = uuos.init()
    print('++++ret:', ret)
    if not ret == 0:
        sys.exit(ret)
    uuos.exec()
    print('done!')
