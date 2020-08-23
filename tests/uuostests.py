import os
import time
import json
import random
import hashlib
import traceback
from pyeoskit import wallet
from pyeoskit import eosapi
from pyeoskit import config
from pyeoskit import util

from pyeoskit.exceptions import HttpAPIError
import sys

config.main_token = 'UUOS'

if len(sys.argv) == 2:
    print(sys.argv)
    eosapi.set_nodes([sys.argv[1]])

if os.path.exists('mywallet.wallet'):
    os.remove('mywallet.wallet')
psw = wallet.create('mywallet')
print(psw)
eosapi.get_info()
priv_keys = [
                '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3',
                '5JEcwbckBCdmji5j8ZoMHLEUS8TqQiqBG1DRx1X9DN124GUok9s',
                '5JbDP55GXN7MLcNYKCnJtfKi9aD2HvHAdY7g8m67zFTAFkY1uBB',
                '5K463ynhZoCDDa4RDcr63cUwWLTnKqmdcoTKTHBjqoKfv4u5V7p',
                '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc',#
                '5KT26sGXAywAeUSrQjaRiX9uk9uDGNqC1CSojKByLMp7KRp8Ncw',#EOS8Ep2idd8FkvapNfgUwFCjHBG4EVNAjfUsRRqeghvq9E91tkDaj
                '5JzVeUR7zgUJ8uFQQQ5nAo6QHVLYkv6PFDbauZYHHrXzkGWzbqj',#EOS7XR7ZhRTD7VNxoRFhCh8ov1cYtMnJVBy1TGGq5TcdDM1CmadpP
            ]
for priv_key in priv_keys:
    wallet.import_key('mywallet', priv_key)

account_name = 'helloworld111'

while True:
    n = random.randint(0,10000000)
    elapsed = 0
    for i in range(n, n+10):
        try:
            r = eosapi.transfer(account_name, 'eosio', 0.0001, str(i))
            print(r['processed']['elapsed'])
            elapsed += int(r['processed']['elapsed'])
        except Exception as e:
            traceback.print_exc()
            print('exception:', e)

    print('avg', elapsed/10)
    print(eosapi.get_balance('uuos'))
    time.sleep(2.0)
