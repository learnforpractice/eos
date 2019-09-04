import os
import time
import json
import random
import hashlib
import traceback
from pyeoskit import wallet
from pyeoskit import eosapi
from pyeoskit import config
from pyeoskit import db

from pyeoskit.exceptions import HttpAPIError
import sys

config.main_token = 'UUOS'
db.reset()

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
                '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc'
            ]
for priv_key in priv_keys:
    wallet.import_key('mywallet', priv_key)

key1 = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
key2 = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'

def create_system_accounts():
    systemAccounts = [
        'eosio.token',
        'eosio.bpay',
        'eosio.msig',
        'eosio.names',
        'eosio.ram',
        'eosio.ramfee',
        'eosio.jit',
        'eosio.jitfee',
        'eosio.saving',
        'eosio.stake',
        'eosio.vpay',
        'eosio.rex',
        'uuos',
        'hello',
        'helloworld12',
        'helloworld11'
    ]
    newaccount = {'creator': 'eosio',
     'name': '',
     'owner': {'threshold': 1,
               'keys': [{'key': key1,
                         'weight': 1}],
               'accounts': [],
               'waits': []},
     'active': {'threshold': 1,
                'keys': [{'key': key2,
                          'weight': 1}],
                'accounts': [],
                'waits': []}}

    for account in systemAccounts:
        if not eosapi.get_account(account):
            actions = []
            print('+++++++++create account', account)
            newaccount['name'] = account
            _newaccount = eosapi.pack_args('eosio', 'newaccount', newaccount)
            act = ['eosio', 'newaccount', _newaccount, {'eosio':'active'}]
            actions.append(act)
            rr, cost = eosapi.push_actions(actions)

create_system_accounts()


contract_path = '/Users/newworld/dev/eos/build/contracts'
contract_path = '/Users/newworld/dev/uuos2/build/externals/eosio.contracts/contracts'

if 1:
    if True:
        code_path = os.path.join(contract_path, 'eosio.token/eosio.token.wasm')
        abi_path = os.path.join(contract_path, 'eosio.token/eosio.token.abi')
    else:
        code_path = os.path.join(contract_path, 'eosio.token.v2/eosio.token.v2.wasm')
    #    code_path = os.path.join(contract_path, '/Users/newworld/dev/wasm/wasm_lua/main.wasm')
        abi_path = os.path.join(contract_path, 'eosio.token.v2/eosio.token.v2.abi')
    print(code_path)
    code = open(code_path, 'rb').read()
    abi = open(abi_path, 'rb').read()

    m = hashlib.sha256()
    m.update(code)
    code_hash = m.hexdigest()
    try:
        r = eosapi.get_code('eosio.token')
        if code_hash != r['code_hash']:
            r = eosapi.set_contract('eosio.token', code, abi, 0)
    except Exception as e:
        r = eosapi.set_contract('eosio.token', code, abi, 0)

    if not eosapi.get_balance('eosio'):
        print('issue system token...')
        msg = {"issuer":"eosio","maximum_supply":f"11000000000000.0000 {config.main_token}"}
        r = eosapi.push_action('eosio.token', 'create', msg, {'eosio.token':'active'})
        assert r
        r = eosapi.push_action('eosio.token','issue',{"to":"eosio","quantity":f"10000000000000.0000 {config.main_token}","memo":""},{'eosio':'active'})
        assert r

code_path = os.path.join(contract_path, 'eosio.msig/eosio.msig.wasm')
abi_path = os.path.join(contract_path, 'eosio.msig/eosio.msig.abi')

code = open(code_path, 'rb').read()
abi = open(abi_path, 'rb').read()

m = hashlib.sha256()
m.update(code)
code_hash = m.hexdigest()
r = eosapi.get_code('eosio.msig')
if code_hash != r['code_hash']:
    r = eosapi.set_contract('eosio.msig', code, abi, 0)
try:
    eosapi.schedule_protocol_feature_activations(['0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd'])
    time.sleep(1.0)
except Exception as e:
    print(e)

code_path = os.path.join(contract_path, 'eosio.system/eosio.system.wasm')
abi_path = os.path.join(contract_path, 'eosio.system/eosio.system.abi')

code = open(code_path, 'rb').read()
abi = open(abi_path, 'rb').read()

m = hashlib.sha256()
m.update(code)
code_hash = m.hexdigest()
r = eosapi.get_code('eosio')
if code_hash != r['code_hash']:
    r = eosapi.set_contract('eosio', code, abi, 0)


args = {'version':0, 'core':'4,UUOS'}
eosapi.push_action('eosio', 'init', args, {'eosio':'active'})

#r = eosapi.push_action('eosio.token', 'issue', {"to":"uuos","quantity":f"1000.0000 {config.main_token}","memo":""}, {'eosio':'active'})
#print(r)
r = eosapi.push_action('eosio.token', 'transfer', {"from":"eosio", "to":"uuos","quantity":f"1000.0000 {config.main_token}","memo":""}, {'eosio':'active'})


balance = eosapi.get_balance('uuos')
print('++++balance: ', balance)
while True:
    n = random.randint(0,10000000)
    for i in range(n, n+10):
        try:
            r = eosapi.transfer('uuos', 'eosio', 0.0001, str(i))
            print(r['processed']['elapsed'])
        except Exception as e:
            traceback.print_exc()
            print('exception:', e)
    print(eosapi.get_balance('uuos'))
    time.sleep(2.0)


