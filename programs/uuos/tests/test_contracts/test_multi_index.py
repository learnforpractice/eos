import json
import struct
from chain import *

idx64 = 0
idx128 = 1
idx256 = 2
idx_double = 3
idx_long_double = 4

primary_type_i64 = 0
primary_type_i256 = 1

class ChainDB(object):
    def __init__(self, primary_type, code, scope, table, data_type):
        self.code = code
        self.scope = scope
        self.table = table
        self.data_type = data_type
        self.primary_type = primary_type

        if primary_type == primary_type_i64:
            self.db_find = db_find_i64
            self.db_get = db_get_i64
            self.db_store = db_store_i64
            self.db_update = db_update_i64
            self.db_remove = db_remove_i64
        else:
            self.db_find = db_find_i256
            self.db_get = db_get_i256
            self.db_store = db_store_i256
            self.db_update = db_update_i256
            self.db_remove = db_remove_i256

    def find(self, primary_key):
        return self.db_find(self.code, self.scope, self.table, primary_key)

    def get(self, itr):
        if itr < 0:
            raise IndexError
        data = self.db_get(itr)
        return self.data_type.unpack(data)

    def load(self, primary_key):
        itr = self.find(primary_key)
        if itr < 0:
            return None
        return self.get(itr)

    def store(self, obj):
        primary_key = obj.get_primary_key()
        itr = self.db_find(self.code, self.scope, self.table, primary_key)
        if itr < 0:
            self.db_store(self.scope, self.table, obj.payer, primary_key, obj.pack())
        else:
            self.db_update(itr, obj.payer, obj.pack())

    def remove(self, primary_key):
        itr = self.db_find(self.code, self.scope, self.table, primary_key)
        if itr < 0:
            raise IndexError
        self.db_remove(itr)

class ChainDBI64(ChainDB):
    def __init__(self, code, scope, table, data_type):
        ChainDB.__init__(self, primary_type_i64, code, scope, table, data_type)

class ChainDBI256(ChainDB):
    def __init__(self, code, scope, table, data_type):
        ChainDB.__init__(self, primary_type_i256, code, scope, table, data_type)

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
        i = 0
        for idx in self.indexes:
            table = self.idx_tables[i]
            itr, secondary = db_idx_find_primary(idx, self.code, self.scope, table, primary_key)
            if itr < 0:
                return None
            values.append(secondary)
            i += 1
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

    def remove(self, primary_key):
        itr = db_find_i64(self.code, self.scope, self.table, primary_key)
        if itr < 0:
            raise IndexError
        db_remove_i64(itr)
        i = 0
        for idx in self.indexes:
            table = self.idx_tables[i]
            itr, _ = db_idx_find_primary(idx, self.code, self.scope, table, primary_key)
            assert itr >= 0
            db_idx_remove(idx, itr)
            i += 1

    def __delitem__(self, primary_key):
        self.remove(primary_key)

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
        return cls(a, b, c, d)

    def get_primary_key(self):
        return self.a

    def get_secondary_values(self):
        return (self.a, self.b, self.c, self.d)

    @staticmethod
    def get_secondary_indexes():
        return (idx64, idx128, idx256, idx_double)

class MyData2(object):
    def __init__(self, a: int, b: int, c: int, d: float):
        self.a = a
        self.b = b
        self.c = c
        self.d = d
        self.payer = 0

    def pack(self):
        data = (self.a, self.b, self.c, self.d)
        return json.dumps(data)

    @classmethod
    def unpack(cls, data):
        data = json.loads(data)
        return cls(data[0], data[1], data[2], data[3])

    def get_primary_key(self):
        return self.a

    def get_secondary_values(self):
        return (self.a, self.b, self.c, self.d)

    @staticmethod
    def get_secondary_indexes():
        return (idx64, idx128, idx256, idx_double)

    def __repr__(self):
        return (self.a, self.b, self.c, self.d)

    def __str__(self):
        data = (self.a, self.b, self.c, self.d)
        return json.dumps(data)

class MyDataI64(object):
    def __init__(self, a: int, b: int, c: int, d: float):
        self.a = a
        self.b = b
        self.c = c
        self.d = d
        self.payer = 0

    def pack(self):
        data = (self.a, self.b, self.c, self.d)
        return json.dumps(data)

    @classmethod
    def unpack(cls, data):
        data = json.loads(data)
        return cls(data[0], data[1], data[2], data[3])

    def get_primary_key(self):
        return self.a

    def __repr__(self):
        return (self.a, self.b, self.c, self.d)

    def __str__(self):
        data = (self.a, self.b, self.c, self.d)
        return json.dumps(data)

class MyDataI256(object):
    def __init__(self, a: int, b: int, c: int, d: float):
        self.a = a
        self.b = b
        self.c = c
        self.d = d
        self.payer = 0

    def pack(self):
        data = (self.a, self.b, self.c, self.d)
        return json.dumps(data)

    @classmethod
    def unpack(cls, data):
        data = json.loads(data)
        return cls(data[0], data[1], data[2], data[3])

    def get_primary_key(self):
        return self.a

    def __repr__(self):
        return (self.a, self.b, self.c, self.d)

    def __str__(self):
        data = (self.a, self.b, self.c, self.d)
        return json.dumps(data)

test1 = name('test1')
test2 = name('test2')
test3 = name('test3')
test4 = name('test4')

def apply(receiver, code, action):
    code = name('alice')
    scope = name('scope')
    payer = name('alice')

    if action == test1:
        table = name('table')
        mi = MultiIndex(code, scope, table, MyData)

        try:
            itr = mi.find(1)
            if itr >= 0:
                data = mi.get(itr)
                print(data.a, data.b, data.c, data.d)
        except Exception as e:
            print(e)

        d = MyData(1, 2, 3, 5.0)
        d.payer = payer
        mi[1] = d

        itr, primary = mi.idx_find(3, 4.0)
        print(itr, primary)

        itr, primary, secondary = mi.idx_lowerbound(3, 1.0)
        print(itr, primary, secondary)

    elif action == test2:
        table = name('table2')
        mi = MultiIndex(code, scope, table, MyData2)
        d = MyData2(1, 2, 3, 5.0)
        d.payer = payer
        mi[1] = d

        print(mi[1])
        itr, primary, secondary = mi.idx_lowerbound(3, 1.0)
        print(itr, primary, secondary)
    elif action == test3:
        table = name('table3')
        db = ChainDBI64(code, scope, table, MyDataI64)
        d = MyDataI64(1, 2, 3, 5.0)
        d.payer = payer
        db.store(d)

        print(db.load(1))
    elif action == test4:
        table = name('table3')
        db = ChainDBI256(code, scope, table, MyDataI64)

        primary_key = 0xffffffffffffffffff
        d = MyDataI256(0xffffffffffffffffff, 2, 3, 5.0)
        d.payer = payer
        db.store(d)
        print(db.load(primary_key))
