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
        cls.chain = ChainTest(uuos_network=True)
        accounts = [
            {
                "permission": {
                    "actor": "helloworld11",
                    "permission": "eosio.code"
                },
                "weight": 1
            },
        ]

        keys = [{
            "key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
            "weight": 1
        }]

        a = {
            "account": 'helloworld11',
            "permission": 'active',
            "parent": 'owner',
            "auth": {
                "threshold": 1,
                "keys": keys,
                "accounts": accounts,
                "waits": []
            }
        }

        cls.chain.push_action('eosio', 'updateauth', a, actor='helloworld11', perm='owner')

    @classmethod
    def teardown_class(cls):
        pass

    def setup_method(self, method):
        logger.info("starting execution of tc: {}".format(method.__name__))

    def teardown_method(self, method):
        logger.info("Ending execution of tc: {}".format(method.__name__))
        self.chain.free()

    def test_send_inline(self):
        code = '''
def apply(receiver, code, action):
    if action == N('test'):
        send_inline('helloworld11', 'test2', 'helloworld11', 'active', b'hello,world')
    elif action == N('test2'):
        data = read_action_data()
        print(data)
        '''
        name = 'helloworld11'
        code = self.chain.compile_py_code(code)
        self.chain.deploy_contract(name, code, b'', vmtype=1)

        r = self.chain.push_action(name, 'test', b'')
        self.chain.produce_block()
