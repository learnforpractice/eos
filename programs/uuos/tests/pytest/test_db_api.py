import os
import sys
import json
import gc
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos import application
from chaintest import ChainTest

logger = application.get_logger(__name__)

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.main_token = 'UUOS'

    @classmethod
    def teardown_class(cls):
        pass

    def setup_method(self, method):
        pass
        self.chain = ChainTest(uuos_network=True)

    def teardown_method(self, method):
        pass
        self.chain.free()

    def test_db_i64(self):
        name = 'helloworld11'
        code = self.chain.compile_py_code_from_file('db_test/db_test_i64.py')
        self.chain.deploy_contract(name, code, b'', vmtype=1)

        r = self.chain.push_action(name, 'destroy', b'')
        r = self.chain.push_action(name, 'init', b'hello,world')
        self.chain.push_action(name, 'test', b'')
        self.chain.push_action(name, 'test1', b'')
        self.chain.push_action(name, 'test2', b'')
        self.chain.push_action(name, 'test3', b'')
        self.chain.produce_block()

        r = self.chain.push_action(name, 'destroy', b'')
        self.chain.produce_block()

    def test_db_idx64(self):
        name = 'helloworld11'
        code = self.chain.compile_py_code_from_file('db_test/db_test_idx64.py')
        self.chain.deploy_contract(name, code, b'', vmtype=1)

        r = self.chain.push_action(name, 'destroy', b'')
        r = self.chain.push_action(name, 'init', b'hello,world')
        self.chain.push_action(name, 'test', b'')
        self.chain.push_action(name, 'test1', b'')
        self.chain.push_action(name, 'test2', b'')
        self.chain.push_action(name, 'test3', b'')
        self.chain.produce_block()

        r = self.chain.push_action(name, 'destroy', b'')
        self.chain.produce_block()

    def test_db_idx_double(self):
        name = 'helloworld11'
        code = self.chain.compile_py_code_from_file('db_test/db_test_idx_double.py')
        self.chain.deploy_contract(name, code, b'', vmtype=1)

        r = self.chain.push_action(name, 'destroy', b'')
        r = self.chain.push_action(name, 'init', b'hello,world')
        self.chain.push_action(name, 'test', b'')
        self.chain.push_action(name, 'test1', b'')
        self.chain.push_action(name, 'test2', b'')
        self.chain.push_action(name, 'test3', b'')

        self.chain.produce_block()

        r = self.chain.push_action(name, 'destroy', b'')
        self.chain.produce_block()

    def test_db_idx128(self):
        name = 'helloworld11'
        code = self.chain.compile_py_code_from_file('db_test/db_test_idx128.py')
        self.chain.deploy_contract(name, code, b'', vmtype=1)

        r = self.chain.push_action(name, 'destroy', b'')
        r = self.chain.push_action(name, 'init', b'hello,world')
        self.chain.push_action(name, 'test', b'')
        self.chain.push_action(name, 'test1', b'')
        self.chain.push_action(name, 'test2', b'')
        self.chain.push_action(name, 'test3', b'')
        self.chain.produce_block()

        r = self.chain.push_action(name, 'destroy', b'')
        self.chain.produce_block()

    def test_db_idx256(self):
        name = 'helloworld11'
        code = self.chain.compile_py_code_from_file('db_test/db_test_idx256.py')
        self.chain.deploy_contract(name, code, b'', vmtype=1)

        r = self.chain.push_action(name, 'destroy', b'')
        r = self.chain.push_action(name, 'init', b'hello,world')
        self.chain.push_action(name, 'test', b'')
        self.chain.push_action(name, 'test1', b'')
        self.chain.push_action(name, 'test2', b'')
        self.chain.push_action(name, 'test3', b'')
        self.chain.produce_block()

        r = self.chain.push_action(name, 'destroy', b'')
        self.chain.produce_block()

    def test_db_idx_long_double(self):
        name = 'helloworld11'
        code = self.chain.compile_py_code_from_file('db_test/db_test_idx_long_double.py')
        self.chain.deploy_contract(name, code, b'', vmtype=1)

        r = self.chain.push_action(name, 'destroy', b'')
        r = self.chain.push_action(name, 'init', b'hello,world')
        self.chain.push_action(name, 'test', b'')
        self.chain.push_action(name, 'test1', b'')
        self.chain.push_action(name, 'test2', b'')
        self.chain.push_action(name, 'test3', b'')
        self.chain.produce_block()

        r = self.chain.push_action(name, 'destroy', b'')
        self.chain.produce_block()
    
    def test_db_mi(self):
        name = 'helloworld11'
        code = self.chain.compile_py_code_from_file('db_test/db_test.py')
        self.chain.deploy_contract(name, code, b'', vmtype=1)

        self.chain.push_action(name, 'test', b'')
        self.chain.produce_block()

    def test_db_mi2(self):
        name = 'helloworld11'
        code = self.chain.compile_py_code_from_file('db_test/db_test2.py')
        self.chain.deploy_contract(name, code, b'', vmtype=1)

        self.chain.push_action(name, 'store', b'')
        self.chain.push_action(name, 'store', b'a')

        self.chain.push_action(name, 'get', b'')
        self.chain.push_action(name, 'update', b'')
        self.chain.push_action(name, 'destory', b'')

        self.chain.produce_block()
    
    def test_db_mi3(self):
        name = 'helloworld11'
        code = self.chain.compile_py_code_from_file('db_test/db_test3.py')
        self.chain.deploy_contract(name, code, b'', vmtype=1)

        self.chain.push_action(name, 'store', b'')
        self.chain.push_action(name, 'get', b'')
        self.chain.push_action(name, 'update', b'')

        self.chain.produce_block()