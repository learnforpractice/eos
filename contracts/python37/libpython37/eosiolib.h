#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>

#ifndef __EOSIOLIB_H_
#define __EOSIOLIB_H_

__attribute__((eosio_wasm_import))
void prints( const char* cstr );

__attribute__((eosio_wasm_import))
void eosio_assert(int cond, const char *msg);

__attribute__((eosio_wasm_import))
uint64_t current_time(void);

__attribute__((eosio_wasm_import))
uint64_t s2n(const char *str, int len);

#endif
