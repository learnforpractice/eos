import os
import sys
import gc
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos import application
from chaintest import ChainTest

logger = application.get_logger(__name__)

class DBTest(ChainTest):

    def test_vm_api(self):
        code = '''
import db
def apply(receiver, code, action):
    code = receiver
    scope = 'scope'
    table = 'table'
    payer = receiver
    id = 'hello'
    data = read_action_data()
    if action == N('destroy'):
        while True:
            itr = db.end_i64(code, scope, table)
            if itr == -1:
                break
            itr, primary = db.previous_i64(itr)
            if itr < 0:
                break
            data = db.get_i64(itr)
            print(itr, data)
            db.remove_i64(itr)
        print('destroy done!')
    elif action == N('init'):
        db.store_i64(scope, table, payer, 1, b'1')
        db.store_i64(scope, table, payer, 3, b'3')
        db.store_i64(scope, table, payer, 5, b'5')
        db.store_i64(scope, table, payer, 7, b'7')
        print('init done!')
    elif action == N('test'):
        itr = db.upperbound_i64(code, scope, table, 3)
        data = db.get_i64(itr)
        print('upperbound of 3:', data)
        assert data == b'5'

        itr = db.upperbound_i64(code, scope, table, 2)
        data = db.get_i64(itr)
        print('upperbound of 2:', data)
        assert data == b'3'

        itr = db.lowerbound_i64(code, scope, table, 3)
        data = db.get_i64(itr)
        print('lowerbound of 3:', data)
        assert data == b'3'

        itr = db.lowerbound_i64(code, scope, table, 2)
        data = db.get_i64(itr)
        print('lowerbound of 2:', data)
        assert data == b'3'
    elif action == N('test1'):
        print('+++++++++test1')
        itr = db.end_i64(code, scope, table)
        assert itr != -1
        while True:
            itr, primary = db.previous_i64(itr)
            if itr < 0:
                break
            data = db.get_i64(itr)
            print(itr, data)
    elif action == N('test2'):
        print('+++++++++test2')
        itr = db.lowerbound_i64(code, scope, table, 1);
        assert itr != -1
        data = db.get_i64(itr)
        print(data)
        while True:
            itr, primary = db.next_i64(itr)
            if itr < 0:
                break
            data = db.get_i64(itr)
            print(primary, data)
        '''
        name = 'helloworld11'
        code = self.compile_py_code(code)
        self.deploy_contract(name, code, b'', 1)
        r = self.push_action(name, 'destroy', b'')
        r = self.push_action(name, 'init', b'hello,world')
        self.push_action(name, 'test', b'')
        self.push_action(name, 'test1', b'')
        self.push_action(name, 'test2', b'')
        self.produce_block()

        self.push_action(name, 'destroy', b'')
        self.produce_block()

class DBTesterCase(unittest.TestCase):
    def __init__(self, testName):
        logger.info('+++++++++++++++++++++DBTesterCase++++++++++++++++')
        super(DBTesterCase, self).__init__(testName)

    def test_vm_api(self):
        logger.info('+++++++++++++db_test1+++++++++++++++')
        DBTesterCase.chain.test_vm_api()

    @classmethod
    def setUpClass(cls):
        logger.info('++++++++++++setUpClass++++++++++++')
        cls.chain = DBTest()

    @classmethod
    def tearDownClass(cls):
        if cls.chain:
            cls.chain.free()
            cls.chain = None

    def setUp(self):
        pass

    def tearDown(self):
        pass

if __name__ == '__main__':
    unittest.main()
