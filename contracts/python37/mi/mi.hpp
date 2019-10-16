#include "mi_internal.hpp"
#include "mi.h"

using namespace std;
using namespace eosio;
using namespace _multi_index_detail;

struct primary_itr {
    int itr;
    uint64_t primary_key;
    primary_itr(int itr, int primary_key) : itr(itr), primary_key(primary_key)
    {

    }
};

class multi_index {
public:
    multi_index(uint64_t code, uint64_t scope, uint64_t table, vector<enum index_type>& indexes);
    multi_index(uint64_t code, uint64_t scope, uint64_t table, int* indexes, uint32_t size);
    void store(uint64_t primary_key, const void *data, uint32_t data_size, struct vm_buffer *secondary_values, uint32_t secondary_values_size, uint64_t payer);
    void modify(int itr, uint64_t primary_key, const void *data, uint32_t data_size, struct vm_buffer *secondary_values, uint32_t secondary_values_size, uint64_t payer);
    void erase(int itr, uint64_t primary_key);

    int find(uint64_t primary_key);
    int get(int itr, struct vm_buffer *vb);

    int32_t next(int32_t itr, uint64_t& primary_key);
    int32_t previous(int32_t itr, uint64_t& primary_key);

    int lowerbound(uint64_t code, uint64_t scope, uint64_t table, uint64_t primary_key);
    int upperbound(uint64_t code, uint64_t scope, uint64_t table, uint64_t primary_key);


    int idx_find(int secondary_index, uint64_t& primary_key, const void *key, uint32_t key_size);
    void idx_update(int secondary_index, int32_t iterator, const void *secondary, uint32_t size, uint64_t payer );
    bool get_by_secondary_key(int secondary_index, const void *secondary_key, uint32_t secondary_key_size, struct vm_buffer *vb);

    int idx_next(int secondary_index, int itr_secondary, uint64_t& primary_key);
    int idx_previous(int secondary_index, int itr_secondary, uint64_t& primary_key);

    int idx_lowerbound(int secondary_index, uint64_t code, uint64_t scope, uint64_t table, void *secondary, uint32_t secondary_size, uint64_t& primary_key);
    int idx_upperbound(int secondary_index, uint64_t code, uint64_t scope, uint64_t table, void *secondary, uint32_t secondary_size, uint64_t& primary_key);
    int get_indexes_count();
    int get_indexes(int *idxes, uint32_t size);
private:
    void add_index(enum index_type type);
    int get_secondary_key_size(int idx);
    uint64_t get_secondary_idx_table(int idx);
    secondary_index_db_functions* get_secondary_idx(int idx);

private:
    vector<secondary_index_db_functions*> secondary_indexes;
    vector<vector<char>> secondary_values;
    int32_t indexes[16];
    uint64_t code;
    uint64_t scope;
    uint64_t table;
};
