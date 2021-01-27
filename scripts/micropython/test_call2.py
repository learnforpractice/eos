
import os
import sys
import time
from uuoskit import uuosapi, wallet, config

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
wallet.import_key('test', '5JHRxntHapUryUetZgWdd3cg6BrpZLMJdqhhXnMaZiiT4qdJPhv')

code = r'''
#include <eosio/eosio.hpp>
#include <eosio/action.hpp>
#include <eosio/print.hpp>

using namespace eosio;

extern "C" {
    __attribute__((eosio_wasm_import))
    int call_contract_get_args(void* args, size_t size1);

    __attribute__((eosio_wasm_import))
    int call_contract_get_results(char* results, uint32_t size1);

    __attribute__((eosio_wasm_import))
    int call_contract_set_results(const char* results, uint32_t size1);

    __attribute__((eosio_wasm_import))
    void call_contract(uint64_t contract, const char* args, uint32_t size);

    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        eosio::print("hello,worldddd");
        uint64_t args[2];
        int size = call_contract_get_args((char *)args, sizeof(args));
        eosio::check(size==16, "bad arguments size");
        args[1] += 1;
        call_contract_set_results((char *)&args[1], 8);
    }
}
'''
code = uuosapi.compile('hello', code, vm_type=0)

try:
    uuosapi.deploy_contract('helloworld12', code, b'')
except:
    pass

import time
args = int.to_bytes(int(time.time()*1000), 8, 'little')

uuosapi.deploy_contract('hello', args, b'', vm_type=1)

code = '''
import struct
import chain
def apply(receiver, code, action):
    args = struct.pack('QQ', chain.s2n('calltest1'), 1)
    print(args)
    ret = chain.call_contract('helloworld12', args)
    print(ret)
'''
code = uuosapi.mp_compile('hello', code)
args = uuosapi.s2b('hello') + code
r = uuosapi.push_action('hello', 'setcode', args, {'hello':'active'})
r = uuosapi.push_action('hello', 'sayhello', b'aa', {'hello':'active'})
r = uuosapi.push_action('hello', 'sayhello', b'bb', {'hello':'active'})
print(r['processed']['action_traces'][0]['console'])
print(r['processed']['elapsed'])

