#include "mi.hpp"

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
    multi_index(uint64_t code, uint64_t scope, uint64_t table, vector<index_type>& indexes): 
    code(code),
    scope(scope),
    table(table)
    {
        check(indexes.size() <= 12, "multi_index does not support table names with a length greater than 12");
        for (int i=0;i<indexes.size();i++) {
            auto& idx = indexes[i];
            indexes[i] = idx;
            add_index(idx);
        }
    }

    void store(uint64_t primary_key, void *data, uint32_t data_size, vector<vector<char>>& secondary_values, uint64_t payer) {
        check(secondary_values.size() == secondary_indexes.size(), "bad secondary value count");
        uint32_t i = 0;
        for (auto& idx: secondary_indexes) {
            auto value = secondary_values[i];
            idx->db_idx_store(scope, (table & 0xFFFFFFFFFFFFFFF0ULL) | ((uint64_t)i & 0x000000000000000FULL), primary_key, value.data(), value.size(), payer);
            i += 1;
        }
    }

    void modify(int itr, uint64_t primary_key, void *data, uint32_t data_size, vector<vector<char>>& secondary_values, uint64_t payer) {
        check(secondary_values.size() == secondary_indexes.size(), "bad secondary value count");
        uint32_t i = 0;
        char temp_buffer[sizeof(uint128_t)*2];
        for (int i=0;i<secondary_indexes.size();i++) {
            auto& idx = *secondary_indexes[i];
            auto& value = secondary_values[i];
            uint64_t idx_table = get_secondary_idx_table(i);
            int secondary_key_size = get_secondary_key_size(i);
            check(secondary_key_size == value.size(), "bad secondary value size");
            itr = idx.db_idx_find_primary(code, scope, idx_table, primary_key, temp_buffer, secondary_key_size);
            idx.db_idx_update(itr, value.data(), value.size(), payer);
//            idx->db_idx_store(scope, idx_table, primary_key, value.data(), value.size(), payer);
        }
    }

    bool find_primary_key(int itr, bool erase, uint64_t& primary_key) {
        for (int j=0;j<itr_buffer.size();j++) {
            auto& it = itr_buffer[j];
            if (it.itr == itr) {
                primary_key = it.primary_key;
                if (erase) {
                    itr_buffer.erase (itr_buffer.begin()+j);
                }
                return true;
            }
        }
        return false;
    }

    int find(uint64_t primary_key) {
        int itr = internal_use_do_not_use::db_find_i64(code, scope, table, primary_key);
        if (itr >= 0) {
            itr_buffer.emplace_back(itr, primary_key);
        }
        return itr;
    }
    
    bool get(int itr, vector<char>& value) {
        if (itr < 0) {
            return false;
        }
        auto size = internal_use_do_not_use::db_get_i64( itr, nullptr, 0 );
//        check( size >= 0, "error reading iterator" );
        value.resize(size);
        internal_use_do_not_use::db_get_i64(itr, value.data(), value.size());
        return true;
    }

    bool get_by_primary_key(uint64_t primary_key, vector<char>& value) {
        auto itr = find(primary_key);
        if (itr < 0) {
            return false;
        }
        return get(itr, value);
    }

    void erase(int itr) {
        uint64_t primary_key;
        bool found = find_primary_key(itr, true, primary_key);
        if (!found) {
            return;
        }
        internal_use_do_not_use::db_remove_i64(itr);
        char temp_buffer[sizeof(uint128_t)*2];
        for (int i=0;i<secondary_indexes.size();i++) {
            auto& idx = *secondary_indexes[i];
            int secondary_size = get_secondary_key_size(i);
            uint64_t idx_table = get_secondary_idx_table(i);
            int itr_idx = idx.db_idx_find_primary(code, scope, idx_table, primary_key, temp_buffer, secondary_size);
            idx.db_idx_remove(itr_idx);
        }
    }

    int lowerbound(uint64_t code, uint64_t scope, uint64_t table, uint64_t primary_key) {
        return internal_use_do_not_use::db_lowerbound_i64(code, scope, table, primary_key);
    }

    int upperbound(uint64_t code, uint64_t scope, uint64_t table, uint64_t primary_key) {
        return internal_use_do_not_use::db_upperbound_i64(code, scope, table, primary_key);
    }

    int find_secondary(int secondary_index, void *key, uint32_t key_size) {
        check(secondary_index<secondary_indexes.size(), "bad secondary index");
        auto& idx = *secondary_indexes[secondary_index];
        uint64_t idx_table = get_secondary_idx_table(secondary_index);
        uint64_t primary_key;
//        int itr = idx.db_idx_lowerbound(code, scope, idx_table, key, key_size, &primary_key);
        return idx.db_idx_find_secondary(code, scope, idx_table, key, key_size, primary_key);
    }

    bool get_by_secondary_key(int secondary_index, void *key, uint32_t key_size, vector<char>& value) {
        check(secondary_index<secondary_indexes.size(), "bad secondary index");
        auto& idx = *secondary_indexes[secondary_index];
        uint64_t idx_table = get_secondary_idx_table(secondary_index);
        uint64_t primary_key;
//        int itr = idx.db_idx_lowerbound(code, scope, idx_table, key, key_size, &primary_key);
        int itr = idx.db_idx_find_secondary(code, scope, idx_table, key, key_size, primary_key);
        if (itr < 0) {
            return false;
        }
        itr = internal_use_do_not_use::db_find_i64(code, scope, table, primary_key);
        if (itr < 0) {
            return false;
        }
        int size = internal_use_do_not_use::db_get_i64(itr, nullptr, 0);
        value.resize(size);
        internal_use_do_not_use::db_get_i64(itr, value.data(), size);
        return true;
    }

    int lowerbound_secondary(int secondary_index, uint64_t code, uint64_t scope, uint64_t table, void* secondary, uint32_t secondary_size, uint64_t primary_key) {
        auto& idx = *secondary_indexes[secondary_index];
        uint64_t idx_table = get_secondary_idx_table(secondary_index);

        return idx.db_idx_lowerbound(code, scope, idx_table, secondary, secondary_size, primary_key);
    }

    int upperbound_secondary(int secondary_index, uint64_t code, uint64_t scope, uint64_t table, void* secondary, uint32_t secondary_size, uint64_t primary_key) {
        auto& idx = *secondary_indexes[secondary_index];
        uint64_t idx_table = get_secondary_idx_table(secondary_index);
        return idx.db_idx_upperbound(code, scope, idx_table, secondary, secondary_size, primary_key);
    }

    void add_index(index_type type) {
        if (type == idx64) {
            secondary_indexes.push_back(new secondary_index_db_functions_idx64());
        } else if (type == idx128) {
            secondary_indexes.push_back(new secondary_index_db_functions_idx128());
        } else if (type == idx256) {
            secondary_indexes.push_back(new secondary_index_db_functions_idx256());
        } else if (type == idx_double) {
            secondary_indexes.push_back(new secondary_index_db_functions_idx_double());
        } else if (type == idx_long_double) {
            secondary_indexes.push_back(new secondary_index_db_functions_idx_long_double());
        } else {
            check(false, "unknow index type");
        }
    }

    int get_secondary_key_size(int idx) {
        switch(indexes[idx]) {
            case idx64:
                return sizeof(uint64_t);
            case idx128:
                return sizeof(uint128_t);
            case idx256:
                return sizeof(uint128_t)*2;
            case idx_double:
                return sizeof(double);
            case idx_long_double:
                return sizeof(long double);
            default:
                check(false, "unknow secondary key");
                return 0;
        }
    }

    uint64_t get_secondary_idx_table(int idx) {
        return (table & 0xFFFFFFFFFFFFFFF0ULL) | ((uint64_t)idx & 0x000000000000000FULL);
    }

    secondary_index_db_functions& get_secondary_idx(int idx) {
        check(idx<secondary_indexes.size() && idx>=0, "bad secondary idx");
        return *secondary_indexes[idx];
    }

  private:
      vector<secondary_index_db_functions*> secondary_indexes;
      vector<vector<char>> secondary_values;
      vector<primary_itr> itr_buffer;
      int32_t indexes[16];
      uint64_t code;
      uint64_t scope;
      uint64_t table;
};

}