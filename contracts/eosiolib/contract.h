#ifndef EOSIOLIB_CONTRACT_H_
#define EOSIOLIB_CONTRACT_H_

#include <eosiolib/types.h>
#ifdef __cplusplus
extern "C" {
#endif

__attribute__((eosio_wasm_import))
int get_code_hash(uint64_t account, char *hash, size_t size);

#ifdef __cplusplus
}
#endif

#endif //__CONTRACT_H_
///@} databasec

