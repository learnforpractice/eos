#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/chain/wasm_eosio_constraints.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/wast_to_wasm.hpp>


#include <fc/io/fstream.hpp>

#include <Runtime/Runtime.h>

#include <fc/variant_object.hpp>
#include <fc/io/json.hpp>

#include "test_wasts.hpp"
#include "test_softfloat_wasts.hpp"

#include <array>
#include <utility>

#include "incbin.h"

#include <vm_api/vm_api.h>
#include <chain_api.hpp>

#include <Python.h>
#include <marshal.h>

#include <contracts.hpp>

#ifdef NON_VALIDATING_TEST
#define TESTER tester
#else
#define TESTER validating_tester
#endif

using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;

using mvo = mutable_variant_object;

INCBIN(BasicTests, "basic_tests.py");
INCBIN(APITests, "api_tests.py");
INCBIN(DBTests, "db_tests.py");


#include <memory>
#include "src/interp.h"
extern "C" uint8_t *vm_allocate_memory(uint32_t initial_pages, uint32_t max_pages);
using namespace wabt::interp;

namespace wabt {
namespace interp {
   void LoadDataToWritableMemory(Memory *memory, uint32_t write_index);
   void LoadDataToWritableMemory(Memory *memory, uint32_t offset_start, uint32_t length);
}}

class pythonvm_tester : public TESTER {
public:
   void set_code( account_name account, int vm_type, bytes& code, const private_key_type* signer = nullptr ) try {
      signed_transaction trx;
      trx.actions.emplace_back( vector<permission_level>{{account,config::active_name}},
                                setcode{
                                   .account    = account,
                                   .vmtype     = (uint8_t)vm_type,
                                   .vmversion  = 0,
                                   .code       = code
                                });

      set_transaction_headers(trx);
      if( signer ) {
         trx.sign( *signer, control->get_chain_id()  );
      } else {
         trx.sign( get_private_key( account, "active" ), control->get_chain_id()  );
      }
      push_transaction( trx );
   } FC_CAPTURE_AND_RETHROW( (account) )

   void set_code( account_name account, int vm_type, const char* code, const private_key_type* signer = nullptr  ) try {
      if (vm_type ==0) {
         TESTER::set_code(account, code, signer);
      } else {
         bytes _code(code, code + strlen(code));
         set_code(account, vm_type, _code, signer);
      }
   } FC_CAPTURE_AND_RETHROW( (account) )

    void set_code_abi(const account_name& account, int vm_type, const char* code, const char* abi, const private_key_type* signer = nullptr) {
       wdump((account));
      set_code(account, vm_type, code, signer);
      set_abi(account, abi, signer);
      if (account == config::system_account_name) {
         const auto& accnt = control->db().get<account_object,by_name>( account );
         abi_def abi_definition;
         BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi_definition), true);
         abi_ser.set_abi(abi_definition, abi_serializer_max_time);
      }
      produce_blocks();
    }

    abi_serializer abi_ser;
};



BOOST_AUTO_TEST_SUITE(pythonvm_tests)

vector<char> compile_python_source(std::string& src) {
   PyObject *code = Py_CompileStringExFlags(src.c_str(), "contract", Py_file_input, NULL, 0);
   if (code == NULL) {
      return {};
   }

   PyObject *marshalled = PyMarshal_WriteObjectToString(code, Py_MARSHAL_VERSION);
   Py_CLEAR(code);
   if (marshalled == NULL) {
      return {};
   }

   assert(PyBytes_CheckExact(marshalled));
   const char *data = (char *) PyBytes_AS_STRING(marshalled);
   int data_size = PyBytes_GET_SIZE(marshalled);
 
   vector<char> v(data, data+data_size);
   Py_CLEAR(marshalled);
   return v;
}


extern "C" int Py_InitFrozenMain(int argc, char **argv);
const char *argv[] = {"pythonvm", "test"};

BOOST_FIXTURE_TEST_CASE(basic_test, pythonvm_tester) try {
   dlog("+++++++++++++++++++++++++");
   auto cleanup = fc::make_scoped_exit([&](){
      Py_FinalizeEx();
   });

   Py_InitializeEx(0);
//   Py_InitFrozenMain(2, (char **)argv);


   produce_blocks(2);
   create_accounts( {N(noop), N(alice)} );
   produce_block();

   std::string src((char*)gBasicTestsData, gBasicTestsSize);
   vector<char> v = compile_python_source(src);

   set_code(N(noop), 1, v);

   produce_block();

{
   vector<uint8_t> data;
   push_action(N(noop), N(test1), data, N(noop));
}

{
   vector<uint8_t> data;
   push_action(N(noop), N(test2), data, N(noop));
}

//global variable test
{
   vector<uint8_t> data;
   push_action(N(noop), N(test3), data, N(noop));
}
   produce_block();
{
   vector<uint8_t> data;
   push_action(N(noop), N(test3), data, N(noop));
}


} FC_LOG_AND_RETHROW()


struct vm_state_backup {
   std::vector<MemorySegment>                                 memory_backup;
   int contract_memory_start;
   int contract_memory_end;
//   std::vector<char>                                 memory_backup;
};


void take_snapshoot(char *data, uint32_t vm_memory_size, char *data_backup, vm_state_backup *backup) {
   int total_count = 0;
   int i = 0;
   uint64_t *ptr1 = (uint64_t *)data_backup;
   uint64_t *ptr2 = (uint64_t *)data;

   vm_memory_size/=sizeof(uint64_t);

   //save diff memory
   while(i<vm_memory_size) {
      if (ptr1[i] == ptr2[i]) {
         i += 1;
         continue;
      }
      int start = i;
      total_count += 1;
      i += 1;
      while (i<vm_memory_size && ptr1[i] != ptr2[i]) {
         i += 1;
         total_count += 1;
      }
      MemorySegment segment;
      segment.offset = start*sizeof(uint64_t);
      segment.data.resize((i-start)*sizeof(uint64_t));
      memcpy(segment.data.data(), &ptr2[start], (i-start)*sizeof(uint64_t));
      backup->memory_backup.emplace_back(std::move(segment));

      printf("++++++++++++++++offset %lu, size %lu\n", start*sizeof(uint64_t), (i-start)*sizeof(uint64_t));
      i += 1;
   }
}

BOOST_FIXTURE_TEST_CASE(testpythonvm5, pythonvm_tester) try {
//   vm_allocate_memory(8, 8);
   wabt::Limits limits(48, 48);
   Memory vm_memory(limits);
   for (int i=0;i<vm_memory.data.size();i++) {
      vm_memory.data[i] = (char)i;
   }
   vm_memory.backup_memory();
   vm_memory.init_cache();
   vm_memory.memory_end = vm_memory.data.size();

   vm_state_backup backup;

   for (int i=0;i<vm_memory.data.size();i+=98) {
      vm_memory.data[i] += 1;
   }

   vector<char> data_backup;
   data_backup.resize(vm_memory.data.size());
   memcpy(data_backup.data(), vm_memory.data.data(), vm_memory.data.size());

   take_snapshoot(vm_memory.data.data(), vm_memory.data.size(), vm_memory.data_backup.data(), &backup);


   vm_memory.memory_segments = &backup.memory_backup;

   memset(vm_memory.data.data(), 0, vm_memory.data.size());
   vm_memory.init_smart_contract = false;
   for (int i=0;i<vm_memory.data.size();i+=8) {
      wabt::interp::LoadDataToWritableMemory(&vm_memory, i, 8);
//      wabt::interp::LoadDataToWritableMemory(&vm_memory, i/8);
      for (int j=i;j<i+8;j++) {
         if (data_backup[j] != vm_memory.data[j]) {
            printf("++++++++++error at index: %d %d %d\n", j, (char)j, vm_memory.data[j]);
            BOOST_REQUIRE_EQUAL(true, (char)j == vm_memory.data[j]);
         }
      }
   }

 } FC_LOG_AND_RETHROW()


BOOST_AUTO_TEST_SUITE_END()
