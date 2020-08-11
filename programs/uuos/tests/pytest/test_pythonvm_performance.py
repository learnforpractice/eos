import os
import time
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

    def test_performance(self):
        code = '''
a = 'aaa bbb ccc d e fff h i Ending execution of tc: ppp aaaa  kkkkk jjjjj aaaa to aab'
        '''
        for i in range(100):
            code += f'''
def test{i}(a):
    aa = a.split(' ')
    return aa
            '''

        code += '''
def apply(receiver, code, action):
'''

        for i in range(100):
            code += f'''
    test{i}(a+' kkk ddd eee hhh iii')
            '''

        code = self.chain.compile_py_code(code)

        contract_name = 'testmetestme'
        self.chain.deploy_contract(contract_name, code, b'', vmtype=1)
        self.chain.push_action(contract_name, 'sayhello', b'hello,world from chain1')
        self.chain.push_action(contract_name, 'sayhello', b'hello,world')
