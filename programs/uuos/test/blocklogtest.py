import os
import sys
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos.blocklog import BlockLog

if __name__ == '__main__':
    log = BlockLog('dd-eos/blocks')
    # count = log.get_block_count()
    print(log.get_first_block_num(), log.get_head_block_num())

