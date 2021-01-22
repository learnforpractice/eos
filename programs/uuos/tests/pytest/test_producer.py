import os
import sys
import unittest
from uuos import application
from uuos import wasmcompiler
from chaintest import ChainTest
import uuos
from uuoskit import uuosapi
print(os.getpid())
# input('<<<')

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

logger = application.get_logger(__name__)


class Test(object):

    @classmethod
    def setup_class(cls):
        cls.main_token = 'UUOS'
        cls.chain = ChainTest(network_type=1, jit=True)

        cls.chain.buy_ram_bytes('alice', 'alice', 10*1024*1024)
        cls.chain.buy_ram_bytes('alice', 'bob', 10*1024*1024)

        cls.chain.delegatebw('alice', 'alice', 500000.0, 500000.0)
        cls.chain.delegatebw('bob', 'bob', 500000.0, 500000.0)

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        pass

    def teardown_method(self, method):
        pass

    def test_produce_block(self):
        code = '''
def apply(receiver, code, action):
    for i in range(3):
        print('hello,world')
'''
        code = uuos.compile(code)
#        print(code)
        contract_name = 'alice'
        self.chain.deploy_python_contract(contract_name, code, b'')
        r = self.chain.push_action(contract_name, 'sayhello', b'')
        logger.info(r['elapsed'])
        self.chain.produce_block()

        args = {
            "producer": 'alice',
            "producer_key": 'UUOS6AjF6hvF7GSuSd4sCgfPKq5uWaXvGM2aQtEUCwmEHygQaqxBSV',
            "url": '',
            "location": 0
        }
        self.chain.push_action('uuos', 'regproducer', args, {'alice':'active'})

        args = {
            "producer": 'bob',
            "producer_key": 'UUOS61MgZLN7Frbc2J7giU7JdYjy2TqnfWFjZuLXvpHJoKzWAj7Nst',
            "url": '',
            "location": 0
        }
        self.chain.push_action('uuos', 'regproducer', args, {'bob':'active'})

        args = {
            "voter": 'bob',
            "proxy": '',
            "producers": ['bob'],
        }
        self.chain.push_action('uuos', 'voteproducer', args, {'bob':'active'})

        args = {
            "voter": 'alice',
            "proxy": '',
            "producers": ['alice'],
        }
        self.chain.push_action('uuos', 'voteproducer', args, {'alice':'active'})

        for i in range(100):
            self.chain.produce_block()