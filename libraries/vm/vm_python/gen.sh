../../..//build/libraries/vm/vm_wasm/wasm_injector ../../../build/contracts/python37/pythonvm/pythonvm.wasm out.wasm
~/dev/wasm/wabt/build/wasm2c -o pythonvm.c out.wasm
mv pythonvm.c pythonvm.c.bin
gc pythonvm.c

