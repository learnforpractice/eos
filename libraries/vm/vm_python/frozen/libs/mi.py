import _mi
import struct

idx64 = 0
idx128 = 1
idx256 = 2
idx_double = 3
idx_long_double = 4

class SecondaryIndex:
    def __init__(self, mi, secondary_index, data_type):
        self.mi = mi
        self.index = secondary_index
        self.data_type = data_type
        self.idx_table = (self.mi.table&0xFFFFFFFFFFFFFFF0) + secondary_index
    def get(self, secondary_key):
        itr, primary_key = _mi.idx_find(self.mi.ptr, self.index, secondary_key)
        if itr < 0:
            raise IndexError
        return self.mi[primary_key]

    def __getitem__(self, secondary_key):
        return self.get(secondary_key)

    def __contains__(self, secondary_key):
        itr, primary = _mi.idx_find(self.mi.ptr, self.index, secondary_key)
        return itr >= 0
        
    def __iter__(self):
        self.itr = _mi.idx_end(self.mi.ptr, self.index, self.mi.code, self.mi.scope, self.idx_table)
        return self

    def __next__(self):
        if self.itr == -1:
            raise StopIteration
        self.itr, self.primary_key = _mi.idx_previous(self.mi.ptr, self.index, self.itr)
        if self.itr < 0:
            raise StopIteration
        itr_primary = self.mi.find(self.primary_key)
        return self.mi.get(itr_primary)

class MultiIndex:
    def __init__(self, code, scope, table, data_type):
        self.code = code
        self.scope = scope
        self.table = table
        self.indexes = data_type.get_secondary_indexes()
        self.ptr = _mi.new(code, scope, table, self.indexes)
        self.data_type = data_type
        self.primary_key = 0

    def find(self, primary_key):
        return _mi.find(self.ptr, primary_key)

    def get(self, itr):
        if itr < 0:
            raise IndexError
        data = _mi.get(self.ptr, itr)
        return self.data_type.unpack(data)

    def get_secondary_values(self, primary_key):
        return _mi.get_secondary_values(self.ptr, primary_key)

    def __getitem__(self, primary_key):
        itr = self.find(primary_key)
        if itr < 0:
            raise IndexError
        return self.get(itr)

    def __setitem__(self, primary_key, obj):
        assert primary_key == obj.get_primary_key()
        self.store(obj)

    def store(self, obj):
        itr = _mi.find(self.ptr, obj.get_primary_key())        
        if itr < 0:
            _mi.store(self.ptr, obj.get_primary_key(), obj.pack(), obj.get_secondary_values(), obj.payer)
        else:
            _mi.modify(self.ptr, itr, obj.get_primary_key(), obj.pack(), obj.get_secondary_values(), obj.payer)

    def __delitem__(self, primary_key):
        itr = _mi.find(self.ptr, primary_key)
        if itr < 0:
            raise IndexError
        _mi.erase(self.ptr, itr, primary_key)

    def __contains__(self, primary_key):
        return _mi.find(self.ptr, primary_key) >= 0

    def __iter__(self):
        self.itr = _mi.end(self.ptr, self.code, self.scope, self.table)
        return self

    def __next__(self):
        if self.itr == -1:
            raise StopIteration
        self.itr, self.primary_key = _mi.previous(self.ptr, self.itr)
        if self.itr < 0:
            raise StopIteration
        return self.get(self.itr)

    def get_secondary_index(self, idx):
        return SecondaryIndex(self, idx, self.data_type)

class MyData(object):
    def __init__(self, a: int, b: int, c: int, d: float):
        self.a = a
        self.b = b
        self.c = c
        self.d = d

    def pack(self):
        b = int.to_bytes(self.b, 16, 'little')
        c = int.to_bytes(self.c, 32, 'little')
        return struct.pack('Q16s32sd', self.a, b, c, self.d)

    @classmethod
    def unpack(cls, data):
        a, b, c, d = struct.unpack('Q16s32sd', data)
        b = int.from_bytes(b, 'little')
        c = int.from_bytes(c, 'little')
        return MyData(a, b, c, d)

    def get_primary_key(self):
        return self.a

    def get_secondary_values(self):
        return (self.a, self.b, self.c, self.d)

    @classmethod
    def get_secondary_indexes(self):
        return (idx64, idx128, idx256, idx_double)