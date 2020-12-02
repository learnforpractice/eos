import os
import sys
import json
import gc
import hashlib
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

import uuos
from uuos import application
from uuos import wasmcompiler

from chaintest import ChainTest

logger = application.get_logger(__name__)

#contents of test_genesis_accounts.bin
# eosio
# 0002c0ded2bc1f1305fb0faac5e6c03ee3a1924234985427b6167ca569d13df435cf
# eosio.jit
# 00036bfc5cc1e9215625a2f25fd1013cde6e7b9d6f294753cce498f575a2e734a700
# eosio.jitfee
# 00036bfc5cc1e9215625a2f25fd1013cde6e7b9d6f294753cce498f575a2e734a700
# eosio.prods
# 0002c0ded2bc1f1305fb0faac5e6c03ee3a1924234985427b6167ca569d13df435cf
# hello
# 00036bfc5cc1e9215625a2f25fd1013cde6e7b9d6f294753cce498f575a2e734a700
# helloworld11
# 00036bfc5cc1e9215625a2f25fd1013cde6e7b9d6f294753cce498f575a2e734a700
# helloworld12
# 00036bfc5cc1e9215625a2f25fd1013cde6e7b9d6f294753cce498f575a2e734a700
# uuos
# 00036bfc5cc1e9215625a2f25fd1013cde6e7b9d6f294753cce498f575a2e734a700
# uuos.fund
# 00028fd007815bb9a382a214827dcb2a3903a14e3f692b41d497d0ecc2c2b91e6b3d
# helloworld13
# 00036bfc5cc1e9215625a2f25fd1013cde6e7b9d6f294753cce498f575a2e734a700
# helloworld14
# 00036bfc5cc1e9215625a2f25fd1013cde6e7b9d6f294753cce498f575a2e734a700
# helloworld15
# 00036bfc5cc1e9215625a2f25fd1013cde6e7b9d6f294753cce498f575a2e734a700

def check_error(msg):
    def inner(func):
        def wrapper(*args, **kwargs):
            try:
                func(*args, **kwargs)
                logger.info((func, msg))
                assert 0, f"function {func} should throw error: {msg}!"
            except Exception as e:
                # logger.info(e)
                _except = e.args[0]['except']
                msg = _except['stack'][0]['data']['s']
                assert msg == msg
        return wrapper
    return inner

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.main_token = 'UUOS'
        cls.chain = ChainTest(network_type=1)
        
        accounts = [
            {
                "permission": {
                    "actor": "alice",
                    "permission": "uuos.code"
                },
                "weight": 1
            },
            {
                "permission": {
                    "actor": "bob",
                    "permission": "uuos.code"
                },
                "weight": 1
            },
        ]

        keys = [{
            "key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
            "weight": 1
        }]

        cls.chain.update_auth('alice', accounts, keys)

        accounts = [
            {
                "permission": {
                    "actor": "bob",
                    "permission": "uuos.code"
                },
                "weight": 1
            },
        ]

        keys = [{
            "key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
            "weight": 1
        }]

        cls.chain.update_auth('bob', accounts, keys)
        cls.chain.transfer('uuos', 'alice', 1000.0, '1')
        cls.chain.produce_block()

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        pass

    def teardown_method(self, method):
        pass

    def check_call_error(self, func, msg):
        try:
            func()
            assert 0, f"function {func} should throw error: {msg}!"
        except Exception as e:
            _except = e.args[0]['except']
            msg = _except['stack'][0]['data']['s']
            assert msg == msg

# data = open('/Users/newworld/dev/uuos3/programs/uuos/tests/pytest/test_genesis_accounts.bin', 'rb').read()
# for i in range(0, len(data), 42):
#     s = eosapi.n2s(int.from_bytes(data[i:i+8], 'little'))
#     print(s)

#    @check_error('account name does not exists on EOS mainnet')
    def create_account_test1(self):
        '''
        testcase for account does not exist on EOS network
        '''
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        account = 'testtesttest'
        self.chain.create_account('uuos', account, key, key, 10*1024, 1, 10)

        # key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        # account = '12341234'
        # self.chain.create_account('alice', account, key, key, 10*1024, 1, 10)
        # assert 0, 'code should not go here!'

    @check_error('public key not match!')
    def create_account_test2(self):
        '''
        test for account exists on EOS network, but with the wrong active key
        '''
        key = 'EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV'
        account = 'helloworld13'
        self.chain.create_account('uuos', account, key, key, 10*1024, 1, 10)

    def create_account_test3(self):
        '''
        test for account exists on EOS network
        '''
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        account = 'helloworld13'
        self.chain.create_account('uuos', account, key, key, 10*1024, 1, 10)

    def create_account_test4(self):
        '''
        testcase for account with 13 charactors
        '''
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        account = 'helloworld133'
        self.chain.create_account('uuos', account, key, key, 10*1024, 1, 10)

    def create_account_test5(self):
        '''
        create account with 8 charactors
        '''
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        account = '12341234'
        self.chain.create_account('alice', account, key, key, 10*1024, 1, 10)

    def create_account_test6(self):
        '''
        create account with 9 charactors
        '''
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        account = '123412345'
        self.chain.create_account('alice', account, key, key, 10*1024, 1, 10)

    @check_error('no active bid for name')
    def create_account_test7(self):
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        account = '123412'
        self.chain.create_account('alice', account, key, key, 10*1024, 1, 10)

    @check_error('no active bid for name')
    def create_account_test8(self):
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        account = '123412'
        self.chain.create_account('alice', account, key, key, 10*1024, 1, 10)

    @check_error('only suffix may create this account')
    def create_account_test9(self):
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        account = '123411.1'
        self.chain.create_account('alice', account, key, key, 10*1024, 1, 10)

    def test_create_account(self):
        self.create_account_test1()
        self.create_account_test2()
        self.create_account_test3()
        self.create_account_test4()
        self.create_account_test6()
        self.create_account_test7()
        self.create_account_test8()
        self.create_account_test9()

    @check_error('public key mismatch')
    def activate_account_test1(self):
    # '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3',#EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
        h = hashlib.sha256()
        h.update(b'activate helloworld14')
        private_key = '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3'
        sign = uuos.sign_digest(h.digest(), private_key)
        args = {
            "activator":"alice",
            "account":"helloworld14",
            "sign": sign
        }
        self.chain.push_action('uuos', 'activateacc', args, actor='alice', perm='active')

    def activate_account_test2(self):
# '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc',#EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV
        h = hashlib.sha256()
        h.update(b'activate helloworld11')
        private_key = '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc'
        sign = uuos.sign_digest(h.digest(), private_key)
        args = {
            "activator":"alice",
            "account":"helloworld11",
            "sign": sign
        }
        #"ram_usage":2916
        old_ram_usage = self.chain.get_account('alice').ram_usage
        self.chain.push_action('uuos', 'activateacc', args, actor='alice', perm='active')

        new_ram_usage = self.chain.get_account('alice').ram_usage
        logger.info('++++%s %s', new_ram_usage, old_ram_usage)
        assert new_ram_usage - old_ram_usage == 240

    @check_error('account not registered')
    def activate_account_test3(self):
# '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc',#EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV
        h = hashlib.sha256()
        h.update(b'activate helloworld51')
        private_key = '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc'
        sign = uuos.sign_digest(h.digest(), private_key)
        args = {
            "activator":"alice",
            "account":"helloworld51",
            "sign": sign
        }
        self.chain.push_action('uuos', 'activateacc', args, actor='alice', perm='active')

    def activate_account_test4(self):
# '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc',#EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV
        h = hashlib.sha256()
        h.update(b'activate helloworld12')
        private_key = '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc'
        sign = uuos.sign_digest(h.digest(), private_key)
        args = {
            "activator":"uuos",
            "account":"helloworld12",
            "sign": sign
        }
        try:
            self.chain.push_action('uuos', 'activateacc', args, actor='uuos', perm='active')
        except Exception as e:
            assert e.args[0]['except']['message'] == 'Missing required authority'

    def activate_account_test5(self):
# '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc',#EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV
        accounts = [ ]

        keys = [{
            "key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
            "weight": 1
        }]

        self.chain.update_auth('uuos', accounts, keys, perm='activateacc', parent='owner')

        args = {
            'account':'uuos',
            'code':'eosio',
            'type':'activateacc',
            'requirement':'activateacc'
        }
        self.chain.push_action('eosio', 'linkauth', args, actor='uuos', perm='active')

        h = hashlib.sha256()
        h.update(b'activate helloworld12')
        private_key = '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc'
        sign = uuos.sign_digest(h.digest(), private_key)
        args = {
            "activator":"uuos",
            "account":"helloworld12",
            "sign": sign
        }
        self.chain.push_action('eosio', 'activateacc', args, actor='uuos', perm='activateacc')

    def test_activate_account(self):
        self.activate_account_test1()
        self.activate_account_test2()
        self.activate_account_test3()
        self.activate_account_test4()
#        self.activate_account_test5()

    @check_error('no res found')
    def reg_producer_test1(self):
        args = {
            "producer": "alice",
            "producer_key": "EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV",
            "url": "https://uuos.io",
            "location": 1
        }

        self.chain.push_action('uuos', 'regproducer', args, 'alice', 'active')

    @check_error('no enough staking')
    def reg_producer_test2(self):
        '''
        stake less than 1000_000 tokens
        '''
        self.chain.buy_ram_bytes('uuos', 'alice', 60*1024)
        self.chain.delegatebw('uuos', 'alice', 1.0, 999998.0, transfer=0)
        args = {
            "producer": "alice",
            "producer_key": "EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV",
            "url": "https://uuos.io",
            "location": 1
        }

        self.chain.push_action('uuos', 'regproducer', args, 'alice', 'active')

    def reg_producer_test3(self):
        '''
        stake tokens equal to 1000000
        '''
        self.chain.buy_ram_bytes('uuos', 'bob', 60*1024)
        self.chain.delegatebw('uuos', 'bob', 1.0, 999999.0, transfer=0)
        args = {
            "producer": "bob",
            "producer_key": "EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV",
            "url": "https://uuos.io",
            "location": 1
        }

        self.chain.push_action('uuos', 'regproducer', args, 'bob', 'active')

    def test_reg_producer(self):
        self.reg_producer_test1()
        self.reg_producer_test2()
        self.reg_producer_test3()


