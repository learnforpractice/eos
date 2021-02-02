#include <vm_api_proxy.hpp>

extern "C" {

int is_feature_active( int64_t feature_name ) {
    return get_vm_api()->is_feature_active(feature_name);
}

void activate_feature( int64_t feature_name ) {
    get_vm_api()->activate_feature(feature_name);
}

void preactivate_feature( const char *digest, size_t size ) {
    get_vm_api()->preactivate_feature( digest, size );
}

/**
   * Deprecated in favor of set_resource_limit.
   */
void set_resource_limits( uint64_t account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight ) {
    get_vm_api()->set_resource_limits(account, ram_bytes, net_weight, cpu_weight);
}

/**
   * Deprecated in favor of get_resource_limit.
   */
void get_resource_limits( uint64_t account, int64_t *ram_bytes, int64_t *net_weight, int64_t *cpu_weight ) {
    get_vm_api()->get_resource_limits(account, ram_bytes, net_weight, cpu_weight);
}

void set_resource_limit( uint64_t account, uint64_t resource, int64_t limit ) {
    get_vm_api()->set_resource_limit(account, resource, limit);
}

int64_t get_resource_limit( uint64_t account, uint64_t resource ) {
    return get_vm_api()->get_resource_limit(account, resource);
}

uint32_t get_wasm_parameters_packed( char *packed_parameters, size_t size, uint32_t max_version ) {
    return get_vm_api()->get_wasm_parameters_packed(packed_parameters, size, max_version);
}

void set_wasm_parameters_packed( const char *packed_parameters, size_t size ) {
    get_vm_api()->set_wasm_parameters_packed(packed_parameters, size);
}

int64_t set_proposed_producers(const char *packed_producer_schedule, size_t size) {
    return get_vm_api()->set_proposed_producers(packed_producer_schedule, size);
}

int64_t set_proposed_producers_ex( uint64_t packed_producer_format, const char *packed_producer_schedule, size_t size) {
    return get_vm_api()->set_proposed_producers_ex(packed_producer_format, packed_producer_schedule, size);
}

uint32_t get_blockchain_parameters_packed( char *packed_blockchain_parameters, size_t size) {
    return get_vm_api()->get_blockchain_parameters_packed(packed_blockchain_parameters, size);
}

void set_blockchain_parameters_packed( const char *packed_blockchain_parameters, size_t size ) {
    get_vm_api()->set_blockchain_parameters_packed(packed_blockchain_parameters, size);
}

uint32_t get_parameters_packed( const char *packed_parameter_ids, size_t size1, char *packed_parameters, size_t size2) {
    return get_vm_api()->get_parameters_packed(packed_parameter_ids, size1, packed_parameters, size2);
}

void set_parameters_packed( const char *packed_parameters, size_t size ) {
    get_vm_api()->set_parameters_packed(packed_parameters, size);
}

uint32_t get_kv_parameters_packed( char *packed_kv_parameters, size_t size, uint32_t max_version ) {
    return get_vm_api()->get_kv_parameters_packed(packed_kv_parameters, size, max_version);
}

void set_kv_parameters_packed( const char *packed_kv_parameters, size_t size ) {
    get_vm_api()->set_kv_parameters_packed(packed_kv_parameters, size);
}

bool is_privileged( uint64_t n ) {
    return get_vm_api()->is_privileged(n);
}

void set_privileged( uint64_t n, bool is_priv ) {
    get_vm_api()->set_privileged(n, is_priv);
}

}
