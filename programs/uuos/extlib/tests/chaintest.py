import os
import json
import pytest
import logging
import tempfile
from uuosio import chain, chainapi, uuos, config
from datetime import datetime, timedelta

import log

logger = log.get_logger(__name__)

data_dir = tempfile.mkdtemp()
config_dir = tempfile.mkdtemp()

chain_config = {
    'sender_bypass_whiteblacklist': [],
    'actor_whitelist': [],
    'actor_blacklist': [],
    'contract_whitelist': [],
    'contract_blacklist': [],
    'action_blacklist': [],
    'key_blacklist': [],
    'blog': {
      'log_dir': os.path.join(data_dir, 'blocks'),
      'retained_dir': '',
      'archive_dir': 'archive',
      'stride': 4294967295,
      'max_retained_files': 10,
      'fix_irreversible_blocks': True
    },
    'state_dir': os.path.join(data_dir, 'state'),
    'state_size': 2147483648,
    'state_guard_size': 134217728,
    'reversible_cache_size': 356515840,
    'reversible_guard_size': 2097152,
    'sig_cpu_bill_pct': 5000,
    'thread_pool_size': 2,
    'max_retained_block_files': 10,
    'blocks_log_stride': 4294967295,
    'backing_store': 0,
    'persistent_storage_num_threads': 1,
    'persistent_storage_max_num_files': -1,
    'persistent_storage_write_buffer_size': 134217728,
    'persistent_storage_bytes_per_sync': 1048576,
    'persistent_storage_mbytes_batch': 50,
    'abi_serializer_max_time_us': 15000000,
    'max_nonprivileged_inline_action_size': 4096,
    'read_only': False,
    'force_all_checks': False,
    'disable_replay_opts': False,
    'contracts_console': True,
    'allow_ram_billing_in_notify': False,
    'maximum_variable_signature_length': 16384,
    'disable_all_subjective_mitigations': False,
    'terminate_at_block': 0,
    'wasm_runtime': 'eos_vm_jit',
    'eosvmoc_config': {'cache_size': 1073741824, 'threads': 1},
    'eosvmoc_tierup': False,
    'read_mode': 'SPECULATIVE',
    'block_validation_mode': 'FULL',
    'db_map_mode': 'mapped',
    'resource_greylist': [],
    'trusted_producers': [],
    'greylist_limit': 1000
 }

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
    "max_block_cpu_usage": 800000,
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

genesis_test = {
  "initial_timestamp": "2018-06-01T12:00:00.000",
  "initial_key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
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
    "max_inline_action_size": 524288,
    "max_inline_action_depth": 4,
    "max_authority_depth": 6,
    "max_action_return_value_size": 256,
  }
}

key_map = {
    'EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV':'5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3',
    'EOS61MgZLN7Frbc2J7giU7JdYjy2TqnfWFjZuLXvpHJoKzWAj7Nst':'5JEcwbckBCdmji5j8ZoMHLEUS8TqQiqBG1DRx1X9DN124GUok9s',
    'EOS5JuNfuZPATy8oPz9KMZV2asKf9m8fb2bSzftvhW55FKQFakzFL':'5JbDP55GXN7MLcNYKCnJtfKi9aD2HvHAdY7g8m67zFTAFkY1uBB',
    'EOS8Znrtgwt8TfpmbVpTKvA2oB8Nqey625CLN8bCN3TEbgx86Dsvr':'5K463ynhZoCDDa4RDcr63cUwWLTnKqmdcoTKTHBjqoKfv4u5V7p',
    'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV':'5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc',
    'EOS8Ep2idd8FkvapNfgUwFCjHBG4EVNAjfUsRRqeghvq9E91tkDaj':'5KT26sGXAywAeUSrQjaRiX9uk9uDGNqC1CSojKByLMp7KRp8Ncw',

    'EOS6AjF6hvF7GSuSd4sCgfPKq5uWaXvGM2aQtEUCwmEHygQaqxBSV':'5JRYimgLBrRLCBAcjHUWCYRv3asNedTYYzVgmiU4q2ZVxMBiJXL',
    'EOS7sPDxfw5yx5SZgQcVb57zS1XeSWLNpQKhaGjjy2qe61BrAQ49o':'5Jbb4wuwz8MAzTB9FJNmrVYGXo4ABb7wqPVoWGcZ6x8V2FwNeDo',
    'EOS89jesRgvvnFVuNtLg4rkFXcBg2Qq26wjzppssdHj2a8PSoWMhx':'5JHRxntHapUryUetZgWdd3cg6BrpZLMJdqhhXnMaZiiT4qdJPhv',
    'EOS73ECcVHVWvuxJVm5ATnqBTCFMtA6WUsdDovdWH5NFHaXNq1hw1':'5Jbh1Dn57DKPUHQ6F6eExX55S2nSFNxZhpZUxNYFjJ1arKGK9Q3',
    'EOS8h8TmXCU7Pzo5XQKqyWwXAqLpPj4DPZCv5Wx9Y4yjRrB6R64ok':'5JJYrXzjt47UjHyo3ud5rVnNEPTCqWvf73yWHtVHtB1gsxtComG',
    'EOS65jj8NPh2EzLwje3YRy4utVAATthteZyhQabpQubxHNJ44mem9':'5J9PozRVudGYf2D4b8JzvGxPBswYbtJioiuvYaiXWDYaihNFGKP',
    'EOS5fVw435RSwW3YYWAX9qz548JFTWuFiBcHT3PGLryWaAMmxgjp1':'5K9AZWR2wEwtZii52vHigrxcSwCzLhhJbNpdXpVFKHP5fgFG5Tx'
}

producer_key_map = {
    'UUOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV':'5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3',
    'UUOS61MgZLN7Frbc2J7giU7JdYjy2TqnfWFjZuLXvpHJoKzWAj7Nst':'5JEcwbckBCdmji5j8ZoMHLEUS8TqQiqBG1DRx1X9DN124GUok9s',
    'UUOS5JuNfuZPATy8oPz9KMZV2asKf9m8fb2bSzftvhW55FKQFakzFL':'5JbDP55GXN7MLcNYKCnJtfKi9aD2HvHAdY7g8m67zFTAFkY1uBB',
    'UUOS8Znrtgwt8TfpmbVpTKvA2oB8Nqey625CLN8bCN3TEbgx86Dsvr':'5K463ynhZoCDDa4RDcr63cUwWLTnKqmdcoTKTHBjqoKfv4u5V7p',
    'UUOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV':'5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc',
    'UUOS8Ep2idd8FkvapNfgUwFCjHBG4EVNAjfUsRRqeghvq9E91tkDaj':'5KT26sGXAywAeUSrQjaRiX9uk9uDGNqC1CSojKByLMp7KRp8Ncw',

    'UUOS6AjF6hvF7GSuSd4sCgfPKq5uWaXvGM2aQtEUCwmEHygQaqxBSV':'5JRYimgLBrRLCBAcjHUWCYRv3asNedTYYzVgmiU4q2ZVxMBiJXL',
    'UUOS7sPDxfw5yx5SZgQcVb57zS1XeSWLNpQKhaGjjy2qe61BrAQ49o':'5Jbb4wuwz8MAzTB9FJNmrVYGXo4ABb7wqPVoWGcZ6x8V2FwNeDo',
    'UUOS89jesRgvvnFVuNtLg4rkFXcBg2Qq26wjzppssdHj2a8PSoWMhx':'5JHRxntHapUryUetZgWdd3cg6BrpZLMJdqhhXnMaZiiT4qdJPhv',
    'UUOS73ECcVHVWvuxJVm5ATnqBTCFMtA6WUsdDovdWH5NFHaXNq1hw1':'5Jbh1Dn57DKPUHQ6F6eExX55S2nSFNxZhpZUxNYFjJ1arKGK9Q3',
    'UUOS8h8TmXCU7Pzo5XQKqyWwXAqLpPj4DPZCv5Wx9Y4yjRrB6R64ok':'5JJYrXzjt47UjHyo3ud5rVnNEPTCqWvf73yWHtVHtB1gsxtComG',
    'UUOS65jj8NPh2EzLwje3YRy4utVAATthteZyhQabpQubxHNJ44mem9':'5J9PozRVudGYf2D4b8JzvGxPBswYbtJioiuvYaiXWDYaihNFGKP',
    'UUOS5fVw435RSwW3YYWAX9qz548JFTWuFiBcHT3PGLryWaAMmxgjp1':'5K9AZWR2wEwtZii52vHigrxcSwCzLhhJbNpdXpVFKHP5fgFG5Tx'
}

class ChainTest(object):

    def __init__(self):
        uuos.set_log_level('default', 0)
        uuos.set_block_interval_ms(1000)

        self.chain_config = json.dumps(chain_config)
        self.genesis_test = json.dumps(genesis_test)
        uuos.set_log_level('default', 10)
        self.chain = chain.Chain(self.chain_config, self.genesis_test, os.path.join(config_dir, "protocol_features"), "")
        self.chain.startup(True)
        self.api = chainapi.ChainApi(self.chain.ptr)

        # logger.info(self.api.get_info())
        # logger.info(self.api.get_account('eosio'))

        self.feature_digests = []
        uuos.set_log_level('default', 0)

        self.feature_digests = ['0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd']
        self.start_block()

    def start_block(self):
        self.chain.start_block(self.calc_pending_block_time(), 0, self.feature_digests)

    def free(self):
        self.chain.free()

    def find_private_key(self, actor, perm_name):
        ret, result = self.api.get_account(actor)
        if not ret:
            return None
        keys = []
        for permission in result['permissions']:
            logger.info("%s %s %s", actor, perm_name, permission)
            if permission['perm_name'] == perm_name:
                for key in permission['required_auth']['keys']:
                    pub_key = key['key']
                    pub_key = pub_key.replace('UUOS', 'EOS', 1)
                    if pub_key in key_map:
                        priv_key = key_map[pub_key]
                        keys.append(priv_key)
        return keys

    def push_action(self, account, action, args, permissions={}):
        auth = []
        for actor in permissions:
            perm = permissions[actor]
            auth.append({'actor': actor, 'permission': perm})
        if not auth:
            auth.append({'actor': account, 'permission': 'active'})

        logger.debug(f'{account}, {action}, {args}')
        if not isinstance(args, bytes):
            _args = self.chain.pack_action_args(account, action, args)
            if not _args:
                error = self.chain.get_last_error()
                raise Exception(f'{error}')
            args = _args
            # logger.error(f'++++{args}')
        a = {
            'account': account,
            'name': action,
            'data': args.hex(),
            'authorization': auth
        }
        ret = self.push_actions([a])
        elapsed = ret['elapsed']
        if not action == 'activate':
            logger.info(f'+++++{account} {action} {elapsed}')
        return ret

    def push_actions(self, actions):
        chain_id = self.chain.id()
        ref_block_id = self.chain.last_irreversible_block_id()
        priv_keys = []
        for act in actions:
            for author in act['authorization']:
                keys = self.find_private_key(author['actor'], author['permission'])
                for key in keys:
                    if not key in priv_keys:
                        priv_keys.append(key)
        assert len(priv_keys) >= 1

        priv_keys = json.dumps(priv_keys)
#        priv_key = '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3'

        actions = json.dumps(actions)
        expiration = datetime.utcnow() + timedelta(seconds=60*60)

#        expiration = self.chain.fork_db_pending_head_block_time()
#        expiration = datetime.strptime(expiration, "%Y-%m-%dT%H:%M:%S.%f")
#        expiration = expiration + timedelta(seconds=60*60)
        raw_signed_trx = self.chain.gen_transaction(actions, expiration, ref_block_id, chain_id, False, priv_keys)
        # signed_trx = PackedTransactionMessage.unpack(raw_signed_trx)
        # logger.info(signed_trx)
        # r = uuos.unpack_native_object(13, bytes.fromhex(signed_trx.packed_trx))
        # logger.info(r)

        deadline = datetime.utcnow() + timedelta(microseconds=10000000)
        billed_cpu_time_us = 100
        result = self.chain.push_transaction(raw_signed_trx, deadline, billed_cpu_time_us)
        return result

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

    def start_block(self):
        self.chain.abort_block()
        self.chain.start_block(self.calc_pending_block_time(), 0, self.feature_digests)
        self.feature_digests.clear()

    def produce_block(self):
        trxs = self.chain.get_scheduled_transactions()
        deadline = datetime.utcnow() + timedelta(microseconds=10000000)
        priv_keys = []
        for pub_key in self.chain.get_producer_public_keys():
            if pub_key in producer_key_map:
                priv_keys.append(producer_key_map[pub_key])

        for scheduled_tx_id in trxs:
            self.chain.push_scheduled_transaction(scheduled_tx_id, deadline, 100)
        self.chain.finalize_block(priv_keys)
        self.chain.commit_block()
        self.start_block()

    def deploy_contract(self, account, code, abi, vm_type=0, show_elapse=True):
        actions = []
        setcode = {"account": account,
                   "vmtype": vm_type,
                   "vmversion": 0,
                   "code": code.hex()
        }

        setcode = self.chain.pack_action_args('uuos', 'setcode', setcode)
        actions = []
        setcode = {
            'account': 'uuos',
            'name': 'setcode',
            'data': setcode.hex(),
            'authorization':[{'actor': account, 'permission':'active'}]
        }
        actions.append(setcode)
        if abi:
            abi = nativeobject.pack_native_object(nativeobject.abi_def_type, abi)
        setabi = self.chain.pack_action_args('uuos', 'setabi', {'account':account, 'abi':abi.hex()})
        setabi = {
            'account': 'uuos',
            'name': 'setabi',
            'data': setabi.hex(),
            'authorization':[{'actor': account, 'permission':'active'}]
        }
        actions.append(setabi)
        # logger.info(actions)
        ret = self.push_actions(actions)
        elapsed = ret.elapsed
        if show_elapse and code:
            logger.info(f'+++++deploy contract: {account} {elapsed}')
        # logger.info(ret)
        self.chain.clear_abi_cache(account)
        return ret

