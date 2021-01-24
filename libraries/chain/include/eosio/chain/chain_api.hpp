#pragma once
#include <eosio/chain/controller.hpp>

namespace eosio { namespace chain {

class chain_api {

public:
    chain_api(const controller::config& conf, controller& ctrl);
    virtual const chainbase::database& db();
    virtual const boost::filesystem::path state_dir();
    virtual bool get_code_by_code_hash(const digest_type& code_hash, const uint8_t vm_type, const uint8_t vm_version, const char** code, size_t* size, uint32_t* first_block_used);

#if defined(EOSIO_EOS_VM_RUNTIME_ENABLED) || defined(EOSIO_EOS_VM_JIT_RUNTIME_ENABLED)
    virtual eosio::vm::wasm_allocator& get_wasm_allocator();
#endif

public:
    controller::config conf;

private:
    controller &c;
};

} } //eosio::chain
