# find string performance tests between python and C++
import os
import sys
import time
from uuoskit import uuosapi, wallet, config

print('find string performance tests between python and C++')

if len(sys.argv) >= 2:
    uuosapi.set_node(sys.argv[1])

config.main_token = 'UUOS'
config.main_token_contract = 'eosio.token'
config.system_contract = 'eosio'

if os.path.exists('test.wallet'):
    os.remove('test.wallet')
psw = wallet.create('test')
wallet.import_key('test', '5KH8vwQkP4QoTwgBtCV5ZYhKmv8mx56WeNrw9AZuhNRXTrPzgYc')
wallet.import_key('test', '5Jbb4wuwz8MAzTB9FJNmrVYGXo4ABb7wqPVoWGcZ6x8V2FwNeDo')
wallet.import_key('test', '5JRYimgLBrRLCBAcjHUWCYRv3asNedTYYzVgmiU4q2ZVxMBiJXL')

uuosapi.deploy_contract('hello', int.to_bytes(int(time.time()*100), 8, 'little'), b'', vm_type=1)

code = '''
import struct

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
    d = MyDataI64(1, 2, 3, 5.0)
    s = 'helloooooooooooooooooooooooooooohelloooooooooooooooooooooooooooohelloooooooooooooooooooooooooooohelloooooooooooooooooooooooooooo,world'
    for i in range(10000):
        s.find('world')
#        d.pack()
#        print(d.pack())
#        print('hello,world')
'''
code = uuosapi.mp_compile('hello', code)
args = uuosapi.s2b('hello') + code
r = uuosapi.push_action('hello', 'setcode', args, {'hello':'active'})
r = uuosapi.push_action('hello', 'sayhello', b'aa', {'hello':'active'})
r = uuosapi.push_action('hello', 'sayhello', b'bb', {'hello':'active'})
print('++++mpy', r['processed']['action_traces'][0]['console'], r['processed']['elapsed'])

code = r'''
#include <eosio/eosio.hpp>
#include <eosio/action.hpp>
#include <eosio/print.hpp>
#include <string>

using namespace eosio;

struct A {
    uint64_t a;
    uint64_t b;
    uint64_t c;
    double d;
    EOSLIB_SERIALIZE(A, (a)(b)(c)(d))
};
static int a;
extern "C" {
    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        A a;
        std::string s("helloooooooooooooooooooooooooooohelloooooooooooooooooooooooooooohelloooooooooooooooooooooooooooohelloooooooooooooooooooooooooooo,world");
        for (int i=0;i<10000;i++) {
            s[0] = i;
            size_t pos = s.find("world");
            continue;
            a.a = i;
            auto data = eosio::pack(a);
            if (!receiver) {
                eosio::print(data.size());
            }
        }
    }
}
'''
code = uuosapi.compile('hello', code, vm_type=0)
r = uuosapi.deploy_contract('hello', code, b'')
r = uuosapi.push_action('hello', 'sayhello', b'aa', {'helloworld11':'active'})
r = uuosapi.push_action('hello', 'sayhello', b'bb', {'helloworld11':'active'})
print('+++c++', r['processed']['elapsed'])
