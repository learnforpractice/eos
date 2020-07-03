import os
import sys
import uuos
from _uuos import start_eosio

print('hello,uuos', os.getpid())
start_eosio(len(sys.argv), sys.argv)

