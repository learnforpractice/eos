import os
import time
import pytest
import logging
from chaintest import ChainTest

logging.basicConfig(filename='logfile.log', level=logging.INFO, format='%(asctime)s %(levelname)s %(lineno)d %(module)s %(message)s')
logger=logging.getLogger(__name__)

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.chain1 = ChainTest(uuos_network=True)
        cls.chain2 = ChainTest(uuos_network=True)

    @classmethod
    def teardown_class(cls):
        cls.chain1.free()
        cls.chain2.free()

    def setup_method(self, method):
        pass

    def teardown_method(self, method):
        pass

    def test_multi_chain(self):
        code = '''
import struct
import db

class MyData(object):
    def __init__(self, a: int, data: bytes):
        self.a = a
        self.data = data

    def pack(self):
        b = int.to_bytes(self.a, 8, 'little')
        return b + self.data

    @classmethod
    def unpack(cls, data):
        a = int.from_bytes(data[:8], 'little')
        return MyData(a, data[8:])

    @property
    def primary_key(self):
        return self.a

    def get_primary_key(self):
        return self.a

    def get_secondary_values(self):
        return ()

    def set_secondary_value(self, idx):
        pass

    @classmethod
    def get_secondary_indexes(self):
        return ()

code = N('helloworld11')
scope = N('scopee')
table = N('tableee')
mi = db.MultiIndex(code, scope, table, MyData)

def apply(receiver, code, action):
    payer = receiver
    data = read_action_data()
    print(data)
    itr = mi.find(111)
    if itr >= 0:
        d = mi.get(itr)
        print(d.data)
        d.data = data
    else:
        d = MyData(111, data)
    d.payer = receiver
    mi.store(d)
'''
        code = self.chain1.compile_py_code(code)

        contract_name = 'helloworld11'
        self.chain1.deploy_contract(contract_name, code, b'', vmtype=1)
        self.chain1.push_action(contract_name, 'sayhello', b'hello,world from chain1')
        self.chain1.push_action(contract_name, 'sayhello', b'hello,world from chain1 again')

        self.chain2.deploy_contract(contract_name, code, b'', vmtype=1)
        self.chain2.push_action(contract_name, 'sayhello', b'hello,world from chain2')
        self.chain2.push_action(contract_name, 'sayhello', b'hello,world from chain2 again')



#./uuos/uuos -m pytest ../../programs/uuos/tests/test.py::Test::test_create_account

