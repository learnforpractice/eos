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

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from datetime import datetime, timedelta
from uuos.jsonobject import JsonObject
from uuos.nativeobject import SignedBlockMessage, PackedTransactionMessage

from uuos import config

from uuos.chain import Chain
from uuos.chainapi import ChainApi

from uuos import nativeobject
from uuos import application
import uuos

gc.set_debug(gc.DEBUG_STATS)

logger = application.get_logger(__name__)

def isoformat(dt):
    return dt.isoformat(timespec='milliseconds')

#logger = LogWrapper(logger)

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

class ChainTest(object):

    def __init__(self):
        logger.info('+++++++++init+++++++++++')
        options = Object()
        options.data_dir = tempfile.mkdtemp()
        options.config_dir = tempfile.mkdtemp()

        print(options.data_dir, options.config_dir)

        options.chain_state_db_size_mb = 50
        options.contracts_console = True
        options.uuos_mainnet = False
        options.network = 'test'
        options.snapshot = ''

        self._chain = None

        chain_cfg = nativeobject.ControllerConfig(config.default_config)
        chain_cfg.contracts_console = options.contracts_console
        chain_cfg.blocks_dir = os.path.join(options.data_dir, 'blocks')
        chain_cfg.state_dir = os.path.join(options.data_dir, 'state')
#
        chain_cfg.state_guard_size = 5*1024*1024
        chain_cfg.reversible_cache_size = 50*1024*1024
        chain_cfg.reversible_guard_size = 5*1024*1024


        uuos.set_default_data_dir(options.data_dir)
        uuos.set_default_config_dir(options.config_dir)

        chain_cfg.state_size = options.chain_state_db_size_mb * 1024 * 1024

        print('config.uuos_mainnet', options.uuos_mainnet)
        chain_cfg.uuos_mainnet = False

        self.options = options
        if self.options.network == 'uuos':
            chain_cfg.uuos_mainnet = True
            chain_cfg.genesis = genesis_uuos
        elif self.options.network == 'eos':
            chain_cfg.genesis = genesis_eos
        elif self.options.network == 'test':
            pass
        else:
            raise Exception('unknown network')

        chain_cfg = chain_cfg.dumps()
        logger.info(chain_cfg)
        self._chain = Chain(chain_cfg, options.config_dir, options.snapshot)

        self.init()
        
        self.chain_api = ChainApi(self.chain.ptr)

    def init(self):
        self.chain.startup()
        uuos.set_accepted_block_callback(self.on_accepted_block)
        self.start_block()
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
                'helloworld11'
        ]
        for a in systemAccounts:
            self.create_account('eosio', a, key, key)

    @property
    def chain(self):
        return self._chain

    def on_accepted_block(self, block, num, block_id):
        pass
        # msg = SignedBlockMessage.unpack(block)
        # print(msg)

    def calc_pending_block_time(self):
#        self.chain.abort_block()
        now = datetime.utcnow()
        base = self.chain.head_block_time()
        if base < now:
            base = now
        min_time_to_next_block = config.block_interval_us - int(base.timestamp()*1e6) % config.block_interval_us
        print('min_time_to_next_block:', min_time_to_next_block)
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
            args = self.chain.pack_action_args('eosio', 'newaccount', args)
        assert type(args) is bytes
        return {
            'account': accunt,
            'name': action,
            'data': args.hex(),
            'authorization':[{'actor': actor, 'permission': perm}]
        }

    def push_action(self, account, action, args, actor, perm='active'):
        if not isinstance(args, bytes):
            logger.info(f'{account}, {action}, {args}')
            args = self.chain.pack_action_args(account, action, args)
            # logger.error(f'++++{args}')
        action = {
            'account': account,
            'name': action,
            'data': args.hex(),
            'authorization':[{'actor': actor, 'permission': perm}]
        }
        return self.push_actions([action])

    def push_actions(self, actions):
        chain_id = self.chain.id()
        ref_block_id = self.chain.last_irreversible_block_id()
        priv_key = '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3'
        actions = json.dumps(actions)
        expiration = datetime.utcnow() + timedelta(seconds=60)
        expiration = isoformat(expiration)
        raw_signed_trx = self.chain.gen_transaction(actions, expiration, ref_block_id, chain_id, False, priv_key)
#        print(PackedTransactionMessage.unpack(raw_signed_trx))
        deadline = datetime.utcnow() + timedelta(microseconds=30000)
        billed_cpu_time_us = 2000
        ret, result = self.chain.push_transaction(raw_signed_trx, isoformat(deadline), billed_cpu_time_us)
#        print(ret, result)
        if not ret:
            result = json.loads(result)
            raise Exception(result['except'])
        return result

    def create_account(self, creator, account, owner_key, active_key):
        actions = []
        logger.info(f'{creator} {account}')
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
        newaccount_args = self.chain.pack_action_args('eosio', 'newaccount', args)
        if not newaccount_args:
            raise Exception('bad args')
        newaccount_action = {
            'account': 'eosio',
            'name': 'newaccount',
            'data': newaccount_args.hex(),
            'authorization':[{'actor':'eosio', 'permission':'active'}]
        }
        actions.append(newaccount_action)
        return self.push_actions(actions)

    def deploy_contract(self, account, code, abi):
        actions = []
        setcode = {"account": account,
                   "vmtype": 0,
                   "vmversion": 0,
                   "code": code.hex()
        }

        setcode_args = self.chain.pack_action_args(account, 'setcode', setcode)
        actions = []
        setcode_action = {
            'account': 'eosio',
            'name': 'setcode',
            'data': setcode_args.hex(),
            'authorization':[{'actor': account, 'permission':'active'}]
        }
        actions.append(setcode)

        abi = nativeobject.pack_native_object(nativeobject.abi_def_type, abi)
        setabi_args = self.chain.pack_action_args('eosio', 'setabi', {'account':account, 'abi':abi.hex()})
        setabi_action = {
            'account': 'eosio',
            'name': 'setabi',
            'data': setabi_args.hex(),
            'authorization':[{'actor': account, 'permission':'active'}]
        }
        actions.append(setabi_action)
        self.push_actions(actions)

    def deploy_eosio_token(self):
        contract_path = '/Users/newworld/dev/eos/build/contracts'
        if platform.system() == 'Linux':
            contract_path = '/home/newworld/dev/uuos2/build/externals/eosio.contracts/contracts'
        else:
            contract_path = '/Users/newworld/dev/uuos2/build/externals/eosio.contracts/contracts'

        code_path = os.path.join(contract_path, 'eosio.token/eosio.token.wasm')
        abi_path = os.path.join(contract_path, 'eosio.token/eosio.token.abi')
        with open(code_path, 'rb') as f:
            code = f.read()
        with open(abi_path, 'rb') as f:
            abi = f.read()
        self.deploy_contract('eosio.token', code, abi)

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
#        print(r)
        return r

        print(r)
        r = PackedTransactionMessage.unpack(r)
        print(r)

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
            self.chain.finalize_block('5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3')
            self.chain.commit_block()
            print('head_block_num:', self.chain.head_block_num())

    def start_block(self):
        self.chain.abort_block()
        self.chain.start_block(isoformat(self.calc_pending_block_time()))

    def produce_block(self):
        self.chain.finalize_block('5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3')
        self.chain.commit_block()
        self.start_block()

    def test1(self):
        logger.info('++++++++++++++test1+++++++++++++++')
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
        main_token = 'UUOS'

        logger.info('deploy eosio.token')
        self.deploy_eosio_token()
        self.produce_block()

        logger.info('issue system token...')

        args = {"issuer":"eosio", "maximum_supply":f"11000000000.0000 {main_token}"}
        r = self.push_action('eosio.token', 'create', args, 'eosio.token', 'active')

        args = {"to":"eosio","quantity":f"1000000000.0000 {main_token}", "memo":""}
        r = self.push_action('eosio.token','issue', args, 'eosio', 'active')
        self.produce_block()

    def test4(self):
        # key = 'EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV'
        # a = 'eosio.token'
        # r = self.create_account('eosio', a, key, key)
        self.produce_block()
        arg = {'account_name':'eosio.token'}
        arg = json.dumps(arg)
        a = self.chain_api.get_account(arg)
        print('++++get_account:', a)

    def free(self):
        self.chain.free()
        shutil.rmtree(self.options.config_dir)
        shutil.rmtree(self.options.data_dir)

class UUOSTester(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        super(UUOSTester, self).__init__(testName)
        self.extra_args = extra_args
        self.chain = ChainTest()
        UUOSTester.chain = self.chain

    def test1(self):
        self.chain.test1()
        # datetime.datetime.utcnow().isoformat()

    def test2(self):
        self.chain.test2()

    def test3(self):
        self.chain.test3()
    
    def test4(self):
        self.chain.test4()

    @classmethod
    def setUpClass(cls):
        pass

    @staticmethod
    def disconnect():
        pass

    @classmethod
    def tearDownClass(cls):
        if UUOSTester.chain:
            UUOSTester.chain.free()
            UUOSTester.chain = None

if __name__ == '__main__':
    unittest.main()
