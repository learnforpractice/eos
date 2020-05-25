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
                assert 0, f"function {func} should throw error: {msg}!"
            except Exception as e:
                _except = e.args[0]['except']
                msg = _except['stack'][0]['data']['s']
                assert msg == msg
        return wrapper
    return inner

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.main_token = 'UUOS'
        cls.chain = ChainTest(uuos_network=True)
        accounts = [
            {
                "permission": {
                    "actor": "alice",
                    "permission": "eosio.code"
                },
                "weight": 1
            },
            {
                "permission": {
                    "actor": "bob",
                    "permission": "eosio.code"
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
                    "permission": "eosio.code"
                },
                "weight": 1
            },
        ]

        keys = [{
            "key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
            "weight": 1
        }]

        cls.chain.update_auth('bob', accounts, keys)

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        logger.info("starting execution of tc: {}".format(method.__name__))

    def teardown_method(self, method):
        logger.info("Ending execution of tc: {}".format(method.__name__))

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

    @check_error('account name does not exists on EOS mainnet')
    def create_account_test1(self):
        '''
        testcase for account does not exist on EOS network
        '''
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        account = 'testtesttest'
        self.chain.create_account('eosio', account, key, key, 10*1024, 1, 10)

    @check_error('public key not match!')
    def create_account_test2(self):
        '''
        test for account exists on EOS network, but with the wrong active key
        '''
        key = 'EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV'
        account = 'helloworld12'
        self.chain.create_account('eosio', account, key, key, 10*1024, 1, 10)

    def create_account_test3(self):
        '''
        test for account exists on EOS network
        '''
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        account = 'helloworld12'
        self.chain.create_account('eosio', account, key, key, 10*1024, 1, 10)

    def create_account_test4(self):
        '''
        testcase for account with 13 charactors
        '''
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        account = 'helloworld123'
        self.chain.create_account('eosio', account, key, key, 10*1024, 1, 10)

    def test_create_account(self):
        self.create_account_test1()
        self.create_account_test2()
        self.create_account_test3()
        self.create_account_test4()

    def test_activate_account(self):
# '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc',#EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV
        h = hashlib.sha256()
        h.update(b'activate helloworld13')
        private_key = '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc'
        sign = uuos.sign_digest(h.digest(), private_key)
        args = {
            "activator":"alice",
            "account":"helloworld13",
            "sign": sign
        }
        self.chain.push_action('eosio', 'activateacc', args, actor='alice', perm='active')
