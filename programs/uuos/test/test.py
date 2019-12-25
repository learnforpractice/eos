import os
import sys
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos.config import Config

class MyTest(unittest.TestCase):

    def __init__(self, testName, extra_args=[]):
        super(MyTest, self).__init__(testName)
        self.extra_args = extra_args

    def test_config(self):
        sys.argv = ['python3', '--abi-serializer-max-time-ms=8888']
        test_config = os.path.join(test_dir, 'test_config.ini')
        config = Config(test_config)
        print(config.get_config())

if __name__ == '__main__':
    unittest.main()
    # suite = unittest.TestSuite()
    # suite.addTest(MyTest('test_config', []))
    # unittest.TextTestRunner(verbosity=2).run(suite)
