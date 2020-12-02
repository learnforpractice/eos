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
        self.chain = ChainTest(network_type=1, jit=True)
        # self.chain = ChainTest(uuos_network=True, jit=False)

    def teardown_method(self, method):
        pass
        self.chain.free()

    def test_cardgame1(self):
        name = 'testmetestme'
        code_file = os.path.join(test_dir, '..', 'test_contracts/cardgame', 'cardgame.wasm')
        with open(code_file, 'rb') as f:
            code = f.read()
        abi_file = os.path.join(test_dir, '..', 'test_contracts/cardgame', 'cardgame.abi')
        with open(abi_file, 'rb') as f:
            abi = f.read()

        self.chain.deploy_contract(name, code, abi, vmtype=0)

        args = {'username':'testmetestme'}
        self.chain.push_action(name, 'login', args, actor='testmetestme', perm='active')
        self.chain.produce_block()
        self.chain.push_action(name, 'login', args, actor='testmetestme', perm='active')
        self.chain.produce_block()
        self.chain.push_action(name, 'startgame', args, actor='testmetestme', perm='active')

        args = {'username':'testmetestme', 'player_card_idx':1}
        self.chain.push_action(name, 'playcard', args, actor='testmetestme', perm='active')


        args = {'username':'testmetestme'}
        self.chain.push_action(name, 'nextround', args, actor='testmetestme', perm='active')

        self.chain.push_action(name, 'endgame', args, actor='testmetestme', perm='active')
        return

    def test_cardgame2(self):
        name = 'dothetesting'
        code_file = os.path.join(test_dir, '..', 'test_contracts/cardgame', 'cardgame.wasm')
        with open(code_file, 'rb') as f:
            code = f.read()
        abi_file = os.path.join(test_dir, '..', 'test_contracts/cardgame', 'cardgame.abi')
        with open(abi_file, 'rb') as f:
            abi = f.read()
        self.chain.deploy_contract(name, code, abi, vmtype=0)

        name = 'testmetestme'
        code = '''
def apply(receiver, code, action):
    if action == N('login'):
        call_contract('dothetesting', b'')
    elif action == N('startgame'):
        call_contract('dothetesting', b'')
    elif action == N('endgame'):
        call_contract('dothetesting', b'')
    elif action == N('playcard'):
        call_contract('dothetesting', b'')
        '''
        code = self.chain.compile_py_code(code)
        abi_file = os.path.join(test_dir, '..', 'test_contracts/cardgame', 'cardgame.abi')
        with open(abi_file, 'rb') as f:
            abi = f.read()
        self.chain.deploy_contract(name, code, abi, vmtype=1)
        args = {'username':'testmetestme'}

        self.chain.push_action(name, 'login', args, actor='testmetestme', perm='active')
        self.chain.produce_block()
        self.chain.push_action(name, 'login', args, actor='testmetestme', perm='active')
        self.chain.produce_block()
        self.chain.push_action(name, 'startgame', args, actor='testmetestme', perm='active')

        args = {'username':'testmetestme', 'player_card_idx':1}
        self.chain.push_action(name, 'playcard', args, actor='testmetestme', perm='active')


        args = {'username':'testmetestme'}
        self.chain.push_action(name, 'nextround', args, actor='testmetestme', perm='active')

        self.chain.push_action(name, 'endgame', args, actor='testmetestme', perm='active')
        return

    def test_cardgame3(self):
        name = 'testmetestme'
        code = self.chain.compile_py_code_from_file('cardgame/cardgame.py')
        logger.info(f'+++++++code size: {len(code)}')
        abi_file = os.path.join(test_dir, '..', 'test_contracts/cardgame', 'cardgame.abi')
        with open(abi_file, 'rb') as f:
            abi = f.read()
        self.chain.deploy_contract(name, code, abi, vmtype=1)
        args = {'username':'testmetestme'}

        self.chain.push_action(name, 'login', args, actor='testmetestme', perm='active')
        self.chain.produce_block()
        self.chain.push_action(name, 'login', args, actor='testmetestme', perm='active')
        self.chain.produce_block()
        self.chain.push_action(name, 'startgame', args, actor='testmetestme', perm='active')

        args = {'username':'testmetestme', 'player_card_idx':1}
        self.chain.push_action(name, 'playcard', args, actor='testmetestme', perm='active')


        args = {'username':'testmetestme'}
        self.chain.push_action(name, 'nextround', args, actor='testmetestme', perm='active')

        self.chain.push_action(name, 'endgame', args, actor='testmetestme', perm='active')
        return