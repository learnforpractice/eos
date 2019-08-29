
void set_resource_limits( uint64_t account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight ) {
   CALL_NOT_ALLOWED();
}

void get_resource_limits( uint64_t account, int64_t* ram_bytes, int64_t* net_weight, int64_t* cpu_weight ) {
   get_chain_api()->get_resource_limits( account, ram_bytes, net_weight, cpu_weight);
}

int64_t set_proposed_producers( char *packed_producer_schedule, uint32_t datalen ) {
   CALL_NOT_ALLOWED();
   return 0;
}

bool is_privileged( uint64_t n )  {
   CALL_NOT_ALLOWED();
   return false;
}

void set_privileged( uint64_t n, bool is_priv ) {
   CALL_NOT_ALLOWED();
}

void set_blockchain_parameters_packed(char* packed_blockchain_parameters, uint32_t datalen) {
   CALL_NOT_ALLOWED();
}

uint32_t get_blockchain_parameters_packed(char* packed_blockchain_parameters, uint32_t buffer_size) {
   auto& gpo = chain_api_get_controller().get_global_properties();

   auto s = fc::raw::pack_size( gpo.configuration );
   if( buffer_size == 0 ) return s;

   if ( s <= buffer_size ) {
      datastream<char*> ds( packed_blockchain_parameters, s );
      fc::raw::pack(ds, gpo.configuration);
      return s;
   }
   return 0;
}

void activate_feature( int64_t f ) {
   EOS_ASSERT( false, unsupported_feature, "Unsupported Hardfork Detected" );
}

