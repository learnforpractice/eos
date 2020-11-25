extern "C" {
/* import: 'env' 'token_issue' */
void (*Z_envZ_token_issueZ_vjjjii)(u64, u64, u64, u32, u32);
/* import: 'env' 'token_transfer' */
void (*Z_envZ_token_transferZ_vjjjjii)(u64, u64, u64, u64, u32, u32);
/* import: 'env' 'token_open' */
void (*Z_envZ_token_openZ_vjjj)(u64, u64, u64);
/* import: 'env' 'token_retire' */
void (*Z_envZ_token_retireZ_vjjii)(u64, u64, u32, u32);
/* import: 'env' 'token_close' */
void (*Z_envZ_token_closeZ_vjj)(u64, u64);
}

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


static void token_open(u64 owner, u64 symbol, u64 ram_payer ) {
    get_vm_api()->token_open(owner, symbol, ram_payer);
}

static void token_retire( u64 amount, u64 symbol, u32 memo_offset, u32 memo_size ) {
    char *memo = (char *)offset_to_ptr(memo_offset, memo_size);
    get_vm_api()->token_retire(amount, symbol, memo, memo_size);
}

static void token_close( u64 owner, u64 symbol ) {
    get_vm_api()->token_close(owner, symbol);
}
