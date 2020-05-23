import struct
import db

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

    @property
    def primary_key(self):
        return self.a

    # @x.setter
    # def primary_key(self, a):
    #     pass

    def get_primary_key(self):
        return self.a

    def get_secondary_values(self):
        return ()

    def set_secondary_value(self, idx):
        if idx == 0:
            pass
        elif idx == 1:
            pass
        elif idx == 2:
            pass
        elif idx == 3:
            pass

    @classmethod
    def get_secondary_indexes(self):
        return ()

code = N('helloworld11')
scope = N('scopee')
table = N('tableee')
mi = db.MultiIndex(code, scope, table, MyData)

def apply(receiver, code, action):
    payer = receiver
    if action == N('store'):
        primary_key = 112
        d = MyData(primary_key, 2, 3, 5.0)
        d.payer = receiver
        mi.store(d)
    elif action == N('get'):
        itr = mi.find(112)
#        print(itr)
        data = mi.get(itr)
#        print(data.a, data.b, data.c, data.d)
    elif action == N('update'):
        itr = mi.find(112)
        print(itr)
        data = mi.get(itr)
        print('+++b:', data.b)
        data.payer = payer
        data.b += 1
        mi.store(data)
