import os
import time
import json
import random
import hashlib
import traceback
import platform

from pyeoskit import wallet
from pyeoskit import eosapi
from pyeoskit import config
from pyeoskit import db
from pyeoskit import util

from pyeoskit.exceptions import HttpAPIError
import sys

config.main_token = 'UUOS'
eosapi.set_public_key_prefix('UUOS')
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
'5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3',#EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
'5JEcwbckBCdmji5j8ZoMHLEUS8TqQiqBG1DRx1X9DN124GUok9s',#EOS61MgZLN7Frbc2J7giU7JdYjy2TqnfWFjZuLXvpHJoKzWAj7Nst
'5JbDP55GXN7MLcNYKCnJtfKi9aD2HvHAdY7g8m67zFTAFkY1uBB',#EOS5JuNfuZPATy8oPz9KMZV2asKf9m8fb2bSzftvhW55FKQFakzFL
'5K463ynhZoCDDa4RDcr63cUwWLTnKqmdcoTKTHBjqoKfv4u5V7p',#EOS8Znrtgwt8TfpmbVpTKvA2oB8Nqey625CLN8bCN3TEbgx86Dsvr
'5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc',#EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV
'5KT26sGXAywAeUSrQjaRiX9uk9uDGNqC1CSojKByLMp7KRp8Ncw',#EOS8Ep2idd8FkvapNfgUwFCjHBG4EVNAjfUsRRqeghvq9E91tkDaj
]
for priv_key in priv_keys:
    wallet.import_key('mywallet', priv_key)

key1 = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
key2 = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'

def create_system_accounts():
    systemAccounts = [
        'eosio.bpay',
        'eosio.msig',
        'eosio.names',
        'eosio.ram',
        'eosio.ramfee',
        'eosio.saving',
        'eosio.stake',
        'eosio.token',
        'eosio.vpay',
        'eosio.rex',

        'eosio.jit',
        'eosio.jitfee',
        'uuos',
        'hello',
        'helloworld12',
        'helloworld11',
        'learnfortest',
        'learnfortest1',
        'learnfortest2'
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

key = 'EOS5yprUeBro1asg4r1b4BKuJViy6wwyJmXiP4Zqzu1Z4ixpPwuLX'
account_name = 'uuos.fund' #'uuosuuos1.m'
if not eosapi.get_account(account_name):
    eosapi.create_account('eosio', account_name, key, key)


contract_path = '/Users/newworld/dev/eos/build/contracts'
if platform.system() == 'Linux':
    contract_path = '/home/newworld/dev/uuos3/build/externals/eosio.contracts/contracts'
else:
    contract_path = '/Users/newworld/dev/uuos3/build/externals/eosio.contracts/contracts'

if 1:
    if False:
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
        print(code_hash, r['code_hash'])
        if code_hash != r['code_hash']:
            print("++++++++++set eosio.token contract")
            r = eosapi.set_contract('eosio.token', code, abi, 0)
    except Exception as e:
        r = eosapi.set_contract('eosio.token', code, abi, 0)

    if not eosapi.get_balance('eosio'):
        print('issue system token...')
        msg = {"issuer":"eosio","maximum_supply":f"11000000000.0000 {config.main_token}"}
        r = eosapi.push_action('eosio.token', 'create', msg, {'eosio.token':'active'})
        assert r
        r = eosapi.push_action('eosio.token','issue',{"to":"eosio","quantity":f"1000000000.0000 {config.main_token}","memo":""},{'eosio':'active'})
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
    eosapi.schedule_protocol_feature_activations(['0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd']) #PREACTIVATE_FEATURE
    time.sleep(2.0)
except Exception as e:
    print(e)

code_path = os.path.join(contract_path, 'eosio.bios/eosio.bios.wasm')
abi_path = os.path.join(contract_path, 'eosio.bios/eosio.bios.abi')

code = open(code_path, 'rb').read()
abi = open(abi_path, 'rb').read()

print('publish eosio.bios contract...')
try:
    r = eosapi.set_contract('eosio', code, abi, 0)
except Exception as e:
    print(e)

feature_digests = [
    'ad9e3d8f650687709fd68f4b90b41f7d825a365b02c23a636cef88ac2ac00c43',#RESTRICT_ACTION_TO_SELF
    'ef43112c6543b88db2283a2e077278c315ae2c84719a8b25f25cc88565fbea99', #REPLACE_DEFERRED
    '4a90c00d55454dc5b059055ca213579c6ea856967712a56017487886a4d4cc0f', #NO_DUPLICATE_DEFERRED_ID
    '8ba52fe7a3956c5cd3a656a3174b931d3bb2abb45578befc59f283ecd816a405', #ONLY_BILL_FIRST_AUTHORIZER
    '299dcb6af692324b899b39f16d5a530a33062804e41f09dc97e9f156b4476707', #WTMSIG_BLOCK_SIGNATURES
    'c3a6138c5061cf291310887c0b5c71fcaffeab90d5deb50d3b9e687cead45071', #ACTION_RETURN_VALUE
    'f1aab764127d9319143327124d14bf1bbfbe001ead8d2f7c329cad891c8d951b', #PYTHON_VM
    '80f35049d9fb83ef812a19bbb07eaafdd135a09276ee9a7b8dcff930ef40ebca', #ETHEREUM_VM
    '93c00d04d4836bc95250fb330d2dc18ecc17973bd0a31a1846ec51e1a98a2140', #NATIVE_EVM_EXECUTE
]

for digest in feature_digests: 
    try:
        args = {'feature_digest': digest}
        eosapi.push_action('eosio', 'activate', args, {'eosio':'active'})
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
    print('publish system contract...')
    r = eosapi.set_contract('eosio', code, abi, 0)


try:
    args = {'version':0, 'core':'4,UUOS'}
    eosapi.push_action('eosio', 'init', args, {'eosio':'active'})
except Exception as e:
    print(e)

#r = eosapi.push_action('eosio.token', 'issue', {"to":"uuos","quantity":f"1000.0000 {config.main_token}","memo":""}, {'eosio':'active'})
#print(r)
if eosapi.get_balance('uuos') <=0:
    r = eosapi.push_action('eosio.token', 'transfer', {"from":"eosio", "to":"uuos","quantity":f"1000.0000 {config.main_token}","memo":""}, {'eosio':'active'})

if eosapi.get_balance('helloworld11') <=0:
    r = eosapi.push_action('eosio.token', 'transfer', {"from":"eosio", "to":"helloworld11","quantity":f"10000000.0000 {config.main_token}","memo":""}, {'eosio':'active'})

#eosapi.schedule_protocol_feature_activations(['ad9e3d8f650687709fd68f4b90b41f7d825a365b02c23a636cef88ac2ac00c43']) #RESTRICT_ACTION_TO_SELF
#eosapi.schedule_protocol_feature_activations(['4a90c00d55454dc5b059055ca213579c6ea856967712a56017487886a4d4cc0f']) #NO_DUPLICATE_DEFERRED_ID


try:
    args = {'vmtype': 1, 'vmversion':0} #activate vm python
    eosapi.push_action('eosio', 'activatevm', args, {'eosio':'active'})
    args = {'vmtype': 2, 'vmversion':0} #activate vm python
    eosapi.push_action('eosio', 'activatevm', args, {'eosio':'active'})
except Exception as e:
    print(e)

from datetime import datetime
data_string = "2019-10-24 08:00:00" #timezone: +8
dt = datetime.fromisoformat(data_string)
print("+++++++++=main net start time:", time.mktime(dt.timetuple()))

if 0:
    key = 'EOS5yprUeBro1asg4r1b4BKuJViy6wwyJmXiP4Zqzu1Z4ixpPwuLX'
    account_name = 'uuos.fund' #'uuosuuos1.m'
    seed_round_found = 100000000.0000
    util.buyrambytes('eosio', account_name, 10*1024)
    util.dbw('eosio', account_name, seed_round_found*0.1, seed_round_found*0.9, transfer=True)

balance = eosapi.get_balance('uuos')
print('++++balance: ', balance)
while False:
    n = random.randint(0,10000000)
    elapsed = 0
    for i in range(n, n+10):
        try:
            r = eosapi.transfer('uuos', 'eosio', 0.0001, str(i))
            print(r['processed']['elapsed'])
            elapsed += int(r['processed']['elapsed'])
        except Exception as e:
            traceback.print_exc()
            print('exception:', e)
        account_name = 'helloworld11'
        try:
            r = eosapi.push_action(account_name, 'sayhello', str(i), {account_name:'active'})
            print(r['processed']['action_traces'][0]['console'])
            print(r['processed']['elapsed'])
        except Exception as e:
            response = json.loads(e.response)
            for detail in response['error']['details']:
        #        print(detail)
                print(detail['message'])

    print('avg', elapsed/10)
    print(eosapi.get_balance('uuos'))
    time.sleep(2.0)



