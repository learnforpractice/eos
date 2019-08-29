
static void send_deferred(u32 sender_id_offset, u64 payer, u32 serialized_transaction_offset, u32 size, u32 replace_existing) {
   uint128_t *sender_id = (uint128_t *)offset_to_ptr(sender_id_offset, sizeof(uint128_t));
   const char *serialized_transaction = (char *)offset_to_ptr(serialized_transaction_offset, size);
   get_vm_api()->send_deferred(sender_id, payer,serialized_transaction, size, replace_existing);
}

static u32 cancel_deferred(u32 sender_id_offset) {
   uint128_t *sender_id = (uint128_t *)offset_to_ptr(sender_id_offset, sizeof(uint128_t));
   return (u32)get_vm_api()->cancel_deferred(sender_id);
}

static u32 read_transaction(u32 buffer_offset, u32 size) {
   char *buffer = (char *)offset_to_ptr(buffer_offset, size);
   return get_vm_api()->read_transaction(buffer, size);
}

static u32 transaction_size() {
   return get_vm_api()->transaction_size();
}

static u32 tapos_block_num() {
   return (u32)get_vm_api()->tapos_block_num();
}

static u32 tapos_block_prefix() {
   return (u32)get_vm_api()->tapos_block_prefix();
}

static u32 expiration() {
   return get_vm_api()->expiration();
}

static u32 get_action( u32 type, u32 index, u32 buff_offset, u32 size ) {
   char *buff = (char *)offset_to_ptr(buff_offset, size);
   return (u32)get_vm_api()->get_action( type, index, buff, size );
}

static void assert_privileged() {
   get_vm_api()->assert_privileged();
}

static void assert_context_free() {
   get_vm_api()->assert_context_free();
}

static u32 get_context_free_data( u32 index, u32 buff_offset, u32 size ) {
   char* buff = (char *)offset_to_ptr(buff_offset, size);
   return get_vm_api()->get_context_free_data( index, buff, size );
}
