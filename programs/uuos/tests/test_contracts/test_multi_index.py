import struct
from chain import *

idx64 = 0
idx128 = 1
idx256 = 2
idx_double = 3
idx_long_double = 4

class MultiIndex:
    def __init__(self, code, scope, table, data_type):
        self.code = code
        self.scope = scope
        self.table = table
        self.indexes = data_type.get_secondary_indexes()
        self.data_type = data_type
        self.primary_key = 0
        self.idx_tables = []
        for i in range(len(self.indexes)):
            table = (int(self.table) & 0xFFFFFFFFFFFFFFF0) or i
            self.idx_tables.append(table)

    def find(self, primary_key):
        return db_find_i64(self.code, self.scope, self.table, primary_key)

    def get(self, itr):
        if itr < 0:
            raise IndexError
        data = db_get_i64(itr)
        return self.data_type.unpack(data)

    def get_secondary_values(self, primary_key):
        values = []
        for idx in self.indexes:
            itr, secondary = db_idx_find_primary(self.code, self.scope, table, primary_key)
            if itr < 0:
                return None
            values.append(secondary)
        return values

    def __getitem__(self, primary_key):
        itr = self.find(primary_key)
        if itr < 0:
            raise IndexError
        return self.get(itr)

    def __setitem__(self, primary_key, obj):
        assert primary_key == obj.get_primary_key()
        self.store(obj)

    def store(self, obj):
        primary = obj.get_primary_key()
        itr = db_find_i64(self.code, self.scope, self.table, primary)        
        if itr < 0:
            db_store_i64(self.scope, self.table, obj.payer, primary, obj.pack())
            i = 0
            for idx in self.indexes:
                table = self.idx_tables[i]
                db_idx_store(idx, self.scope, table, obj.payer, primary, obj.get_secondary_values()[i])
                i += 1
        else:
            db_update_i64(itr, obj.payer, obj.pack())
            i = 0
            for idx in self.indexes:
                table = self.idx_tables[i]
                itr, old_secondary = db_idx_find_primary(idx, self.code, self.scope, table, primary)
                secondary = obj.get_secondary_values()[i]
                if not secondary == old_secondary:
                    db_idx_update(idx, itr, obj.payer, secondary)
                i += 1

    def __delitem__(self, primary_key):
        itr = db_find_i64(self.code, self.scope, self.table, primary_key)
        if itr < 0:
            raise IndexError

        i = 0
        for idx in self.indexes:
            table = self.idx_tables[i]
            itr, _ = db_idx_find_primary(idx, self.code, self.scope, table, primary_key)
            assert itr >= 0
            db_idx_remove(idx, itr)
            i += 1

    def __contains__(self, primary_key):
        return db_find_i64(self.code, self.scope, self.table, primary_key) >= 0

    def __iter__(self):
        self.itr = db_end_i64(self.code, self.scope, self.table)
        return self

    def __next__(self):
        if self.itr == -1:
            raise StopIteration
        self.itr, self.primary_key = db_previous_i64(self.itr)
        if self.itr < 0:
            raise StopIteration
        return self.get(self.itr)

    def get_secondary_index(self, idx):
        return SecondaryIndex(self, self.indexes[idx], self.data_type)

    def upperbound(self, primary):
        return db_upperbound_i64(self.code, self.scope, self.table, primary)

    def lowerbound(self, primary):
        return db_lowerbound_i64(self.code, self.scope, self.table, primary)

    def idx_find(self, index, secondary_key):
        idx = self.indexes[index]
        return db_idx_find_secondary(idx, self.code, self.scope, self.table, secondary_key)

    def idx_upperbound(self, index, secondary_key):
        idx = self.indexes[index]
        return db_idx_upperbound(idx, self.code, self.scope, self.table, secondary_key)

    def idx_lowerbound(self, index, secondary_key):
        idx = self.indexes[index]
        return db_idx_lowerbound(idx, self.code, self.scope, self.table, secondary_key)

class MyData(object):
    def __init__(self, a: int, b: int, c: int, d: float):
        self.a = a
        self.b = b
        self.c = c
        self.d = d
        self.payer = 0

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

def apply(receiver, code, action):
    code = name('alice')
    scope = name('scope')
    table = name('table')
    payer = name('alice')

    mi = MultiIndex(code, scope, table, MyData)

    try:
        itr = mi.find(1)
        if itr >= 0:
            data = mi.get(itr)
            print(data.a)
    except Exception as e:
        print(e)

    d = MyData(1, 2, 3, 5.0)
    d.payer = payer
    mi[1] = d

    itr, primary = mi.idx_find(3, 4.0)
    print(itr, primary)

    itr, primary, secondary = mi.idx_lowerbound(3, 1.0)
    print(itr, primary, secondary)


