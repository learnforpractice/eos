void set_resource_limits(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void get_resource_limits(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void set_proposed_producers(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void set_proposed_producers_ex(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void is_privileged(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void set_privileged(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void set_blockchain_parameters_packed(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void get_blockchain_parameters_packed(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void activate_feature(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void eosio_abort(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void eosio_assert(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void eosio_assert_message(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void eosio_assert_code(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void eosio_exit(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void current_time(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void now(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void checktime(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void check_context_free(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void send_deferred(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void cancel_deferred(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void read_transaction(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void transaction_size(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void tapos_block_num(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void tapos_block_prefix(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void expiration(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{

}
void get_action(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void assert_privileged(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void assert_context_free(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void get_context_free_data(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void token_create(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void token_issue(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void token_transfer(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void token_open(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void token_retire(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void token_close(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void call_contract_get_args(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void call_contract_set_results(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void call_contract_get_results(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int ret = get_vm_api()->call_contract_get_results((char *)args[0].ptr, args[0].size);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = ret;
}
void call_contract(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret) {
    get_vm_api()->call_contract(args[0].u64, (char *)args[1].ptr, args[1].size);
}

void is_feature_activated(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void preactivate_feature(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void get_sender(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}
void call_native(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{}