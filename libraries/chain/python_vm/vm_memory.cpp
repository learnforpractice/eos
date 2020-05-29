#include <python_vm_config.h>
#include <string.h>
#include <eosio/chain/python_vm/vm_memory.h>
#include <vm_api/vm_api.h>

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

void vm_memory::inc_counter() {
    counter += 1;
    if (counter == 0) {//overflow, reset use flags
        counter = 1;
        memset(in_use.data(), 0, in_use.size());
    }
}


void vm_memory::restore_memory() {
    if (segments == nullptr) {
        return;
    }
    for (auto itr=segments->begin();itr!=segments->end();itr++) {
        memcpy(base_address+itr->offset, itr->data.data(), itr->data.size());
        int start_index = itr->offset/COPY_UNIT;
        int end_index = (itr->offset+itr->data.size())/COPY_UNIT;
        for (int i=start_index;i<end_index;i+=1) {
            in_use[i] = counter;
        }
    }
}

void vm_memory::load_data_to_writable_memory(uint32_t write_index) {
    if (init_smart_contract) {
        return;
    }

    if (is_write_memory_in_use(write_index)) {
        return;
    }

    uint32_t copy_offset = write_index*COPY_UNIT;

    if (copy_offset < PYTHON_VM_STACK_SIZE) {
        if (copy_offset == 0) {
            get_vm_api()->eosio_assert(false, "load_data_to_writable_memory: vm stack overflow!");
        }
        int count = 2;
        for (int i=write_index;i>=1;i--) {
            if (in_use[i] == counter) {
                break;
            } else {
                memset(base_address+i*COPY_UNIT, 0, COPY_UNIT);
                in_use[i] = counter;
                count -= 1;
                if (count <= 0) {
                    break;
                }
            }
        }
        return;
    }

    if (malloc_memory_start > 0 && copy_offset >= malloc_memory_start) {
        int max_reset_size  = 64; // must be 8 bytes aligned
        if (copy_offset + max_reset_size  > data.size()) {
            max_reset_size  = data.size() - copy_offset;
        }
        int end_index = write_index+max_reset_size/COPY_UNIT;
        //initilize memory as much as possible
        for (int i=write_index;i<end_index;i++) {
            if (in_use[i] == counter) {
                break;
            } else {
                in_use[i] = counter;
                memset(base_address+i*COPY_UNIT, 0, COPY_UNIT);
            }
        }
    } else {
//        printf("++++++++++copy_offset %d\n", copy_offset);
        memcpy(base_address+copy_offset, data_backup.data()+copy_offset, COPY_UNIT);
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
