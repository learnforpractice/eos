#pragma once

#include <eosio/chain/wasm_interface.hpp>
#include <eosio/chain/webassembly/wavm.hpp>
#include <eosio/chain/webassembly/wabt.hpp>
#include <eosio/chain/webassembly/runtime_interface.hpp>
#include <eosio/chain/wasm_eosio_injection.hpp>
#include <eosio/chain/transaction_context.hpp>
#include <eosio/chain/code_object.hpp>
#include <eosio/chain/exceptions.hpp>
#include <fc/scoped_exit.hpp>
#include <chain_api.hpp>
#include <eosiolib_native/vm_api.h>
#include "IR/Module.h"
#include "Runtime/Intrinsics.h"
#include "Platform/Platform.h"
#include "WAST/WAST.h"
#include "IR/Validate.h"

using namespace fc;
using namespace eosio::chain::webassembly;
using namespace IR;
using namespace Runtime;
using boost::multi_index_container;


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
    fclose (pFile);
    return buffer;
}


int main(int argc, char **argv) {
    if (argc != 3) {
        printf("usage wasm_injector in_wasm_file out_wasm_file");
        return 0;
    }
    int file_size;
    const char *code = read_file(argv[1], &file_size);
    if (code == nullptr || file_size <= 0) {
        printf("read wasm file failed!\n");
        return -1;
    }

    wasm_constraints::set_maximum_linear_memory_init(6*1024*1024);
    wasm_constraints::set_maximum_table_elements(10240);
    wasm_constraints::set_maximum_func_local_bytes(128*1024);
    wasm_constraints::set_maximum_section_elements(10240);

    IR::Module module;
    try {
        Serialization::MemoryInputStream stream((const U8*)code, file_size);
        WASM::serialize(stream, module);
        module.userSections.clear();
    } catch(const Serialization::FatalSerializationException& e) {
        EOS_ASSERT(false, wasm_serialization_error, e.message.c_str());
    } catch(const IR::ValidationException& e) {
        EOS_ASSERT(false, wasm_serialization_error, e.message.c_str());
    }

    wasm_injections::wasm_binary_injection injector(module);
    injector.inject();

    std::vector<U8> bytes;
    try {
        Serialization::ArrayOutputStream outstream;
        WASM::serialize(outstream, module);
        bytes = outstream.getBytes();
        FILE *fp = fopen(argv[2], "wb");
        fwrite(bytes.data(), 1, bytes.size(), fp);
        fclose(fp);
    } catch(const Serialization::FatalSerializationException& e) {
        EOS_ASSERT(false, wasm_serialization_error, e.message.c_str());
    } catch(const IR::ValidationException& e) {
        EOS_ASSERT(false, wasm_serialization_error, e.message.c_str());
    }
}
