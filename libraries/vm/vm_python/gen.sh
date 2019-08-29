/Users/newworld/dev/eos/build/libraries/vm/vm_python/wasm_injector ../../../build/contracts/python37/pythonvm/pythonvm.wasm
~/dev/wasm/wabt/build/wasm2c -o pythonvm.c out.wasm
mv pythonvm.c pythonvm.c.bin
gc pythonvm.c

