import json
import struct
from db import *

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

def apply(receiver, code, action):
    code = name('alice')
    scope = name('scope1')
    table = name('table')
    payer = name('alice')

    if True:
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

    if True:
        table = name('table2')
        mi = MultiIndex(code, scope, table, MyData2)
        d = MyData2(1, 2, 3, 5.0)
        d.payer = payer
        mi[1] = d

        print(mi[1])
        itr, primary, secondary = mi.idx_lowerbound(3, 1.0)
        print(itr, primary, secondary)
