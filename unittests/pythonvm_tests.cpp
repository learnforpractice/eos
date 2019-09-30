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

#include <eosiolib_native/vm_api.h>
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

#define TEST_SOURCE_PATH ""

#define SET_CODE(src_path) \
{ \
   const char *src_file = TEST_SOURCE_PATH "/" src_path; \
   int compiled_code_size; \
   char *compiled_code = compile_python_source_file(src_file, &compiled_code_size); \
   vector<char> v(compiled_code, compiled_code + compiled_code_size); \
   free(compiled_code); \
   set_code(N(noop), 1, v); \
}


BOOST_AUTO_TEST_SUITE(pythonvm_tests)


extern "C" void wasm_interface_run_module_4(const char* data, size_t size);
extern "C" void wasm_interface_run_module_3(const char* data, size_t size);

#define wasm_interface_run_module wasm_interface_run_module_3

const char *python_wasm_path = "/Users/newworld/dev/eos/build/contracts/pythontest/pythontest.wasm";
//const char *python_wasm_path = "/Users/newworld/dev/wasm/cpython-emscripten/examples/01-print/python.asm.wasm";
//const char *python_wasm_path = "/Users/newworld/dev/research/wasm/t.wasm";
const char *python_test_script_file = "/Users/newworld/dev/eos/unittests/python/test.py.m";
//const char *python_test_script_file = "/Users/newworld/dev/eos/unittests/python/large_file_test.py.m";
//const char *python_test_script_file = "/Users/newworld/dev/eos.bk/contracts/libpython/Python-3.5.2/Tools/freeze/classtest.pyc";

char *read_file(const char *wasm_path, int *size) {
    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;

    pFile = fopen (wasm_path, "rb");
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    // allocate memory to contain the whole file:
    buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    // copy the file into the buffer:
    result = fread (buffer,1,lSize,pFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

    /* the whole file is now loaded in the memory buffer. */
    printf("+++++lSize %ld \n", lSize);
    *size = lSize;
//    wasm_interface_run_module(buffer, lSize);

    // terminate
    fclose (pFile);
    return buffer;
}
// extra 1 byte for store  '\0'
char *read_python_file(const char *python_path, int *size) {
    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;

    pFile = fopen (python_path, "rb");
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    // allocate memory to contain the whole file:
    buffer = (char*) malloc (sizeof(char)*lSize+sizeof(char));
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
    buffer[lSize] = 0;
    // copy the file into the buffer:
    result = fread (buffer,1,lSize,pFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

    /* the whole file is now loaded in the memory buffer. */
    printf("+++++lSize %ld \n", lSize);
    *size = lSize;
//    wasm_interface_run_module(buffer, lSize);

    // terminate
    fclose (pFile);
    return buffer;
}

char *compile_python_source(const char *src, int *compiled_size) {
   *compiled_size = 0;
   PyObject *code = Py_CompileStringExFlags(src, "contract", Py_file_input, NULL, 0);
   if (code == NULL)
      return NULL;

   PyObject *marshalled = PyMarshal_WriteObjectToString(code, Py_MARSHAL_VERSION);
   Py_CLEAR(code);
   if (marshalled == NULL)
      return NULL;

   assert(PyBytes_CheckExact(marshalled));
   const char *data = (char *) PyBytes_AS_STRING(marshalled);
   int data_size = PyBytes_GET_SIZE(marshalled);
   Py_CLEAR(marshalled);
   *compiled_size = data_size;
   char *_data = (char *)malloc(data_size);
   memcpy(_data, data, data_size);
   return _data;
}

char *compile_python_source_file(const char *filename, int *compiled_size) {
   int source_file_size;
   char *source_code = read_python_file(filename, &source_file_size);
   if (source_code == NULL) {
      return NULL;
   }
   char * compiled_code = compile_python_source(source_code, compiled_size);
   FC_ASSERT(compiled_code != NULL, "compile code failed");
   free(source_code);
   return compiled_code;
}

int python_test() {
   return 0;
    int size = 0;
    char *buffer = read_file(python_wasm_path, &size);
    if (buffer) {
//        wasm_interface_run_module(buffer, size);
    }
    free (buffer);
    return 0;
}

/// Test processing of unbalanced strings
BOOST_AUTO_TEST_CASE(testpythonvm)
{

#if 1
   python_test();
#endif
}

BOOST_FIXTURE_TEST_CASE(testpythonvm2, TESTER) try {

} FC_LOG_AND_RETHROW()


//INCBIN(fuzz1, "fuzz1.wasm");

BOOST_FIXTURE_TEST_CASE(testpythonvm3, pythonvm_tester) try {
   produce_blocks(2);
   create_accounts( {N(noop), N(alice)} );
   produce_block();

   wlog("++++++++++set code begin...\n");
   SET_CODE("python/test.py")

   const auto& accnt  = control->db().get<account_object,by_name>(N(noop));
   abi_def abi;
   BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
   abi_serializer abi_ser(abi, abi_serializer_max_time);
   for (int i=0;i<3;i++)
   {
      produce_blocks(5);
      signed_transaction trx;
      action act;
      act.account = N(noop);
      act.name = N(anyaction);
      act.authorization = vector<permission_level>{{N(noop), config::active_name}};

      act.data = abi_ser.variant_to_binary("anyaction", mutable_variant_object()
                                           ("from", "noop")
                                           ("type", "some type")
                                           ("data", "some data goes here"),
                                           abi_serializer_max_time
                                           );

      trx.actions.emplace_back(std::move(act));

      set_transaction_headers(trx);
      trx.sign(get_private_key(N(noop), "active"), control->get_chain_id());
      auto r =push_transaction(trx);
      vmdlog("r->elapsed.count() %ld\n", r->elapsed.count());
      produce_block();

      BOOST_REQUIRE_EQUAL(true, chain_has_transaction(trx.id()));
   }

 } FC_LOG_AND_RETHROW()


BOOST_FIXTURE_TEST_CASE(testpythonvm4, pythonvm_tester) try {
   produce_blocks(2);
   create_accounts( {N(noop), N(alice)} );
   produce_block();

   wlog("++++++++++set abi begin...\n");

#if 1
//    vector<char> wasm(gfuzz1Data, gfuzz1Data + gfuzz1Size);
#else
    int size = 0;
    const char *script = "/Users/newworld/dev/eos/build/contracts/hello/hello.wast";
    char *buffer = read_file(script, &size);
    vmdlog("++++++++++++++++++++script size %d\n", size);
    vector<char> wasm(buffer, buffer + size);
#endif
   
   SET_CODE("python/test.py")

   const auto& accnt  = control->db().get<account_object,by_name>(N(noop));
   abi_def abi;
   BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
   abi_serializer abi_ser(abi, abi_serializer_max_time);
   
   wlog("++++++++++send anyaction begin...\n"); 

   {
      produce_blocks(5);
      signed_transaction trx;
      action act;
      act.account = N(noop);
      act.name = N(anyaction);
      act.authorization = vector<permission_level>{{N(noop), config::active_name}};

      act.data = abi_ser.variant_to_binary("anyaction", mutable_variant_object()
                                           ("from", "noop")
                                           ("type", "some type")
                                           ("data", "some data goes here"),
                                           abi_serializer_max_time
                                           );

      trx.actions.emplace_back(std::move(act));

      set_transaction_headers(trx);
      trx.sign(get_private_key(N(noop), "active"), control->get_chain_id());
      auto r =push_transaction(trx);
      vmdlog("r->elapsed.count() %ld\n", r->elapsed.count());
      produce_block();

      BOOST_REQUIRE_EQUAL(true, chain_has_transaction(trx.id()));
   }

   for (int i=0;i<3;i++)
   {
      produce_blocks(5);
      signed_transaction trx;
      action act;
      act.account = N(noop);
      act.name = N(anyaction);
      act.authorization = vector<permission_level>{{N(noop), config::active_name}};

      act.data = abi_ser.variant_to_binary("anyaction", mutable_variant_object()
                                           ("from", "noop")
                                           ("type", "some type")
                                           ("data", "some data goes here"),
                                           abi_serializer_max_time
                                           );

      trx.actions.emplace_back(std::move(act));

      set_transaction_headers(trx);
      trx.sign(get_private_key(N(noop), "active"), control->get_chain_id());
      auto r =push_transaction(trx);
      vmdlog("r->elapsed.count() %ld\n", r->elapsed.count());
      produce_block();

      BOOST_REQUIRE_EQUAL(true, chain_has_transaction(trx.id()));
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
