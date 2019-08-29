#include <eosio/chain/wasm_eosio_constraints.hpp>

namespace eosio { namespace chain { namespace wasm_constraints {

static unsigned _maximum_table_elements       = maximum_table_elements;
static unsigned _maximum_section_elements     = maximum_section_elements;
static unsigned _maximum_linear_memory_init   = maximum_linear_memory_init;
static unsigned _maximum_func_local_bytes     = maximum_func_local_bytes;

void set_maximum_table_elements(unsigned max) {
   _maximum_table_elements = max;
}

void set_maximum_section_elements(unsigned max) {
   _maximum_section_elements = max;
}

void set_maximum_linear_memory_init(unsigned max) {
   _maximum_linear_memory_init = max;
}

void set_maximum_func_local_bytes(unsigned max) {
   _maximum_func_local_bytes = max;
}

int get_maximum_table_elements() {
   return _maximum_table_elements;
}

int get_maximum_section_elements() {
   return _maximum_section_elements;
}

int get_maximum_linear_memory_init() {
   return _maximum_linear_memory_init;
}

int get_maximum_func_local_bytes() {
   return _maximum_func_local_bytes;
}

void restore_to_default_values() {
    _maximum_table_elements       = maximum_table_elements;
    _maximum_section_elements     = maximum_section_elements;
    _maximum_linear_memory_init   = maximum_linear_memory_init;
    _maximum_func_local_bytes     = maximum_func_local_bytes;
}

} // namespace  wasm_constraints

}
}