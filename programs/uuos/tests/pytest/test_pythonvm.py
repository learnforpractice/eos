import os
import time
import struct
import pytest
import logging
from chaintest import ChainTest

logging.basicConfig(filename='logfile.log', level=logging.INFO, format='%(asctime)s %(levelname)s %(lineno)d %(module)s %(message)s')
logger=logging.getLogger(__name__)

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.chain = ChainTest(uuos_network=True)

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        pass

    def teardown_method(self, method):
        pass

    def test_hello_world(self):
        logger.info('hello,world')

    def test_api(self):
        contract_name = 'testmetestme'
        code = self.chain.compile_py_code_from_file('pythonvm_test.py')
        self.chain.deploy_contract(contract_name, code, b'', vmtype=1)
        self.chain.produce_block()

        self.chain.push_action(contract_name, 'test1', b'hello,world!')
        self.chain.push_action(contract_name, 'test2', b'hello,world!')
        self.chain.push_action(contract_name, 'test3', '你好，世界!'.encode('utf8'))

        try:
            self.chain.push_action(contract_name, 'test4', b'')
        except Exception as e:
            # logger.info(f'{type(e)}, {e.args[0]}')
            assert e.args[0]['except']['code'] == 3080006 #deadline_exception

        try:
            self.chain.push_action(contract_name, 'test5', b'')
        except Exception as e:
            assert e.args[0]['except']['code'] == 3050003 #eosio_assert_message_exception
            assert e.args[0]['except']['stack'][0]['data']['s'] == 'no free vm memory left!'
        self.chain.produce_block()

        start = 0
        for i in range(10):
            end = start + 10 * 1024
            args = struct.pack('III', 3*1024*1024, start, end)
            r = self.chain.push_action(contract_name, 'test51', args)
            if i % 50 == 0:
                self.chain.produce_block()
            start += 10*1024

        try:
            self.chain.push_action(contract_name, 'test52', b'')
        except Exception as e:
            assert e.args[0]['except']['code'] == 3050003 #eosio_assert_message_exception
            assert e.args[0]['except']['stack'][0]['data']['s'] == 'vm error out of bounds'
        self.chain.produce_block()


#./uuos/uuos -m pytest ../../programs/uuos/tests/test.py::Test::test_create_account

