import os
import sys
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

from uuos.blocklog import BlockLog

if __name__ == '__main__':
    log = BlockLog('dd-eos/blocks')
    # count = log.get_block_count()
    print(log.first_block_num, log.get_head_block_num())
    block = log.read_block_by_num(log.first_block_num)
    print(block['previous'])

    previous = block['previous'][:8]
    previous = bytes.fromhex(previous)
    previous = int.from_bytes(previous, 'big')
    print(previous)

    block = log.read_block_by_num(log.first_block_num+1)
    print(block)
    
    block = log.read_block_by_id(block['previous'])
    print(block)