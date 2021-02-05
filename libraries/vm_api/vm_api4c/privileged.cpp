#include "vm_api4c.hpp"
#include <vm_api_proxy.hpp>

void vm_api4c_proxy::get_resource_limits( u64 account, u32 ram_bytes_offset, u32 net_weight_offset, u32 cpu_weight_offset ) {
   int64_t *ram_bytes = (int64_t *)offset_to_ptr(ram_bytes_offset, sizeof(int64_t));
   int64_t *net_weight = (int64_t *)offset_to_ptr(net_weight_offset, sizeof(int64_t));
   int64_t *cpu_weight = (int64_t *)offset_to_ptr(cpu_weight_offset, sizeof(int64_t));
   get_vm_api()->get_resource_limits( account, ram_bytes, net_weight, cpu_weight );
}

/* import: 'env' 'set_resource_limits' */
void vm_api4c_proxy::set_resource_limits( u64 account, u64 ram_bytes, u64 net_weight, u64 cpu_weight ) {
   get_vm_api()->set_resource_limits( account, (int64_t)ram_bytes, (int64_t)net_weight, (int64_t)cpu_weight );
}

/* import: 'env' 'set_proposed_producers' */
u64 vm_api4c_proxy::set_proposed_producers( u32 producer_data_offset, u32 producer_data_size ) {
   char *producer_data = (char *)offset_to_ptr(producer_data_offset, producer_data_size);
   return (u64)get_vm_api()->set_proposed_producers( producer_data, producer_data_size );
}

/* import: 'env' 'is_privileged' */
u32 vm_api4c_proxy::is_privileged( u64 account )  {
   return (u32)get_vm_api()->is_privileged( account );
}

/* import: 'env' 'set_privileged' */
void vm_api4c_proxy::set_privileged( u64 account, u32 is_priv ) {
   get_vm_api()->set_privileged( account, is_priv );
}

/* import: 'env' 'set_blockchain_parameters_packed' */
void vm_api4c_proxy::set_blockchain_parameters_packed(u32 data_offset, u32 datalen) {
   char* data = (char *)offset_to_ptr(data_offset, datalen);
   get_vm_api()->set_blockchain_parameters_packed(data, datalen);
}

/* import: 'env' 'get_blockchain_parameters_packed' */
u32 vm_api4c_proxy::get_blockchain_parameters_packed(u32 data_offset, u32 datalen) {
   char* data = (char *)offset_to_ptr(data_offset, datalen);
   return get_vm_api()->get_blockchain_parameters_packed(data, datalen);
}

/* import: 'env' 'activate_feature' */
void vm_api4c_proxy::activate_feature( u64 f ) {
   get_vm_api()->activate_feature( (int64_t)f );
}

void vm_api4c_proxy::preactivate_feature(u32 feature_offset) {
    capi_checksum256* feature = (capi_checksum256 *)offset_to_ptr(feature_offset, 32);
    get_vm_api()->preactivate_feature(feature);
}


