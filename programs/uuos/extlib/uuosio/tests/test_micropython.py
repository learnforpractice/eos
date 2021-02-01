import os
import time
import json
from uuosio.chaintester import ChainTester
from uuosio import log, uuos
logger = log.get_logger(__name__)

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

    @classmethod
    def teardown_class(cls):
        cls.tester.free()

    def setup_method(self, method):
        logger.warning('test start: %s', method.__name__)

    def teardown_method(self, method):
        self.tester.produce_block()

    def test_mpy(self):
        code = '''
def apply(a, b, c):
    print('hello,world')
'''
        code = self.tester.mp_compile('hello', code)
        args = uuos.s2b('hello') + code
        self.tester.push_action('hello', 'setcode', args, {'hello':'active'})
        r = self.tester.push_action('hello', 'sayhello', b'', {'hello':'active'})

        code = '''
def apply(a, b, c):
    print('hello,world from alice')
'''
        code = self.tester.mp_compile('hello', code)
        args = uuos.s2b('alice') + code
        self.tester.push_action('hello', 'setcode', args, {'alice':'active'})

        args = uuos.s2b('alice')
        r = self.tester.push_action('hello', 'exec', args, {'hello':'active'})
        logger.info(r['action_traces'][0]['console'])
        print(r['elapsed'])

    def test_setcode(self):
        r = self.tester.push_action('hello', 'hellompy', b'', {'hello':'active'})
        logger.info(r['action_traces'][0]['console'])

    def test_hello(self):
        r = self.tester.push_action('eosio.mpy', 'hellompy', b'', {'hello':'active'})
        logger.info(r['action_traces'][0]['console'])

