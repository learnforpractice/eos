import io
import os
import struct
import ujson as json
from .jsonobject import JsonObject
from . import unpack_native_object

min_supported_version = 1
max_supported_version = 2

class BlockLog(object):

    def __init__(self, block_log_dir):
        self.block_log_dir = block_log_dir
        self.block_log_index_file = os.path.join(block_log_dir, 'blocks.index')
        self.block_log_file = os.path.join(block_log_dir, 'blocks.log')
        
        self.index_fd = open(self.block_log_index_file, 'rb+')
        self.fd = open(self.block_log_file, 'rb+')
        self._version = self.fd.read(4)
        self._version, = struct.unpack('I', self._version)

        assert self._version >= min_supported_version and self._version <= max_supported_version, \
                f"Unsupported version of block log. Block log version is {self._version} while code supports version(s) [{min_supported_version},{max_supported_version}]"

        if self._version > 1:
            self._first_block_num = self.fd.read(4)
            self._first_block_num, = struct.unpack('I', self._first_block_num)
        else:
            self._first_block_num = 1

        self.fd.seek(-8, io.SEEK_END)
        pos = self.fd.read(8)
        print(pos)
        pos, = struct.unpack('Q', pos)
        print(self._version, self._first_block_num, pos)

        self.index_fd.seek(-8, io.SEEK_END)
        pos = self.index_fd.read(8)
        pos, = struct.unpack('Q', pos)
        print(pos)

        # self.fd.seek(pos)
        # raw_block = self.fd.read(1024*1024)
        # block = unpack_native_object(7, raw_block)
        # print(block)

        self.index_fd.seek(-16, io.SEEK_END)
        pos = self.index_fd.read(8)
        pos, = struct.unpack('Q', pos)
        print(pos)

        self.index_fd.seek(0)
        pos = self.index_fd.read(8)
        pos, = struct.unpack('Q', pos)
        print(pos)
    
    @property
    def version(self):
        return self._version

    def get_block_count(self):
        print(self.block_log_index_file)
        size = os.path.getsize(self.block_log_index_file)
        assert size % 8 == 0
        return size // 8

    def get_first_block_num(self):
        return self._first_block_num

    @property
    def first_block_num(self):
        return self._first_block_num

    def get_head_block_num(self):
        return self._first_block_num + os.path.getsize(self.block_log_index_file)//8 - 1

    @property
    def head_block_num(self):
        return get_head_block_num()

    def read_block_by_id(self):
        pass

    def read_block_by_num(self, num):
        index = num - self._first_block_num
        self.index_fd.seek(index*8)
        index_data = self.index_fd.read(16)
        pos, next_pos = struct.unpack('QQ', index_data)
        block_size = next_pos - pos
        self.fd.seek(pos)
        raw_block = self.fd.read(block_size)
        block = unpack_native_object(7, raw_block)
        if block:
            return json.loads(block)

    def read_block_by_id(self, id):
        previous = id[:8]
        previous = bytes.fromhex(previous)
        previous = int.from_bytes(previous, 'big')
        return self.read_block_by_num(previous + 1)

    def extract_genesis_state(self):
        self.fd.seek(0)
        version = self.fd.read(4)
        version, = struct.unpack('I', version)
        if version != 1:
            first_block_num = self.fd.read(4)
        genesis_state = self.fd.read(1024*10)
        genesis_state = unpack_native_object(11, genesis_state)
        genesis_state = genesis_state.decode('utf8')
        genesis_state = json.loads(genesis_state)
        return JsonObject(genesis_state)

if __name__ == '__main__':
#    from uuos import blocklog
    blog = blocklog.BlockLog('dd-uuosmain/blocks')
    print(blog.extract_genesis_state())
