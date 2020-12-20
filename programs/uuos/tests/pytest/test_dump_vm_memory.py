import os
import sys
import time
import json
import pytest
import logging
import subprocess
import shutil
import gc

from chaintest import ChainTest
import uuos
from uuos import log
from uuos import wasmcompiler
from pyeoskit import eosapi

test_dir = os.path.dirname(__file__)

logger = log.get_logger(__name__)

# print(os.getpid())
# input('<<<')

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.chain = ChainTest(network_type=1, jit=False)
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

        cls.chain.push_action(cls.chain.system_contract, 'updateauth', a, actor='alice')

        cls.chain.buy_ram_bytes('alice', 'alice', 256*1024)
        cls.chain.buy_ram_bytes('alice', 'bob', 256*1024)

        cls.chain.delegatebw('alice', 'alice', 1.0, 1.0)
        cls.chain.delegatebw('alice', 'bob', 1.0, 1.0)
        cls.chain.produce_block()

    @classmethod
    def teardown_class(cls):
        cls.chain.free()
        shutil.rmtree(cls.chain.options.config_dir)
        shutil.rmtree(cls.chain.options.data_dir)

    def setup_method(self, method):
        logger.warning('test start: %s', method.__name__)

    def teardown_method(self, method):
        try:
            self.chain.deploy_contract('alice', b'', b'', vm_type=1)
        except Exception as e:
            assert e.args[0]['except']['name'] == 'set_exact_code'
            assert e.args[0]['except']['message'] == 'Contract is already running this version of code'
        self.chain.produce_block()
        logger.warning('test end: %s', method.__name__)
        gc.collect()


    def test_memory_dump(self):
        # print(os.getpid())
        # input('<<<')
        files = ('after_apply_dump.bin', 'before_apply_dump.bin', 'out.wasm')
        for f in files:
            if os.path.exists(f):
                os.remove(f)

        self.chain.buy_ram_bytes('alice', 'bob', 3*1024*1024)
        wasm_file = '/Users/newworld/dev/uuos3/build/externals/micropython/ports/uuosio/micropython_eosio.wasm'
        with open(wasm_file, 'rb') as f:
            code = f.read()
        contract_name = 'bob'
        self.chain.deploy_contract(contract_name, code, b'')
        self.chain.produce_block()

        r = self.chain.push_action(contract_name, 'init', b'')
        logger.info('+++elapsed: %s', r['elapsed'])

        after_apply_dump = None
        with open('after_apply_dump.bin', 'rb') as f:
            after_apply_dump = f.read()

        cmd = './build/libraries/vm/vm_wasm/wasm_preloader ./build/externals/micropython/ports/uuosio/micropython_eosio.wasm after_apply_dump.bin out.wasm'
        os.system(cmd)

        self.chain.buy_ram_bytes('alice', 'bob', 3*1024*1024)
        wasm_file = 'out.wasm'
        with open(wasm_file, 'rb') as f:
            code = f.read()
        contract_name = 'bob'
        self.chain.deploy_contract(contract_name, code, b'')
        self.chain.produce_block()

        r = self.chain.push_action(contract_name, 'init', b'')
        logger.info('+++elapsed: %s', r['elapsed'])

        with open('before_apply_dump.bin', 'rb') as f:
            before_apply_dump = f.read()
        
        i = 0
        for i in range(5*1024, 65536):
            a = before_apply_dump[i]
            b = after_apply_dump[i]
            if not a == b:
                logger.info(i)
#                assert False, "not the same!"


    def test_exec(self):
        # print(os.getpid())
        # input('<<<')
        self.chain.buy_ram_bytes('alice', 'bob', 3*1024*1024)
        wasm_file = '/Users/newworld/dev/uuos3/build/libraries/vm/vm_wasm/out.wasm'
        with open(wasm_file, 'rb') as f:
            code = f.read()
        contract_name = 'bob'
        self.chain.deploy_contract(contract_name, code, b'')
        self.chain.produce_block()

        code = r'''
def apply(a, b, c):
    print('hello, blockchain world')
'''
        code = uuos.compile(code)
        setcode = b'\x00\x00\x00\x00\x00\x00\x0e=' + code
        self.chain.push_action(contract_name, 'setcode', setcode)
        self.chain.produce_block()

        args = b'\x00\x00\x00\x00\x00\x00\x0e='
        r = self.chain.push_action(contract_name, 'exec', args)
        logger.info('+++elapsed: %s', r['elapsed'])

