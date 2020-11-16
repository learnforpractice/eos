static void assert_sha256(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    get_vm_api()->assert_sha256((char *)args[0].ptr, args[0].size, (checksum256 *)args[1].ptr);
}

static void assert_sha1(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    get_vm_api()->assert_sha1((char *)args[0].ptr, args[0].size, (checksum160 *)args[1].ptr);
}

static void assert_sha512(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    get_vm_api()->assert_sha512((char *)args[0].ptr, args[0].size, (checksum512 *)args[1].ptr);
}

static void assert_ripemd160(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    get_vm_api()->assert_ripemd160((char *)args[0].ptr, args[0].size, (checksum160 *)args[1].ptr);
}

static void assert_recover_key(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    get_vm_api()->assert_recover_key((checksum256 *)args[0].ptr, (char *)args[1].ptr, args[1].size, (char *)args[2].ptr, args[2].size);
}

static void sha256(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    // if (args[1].size < 32) {
    //     return 0;
    // }
    get_vm_api()->sha256((char *)args[0].ptr, args[0].size, (checksum256 *)args[1].ptr);            
}

static void sha1(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    // if (args[1].size < 20) {
    //     return 0;
    // }
    get_vm_api()->sha1((char *)args[0].ptr, args[0].size, (checksum160 *)args[1].ptr);
}

static void sha512(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    // if (args[1].size < 64) {
    //     return 0;
    // }
    get_vm_api()->sha512((char *)args[0].ptr, args[0].size, (checksum512 *)args[1].ptr);
}

static void ripemd160(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    get_vm_api()->ripemd160((char *)args[0].ptr, args[0].size, (checksum160 *)args[1].ptr);
}

static void recover_key(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int ret = get_vm_api()->recover_key((checksum256*)args[0].ptr, (char *)args[1].ptr, args[1].size, (char *)args[2].ptr, args[2].size);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = ret;
}

static void sha3(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    get_vm_api()->sha3((char *)args[0].ptr, args[0].size, (char *)args[1].ptr, args[1].size);
}
