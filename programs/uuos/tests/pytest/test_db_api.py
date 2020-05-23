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
        cls.chain = chain = ChainTest(uuos_network=True)

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        logger.info("starting execution of tc: {}".format(method.__name__))

    def teardown_method(self, method):
        logger.info("Ending execution of tc: {}".format(method.__name__))

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
        self.chain.produce_block()

        r = self.chain.push_action(name, 'destroy', b'')
        self.chain.produce_block()