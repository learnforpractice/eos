import os
import sys
import time
import json
import random
import hashlib
import traceback
import platform
import logging

from uuoskit import config
from uuoskit import wallet
from uuoskit import uuosapi
from uuoskit import util

logging.basicConfig(level=logging.INFO, format='%(asctime)s %(levelname)s %(lineno)d %(module)s %(message)s')
logger=logging.getLogger(__name__)

# formatter = logging.Formatter('%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s')
# handler = logging.StreamHandler()
# handler.setFormatter(formatter)

cur_dir = os.path.dirname(__file__)

config.setup_eos_network()

if len(sys.argv) == 2:
    print(sys.argv)
    uuosapi.set_nodes([sys.argv[1]])

if os.path.exists('mywallet.wallet'):
    os.remove('mywallet.wallet')
psw = wallet.create('mywallet')
print(psw)
priv_keys = [
    '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3',#EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
    '5JEcwbckBCdmji5j8ZoMHLEUS8TqQiqBG1DRx1X9DN124GUok9s',#EOS61MgZLN7Frbc2J7giU7JdYjy2TqnfWFjZuLXvpHJoKzWAj7Nst
    '5JbDP55GXN7MLcNYKCnJtfKi9aD2HvHAdY7g8m67zFTAFkY1uBB',#EOS5JuNfuZPATy8oPz9KMZV2asKf9m8fb2bSzftvhW55FKQFakzFL
    '5K463ynhZoCDDa4RDcr63cUwWLTnKqmdcoTKTHBjqoKfv4u5V7p',#EOS8Znrtgwt8TfpmbVpTKvA2oB8Nqey625CLN8bCN3TEbgx86Dsvr
    '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc',#EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV
    '5KT26sGXAywAeUSrQjaRiX9uk9uDGNqC1CSojKByLMp7KRp8Ncw',#EOS8Ep2idd8FkvapNfgUwFCjHBG4EVNAjfUsRRqeghvq9E91tkDaj

    '5JRYimgLBrRLCBAcjHUWCYRv3asNedTYYzVgmiU4q2ZVxMBiJXL',#EOS6AjF6hvF7GSuSd4sCgfPKq5uWaXvGM2aQtEUCwmEHygQaqxBSV
    '5Jbb4wuwz8MAzTB9FJNmrVYGXo4ABb7wqPVoWGcZ6x8V2FwNeDo',#EOS7sPDxfw5yx5SZgQcVb57zS1XeSWLNpQKhaGjjy2qe61BrAQ49o
    '5JHRxntHapUryUetZgWdd3cg6BrpZLMJdqhhXnMaZiiT4qdJPhv',#EOS89jesRgvvnFVuNtLg4rkFXcBg2Qq26wjzppssdHj2a8PSoWMhx
    '5Jbh1Dn57DKPUHQ6F6eExX55S2nSFNxZhpZUxNYFjJ1arKGK9Q3',#EOS73ECcVHVWvuxJVm5ATnqBTCFMtA6WUsdDovdWH5NFHaXNq1hw1
    '5JJYrXzjt47UjHyo3ud5rVnNEPTCqWvf73yWHtVHtB1gsxtComG',#EOS8h8TmXCU7Pzo5XQKqyWwXAqLpPj4DPZCv5Wx9Y4yjRrB6R64ok
    '5J9PozRVudGYf2D4b8JzvGxPBswYbtJioiuvYaiXWDYaihNFGKP',#EOS65jj8NPh2EzLwje3YRy4utVAATthteZyhQabpQubxHNJ44mem9
    '5K9AZWR2wEwtZii52vHigrxcSwCzLhhJbNpdXpVFKHP5fgFG5Tx',#EOS5fVw435RSwW3YYWAX9qz548JFTWuFiBcHT3PGLryWaAMmxgjp1
]

for priv_key in priv_keys:
    wallet.import_key('mywallet', priv_key)

key1 = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
key2 = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'


def deploy_contract(account_name, contract_name, contracts_path=None):
    logger.info('++++deploy_contract %s %s', account_name, contract_name)
    if not contracts_path:
        contracts_path = os.path.dirname(__file__)
        # contracts_path = '../../../build/externals/eosio.contracts'
        # contracts_path = '.'
        contracts_path = os.path.join(contracts_path, f'contracts/{contract_name}')

    code_path = os.path.join(contracts_path, f'{contract_name}.wasm')
    abi_path = os.path.join(contracts_path, f'{contract_name}.abi')

    logger.info(code_path)
    code = open(code_path, 'rb').read()
    abi = open(abi_path, 'rb').read()

    m = hashlib.sha256()
    m.update(code)
    code_hash = m.hexdigest()

    try:
        r = uuosapi.get_raw_code(account_name)
        logger.info((code_hash, r['code_hash']))
        if code_hash != r['code_hash']:
            logger.info(f"++++++++++set contract: {account_name}")
            r = uuosapi.deploy_contract(account_name, code, abi, vm_type=0)
            return True
    except Exception as e:
        logger.exception(e)
#        r = uuosapi.set_contract(account_name, code, abi, 0)


def deploy_micropython_contract():
    logger.info("++++++++deploy_micropython_contract")
    code_path = os.path.join(cur_dir, './contracts/micropython/micropython_uuos.wasm')
    code_path = os.path.join(cur_dir, './contracts/micropython/micropython.wasm')
    abi_path = os.path.join(cur_dir, './contracts/micropython/micropython.abi')

    code = open(code_path, 'rb').read()
    abi = open(abi_path, 'rb').read()

    try:
        r = uuosapi.deploy_contract('hello', code, abi, vm_type=0)
        r = uuosapi.deploy_contract('eosio.mpy', code, abi, vm_type=0)
    except Exception as e:
        logger.exception(e)

    code = '''
def apply(a, b, c):
    pass
'''
    account = 'hello'

    code = uuosapi.mp_compile(account, code)

    account = 'hello'
    args = uuosapi.s2b(account) + code
    uuosapi.push_action(account, 'setcode', args, {account:'active'})

    account = 'eosio.mpy'
    args = uuosapi.s2b(account) + code
    uuosapi.push_action(account, 'setcode', args, {account:'active'})

def create_account(account, key1, key2):
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
    if not uuosapi.get_account(account):
        actions = []
        logger.info(('+++++++++create account', account))
        newaccount['name'] = account
        _newaccount = uuosapi.pack_args('eosio', 'newaccount', newaccount)
        act = ['eosio', 'newaccount', _newaccount, {'eosio':'active'}]
        actions.append(act)
        r = uuosapi.push_actions(actions)

systemAccounts = [
    'eosio.mpy',
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
]
for a in systemAccounts:
    create_account(a, key1, key2)

accounts = (
    'hello',
    'helloworld11',
    'helloworld12',
    'helloworld13',
    'helloworld14',
    'helloworld15',
    'helloworld33',
    'learnfortest'
)

pub_keys = (
    'EOS6AjF6hvF7GSuSd4sCgfPKq5uWaXvGM2aQtEUCwmEHygQaqxBSV',
    'EOS7sPDxfw5yx5SZgQcVb57zS1XeSWLNpQKhaGjjy2qe61BrAQ49o',
    'EOS89jesRgvvnFVuNtLg4rkFXcBg2Qq26wjzppssdHj2a8PSoWMhx',
    'EOS73ECcVHVWvuxJVm5ATnqBTCFMtA6WUsdDovdWH5NFHaXNq1hw1',
    'EOS8h8TmXCU7Pzo5XQKqyWwXAqLpPj4DPZCv5Wx9Y4yjRrB6R64ok',
    'EOS65jj8NPh2EzLwje3YRy4utVAATthteZyhQabpQubxHNJ44mem9',
    'EOS5fVw435RSwW3YYWAX9qz548JFTWuFiBcHT3PGLryWaAMmxgjp1',
    'EOS4vtCi4jbaVCLVJ9Moenu9j7caHeoNSWgWY65bJgEW8MupWsRMo',
)
i = 0
for a in accounts:
    key = pub_keys[i]
    create_account(a, key, key)
    i += 1

try:
    uuosapi.schedule_protocol_feature_activations(['0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd']) #PREACTIVATE_FEATURE
    time.sleep(6.0)
    logger.info('set PREACTIVATE_FEATURE done!')
except Exception as e:
    logger.exception(e)

# try:
#     uuosapi.update_runtime_options(max_transaction_time=230)
#     time.sleep(2.0)
# except Exception as e:
#     logger.exception(e)

contracts_path = os.path.dirname(__file__)
contracts_path = os.path.join(contracts_path, 'contracts')

if not uuosapi.get_raw_code_and_abi('eosio')['wasm']:
    deploy_contract('eosio', 'eosio.bios')

feature_digests = [
    '1a99a59d87e06e09ec5b028a9cbb7749b4a5ad8819004365d02dc4379a8b7241', #'ONLY_LINK_TO_EXISTING_PERMISSION' 
    '2652f5f96006294109b3dd0bbde63693f55324af452b799ee137a81a905eed25', #'FORWARD_SETCODE' 
    '299dcb6af692324b899b39f16d5a530a33062804e41f09dc97e9f156b4476707', #'WTMSIG_BLOCK_SIGNATURES' 
    'ef43112c6543b88db2283a2e077278c315ae2c84719a8b25f25cc88565fbea99', #'REPLACE_DEFERRED' 
    '4a90c00d55454dc5b059055ca213579c6ea856967712a56017487886a4d4cc0f', #'NO_DUPLICATE_DEFERRED_ID' 
    '4e7bf348da00a945489b2a681749eb56f5de00b900014e137ddae39f48f69d67', #'RAM_RESTRICTIONS' 
    '4fca8bd82bbd181e714e283f83e1b45d95ca5af40fb89ad3977b653c448f78c2', #'WEBAUTHN_KEY'
    '5443fcf88330c586bc0e5f3dee10e7f63c76c00249c87fe4fbf7f38c082006b4', #'BLOCKCHAIN_PARAMETERS'
    '68dcaa34c0517d19666e6b33add67351d8c5f69e999ca1e37931bc410a297428', #'DISALLOW_EMPTY_PRODUCER_SCHEDULE'
    '825ee6288fb1373eab1b5187ec2f04f6eacb39cb3a97f356a07c91622dd61d16', #'KV_DATABASE'
    '8ba52fe7a3956c5cd3a656a3174b931d3bb2abb45578befc59f283ecd816a405', #'ONLY_BILL_FIRST_AUTHORIZER'
    'ad9e3d8f650687709fd68f4b90b41f7d825a365b02c23a636cef88ac2ac00c43', #'RESTRICT_ACTION_TO_SELF'
    'bf61537fd21c61a60e542a5d66c3f6a78da0589336868307f94a82bccea84e88', #'CONFIGURABLE_WASM_LIMITS'
    'c3a6138c5061cf291310887c0b5c71fcaffeab90d5deb50d3b9e687cead45071', #'ACTION_RETURN_VALUE'
    'e0fb64b1085cc5538970158d05a009c24e276fb94e1a0bf6a528b48fbc4ff526', #'FIX_LINKAUTH_RESTRICTION'
    'f0af56d2c5a48d60a4a5b5c903edfb7db3a736a94ed589d0b797df33ff9d3e1d', #'GET_SENDER'
]

for digest in feature_digests: 
    try:
        args = {'feature_digest': digest}
        logger.info(f'activate {digest}')
        uuosapi.push_action('eosio', 'activate', args, {'eosio':'active'})
    except Exception as e:
        logger.error(e)

deploy_micropython_contract()

deploy_contract('eosio.token', 'eosio.token')

if not uuosapi.get_balance('eosio'):
    logger.info('issue system token...')
    msg = {"issuer":"eosio","maximum_supply":f"11000000000.0000 {config.main_token}"}
    r = uuosapi.push_action('eosio.token', 'create', msg, {'eosio.token':'active'})
    assert r
    r = uuosapi.push_action('eosio.token','issue',{"to":"eosio","quantity":f"1000000000.0000 {config.main_token}","memo":""},{'eosio':'active'})
    assert r

try:
    deploy_contract('eosio.msig', 'eosio.msig')
except Exception as e:
    logger.exception(e)

for i in range(3):
    try:
        if deploy_contract('eosio', 'eosio.system'):
            break
    except Exception as e:
        logger.exception(e)

if True:
    args = dict(version = 0,
                core = f'4,{config.main_token}',
                min_bp_staking_amount = 0,
                vote_producer_limit = 100,
                mini_voting_requirement = 21
    )

    args['min_bp_staking_amount'] = 10000000000
    try:
        uuosapi.push_action('eosio', 'init', args, {'eosio':'active'})
    except Exception as e:
        logger.exception(e)

if uuosapi.get_balance('helloworld11') <=0:
    r = uuosapi.push_action('eosio.token', 'transfer', {"from":"eosio", "to":"helloworld11","quantity":f"100000000.0000 {config.main_token}","memo":""}, {'eosio':'active'})

if uuosapi.get_balance('helloworld12') <=0:
    r = uuosapi.push_action('eosio.token', 'transfer', {"from":"eosio", "to":"helloworld12","quantity":f"100000000.0000 {config.main_token}","memo":""}, {'eosio':'active'})

if uuosapi.get_balance('helloworld13') <=0:
    r = uuosapi.push_action('eosio.token', 'transfer', {"from":"eosio", "to":"helloworld13","quantity":f"100000000.0000 {config.main_token}","memo":""}, {'eosio':'active'})

if uuosapi.get_balance('helloworld14') <=0:
    r = uuosapi.push_action('eosio.token', 'transfer', {"from":"eosio", "to":"helloworld14","quantity":f"100000000.0000 {config.main_token}","memo":""}, {'eosio':'active'})


if uuosapi.get_balance('hello') <=0:
    r = uuosapi.push_action('eosio.token', 'transfer', {"from":"eosio", "to":"hello","quantity":f"10000000.0000 {config.main_token}","memo":""}, {'eosio':'active'})

for account in  ('helloworld11', 'helloworld12', 'helloworld13', 'helloworld14', 'helloworld15', 'learnfortest'):
    uuosapi.transfer('eosio', account, 1000.0)
    util.buyrambytes('eosio', account, 5*1024*1024)
    util.dbw('eosio', account, 1.0, 1000)

try:
    args = {'vmtype': 1, 'vmversion':0} #activate vm python
    uuosapi.push_action('eosio', 'activatevm', args, {'eosio':'active'})
except Exception as e:
    logger.info(e)

try:
    args = {'vmtype': 2, 'vmversion':0} #activate vm python
    uuosapi.push_action('eosio', 'activatevm', args, {'eosio':'active'})
except Exception as e:
    logger.info(e)

balance = uuosapi.get_balance('hello')
logger.info(f'++++balance: {balance}')
while False:
    n = random.randint(0,10000000)
    elapsed = 0
    for i in range(n, n+10):
        try:
            r = uuosapi.transfer('hello', 'eosio', 0.0001, str(i))
            logger.info(r['processed']['elapsed'])
            elapsed += int(r['processed']['elapsed'])
        except Exception as e:
            traceback.print_exc()
            logger.info(f'exception:{e}')

    logger.info(f'AVG: {elapsed/10}')
    logger.info(uuosapi.get_balance('hello'))
    time.sleep(2.0)

accounts = (
    'hello',
    'helloworld11',
    'helloworld12',
    'helloworld13',
    'helloworld14',
    'helloworld15',
    'helloworld33',
    'learnfortest'
)

from uuoskit import util
for account in accounts:
    print('buy ram', account)
    util.buyrambytes('hello', account, 10*1024*1024)
    print('buy cpu', account)
    util.dbw('hello', account, 1.0, 1.0)

