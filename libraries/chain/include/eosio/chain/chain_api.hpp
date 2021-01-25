#pragma once
#include <eosio/chain/controller.hpp>

namespace eosio { namespace chain {

class apply_context;

class chain_api {

public:
    chain_api(const controller::config& conf, controller& ctrl);
    virtual const chainbase::database& db();
    virtual const boost::filesystem::path state_dir();
    virtual bool get_code_by_code_hash(const digest_type& code_hash, const uint8_t vm_type, const uint8_t vm_version, const char** code, size_t* size, uint32_t* first_block_used);

    virtual void* get_eos_vm_interface();
    virtual void eos_vm_interface_apply(const digest_type& code_hash, const uint8_t vm_type, const uint8_t vm_version, eosio::chain::apply_context& context);
    virtual void eos_vm_micropython_apply(const digest_type& code_hash, const uint8_t vm_type, const uint8_t vm_version, eosio::chain::apply_context& context);

public:
    controller::config conf;

private:
    controller &c;
    void *eos_vm_interface;
    void *eos_vm_micropython;

};

} } //eosio::chain
