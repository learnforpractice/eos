
void _send_deferred(const uint128_t* sender_id, uint64_t payer, const char *data, size_t data_len, uint32_t replace_existing) {
   CALL_NOT_ALLOWED();
}

int _cancel_deferred(const uint128_t* val) {
   CALL_NOT_ALLOWED(); 
   return 0;
}

size_t read_transaction(char *data, size_t buffer_size) {
   CALL_NOT_ALLOWED();
   return 0;
}

size_t transaction_size() {
   return 0;
}

int tapos_block_num() {
   CALL_NOT_ALLOWED();
   return 0;
}

int tapos_block_prefix() {
   return 0;
}

uint32_t expiration() {
   CALL_NOT_ALLOWED();
   return 0;
}

int get_action( uint32_t type, uint32_t index, char* buffer, size_t buffer_size ) {
   return 0;
//   return ctx().get_action( type, index, buffer, buffer_size );
}

void assert_privileged() {
   CALL_NOT_ALLOWED();
}

void assert_context_free() {
   CALL_NOT_ALLOWED();
}

int get_context_free_data( uint32_t index, char* buffer, size_t buffer_size ) {
   CALL_NOT_ALLOWED();
   return 0;
}

