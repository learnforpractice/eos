import json
import struct
from chain import *
import db

_code = None  # N('eosio.token')
# _code = N('mytoken')


def to_symbol_name(s):
    n = int.from_bytes(s, 'little')
    return n >> 8

class Symbol(object):

    def __init__(self, raw_bytes):
        self.raw_bytes = raw_bytes
        self._name = None
        self._symbol = None

    def pack(self):
        return self.raw_bytes

    @property
    def precision(self):
        return self.raw_bytes[0]

    @property
    def name(self):
        if not self._name:
            i = 1
            while self.raw_bytes[i]:
                i += 1
            self._name = self.raw_bytes[1:i].decode()
        return self._name

    @classmethod
    def build(cls, precision, name):
        raw_bytes = bytearray(8)
        raw_bytes[0] = precision
        i = 1
        for c in name:
            raw_bytes[i] = ord(c)
            i += 1
        return cls(raw_bytes)

    @classmethod
    def unpack(cls, raw_bytes: bytes):
        return cls(raw_bytes)

    def __repr__(self):
        return self.name

    def __str__(self):
        return self.name

class Asset(object):
    def __init__(self, amount=0, symbol=None):
        self.amount = amount
        self.symbol = symbol

    def pack(self):
        buffer = bytearray(16)
        struct.pack_into('Q8s', buffer, 0, self.amount, self.symbol.pack())
        return bytes(buffer)

    @classmethod
    def unpack(cls, data):
        a, s = struct.unpack('Q8s', data)
        s = Symbol(s)
        return cls(a, s)

    def __repr__(self):
        a = {'amount': self.amount, 'symbol': self.symbol}
        return json.dumps(a)

    def __str__(self):
        a = {'amount': self.amount, 'symbol': self.symbol}
        return json.dumps(a)

CONTRACT_NAME = name('alice')
MAX_INT64 = 0x7fffffffffffffff

class ChainDB(object):
    def __init__(self, code, scope, table, primary_key, data_type):
        self.code = code
        self.scope = scope
        self.table = table
        self.data_type = data_type
        self.primary_key = primary_key

    def find(self):
        return db_find_i64(self.code, self.scope, self.table, self.primary_key)

    def get(self, itr):
        if itr < 0:
            raise IndexError
        data = db_get_i64(itr)
        return self.data_type.unpack(data)

    def load(self):
        itr = self.find()
        if itr < 0:
            return None
        return self.get(itr)

    def store(self, obj):
        itr = db_find_i64(self.code, self.scope, self.table, self.primary_key)
        if itr < 0:
            db_store_i64(self.scope, self.table, obj.payer, self.primary_key, obj.pack())
        else:
            db_update_i64(itr, obj.payer, obj.pack())

    def remove(self):
        itr = db_find_i64(self.code, self.scope, self.table, self.primary_key)
        if itr < 0:
            raise IndexError
        db_remote_i64(itr)


class Balance(object):
    def __init__(self, owner, symbol):
        self.owner = owner
        self.payer = 0
        self.symbol = symbol

        self.primary = to_symbol_name(self.symbol.raw_bytes)

        table_id = name('accounts')
        self.storage = ChainDB(CONTRACT_NAME, owner, table_id, self.primary, Asset)
        self.load()

    def load(self):
        self.a = self.storage.load()
        if not self.a:
            self.a = Asset(0, self.symbol)
        print(self.a, self.a.amount)

    def add(self, amount, payer):
        self.a.amount += amount
        assert self.a.amount < MAX_INT64, 'balance overflow'
        self.a.payer = payer
        self.storage.store(self.a)

    def sub(self, amount, payer):
        assert(self.a.amount >= amount)
        self.a.amount -= amount
        assert self.a.amount >= 0, 'balance underflow'
        self.a.payer = payer
        if self.a.amount == 0:
            self.storage.remove()
        else:
            self.storage.store(self.a)

class CurrencyStats(object):
    def __init__(self, symbol):
        table_id = name('stat')
        self.primary = scope = to_symbol_name(symbol.raw_bytes)
        self.payer = 0
        self.symbol = symbol

        self.storage = ChainDB(CONTRACT_NAME, scope, table_id, self.primary, type(self))

    def load(self):
        ret = self.storage.load()
        if ret:
            self.supply, self.max_supply, self.issuer = ret
        else:
            self.supply = Asset(0, self.symbol)
            self.max_supply = Asset(0, self.symbol)
            self.issuer = 0

    def store(self):
        self.storage.store(self)

    def pack(self):
        result = bytearray(40)
        struct.pack_into('16s16sQ', result, 0, self.supply.pack(), self.max_supply.pack(), self.issuer)
        return bytes(result)

    @classmethod
    def unpack(self, data):
        a1, s1, a2, s2, issuer = struct.unpack_from('Q8sQ8sQ', data)
        supply = Asset(a1, Symbol(s1))
        max_supply = Asset(a2, Symbol(s2))
        return supply, max_supply, issuer

'''
def _create(msg):
    require_auth(_code)
    issuer, amount, symbol = struct.unpack('QQ8s', msg)

    cs = currency_stats(symbol)
    if cs.issuer:
        raise Exception('currency has already been created')
    cs.issuer = issuer
    cs.max_supply.amount = amount
    cs.store(_code)


def _issue(msg):
    _to, amount, symbol = struct.unpack('QQ8s', msg[:24])
#        memo = unpack_bytes(msg[24:])
    cs = currency_stats(symbol)
    assert cs.issuer, 'currency does not exists'
    require_auth(cs.issuer)
    cs.supply.amount += amount
    assert cs.supply.amount < cs.max_supply.amount
    cs.store(cs.issuer)

    acc = Balance(_to, symbol)
    acc.add(amount, cs.issuer)


def _transfer(msg):
    _from, to, amount, symbol = struct.unpack('QQQ8s', msg[:32])
#    print('transfer:', _from, to, amount, symbol)
    require_auth(_from)
    require_recipient(_from)
    require_recipient(to)
#        memo = unpack_bytes(msg[32:])
    a1 = Balance(_from, symbol)
    a2 = Balance(to, symbol)
    a1.sub(amount)
    a2.add(amount, _from)


def apply(receiver, account, act):
    global _code
    _code = account

    if act == N('create'):
        msg = read_action_data()
        _create(msg)
    elif act == N('issue'):
        msg = read_action_data()
        _issue(msg)
    elif act == N('transfer'):
        msg = read_action_data()
        _transfer(msg)
'''

def apply(receiver, account, act):
    if act == name('test1'):
        test1()
    elif act == name('test2'):
        test2()
    elif act == name('test3'):
        test3()

def test1():
    s = Symbol(b'\x04UUOS\x00\x00\x00')
    assert s.name == 'UUOS'
    assert s.precision == 4

    s = Symbol.build(4, 'UUOS')
    assert s.pack() == b'\x04UUOS\x00\x00\x00'
    assert s.name == 'UUOS'
    assert s.precision == 4

    a = Asset(1000, s)
    raw_data = b'\xe8\x03\x00\x00\x00\x00\x00\x00\x04UUOS\x00\x00\x00'
    assert a.pack() == raw_data
    a = Asset.unpack(raw_data)
    assert a.amount == 1000
    assert a.symbol.pack() == b'\x04UUOS\x00\x00\x00'

def test2():
    payer = name('alice')
    owner = name('alice')

    s = Symbol(b'\x04UUOS\x00\x00\x00')
    b = Balance(owner, s)

    b.add(100, payer)
    print(b.a)


ss = Symbol(b'\x04UUOS\x00\x00\x00')
aa = CurrencyStats(ss)

def test3():
    global ss
    global aa
    aa.load()
    aa.payer = name('alice')
    aa.supply = Asset(100, ss)
    aa.max_supply = Asset(1000, ss)
    aa.store()

