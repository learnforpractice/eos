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
        logger.info("starting execution of tc: {}".format(method.__name__))
        # self.chain = ChainTest(uuos_network=True, jit=True)
        self.chain = ChainTest(uuos_network=True, jit=True)

    def teardown_method(self, method):
        logger.info("Ending execution of tc: {}".format(method.__name__))
        self.chain.free()

    def test_cardgame1(self):
        name = 'helloworld11'
        code_file = os.path.join(test_dir, '..', 'test_contracts/cardgame', 'cardgame.wasm')
        with open(code_file, 'rb') as f:
            code = f.read()
        abi_file = os.path.join(test_dir, '..', 'test_contracts/cardgame', 'cardgame.abi')
        with open(abi_file, 'rb') as f:
            abi = f.read()

        self.chain.deploy_contract(name, code, abi, vmtype=0)

        args = {'username':'helloworld11'}
        self.chain.push_action(name, 'login', args, actor='helloworld11', perm='active')
        self.chain.produce_block()
        self.chain.push_action(name, 'login', args, actor='helloworld11', perm='active')
        self.chain.produce_block()
        self.chain.push_action(name, 'startgame', args, actor='helloworld11', perm='active')

        args = {'username':'helloworld11', 'player_card_idx':1}
        self.chain.push_action(name, 'playcard', args, actor='helloworld11', perm='active')


        args = {'username':'helloworld11'}
        self.chain.push_action(name, 'nextround', args, actor='helloworld11', perm='active')

        self.chain.push_action(name, 'endgame', args, actor='helloworld11', perm='active')
        return

    def test_cardgame2(self):
        name = 'helloworld12'
        code_file = os.path.join(test_dir, '..', 'test_contracts/cardgame', 'cardgame.wasm')
        with open(code_file, 'rb') as f:
            code = f.read()
        abi_file = os.path.join(test_dir, '..', 'test_contracts/cardgame', 'cardgame.abi')
        with open(abi_file, 'rb') as f:
            abi = f.read()
        self.chain.deploy_contract(name, code, abi, vmtype=0)

        name = 'helloworld11'
        code = '''
def apply(receiver, code, action):
    if action == N('login'):
        call_contract('helloworld12', b'')
    elif action == N('startgame'):
        call_contract('helloworld12', b'')
    elif action == N('endgame'):
        call_contract('helloworld12', b'')
    elif action == N('playcard'):
        call_contract('helloworld12', b'')
        '''
        code = self.chain.compile_py_code(code)
        abi_file = os.path.join(test_dir, '..', 'test_contracts/cardgame', 'cardgame.abi')
        with open(abi_file, 'rb') as f:
            abi = f.read()
        self.chain.deploy_contract(name, code, abi, vmtype=1)
        args = {'username':'helloworld11'}

        self.chain.push_action(name, 'login', args, actor='helloworld11', perm='active')
        self.chain.produce_block()
        self.chain.push_action(name, 'login', args, actor='helloworld11', perm='active')
        self.chain.produce_block()
        self.chain.push_action(name, 'startgame', args, actor='helloworld11', perm='active')

        args = {'username':'helloworld11', 'player_card_idx':1}
        self.chain.push_action(name, 'playcard', args, actor='helloworld11', perm='active')


        args = {'username':'helloworld11'}
        self.chain.push_action(name, 'nextround', args, actor='helloworld11', perm='active')

        self.chain.push_action(name, 'endgame', args, actor='helloworld11', perm='active')
        return

    def test_cardgame3(self):
        name = 'helloworld11'
        code = self.chain.compile_py_code_from_file('cardgame/cardgame.py')
        abi_file = os.path.join(test_dir, '..', 'test_contracts/cardgame', 'cardgame.abi')
        with open(abi_file, 'rb') as f:
            abi = f.read()
        self.chain.deploy_contract(name, code, abi, vmtype=1)
        args = {'username':'helloworld11'}

        self.chain.push_action(name, 'login', args, actor='helloworld11', perm='active')
        self.chain.produce_block()
        self.chain.push_action(name, 'login', args, actor='helloworld11', perm='active')
        self.chain.produce_block()
        self.chain.push_action(name, 'startgame', args, actor='helloworld11', perm='active')

        args = {'username':'helloworld11', 'player_card_idx':1}
        self.chain.push_action(name, 'playcard', args, actor='helloworld11', perm='active')


        args = {'username':'helloworld11'}
        self.chain.push_action(name, 'nextround', args, actor='helloworld11', perm='active')

        self.chain.push_action(name, 'endgame', args, actor='helloworld11', perm='active')
        return




        rows = self.chain.get_table_rows(False, 'helloworld11', 'scoppe', 'users', 'helloworld11', '', '', 10)
        logger.info(rows)
        from uuos.db import DB
        db = DB(self.chain.chain.ptr)
        (helloworld11, scope, users) = (7684014468695212560, 13990807347690209280, 15426372438069346304)
        itr = db.find_i64(helloworld11, scope, users, helloworld11)
        print(itr, db.get_i64(itr))

