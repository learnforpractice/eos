
#ifndef VM_PYTHON_H_
#define VM_PYTHON_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VM_PAGE_SIZE
    #define VM_PAGE_SIZE 65536U
#endif

typedef void (*fn_vm_load_memory)(uint32_t offset_start, uint32_t length);
typedef void (*fn_python_vm_apply)(uint64_t receiver, uint64_t code, uint64_t action);
typedef void (*fn_python_vm_call)(uint64_t func_name, uint64_t receiver, uint64_t code, uint64_t action);


struct vm_python_info {
    fn_python_vm_apply apply;
    fn_python_vm_call call;

    uint8_t vmtype;
    uint8_t vmversion;

    fn_vm_load_memory load_memory;
    uint8_t *memory_start;
    size_t memory_size;
};

#ifdef __cplusplus
}
#endif

#endif