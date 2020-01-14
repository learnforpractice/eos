import os
import sys
import json
import gc
import unittest

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

import uuos
from uuos import application
from uuos import wasmcompiler

from chaintest import ChainTest

gc.set_debug(gc.DEBUG_STATS)

logger = application.get_logger(__name__)


class CallContractTest(ChainTest):

    def __init__(self, uuos_network=False, jit=False):
        super(CallContractTest, self).__init__(uuos_network, jit)

    def test_call_offchain(self):
        code = r'''
#include <eosio/print.hpp>
#include <eosio/name.hpp>
#include <eosio/action.hpp>
#include <eosio/multi_index.hpp>

#include <string>

using namespace std;
using namespace eosio;

struct record {
   uint64_t  primary;
   string    data;
   uint64_t primary_key() const { return primary; }
   EOSLIB_SERIALIZE( record, (primary)(data))
};

struct getvalue {
   uint64_t  primary;
   uint64_t primary_key() const { return primary; }
   EOSLIB_SERIALIZE( getvalue, (primary))
};


template<typename T>
T unpack_args() {
    size_t raw_args_size = action_data_size();
    check(raw_args_size > 0, "bad args");
    vector<char> raw_args(raw_args_size);
    read_action_data(raw_args.data(), raw_args_size);
    T rec = eosio::unpack<T>(raw_args);
    return rec;
}

extern "C" {
    __attribute__((eosio_wasm_import))
    void set_action_return_value(const char *packed_blob, size_t size);

    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        if ("setvalue"_n.value == action) {
            uint64_t scope = code;
            uint64_t payer = receiver;
            multi_index<"record"_n, record> mytable(name(code), scope);
            
            record rec = unpack_args<record>();
            auto itr = mytable.find(rec.primary);
            
            if (mytable.end() == itr) {
                mytable.emplace( name(payer), [&]( auto& row ) {
                    row = rec;
                });
            } else {
                mytable.modify( itr, name(payer), [&]( auto& row ) {
                    row.data = rec.data;
                });
            }
        } else if ("getvalue"_n.value == action) {
            uint64_t scope = code;
            uint64_t payer = receiver;
            multi_index<"record"_n, record> mytable(name(code), scope);
            getvalue rec = unpack_args<getvalue>();
            auto itr = mytable.find(rec.primary);
            auto packed_rec = eosio::pack<record>(*itr);
            set_action_return_value(packed_rec.data(), packed_rec.size());
        }
    }
}
        '''

        abi = '''
{
   "version": "eosio::abi/1.0",
   "types": [],
  "structs": [{
      "name": "record",
      "base": "",
      "fields": [
        {"name":"primary", "type":"uint64"},
        {"name":"data", "type":"string"},
       ]
    },{
      "name": "getvalue",
      "base": "",
      "fields": [
        {"name":"primary", "type":"uint64"},
       ]
    }
  ],
  "actions": [{
      "name": "getvalue",
      "type": "getvalue",
      "ricardian_contract": ""
    },{
      "name": "setvalue",
      "type": "record",
      "ricardian_contract": ""
    }
  ],
  "tables": [
  ]
}
        '''
        contract_name = 'helloworld11'
        code = wasmcompiler.compile_cpp_src(contract_name, code, entry='apply')
        self.deploy_contract(contract_name, code, abi)
        self.produce_block()

        args = {'primary':11, 'data':'hello,worldddd'}
        r = self.push_action(contract_name, 'setvalue', args)
        # print(r)
        args = self.pack_args(contract_name, 'getvalue', {'primary':11})
        params = dict(
            code = contract_name,
            action = 'getvalue',
            binargs = args.hex()
        )
        params = json.dumps(params)
        r = uuos.call_contract_off_chain(params)
        r = bytes.fromhex(r['output'])
        r = self.unpack_args(contract_name, 'setvalue', r)
        r = json.loads(r)
        assert r['primary'] == 11
        assert r['data'] == 'hello,worldddd'
        logger.info(f'++++++++call_contract_off_chain return {r}')

    def test_call_offchain2(self):
        code = r'''
#include <eosio/print.hpp>
#include <eosio/name.hpp>
#include <eosio/action.hpp>
#include <eosio/multi_index.hpp>

#include <string>

using namespace std;
using namespace eosio;

struct record {
   uint64_t  primary;
   string    data;
   uint64_t primary_key() const { return primary; }
   EOSLIB_SERIALIZE( record, (primary)(data))
};

extern "C" {
    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        if ("getvalue"_n.value == action) {
            uint64_t scope = code;
            uint64_t payer = receiver;
            multi_index<"record"_n, record> mytable(name(code), scope);
            mytable.emplace( name(payer), [&]( auto& row ) {

            });

        }
    }
}
        '''
        contract_name = 'helloworld11'
        code = wasmcompiler.compile_cpp_src(contract_name, code, entry='apply')
        self.deploy_contract(contract_name, code, b'')
        self.produce_block()

        params = dict(
            code = contract_name,
            action = 'getvalue',
            binargs = ''
        )
        params = json.dumps(params)
        r = uuos.call_contract_off_chain(params)
        assert 'error' in r and r['error']['code'] == 3050007
        logger.info(r['error'])

    def test_call_offchain3(self):
        code = r'''
#include <eosio/print.hpp>
#include <eosio/name.hpp>
#include <eosio/action.hpp>
#include <eosio/system.hpp>
#include <eosio/multi_index.hpp>

#include <string>

using namespace std;
using namespace eosio;

struct record {
   uint64_t  primary;
   string    data;
   uint64_t primary_key() const { return primary; }
   EOSLIB_SERIALIZE( record, (primary)(data))
};

extern "C" {
    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        if ("getvalue"_n.value == action) {
            eosio::print(current_time_point().sec_since_epoch());
        }
    }
}
        '''
        contract_name = 'helloworld11'
        code = wasmcompiler.compile_cpp_src(contract_name, code, entry='apply')
        self.deploy_contract(contract_name, code, b'')
        self.produce_block()

        params = dict(
            code = contract_name,
            action = 'getvalue',
            binargs = ''
        )
        params = json.dumps(params)
        r = uuos.call_contract_off_chain(params)
        assert 'error' in r and r['error']['code'] == 3050007
        logger.info(r['error'])

    def test_call_offchain4(self):
        code = r'''
#include <eosio/print.hpp>
#include <eosio/name.hpp>
#include <eosio/action.hpp>
#include <eosio/system.hpp>
#include <eosio/multi_index.hpp>

#include <string>

using namespace std;
using namespace eosio;

struct record {
   uint64_t  primary;
   string    data;
   uint64_t primary_key() const { return primary; }
   EOSLIB_SERIALIZE( record, (primary)(data))
};

extern "C" {
    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
        if ("getvalue"_n.value == action) {
            while(true){};
        }
    }
}
        '''
        contract_name = 'helloworld11'
        code = wasmcompiler.compile_cpp_src(contract_name, code, entry='apply')
        self.deploy_contract(contract_name, code, b'')
        self.produce_block()

        params = dict(
            code = contract_name,
            action = 'getvalue',
            binargs = ''
        )
        params = json.dumps(params)
        r = uuos.call_contract_off_chain(params)
        assert 'error' in r and r['error']['code'] == 3080004 #tx_cpu_usage_exceeded
        logger.info(r['error'])

class CallContractOffChainTestCase(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.info('+++++++++++++++++++++CallContractOffChainTestCase++++++++++++++++')
        super(CallContractOffChainTestCase, self).__init__(testName)
        self.extra_args = extra_args
#        UUOSTester.chain = self.chain

    def test_call_offchain(self):
        CallContractOffChainTestCase.chain.test_call_offchain()

    def test_call_offchain2(self):
        CallContractOffChainTestCase.chain.test_call_offchain2()

    def test_call_offchain3(self):
        CallContractOffChainTestCase.chain.test_call_offchain3()

    def test_call_offchain4(self):
        CallContractOffChainTestCase.chain.test_call_offchain4()

    @classmethod
    def setUpClass(cls):
        cls.chain = CallContractTest(uuos_network=False, jit=False)

    @classmethod
    def tearDownClass(cls):
        if cls.chain:
            cls.chain.free()
            cls.chain = None

    def setUp(self):
        pass

    def tearDown(self):
        pass

class CallContractOffChainTestCaseJIT(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.info('+++++++++++++++++++++CallContractOffChainTestCaseJIT++++++++++++++++')
        super(CallContractOffChainTestCaseJIT, self).__init__(testName)
        self.extra_args = extra_args
#        UUOSTester.chain = self.chain

    def test_call_offchain(self):
        CallContractOffChainTestCaseJIT.chain.test_call_offchain()

    def test_call_offchain2(self):
        CallContractOffChainTestCaseJIT.chain.test_call_offchain2()

    def test_call_offchain3(self):
        CallContractOffChainTestCaseJIT.chain.test_call_offchain3()

    def test_call_offchain4(self):
        CallContractOffChainTestCaseJIT.chain.test_call_offchain4()

    @classmethod
    def setUpClass(cls):
        cls.chain = CallContractTest(uuos_network=False, jit=True)

    @classmethod
    def tearDownClass(cls):
        if cls.chain:
            cls.chain.free()
            cls.chain = None

    def setUp(self):
        pass

    def tearDown(self):
        pass

if __name__ == '__main__':
    unittest.main()
