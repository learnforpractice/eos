import os
import time
import json
import platform

from uuosio.chaintester import ChainTester
from uuosio import log, uuos
logger = log.get_logger(__name__)

# print(os.getpid())
# input('<<<')

test_dir = os.path.dirname(__file__)

class TestMicropython(object):

    @classmethod
    def setup_class(cls):
        cls.tester = ChainTester()
        args = {"account": 'hello',
            "vmtype": 1,
            "vmversion": 0,
            "code": int.to_bytes(int(time.time()*1000), 8, 'little').hex()
        }
        r = cls.tester.push_action('eosio', 'setcode', args, {'hello':'active'})

        if platform.system() == 'Linux':
            cls.so = 'so'
        else:
            cls.so = 'dylib'
        
        with open(os.path.join(test_dir, 'activate_kv.wasm'), 'rb') as f:
            code = f.read()
            cls.tester.deploy_contract('alice', code, b'')
        cls.tester.push_action('eosio', 'setpriv', {'account':'alice', 'is_priv':True})
        cls.tester.push_action('alice', 'setkvparams', b'')
        cls.tester.push_action('eosio', 'setpriv', {'account':'alice', 'is_priv':False})

        args = {"account": 'alice',
            "vmtype": 1,
            "vmversion": 0,
            "code": int.to_bytes(int(time.time()*1000), 8, 'little').hex()
        }
        r = cls.tester.push_action('eosio', 'setcode', args, {'alice':'active'})

        cls.tester.produce_block()

    @classmethod
    def teardown_class(cls):
        cls.tester.free()

    def setup_method(self, method):
        logger.warning('test start: %s', method.__name__)

    def teardown_method(self, method):
        self.tester.produce_block()

    def test_mpy(self):
        code = '''
import chain
def apply(a, b, c):
    r = chain.kv_set('alice', 'hello', 'world', 'alice')
    print(r)
    r = chain.kv_get('alice', 'hello')
    print(r)
    r = chain.kv_get_data(0)
    print(r)
'''
        code = self.tester.mp_compile('alice', code)
        args = uuos.s2b('alice') + code
        self.tester.push_action('alice', 'setcode', args, {'alice':'active'})
        r = self.tester.push_action('alice', 'sayhello', b'', {'alice':'active'})

    def test_hello(self):
        r = self.tester.push_action('eosio.mpy', 'hellompy', b'', {'alice':'active'})
        logger.info(r['action_traces'][0]['console'])
