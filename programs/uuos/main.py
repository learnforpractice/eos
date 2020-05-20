import os
import sys
import _uuos

import struct;print(struct)
import os;import sys;sys.path.append('.')
uuos_lib=os.getenv('UUOS_EXT_LIB')
print(uuos_lib)
sys.path.append(uuos_lib)

def run():
    _uuos.say_hello()
    import sys
    print(sys.argv)
    sys.exit()
