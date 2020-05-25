import os
import io
import gc
import time
import sys
import ujson as json
import struct
import asyncio
import shutil
import tempfile
import unittest
import platform
import marshal

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from datetime import datetime, timedelta
from uuos.jsonobject import JsonObject
from uuos.nativeobject import SignedBlockMessage, PackedTransactionMessage

#from uuos import db
from uuos import config
from uuos.chain import Chain
from uuos.chainapi import ChainApi

from uuos import nativeobject
from uuos import application
import uuos

logger = application.get_logger(__name__)

def isoformat(dt):
    return dt.isoformat(timespec='milliseconds')

#logger = LogWrapper(logger)

genesis_test = {
  "initial_timestamp": "2019-10-24T00:00:00.888",
  "initial_key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
  "initial_configuration": {
    "max_block_net_usage": 1048576,
    "target_block_net_usage_pct": 1000,
    "max_transaction_net_usage": 524288,
    "base_per_transaction_net_usage": 12,
    "net_usage_leeway": 500,
    "context_free_discount_net_usage_num": 20,
    "context_free_discount_net_usage_den": 100,
    "max_block_cpu_usage": 500000,
    "target_block_cpu_usage_pct": 1000,
    "max_transaction_cpu_usage": 450000,
    "min_transaction_cpu_usage": 100,
    "max_transaction_lifetime": 3600,
    "deferred_trx_expiration_window": 600,
    "max_transaction_delay": 3888000,
    "max_inline_action_size": 4096,
    "max_inline_action_depth": 4,
    "max_authority_depth": 6
  }
}

genesis_uuos = {
  "initial_timestamp": "2019-10-24T00:00:00.888",
  "initial_key": "EOS7rqzK4qFGTSbgQqr5ynNWKqsZTdJxgKUUELkbFXNcjn4JwUuoS",
  "initial_configuration": {
    "max_block_net_usage": 1048576,
    "target_block_net_usage_pct": 1000,
    "max_transaction_net_usage": 524288,
    "base_per_transaction_net_usage": 12,
    "net_usage_leeway": 500,
    "context_free_discount_net_usage_num": 20,
    "context_free_discount_net_usage_den": 100,
    "max_block_cpu_usage": 200000,
    "target_block_cpu_usage_pct": 1000,
    "max_transaction_cpu_usage": 150000,
    "min_transaction_cpu_usage": 100,
    "max_transaction_lifetime": 3600,
    "deferred_trx_expiration_window": 600,
    "max_transaction_delay": 3888000,
    "max_inline_action_size": 4096,
    "max_inline_action_depth": 4,
    "max_authority_depth": 6
  }
}

genesis_eos = {
    "initial_timestamp": "2018-06-08T08:08:08.888",
    "initial_key": "EOS7EarnUhcyYqmdnPon8rm7mBCTnBoot6o7fE2WzjvEX2TdggbL3",
    "initial_configuration": {
        "max_block_net_usage": 1048576,
        "target_block_net_usage_pct": 1000,
        "max_transaction_net_usage": 524288,
        "base_per_transaction_net_usage": 12,
        "net_usage_leeway": 500,
        "context_free_discount_net_usage_num": 20,
        "context_free_discount_net_usage_den": 100,
        "max_block_cpu_usage": 200000,
        "target_block_cpu_usage_pct": 1000,
        "max_transaction_cpu_usage": 150000,
        "min_transaction_cpu_usage": 100,
        "max_transaction_lifetime": 3600,
        "deferred_trx_expiration_window": 600,
        "max_transaction_delay": 3888000,
        "max_inline_action_size": 4096,
        "max_inline_action_depth": 4,
        "max_authority_depth": 6
    }
}

class Object():
    pass

# print(os.getpid())
# print('>>>')
# input()

class ChainTest(object):

    def __init__(self, uuos_network=False, jit=False):
        uuos.set_log_level('default', 0)
        self.feature_digests = []

        self.uuos_network = uuos_network
        self.feature_activated = False
        self.main_token = 'UUOS'
        logger.info(('++++++++++++pid:', os.getpid()))
        # input()
        uuos.set_default_log_level(0)

        options = Object()
        options.data_dir = tempfile.mkdtemp()
        options.config_dir = tempfile.mkdtemp()

        logger.debug(f'{options.data_dir}, {options.config_dir}')
        if uuos_network:
            options.chain_state_db_size_mb = 50 #350
        else:
            options.chain_state_db_size_mb = 30

        options.contracts_console = True
        options.uuos_mainnet = uuos_network
        options.network = 'test'
        options.snapshot = ''

        self._chain = None

        chain_cfg = nativeobject.ControllerConfig(config.default_config)
        chain_cfg.contracts_console = options.contracts_console
        chain_cfg.blocks_dir = os.path.join(options.data_dir, 'blocks')
        chain_cfg.state_dir = os.path.join(options.data_dir, 'state')

        if uuos_network:
            chain_cfg.genesis_accounts_file = os.path.join(test_dir, 'test_genesis_accounts.bin')

        chain_cfg.state_guard_size = 5*1024*1024
        chain_cfg.reversible_cache_size = 50*1024*1024
        chain_cfg.reversible_guard_size = 5*1024*1024

        if jit:
            chain_cfg.wasm_runtime = 'eos_vm_jit'
        else:
            chain_cfg.wasm_runtime = 'wabt'

        logger.info(f'+++++++++++++++++++++wasm_runtime: {chain_cfg.wasm_runtime}')

        uuos.set_default_data_dir(options.data_dir)
        uuos.set_default_config_dir(options.config_dir)

        chain_cfg.state_size = options.chain_state_db_size_mb * 1024 * 1024

        print('config.uuos_mainnet', options.uuos_mainnet)
        chain_cfg.uuos_mainnet = options.uuos_mainnet

        self.options = options
        if self.options.network == 'uuos':
            chain_cfg.uuos_mainnet = True
            genesis = genesis_uuos
        elif self.options.network == 'eos':
            genesis = genesis_eos
        elif self.options.network == 'test':
            genesis = genesis_test
        else:
            raise Exception('unknown network')

        # logger.info(chain_cfg)
        chain_cfg = chain_cfg.dumps()
#        logger.info(chain_cfg)
        genesis = json.dumps(genesis)
        logger.debug(genesis)
        uuos.set_default_log_level(10)
        self.chain = Chain(chain_cfg, genesis, options.config_dir, options.snapshot)

        self.init()

        self.chain_api = ChainApi(self.chain.ptr)

    def init(self):
        self.chain.startup(True)
        uuos.set_default_log_level(0)
        uuos.set_accepted_block_callback(self.on_accepted_block)
        self.feature_digests = ['0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd']
        self.start_block()
        self.produce_block()
        key = 'EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV'
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
                'alice',
                'bob'
        ]
        for a in systemAccounts:
            self.create_account('eosio', a, key, key)

        logger.info('deploy eosio.token')
        self.deploy_eosio_token()

        logger.info('deploy eosio.bios...')
        self.deploy_eosio_bios()
        self.produce_block()

        feature_digests = [
            '1a99a59d87e06e09ec5b028a9cbb7749b4a5ad8819004365d02dc4379a8b7241', #'ONLY_LINK_TO_EXISTING_PERMISSION'
            '2652f5f96006294109b3dd0bbde63693f55324af452b799ee137a81a905eed25', #'FORWARD_SETCODE'
            '299dcb6af692324b899b39f16d5a530a33062804e41f09dc97e9f156b4476707',#'WTMSIG_BLOCK_SIGNATURES'
            'ef43112c6543b88db2283a2e077278c315ae2c84719a8b25f25cc88565fbea99',#'REPLACE_DEFERRED'
            '4a90c00d55454dc5b059055ca213579c6ea856967712a56017487886a4d4cc0f',#'NO_DUPLICATE_DEFERRED_ID'
            '4e7bf348da00a945489b2a681749eb56f5de00b900014e137ddae39f48f69d67',#'RAM_RESTRICTIONS'
            '4fca8bd82bbd181e714e283f83e1b45d95ca5af40fb89ad3977b653c448f78c2',#'WEBAUTHN_KEY'
            '68dcaa34c0517d19666e6b33add67351d8c5f69e999ca1e37931bc410a297428',#'DISALLOW_EMPTY_PRODUCER_SCHEDULE'
            '8ba52fe7a3956c5cd3a656a3174b931d3bb2abb45578befc59f283ecd816a405',#'ONLY_BILL_FIRST_AUTHORIZER'
            'ad9e3d8f650687709fd68f4b90b41f7d825a365b02c23a636cef88ac2ac00c43',#'RESTRICT_ACTION_TO_SELF'
            'e0fb64b1085cc5538970158d05a009c24e276fb94e1a0bf6a528b48fbc4ff526',#'FIX_LINKAUTH_RESTRICTION'
            'f0af56d2c5a48d60a4a5b5c903edfb7db3a736a94ed589d0b797df33ff9d3e1d',#'GET_SENDER'
            'f1aab764127d9319143327124d14bf1bbfbe001ead8d2f7c329cad891c8d951b',#'PYTHONVM'
            '3dabae1906d16d6c1e72d6ce87574c2291ce5d5e4830d3b46589cbe96ce7af9c',#'ETHEREUM_VM'
        ]

        for digest in feature_digests: 
            try:
                args = {'feature_digest': digest}
                self.push_action('eosio', 'activate', args, 'eosio', 'active')
                self.feature_digests.append(digest)
            except Exception as e:
                logger.info(e)
        self.produce_block()

        logger.info('deploy eosio.system...')
        if self.uuos_network:
            self.deploy_eosio_system_uuos()
        else:
            self.deploy_eosio_system()
        self.produce_block()

        args = {"issuer":"eosio", "maximum_supply":f"11000000000.0000 {self.main_token}"}
        r = self.push_action('eosio.token', 'create', args, 'eosio.token', 'active')

        args = {"to":"eosio","quantity":f"1000000000.0000 {self.main_token}", "memo":""}
        r = self.push_action('eosio.token','issue', args, 'eosio', 'active')

        args = {'version':0, 'core':'4,UUOS'}
        self.push_action('eosio', 'init', args, 'eosio', 'active')
        self.produce_block()

        if self.uuos_network:
            args = {'vmtype': 1, 'vmversion':0} #activate vm python
            self.push_action('eosio', 'activatevm', args, 'eosio', 'active')

        self.produce_block()

    @property
    def chain(self):
        return self._chain

    @chain.setter
    def chain(self, c):
        self._chain = c

    def on_accepted_block(self, block, num, block_id):
        pass
        # msg = SignedBlockMessage.unpack(block)
        # logger.info(msg)

    def get_balance(self, account, token_account='eosio.token', token_name='UUOS'):
        # symbol = b'\x04'+token_name.encode('utf8')
        # symbol = symbol.ljust(8, b'\x00')
        # symbol = int.from_bytes(symbol, 'little')
        # symbol >>= 8
        # itr = db.find_i64(token_account, account, 'accounts', symbol)
        # if itr < 0:
        #     return 0.0
        # value = db.get_i64(itr)
        # amount, symbol = struct.unpack('q8s', value)
        # amount /=10000
        # # logger.info(f'{amount}, {symbol}')
        # return amount

        params = {
            'code': token_account,
            'account': account,
            'symbol': token_name
        }
        params = json.dumps(params)
        ret, result = self.chain_api.get_currency_balance(params)
        if ret:
            result = json.loads(result)
            if result:
                return float(result[0].split(' ')[0])
        return 0.0

    def calc_pending_block_time(self):
#        self.chain.abort_block()
        now = datetime.utcnow()
        base = self.chain.head_block_time()
        if base < now:
            base = now
        min_time_to_next_block = config.block_interval_us - int(base.timestamp()*1e6) % config.block_interval_us
        # print('min_time_to_next_block:', min_time_to_next_block)
        block_time = base + timedelta(microseconds=min_time_to_next_block)
        if block_time - now < timedelta(microseconds=config.block_interval_us/10):
            block_time += timedelta(microseconds=config.block_interval_us)        
        return block_time

    #   permission_level
    #   account_name    actor;
    #   permission_name permission;

    #   account_name               account;
    #   action_name                name;
    #   vector<permission_level>   authorization;
    #   bytes                      data;

    def gen_action(self, account, action, args, actor, perm='active'):
        if isinstance(args, dict):
            args = self.pack_args(account, action, args)
        assert type(args) is bytes
        return {
            'account': account,
            'name': action,
            'data': args.hex(),
            'authorization':[{'actor': actor, 'permission': perm}]
        }

    def push_action(self, account, action, args, actor=None, perm='active'):
        if not actor:
            actor = account
        logger.debug(f'{account}, {action}, {args}')
        if not isinstance(args, bytes):
            _args = self.chain.pack_action_args(account, action, args)
            if not _args:
                raise Exception(f'pack error for {args}')
            args = _args
            # logger.error(f'++++{args}')
        a = {
            'account': account,
            'name': action,
            'data': args.hex(),
            'authorization':[{'actor': actor, 'permission': perm}]
        }
        ret = self.push_actions([a])
        elapsed = ret.elapsed
        logger.info(f'+++++{account} {action} {elapsed}')
        return ret

    def push_actions(self, actions):
        chain_id = self.chain.id()
        ref_block_id = self.chain.last_irreversible_block_id()
        priv_key = '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3'
        actions = json.dumps(actions)
        expiration = datetime.utcnow() + timedelta(seconds=60)
        expiration = isoformat(expiration)
        raw_signed_trx = self.chain.gen_transaction(actions, expiration, ref_block_id, chain_id, False, priv_key)
        signed_trx = PackedTransactionMessage.unpack(raw_signed_trx)
        # logger.info(signed_trx)
        # r = uuos.unpack_native_object(13, bytes.fromhex(signed_trx.packed_trx))
        # logger.info(r)

        deadline = datetime.utcnow() + timedelta(microseconds=100000)
        billed_cpu_time_us = 2000
        ret, result = self.chain.push_transaction(raw_signed_trx, isoformat(deadline), billed_cpu_time_us)
#        print(ret, result)
        result = json.loads(result)
        if not ret:
            raise Exception(result)
        result = JsonObject(result)
        return result

    def transfer(self, _from, _to, _amount, _memo='', token_account='eosio.token', token_name='', permission='active'):
        if not token_name:
            token_name = self.main_token
        args = {"from":_from, "to":_to, "quantity":'%.4f %s'%(_amount,token_name), "memo":_memo}
        return self.push_action(token_account, 'transfer', args, _from, permission)

    def pack_args(self, account, action , args):
        ret = self.chain.pack_action_args(account, action, args)
        if not ret:
            raise Exception('pack error')
        return ret

    def unpack_args(self, account, action, raw_args):
        ret = self.chain.unpack_action_args(account, action, raw_args)
        if not ret:
            raise Exception('unpack error')
        return ret

    def create_account(self, creator, account, owner_key, active_key, ram_bytes=0, stake_net=0.0, stake_cpu=0.0):
        actions = []
        logger.debug(f'{creator} {account}')
        args = {
            'creator': creator,
            'name': account,
            'owner': {'threshold': 1,
                    'keys': [{'key': owner_key, 'weight': 1}],
                    'accounts': [],
                    'waits': []
                    },
            'active': {'threshold': 1,
                        'keys': [{'key': active_key, 'weight': 1}],
                        'accounts': [],
                        'waits': []
                    }
        }
        newaccount_args = self.pack_args('eosio', 'newaccount', args)
        if not newaccount_args:
            raise Exception('bad args')
        newaccount_action = {
            'account': 'eosio',
            'name': 'newaccount',
            'data': newaccount_args.hex(),
            'authorization':[{'actor':'eosio', 'permission':'active'}]
        }
        actions.append(newaccount_action)

        if ram_bytes:
            args = {'payer':creator, 'receiver':account, 'bytes':ram_bytes}
            act = self.gen_action('eosio', 'buyrambytes', args, creator, perm='active')
            actions.append(act)

        if stake_net or stake_cpu:
            args = {
                'from': creator,
                'receiver': account,
                'stake_net_quantity': '%0.4f %s'%(stake_net, self.main_token),
                'stake_cpu_quantity': '%0.4f %s'%(stake_cpu, self.main_token),
                'transfer': 1
            }
            act = self.gen_action('eosio', 'delegatebw', args, creator, perm='active')
            actions.append(act)

        return self.push_actions(actions)

    def deploy_contract(self, account, code, abi, vmtype=0):
        actions = []
        setcode = {"account": account,
                   "vmtype": vmtype,
                   "vmversion": 0,
                   "code": code.hex()
        }

        setcode = self.chain.pack_action_args('eosio', 'setcode', setcode)
        actions = []
        setcode = {
            'account': 'eosio',
            'name': 'setcode',
            'data': setcode.hex(),
            'authorization':[{'actor': account, 'permission':'active'}]
        }
        actions.append(setcode)
        if abi:
            abi = nativeobject.pack_native_object(nativeobject.abi_def_type, abi)
        setabi = self.chain.pack_action_args('eosio', 'setabi', {'account':account, 'abi':abi.hex()})
        setabi = {
            'account': 'eosio',
            'name': 'setabi',
            'data': setabi.hex(),
            'authorization':[{'actor': account, 'permission':'active'}]
        }
        actions.append(setabi)
        # logger.info(actions)
        ret = self.push_actions(actions)
        elapsed = ret.elapsed
        logger.info(f'+++++deploy contract: {account} {elapsed}')
        # logger.info(ret)
        return ret

    def update_auth(self, account, accounts, keys, perm='active', parent='owner'):
        a = {
            "account": account,
            "permission": perm,
            "parent": parent,
            "auth": {
                "threshold": 1,
                "keys": keys,
                "accounts": accounts,
                "waits": []
            }
        }

        self.push_action('eosio', 'updateauth', a, actor=account, perm='owner')

    def deploy_eosio_token(self):
        # contract_path = os.path.join(test_dir, '../../..', 'build/externals/eosio.contracts/contracts')
        # code_path = os.path.join(contract_path, 'eosio.token/eosio.token.wasm')
        # abi_path = os.path.join(contract_path, 'eosio.token/eosio.token.abi')
        code_path = os.path.join(test_dir, 'contracts/eosio.token.wasm')
        abi_path = os.path.join(test_dir, 'contracts/eosio.token.abi')
        with open(code_path, 'rb') as f:
            code = f.read()
        with open(abi_path, 'rb') as f:
            abi = f.read()
        self.deploy_contract('eosio.token', code, abi)

    def deploy_eosio_system_uuos(self):
        code_path = os.path.join(test_dir, 'contracts/eosio.system.uuos.wasm')
        abi_path = os.path.join(test_dir, 'contracts/eosio.system.uuos.abi')
        with open(code_path, 'rb') as f:
            code = f.read()
        with open(abi_path, 'rb') as f:
            abi = f.read()
        self.deploy_contract('eosio', code, abi)

    def deploy_eosio_system(self):
        code_path = os.path.join(test_dir, 'contracts/eosio.system.wasm')
        abi_path = os.path.join(test_dir, 'contracts/eosio.system.abi')
        with open(code_path, 'rb') as f:
            code = f.read()
        with open(abi_path, 'rb') as f:
            abi = f.read()
        self.deploy_contract('eosio', code, abi)

    def deploy_eosio_bios(self):
        # contract_path = os.path.join(test_dir, '../../..', 'build/externals/eosio.contracts/contracts')
        # code_path = os.path.join(contract_path, 'eosio.bios/eosio.bios.wasm')
        # abi_path = os.path.join(contract_path, 'eosio.bios/eosio.bios.abi')
        code_path = os.path.join(test_dir, 'contracts/eosio.bios.wasm')
        abi_path = os.path.join(test_dir, 'contracts/eosio.bios.abi')
        with open(code_path, 'rb') as f:
            code = f.read()
        with open(abi_path, 'rb') as f:
            abi = f.read()
        self.deploy_contract('eosio', code, abi)

    def gen_trx(self):
        chain_id = self.chain.id()
        ref_block_id = self.chain.last_irreversible_block_id()
        priv_key = '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3'
        actions = []
        action = {
            'account':'eosio',
            'name':'sayhello',
            'data':'aabbcc',
            'authorization':[{'actor':'eosio', 'permission':'active'}]
        }
        actions.append(action)
        actions = json.dumps(actions)
        expiration = datetime.utcnow() + timedelta(seconds=60)
        expiration = isoformat(expiration)
        r = self.chain.gen_transaction(actions, expiration, ref_block_id, chain_id, False, priv_key)
#        logger.info(r)
        return r

        logger.info(r)
        r = PackedTransactionMessage.unpack(r)
        logger.info(r)

    def start_block_test(self):
        for i in range(10):
            self.chain.abort_block()
            self.chain.start_block(isoformat(self.calc_pending_block_time()))
            trx = self.gen_trx()
            deadline = datetime.utcnow() + timedelta(microseconds=30000)
            billed_cpu_time_us = 2000
            ret, result = self.chain.push_transaction(trx, isoformat(deadline), billed_cpu_time_us)
            print(ret, result)
            trxs = self.chain.get_unapplied_transactions()
            print(trxs)
            self.chain.finalize_block(['5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3'])
            self.chain.commit_block()
            print('head_block_num:', self.chain.head_block_num())

    def start_block(self):
        self.chain.abort_block()
#        num = self.chain.head_block_num() - self.chain.last_irreversible_block_num()

        self.chain.start_block(isoformat(self.calc_pending_block_time()), 0, self.feature_digests)
        self.feature_digests.clear()



    def produce_block(self):
        self.chain.finalize_block(['5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3'])
        self.chain.commit_block()
        self.start_block()

    def test_create_account_uuos(self):
        # '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc',#EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        self.create_account('eosio', 'testtesttest1', key, key, 10*1024, 1, 10)
        params = {'account_name':'testtesttest1'}
        params = json.dumps(params)
        ret, result = self.chain_api.get_account(params)
        assert ret, result
        result = JsonObject(result)
        # logger.info(result)

    def test_create_account(self):
        # '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc',#EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        self.create_account('eosio', 'testtesttest', key, key, 10*1024, 1, 10)
        params = {'account_name':'testtesttest'}
        params = json.dumps(params)
        ret, result = self.chain_api.get_account(params)
        assert ret, result
        result = JsonObject(result)
        # logger.info(result)

    def test1(self):
        logger.info('++++++++++++++test1+++++++++++++++')
        return
        # datetime.strptime('2020-01-08T12:07:18.669513', "%Y-%m-%dT%H:%M:%S.%f")
        # t = datetime.strptime("2018-06-01T12:00:00.000", "%Y-%m-%dT%H:%M:%S.%f")
        # print('+++datetime:', t)
        # t += timedelta(milliseconds=500)
        # print(os.getpid())
        # # input('>>>')
        # print('++++t:', t.isoformat())
        # print('++++self._chain', self.chain)
        # print('head_block_num:', self.chain.head_block_num())
        # print('++++head_block_time:', self.chain.head_block_time())
        skip_time = timedelta(microseconds=500)
        head_time = self.chain.head_block_time()
        next_time = head_time + skip_time
        producer = self.chain.get_scheduled_producer(next_time)
        print(producer)
        self.start_block()
        # self.producer_block(skip_time)

    def test2(self):
        self.calc_pending_block_time()

    def test3(self):
        params = {'account_name':'eosio.token'}
        params = json.dumps(params)
        ret = self.chain_api.get_code_hash(params)
        print('++++code_hash', ret)

        # ret = self.chain.get_account('eosio.token')
        # print(ret)

        r = self.push_action('eosio.token', 'transfer', {"from":"eosio", "to":"uuos","quantity":f"1.0000 {self.main_token}","memo":""}, 'eosio', 'active')
        # print(r)
    # struct get_currency_balance_params {
    #   name             code;
    #   name             account;
    #   optional<string> symbol;
    # };
        params = {
            'code': 'eosio.token', 
            'account': 'eosio',
            'symbol': 'UUOS'
        }
        params = json.dumps(params)
        ret = self.chain_api.get_currency_balance(params)
        print(ret)

    def test4(self):
        logger.info('++++++++++++++++test4+++++++++++++')
        # key = 'EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV'
        # a = 'eosio.token'
        # r = self.create_account('eosio', a, key, key)
        self.produce_block()
        arg = {'account_name':'eosio.token'}
        arg = json.dumps(arg)
        a = self.chain_api.get_account(arg)
        print('++++get_account:', a)

        params = {'account_name':'helloworld11'}
        params = json.dumps(params)
        ret = self.chain_api.get_code_hash(params)
        print('++++code_hash', ret)

        params = {'account_name':'helloworld11'}
        params = json.dumps(params)
        ret = self.chain_api.get_raw_code_and_abi(params)
        print('++++code', ret)
        try:
            r = self.push_action('helloworld11', 'sayhello', b'', 'helloworld11')
        except Exception as e:
            print(e)
        self.produce_block()

    def test5(self):
        logger.info('+++++++++++++++test5++++++++++++++')
        code = '''
def apply(receiver, first_receiver, action):
    for i in range(10):
        pass
        #print('hello,world')
        '''
        code += '\n'
        for i in range(2000):
            code += f'    a{i} = dict(abc={i})\n'
        code = compile(code, "contract", 'exec')
        code = marshal.dumps(code)
        self.deploy_contract('helloworld11', code, b'', 1)
        r = self.push_action('helloworld11', 'sayhello', b'', 'helloworld11')
        r = JsonObject(r)
        logger.info(r)
        r = self.push_action('helloworld11', 'sayhello', b'1122', 'helloworld11')
        r = JsonObject(r)
        logger.info(r)
        self.produce_block()

    def test6(self):
        code = '''
def apply(receiver, code, action):
    print(read_action_data())
    require_recipient('bob')
    print('done!')
        '''
        code = compile(code, "contract", 'exec')
        code = marshal.dumps(code)
        self.deploy_contract('alice', code, b'', 1)

        code = '''
def apply(receiver, code, action):
    print(n2s(receiver), n2s(code), n2s(action))
    print(read_action_data())
        '''

        code = compile(code, "contract", 'exec')
        code = marshal.dumps(code)
        self.deploy_contract('bob', code, b'', 1)
        r = self.push_action('alice', 'sayhello', b'1122')
        self.produce_block()

    def test7(self):
        for i in range(2):
            for j in range(10):
                args = int.to_bytes(j, 4, 'little')
                r = self.push_action('helloworld11', 'sayhello', args, 'helloworld11')
                r = self.push_action('alice', 'sayhello', args)
            self.produce_block()

    def compile_py_code_from_file(self, code_file):
        db_test = os.path.join(test_dir, '..', 'test_contracts', code_file)
        with open(db_test, 'r') as f:
            code = f.read()
        code = compile(code, "contract", 'exec')
        code = marshal.dumps(code)
        return code

    def compile_py_code(self, code):
        code = compile(code, "contract", 'exec')
        code = marshal.dumps(code)
        return code

    def db_test1(self):
        logger.info('+++++++++++++++db_test1++++++++++++++')
        code = self.compile_py_code_from_file('db_test.py')
        self.deploy_contract('helloworld11', code, b'', 1)
        r = self.push_action('helloworld11', 'sayhello', b'', 'helloworld11')
        r = JsonObject(r)
#        print(r)
        self.produce_block()

    def db_test2(self):
        code = self.compile_py_code_from_file('db_test2.py')
        self.deploy_contract('helloworld11', code, b'', 1)
        r = self.push_action('helloworld11', 'store', b'')
        r = JsonObject(r)
        r = self.push_action('helloworld11', 'get', b'')
        r = JsonObject(r)

        r = self.push_action('helloworld11', 'update', b'')
        # logger.info(r)
        r = self.push_action('helloworld11', 'update', b'1')
        # logger.info(r)

#        print(r)
        self.produce_block()

    def db_test3(self):
        code = self.compile_py_code_from_file('db_test3.py')

        code = compile(code, "contract", 'exec')
        code = marshal.dumps(code)
        self.deploy_contract('helloworld11', code, b'', 1)
        r = self.push_action('helloworld11', 'store', b'')
        print('++++store: elapsed:', r.elapsed)
        r = self.push_action('helloworld11', 'get', b'')
        print('++++get: elapsed:', r.elapsed)

        r = self.push_action('helloworld11', 'update', b'')
        print('++++update: elapsed:', r.elapsed)
        r = self.push_action('helloworld11', 'update', b'1')
        print('++++update: elapsed:', r.elapsed)

#        print(r)
        self.produce_block()

    def test_set_action_return_value(self):
        code = '''
def apply(receiver, code, action):
    set_action_return_value(b'hello,world')
        '''
        code = compile(code, "contract", 'exec')
        code = marshal.dumps(code)
        self.deploy_contract('helloworld11', code, b'', 1)
        r = self.push_action('helloworld11', 'test', b'')
        return_value = r.action_traces[0]['receipt']['return_value']
        logger.info(f'+++return_value: {return_value}')
        assert return_value == b'hello,world'.hex()
#        print(r)

    def test_jit(self):
        wasm_file = os.path.join(test_dir, '../../../', 'build/unittests/test-contracts/test_uuos')
        wasm_file = os.path.join(wasm_file, 'test_uuos.wasm')
        abi_file = os.path.join(test_dir, '../../../', 'unittests/test-contracts/test_uuos')
        abi_file = os.path.join(abi_file, 'test_uuos.abi')

        with open(wasm_file, 'rb') as f:
            code = f.read()
        with open(abi_file, 'rb') as f:
            abi = f.read()
        self.deploy_contract('helloworld11', code, abi)
        self.produce_block()

        r = self.push_action('helloworld11', 'sayhello', b'1')
        logger.info(r.elapsed)

        r = self.push_action('helloworld11', 'sayhello', b'2')
        logger.info(r.elapsed)

        ret_value = r.action_traces[0]['receipt']['return_value']
        ret_value = bytes.fromhex(ret_value)
        logger.info(ret_value)
        self.produce_block()

    def free(self):
        self.chain.free()
        shutil.rmtree(self.options.config_dir)
        shutil.rmtree(self.options.data_dir)
