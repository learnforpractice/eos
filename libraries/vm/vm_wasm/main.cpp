
#include <eosio/chain/wasm_interface.hpp>
#include <eosio/chain/webassembly/wavm.hpp>
#include <eosio/chain/webassembly/wabt.hpp>
#include <eosio/chain/webassembly/runtime_interface.hpp>
#include <eosio/chain/transaction_context.hpp>
#include <eosio/chain/code_object.hpp>
#include <eosio/chain/exceptions.hpp>
#include <fc/scoped_exit.hpp>
#include <chain_api.hpp>
#include <vm_api/vm_api.h>
#include "IR/Module.h"
#include "Runtime/Intrinsics.h"
#include "Platform/Platform.h"
#include "WAST/WAST.h"
#include "IR/Validate.h"

#include "./wasm_eosio_injection.hpp"

using namespace eosio::chain;
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


size_t read_u32(unsigned char *data, uint32_t& n) {
    size_t shift = 0;
    size_t i = 0;
    uint32_t c;

    n = 0;
    do {
        c = data[i];
        n |= ((c & 0x7f) << shift);
        shift += 7;
        i += 1;
    } while ((c & 0x80) == 0x80);
    return i;
}

size_t write_u32(unsigned char *data, uint32_t n) {
    size_t i = 0;
    do {
        data[i] = n & 0x7f;
        n >>= 7;
        if (n > 0) {
            data[i] |= 0x80;
        }
        i += 1;
    } while (n);
    return i;
}

size_t calc_size(uint32_t n) {
    size_t i = 0;
    do {
        n >>= 7;
        i += 1;
    } while (n);
    return i;
}

size_t write_str(U8 *ptr2, const char *str, size_t size) {
    memcpy(ptr2, str, size);
    return size;
}

using namespace eosio::chain::wasm_injections;

#include <stdio.h>
int main(int argc, char **argv) {
    if (argc != 3) {
        printf("usage wasm_injector in_wasm_file out_wasm_file\n");
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
        for (auto& section: module.userSections) {
            ilog("+++++++${n1} ${n2}", ("n1", section.name)("n2", section.data.size()));
        }
//        module.userSections.clear();
    } catch(const Serialization::FatalSerializationException& e) {
        printf("error: %s\n", e.message.c_str());
        return -1;
    } catch(const IR::ValidationException& e) {
        printf("error: %s\n", e.message.c_str());
        return -1;
    } FC_LOG_AND_DROP()

    try {
        wasm_injections::wasm_binary_injection<false> injector(module);
        injector.inject();
    } FC_LOG_AND_DROP()


	unsigned char *ptr = (unsigned char *)module.userSections[1].data.data();
    vector<U8> new_data(module.userSections[1].data.size()*2);
    unsigned char *ptr2 = new_data.data();

    uint32_t name_type = 0;
    ptr += read_u32(ptr, name_type);
//    ptr2 += write_u32(ptr2, name_type);

    uint32_t sub_section_size = 0;
    ptr += read_u32(ptr, sub_section_size);

    uint32_t num_names = 0;
    ptr += read_u32(ptr, num_names);

    std::map<uint32_t, std::string> index_name_map;

    for (auto& item:injector_utils::registered_injected) {
        auto itr = injector_utils::injected_index_mapping.find(item.second);
        uint32_t function_index = itr->second;
        index_name_map[function_index] = item.first;
    }

    for (auto& item: index_name_map) {
        ptr2 += write_u32(ptr2, item.first);
        ptr2 += write_u32(ptr2, item.second.size());
        ptr2 += write_str(ptr2, item.second.c_str(), item.second.size());
        printf("%u %s\n", item.first, item.second.c_str());
    }

    for (size_t i=0;i<num_names;i++) {
        uint32_t function_index;
        uint32_t name_len;

        ptr += read_u32(ptr, function_index);
        ptr += read_u32(ptr, name_len);
        // printf("%u %s\n", function_index, name);
        ptr2 += write_u32(ptr2, function_index+injector_utils::registered_injected.size());
        ptr2 += write_u32(ptr2, name_len);
        memcpy(ptr2, ptr, name_len);
        ptr2 += name_len;
        ptr += name_len;
    }

    size_t new_data_size = ptr2 - new_data.data();
    size_t new_sub_section_size = new_data_size + calc_size(num_names+injector_utils::registered_injected.size());

    vector<U8> new_section_data(calc_size(name_type) + calc_size(new_sub_section_size) + new_sub_section_size);
    U8 *ptr3 = new_section_data.data();
    ptr3 += write_u32(ptr3, name_type);
    ptr3 += write_u32(ptr3, new_sub_section_size);
    ptr3 += write_u32(ptr3, num_names+injector_utils::registered_injected.size());
    memcpy(ptr3, new_data.data(), new_data_size);

    module.userSections[1].data = new_section_data;

    std::vector<U8> bytes;
    try {
        Serialization::ArrayOutputStream outstream;
        WASM::serialize(outstream, module);
        bytes = outstream.getBytes();
        FILE *fp = fopen(argv[2], "wb");
        fwrite(bytes.data(), 1, bytes.size(), fp);
        fclose(fp);
    } catch(const Serialization::FatalSerializationException& e) {
        printf("error: %s\n", e.message.c_str());
        return -1;
    } catch(const IR::ValidationException& e) {
        printf("error: %s\n", e.message.c_str());
        return -1;
    } FC_LOG_AND_DROP()
    return 0;
}
