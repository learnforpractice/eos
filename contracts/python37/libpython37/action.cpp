#include <eosiolib/action.hpp>
#include <eosiolib/transaction.hpp>

/*
   struct action {
      account_name               account;
      action_name                name;
      vector<permission_level>   authorization;
      bytes                      data;
*/

extern "C" void pythonvm_send_inline(uint64_t account, uint64_t action_name, uint64_t actor, uint64_t permission, char *data, size_t len)
{
    eosio::permission_level per{actor, permission};
    eosio::action a;
    a.account = account;
    a.name = action_name;
    a.authorization.emplace_back(per);
    a.data.resize(len);
    memcpy(a.data.data(), data, len);

    auto serialize = eosio::pack(a);
    send_inline(serialize.data(), serialize.size());
}

extern "C" void* pythonvm_action_new(uint64_t account, 
                                    uint64_t act_name, 
                                    uint64_t actor,
                                    uint64_t permission,
                                    const char *data,
                                    size_t len) {
    eosio::action *a = (eosio::action *)malloc(sizeof(eosio::action));
    eosio::permission_level per{actor, permission};
    a->account = account;
    a->name = act_name; 
    a->authorization.push_back(per);
    a->data.resize(len);
    memcpy(a->data.data(), data, len);
    return a;
}

extern "C" void pythonvm_action_add_permission(void *_a, uint64_t actor, uint64_t permission) {
    eosio::action *a = (eosio::action *)_a;
    eosio::permission_level per{actor, permission};
    a->authorization.push_back(per);
}

extern "C" void pythonvm_send_inline2(void *_a)
{
    eosio::action *a = (eosio::action *)_a;
    auto serialize = eosio::pack(*a);
    send_inline(serialize.data(), serialize.size());
}

extern "C" void pythonvm_action_free(void *a) {
    free(a);
}

#if 0
      time_point_sec         expiration;                ///< the time at which a transaction expires
      uint16_t               ref_block_num       = 0U;  ///< specifies a block num in the last 2^16 blocks.
      uint32_t               ref_block_prefix    = 0UL; ///< specifies the lower 32 bits of the blockid at get_ref_blocknum
      unsigned_int           max_net_usage_words = 0UL; /// upper limit on total network bandwidth (in 8 byte words) billed for this transaction
      uint8_t                max_cpu_usage_ms    = 0;   /// upper limit on the total CPU time billed for this transaction
      unsigned_int           delay_sec           = 0UL; /// number of seconds to delay this transaction for during which it may be canceled.
#endif

extern "C" void* pythonvm_transaction_new(uint32_t expiration, 
                                    uint32_t ref_block_num, 
                                    uint32_t ref_block_prefix, 
                                    uint32_t max_net_usage_words, 
                                    uint32_t max_cpu_usage_ms, 
                                    uint32_t delay_sec) {
    eosio::transaction *t = (eosio::transaction *)malloc(sizeof(eosio::transaction));

    t->expiration = eosio::time_point_sec(expiration);
    t->ref_block_num = (uint16_t)ref_block_num; 
    t->ref_block_prefix = ref_block_prefix; 
    t->max_net_usage_words = max_net_usage_words; 
    t->max_cpu_usage_ms = (uint8_t)max_cpu_usage_ms; 
    t->delay_sec = delay_sec;
    return t;
}

extern "C" {

void pythonvm_transaction_add_action(void *_t, void *_a) {
    eosio::transaction *t = (eosio::transaction *)_t;
    t->actions.push_back(*(eosio::action*)_a);
}

void pythonvm_transaction_add_context_free_action(void *_t, void *_a) {
    eosio::transaction *t = (eosio::transaction *)_t;
    t->context_free_actions.push_back(*(eosio::action*)_a);
}

void pythonvm_transaction_send(void *t, uint128_t sender_id, uint64_t payer, bool replace_existing) {
    auto serialized = pack(*(eosio::transaction*)t);
    ::send_deferred(sender_id, payer, serialized.data(), serialized.size(), replace_existing);
}

void pythonvm_transaction_free(void *t) {
    free(t);
}

}
