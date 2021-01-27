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
        uint64_t i=111;
        call_contract_set_results((char *)&i, 8);
    }
}
'''
code = uuosapi.compile('hello', code, vm_type=0)

try:
    uuosapi.deploy_contract('helloworld12', code, b'')
except:
    pass

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
    void call_contract(uint64_t contract, const char* args, uint32_t size);

    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        uint64_t args[2];
        args[0] = name("calltest1").value;
        args[1] = 1;
        call_contract(name("helloworld12").value, (char *)args, sizeof(args));
        uint64_t ret = 0;
        call_contract_get_results((char *)&ret, sizeof(ret));
        eosio::print("++++ret:", ret);
//        eosio::check(ret == 2, "bad return value!");
    }
}
'''
code = uuosapi.compile('hello', code, vm_type=0)
try:
    uuosapi.deploy_contract('helloworld11', code, b'')
except:
    pass
r = uuosapi.push_action('helloworld11', 'sayhello', b'', {'helloworld11':'active'})
print(r['processed']['action_traces'][0]['console'])
print(r['processed']['elapsed'])
