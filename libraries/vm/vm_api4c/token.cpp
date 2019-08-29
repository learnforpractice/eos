static void token_create(u64 issuer, u64 maximum_supply, u64 symbol) {
    get_vm_api()->token_create(issuer, maximum_supply, symbol);
}

static void token_issue(u64 to, u64 quantity, u64 symbol, u32 memo_offset, u32 memo_size) {
    char *memo = (char *)offset_to_ptr(memo_offset, memo_size);
    get_vm_api()->token_issue(to, quantity, symbol, memo, memo_size);
}

static void token_transfer(u64 from, u64 to, u64 quantity, u64 symbol, u32 memo_offset, u32 size2) {
    char *memo = (char *)offset_to_ptr(memo_offset, size2);
    get_vm_api()->token_transfer(from, to, quantity, symbol, memo, size2);
}
