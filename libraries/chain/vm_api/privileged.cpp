#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include "vm_api_proxy.hpp"


int vm_api_proxy::is_feature_active( int64_t feature_name ) { return false; }

void vm_api_proxy::activate_feature( int64_t feature_name ) {
   EOS_ASSERT( false, unsupported_feature, "Unsupported Hardfork Detected" );
}

void vm_api_proxy::preactivate_feature( const char *digest, size_t size ) {
    digest_type _digest(digest, size);
    legacy_ptr<const digest_type> feature_digest((void *)&_digest);
    _interface->preactivate_feature( std::move(feature_digest) );
}

/**
   * Deprecated in favor of set_resource_limit.
   */
void vm_api_proxy::set_resource_limits( uint64_t account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight ) {
    _interface->set_resource_limits(name(account), ram_bytes, net_weight, cpu_weight);
}

/**
   * Deprecated in favor of get_resource_limit.
   */
void vm_api_proxy::get_resource_limits( uint64_t account, int64_t *ram_bytes, int64_t *net_weight, int64_t *cpu_weight ) {
    legacy_ptr<int64_t> _ram_bytes(ram_bytes);
    legacy_ptr<int64_t> _net_weight(net_weight);
    legacy_ptr<int64_t> _cpu_weight(cpu_weight);
    _interface->get_resource_limits(name(account), std::move(_ram_bytes), std::move(_net_weight), std::move(_cpu_weight));
}

void vm_api_proxy::set_resource_limit( uint64_t account, uint64_t resource, int64_t limit ) {
    _interface->set_resource_limit(name(account), name(resource), limit);
}

int64_t vm_api_proxy::get_resource_limit( uint64_t account, uint64_t resource ) {
    return _interface->get_resource_limit(name(account), name(resource));
}

uint32_t vm_api_proxy::get_wasm_parameters_packed( char *packed_parameters, size_t size, uint32_t max_version ) {
    span<char> _packed_parameters(packed_parameters, size);
    return _interface->get_wasm_parameters_packed(std::move(_packed_parameters), max_version);
}

void vm_api_proxy::set_wasm_parameters_packed( const char *packed_parameters, size_t size ) {
    span<const char> _packed_parameters(packed_parameters, size);
    _interface->set_wasm_parameters_packed(std::move(_packed_parameters));
}

int64_t vm_api_proxy::set_proposed_producers(const char *packed_producer_schedule, size_t size) {
    legacy_span<const char> _packed_producer_schedule((void *)packed_producer_schedule, size);
    return _interface->set_proposed_producers(std::move(_packed_producer_schedule));
}

int64_t vm_api_proxy::set_proposed_producers_ex( uint64_t packed_producer_format, const char *packed_producer_schedule, size_t size) {
    legacy_span<const char> _packed_producer_schedule((void *)packed_producer_schedule, size);
    return _interface->set_proposed_producers_ex(packed_producer_format, std::move(_packed_producer_schedule));
}

uint32_t vm_api_proxy::get_blockchain_parameters_packed( char *packed_blockchain_parameters, size_t size) {
    legacy_span<char> _packed_blockchain_parameters(packed_blockchain_parameters, size);
    return _interface->get_blockchain_parameters_packed(std::move(_packed_blockchain_parameters));
}

void vm_api_proxy::set_blockchain_parameters_packed( const char *packed_blockchain_parameters, size_t size ) {
    legacy_span<const char> _packed_blockchain_parameters((void *)packed_blockchain_parameters, size);
    _interface->set_blockchain_parameters_packed(std::move(_packed_blockchain_parameters));
}

uint32_t vm_api_proxy::get_parameters_packed( const char *packed_parameter_ids, size_t size1, char *packed_parameters, size_t size2) {
    span<const char> _packed_parameter_ids(packed_parameter_ids, size1);
    span<char> _packed_parameters(packed_parameters, size2);
    return _interface->get_parameters_packed(std::move(_packed_parameter_ids), std::move(_packed_parameters));
}

void vm_api_proxy::set_parameters_packed( const char *packed_parameters, size_t size ) {
    span<const char> _packed_parameters(packed_parameters, size);
    _interface->set_parameters_packed(std::move(_packed_parameters));
}

uint32_t vm_api_proxy::get_kv_parameters_packed( char *packed_kv_parameters, size_t size, uint32_t max_version ) {
    span<char> _packed_kv_parameters(packed_kv_parameters, size);
    return _interface->get_kv_parameters_packed(std::move(_packed_kv_parameters), max_version);
}

void vm_api_proxy::set_kv_parameters_packed( const char *packed_kv_parameters, size_t size ) {
    span<const char> _packed_kv_parameters(packed_kv_parameters, size);
    _interface->set_kv_parameters_packed(_packed_kv_parameters);
}

bool vm_api_proxy::is_privileged( uint64_t n ) {
    return _interface->is_privileged(name(n));
}

void vm_api_proxy::set_privileged( uint64_t n, bool is_priv ) {
    _interface->set_privileged(name(n), is_priv);
}
