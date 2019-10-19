../../..//build/libraries/vm/vm_wasm/wasm_injector ../../../build/contracts/python37/pythonvm/pythonvm.wasm ../../../build/contracts/python37/pythonvm/pythonvm.wasm2
~/dev/wasm/wabt/build/wasm2c -o pythonvm.c ../../../build/contracts/python37/pythonvm/pythonvm.wasm2
mv pythonvm.c pythonvm.c.bin
git checkout pythonvm.c
python3 patch.py
