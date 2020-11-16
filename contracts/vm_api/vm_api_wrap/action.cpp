static void read_action_data(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret) {
    uint32_t size = get_vm_api()->read_action_data(args[0].ptr, args[0].size);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_u32;
    vm_ret->u32 = size;
}

static void action_data_size(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    uint32_t size = get_vm_api()->action_data_size();
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_u32;
    vm_ret->u32 = size;
}

static void require_recipient(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    // if (args_count != 1) {
    //     return 0;
    // }
    // if (args[0].size != sizeof(uint64_t)) {
    //     return 0;
    // }
    get_vm_api()->require_recipient(args[0].u64);
}

static void require_auth(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    // if (args_count != 1) {
    //     return 0;
    // }
    // if (args[0].size != sizeof(uint64_t)) {
    //     return 0;
    // }
    get_vm_api()->require_auth(args[0].u64);
}

static void require_auth2(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    // if (args_count != 2) {
    //     return 0;
    // }
    
    // if (args[0].size != sizeof(uint64_t)) {
    //     return 0;
    // }
    
    // if (args[1].size != sizeof(uint64_t)) {
    //     return 0;
    // }
    get_vm_api()->require_auth2(args[0].u64, args[1].u64);
}

static void has_auth(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret) {
    bool ret = false;
    ret = get_vm_api()->has_auth(args[0].u64);
    vm_ret->size = 1;
    vm_ret->u8 = ret;
    vm_ret->type = enum_arg_type_u8;
}

static void is_account(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    // if (args_count != 1) {
    //     return 0;
    // }
    
    // if (args[0].size != sizeof(uint64_t)) {
    //     return 0;
    // }
    
    bool ret = false;
    ret = get_vm_api()->is_account(args[0].u64);
    vm_ret->size = 1;
    vm_ret->u8 = ret;
    vm_ret->type = enum_arg_type_u8;
}

static void send_inline(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret) {
    // if (args_count != 1) {
    //     return 0;
    // }
    get_vm_api()->send_inline((char *)args[0].ptr, args[0].size);
}

static void send_context_free_inline(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    // if (args_count != 1) {
    //     return 0;
    // }
    get_vm_api()->send_context_free_inline((char *)args[0].ptr, args[0].size);
}

static void publication_time(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    uint64_t pub_time = get_vm_api()->publication_time();
    vm_ret->size = 8;
    vm_ret->type = enum_arg_type_u64;
    vm_ret->u64 = pub_time;
}

static void current_receiver(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    uint64_t receiver = get_vm_api()->current_receiver();
    vm_ret->size = 8;
    vm_ret->type = enum_arg_type_u64;
    vm_ret->u64 = receiver;
}
