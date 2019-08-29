
static int is_feature_active( int64_t feature_name ) {
   return false;
}

static void get_resource_limits( u64 account, u32 ram_bytes_offset, u32 net_weight_offset, u32 cpu_weight_offset ) {
   int64_t *ram_bytes = (int64_t *)offset_to_ptr(ram_bytes_offset, sizeof(int64_t));
   int64_t *net_weight = (int64_t *)offset_to_ptr(net_weight_offset, sizeof(int64_t));
   int64_t *cpu_weight = (int64_t *)offset_to_ptr(cpu_weight_offset, sizeof(int64_t));
   get_vm_api()->get_resource_limits( account, ram_bytes, net_weight, cpu_weight );
}

static void set_resource_limits( u64 account, u64 ram_bytes, u64 net_weight, u64 cpu_weight ) {
   get_vm_api()->set_resource_limits( account, (int64_t)ram_bytes, (int64_t)net_weight, (int64_t)cpu_weight );
}

static u64 set_proposed_producers( u32 producer_data_offset, u32 producer_data_size ) {
   char *producer_data = (char *)offset_to_ptr(producer_data_offset, producer_data_size);
   return (u64)get_vm_api()->set_proposed_producers( producer_data, producer_data_size );
}

static u32 is_privileged( u64 account )  {
   return (u32)get_vm_api()->is_privileged( account );
}

static void set_privileged( u64 account, u32 is_priv ) {
   get_vm_api()->set_privileged( account, is_priv );
}

static void set_blockchain_parameters_packed(u32 data_offset, u32 datalen) {
   char* data = (char *)offset_to_ptr(data_offset, datalen);
   get_vm_api()->set_blockchain_parameters_packed(data, datalen);
}

static u32 get_blockchain_parameters_packed(u32 data_offset, u32 datalen) {
   char* data = (char *)offset_to_ptr(data_offset, datalen);
   return get_vm_api()->get_blockchain_parameters_packed(data, datalen);
}

static void activate_feature( u64 f ) {
   get_vm_api()->activate_feature( (int64_t)f );
}


