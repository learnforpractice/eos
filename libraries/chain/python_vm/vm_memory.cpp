#include <python_vm_config.h>
#include <string.h>
#include <eosio/chain/python_vm/vm_memory.h>

#define COPY_UNIT (8) //8 bytes alignment

namespace pythonvm {

vm_memory::vm_memory(uint32_t initial_pages, uint32_t max_pages):
initial_pages(initial_pages)
,max_pages(max_pages)
,counter(1)
,init_smart_contract(true)
,segments(nullptr)
{
    data.resize(initial_pages * VM_PAGE_SIZE, 0x00);
    base_address = data.data();
    malloc_memory_start = 0;

    init_cache();
}

void vm_memory::backup_memory() {
    data_backup = data;
}

void vm_memory::init_cache() {
    uint32_t memory_size = data.size();
    in_use.resize(memory_size/(sizeof(uint64_t)), {});
}

const memory_segment* vm_memory::find_memory_segment(uint32_t offset) {
    if (segments == nullptr) {
      return nullptr;
    }
    
    if (segments->begin() == segments->end()) {
        return nullptr;
    }

    auto itr = segments->upper_bound({offset, {}});
    if (itr == segments->begin()) {
        return nullptr;
    }
    --itr;
    if (offset >=itr->offset && offset < itr->offset+itr->data.size()) {
        return &*itr;
    }
    return nullptr;
}

bool vm_memory::is_write_memory_in_use(uint32_t write_index) {
  return in_use[write_index] == counter;
}

void vm_memory::load_data_to_writable_memory(uint32_t write_index) {
    if (init_smart_contract) {
        return;
    }

    if (is_write_memory_in_use(write_index)) {
        return;
    }

    uint32_t copy_offset = write_index*COPY_UNIT;

    if (write_index < PYTHON_VM_STACK_SIZE/COPY_UNIT) {
        memset(base_address+copy_offset, 0, COPY_UNIT);
        in_use[write_index] = counter;
        return;
    }

    if (malloc_memory_start > 0 && copy_offset >= malloc_memory_start) {
        memset(base_address+copy_offset, 0, COPY_UNIT);
    } else {
        const memory_segment *segment = find_memory_segment(copy_offset);
        if (segment) {
            memcpy(base_address+segment->offset, segment->data.data(), segment->data.size());
            for (int i=segment->offset;i<segment->offset+segment->data.size();i+=COPY_UNIT) {
                in_use[i/COPY_UNIT] = counter;
            }
        } else {
            memcpy(base_address+copy_offset, data_backup.data()+copy_offset, COPY_UNIT);
        }
    }
    in_use[write_index] = counter;
}

void vm_memory::load_data_to_writable_memory(uint32_t offset_start, uint32_t length) {
    uint32_t aligned_start_index = offset_start/COPY_UNIT;
    uint32_t aligned_end_index = (offset_start + length + COPY_UNIT-1)/COPY_UNIT;
    for (uint32_t write_index=aligned_start_index;write_index<aligned_end_index;write_index++) {
        load_data_to_writable_memory(write_index);
    }
}

}
