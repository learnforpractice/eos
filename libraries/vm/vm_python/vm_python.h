
#ifndef VM_PYTHON_H_
#define VM_PYTHON_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*fn_vm_load_memory)(uint32_t offset_start, uint32_t length);


#ifdef __cplusplus
}
#endif

#endif