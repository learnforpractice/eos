from _db import *
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
        itr, primary = _mi.idx_find(self.mi.ptr, self.index, secondary_key)
        return itr >= 0
        
    def __iter__(self):
        self.itr = _mi.idx_end(self.mi.ptr, self.index)
        return self

    def __next__(self):
        if self.itr == -1:
            raise StopIteration
<<<<<<< HEAD
        self.itr, self.primary_key = _mi.idx_previous(self.mi.ptr, self.index, self.itr)
        if self.itr < 0:
            raise StopIteration
        itr_primary = self.mi.find(self.primary_key)
        return self.mi.get(itr_primary)

class MultiIndex:
=======
        self.itr, self.key = idx64_previous(self.itr)
        if self.itr < 0:
            raise StopIteration
        return self.key


class Idx:

    def __init__(self, primary_index, index, index_type=64):
        self.primary_index = primary_index
        self.code = primary_index.code
        self.scope = primary_index.scope
        self.table = (primary_index.table&0xFFFFFFFFFFFFFFF0)+index
        self.payer = 0
        if index_type == 64:
            self.store = idx64_store
            self.update = idx64_update
            self.remove = idx64_remove
            self.next = idx64_next
            self.previous = idx64_previous
            self.find_primary = idx64_find_primary
            self.find_secondary = idx64_find_secondary
            self.lowerbound = idx64_lowerbound
            self.upperbound = idx64_upperbound
            self.end = idx64_end
        elif index_type == 128:
            self.store = idx128_store
            self.update = idx128_update
            self.remove = idx128_remove
            self.next = idx128_next
            self.previous = idx128_previous
            self.find_primary = idx128_find_primary
            self.find_secondary = idx128_find_secondary
            self.lowerbound = idx128_lowerbound
            self.upperbound = idx128_upperbound
            self.end = idx128_end
        elif index_type == 256:
            self.store = idx256_store
            self.update = idx256_update
            self.remove = idx256_remove
            self.next = idx256_next
            self.previous = idx256_previous
            self.find_primary = idx256_find_primary
            self.find_secondary = idx256_find_secondary
            self.lowerbound = idx256_lowerbound
            self.upperbound = idx256_upperbound
            self.end = idx256_end

    def __getitem__(self, secondary):
        itr, primary = self.find_secondary(self.code, self.scope, self.table, secondary)
        if itr >= 0:
            return self.primary_index[primary]
        raise KeyError

    def __setitem__(self, key, value):
        itr, secondary = self.find_primary(self.code, self.scope, self.table, key)
        if itr >= 0:
            if secondary == value:
                return
            self.update(itr, self.payer, value)
        else:
            self.store(self.scope, self.table, self.payer, key, value)

    def __delitem__(self, key):
        itr, secondary = self.find_primary(self.code, self.scope, self.table, key)
        if itr >= 0:
            return self.remove(itr)
        raise KeyError

    def __contains__(self, secondary):
        itr, primary = self.find_secondary(self.code, self.scope, self.table, secondary)
        return itr >= 0

    def __iter__(self):
        self.itr = self.end(self.code, self.scope, self.table)
        return self

    def __next__(self):
        if self.itr == -1:
            raise StopIteration
        self.itr, self.key = self.previous(self.itr)
        if self.itr < 0:
            raise StopIteration
        return self.primary_index[self.key]

class MultiIndex:

>>>>>>> master-uuos
    def __init__(self, code, scope, table, data_type):
        self.code = code
        self.scope = scope
        self.table = table
<<<<<<< HEAD
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
        self.itr = _mi.end(self.ptr)
=======
        self.primary_index = DBi64(code, scope, table, data_type)
        self.indexes = []
        index = 0
        for index_type in data_type.get_secondary_indexes():
            index_obj = Idx(self.primary_index, index, index_type)
            self.indexes.append(index_obj)
            index += 1
        self.data_type = data_type

    def get_primary_index(self):
        return self.primary_index

    def get_secondary_index(self, index):
        return self.indexes[index]

    def __getitem__(self, key):
        return self.primary_index[key]

    def __setitem__(self, key, obj):
#        if key in self.primary_index:
#            self.__delitem__(key)
        self.primary_index[key] = obj
        i = 0
        for index in self.indexes:
            index.payer = obj.payer
            index[key] = obj.get_secondary_value(i)
            i += 1

    def store(self, obj):
        self.__setitem__(obj.get_primary_value(), obj)

    def __delitem__(self, key):
        obj = self.primary_index[key]
        del self.primary_index[key]
        i = 0
        for index in self.indexes:
            secondary_key = obj.get_secondary_value(i)
            del index[secondary_key]
            i += 1

    def __contains__(self, id):
        itr = find_i64(self.code, self.scope, self.table, id)
        return itr >= 0

    def __iter__(self):
        self.itr = end_i64(self.code, self.scope, self.table)
>>>>>>> master-uuos
        return self

    def __next__(self):
        if self.itr == -1:
            raise StopIteration
<<<<<<< HEAD
        self.itr, self.primary_key = _mi.previous(self.ptr, self.itr)
        if self.itr < 0:
            raise StopIteration
        return self.get(self.itr)

    def get_secondary_index(self, idx):
        return SecondaryIndex(self, idx, self.data_type)
        
    def upperbound(self, primary):
        return _mi.upperbound(self.ptr, primary)

    def lowerbound(self, primary):
        return _mi.lowerbound(self.ptr, primary)

    def idx_upperbound(self, index, secondary_key):
        return _mi.idx_upperbound(self.ptr, index, secondary_key)

    def idx_lowerbound(self, index, secondary_key):
        return _mi.idx_lowerbound(self.ptr, index, secondary_key)

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
=======
        self.itr, self.key = previous_i64(self.itr)
        if self.itr < 0:
            raise StopIteration
        return self.primary_index[self.key]
>>>>>>> master-uuos
