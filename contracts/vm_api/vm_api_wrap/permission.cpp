void check_transaction_authorization(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t ret = get_vm_api()->check_transaction_authorization(
                        (char *)args[0].ptr, args[0].size,
                        (char *)args[1].ptr, args[1].size, 
                        (char *)args[2].ptr, args[2].size
    );
    vm_ret->type = enum_arg_type_i32;
    vm_ret->size = 4;
    vm_ret->i32 = ret;
}

void check_permission_authorization(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t ret = get_vm_api()->check_permission_authorization(
        args[0].u64,
        args[1].u64,
        (char *)args[2].ptr, args[2].size,
        (char *)args[3].ptr, args[3].size,
        args[4].u64
    );
    vm_ret->type = enum_arg_type_i32;
    vm_ret->size = 4;
    vm_ret->i32 = ret;
}

void get_permission_last_used(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int64_t ret = get_vm_api()->get_permission_last_used(args[0].u64, args[1].u64);
    vm_ret->type = enum_arg_type_i64;
    vm_ret->size = 8;
    vm_ret->i64 = ret;
}

void get_account_creation_time(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int64_t ret = get_vm_api()->get_account_creation_time(args[0].u64);
    vm_ret->type = enum_arg_type_i64;
    vm_ret->size = 8;
    vm_ret->i64 = ret;
}
