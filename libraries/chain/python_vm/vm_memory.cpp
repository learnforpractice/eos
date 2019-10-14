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
}

void vm_memory::backup_memory() {
    data_backup = data;
}

void vm_memory::init_cache() {
    uint32_t memory_size = data.size();
    in_use.resize(memory_size/(sizeof(uint64_t)), {});
    segments_cache.resize(memory_size/(sizeof(uint64_t)), {});
}

memory_segment* vm_memory::find_memory_segment(uint32_t offset) {
    if (segments == nullptr) {
      return nullptr;
    }
    uint32_t cache_index = offset/COPY_UNIT;
  #if 1
  //  printf("offset %u, cache_index %u, memory->segments_cache.size() %lu\n", offset, cache_index, memory->segments_cache.size());
    if (segments_cache[cache_index].counter == counter) {
        uint32_t segment_index = segments_cache[cache_index].index;
        return &segments->at(segment_index);
    }
  #endif
    int size = segments->size();
    int low = 0;
    int high = size - 1;
    while (low <= high)
    {
        int mid = low + (high - low) / 2; 
        auto& segment = segments->at(mid);
        if (offset >= segment.offset + segment.data.size()) {
            low = mid + 1;
        }
        else if ( offset < segment.offset) {
            high = mid - 1;
        } else {
            segments_cache[cache_index] = {counter, uint32_t(mid)};
            return &segment;
        }
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

    if (copy_offset >= uint32_t(malloc_memory_start)) {
        memset(base_address+copy_offset, 0, COPY_UNIT);
    } else {
        memory_segment *segment = find_memory_segment(copy_offset);
        if (segment) {
            //no need to handle segment overflow, as the offset is 8 bytes aligned, it will never overflow under this situation
            memcpy(base_address+copy_offset, segment->data.data()+(copy_offset-segment->offset), COPY_UNIT);
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
