#include <string.h>
#include <vm_api_wrap.h>
#include "micropython.h"

void *get_memory_ptr(uint32_t offset, uint32_t size);

u32 _call_vm_api(u32 function_type, u32 args_offset, u32 args_size, u32 output_offset) {
  struct vm_api_arg *vm_args = 0;
  struct vm_api_arg *output = 0;
  struct vm_api_arg vm_args_copy[10];

  if (args_size > 10) {
    return 0;
  }

  if (args_offset) {
    vm_args = (struct vm_api_arg *)get_memory_ptr(args_offset, args_size*sizeof(struct vm_api_arg));
  }

  if (output_offset) {
    output = (struct vm_api_arg *)get_memory_ptr(output_offset, sizeof(struct vm_api_arg));
  }

  memcpy(vm_args_copy, vm_args, args_size * sizeof(struct vm_api_arg));


  for (int i=0;i<args_size;i++) {
    if (vm_args_copy[i].type == enum_arg_type_ptr) {
      // pointer type in vm is 32 bit long, convert pointer offset to pointer
      if (vm_args[i].u32) {
        vm_args_copy[i].ptr = get_memory_ptr(vm_args[i].u32, vm_args[i].size);
      } else {
        vm_args_copy[i].ptr = 0;
      }
    }
  }

  return call_vm_api(function_type, vm_args_copy, args_size, output);
}
