/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

void eosio_abort() {
   edump(("abort() called"));
   EOS_ASSERT( false, abort_called, "abort() called");
}

void  eosio_assert( uint32_t test, const char* msg ) {
   if( BOOST_UNLIKELY( !test ) ) {
      std::string message( msg );
      edump((message));
      EOS_THROW( eosio_assert_message_exception, "assertion failure with message: ${s}", ("s",message) );
   }
}

void  eosio_assert_message( uint32_t test, const char* msg, uint32_t msg_len ) {
   if( BOOST_UNLIKELY( !test ) ) {
      std::string message( msg, msg_len );
      edump((message));
      EOS_THROW( eosio_assert_message_exception, "assertion failure with message: ${s}", ("s",message) );
   }
}

void  eosio_assert_code( uint32_t test, uint64_t error_code ) {
   if( BOOST_UNLIKELY( !test ) ) {
      edump((error_code));
      EOS_THROW( eosio_assert_code_exception,
                 "assertion failure with error code: ${error_code}", ("error_code", error_code) );
   }
}

void  eosio_exit( int32_t code ) {
   throw wasm_exit{code};
}

uint64_t  current_time() {
   CALL_NOT_ALLOWED();
}

uint32_t  now() {
   return (uint32_t)( current_time() / 1000000 );
}

void checktime() {
   try {
      ctx().checktime();
   } FC_LOG_AND_RETHROW();
}

void check_context_free(bool context_free) {
   return;
//   CALL_NOT_ALLOWED();
}

bool contracts_console() {
   return chain_api_get_controller().contracts_console();
}

static std::vector<char> last_error;

void set_last_error(const char* error, size_t size) {
   last_error.resize(size);
   memcpy(last_error.data(), error, size);
}

size_t get_last_error(char* error, size_t size) {
   if (size == 0 || error == nullptr) {
      return last_error.size();
   }
   int copy_size = std::min(size, last_error.size());
   memcpy(error, last_error.data(), copy_size);
   return copy_size;
}

void clear_last_error() {
   last_error.clear();
}

