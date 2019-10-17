#include "mi.hpp"
#include <mi.h>

multi_index::multi_index(uint64_t code, uint64_t scope, uint64_t table, vector<enum index_type>& _indexes): 
code(code),
scope(scope),
table(table)
{
    check(_indexes.size() <= 12, "multi_index does not support table names with a length greater than 12");
    for (int i=0;i<_indexes.size();i++) {
        auto idx = _indexes[i];
        indexes[i] = idx;
        add_index(idx);
    }
}

multi_index::multi_index(uint64_t code, uint64_t scope, uint64_t table, int* _indexes, uint32_t size):
code(code),
scope(scope),
table(table)
{
    check(size <= 12, "multi_index does not support table names with a length greater than 12");
    for (int i=0;i<size;i++) {
        auto idx = (enum index_type)_indexes[i];
        indexes[i] = idx;
        add_index(idx);
    }
}

void multi_index::store(uint64_t primary_key, const void *data, uint32_t data_size, struct vm_buffer *secondary_values, uint32_t secondary_values_size, uint64_t payer) {
    check(secondary_values_size == secondary_indexes.size(), "bad secondary value count");
    uint32_t i = 0;
    internal_use_do_not_use::db_store_i64(scope, table, payer, primary_key, data, data_size);
    for (int i=0;i<secondary_indexes.size();i++) {
        auto& value = secondary_values[i];
        uint64_t idx_table = get_secondary_idx_table(i);
        secondary_indexes[i]->db_idx_store(scope, idx_table, primary_key, value.data, value.size, payer);
    }
}

void multi_index::modify(int itr, uint64_t primary_key, const void *data, uint32_t data_size, struct vm_buffer *secondary_values, uint32_t secondary_values_size, uint64_t payer) {
    check(secondary_values_size == secondary_indexes.size(), "bad secondary value count");
    uint32_t i = 0;
    internal_use_do_not_use::db_update_i64(itr, payer, data, data_size);
    char temp_buffer[sizeof(uint128_t)*2];
    for (int i=0;i<secondary_indexes.size();i++) {
        auto idx = secondary_indexes[i];
        auto& value = secondary_values[i];
        uint64_t idx_table = get_secondary_idx_table(i);
        int secondary_key_size = get_secondary_key_size(i);
        check(secondary_key_size == value.size, "bad secondary value size");
        itr = idx->db_idx_find_primary(code, scope, idx_table, primary_key, temp_buffer, secondary_key_size);
        check(itr >= 0, "secondary value not found when calling modify");
        if (memcmp(temp_buffer, value.data, value.size) != 0) {
            idx->db_idx_update(itr, value.data, value.size, payer);
        }
//            idx->db_idx_store(scope, idx_table, primary_key, value.data(), value.size(), payer);
    }
}

void multi_index::erase(int itr, uint64_t primary_key) {
    internal_use_do_not_use::db_remove_i64(itr);
    char temp_buffer[sizeof(uint128_t)*2];
    for (int i=0;i<secondary_indexes.size();i++) {
        auto idx = secondary_indexes[i];
        int secondary_size = get_secondary_key_size(i);
        uint64_t idx_table = get_secondary_idx_table(i);
        int itr_idx = idx->db_idx_find_primary(code, scope, idx_table, primary_key, temp_buffer, secondary_size);
        idx->db_idx_remove(itr_idx);
    }
}

int multi_index::find(uint64_t primary_key) {
    int itr = internal_use_do_not_use::db_find_i64(code, scope, table, primary_key);
    return itr;
}

int multi_index::get(int itr, struct vm_buffer *vb) {
    int data_size = internal_use_do_not_use::db_get_i64( itr, nullptr, 0 );
    vb->size = data_size;
    vb->data = (char *)malloc(data_size);
    return internal_use_do_not_use::db_get_i64(itr, vb->data, data_size);
}

int32_t multi_index::next(int32_t itr, uint64_t& primary_key) {
    return internal_use_do_not_use::db_next_i64(itr, &primary_key);
}

int32_t multi_index::previous(int32_t itr, uint64_t& primary_key) {
    return internal_use_do_not_use::db_previous_i64(itr, &primary_key);
}

int32_t multi_index::end(uint64_t code, uint64_t scope, uint64_t table) {
    return internal_use_do_not_use::db_end_i64(code, scope, table);
}

int multi_index::lowerbound(uint64_t code, uint64_t scope, uint64_t table, uint64_t primary_key) {
    return internal_use_do_not_use::db_lowerbound_i64(code, scope, table, primary_key);
}

int multi_index::upperbound(uint64_t code, uint64_t scope, uint64_t table, uint64_t primary_key) {
    return internal_use_do_not_use::db_upperbound_i64(code, scope, table, primary_key);
}

int multi_index::idx_find(int secondary_index, uint64_t& primary_key, const void *key, uint32_t key_size) {
    auto idx = get_secondary_idx(secondary_index);
    uint64_t idx_table = get_secondary_idx_table(secondary_index);
//        int itr = idx.db_idx_lowerbound(code, scope, idx_table, key, key_size, &primary_key);
    return idx->db_idx_find_secondary(code, scope, idx_table, key, key_size, primary_key);
}

void multi_index::idx_update(int secondary_index, int32_t iterator, const void *secondary, uint32_t size, uint64_t payer ) {
    auto idx = get_secondary_idx(secondary_index);
    idx->db_idx_update(iterator, secondary, size, payer);
}

int multi_index::idx_next(int secondary_index, int itr_secondary, uint64_t& primary_key) {
    check(secondary_index<secondary_indexes.size(), "bad secondary index");
    auto idx = secondary_indexes[secondary_index];
    return idx->db_idx_next(itr_secondary, &primary_key);
}

int multi_index::idx_previous(int secondary_index, int itr_secondary, uint64_t& primary_key) {
    auto idx = secondary_indexes[secondary_index];
    return idx->db_idx_previous(itr_secondary, &primary_key);
}

int32_t multi_index::idx_end(int secondary_index, uint64_t code, uint64_t scope, uint64_t table) {
    auto idx = secondary_indexes[secondary_index];
    return idx->db_idx_end(code, scope, table);
}

int multi_index::idx_lowerbound(int secondary_index, uint64_t code, uint64_t scope, uint64_t table, void *secondary, uint32_t secondary_size, uint64_t& primary_key) {
    auto idx = secondary_indexes[secondary_index];
    uint64_t idx_table = get_secondary_idx_table(secondary_index);

    return idx->db_idx_lowerbound(code, scope, idx_table, secondary, secondary_size, primary_key);
}

int multi_index::idx_upperbound(int secondary_index, uint64_t code, uint64_t scope, uint64_t table, void *secondary, uint32_t secondary_size, uint64_t& primary_key) {
    auto idx = secondary_indexes[secondary_index];
    uint64_t idx_table = get_secondary_idx_table(secondary_index);
    return idx->db_idx_upperbound(code, scope, idx_table, secondary, secondary_size, primary_key);
}

bool multi_index::get_by_secondary_key(int secondary_index, const void *secondary_key, uint32_t secondry_key_size, vm_buffer *vb) {
    check(secondary_index<secondary_indexes.size(), "bad secondary index");
    auto idx = secondary_indexes[secondary_index];
    uint64_t idx_table = get_secondary_idx_table(secondary_index);
    uint64_t primary_key;
//        int itr = idx.db_idx_lowerbound(code, scope, idx_table, key, key_size, &primary_key);
    int itr = idx->db_idx_find_secondary(code, scope, idx_table, secondary_key, secondry_key_size, primary_key);
    if (itr < 0) {
        return false;
    }
    itr = internal_use_do_not_use::db_find_i64(code, scope, table, primary_key);
    if (itr < 0) {
        return false;
    }
    int size = internal_use_do_not_use::db_get_i64(itr, nullptr, 0);
    vb->size = size;
    vb->data = (char *)malloc(size);
    internal_use_do_not_use::db_get_i64(itr, vb->data, size);
    return true;
}

void multi_index::add_index(enum index_type type) {
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

int multi_index::get_secondary_key_size(int idx) {
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

uint64_t multi_index::get_secondary_idx_table(int idx) {
    return (table & 0xFFFFFFFFFFFFFFF0ULL) | ((uint64_t)idx & 0x000000000000000FULL);
}

secondary_index_db_functions* multi_index::get_secondary_idx(int idx) {
    check(idx<secondary_indexes.size() && idx>=0, "bad secondary idx");
    return secondary_indexes[idx];
}

int multi_index::get_indexes_count() {
    return secondary_indexes.size();
}

int multi_index::get_indexes(int *idxes, uint32_t size) {
    check(size >= secondary_indexes.size(), "indexes size not large enough!");
    memcpy(idxes, indexes, secondary_indexes.size()*sizeof(int));
    return secondary_indexes.size();
}

extern "C" {
    void *mi_new(uint64_t code, uint64_t scope, uint64_t table, int * _indexes, uint32_t size) {
        auto mi = new multi_index(code, scope, table, _indexes, size);
        return mi;
    }
    
    void mi_store(void *ptr, uint64_t primary_key, const void *data, uint32_t data_size, struct vm_buffer *secondary_values, uint32_t secondary_values_size, uint64_t payer) {
        multi_index* mi = (multi_index*)ptr;
        mi->store(primary_key, data, data_size, secondary_values, secondary_values_size, payer);
    }

    void mi_modify(void *ptr, int32_t itr, uint64_t primary_key, const void *data, uint32_t data_size, struct vm_buffer* secondary_values, uint32_t secondary_values_size, uint64_t payer) {
        multi_index* mi = (multi_index*)ptr;
        mi->modify(itr, primary_key, data, data_size, secondary_values, secondary_values_size, payer);
    }

    void mi_erase(void *ptr, int32_t itr, uint64_t primary_key) {
        multi_index* mi = (multi_index*)ptr;
        mi->erase(itr, primary_key);
    }

    int32_t mi_find(void *ptr, uint64_t primary_key) {
        multi_index* mi = (multi_index*)ptr;
        return mi->find(primary_key);
    }

    int mi_get(void *ptr, int32_t itr, struct vm_buffer *vb) {
        multi_index* mi = (multi_index*)ptr;
        return mi->get(itr, vb);
    }

    int32_t mi_next(void *ptr, int32_t itr, uint64_t* primary_key) {
        multi_index* mi = (multi_index*)ptr;
        return mi->next(itr, *primary_key);
    }

    int32_t mi_previous(void *ptr, int32_t itr, uint64_t* primary_key) {
        multi_index* mi = (multi_index*)ptr;
        return mi->previous(itr, *primary_key);
    }

    int32_t mi_end(void *ptr, uint64_t code, uint64_t scope, uint64_t table) {
        multi_index* mi = (multi_index*)ptr;
        return mi->end(code, scope, table);
    }

    int32_t mi_lowerbound(void *ptr, uint64_t code, uint64_t scope, uint64_t table, uint64_t primary_key) {
        multi_index* mi = (multi_index*)ptr;
        return mi->lowerbound(code, scope, table, primary_key);
    }

    int32_t mi_upperbound(void *ptr, uint64_t code, uint64_t scope, uint64_t table, uint64_t primary_key) {
        multi_index* mi = (multi_index*)ptr;
        return mi->upperbound(code, scope, table, primary_key);
    }

    int32_t mi_idx_find(void *ptr, int32_t secondary_index, uint64_t *primary_key, const void *key, uint32_t key_size) {
        multi_index* mi = (multi_index*)ptr;
        return mi->idx_find(secondary_index, *primary_key, key, key_size);
    }

    void mi_idx_update(void *ptr, int32_t secondary_index, int32_t iterator, 
                      const void* secondary_key, uint32_t secondary_key_size, uint64_t payer ) {
        multi_index* mi = (multi_index*)ptr;
        mi->idx_update(secondary_index, iterator, secondary_key, secondary_key_size, payer);
    }

    bool mi_get_by_secondary_key(void *ptr, int32_t secondary_index, 
                                const void *secondary_key, uint32_t secondary_key_size, 
                                vm_buffer *vb) {
        multi_index* mi = (multi_index*)ptr;
        return mi->get_by_secondary_key(secondary_index, secondary_key, secondary_key_size, vb);
    }

    int32_t mi_idx_next(void *ptr, int32_t secondary_index, int32_t itr_secondary, uint64_t *primary_key) {
        multi_index* mi = (multi_index*)ptr;
        return mi->idx_next(secondary_index, itr_secondary, *primary_key);
    }

    int32_t mi_idx_previous(void *ptr, int32_t secondary_index, int32_t itr_secondary, uint64_t *primary_key) {
        multi_index* mi = (multi_index*)ptr;
        return mi->idx_previous(secondary_index, itr_secondary, *primary_key);
    }

    int32_t mi_idx_end(void *ptr, int secondary_index, uint64_t code, uint64_t scope, uint64_t table) {
        multi_index* mi = (multi_index*)ptr;
        return mi->idx_end(secondary_index, code, scope, table);
    }

    int32_t mi_idx_lowerbound(void *ptr, int32_t secondary_index, uint64_t code, uint64_t scope, uint64_t table, void *secondary, uint32_t secondary_size, uint64_t *primary_key) {
        multi_index* mi = (multi_index*)ptr;
        return mi->idx_lowerbound(secondary_index, code, scope, table, secondary, secondary_size, *primary_key);
    }

    int32_t mi_idx_upperbound(void *ptr, int32_t secondary_index, uint64_t code, uint64_t scope, uint64_t table, void *secondary, uint32_t secondary_size, uint64_t *primary_key) {
        multi_index* mi = (multi_index*)ptr;
        return mi->idx_upperbound(secondary_index, code, scope, table, secondary, secondary_size, *primary_key);
    }

    int mi_get_indexes_count(void *ptr) {
        multi_index* mi = (multi_index*)ptr;
        return mi->get_indexes_count();
    }

    int mi_get_indexes(void *ptr, int *idxes, uint32_t size) {
        multi_index* mi = (multi_index*)ptr;
        return mi->get_indexes(idxes, size);
    }

}
