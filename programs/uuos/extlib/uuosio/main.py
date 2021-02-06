import sys
from uuosio import uuos

def run():
    ret = uuos.init()
    print('++++ret:', ret)
    if not ret == 0:
        sys.exit(ret)
    uuos.exec()
    print('done!')

if __name__ == "__main__":
    run()
