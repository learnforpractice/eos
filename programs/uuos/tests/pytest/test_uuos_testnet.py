import os
import sys
import json
import gc
import hashlib

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

import uuos
from uuos import application
from uuos import wasmcompiler

from chaintest import ChainTest

logger = application.get_logger(__name__)

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.main_token = 'UUOS'
        cls.chain = ChainTest(network_type=3)

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        pass

    def teardown_method(self, method):
        pass

    def test_create_account_with_uuos(self):
        '''
        testcase for account does not exist on EOS network
        '''
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        account = 'testtesttest'
        self.chain.create_account('uuos', account, key, key, 10*1024, 1, 10)

    def test_create_account(self):
        '''
        test for only  uuos can create account
        '''
        key = 'EOS7ent7keWbVgvptfYaMYeF2cenMBiwYKcwEuc11uCbStsFKsrmV'
        account = 'testtesttest1'
        try:
            self.chain.create_account('alice', account, key, key, 10*1024, 1, 10)
            assert 0
        except Exception as e:
                _except = e.args[0]['except']
                msg = _except['stack'][0]['data']['s']
                assert msg == 'only uuos can create new account!'
