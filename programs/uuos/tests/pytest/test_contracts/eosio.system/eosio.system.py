import struct
import json
import db

def symbol_to_bytes(precision, s):
    a = bytearray(8)
    a[0] = precision
    i = 1
    for c in s:
        a[i] = ord(c)
        i += 1
    return a

class Symbol(object):

    def __init__(self, precision, s)
        self.precision = precision
        self.s = s
        self.raw = symbol_to_bytes(precision, s)
        self.n = int.from_bytes(self,raw, 'little')

    def to_int(self):
        return self.n

class Asset(object):

    def __init__(self, amount, symbol):
        self.amount = amount
        self.symbol = symbol

    def pack(self):
        return struct.pack('QQ', self.amount, self.symbol.to_int())

    @classmethod
    def unpack(cls, data):
        a, b = struct.unpack('QQ', data)
        return asset(a, b)

class Namebid(object):

    def __init__(self, newname, high_bidder, high_bid, last_bid_time):
        self.newname = newname # name
        self.high_bidder = high_bidder# name
        self.high_bid = high_bid # int64_t ///< negative high_bid == closed auction waiting to be claimed
        self.last_bid_time = last_bid_time #int64_t
        self.payer = 0

    def pack(self):
        return struct.pack('QQQQ', self.newname.to_int(), self.high_bidder.to_int(), self.high_bid, self.last_bid_time)

    @classmethod
    def unpack(cls, data):
        a, b, c, d = struct.unpack('QQQQ', data)
        return cls(a, b, c, d)

    def get_primary_key(self):
        return self.newname.to_int()

    def get_secondary_values(self):
        return (-self.high_bid & 0xffffffffffffffff,)

    @staticmethod
    def get_secondary_indexes():
        return (idx64,)

code = name('alice')
scope = name('scope')
table = name('namebids')
name_bid = db.MultiIndex(code, scope, table, Namebid)
DAY_IN_MICROSECONDS = 24*60*60*1000000
def apply(a, b, c):
    if c == name('bid'):
        data = read_action_data()
        bidder, newname, amount, symbol = struct.unpack('QQQQ', data)
        itr = name_bid.find(newname)
        if itr >= 0:
            bid = name_bid.get(itr)
            current = current_time()
            assert current - bid.last_bid_time < DAY_IN_MICROSECONDS
            assert amount - bid.high_bid > amount * 15 // 100
            bid.bidder = name(bidder)
            bid.amount = amount
            bid.last_bid_time = current
            name_bid.update(bid)
        else:
            last_bid_time = current_time()
            bid = Namebid(name(newname), name(bidder), high_bid, last_bid_time):
            name_bid.store(bid)

    

