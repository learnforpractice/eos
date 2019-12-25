import os
import sys
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos.config import Config

class Test(unittest.TestCase):

    def test_config(self):
        test_config = os.path.join(test_dir, 'test_config.ini')
        config = Config(test_config)


if __name__ == '__main__':
    unittest.main()


