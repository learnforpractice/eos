
#ifndef WASM_RT_MODULE_PREFIX
#define WASM_RT_MODULE_PREFIX
#endif

#define WASM_RT_PASTE_(x, y) x ## y
#define WASM_RT_PASTE(x, y) WASM_RT_PASTE_(x, y)
#define WASM_RT_ADD_PREFIX(x) WASM_RT_PASTE(WASM_RT_MODULE_PREFIX, x)


void init_vm_api4c();
extern void (*WASM_RT_ADD_PREFIX(Z_python_initZ_vv))(void);

int main(int argc, char *argv[]) {
   init_vm_api4c();
}

