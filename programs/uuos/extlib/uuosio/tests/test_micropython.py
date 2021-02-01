import os
import json
from chaintester import ChainTester
from uuosio import log, uuos
logger = log.get_logger(__name__)

class TestMicropython(object):

    @classmethod
    def setup_class(cls):
        cls.tester = ChainTester()

    @classmethod
    def teardown_class(cls):
        cls.tester.free()

    def setup_method(self, method):
        logger.warning('test start: %s', method.__name__)

    def teardown_method(self, method):
        pass

    def test_mpy(self):
        args = {"account": 'hello',
            "vmtype": 1,
            "vmversion": 0,
            "code": b'aa'.hex()
        }
        r = self.tester.push_action('eosio', 'setcode', args, {'hello':'active'})

        code = '''
def apply(a, b, c):
    print('hello,world')
'''
        code = self.tester.mp_compile('hello', code)
        args = uuos.s2b('hello') + code
        self.tester.push_action('hello', 'setcode', args, {'hello':'active'})
        r = self.tester.push_action('hello', 'sayhello', b'', {'hello':'active'})
        print(r['elapsed'])

    def test_setcode(self):
        args = {"account": 'hello',
            "vmtype": 1,
            "vmversion": 0,
            "code": b'aa'.hex()
        }
        r = self.tester.push_action('eosio', 'setcode', args, {'hello':'active'})
        r = self.tester.push_action('hello', 'hellompy', b'', {'hello':'active'})
        logger.info(r['action_traces'][0]['console'])

    def test_hello(self):
        r = self.tester.push_action('eosio.mpy', 'hellompy', b'', {'hello':'active'})
        logger.info(r['action_traces'][0]['console'])
        self.tester.produce_block()

