#define DB_SECONDARY_INDEX_METHODS_SIMPLE(IDX, IDX_TYPE) \
    void db_##IDX##_store(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)                \
    {                                                  \
        uint64_t scope =args[0].u64;   \
        uint64_t table = args[1].u64;    \
        uint64_t payer = args[2].u64; \
        uint64_t id = args[3].u64; \
        int32_t itr = get_vm_api()->db_##IDX##_store(scope, table, payer, id, (IDX_TYPE *)args[4].ptr ); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    }                                          \
    void db_##IDX##_update(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)               \
    { \
        get_vm_api()->db_##IDX##_update(args[0].i32, args[1].u64, (IDX_TYPE *)args[2].ptr); \
    } \
\
    void db_##IDX##_remove(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)               \
    { \
        get_vm_api()->db_##IDX##_remove(args[0].i32); \
    } \
    void db_##IDX##_next(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)                 \
    { \
        int32_t itr = get_vm_api()->db_##IDX##_next(args[0].i32, (uint64_t *)args[1].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
    void db_##IDX##_previous(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)             \
    { \
        int32_t itr = get_vm_api()->db_##IDX##_previous(args[0].i32, (uint64_t *)args[1].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
    void db_##IDX##_find_primary(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)         \
    { \
        uint64_t code = args[0].u64;   \
        uint64_t scope = args[1].u64;    \
        uint64_t table = args[2].u64; \
        uint64_t primary = args[4].u64; \
        int32_t itr = get_vm_api()->db_##IDX##_find_primary(code, scope, table, (IDX_TYPE *)args[3].ptr, primary); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
    void db_##IDX##_find_secondary(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)       \
    { \
        uint64_t code = args[0].u64;   \
        uint64_t scope = args[1].u64;    \
        uint64_t table = args[2].u64; \
        int32_t itr = get_vm_api()->db_##IDX##_find_secondary(code, scope, table, (IDX_TYPE *)args[3].ptr, (uint64_t *)args[4].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
    void db_##IDX##_lowerbound(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)           \
    { \
        uint64_t code = args[0].u64;   \
        uint64_t scope = args[1].u64;    \
        uint64_t table = args[2].u64; \
        int32_t itr = get_vm_api()->db_##IDX##_lowerbound(code, scope, table, (IDX_TYPE *)args[3].ptr, (uint64_t *)args[4].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
    void db_##IDX##_upperbound(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)           \
    { \
        uint64_t code = args[0].u64;   \
        uint64_t scope = args[1].u64;    \
        uint64_t table = args[2].u64; \
        int32_t itr = get_vm_api()->db_##IDX##_upperbound(code, scope, table, (IDX_TYPE *)args[3].ptr, (uint64_t *)args[4].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
    void db_##IDX##_end(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)                  \
    { \
        uint64_t code = args[0].u64;   \
        uint64_t scope = args[1].u64;    \
        uint64_t table = args[2].u64; \
        int32_t itr = get_vm_api()->db_##IDX##_end(code, scope, table); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    }


#define DB_SECONDARY_INDEX_METHODS_ARRAY(IDX, ARR_TYPE, ARR_SIZE) \
    void db_##IDX##_store(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)                \
    {                                                  \
        uint64_t scope = args[0].u64;   \
        uint64_t table = args[1].u64;    \
        uint64_t payer = args[2].u64; \
        uint64_t id = args[3].u64; \
        int32_t itr = get_vm_api()->db_##IDX##_store(scope, table, payer, id, (ARR_TYPE *)args[4].ptr, 2); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    }                                          \
    void db_##IDX##_update(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)               \
    { \
        get_vm_api()->db_##IDX##_update(args[0].i32, args[1].u64, args[2].ptr, 2); \
    } \
\
    void db_##IDX##_remove(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)               \
    { \
        get_vm_api()->db_##IDX##_remove(args[0].i32); \
    } \
    void db_##IDX##_next(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)                 \
    { \
        int32_t itr = get_vm_api()->db_##IDX##_next(args[0].i32, (uint64_t *)args[1].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
    void db_##IDX##_previous(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)             \
    { \
        int32_t itr = get_vm_api()->db_##IDX##_previous(args[0].i32, (uint64_t *)args[1].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
    void db_##IDX##_find_primary(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)         \
    { \
        uint64_t code = args[0].u64;   \
        uint64_t scope = args[1].u64;    \
        uint64_t table = args[2].u64; \
        uint64_t primary = args[4].u64; \
        int32_t itr = get_vm_api()->db_##IDX##_find_primary(code, scope, table, (ARR_TYPE *)args[3].ptr, 2, primary); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
    void db_##IDX##_find_secondary(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)       \
    { \
        uint64_t code = args[0].u64;   \
        uint64_t scope = args[1].u64;    \
        uint64_t table = args[2].u64; \
        int32_t itr = get_vm_api()->db_##IDX##_find_secondary(code, scope, table, (ARR_TYPE *)args[3].ptr, 2, (uint64_t *)args[4].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
    void db_##IDX##_lowerbound(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)           \
    { \
        uint64_t code = args[0].u64;   \
        uint64_t scope = args[1].u64;    \
        uint64_t table = args[2].u64; \
        int32_t itr = get_vm_api()->db_##IDX##_lowerbound(code, scope, table, (ARR_TYPE *)args[3].ptr, 2, (uint64_t *)args[4].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
    void db_##IDX##_upperbound(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)           \
    { \
        uint64_t code = args[0].u64;   \
        uint64_t scope = args[1].u64;    \
        uint64_t table = args[2].u64; \
        int32_t itr = get_vm_api()->db_##IDX##_upperbound(code, scope, table, (ARR_TYPE *)args[3].ptr, 2, (uint64_t *)args[4].ptr); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    } \
    void db_##IDX##_end(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)                  \
    { \
        uint64_t code = args[0].u64;   \
        uint64_t scope = args[1].u64;    \
        uint64_t table = args[2].u64; \
        int32_t itr = get_vm_api()->db_##IDX##_end(code, scope, table); \
        vm_ret->size = 4; \
        vm_ret->type = enum_arg_type_i32; \
        vm_ret->i32 = itr; \
    }


static void db_store_i64(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    uint64_t scope = args[0].u64;
    uint64_t table = args[1].u64;
    uint64_t payer = args[2].u64;
    uint64_t id = args[3].u64;
    int32_t itr = get_vm_api()->db_store_i64(scope, table, payer, id, (char *)args[4].ptr, args[4].size);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = itr;
}

static void db_update_i64(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t iterator = args[0].i32;
    uint64_t payer = args[1].u64;
    get_vm_api()->db_update_i64(iterator, payer, (char *)args[2].ptr, args[2].size);
}

static void db_remove_i64(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    get_vm_api()->db_remove_i64(args[0].i32);
}

static void db_get_i64(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t itr = args[0].i32;
    int32_t size = get_vm_api()->db_get_i64(itr, (char *)args[1].ptr, args[1].size);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = size;
}

static void db_next_i64(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t itr = args[0].i32;
    itr = get_vm_api()->db_next_i64(itr, (uint64_t *)args[1].ptr);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = itr;
}

static void db_previous_i64(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t itr = args[0].i32;
    itr = get_vm_api()->db_previous_i64(itr, (uint64_t *)args[1].ptr);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = itr;
}

static void db_find_i64(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t itr = get_vm_api()->db_find_i64(args[0].u64, args[1].u64, args[2].u64, args[3].u64);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = itr;
}

static void db_lowerbound_i64(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t itr = get_vm_api()->db_lowerbound_i64(args[0].u64, args[1].u64, args[2].u64, args[3].u64);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = itr;
}

static void db_upperbound_i64(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t itr = get_vm_api()->db_upperbound_i64(args[0].u64, args[1].u64, args[2].u64, args[3].u64);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = itr;
}

static void db_end_i64(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t itr = get_vm_api()->db_end_i64(args[0].u64, args[1].u64, args[2].u64);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = itr;
}

static void db_store_i256(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    uint64_t scope = args[0].u64;
    uint64_t table = args[1].u64;
    uint64_t payer = args[2].u64;
    int32_t itr = get_vm_api()->db_store_i256(scope, table, payer, args[3].ptr, args[3].size, (char *)args[4].ptr, args[4].size);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = itr;
}

static void db_update_i256(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t iterator = args[0].i32;
    uint64_t payer = args[1].u64;
    get_vm_api()->db_update_i256(iterator, payer, (char *)args[2].ptr, args[2].size);
}

static void db_remove_i256(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    get_vm_api()->db_remove_i256(args[0].i32);
}

static void db_get_i256(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t itr = args[0].i32;
    int32_t size = get_vm_api()->db_get_i256(itr, (char *)args[1].ptr, args[1].size);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = size;
}

static void db_next_i256(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t itr = args[0].i32;
    itr = get_vm_api()->db_next_i256(itr, args[1].ptr, args[1].size);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = itr;
}

static void db_previous_i256(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t itr = args[0].i32;
    itr = get_vm_api()->db_previous_i256(itr, args[1].ptr, args[1].size);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = itr;
}

static void db_find_i256(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t itr = get_vm_api()->db_find_i256(args[0].u64, args[1].u64, args[2].u64, args[3].ptr, args[3].size);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = itr;
}

static void db_lowerbound_i256(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t itr = get_vm_api()->db_lowerbound_i256(args[0].u64, args[1].u64, args[2].u64, args[3].ptr, args[3].size);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = itr;
}

static void db_upperbound_i256(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t itr = get_vm_api()->db_upperbound_i256(args[0].u64, args[1].u64, args[2].u64, args[3].ptr, args[3].size);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = itr;
}

static void db_end_i256(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    int32_t itr = get_vm_api()->db_end_i256(args[0].u64, args[1].u64, args[2].u64);
    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_i32;
    vm_ret->i32 = itr;
}

static void db_get_table_count(vm_api_arg *args, size_t args_count, vm_api_arg *vm_ret)
{
    uint32_t n = get_vm_api()->db_get_table_count(args[0].u64, args[1].u64, args[2].u64);

    vm_ret->size = 4;
    vm_ret->type = enum_arg_type_u32;
    vm_ret->u32 = n;
}

DB_SECONDARY_INDEX_METHODS_SIMPLE(idx64, uint64_t)
DB_SECONDARY_INDEX_METHODS_SIMPLE(idx128, __uint128)
DB_SECONDARY_INDEX_METHODS_ARRAY(idx256, __uint128, 2)
DB_SECONDARY_INDEX_METHODS_SIMPLE(idx_double, float64_t)
DB_SECONDARY_INDEX_METHODS_SIMPLE(idx_long_double, float128_t)
