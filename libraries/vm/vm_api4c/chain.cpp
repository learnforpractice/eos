
static u32 get_active_producers( u32 producers_offset, uint32_t datalen ) {
   u64* producers = (u64*)offset_to_ptr(producers_offset, datalen);
   return get_vm_api()->get_active_producers(producers, datalen);
}

