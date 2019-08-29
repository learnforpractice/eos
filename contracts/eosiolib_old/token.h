#ifndef EOSIOLIB_TOKEN_H_
#define EOSIOLIB_TOKEN_H_

#include <eosiolib/types.h>
#ifdef __cplusplus
extern "C" {
#endif

WASM_IMPORT void token_create( uint64_t issuer, int64_t maximum_supply, uint64_t sym);
WASM_IMPORT void token_issue( uint64_t to, int64_t quantity, uint64_t sym, const char* memo, size_t size2 );
WASM_IMPORT void token_transfer( uint64_t from, uint64_t to, int64_t quantity, uint64_t sym, const char* memo, size_t size2);

#ifdef __cplusplus
}
#endif

#endif
