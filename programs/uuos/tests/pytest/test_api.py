import os
import sys
import json
import gc
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos import application
from uuos import wasmcompiler

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
                    "actor": "alice",
                    "permission": "eosio.code"
                },
                "weight": 1
            },
        ]

        keys = [{
            "key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
            "weight": 1
        }]

        cls.chain.update_auth('alice', accounts, keys)

        accounts = [
            {
                "permission": {
                    "actor": "helloworld12",
                    "permission": "eosio.code"
                },
                "weight": 1
            },
        ]

        keys = [{
            "key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
            "weight": 1
        }]

        cls.chain.update_auth('helloworld12', accounts, keys)

    @classmethod
    def teardown_class(cls):
        pass

    def setup_method(self, method):
        logger.info("starting execution of tc: {}".format(method.__name__))

    def teardown_method(self, method):
        logger.info("Ending execution of tc: {}".format(method.__name__))
        self.chain.free()

    def test_send_inline1(self):
        code = '''
def apply(receiver, code, action):
    if action == N('test'):
        send_inline('alice', 'test2', 'alice', 'active', b'hello,world from test')
    elif action == N('test2'):
        data = read_action_data()
        print(data)
        send_inline('alice', 'test3', 'alice', 'active', b'hello,world from test2')
    elif action == N('test3'):
        data = read_action_data()
        print(data)
        '''
        name = 'alice'
        code = self.chain.compile_py_code(code)
        self.chain.deploy_contract(name, code, b'', vmtype=1)

        r = self.chain.push_action(name, 'test', b'')
        r = self.chain.push_action(name, 'test', b'a')
        self.chain.produce_block()

    def test_send_inline2(self):
        code = r'''
#include <eosio/eosio.hpp>
#include <eosio/action.hpp>
#include <eosio/print.hpp>
extern "C" {
    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
         eosio::print("hello,world");
    }
}
        '''
        contract_name = 'bob'
        code = wasmcompiler.compile_cpp_src(contract_name, code, entry='apply', force=True)
        self.chain.deploy_contract(contract_name, code, b'')

        code = '''
def apply(receiver, code, action):
    if action == N('test'):
        send_inline('alice', 'test2', 'alice', 'active', b'hello,world from test')
    elif action == N('test2'):
        data = read_action_data()
        print(data)
        send_inline('alice', 'test3', 'alice', 'active', b'hello,world from test2')
    elif action == N('test3'):
        data = read_action_data()
        print(data)
        send_inline('bob', 'test', 'alice', 'active', b'hello,world from test2')
        '''
        name = 'alice'
        code = self.chain.compile_py_code(code)
        self.chain.deploy_contract(name, code, b'', vmtype=1)

        r = self.chain.push_action(name, 'test', b'')
        r = self.chain.push_action(name, 'test', b'a')
        self.chain.produce_block()
