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

    def get(self, secondary_key):
        itr, primary_key = _mi.idx_find(self.mi.ptr, self.index, secondary_key)
        if itr < 0:
            raise IndexError
        return self.mi[primary_key]

    def __getitem__(self, secondary_key):
        return self.get(secondary_key)

    def __contains__(self, secondary_key):
        return _mi.idx_find(self.mi.ptr, secondary_key) >= 0

    def __iter__(self):
        self.itr = _mi.idx_end(self.mi.ptr, self.code, self.scope, self.table)
        return self

    def __next__(self):
        if self.itr == -1:
            raise StopIteration
        self.itr, self.primary_key = _mi.idx_previous(self.itr)
        if self.itr < 0:
            raise StopIteration
        return self.mi.get(self.primary_key)

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
        print('++++++del', itr, primary_key)
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

