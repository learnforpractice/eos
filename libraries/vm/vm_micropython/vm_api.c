#include <vm_api_wrap.h>
#include "micropython.h"

void *get_memory_ptr(int offset);

u32 _call_vm_api(u32 function_type, u32 input_offset, u32 input_size, u32 output_offset) {
  struct vm_api_arg *input = 0;
  struct vm_api_arg *output = 0;

  if (input_offset) {
    input = (struct vm_api_arg *)get_memory_ptr(input_offset);
  }

  if (output_offset) {
    output = (struct vm_api_arg *)get_memory_ptr(output_offset);
  }

  for (int i=0;i<input_size;i++) {
    if (input[i].type == enum_arg_type_ptr) {
      // pointer type in vm is 32 bit long, convert pointer offset to pointer
      input[i].ptr = get_memory_ptr(input[i].u32);
    }
  }

  return call_vm_api(function_type, input, input_size, output);
}
