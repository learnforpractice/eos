#include <eosio/chain/wasm_interface.hpp>
#include <eosio/chain/chain_proxy.hpp>

using namespace eosio::chain;
using namespace eosio::chain::webassembly::common;

extern "C" eosio::chain::wasm_interface* eos_vm_interface_init2(int vmtype, bool tierup, eosio::chain::chain_proxy& api);

extern "C" eosio::chain::wasm_interface* eos_vm_interface_init3(int vmtype, bool tierup, eosio::chain::chain_proxy& api) {
    return eos_vm_interface_init2(vmtype, tierup, api);
}

#include <boost/test/unit_test_suite.hpp>

::boost::unit_test::test_suite*
init_unit_test_suite( int, char* [] )   {
	return 0;
}
