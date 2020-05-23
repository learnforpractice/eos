#../../../build/libraries/vm/vm_wasm/wasm_injector ../../../build/contracts/python37/pythonvm/pythonvm.wasm ../../../build/contracts/python37/pythonvm/pythonvm.wasm2
cp ../../../build/contracts/python37/pythonvm/pythonvm.wasm .
~/dev/wasm/wabt/build/wasm2c -o pythonvm.c pythonvm.wasm

mv pythonvm.c pythonvm.c.bin
git checkout pythonvm.c
python3 patch.py
