config.main_token = 'UUOS'
config.main_token_contract = 'eosio.token'
config.system_contract = 'eosio'

if os.path.exists('test.wallet'):
    os.remove('test.wallet')
psw = wallet.create('test')
wallet.import_key('test', '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc')
wallet.import_key('test', '5Jbb4wuwz8MAzTB9FJNmrVYGXo4ABb7wqPVoWGcZ6x8V2FwNeDo')
wallet.import_key('test', '5JRYimgLBrRLCBAcjHUWCYRv3asNedTYYzVgmiU4q2ZVxMBiJXL')

try:
    uuosapi.deploy_contract('hello', b'a', b'', vm_type=1)
except:
    pass

code = '''
import json
import struct

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
        return struct.pack('lllf', self.a, self.b, self.c, self.d)

    @classmethod
    def unpack(cls, data):
        data = struct.unpack('lllf', data)
        return cls(data[0], data[1], data[2], data[3])

    def get_primary_key(self):
        return self.a

    def __str__(self):
        data = (self.a, self.b, self.c, self.d)
        return json.dumps(data)

def apply(a, b, c):
#    while True: pass
    for i in range(100):
        d = MyDataI64(1, 2, 3, 5.0)
        d.pack()
#        print(d.pack())
#        print('hello,world')
'''
code = uuosapi.mp_compile('hello', code)
args = uuosapi.s2b('hello') + code
uuosapi.push_action('hello', 'setcode', args, {'hello':'active'})

r = uuosapi.push_action('hello', 'sayhello', b'', {'hello':'active'})
print(r['processed']['action_traces'][0]['console'], r['processed']['elapsed'])

