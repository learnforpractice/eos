import os
import time
import json
import pytest
import logging
import subprocess
import shutil

from chaintest import ChainTest
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
        cls.chain = ChainTest(network_type=1, jit=True)
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

    def setup_method(self, method):
        logger.warning('test start: %s', method.__name__)

    def teardown_method(self, method):
        try:
            self.chain.deploy_contract('alice', b'', b'', vm_type=3)
        except Exception as e:
            assert e.args[0]['except']['name'] == 'set_exact_code'
            assert e.args[0]['except']['message'] == 'Contract is already running this version of code'
        self.chain.produce_block()
        logger.warning('test end: %s', method.__name__)

    def compile_cpp_file(self, name):
        code_file = os.path.join(test_dir, 'test_contracts', f'{name}.cpp')
        wasm_file = os.path.join(test_dir, 'test_contracts', f'{name}.wasm')
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
        return code

    def compile(self, code):
#        code = eosapi.compile_py_src(code)
        with open('tmp.py', 'w') as f:
            f.write(code)
        subprocess.check_output(['mpy-cross', '-o', 'tmp.mpy', 'tmp.py'])
        with open('tmp.mpy', 'rb') as f:
            code = f.read()
        os.remove('tmp.py')
        os.remove('tmp.mpy')

        mpy_code = ((code, len(code)),)

        code_region = b''
        code_size_region = b''
        for code, size in mpy_code:
            code_region += code
            code_size_region += int.to_bytes(size, 4, 'little')

        name_region = b'main.mpy\x00'

        region_sizes = b''
        region_sizes += int.to_bytes(len(name_region), 4, 'little')
        region_sizes += int.to_bytes(len(code_size_region), 4, 'little')
        region_sizes += int.to_bytes(len(code_region), 4, 'little')

        header = int.to_bytes(5, 4, 'little')
        header += bytearray(60)
        frozen_code = header + region_sizes + name_region + code_size_region + code_region
        return frozen_code

    def compile_all(self, code_info):
#        code = eosapi.compile_py_src(code)
        mpy_code = []
        if not os.path.exists('tmp'):
            os.mkdir('tmp')
        for name, code in code_info:
            py_file = f'tmp/{name}.py'
            mpy_file = f'tmp/{name}.mpy'

            with open(py_file, 'w') as f:
                f.write(code)
            subprocess.check_output(['mpy-cross', '-o', mpy_file, py_file])
            with open(mpy_file, 'rb') as f:
                code = f.read()
            os.remove(py_file)
            os.remove(mpy_file)

            mpy_code.append((name, code, len(code)))
        shutil.rmtree('tmp')

        name_region = b''
        code_region = b''
        code_size_region = b''
        for name, code, size in mpy_code:
            code_region += code
            code_size_region += int.to_bytes(size, 4, 'little')
            name_region += b'%s.mpy\x00'%(name.encode(),)

        region_sizes = b''
        region_sizes += int.to_bytes(len(name_region), 4, 'little')
        region_sizes += int.to_bytes(len(code_size_region), 4, 'little')
        region_sizes += int.to_bytes(len(code_region), 4, 'little')

        header = int.to_bytes(5, 4, 'little')
        header += bytearray(60)
        frozen_code = header + region_sizes + name_region + code_size_region + code_region
        return frozen_code

    #test bill_to_first_authorizer
    def test_permissions(self):
        args = {
            'from': 'alice',
            'to': 'bob',
            'quantity': '0.1000 UUOS',
            'memo':'hello'
        }

        perms = {
            'bob':'active',
            'alice':'active'
        }
        cpu_limit_alice = self.chain.get_account('alice')['cpu_limit']
        cpu_limit_bob = self.chain.get_account('bob')['cpu_limit']

        for i in range(1):
            args['memo'] = str(i)
            self.chain.push_action_with_multiple_permissions('uuos.token', 'transfer', args, perms)

        cpu_limit_alice2 = self.chain.get_account('alice')['cpu_limit']
        cpu_limit_bob2 = self.chain.get_account('bob')['cpu_limit']

        assert cpu_limit_alice == cpu_limit_alice2
        assert not cpu_limit_bob == cpu_limit_bob2

