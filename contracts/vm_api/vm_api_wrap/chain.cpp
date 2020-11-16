static void get_active_producers(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    uint32_t ret = get_vm_api()->get_active_producers((uint64_t *)args[0].ptr, args[0].size);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = ret;
}
