#pragma once
#include <stdint.h>
#include <vector>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>


#define VM_PAGE_SIZE (65536)
namespace pythonvm {

using boost::multi_index_container;
using namespace boost::multi_index;

struct memory_segment {
    uint32_t offset;
    std::vector<char> data;
};

struct memory_segments_cache {
    uint32_t counter;
    uint32_t index;
};

struct by_offset;

typedef boost::multi_index_container<memory_segment,
    indexed_by<
        ordered_unique<tag<by_offset>,
            member<memory_segment, uint32_t, &memory_segment::offset>
        >
    >
> memory_segment_index;

struct vm_memory {
    vm_memory() = default;
    explicit vm_memory(uint32_t initial_pages, uint32_t max_pages);
    void backup_memory();
    void init_cache();

    inline bool is_write_memory_in_use(uint32_t write_index);

    void load_data_to_writable_memory(uint32_t offset_start, uint32_t length);
    inline void load_data_to_writable_memory(uint32_t write_index);
    inline const memory_segment* find_memory_segment(uint32_t offset);
    
    uint32_t initial_pages;
    uint32_t max_pages;
    uint32_t malloc_memory_start;
    std::vector<uint32_t> in_use;
    uint32_t counter;
    bool init_smart_contract;

    char *base_address;
    std::vector<char> data;
    std::vector<char> data_backup;
//    std::vector<memory_segment> *segments;
    memory_segment_index *segments;
};

}