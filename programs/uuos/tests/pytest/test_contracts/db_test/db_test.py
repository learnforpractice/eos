import _mi
import struct

idx64 = 0
idx128 = 1
idx256 = 2
idx_double = 3
idx_long_double = 4
import db
from db import MultiIndex


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

    @staticmethod
    def unpack(data):
        a, b, c, d = struct.unpack('Q16s32sd', data)
        b = int.from_bytes(b, 'little')
        c = int.from_bytes(c, 'little')
        return MyData(a, b, c, d)

    def get_primary_key(self):
        return self.a

    def get_secondary_values(self):
        return (self.a, self.b, self.c, self.d)

    def set_secondary_value(self, idx):
        if idx == 0:
            pass
        elif idx == 1:
            pass
        elif idx == 2:
            pass
        elif idx == 3:
            pass

    @staticmethod
    def get_secondary_indexes():
        return (idx64, idx128, idx256, idx_double)


def test_del(receiver, code, action):
    payer = receiver

    code = N('testmetestme')
    scope = N('scopee')
    table = N('tableee')
    primary_key =  N('hello')
    mi = MultiIndex(code, scope, table, MyData)

    primary_key = 112
    d = MyData(primary_key, 2, 3, 5.0)
    d.payer = receiver
    mi.store(d)

    try:
        del mi[primary_key]
    except IndexError:
        print('primary key not exists!')
    return


def get_secondary_values_test(receiver, code, action):
    payer = receiver
    code = N('testmetestme')
    scope = N('scopee')
    table = N('tableee')
    primary_key =  110
    mi = MultiIndex(code, scope, table, MyData)
    values = mi.get_secondary_values(primary_key)
    print(values)


def test(receiver, code, action):
    payer = receiver
    code = N('testmetestme')
    scope = N('scopee')
    table = N('mytable')
    table = table&0xFFFFFFFFFFFFFFF0
    itr, primary = db.idx128_find_primary(code, scope, table + 1, 113)
    print(itr, primary)

    itr, primary = db.idx128_find_primary(code, scope, table + 1, 113)
    print(itr, primary)

    itr = db.idx128_end(code, scope, table+1)
    print(itr)
    itr = db.idx128_previous(itr)
    print(itr)


def test2(receiver, code, action):
    payer = receiver

    code = N('testmetestme')
    scope = N('scopee')
    table = N('mytable')

    ptr = _mi.new(code, scope, table, (idx64, idx128, idx256, idx_double))
    print(ptr)
    itr = _mi.idx_end(ptr, 1, code, scope, table)
    print('itr', itr)
    itr, primary = _mi.idx_previous(ptr, 1, itr)
    print(itr, primary)    


def secondary_test(receiver, code, action):
    payer = receiver
    code = N('testmetestme')
    scope = N('scopee')
    table = N('mytable')
    mi = MultiIndex(code, scope, table, MyData)

    sec = mi.get_secondary_index(0)
    print('+++++++++by secondary 0')
    for item in sec:
        print('+++++++++by secondary index 0:', item.a, item.b, item.c, item.d)

    sec = mi.get_secondary_index(1)
    print('+++++++++by secondary 1')
    for item in sec:
        print('+++++++++by secondary index 1:', item.a, item.b, item.c, item.d)

    sec = mi.get_secondary_index(2)
    print('+++++++++by secondary 2')
    for item in sec:
        print('+++++++++by secondary index 2:', item.a, item.b, item.c, item.d)

    sec = mi.get_secondary_index(3)
    print('+++++++++by secondary 3')
    for item in sec:
        print('+++++++++by secondary index 3:', item.a, item.b, item.c, item.d)
    return


def bound_tests(receiver, code, action):
    payer = receiver
    code = N('testmetestme')
    scope = N('scopee')
    table = N('mytable')
    mi = MultiIndex(code, scope, table, MyData)
    print('+++++++++++++bound tests+++++++++++')
    
    itr = mi.lowerbound(11)
    item = mi.get(itr)
    print('lowerbound of 11:', item.a, item.b, item.c, item.d)

    itr = mi.lowerbound(22)
    item = mi.get(itr)
    print('lowerbound of 22:', item.a, item.b, item.c, item.d)

    itr = mi.upperbound(11)
    item = mi.get(itr)
    print('upperbound of 11:', item.a, item.b, item.c, item.d)

    itr = mi.upperbound(22)
    item = mi.get(itr)
    print('upperbound of 22:', item.a, item.b, item.c, item.d)

    itr = mi.upperbound(33)
    item = mi.get(itr)
    print('upperbound of 33:', item.a, item.b, item.c, item.d)

    itr, primary = mi.idx_lowerbound(0, 11)
    item = mi[primary]
    print('idx_lowerbound of 0 11:', item.a, item.b, item.c, item.d)

    itr, primary = mi.idx_lowerbound(0, 22)
    item = mi[primary]
    print('idx_lowerbound of 0 22:', item.a, item.b, item.c, item.d)


    itr, primary = mi.idx_lowerbound(1, 1)
    item = mi[primary]
    print('idx_lowerbound of 1 1:', item.a, item.b, item.c, item.d)

    itr, primary = mi.idx_lowerbound(1, 2)
    item = mi[primary]
    print('idx_lowerbound of 1 2:', item.a, item.b, item.c, item.d)

    itr, primary = mi.idx_lowerbound(2, 1)
    item = mi[primary]
    print('idx_lowerbound of 2 1:', item.a, item.b, item.c, item.d)

    itr, primary = mi.idx_lowerbound(2, 2)
    item = mi[primary]
    print('idx_lowerbound of 2 2:', item.a, item.b, item.c, item.d)


    itr, primary = mi.idx_lowerbound(3, 1.0)
    item = mi[primary]
    print('idx_lowerbound of 3 1.0:', item.a, item.b, item.c, item.d)

    itr, primary = mi.idx_lowerbound(3, 2.0)
    item = mi[primary]
    print('idx_lowerbound of 3 2.0:', item.a, item.b, item.c, item.d)


    itr, primary = mi.idx_upperbound(0, 11)
    item = mi[primary]
    print('idx_upperbound of 0 11:', item.a, item.b, item.c, item.d)

    itr, primary = mi.idx_upperbound(0, 22)
    item = mi[primary]
    print('idx_upperbound of 0 22:', item.a, item.b, item.c, item.d)

    itr, primary = mi.idx_upperbound(0, 33)
    item = mi[primary]
    print('idx_upperbound of 0 33:', item.a, item.b, item.c, item.d)



    itr, primary = mi.idx_upperbound(1, 1)
    item = mi[primary]
    print('idx_upperbound of 1 1:', item.a, item.b, item.c, item.d)

    itr, primary = mi.idx_upperbound(1, 2)
    item = mi[primary]
    print('idx_upperbound of 1 2:', item.a, item.b, item.c, item.d)

    itr, primary = mi.idx_upperbound(1, 3)
    item = mi[primary]
    print('idx_upperbound of 1 3:', item.a, item.b, item.c, item.d)


    itr, primary = mi.idx_upperbound(2, 1)
    item = mi[primary]
    print('idx_upperbound of 2 1:', item.a, item.b, item.c, item.d)

    itr, primary = mi.idx_upperbound(2, 2)
    item = mi[primary]
    print('idx_upperbound of 2 2:', item.a, item.b, item.c, item.d)

    itr, primary = mi.idx_upperbound(2, 3)
    item = mi[primary]
    print('idx_upperbound of 2 3:', item.a, item.b, item.c, item.d)


    itr, primary = mi.idx_upperbound(3, 1.0)
    item = mi[primary]
    print('idx_upperbound of 3 1.0:', item.a, item.b, item.c, item.d)

    itr, primary = mi.idx_upperbound(3, 2.0)
    item = mi[primary]
    print('idx_upperbound of 3 2.0:', item.a, item.b, item.c, item.d)

    itr, primary = mi.idx_upperbound(3, 3.0)
    item = mi[primary]
    print('idx_upperbound of 3 3.0:', item.a, item.b, item.c, item.d)


def apply(receiver, code, action):
#    test(receiver, code, action)
#    test2(receiver, code, action)    
#    return
#    secondary_test(receiver, code, action)

    payer = receiver

    code = N('testmetestme')
    scope = N('scopee')
    table = N('mytable')
    mi = MultiIndex(code, scope, table, MyData)

    if True:
        for i in (111, 112, 113):
            try:
                del mi[i]
            except IndexError:
                pass
    primary_key = 11
    d = MyData(primary_key, 3, 5, 1.0)
    d.payer = receiver
    mi.store(d)

    primary_key = 33
    d = MyData(primary_key, 5, 1, 3.0)
    d.payer = receiver
    mi.store(d)

    primary_key = 55
    d = MyData(primary_key, 1, 3, 5.0)
    d.payer = receiver
    mi.store(d)
    
    secondary_test(receiver, code, action)
    bound_tests(receiver, code, action)

    print(110 in mi)
    for item in mi:
        print(item.a, item.b, item.c, item.d)

    del mi[primary_key]
    print(primary_key in mi)    
    print(88 in mi)
    
    for item in mi:
        print(item.a, item.b, item.c, item.d)
        values = mi.get_secondary_values(item.a)
        print('+++++++get_secondary_values:', item.a, values)

    primary_key = 33
    table = table & 0xFFFFFFFFFFFFFFF0
    itr, secondary_key = db.idx64_find_primary(code, scope, table, primary_key)    
    print(itr, secondary_key)

    itr, secondary_key = db.idx128_find_primary(code, scope, table+1, primary_key)    
    print(itr, secondary_key)

    itr, secondary_key = db.idx256_find_primary(code, scope, table+2, primary_key)    
    print(itr, secondary_key)

    itr, secondary_key = db.idx_double_find_primary(code, scope, table+3, primary_key)    
    print(itr, secondary_key)

    itr = mi.lowerbound(0)
    while itr >= 0:
        data = mi.get(itr)
        print(data.a, data.b, data.c)
        del mi[data.a]
        itr = mi.lowerbound(0)