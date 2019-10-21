
#include <stdint.h>
#include <stdlib.h>

#include <vm_api/vm_api.h>
#include "vm_api4c.h"

static int is_feature_active( int64_t feature_name ) {
   return false;
}


extern "C" {
static void get_resource_limits( u64 account, u32 ram_bytes_offset, u32 net_weight_offset, u32 cpu_weight_offset ) {
   int64_t *ram_bytes = (int64_t *)offset_to_ptr(ram_bytes_offset, sizeof(int64_t));
   int64_t *net_weight = (int64_t *)offset_to_ptr(net_weight_offset, sizeof(int64_t));
   int64_t *cpu_weight = (int64_t *)offset_to_ptr(cpu_weight_offset, sizeof(int64_t));
   get_vm_api()->get_resource_limits( account, ram_bytes, net_weight, cpu_weight );
}

/* import: 'env' 'set_resource_limits' */
static void set_resource_limits( u64 account, u64 ram_bytes, u64 net_weight, u64 cpu_weight ) {
   get_vm_api()->set_resource_limits( account, (int64_t)ram_bytes, (int64_t)net_weight, (int64_t)cpu_weight );
}

/* import: 'env' 'set_proposed_producers' */
static u64 set_proposed_producers( u32 producer_data_offset, u32 producer_data_size ) {
   char *producer_data = (char *)offset_to_ptr(producer_data_offset, producer_data_size);
   return (u64)get_vm_api()->set_proposed_producers( producer_data, producer_data_size );
}

/* import: 'env' 'is_privileged' */
static u32 is_privileged( u64 account )  {
   return (u32)get_vm_api()->is_privileged( account );
}

/* import: 'env' 'set_privileged' */
static void set_privileged( u64 account, u32 is_priv ) {
   get_vm_api()->set_privileged( account, is_priv );
}

/* import: 'env' 'set_blockchain_parameters_packed' */
static void set_blockchain_parameters_packed(u32 data_offset, u32 datalen) {
   char* data = (char *)offset_to_ptr(data_offset, datalen);
   get_vm_api()->set_blockchain_parameters_packed(data, datalen);
}

/* import: 'env' 'get_blockchain_parameters_packed' */
static u32 get_blockchain_parameters_packed(u32 data_offset, u32 datalen) {
   char* data = (char *)offset_to_ptr(data_offset, datalen);
   return get_vm_api()->get_blockchain_parameters_packed(data, datalen);
}

/* import: 'env' 'activate_feature' */
static void activate_feature( u64 f ) {
   get_vm_api()->activate_feature( (int64_t)f );
}

static void preactivate_feature(u32 feature_offset) {
   char* feature = (char *)offset_to_ptr(feature_offset, 32);
    get_vm_api()->preactivate_feature(feature, 32);
}


//privileged.cpp	
void (*Z_envZ_get_resource_limitsZ_vjiii)(u64, u32, u32, u32);	
/* import: 'env' 'set_resource_limits' */	
void (*Z_envZ_set_resource_limitsZ_vjjjj)(u64, u64, u64, u64);	
/* import: 'env' 'set_proposed_producers' */	
u64 (*Z_envZ_set_proposed_producersZ_jii)(u32, u32);	
/* import: 'env' 'is_privileged' */	
u32 (*Z_envZ_is_privilegedZ_ij)(u64);	
/* import: 'env' 'set_privileged' */	
void (*Z_envZ_set_privilegedZ_vji)(u64, u32);	
/* import: 'env' 'set_blockchain_parameters_packed' */	
void (*Z_envZ_set_blockchain_parameters_packedZ_vii)(u32, u32);	
/* import: 'env' 'get_blockchain_parameters_packed' */	
u32 (*Z_envZ_get_blockchain_parameters_packedZ_iii)(u32, u32);	
/* import: 'env' 'activate_feature' */	
void (*Z_envZ_activate_featureZ_vj)(u64);	
/* import: 'env' 'preactivate_feature' */
void (*Z_envZ_preactivate_featureZ_vi)(u32);

void init_privileged() {
    Z_envZ_get_resource_limitsZ_vjiii = get_resource_limits;	
    Z_envZ_set_resource_limitsZ_vjjjj = set_resource_limits;	
    Z_envZ_set_proposed_producersZ_jii = set_proposed_producers;	
    Z_envZ_is_privilegedZ_ij = is_privileged;	
    Z_envZ_set_privilegedZ_vji = set_privileged;	
    Z_envZ_set_blockchain_parameters_packedZ_vii = set_blockchain_parameters_packed;	
    Z_envZ_get_blockchain_parameters_packedZ_iii = get_blockchain_parameters_packed;	
    Z_envZ_activate_featureZ_vj = activate_feature;
    Z_envZ_preactivate_featureZ_vi = preactivate_feature;
}

}


