import os
import hashlib
import marshal
from pyeoskit import eosapi, wallet

if os.path.exists('test.wallet'):
    os.remove('test.wallet')
psw = wallet.create('test')
wallet.import_key('test', '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc')

def publish_contract(account_name, code, abi):
    m = hashlib.sha256()
    code = compile(code, "contract", 'exec')
    code = marshal.dumps(code)
    m.update(code)
    code_hash = m.hexdigest()
    r = eosapi.get_code(account_name)
    if code_hash != r['code_hash']:
        eosapi.set_contract(account_name, code, abi, 1)
    return True
#eosapi.set_nodes(['https://nodes.uuos.network'])
eosapi.set_nodes(['http://127.0.0.1:8888'])

import time
src1 = '''
import db
#import io
import sys

def test():
    return
    print(sys.builtin_module_names)

def apply(receiver, code, action):
    import _mi
    print(dir(_mi))
    print('hello,world')
    read_action_data().split(b',')
'''

src2 = '''
import db
#import io
import sys

def test():
    return
    print(sys.builtin_module_names)

def apply(receiver, code, action):
    import _mi
    print(dir(_mi))
    print('goodbye, world')
    read_action_data().split(b',')
'''

account_name = 'helloworld11'
import json

n = 0
while True:
    publish_contract(account_name, src1 + f'\nprint("{n}")', '')
    n += 1
    try:
        r = eosapi.push_action(account_name, 'sayhello', str(time.time())+str(n), {account_name:'active'})
        print(r['processed']['action_traces'][0]['console'])
        print(r['processed']['elapsed'])
    except Exception as e:
        print(e)
        response = json.loads(e.response)
        for detail in response['error']['details']:
            print(detail['message'])
    publish_contract(account_name, src2 + f'\nprint("{n}")', '')
    time.sleep(0.1)
    n += 1
    try:
        r = eosapi.push_action(account_name, 'sayhello', str(time.time())+str(n), {account_name:'active'})
        print(r['processed']['action_traces'][0]['console'])
        print(r['processed']['elapsed'])
    except Exception as e:
        print(e)
        response = json.loads(e.response)
        for detail in response['error']['details']:
            print(detail['message'])
    time.sleep(0.01)
