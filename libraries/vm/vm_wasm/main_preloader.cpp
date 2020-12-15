
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
#include <stdio.h>

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

extern "C" {
    int micropython_init();
    void *micropython_get_memory();
    size_t micropython_get_memory_size();
}

using namespace std;

struct data_segment {
    int offset;
    vector<uint8_t> data;
};

vector<data_segment> take_snapshot(const char *inital_memory, const char *current_memory) {
    vector<data_segment> segments;

    const char *ptr1 = inital_memory;
    const char *ptr2 = current_memory;
    int block_size = 64;

    int initial_memory_size = 64*1024;
    int pos = PYTHON_VM_STACK_SIZE; //do not save stack data as it's temperately
    int total_size = 0;
    while(pos<initial_memory_size) {
        if (memcmp(ptr1+pos, ptr2+pos, block_size) == 0) {
            pos += block_size;
            continue;
        }
        int start = pos;
        pos += block_size;
        while (pos<initial_memory_size) {
            if (memcmp(ptr1+pos, ptr2+pos, block_size) == 0) {
                break;
            }
            pos += block_size;
        }

        int copy_size = pos-start;
        pos += block_size;
        total_size += copy_size;
        data_segment segment;
        segment.offset = start;
        segment.data.resize(copy_size);
        memcpy(segment.data.data(), ptr2 + start, copy_size);
        segments.push_back(segment);
        printf("++++++++start: %d, size: %d\n", start, copy_size);
    }
    printf("++++++++total_size: %d \n", total_size);
    return segments;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("usage wasm_preloader in_wasm_file in_wasm_memory_dump.bin out_wasm_file\n");
        return 0;
    }
    
    int wasm_memory_dump_size;
    const char *wasm_memory_dump = read_file(argv[2], &wasm_memory_dump_size);
    if (wasm_memory_dump == nullptr || wasm_memory_dump_size <= 0) {
        printf("read dump file failed!\n");
        return -1;
    }

    char *origin_memory = (char *)malloc(wasm_memory_dump_size);
    memset(origin_memory, 0, wasm_memory_dump_size);
    vector<data_segment> segments = take_snapshot(origin_memory, wasm_memory_dump);

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

    // int total_size = 0;
    // for ( const DataSegment& ds : module.dataSegments ) {
    //     printf("+++++offset:%d, size: %d\n", ds.baseOffset.i32, ds.data.size());
    //     total_size += ds.data.size();
    // }
    // printf("++++total_size %d\n", total_size);

    DataSegment ds = module.dataSegments[0];
    module.dataSegments.clear();
    int max_segment_size = 8192/2;
    for(auto& _ds: segments) {
        for(int i=0;i<_ds.data.size();i+=max_segment_size) {
            int segment_size;
            if (i + max_segment_size > _ds.data.size()) {
                segment_size = _ds.data.size() - i;
            } else {
                segment_size = max_segment_size;
            }
            ds.baseOffset.i32 = _ds.offset + i;
            ds.data.resize(segment_size);
            memcpy(ds.data.data(), &_ds.data[i], segment_size);
            module.dataSegments.push_back(ds);
        }
    }

    std::vector<U8> bytes;
    try {
        Serialization::ArrayOutputStream outstream;
        WASM::serialize(outstream, module);
        bytes = outstream.getBytes();
        FILE *fp = fopen(argv[3], "wb");
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
