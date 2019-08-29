#include "src/interp.h"

#define COPY_UNIT (8) //8 bytes alignment

namespace wabt {
namespace interp {

uint32_t Memory::counter = 1;

std::vector<char> Memory::data;
char *Memory::base_address = nullptr;

std::vector<char> Memory::data_backup;

bool Memory::init_smart_contract = true;
std::vector<uint32_t> Memory::in_use;
std::vector<MemorySegment> *Memory::memory_segments = nullptr;
std::vector<MemorySegmentsCache> Memory::memory_segments_cache;


MemorySegment* FindMemorySegment(Memory* memory, uint32_t offset) {
  if (memory->memory_segments == nullptr) {
    return nullptr;
  }
  uint32_t cache_index = offset/COPY_UNIT;
#if 1
//  printf("offset %u, cache_index %u, memory->memory_segments_cache.size() %lu\n", offset, cache_index, memory->memory_segments_cache.size());
  if (memory->memory_segments_cache[cache_index].counter == memory->counter) {
    uint32_t segment_index = memory->memory_segments_cache[cache_index].index;
    return &memory->memory_segments->at(segment_index);
  }
#endif
  int size = memory->memory_segments->size();
  int low = 0;
  int high = size - 1;
  while (low <= high)
  {
    int mid = low + (high - low) / 2; 
    auto& segment = memory->memory_segments->at(mid);
//      printf("+++++++++++mid %d \n", mid);
    if (offset >= segment.offset + segment.data.size()) {
      low = mid + 1;
    }
    else if ( offset < segment.offset) {
      high = mid - 1;
    } else {
      //if ((offset >= segment.offset) && (offset < segment.offset + segment.data.size()))
//      memory->memory_segments_cache[cache_index].counter = memory->counter;
//      memory->memory_segments_cache[cache_index].index = mid;
      memory->memory_segments_cache[cache_index] = {memory->counter, uint32_t(mid)};
      return &segment;
    }
  }
  return nullptr;
}

bool IsWriteMemoryInUse(Memory *memory, uint32_t write_index) {
  return memory->in_use[write_index] == memory->counter;
}

bool IsInMemorySegments(Memory *memory, uint32_t index) {
  return memory->memory_segments_cache[index].counter == memory->counter;
}

void LoadDataToWritableMemory(Memory *memory, uint32_t write_index) {
    if (memory->init_smart_contract) {
      return;
    }
    if (IsWriteMemoryInUse(memory, write_index)) {
      return;
    }
//    printf("+++LoadDataToWritableMemory %d \n", write_index);
    uint32_t copy_offset = write_index*COPY_UNIT;
    if (copy_offset >= uint32_t(memory->memory_end)) {
      //new malloc memory, no need to find it in memory segments
      //memcpy(memory->base_address+copy_offset, memory->data_backup.data()+copy_offset, COPY_UNIT);
      memset(memory->base_address+copy_offset, 0, COPY_UNIT);
    } else {
      MemorySegment *segment = FindMemorySegment(memory, copy_offset);
      if (segment) {
        //no need to handle segment overflow, as the offset is 8 bytes aligned, it will never overflow under this situation
        memcpy(memory->base_address+copy_offset, segment->data.data()+(copy_offset-segment->offset), COPY_UNIT);
      } else {
        memcpy(memory->base_address+copy_offset, memory->data_backup.data()+copy_offset, COPY_UNIT);
      }
    }
    memory->in_use[write_index] = memory->counter;
}

void LoadDataToWritableMemory(Memory *memory, uint32_t offset_start, uint32_t length) {
    uint32_t aligned_start_index = offset_start/COPY_UNIT;
    uint32_t aligned_end_index = (offset_start + length + COPY_UNIT-1)/COPY_UNIT;
//    printf("++++LoadDataToWritableMemory: %u, %u\n", aligned_start_index, aligned_end_index);
    for (uint32_t write_index=aligned_start_index;write_index<aligned_end_index;write_index++) {
      LoadDataToWritableMemory(memory, write_index);
    }
}

}}
