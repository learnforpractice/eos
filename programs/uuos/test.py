from uuos.nativeobject import *
from uuos import chainapi

import ujson

import unittest
import os
import base64
from uuos import chainapi

from pyeoskit import eosapi, wallet
eosapi.set_nodes(['http://127.0.0.1:8889'])

class Test(unittest.TestCase):

    def test_set_attr1(self):
        a = NativeObject({'a':1})
        a.a = 123

        b = NativeObject({'b':1})
        b.b = 123

    def test_recovery(self):
        backup_dir = chainapi.repair_log('dd/blocks', 0)
        print(backup_dir)
        old_reversible = os.path.join(backup_dir, 'reversible')
        new_reversible = os.path.join('dd/blocks', 'reversible')
        if os.path.exists(old_reversible):
            ret = chainapi.recover_reversible_blocks(old_reversible, new_reversible)
            if not ret:
                import shutil
                shutil.copytree(old_reversible, new_reversible)
            print(ret)

    @unittest.expectedFailure
    def test_set_attr2(self):
        a = NativeObject({'a':1})
        a.b = 123

    def test_controller_config(self):
        cfg = ControllerConfig(config)
        packed_cfg = cfg.pack()
        print(packed_cfg)

    #    packed_cfg = bytes.fromhex('00000000000000322f55736572732f6e6577776f726c642f6465762f75756f73322f6275696c642f70726f6772616d732f64642f626c6f636b73312f55736572732f6e6577776f726c642f6465762f75756f73322f6275696c642f70726f6772616d732f64642f73746174650000004000000000000000080000000000004015000000000000200000000000881300000200000000000000010000d0c752936d05000002c0ded2bc1f1305fb0faac5e6c03ee3a1924234985427b6167ca569d13df435cf0000100000000000e8030000000008000c000000f40100001400000064000000400d0300e8030000f049020064000000100e00005802000080533b0000100000040006000100000000000000000000000000000000000000000000000000000000000000000000e8030000')
    #    print(packed_cfg)
        cfg2 = ControllerConfig.unpack(packed_cfg)
        print(cfg2)

        self.assertTrue(cfg == cfg2, 'shoud be True')

    def test_chain_new(self):
        cfg = ControllerConfig(config)
        # "blocks_dir": "/Users/newworld/dev/uuos2/build/programs/dd/blocks",
        # "state_dir": "/Users/newworld/dev/uuos2/build/programs/dd/state",
        cfg.blocks_dir = 'dd/blocks'
        cfg.state_dir = 'dd/state'
        cfg = ujson.dumps(cfg)
        ptr = chain_new(cfg, 'cd')
        print(ptr)
        chain_free(ptr)

    def test_get_account(self):
        a = eosapi.get_account('eosio')
        print(a)

    def test_get_code(self):
        h = eosapi.get_code('eosio')
        print(h)

    def test_get_code_hash(self):
        h = eosapi.get_code_hash('eosio')
        print(h)

    def test_get_activated_protocol_features(self):
        h = eosapi.get_activated_protocol_features()
        print(h)

    def test_get_block(self):
        h = eosapi.get_block(1)
        print(h)

    def test_get_block_header_state(self):
        h = eosapi.get_block_header_state(1)
        print(h)

    def test_get_abi(self):
        h = eosapi.get_abi('eosio')
        print(h)

    def test_get_raw_code_and_abi(self):
        r = eosapi.get_raw_code_and_abi('eosio')
        print(r['abi'])
        print(r['wasm'])
        print(base64.b64decode(r['abi']))

    def test_get_raw_abi(self):
        r = eosapi.get_raw_abi('eosio')
        print(base64.b64decode(r['abi']))

    def test_get_table_rows(self):
        r = eosapi.get_table_rows(True, 'eosio', 'eosio', 'rammarket', 'RAMCORE', '', '', 10)
        print(r)
        r = eosapi.get_table_rows(True, 'eosio', 'eosio', 'global', 'global', '', '', 1)
        print(r)
        r = eosapi.get_table_rows(True, 'eosio.token', 'EOS', 'stat', 'EOS', '', '', 10)
        print(r)

    def test_get_table_by_scope(self):
        r = eosapi.get_table_by_scope('eosio', 'rammarket', "", "zzzzzzzzzzzzj")
        print(r)
        r = eosapi.get_table_by_scope('eosio.token', 'stat', "", "zzzzzzzzzzzzj")
        print(r)

    def test_get_currency_balance(self):
        r = eosapi.get_currency_balance('eosio.token', 'eosio', "EOS")
        print(r)

    def test_get_currency_stats(self):
        r = eosapi.get_currency_stats('eosio.token', "EOS")
        print(r)

    def test_get_producers(self):
        '''
            bool        json = false;
            string      lower_bound;
            uint32_t    limit = 50;
        '''
        r = eosapi.get_producers(False, "", 50)
        print(r)

    def test_get_producer_schedule(self):
        r = eosapi.get_producer_schedule()
        print(r)

    def test_push_transaction(self):
        if os.path.exists('test.wallet'):
            os.remove('test.wallet')
        psw = wallet.create('test')
        print(psw)
        print(eosapi.get_info())
        priv_keys = [
            '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3',#EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
            '5JEcwbckBCdmji5j8ZoMHLEUS8TqQiqBG1DRx1X9DN124GUok9s',#EOS61MgZLN7Frbc2J7giU7JdYjy2TqnfWFjZuLXvpHJoKzWAj7Nst
            '5JbDP55GXN7MLcNYKCnJtfKi9aD2HvHAdY7g8m67zFTAFkY1uBB',#EOS5JuNfuZPATy8oPz9KMZV2asKf9m8fb2bSzftvhW55FKQFakzFL
            '5K463ynhZoCDDa4RDcr63cUwWLTnKqmdcoTKTHBjqoKfv4u5V7p',#EOS8Znrtgwt8TfpmbVpTKvA2oB8Nqey625CLN8bCN3TEbgx86Dsvr
            '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc',#EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV
            '5KT26sGXAywAeUSrQjaRiX9uk9uDGNqC1CSojKByLMp7KRp8Ncw',#EOS8Ep2idd8FkvapNfgUwFCjHBG4EVNAjfUsRRqeghvq9E91tkDaj
        ]
        for priv_key in priv_keys:
            wallet.import_key('test', priv_key)

        account_name = 'helloworld11'
        r = eosapi.push_action(account_name, 'sayhello', 'hello,world', {account_name:'active'})
        print(r)

if __name__ == '__main__':
    unittest.main()


