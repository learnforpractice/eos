import os
import sys
import time
from uuoskit import uuosapi, wallet, config

if len(sys.argv) >= 2:
    uuosapi.set_node(sys.argv[1])

config.main_token = 'UUOS'
config.main_token_contract = 'eosio.token'
config.system_contract = 'eosio'

if os.path.exists('test.wallet'):
    os.remove('test.wallet')
psw = wallet.create('test')

wallet.import_key('test', '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc')
wallet.import_key('test', '5Jbb4wuwz8MAzTB9FJNmrVYGXo4ABb7wqPVoWGcZ6x8V2FwNeDo')
wallet.import_key('test', '5JRYimgLBrRLCBAcjHUWCYRv3asNedTYYzVgmiU4q2ZVxMBiJXL')
wallet.import_key('test', '5JHRxntHapUryUetZgWdd3cg6BrpZLMJdqhhXnMaZiiT4qdJPhv')

code = '../contracts/micropython/micropython_uuos_call_contract.wasm'
code = os.path.join(os.path.dirname(__file__), code)

with open(code, 'rb') as f:
    code = f.read()
uuosapi.deploy_contract('eosio.mpy', code, b'', vm_type=0)