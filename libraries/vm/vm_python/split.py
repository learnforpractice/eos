with open('pythonvm.c.bin', 'r') as f:
    data = f.read()

head_end = data.find('static u32 func_types[84]')

with open('pythonvm_head.h', 'w') as f:
    f.write(data[:head_end])


data_start = data.find('static const u8 data_segment_data')

file_function = open('pythonvm_function.c.bin', 'w')
file_function.write('#include "pythonvm_head.h"\n')
file_function.write('void init_memory(void);\n')
file_function.write(data[head_end:data_start].replace('static wasm_rt_memory_t M0;', 'extern wasm_rt_memory_t M0;'))

data_end = data.find('static void init_table(void)')

file_function.write(data[data_end:])
file_function.close()

with open('pythonvm_init_memory.c.bin', 'w') as f:
    f.write('#include "pythonvm_head.h"\n')
    f.write('wasm_rt_memory_t M0;')
    f.write(data[data_start:data_end].replace('static void init_memory(void)', 'void init_memory(void)'))

with open('pythonvm_function.c', 'w') as f:
    f.write('#include "pythonvm_function.c.bin"')

with open('pythonvm_init_memory.c', 'w') as f:
    f.write('#include "pythonvm_init_memory.c.bin"')

