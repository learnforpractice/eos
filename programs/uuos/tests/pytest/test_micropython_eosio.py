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
from uuoskit import uuosapi

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

        cls.chain.push_action(cls.chain.system_contract, 'updateauth', a, {'alice':'active'})

        cls.chain.buy_ram_bytes('alice', 'alice', 10*1024*1024)
        cls.chain.buy_ram_bytes('alice', 'bob', 10*1024*1024)

        cls.chain.delegatebw('alice', 'alice', 1.0, 1.0)
        cls.chain.delegatebw('alice', 'bob', 1.0, 1.0)
        cls.chain.produce_block()

        cls.deploy_vm()

    @classmethod
    def deploy_vm(cls):
        wasm_file = os.path.join(test_dir, '../../../../build', 'externals/micropython/ports/uuosio/micropython_eosio.wasm')
        abi = '''{
    "version": "eosio::abi/1.0",
    "types": [],
    "structs": [],
    "actions": [{
        "name": "sayhello",
        "type": "string",
        "ricardian_contract": ""
    },
    {
        "name": "initlog",
        "type": "string",
        "ricardian_contract": ""
    }
    ],
    "tables": [
        {
            "name": "codecache",
            "type": "string",
            "index_type": "i64",
            "key_names": [],
            "key_types": []
        },
        {
            "name": "codetable",
            "type": "string",
            "index_type": "i64",
            "key_names": [],
            "key_types": []
        },
        {
            "name": "abitable",
            "type": "string",
            "index_type": "i64",
            "key_names": [],
            "key_types": []
        }
    ],
    "ricardian_clauses": [],
    "error_messages": [],
    "abi_extensions": []
}
'''
        with open(wasm_file, 'rb') as f:
            code = f.read()
        contract_name = 'bob'
        cls.chain.deploy_contract(contract_name, code, abi, vm_type=0)
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
        code = uuos.compile(code)
        assert code
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

    def test_ram_usage(self):
        # print(os.getpid())
        # input('<<<')

        wasm_file = os.path.join(test_dir, '../../../../build', 'externals/micropython/ports/uuosio/micropython_eosio.wasm')
        abi = '''{
    "version": "eosio::abi/1.0",
    "types": [],
    "structs": [],
    "actions": [{
        "name": "sayhello",
        "type": "string",
        "ricardian_contract": ""
    },
    {
        "name": "initlog",
        "type": "string",
        "ricardian_contract": ""
    }
    ],
    "tables": [
        {
            "name": "codecache",
            "type": "string",
            "index_type": "i64",
            "key_names": [],
            "key_types": []
        },
        {
            "name": "codetable",
            "type": "string",
            "index_type": "i64",
            "key_names": [],
            "key_types": []
        },
    ],
    "ricardian_clauses": [],
    "error_messages": [],
    "abi_extensions": []
}
'''
        with open(wasm_file, 'rb') as f:
            code = f.read()
        contract_name = 'bob'
#        self.chain.deploy_contract(contract_name, code, abi)
        self.chain.produce_block()

        code = '''
import json
import struct
import db

class MyData(object):
    def __init__(self, a: int, b: int, c: int, d: float):
        self.a = a
        self.b = b
        self.c = c
        self.d = d
        self.payer = 0

    def pack(self):
        b = int.to_bytes(self.b, 16, 'little')
        c = int.to_bytes(self.c, 32, 'little')
        return struct.pack('Q16s32sd', self.a, b, c, self.d)

    @classmethod
    def unpack(cls, data):
        a, b, c, d = struct.unpack('Q16s32sd', data)
        b = int.from_bytes(b, 'little')
        c = int.from_bytes(c, 'little')
        return cls(a, b, c, d)

    def get_primary_key(self):
        return self.a

    def get_secondary_values(self):
        return (self.a, self.b, self.c, self.d)

    @staticmethod
    def get_secondary_indexes():
        return (db.idx64, db.idx128, db.idx256, db.idx_double)

    def __str__(self):
        data = (self.a, self.b, self.c, self.d)
        return json.dumps(data)

def apply(receiver, code, action):
    for i in range(10):
        d = MyData(1, 2, 3, 5.0)
        d.pack()
    pass
'''
        # let data memory exceed the first 64KB
        fill_code = ''
        if 0:
            for i in range(64*1024):
                fill_code += '    pass\n'
        fill_code += '    print("hello,world")\n'
        code += fill_code
        code = uuos.compile(code)
        args = int.to_bytes(uuosapi.s2n('alice'), 8, 'little')
        args += code

        r = self.chain.push_action('bob', 'setcode', args, {'alice':'active'})
        self.chain.produce_block()
        r = self.chain.push_action('bob', 'setcode', args, {'alice':'active'})
        logger.info('+++setcode: elapsed: %s', r['elapsed'])
        err, r = self.chain.get_table_rows(False, 'bob', 'alice', 'codecache', '', 'alice', 1)
        codecache = r['rows'][0]
        codecache = bytes.fromhex(codecache)
        logger.info('%s %s %s %s', len(codecache), codecache.find(code), len(codecache) - codecache.find(code), len(code))
#42452
        contract_name = 'alice'
        args = int.to_bytes(uuosapi.s2n('alice'), 8, 'little') + b'hello,world'
        r = self.chain.push_action('bob', 'exec', args, {'alice':'active'})
        logger.info('+++exec: elapsed: %s', r['elapsed'])
        # assert r['action_traces'][0]['console'] == 'hello,world\r\n'

    def test_upgrade_vm(self):
        # print(os.getpid())
        # input('<<<')

        wasm_file = os.path.join(test_dir, '../../../../build', 'externals/micropython/ports/uuosio/micropython_eosio.wasm')
        abi = '''{
    "version": "eosio::abi/1.0",
    "types": [],
    "structs": [],
    "actions": [{
        "name": "sayhello",
        "type": "string",
        "ricardian_contract": ""
    },
    {
        "name": "initlog",
        "type": "string",
        "ricardian_contract": ""
    }
    ],
    "tables": [
        {
            "name": "codecache",
            "type": "string",
            "index_type": "i64",
            "key_names": [],
            "key_types": []
        },
        {
            "name": "codetable",
            "type": "string",
            "index_type": "i64",
            "key_names": [],
            "key_types": []
        },
    ],
    "ricardian_clauses": [],
    "error_messages": [],
    "abi_extensions": []
}
'''
        with open(wasm_file, 'rb') as f:
            code = f.read()
        contract_name = 'bob'
        self.chain.deploy_contract(contract_name, code, abi)
        self.chain.produce_block()

        code = '''
print('+++init module')
def apply(receiver, code, action):
    print('hello,world')
'''
        # let data memory exceed the first 64KB
        fill_code = '    count = 1\n'
        for i in range(32*1024):
            fill_code += '    count += 1\n'
        fill_code += '    print("hello,world")\n'
        fill_code += '    print("hello,world")\n'
        fill_code += '    print("hello,world")\n'
        fill_code += '    print(count)\n'
        code += fill_code
        code = uuos.compile(code)
        args = int.to_bytes(uuosapi.s2n('alice'), 8, 'little')
        args += code

        r = self.chain.push_action('bob', 'setcode', args, {'alice':'active'})
        self.chain.produce_block()
        r = self.chain.push_action('bob', 'setcode', args, {'alice':'active'})
        logger.info('+++setcode: elapsed: %s', r['elapsed'])
        err, r = self.chain.get_table_rows(False, 'bob', 'alice', 'codecache', '', 'alice', 1)
        codecache = r['rows'][0]
        codecache = bytes.fromhex(codecache)
        logger.info('%s %s %s %s', len(codecache), codecache.find(code), len(codecache) - codecache.find(code), len(code))
#42452
        contract_name = 'alice'
        args = int.to_bytes(uuosapi.s2n('alice'), 8, 'little') + b'hello,world'
        r = self.chain.push_action('bob', 'exec', args, {'alice':'active'})
        logger.info('+++exec: elapsed: %s', r['elapsed'])
        # assert r['action_traces'][0]['console'] == 'hello,world\r\n'

        wasm_file = os.path.join(test_dir, '../../../../build', 'externals/micropython/ports/uuosio/micropython_eosio_test.wasm')
        with open(wasm_file, 'rb') as f:
            code = f.read()
        contract_name = 'bob'
        self.chain.deploy_contract(contract_name, code, abi)
        self.chain.produce_block()

        contract_name = 'alice'
        args = int.to_bytes(uuosapi.s2n('alice'), 8, 'little') + b'hello,world'
        r = self.chain.push_action('bob', 'exec', args, {'alice':'active'})
        logger.info('+++exec: elapsed: %s', r['elapsed'])
        # assert r['action_traces'][0]['console'] == 'hello,world\r\n'
        self.chain.produce_block()

        contract_name = 'alice'
        args = int.to_bytes(uuosapi.s2n('alice'), 8, 'little') + b'hello,world'
        r = self.chain.push_action('bob', 'exec', args, {'alice':'active'})
        logger.info('+++exec: elapsed: %s', r['elapsed'])
        # assert r['action_traces'][0]['console'] == 'hello,world\r\n'


    def test_crypto(self):
        # print(os.getpid())
        # input('<<<')

        wasm_file = os.path.join(test_dir, '../../../../build', 'externals/micropython/ports/uuosio/micropython_eosio.wasm')
        abi = '''
'''
        with open(wasm_file, 'rb') as f:
            code = f.read()
        contract_name = 'bob'
        self.chain.deploy_contract(contract_name, code, abi, vm_type=0)
        self.chain.produce_block()

        code = '''
import chain
def apply(receiver, code, action):
    h = chain.sha256('hello,world')
    print(h)
'''
        code = uuos.compile(code)
        args = int.to_bytes(uuosapi.s2n('alice'), 8, 'little')
        args += code

        r = self.chain.push_action('bob', 'setcode', args, {'alice':'active'})
        self.chain.produce_block()

        contract_name = 'alice'
        args = int.to_bytes(uuosapi.s2n('alice'), 8, 'little') + b'hello,world'
        r = self.chain.push_action('bob', 'exec', args, {'alice':'active'})
        logger.info('+++exec: elapsed: %s', r['elapsed'])
        # assert r['action_traces'][0]['console'] == 'hello,world\r\n'

    def test_upgrade_vm(self):
        # print(os.getpid())
        # input('<<<')

        wasm_file = os.path.join(test_dir, '../../../../build', 'externals/micropython/ports/uuosio/micropython_eosio.wasm')
        abi = '''{
    "version": "eosio::abi/1.0",
    "types": [],
    "structs": [],
    "actions": [{
        "name": "sayhello",
        "type": "string",
        "ricardian_contract": ""
    },
    {
        "name": "initlog",
        "type": "string",
        "ricardian_contract": ""
    }
    ],
    "tables": [
        {
            "name": "codecache",
            "type": "string",
            "index_type": "i64",
            "key_names": [],
            "key_types": []
        },
        {
            "name": "codetable",
            "type": "string",
            "index_type": "i64",
            "key_names": [],
            "key_types": []
        },
    ],
    "ricardian_clauses": [],
    "error_messages": [],
    "abi_extensions": []
}
'''
        with open(wasm_file, 'rb') as f:
            code = f.read()
        contract_name = 'bob'
        self.chain.deploy_contract(contract_name, code, abi)
        self.chain.produce_block()

        code = '''
print('+++init module')
def apply(receiver, code, action):
    print('hello,world')
'''
        # let data memory exceed the first 64KB
        fill_code = '    count = 1\n'
        for i in range(32*1024):
            fill_code += '    count += 1\n'
        fill_code += '    print("hello,world")\n'
        fill_code += '    print("hello,world")\n'
        fill_code += '    print("hello,world")\n'
        fill_code += '    print(count)\n'
        code += fill_code
        code = uuos.compile(code)
        args = int.to_bytes(uuosapi.s2n('alice'), 8, 'little')
        args += code

        r = self.chain.push_action('bob', 'setcode', args, {'alice':'active'})
        self.chain.produce_block()
        r = self.chain.push_action('bob', 'setcode', args, {'alice':'active'})
        logger.info('+++setcode: elapsed: %s', r['elapsed'])
        err, r = self.chain.get_table_rows(False, 'bob', 'alice', 'codecache', '', 'alice', 1)
        codecache = r['rows'][0]
        codecache = bytes.fromhex(codecache)
        logger.info('%s %s %s %s', len(codecache), codecache.find(code), len(codecache) - codecache.find(code), len(code))
#42452
        contract_name = 'alice'
        args = int.to_bytes(uuosapi.s2n('alice'), 8, 'little') + b'hello,world'
        r = self.chain.push_action('bob', 'exec', args, {'alice':'active'})
        logger.info('+++exec: elapsed: %s', r['elapsed'])
        # assert r['action_traces'][0]['console'] == 'hello,world\r\n'

        wasm_file = os.path.join(test_dir, '../../../../build', 'externals/micropython/ports/uuosio/micropython_eosio_test.wasm')
        with open(wasm_file, 'rb') as f:
            code = f.read()
        contract_name = 'bob'
        self.chain.deploy_contract(contract_name, code, abi)
        self.chain.produce_block()

        contract_name = 'alice'
        args = int.to_bytes(uuosapi.s2n('alice'), 8, 'little') + b'hello,world'
        r = self.chain.push_action('bob', 'exec', args, {'alice':'active'})
        logger.info('+++exec: elapsed: %s', r['elapsed'])
        # assert r['action_traces'][0]['console'] == 'hello,world\r\n'
        self.chain.produce_block()

        contract_name = 'alice'
        args = int.to_bytes(uuosapi.s2n('alice'), 8, 'little') + b'hello,world'
        r = self.chain.push_action('bob', 'exec', args, {'alice':'active'})
        logger.info('+++exec: elapsed: %s', r['elapsed'])
        # assert r['action_traces'][0]['console'] == 'hello,world\r\n'

    def test_import(self):
        # print(os.getpid())
        # input('<<<')

        code = '''
def apply(receiver, code, action):
    import chain
    from alice import hello
    hello.say_hello()
'''
        code = uuos.compile(code)
        args = uuosapi.s2b('alice') + code

        r = self.chain.push_action('bob', 'setcode', args, {'alice':'active'})
        self.chain.produce_block()

        code = '''
def say_hello():
    print('hello,world, all')
'''
        code = uuos.compile(code)
        args = uuosapi.s2b('alice') + uuosapi.s2b('hello') + code
        print(args)
        r = self.chain.push_action('bob', 'setmodule', args, {'alice':'active'})

        try:
            args = uuosapi.s2b('alice') + b'hello,world'
            r = self.chain.push_action('bob', 'exec', args, {'alice':'active'})
        except Exception as e:
            logger.info(e.args[0]['action_traces'][0]['console'])

    def test_setabi(self):
        abi = '''{
    "version": "eosio::abi/1.0",
    "types": [],
    "structs": [],
    "actions": [{
        "name": "sayhello",
        "type": "string",
        "ricardian_contract": ""
    },
    {
        "name": "initlog",
        "type": "string",
        "ricardian_contract": ""
    }
    ],
    "tables": [
        {
            "name": "codecache",
            "type": "string",
            "index_type": "i64",
            "key_names": [],
            "key_types": []
        },
        {
            "name": "codetable",
            "type": "string",
            "index_type": "i64",
            "key_names": [],
            "key_types": []
        },
        {
            "name": "abitable",
            "type": "string",
            "index_type": "i64",
            "key_names": [],
            "key_types": []
        },
    ],
    "ricardian_clauses": [],
    "error_messages": [],
    "abi_extensions": []
}
'''
        abi = uuosapi.pack_abi(abi)
        args = uuosapi.s2b('alice') + abi
        r = self.chain.push_action('bob', 'setabi', args, {'alice':'active'})
        err, r = self.chain.get_table_rows(False, 'bob', 'alice', 'abitable', '', 'alice', 1)
        logger.info(r['rows'][0])
        abi = bytes.fromhex(r['rows'][0])
        abi = uuosapi.unpack_abi(abi)
        logger.info(abi)
