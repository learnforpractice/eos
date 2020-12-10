#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <vm_api.h>

#include "frozen/frozen.c"

/*
-----------------contract code struct---------------
header: 64 bytes
region size: 12 bytes |name region size|code size region size|code region size|
name_region:
code_size_region:
code_region:
*/

#define CODE_HEADER_SIZE 64
#define REGION_SIZES 12
#define SIZE_LIMIT 10*1024*1024
#define NAME_REGION_SIZE_LIMIT (256*64) //max frozen modules: 64, max file name size: 256 bytes
#define MAX_FORZEN_MODULE 100

typedef enum {
    MP_IMPORT_STAT_NO_EXIST,
    MP_IMPORT_STAT_DIR,
    MP_IMPORT_STAT_FILE,
} mp_import_stat_t;

uint32_t load_uint32(const char *data) {
    uint32_t n;
    memcpy(&n, data, 4);
    return n;
}

int micropython_validate_frozen_code(const char *contract_code, size_t code_size) {
    
    get_vm_api()->eosio_assert(code_size > CODE_HEADER_SIZE + REGION_SIZES, "invalid code size!");
    
    get_vm_api()->eosio_assert(load_uint32(contract_code) == 5, "invalid version");

    size_t name_region_size = load_uint32(contract_code + CODE_HEADER_SIZE);
    size_t code_size_region_size = load_uint32(contract_code + CODE_HEADER_SIZE + 4);
    size_t code_region_size = load_uint32(contract_code + CODE_HEADER_SIZE + 8);

    size_t total_frozen_module = code_size_region_size/sizeof(uint32_t);
    get_vm_api()->eosio_assert(total_frozen_module <= MAX_FORZEN_MODULE, "frozen module count must <= 100");

    get_vm_api()->eosio_assert(name_region_size < NAME_REGION_SIZE_LIMIT, "name region size too large!");
    get_vm_api()->eosio_assert(code_size_region_size < SIZE_LIMIT, "code size region too large!");
    get_vm_api()->eosio_assert(code_region_size < SIZE_LIMIT, "code region size too large!");

    uint32_t total_size = name_region_size + code_size_region_size + code_region_size;
    total_size += CODE_HEADER_SIZE + REGION_SIZES;
    get_vm_api()->eosio_assert(code_size == total_size, "contract_code not valid!");

    const char *name_region = contract_code + CODE_HEADER_SIZE + REGION_SIZES;
    const uint32_t *code_size_region = (uint32_t *)(contract_code + CODE_HEADER_SIZE + REGION_SIZES + name_region_size);
    const char *code_region = contract_code + CODE_HEADER_SIZE + REGION_SIZES + name_region_size + code_size_region_size;
    //validate name region
    
    size_t max_name_size = name_region_size;
    size_t name_count = 0;
    for (int i=0;i<name_region_size;i++) {
        if (name_region[i] == '\0') {
            name_count += 1;
        }
        if (name_count > total_frozen_module) {
            get_vm_api()->eosio_assert(false, "name region size exceed module count!");
        }
    }
    get_vm_api()->eosio_assert(name_count == total_frozen_module, "invalid module name count!");

    //validate code region size
    total_size = 0;
    for (size_t i=0;i<total_frozen_module;i++) {
        size_t module_size = code_size_region[i];
        get_vm_api()->eosio_assert(module_size < SIZE_LIMIT, "module_size too large!");
        total_size += module_size;
    }

    get_vm_api()->eosio_assert(total_size == code_region_size, "invalid code region size!");

    return 1;
}

size_t micropython_load_frozen_code(const char *str, size_t len, char *content, size_t content_size) {
    if (!get_vm_api()->is_in_apply_context) {
        return 0;
    }

    const char *contract_code = NULL;
    size_t code_size = get_vm_api()->get_code(&contract_code);
    if (code_size <= 0) {
        return 0;
    }

    uint32_t name_region_size = load_uint32(contract_code + CODE_HEADER_SIZE);
    uint32_t code_size_region_size = load_uint32(contract_code + CODE_HEADER_SIZE + 4);
    uint32_t code_region_size = load_uint32(contract_code + CODE_HEADER_SIZE + 8);

    get_vm_api()->eosio_assert(name_region_size < SIZE_LIMIT, "name region size too large!");
    get_vm_api()->eosio_assert(code_size_region_size < SIZE_LIMIT, "code size region too large!");
    get_vm_api()->eosio_assert(code_region_size < SIZE_LIMIT, "code region size too large!");

    uint32_t total_size = name_region_size + code_size_region_size + code_region_size;
    total_size += CODE_HEADER_SIZE + REGION_SIZES;
    get_vm_api()->eosio_assert(code_size == total_size, "contract_code not valid!");

    const char *name_region = contract_code + CODE_HEADER_SIZE + REGION_SIZES;
    const uint32_t *code_size_region = (uint32_t *)(contract_code + CODE_HEADER_SIZE + REGION_SIZES + name_region_size);
    const char *code_region = contract_code + CODE_HEADER_SIZE + REGION_SIZES + name_region_size + code_size_region_size;

    uint32_t max_name_size = name_region_size;

    size_t offset = 0;
    const char *name = name_region;
    const char *mpy_code = code_region;

    for (int i=0; *name && offset < name_region_size; i++) {
        size_t name_size = strnlen(name, max_name_size);
        size_t code_size = code_size_region[i];
        if (len == name_size && memcmp(name, str, len) == 0) {
            if (content == NULL || content_size <= 0) {
                return code_size;
            }
            size_t copy_size;
            if (content_size < code_size) {
                copy_size = content_size;
            } else {
                copy_size = code_size;
            }
            memcpy(content, mpy_code, copy_size);
            return copy_size;
        }
        mpy_code += code_size;
        name += name_size + 1;
        offset += name_size + 1;
        max_name_size -= name_size - 1;
    }
    return 0;
}

mp_import_stat_t contract_stat(const char *str, size_t len) {
    if (!get_vm_api()->is_in_apply_context) {
        return MP_IMPORT_STAT_NO_EXIST;
    }

    const char *contract_code = NULL;
    size_t code_size = get_vm_api()->get_code(&contract_code);
    if (code_size <= 0) {
        return MP_IMPORT_STAT_NO_EXIST;
    }

    uint32_t name_region_size = load_uint32(contract_code + CODE_HEADER_SIZE);
    uint32_t code_size_region_size = load_uint32(contract_code + CODE_HEADER_SIZE + 4);
    uint32_t code_region_size = load_uint32(contract_code + CODE_HEADER_SIZE + 8);

    get_vm_api()->eosio_assert(name_region_size < SIZE_LIMIT, "size too large!");
    get_vm_api()->eosio_assert(code_size_region_size < SIZE_LIMIT, "size too large!");
    get_vm_api()->eosio_assert(code_region_size < SIZE_LIMIT, "size too large!");

    uint32_t total_size = name_region_size + code_size_region_size + code_region_size;
    total_size += CODE_HEADER_SIZE + REGION_SIZES;
    get_vm_api()->eosio_assert(code_size == total_size, "contract_code not valid!");

    const char *name_region = contract_code + CODE_HEADER_SIZE + REGION_SIZES;
    const uint32_t *code_size_region = (uint32_t *)(contract_code + CODE_HEADER_SIZE + REGION_SIZES + name_region_size);
    const char *code_region = contract_code + CODE_HEADER_SIZE + REGION_SIZES + name_region_size + code_size_region_size;

    uint32_t max_name_size = name_region_size;

    size_t offset = 0;
    const char *name = name_region;
    const char *mpy_code = code_region;

    for (int i=0; *name && offset < name_region_size; i++) {
        size_t name_size = strnlen(name, max_name_size);
        size_t code_size = code_size_region[i];
        if (len == name_size && memcmp(name, str, len) == 0) {
            return MP_IMPORT_STAT_FILE;
        }
        mpy_code += code_size;
        name += name_size + 1;
        max_name_size -= name_size - 1;
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

static mp_import_stat_t mp_frozen_stat_helper(const char *name, const char *str) {
    size_t len = strlen(str);

    for (int i = 0; *name != 0; i++) {
        size_t l = strlen(name);
        if (l >= len && !memcmp(str, name, len)) {
            if (name[len] == 0) {
                return MP_IMPORT_STAT_FILE;
            } else if (name[len] == '/') {
                return MP_IMPORT_STAT_DIR;
            }
        }
        name += l + 1;
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

uint32_t vm_frozen_stat(const char *str) {
    mp_import_stat_t stat;

    stat = mp_frozen_stat_helper(mp_frozen_str_names, str);
    if (stat != MP_IMPORT_STAT_NO_EXIST) {
        return (uint32_t)stat;
    }

    stat = contract_stat(str, strlen(str));
    if (stat != MP_IMPORT_STAT_NO_EXIST) {
        return (uint32_t)stat;
    }

    return (uint32_t)MP_IMPORT_STAT_NO_EXIST;
}

size_t vm_load_frozen_module(const char *str, size_t len, char *content, size_t content_size) {
    size_t code_size = micropython_load_frozen_code(str, len, content, content_size);

    if (code_size) {
        return code_size;
    }

    const char *name = mp_frozen_str_names;
    size_t offset = 0;
    for (int i = 0; *name != 0; i++) {
        size_t l = strnlen(name, 64);
        if (l == len && !memcmp(str, name, l)) {
            size_t str_size = mp_frozen_str_sizes[i];
            if (content == NULL || content_size == 0) {
                return str_size;
            }
            size_t copy_size = str_size;
            if (copy_size > content_size) {
                copy_size = content_size;
            }
            memcpy(content, mp_frozen_str_content + offset, copy_size);
            return copy_size;
        }
        name += l + 1;
        offset += mp_frozen_str_sizes[i] + 1;
    }
    return 0;
}
