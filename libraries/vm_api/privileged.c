#include <capi/types.h>
#include <vm_api_proxy.hpp>

void get_resource_limits( capi_name account, int64_t* ram_bytes, int64_t* net_weight, int64_t* cpu_weight ) {
    get_vm_api()->get_resource_limits(account, ram_bytes, net_weight, cpu_weight);
}

void set_resource_limits( capi_name account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight ) {
    get_vm_api()->set_resource_limits(account, ram_bytes, net_weight, cpu_weight);
}

int64_t set_proposed_producers( char *producer_data, uint32_t producer_data_size ) {
    return get_vm_api()->set_proposed_producers(*producer_data, producer_data_size);
}

int64_t set_proposed_producers_ex( uint64_t producer_data_format, char *producer_data, uint32_t producer_data_size ) {
    return get_vm_api()->set_proposed_producers_ex(producer_data_format, *producer_data, producer_data_size);
}

bool is_privileged( capi_name account ) {
    return get_vm_api()->is_privileged(account);
}

void set_privileged( capi_name account, bool is_priv ) {
    get_vm_api()->set_privileged(account, is_priv);
}

void set_blockchain_parameters_packed( char* data, uint32_t datalen ) {
    get_vm_api()->set_blockchain_parameters_packed(data, datalen);
}

uint32_t get_blockchain_parameters_packed( char* data, uint32_t datalen ) {
    return get_vm_api()->get_blockchain_parameters_packed(data, datalen);
}

void preactivate_feature( const capi_checksum256* feature_digest ) {
    get_vm_api()->preactivate_feature(feature_digest);
}
