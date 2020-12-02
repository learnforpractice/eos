import os
import time
import json
import pytest
import logging
import subprocess

from chaintest import ChainTest
from uuos import log
from uuos import wasmcompiler
from pyeoskit import eosapi

from datetime import datetime, timedelta

test_dir = os.path.dirname(__file__)

logger = log.get_logger(__name__)

# print(os.getpid())
# input('<<<')

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.chain = ChainTest(network_type=1, jit=True)

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        logger.warning('test start: %s', method.__name__)

    def teardown_method(self, method):
        logger.warning('test end: %s', method.__name__)

    def test_get_scheduled_transactions(self):
        a = {
            "account": 'alice',
            "permission": "active",
            "parent": "owner",
            "auth": {
                "threshold": 1,
                "keys": [
                    {
                        "key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
                        "weight": 1
                    },
                ],
                "accounts": [{"permission":{"actor":'alice',"permission":"uuos.code"},"weight":1}],
                "waits": []
            }
        }

        r = self.chain.push_action(self.chain.system_contract, 'updateauth', a, actor='alice')

        code_file = os.path.join(test_dir, 'test_contracts', 'test_send_deffered.cpp')
        wasm_file = os.path.join(test_dir, 'test_contracts', 'test_send_deffered.wasm')
        need_compile = True
        try:
            t1 = os.path.getmtime(code_file)
            t2 = os.path.getmtime(wasm_file)
            if t1 < t2:
                need_compile = False
        except:
            pass
        code = None
        if need_compile:
            code = wasmcompiler.compile_cpp_file(code_file)
        else:
            with open(wasm_file, 'rb') as f:
                code = f.read()

        self.chain.deploy_contract('alice', code, b'')
    
        r = self.chain.push_action('alice', 'sayhello', b'hello,world')
        self.chain.produce_block()

        trxs = self.chain.chain.get_scheduled_transactions()
        logger.info(trxs)

        old_balance = self.chain.get_balance('alice')
        for _ in range(3):
            self.chain.produce_block()
        assert old_balance - self.chain.get_balance('alice') == 1.0

