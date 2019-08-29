/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

uint32_t read_action_data( void* msg, uint32_t buffer_size ) {
   auto s = ctx().get_action().data.size();
   if( buffer_size == 0 || msg == NULL) return s;

   auto copy_size = std::min( (size_t)buffer_size, s );
   memcpy( msg, ctx().get_action().data.data(), copy_size );

   return copy_size;

}

uint32_t action_data_size() {
   return ctx().get_action().data.size();
}

void get_action_info(uint64_t* account, uint64_t* name) {
   *account = ctx().get_action().account;
   *name = ctx().get_action().name;
}

uint64_t current_receiver() {
   return ctx().get_receiver();
}

void require_recipient( uint64_t name ) {
   CALL_NOT_ALLOWED();
//   ctx().require_recipient(name);
}

void require_auth( uint64_t name ) {
   CALL_NOT_ALLOWED();
}

void require_auth2( uint64_t name, uint64_t permission ) {
   CALL_NOT_ALLOWED();
}

bool has_auth( uint64_t name ) {
   CALL_NOT_ALLOWED();
   return false;
}

bool is_account( uint64_t name ) {
   return ctx().is_account(name);
}

void send_inline(const char *data, size_t data_len) {
   CALL_NOT_ALLOWED();
}

void send_context_free_inline(const char *data, size_t data_len) {
   CALL_NOT_ALLOWED();
}

uint64_t  publication_time() {
   CALL_NOT_ALLOWED();
}

bool is_protocol_feature_activated(const char *digest, size_t size) {
   CALL_NOT_ALLOWED();
   return false;
}

uint64_t get_sender() {
   CALL_NOT_ALLOWED();
   return 0;
}

