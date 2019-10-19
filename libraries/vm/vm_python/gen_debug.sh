~/dev/wasm/wabt/build/wasm2c -o pythonvm.c ../../../build/contracts/python37/pythonvm/pythonvm.wasm
mv pythonvm.c pythonvm.c.bin
git checkout pythonvm.c

