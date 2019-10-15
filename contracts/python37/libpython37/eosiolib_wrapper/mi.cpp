#include "mi.hpp"

using namespace std;
using namespace _multi_index_detail;

enum index_type {
    idx64,
    idx128,
    idx256,
    idx_double,
    idx_long_double,
};

class multi_index {
public:
    multi_index(uint64_t code, uint64_t scope) {
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
  private:
      vector<secondary_index_db_functions*> secondary_indexes;
      uint64_t code;
      uint64_t scope;
};

}