#pragma once
#include <stdint.h>
#include <vector>

#define VM_PAGE_SIZE (65536)
namespace pythonvm {

struct memory_segment {
    uint32_t offset;
    std::vector<char> data;
};

struct memory_segments_cache {
    uint32_t counter;
    uint32_t index;
};

struct vm_memory {
    vm_memory() = default;
    explicit vm_memory(uint32_t initial_pages, uint32_t max_pages);
    void backup_memory();
    void init_cache();

    bool is_write_memory_in_use(uint32_t write_index);

    void load_data_to_writable_memory(uint32_t offset_start, uint32_t length);
    void load_data_to_writable_memory(uint32_t write_index);
    memory_segment* find_memory_segment(uint32_t offset);
    
    uint32_t initial_pages;
    uint32_t max_pages;
    int malloc_memory_start;
    std::vector<uint32_t> in_use;
    uint32_t counter;
    bool init_smart_contract;

    char *base_address;
    std::vector<char> data;
    std::vector<char> data_backup;
    std::vector<memory_segment> *segments;
    std::vector<memory_segments_cache> segments_cache;
};

}