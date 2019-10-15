#include "mi_internal.hpp"
using namespace std;
using namespace eosio;
using namespace _multi_index_detail;


enum index_type {
    idx64,
    idx128,
    idx256,
    idx_double,
    idx_long_double,
};

struct primary_itr {
    int itr;
    uint64_t primary_key;
    primary_itr(int itr, int primary_key) : itr(itr), primary_key(primary_key)
    {

    }
};

class multi_index {
public:
    multi_index(uint64_t code, uint64_t scope, uint64_t table, vector<index_type>& indexes);
    void store(uint64_t primary_key, void *data, uint32_t data_size, vector<vector<char>>& secondary_values, uint64_t payer);
    void modify(int itr, uint64_t primary_key, void *data, uint32_t data_size, vector<vector<char>>& secondary_values, uint64_t payer);
    void erase(int itr, uint64_t primary_key);

    int find(uint64_t primary_key);
    bool get(int itr, vector<char>& value);
    bool get_by_primary_key(uint64_t primary_key, vector<char>& value);
    

    int lowerbound(uint64_t code, uint64_t scope, uint64_t table, uint64_t primary_key);
    int upperbound(uint64_t code, uint64_t scope, uint64_t table, uint64_t primary_key);

    int find_secondary(int secondary_index, void *key, uint32_t key_size);
    int lowerbound_secondary(int secondary_index, uint64_t code, uint64_t scope, uint64_t table, void* secondary, uint32_t secondary_size, uint64_t primary_key);
    int upperbound_secondary(int secondary_index, uint64_t code, uint64_t scope, uint64_t table, void* secondary, uint32_t secondary_size, uint64_t primary_key);
    bool get_by_secondary_key(int secondary_index, void *key, uint32_t key_size, vector<char>& value);

private:
    void add_index(index_type type);
    int get_secondary_key_size(int idx);
    uint64_t get_secondary_idx_table(int idx);
    secondary_index_db_functions& get_secondary_idx(int idx);

private:
    vector<secondary_index_db_functions*> secondary_indexes;
    vector<vector<char>> secondary_values;
    int32_t indexes[16];
    uint64_t code;
    uint64_t scope;
    uint64_t table;
};
