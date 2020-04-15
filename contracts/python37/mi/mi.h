#include <stdint.h>
#ifndef _MI_H_
#define _MI_H_

#ifdef __cplusplus
extern "C"
{
#else
typedef char bool;
#endif

enum index_type {
    idx64,
    idx128,
    idx256,
    idx_double,
    idx_long_double,
};

struct vm_buffer {
    int size;
    int max_size;
    char *data;
};

#define MAX_INDEXES 12
#define MAX_SECONDARY_KEY_SIZE 32

void *mi_new(uint64_t code, uint64_t scope, uint64_t table, int * _indexes, uint32_t size);    
void mi_store(void *ptr, uint64_t primary_key, const void *data, uint32_t data_size, 
            struct vm_buffer *_secondary_values, uint32_t size, uint64_t payer);
void mi_modify(void *ptr, int32_t itr, uint64_t primary_key, const void *data, uint32_t data_size, 
            struct vm_buffer* _secondary_values, uint32_t size, uint64_t payer);
void mi_erase(void *ptr, int32_t itr, uint64_t primary_key);

int32_t mi_find(void *ptr, uint64_t primary_key);
int32_t mi_idx_find_primary(void *ptr, int secondary_index, uint64_t primary_key, struct vm_buffer *vb);

//user must take care of the buffer returned!
int mi_get(void *ptr, int32_t itr, struct vm_buffer *vb);
int mi_get_secondary_values(void *ptr, uint64_t primary_key, struct vm_buffer *vb, uint32_t vb_size);

int32_t mi_next(void *ptr, int32_t itr, uint64_t* primary_key);

int32_t mi_previous(void *ptr, int32_t itr, uint64_t* primary_key);

int32_t mi_end(void *ptr);

int32_t mi_lowerbound(void *ptr, uint64_t primary_key);

int32_t mi_upperbound(void *ptr, uint64_t primary_key);

int32_t mi_idx_find(void *ptr, int32_t secondary_index, uint64_t *primary_key, const void *key, uint32_t key_size);

void mi_idx_update(void *ptr, int32_t secondary_index, int32_t iterator, const void* secondary_key, uint32_t secondary_key_size, uint64_t payer );

//user must take care of the buffer returned!
bool mi_get_by_secondary_key(void *ptr, int32_t secondary_index, 
                                const void *secondary_key, uint32_t secondary_key_size, 
                                struct vm_buffer *vb);

int32_t mi_idx_next(void *ptr, int32_t secondary_index, int32_t itr_secondary, uint64_t *primary_key);

int32_t mi_idx_previous(void *ptr, int32_t secondary_index, int32_t itr_secondary, uint64_t *primary_key);

int32_t mi_idx_end(void *ptr, int secondary_index);


int32_t mi_idx_lowerbound(void *ptr, int32_t secondary_index, void *secondary, uint32_t secondary_size, uint64_t *primary_key);

int32_t mi_idx_upperbound(void *ptr, int32_t secondary_index, void *secondary, uint32_t secondary_size, uint64_t *primary_key);

int mi_get_indexes_count(void *ptr);
int mi_get_indexes(void *ptr, int *idxes, uint32_t size);

#ifdef __WASM
    #define WASM_IMPORT  __attribute__((eosio_wasm_import))
#else
    #define WASM_IMPORT  
#endif
WASM_IMPORT void prints( const char* cstr );
WASM_IMPORT void printui( uint64_t value );
WASM_IMPORT void printi( int64_t n);
WASM_IMPORT void eosio_exit(int code);


#ifdef __cplusplus
}
#endif

#endif